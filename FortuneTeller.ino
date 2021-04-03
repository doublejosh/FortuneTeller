#include <LiquidCrystal.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <math.h>
#include <string>
#include "utilities.h"
#include "animation.h"
#include "messages.h"

uint8_t __trigger, __sleepFrame = 0;
uint32_t __timer = 1;
bool __offline = false, __randomChoice = false;
String __interactionId = "";

LiquidCrystal __lcd(
	displayRS, displayEN, displayD4,
	displayD5, displayD6, displayD7
);

FirebaseData __fbData;
DynamicJsonDocument __jsonDoc(JSON_DOC_BYTES);

typedef struct {
	unsigned int id;
	String text;
} Fortune;

typedef struct {
	uint8_t id;
	String name;
	String nextYes;
	String nextNo;
	String text;
	unsigned int version;
} Question;
Question _questionList[QUESTION_LIST_LENGTH];

// SCREEN...

/**
 * Show animation frame to the screen.
 */
void paint (char screen[HEIGHT][WIDTH+1], int wait) {
	__lcd.noDisplay();
	__lcd.clear();
	for (uint8_t i = 0; i < sizeof(screen); i++) {
		__lcd.setCursor(0, i);
		__lcd.print(String(screen[i]));
	}
	__lcd.display();
	delay(wait);
}

/**
 * Loop through and show animation frames.
 */
void play (char frames[][HEIGHT][WIDTH+1], uint8_t count) {
	for (uint8_t f = 0; f < count; f++) {
		paint(frames[f], DELAY_ANIMATION);
	}
}

/**
 * Show text message on the screen.
 */
void txtToScreen (String msg, int wait, int row, bool ignoreChromeCheck = false) {
	printDebug(msg);
	if (CHROME || ignoreChromeCheck) {
		__lcd.clear();
		__lcd.setCursor(WIDTH-1, HEIGHT);
		__lcd.print("~");
		__lcd.setCursor(0, row);
		__lcd.print(msg);
		delay(wait);
	}
}

/**
 * Handle wifi connection.
 */
bool connect (void) {
	String msg;
	uint8_t wifiTry = 0;

	for (uint8_t i = 0; i < sizeof(WIFI_SSID)/sizeof(WIFI_SSID[0]); i++) {
		txtToScreen("Connecting: " + WIFI_SSID[i].substring(0, 8), DELAY_MSG, 1);

		WiFi.begin(WIFI_SSID[i], WIFI_PASSWORD[i]);
		while (WiFi.status() != WL_CONNECTED) {
			delay(WIFI_RETRY_DELAY);
			if (wifiTry == 4) {
				WiFi.disconnect();
				wifiTry = 0;
				break;
			}
			else {
				wifiTry++;
				printDebug(".");
			}
		}
		if (WiFi.status() == WL_CONNECTED) {
			msg = "Done: " + WiFi.localIP().toString();
			txtToScreen(msg, DELAY_QUICK_MSG, 1);
			break;
		}
		else {
			txtToScreen("Unable to connect.", DELAY_MSG, 1);
		}
	}

  if (WiFi.status() != WL_CONNECTED) {
		printDebug("Wifi Error, please restart.");
    paint(MESSAGES[2], DELAY_MSG);
		__offline = true;
		return false;
  } else {
		// Setup Firebase connection and fetch handler.
		Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
		Firebase.reconnectWiFi(true);
		Firebase.setReadTimeout(__fbData, READ_TIMEOUT);
		Firebase.setMaxRetry(__fbData, FIREBASE_MAX_TRY);
		__fbData.setBSSLBufferSize(FIREBASE_BUFFER_SIZE_RECEIVE, FIREBASE_BUFFER_SIZE_SEND);
		__fbData.setResponseSize(FIREBASE_RESPONSE_SIZE);
		return true;
	}
}


// APPLICATION...

/**
 * Retrieve string field within JSON data.
 */
String getString (String key) {
	if (Firebase.getShallowData(__fbData, key)) {
		if (__fbData.dataType() == "string") {
			String text = __fbData.stringData();
			return text;
		}
	} else printDebug(__fbData.errorReason());
}

/**
 * Retrieve int field within JSON data.
 */
unsigned int getInt (String key) {
	if (Firebase.getShallowData(__fbData, key)) {
		if (__fbData.dataType() == "int") {
			int num = __fbData.intData();
			return num;
		}
	} else printDebug(__fbData.errorReason());
}

/**
 * Get list of questions online and stash for later.
 */
void fetchQuestions () {
	txtToScreen("Fetching questions.", DELAY_QUICK_MSG, 1);
	if (Firebase.getShallowData(__fbData, QUESTION_PATH)) {
		FirebaseJson json;
		size_t lineCount = json.iteratorBegin(__fbData.jsonString().c_str());
		String _key, _val, prefix;
		int _type = 0;
		for (size_t i = 0; i < lineCount; i++) {
			// Assemble object from return data.
			json.iteratorGet(i, _type, _key, _val);
			prefix = QUESTION_PATH + "/" + _key + "/";
			if (_val == "true") {
				Question question = { i, _key,
					getString(prefix + FIELD_NEXT_YES),
					getString(prefix + FIELD_NEXT_NO),
					getString(prefix + FIELD_TEXT),
					getInt(prefix + FIELD_VERSION)
				};
				_questionList[i] = question;
			}
			delay(0);
		}
		json.iteratorEnd();
	}
	else printDebug(__fbData.errorReason());
	__fbData.clear();
}

/**
 * Boost a metric of a fortune.
 */
void increaseMetric(String fortuneId, String field) {
	int metric;
	String path = FORTUNES_PATH + "/" + fortuneId + "/" + field;
	if (Firebase.getInt(__fbData, path)) {
		metric = __fbData.intData();
		printDebug("Count " + field + ": " + metric);
		// Update value.
		if (SAVING) {
			if (Firebase.setInt(__fbData, path, metric+1)) {
				printDebug("Updated " + field + ".");
			} else printDebug(__fbData.errorReason());
		} else printDebug("Skipped increase for testing mode.");
	} else printDebug(__fbData.errorReason());
	__fbData.clear();
}

/**
 * Wait and collect a yes/no answer from user.
 */
#define ANSWER_YES 1
#define ANSWER_NO 0
#define ANSWER_SKIP -1
int8_t ask (uint16_t timeout) {
	int seconds;
	unsigned long startTime = millis(), currentTime = millis();
	// Time boundary on question.
	while (currentTime - startTime <= timeout) {
		currentTime = millis();
		uint8_t seconds = (currentTime - startTime) / 1000;
		__lcd.setCursor(18, 3);
		__lcd.print("  ");
		__lcd.setCursor(18, 3);
		__lcd.print(String((timeout / 1000) - seconds));
		// Accept the answer.
		bool voteYes = (digitalRead(BTN2_PULLUP) == LOW);
		if (voteYes || (digitalRead(BTN1_PULLUP) == LOW)) {
			return (voteYes) ? ANSWER_YES : ANSWER_NO;
		}
		// @todo Use interrupts button listening.
		// https://github.com/doublejosh/FortuneTeller/issues/1
		// Keep the ESP watchdog happy (avoid soft reset).
		delay(200);
	}
	return ANSWER_SKIP;
}

/**
 * Organize fortunes into indexed list, to pick randomly.
 */
JsonObject buildFortuneIndex(String jsonStr, String index[]) {
	printDebug("fortunes indexed.");
	// Parse JSON from string.
	DeserializationError e = deserializeJson(__jsonDoc, jsonStr);
	if (e.code() == DeserializationError::NoMemory) {
		printDebug("Not enough JSON memory.");
	}
	// Create index of ids.
	JsonObject listObj = __jsonDoc.as<JsonObject>();
	uint8_t id = 0;
	for (JsonObject::iterator i=listObj.begin(); i!=listObj.end(); ++i) {
		index[id] = String(i->key().c_str());
		printDebug(String(id) + ": " + String(i->key().c_str()));
		id++;
		// Keep the ESP watchdog happy (avoid soft reset).
		delay(0);
	}

	return listObj;
}

/**
 * Create inital interaction event to Firebase, for crash awareness.
 */
void saveInteractionInit () {
	printDebug("SAVING...");
	FirebaseJson fbJson;
	fbJson.set(FIELD_MACHINE, MACHINE_ID);
	// Send interaction data.
	if (SAVING) {
		if (Firebase.pushJSON(__fbData, INTERACTIONS_PATH, fbJson)) {
			__interactionId = __fbData.pushName();
			// Add a timestamp.
			Firebase.setTimestamp(__fbData, INTERACTIONS_PATH + "/" + __interactionId + "/" + FIELD_CREATED);
			printDebug(F("Created interaction record."));
		} else printDebug(F("SAVE ERROR"));
	} else printDebug(F("Save skipped for testing."));
}

/**
 * Save mid-point interaction data to Firebase, for crash awareness.
 */
void saveInteractionMiddle (const String category, double sensor, int version) {
	printDebug("SAVING...");
	FirebaseJson fbJson;
	fbJson.set(FIELD_CATEGORY, category);
	fbJson.set(FIELD_SENSOR, sensor);
	fbJson.set(FIELD_VERSION, version);
	fbJson.set(FIELD_RANDOM, __randomChoice);
	// Send interaction data.
	if (SAVING) {
		if (Firebase.updateNode(__fbData, INTERACTIONS_PATH + "/" + __interactionId, fbJson)) {
			printDebug("Interaction data saved.");
		} else printDebug("SAVE ERROR");
	} else {
		printDebug("Save skipped for testing.");
	}
}

/**
 * Save final interaction data to Firebase.
 */
void saveInteractionEnd (int fortune, int accurate) {
	printDebug("SAVING...");
	FirebaseJson fbJson;
	fbJson.set(FIELD_FORTUNE_ID, fortune);
	fbJson.set(FIELD_ACCURATE, accurate);
	// Send interaction data.
	if (CHROME) paint(MESSAGES[SAVE], DELAY_MSG);
	if (SAVING) {
		if (Firebase.updateNode(__fbData, INTERACTIONS_PATH + "/" + __interactionId, fbJson)) {
			printDebug("Ready.");
			if (CHROME) paint(MESSAGES[FUTURE], DELAY_MSG);
		} else printDebug("SAVE ERROR");
	} else {
		printDebug("Save skipped for testing.");
	}
}

/**
 * Get fortune online based on chosen category.
 */
void fetchFortune (const String category, uint16_t timeout) {
	if (CHROME) play(APPEAR_FRAMES, 6);
	paint(MESSAGES[FETCHING], DELAY_MSG);
	printDebug("Fortune category: " + category);

	// Fetch relevant fortunes.
	QueryFilter query;
	query.orderBy(FIELD_CATEGORY);
	query.equalTo(category);
	query.limitToFirst(FORTUNE_GET_MAX);
	String index[FORTUNE_INDEX_MAX];
	// @todo Catch or handle fortune network fails.
	// https://github.com/doublejosh/FortuneTeller/issues/3
	if (Firebase.getJSON(__fbData, FORTUNES_PATH, query)) {

		// Parse fortune list results.
		printDebug("Fortunes fetched.");
		FirebaseJson &json = __fbData.jsonObject();
		String jsonStr;
		json.toString(jsonStr, false); // Param two is prettify.
		printDebug(jsonStr);
		JsonObject listObj = buildFortuneIndex(jsonStr, index);

		// Pick from the list.
		String fortuneId = index[random(0, listObj.size())];
		const char* fortune = listObj[fortuneId][FIELD_TEXT];
		printDebug(fortune);
		wrapTxtToScreen(__lcd, fortune);
		delay(DELAY_FORTUNE);
		// Analytics for views of each fortune.
		increaseMetric(fortuneId, FIELD_SHOWN);

		// Ask for accuracy.
		paint(MESSAGES[ACCURATE], DELAY_NONE);
		int result = ask(timeout);
		if (result == ANSWER_YES) {
			paint(MESSAGES[CORRECT], DELAY_MSG);
			increaseMetric(fortuneId, FIELD_VOTES);
		} else if (result == ANSWER_NO) {
			paint(MESSAGES[WRONG], DELAY_MSG);
		} else {
			paint(MESSAGES[TIMEOUT], DELAY_MSG);
		}
		// Record full interaction data.
		saveInteractionEnd(atoi(fortuneId.c_str()), result);

	} else printDebug(__fbData.errorReason());
	__fbData.clear();
}

/**
 * Get the question from stash.
 * Handle end of question tree.
 * Display and handle answer.
 */
void askQuestion (String id, unsigned int version) {
	// Grab data from cached global and show.
	Question question;
	bool questionFound = false;
	for (uint8_t i = 0; i < sizeof(_questionList) / sizeof(_questionList[0]); i++) {
		question = _questionList[i];
		if (question.name == id) {
			if (CHROME) play(APPEAR_FRAMES, 6);
			txtToScreen(question.text, DELAY_NONE, 0, true);
			questionFound = true;
			break;
		}
		delay(0);
	}
	// End of the question tree.
	if (!questionFound) {
		// @todo Get sensor reading.
		double sensor = 123.456;
		saveInteractionMiddle(id, sensor, version);
		fetchFortune(id, QUESTION_TIMEOUT);
		// @todo Handle no fortune.
		return;
	}
	int result = ask(QUESTION_TIMEOUT);

	// Go to the next step.
	String next;
	if (result == ANSWER_YES) {
		paint(MESSAGES[PICK_YES], DELAY_QUICK_MSG);
		next = question.nextYes;
	} else if (result == ANSWER_NO) {
		paint(MESSAGES[PICK_NO], DELAY_QUICK_MSG);
		next = question.nextNo;
	} else {
		paint(MESSAGES[RANDOM], DELAY_MSG);
		// Mark interactions with random selection.
		__randomChoice = true;
		next = (random(9) % 2) ? question.nextYes : question.nextNo;
	}
	askQuestion(next, question.version);
}

/**
 * Kickoff new interaction.
 */
void coin () {
	// Greeting routine.
	printDebug("Coin! **********");
	if (CHROME) {
		play(WAKE_FRAMES, 19);
		paint(MESSAGES[GREET1], DELAY_MSG);
		play(APPEAR_FRAMES, 6);
		paint(MESSAGES[GREET2], DELAY_QUICK_MSG);
		// Button instructions.
		// play(APPEAR_FRAMES, 6);
		// paint(MESSAGES[GREET3], DELAY_QUICK_MSG);
	}
	// Start question tree.
	__interactionId = "";
	__randomChoice = false;
	String next = FIRST_QUESTION_ID;
	saveInteractionInit();
	askQuestion(next, 0);
}

/**
 * In case of shut down due to: watchdog reset, crash, network failure, etc.
 */
void rebootFortune () {
	int len = sizeof(REBOOT_FORTUNES) / sizeof(REBOOT_FORTUNES[0]);
	int pick = random(0, len);
	__lcd.clear();
	wrapTxtToScreen(__lcd, REBOOT_FORTUNES[pick]);
	delay(DELAY_FORTUNE);
	txtToScreen(F("    Reconnecting"), DELAY_MSG, 1);
}

/**
 * Listen for coins and nudge animation.
 */
void sleep () {
	__timer++;
	if (__timer == DELAY_SLEEP) {
		paint(SLEEP_FRAMES[__sleepFrame], DELAY_NONE);
		__sleepFrame = (__sleepFrame < 3) ? __sleepFrame + 1 : 0;
		__timer = 0;
	}
	delay(10);
}

void saveFirebaseData () {
  if (!Firebase.backup(__fbData, StorageType::SD, "/data", "/{BACKUP_FILE}") {
    printDebug("Backup failed: " + __fbData.fileTransferError());
  } else {
    printDebug("Backup success!");
    printDebug("SIZE: " + String(__fbData.getBackupFileSize()));
  }
}

void restoreFirebaseData () {
  if (!Firebase.restore(__fbData, StorageType::SD, "/data", "/{BACKUP_FILE}") {
		printDebug("Restore backup failed: " + __fbData.fileTransferError());
  } else {
		printDebug("Restore backup success!");
		printDebug("SIZE: " + String(__fbData.getBackupFileSize()));
  }
}

void attemptDataRefresh() {
	while (sizeof(_questionList) > 0) {
		txtToScreen("Fetching questions.", DELAY_QUICK_MSG, 1);
		fetchQuestions();
		// if data was reteieved, save it
		// else, keep current backup data
	}

	// fetch all fortunes
	printDebug("Fetching fortune network.");
	printDebug("Ready.");

	if (Firebase.getJSON(__fbData, FORTUNES_PATH, query)) {
		saveFirebaseData();
	} else {
		printDebug("Using backup");
	}
}

/**
 * Quickly try a series of critical actions.
 */
void runTests () {
	if (!TESTING) return;
	String tests[] = {"heart", "self", "wave", "health", "fortune"};
	printDebug(F("Running fortune tests..."));
	for (uint8_t t = 0; t < sizeof(tests)/sizeof(tests[0]); t++) {
		saveInteractionInit();
		delay(FAST_TIMEOUT);
		saveInteractionMiddle("heart", 123.456, 0);
		delay(FAST_TIMEOUT);
		fetchFortune(tests[t], FAST_TIMEOUT);
		printDebug(F(" "));
		printDebug(F("- - - - - - - - - - - - - - - - - - - - - - -"));
	}
}


// STANDARD...

void setup (void) {
	if (DEBUG) Serial.begin(SERIAL_RATE);
	__lcd.begin(WIDTH, HEIGHT);
	pinMode(TRIGGER_PIN, INPUT);
	pinMode(BTN1_PULLUP, INPUT_PULLUP);
	pinMode(BTN2_PULLUP, INPUT_PULLUP);
	int analogVal = analogRead(ANALOG_PIN);
	randomSeed(analogRead(analogVal));

	// On-the-fly alternate modes.
	if (digitalRead(BTN1_PULLUP) == LOW) {
		CHROME = false;
	}
	if (digitalRead(BTN2_PULLUP) == LOW) {
		FREEPLAY = true;
	}

	// Improve UX on reboot.
	if (CHROME) {
		// Delay required for random seed.
		delay(1000);
		rebootFortune();
	}

	// Welcome messages.
	if (CHROME) paint(MESSAGES[BOOT], DELAY_QUICK_MSG);
	else {
		__lcd.clear();
		__lcd.setCursor(0, 1);
		__lcd.print("      8(*__*)8");
	}

	// Fetch and stash question data.
	if (connect()) {
		runTests();
		restoreFbData();
		attemptDataRefresh();
		paint(SLEEP_FRAMES[0], DELAY_NONE);
	} else {
		__offline = true;
		txtToScreen("No internet connection.", DELAY_QUICK_MSG, 1);
		printDebug("No internet connection.");
	}
}

void loop (void) {
	// @todo Use interrupts for coin trigger listening.
	// https://github.com/doublejosh/FortuneTeller/issues/1
	__trigger = digitalRead(TRIGGER_PIN);
	if (__trigger == HIGH) {
		coin();
	} else if (FREEPLAY && digitalRead(BTN2_PULLUP) == LOW) {
		paint(MESSAGES[FREEBIE], DELAY_MSG);
		coin();
	} else sleep();
}
