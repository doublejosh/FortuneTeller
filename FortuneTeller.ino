#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <math.h>
#include <FirebaseESP8266.h>
#include "utilities.h"
#include "animation.h"
#include "messages.h"
#include "ESPSafeMaster.h"

uint8_t __trigger, __sleepFrame = 0;
uint32_t __timer = 1;
bool __offline = false, __randomChoice = false;
String __interactionId = "";

LiquidCrystal_I2C __lcd(0x27, 20, 4);
ESPSafeMaster esp(SS);

FirebaseData __fbData;
DynamicJsonDocument __jsonDoc(JSON_DOC_BYTES);
FirebaseJson __fbJson;
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
void txtToScreen (String msg, int wait, int row) {
	__lcd.clear();
	__lcd.setCursor(0, row);
	__lcd.print(msg);
	delay(wait);
}

/**
 * Pass data to audio player board (ruxpin).
 */
void sendSpi (const char * message) {
	printDebug("Commander: ");
	printDebug(message);
	esp.writeData(message);
	delay(10);
	printDebug("Audio Player: ");
	printDebug(esp.readData());
}

/**
 * Handle wifi connection.
 */
bool connect (void) {
	String msg;
	uint8_t wifiTry = 0;

	for (uint8_t i = 0; i < sizeof(WIFI_SSID)/sizeof(WIFI_SSID[0]); i++) {
		printDebug("Connecting: " + WIFI_SSID[i]);
		if (CHROME) txtToScreen("Connecting: " + WIFI_SSID[i].substring(0, 8), DELAY_MSG, 1);

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
			if (CHROME) txtToScreen(msg, DELAY_QUICK_MSG, 1);
			printDebug(msg);
			break;
		}
		else {
			msg = "Unable to connect.";
			printDebug(msg);
			txtToScreen(msg, DELAY_MSG, 1);
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
	if (CHROME) txtToScreen("Fetching questions.", DELAY_QUICK_MSG, 1);
	printDebug("Fetching questions.");
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
	} else {
		txtToScreen("Fetching failed, using backup", DELAY_MSG, 1);
		// File file = SD.open(LOCAL_DATA_PATH);
		// DeserializationError e = deserializeJson(__jsonDoc, file);
		// if (e.code() == DeserializationError::NoMemory) {
		// 	printDebug("Not enough JSON memory.");
		// }
		// file.close();

		//printDebug(__jsonDoc);

		//_questionList[]

		printDebug(__fbData.errorReason());
	}
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
	__fbJson.set(FIELD_MACHINE, MACHINE_ID);
	// Send interaction data.
	if (SAVING) {
		if (Firebase.pushJSON(__fbData, INTERACTIONS_PATH, __fbJson)) {
			__interactionId = __fbData.pushName();
			// Add a timestamp.
			Firebase.setTimestamp(__fbData, INTERACTIONS_PATH + "/" + __interactionId + "/" + FIELD_CREATED);
			printDebug(F("Created interaction record."));
		} else {
			txtToScreen("Save failed", DELAY_MSG, 1);
			printDebug(__fbData.errorReason());
			printDebug(__fbData.stringData());

		}
	} else printDebug(F("Save skipped for testing."));
	__fbJson.clear();
}

/**
 * Save mid-point interaction data to Firebase, for crash awareness.
 */
// void saveInteractionMiddle (const String category, double sensor, int version) {
// 	__fbJson.clear();
// 	__fbJson.set(FIELD_CATEGORY, category);
// 	__fbJson.set(FIELD_SENSOR, sensor);
// 	__fbJson.set(FIELD_VERSION, version);
// 	__fbJson.set(FIELD_RANDOM, __randomChoice);
// 	if (DEBUG) {
// 		String buffer;
// 		__fbJson.toString(buffer, true);
// 		printDebug(buffer);
// 		delay(0);
// 	}

// 	// Send interaction data.
// 	if (SAVING) {
// 		printDebug("SAVING...");
// 		if (Firebase.updateNode(__fbData, INTERACTIONS_PATH + "/" + __interactionId, __fbJson)) {
// 			printDebug("Interaction data saved.");
// 		} else {
// 			txtToScreen("Save failed", DELAY_MSG, 1);
// 			printDebug(__fbData.errorReason());
// 			printDebug(__fbData.stringData());
// 		}
// 	} else {
// 		printDebug("Save skipped for testing.");
// 	}
// 	__fbJson.clear();
// }

/**
 * Save final interaction data to Firebase.
 *
 * @todo Test if int fortuneIDs is the problem.
 */
// void saveInteractionEnd (String fortuneId, const String category, int accurate, unsigned int version) {
void saveInteractionEnd (String fortuneId, int accurate, const String category, int version) {
	__fbJson.set(FIELD_FORTUNE_ID, fortuneId);
	__fbJson.set(FIELD_ACCURATE, accurate);
	__fbJson.set(FIELD_CATEGORY, category);
	__fbJson.set(FIELD_VERSION, version);
	__fbJson.set(FIELD_RANDOM, __randomChoice);
	// @todo Get sensor reading.
	// double sensor = 123.456;
	//__fbJson.set(FIELD_SENSOR, sensor);
	if (DEBUG) {
		String buffer;
		__fbJson.toString(buffer, true);
		printDebug(buffer);
		delay(0);
	}

	// Send interaction data.
	if (CHROME) paint(MESSAGES[SAVE], DELAY_MSG);
	if (SAVING) {
		printDebug("SAVING...");
		if (Firebase.updateNode(__fbData, INTERACTIONS_PATH + "/" + __interactionId, __fbJson)) {
			printDebug("Ready.");
			if (CHROME) paint(MESSAGES[FUTURE], DELAY_MSG);
			sendSpi("/audio-phrases-1.mp3");
		} else {
			txtToScreen("Save failed", DELAY_MSG, 1);
			printDebug(__fbData.errorReason());
			printDebug(__fbData.stringData());
		}
	} else {
		printDebug("Save skipped for testing.");
	}
	__fbJson.clear();
}

/**
 * Get fortune online based on chosen category.
 */
void fetchFortune (const String category, uint16_t timeout, int version) {
	if (CHROME) play(APPEAR_FRAMES, 6);
	sendSpi("/audio-phrases-6.mp3");
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
		
		// speak the fortune
		char* filename = "";
		strcpy(filename, "/audio-fortunes-");
		strcat(filename, fortuneId.c_str());
		strcat(filename, ".mp3");
		sendSpi(filename);

		delay(DELAY_FORTUNE);
		// Analytics for views of each fortune.
		increaseMetric(fortuneId, FIELD_SHOWN);

		// Ask for accuracy.
		sendSpi("/audio-phrases-3.mp3");
		paint(MESSAGES[ACCURATE], DELAY_NONE);
		int result = ask(timeout);
		if (result == ANSWER_YES) {
			sendSpi("/audio-phrases-4.mp3");
			paint(MESSAGES[CORRECT], DELAY_MSG);
			increaseMetric(fortuneId, FIELD_VOTES);
		} else if (result == ANSWER_NO) {
			sendSpi("/audio-phrases-5.mp3");
			paint(MESSAGES[WRONG], DELAY_MSG);
		} else {
			sendSpi("/audio-phrases-10.mp3");
			paint(MESSAGES[TIMEOUT], DELAY_MSG);
		}
		// Record full interaction data.
		saveInteractionEnd(fortuneId, result, category, version);
	} else {
		txtToScreen("Fetching failed", DELAY_MSG, 1);
		printDebug(__fbData.errorReason());
	}
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
	char* filename = "";
	strcpy(filename, "/audio-questions-");
	strcat(filename, id.c_str());
	strcat(filename, ".mp3");

	for (uint8_t i = 0; i < sizeof(_questionList) / sizeof(_questionList[0]); i++) {
		question = _questionList[i];
		if (question.name == id) {
			printDebug(question.text);
			if (CHROME) play(APPEAR_FRAMES, 6);
			sendSpi(filename);
			txtToScreen(question.text, DELAY_NONE, 0);
			questionFound = true;
			break;
		}
		delay(0);
	}
	// End of the question tree.
	if (!questionFound) {
		// saveInteractionMiddle(id, sensor, version);
		fetchFortune(id, QUESTION_TIMEOUT, version);
		// @todo Handle no fortune.
		return;
	}
	int result = ask(QUESTION_TIMEOUT);

	// Go to the next step.
	String next;
	if (result == ANSWER_YES) {
		sendSpi("/audio-phrases-8.mp3");
		paint(MESSAGES[PICK_YES], DELAY_QUICK_MSG);
		next = question.nextYes;
	} else if (result == ANSWER_NO) {
		sendSpi("/audio-phrases-9.mp3");
		paint(MESSAGES[PICK_NO], DELAY_QUICK_MSG);
		next = question.nextNo;
	} else {
		sendSpi("/audio-phrases-2.mp3");
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
		sendSpi("/audio-phrases-0.mp3");
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

/**
 * Quickly try a series of critical actions.
 */
void runTests () {
	if (!TESTING) return;
	String tests[] = {"heart", "self", "wave", "health", "fortune"};
	printDebug(F("Running fortune tests..."));
	for (uint8_t t = 0; t < sizeof(tests)/sizeof(tests[0]); t++) {
		saveInteractionInit();
		// delay(FAST_TIMEOUT);
		// saveInteractionMiddle("heart", 123.456, 0);
		delay(FAST_TIMEOUT);
		fetchFortune(tests[t], FAST_TIMEOUT, 0);
		printDebug(F(" "));
		printDebug(F("- - - - - - - - - - - - - - - - - - - - - - -"));
	}
}


// STANDARD...

void setup (void) {
	if (DEBUG) Serial.begin(SERIAL_RATE);
	Wire.begin();
	__lcd.init();
	__lcd.begin(WIDTH, HEIGHT);
	__lcd.backlight();

	SPI.begin();
  	esp.begin();

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
		fetchQuestions();
		printDebug("Ready.");
		paint(SLEEP_FRAMES[0], DELAY_NONE);
	}
}

void loop (void) {
	// @todo Use interrupts for coin trigger listening.
	// https://github.com/doublejosh/FortuneTeller/issues/1
	__trigger = digitalRead(TRIGGER_PIN);
	if (__trigger == HIGH) {
		coin();
	} else if (FREEPLAY && digitalRead(BTN2_PULLUP) == LOW) {
		sendSpi("/audio-phrases-7.mp3");
		paint(MESSAGES[FREEBIE], DELAY_MSG);
		coin();
	} else sleep();
}
