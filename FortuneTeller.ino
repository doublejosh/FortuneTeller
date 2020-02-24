#include <LiquidCrystal.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <math.h>
#include "utilities.h"
#include "animation.h"
#include "messages.h"

uint8_t trigger, wifiTry = 0, sleepFrame = 0;
uint32_t timer = 1;
boolean offline = false;
bool randomChoice = false;

LiquidCrystal lcd(
	displayRS, displayEN, displayD4,
	displayD5, displayD6, displayD7
);

FirebaseData fbData;
DynamicJsonDocument jsonDoc(JSON_DOC_BYTES);

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
Question Q_LIST[QUESTION_LIST_LENGTH];

// SCREEN...

/**
 * Show animation frame to the screen.
 */
void paint (char screen[HEIGHT][WIDTH+1], int wait) {
	lcd.noDisplay();
	lcd.clear();
	for (uint8_t i = 0; i < sizeof(screen); i++) {
		lcd.setCursor(0, i);
		lcd.print(String(screen[i]));
	}
	lcd.display();
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
	lcd.clear();
	lcd.setCursor(0, row);
	lcd.print(msg);
	delay(wait);

	// @todo Pretty fortune rendering.
}

/**
 * Handle wifi connection.
 */
bool connect (void) {
	String msg;

	for (uint8_t i = 0; i < sizeof(WIFI_SSID)/sizeof(WIFI_SSID[0]); i++) {
		printDebug("Connecting: " + WIFI_SSID[i]);
		if (!NOCHROME) txtToScreen("Connecting: " + WIFI_SSID[i].substring(0, 8), DELAY_MSG, 1);

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
			if (!NOCHROME) txtToScreen(msg, DELAY_QUICK_MSG, 1);
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
		offline = true;
		return false;
  	} else {
		Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
		Firebase.reconnectWiFi(true);
		Firebase.setReadTimeout(fbData, READ_TIMEOUT);
		Firebase.setMaxRetry(fbData, FIREBASE_MAX_TRY);
		return true;
	}
}


// APPLICATION...

/**
 * Retrieve string field within JSON data.
 */
String getString (String key) {
	if (Firebase.getShallowData(fbData, key)) {
		if (fbData.dataType() == "string") {
			String text = fbData.stringData();
			return text;
		}
	} else printDebug(fbData.errorReason());
}

/**
 * Retrieve int field within JSON data.
 */
unsigned int getInt (String key) {
	if (Firebase.getShallowData(fbData, key)) {
		if (fbData.dataType() == "int") {
			int num = fbData.intData();
			return num;
		}
	} else printDebug(fbData.errorReason());
}

/**
 * Get list of questions online and stash for later.
 */
void fetchQuestions () {
	if (!NOCHROME) txtToScreen("Fetching questions.", DELAY_QUICK_MSG, 1);
	printDebug("Fetching questions.");
	if (Firebase.getShallowData(fbData, QUESTION_PATH)) {
		FirebaseJson json;
		size_t lineCount = json.iteratorBegin(fbData.jsonString().c_str());
		String _key, _val, prefix;
		int _type = 0;
		for (size_t i = 0; i < lineCount; i++) {
			json.iteratorGet(i, _type, _key, _val);
			prefix = QUESTION_PATH + "/" + _key + "/";
			if (_val == "true") {
				Question question = { i, _key,
					getString(prefix + FIELD_NEXT_YES),
					getString(prefix + FIELD_NEXT_NO),
					getString(prefix + FIELD_TEXT),
					getInt(prefix + FIELD_VERSION)
				};
				Q_LIST[i] = question;
			}
			delay(0);
		}
		json.iteratorEnd();
	}
	else printDebug(fbData.errorReason());
	fbData.clear();
}

/**
 * Boost a metric of a fortune.
 */
void increaseMetric(String fortuneId, String field) {
	int metric;
	String path = FORTUNES_PATH + "/" + fortuneId + "/" + field;
	if (Firebase.getInt(fbData, path)) {
		metric = fbData.intData();
		printDebug("Count " + field + ": " + metric);
		// Update value.
		if (Firebase.setInt(fbData, path, metric+1)) {
			printDebug("Updated " + field + ".");
		} else printDebug(fbData.errorReason());
	} else printDebug(fbData.errorReason());
	fbData.clear();
}

/**
 * Wait and collect a yes no answer from user.
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
		lcd.setCursor(18, 3);
		lcd.print("  ");
		lcd.setCursor(18, 3);
		lcd.print(String((timeout / 1000) - seconds));
		// Accept the answer.
		bool voteYes = (digitalRead(BTN2_PULLUP) == LOW);
		if (voteYes || (digitalRead(BTN1_PULLUP) == LOW)) {
			return (voteYes) ? ANSWER_YES : ANSWER_NO;
		}
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
	DeserializationError e = deserializeJson(jsonDoc, jsonStr);
	if (e.code() == DeserializationError::NoMemory) {
		printDebug("Not enough JSON memory.");
	}
	// Create index of ids.
	JsonObject listObj = jsonDoc.as<JsonObject>();
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
 * Save interaction online.
 */
void saveInteraction (int fortune, String category, int accurate, double sensor, int version, bool random) {
	printDebug("SAVING...");
	FirebaseJson fbJson;
	fbJson.set(FIELD_FORTUNE_ID, fortune);
	fbJson.set(FIELD_CATEGORY, category);
	fbJson.set(FIELD_ACCURATE, accurate);
	fbJson.set(FIELD_SENSOR, sensor);
	fbJson.set(FIELD_VERSION, version);
	fbJson.set(FIELD_RANDOM, random);

	// @todo Save the timestamp.

	if (!NOCHROME) paint(MESSAGES[SAVE], DELAY_MSG);
	if (Firebase.pushJSON(fbData, INTERACTIONS_PATH, fbJson)) {
		printDebug("Ready.");
		if (!NOCHROME) paint(MESSAGES[FUTURE], DELAY_MSG);
	} else printDebug("SAVE ERROR");
	fbJson.clear();
}

/**
 * Get fortune online based on chosen category.
 */
void fetchFortune (const String category, double sensor, unsigned int version, uint16_t timeout) {
	if (!NOCHROME) play(APPEAR_FRAMES, 6);
	paint(MESSAGES[FETCHING], DELAY_MSG);
	printDebug("Fortune category: " + category);
	// Fetch relevant fortunes.
	QueryFilter query;
	query.orderBy(FIELD_CATEGORY);
	query.equalTo(category);
	query.limitToFirst(FORTUNE_GET_MAX);
	String index[FORTUNE_INDEX_MAX];
	if (Firebase.getJSON(fbData, FORTUNES_PATH, query)) {
		// Create string.
		printDebug("Fortunes fetched.");
		FirebaseJson &json = fbData.jsonObject();
		String jsonStr;
		json.toString(jsonStr, false); // Param two is prettify.
		printDebug(jsonStr);

		// Pick from the list.
		uint8_t picker;
		JsonObject listObj = buildFortuneIndex(jsonStr, index);
		printDebug("SIZE:" + String(listObj.size()));

		String fortuneId = index[random(0, listObj.size())];
		const char* fortune = listObj[fortuneId][FIELD_TEXT];
		printDebug(fortune);
		txtToScreen(fortune, DELAY_FORTUNE, 0);
		increaseMetric(fortuneId, FIELD_SHOWN);

		// Ask for accuracy.
		paint(MESSAGES[ACCURATE], DELAY_NONE);
		int result = ask(timeout);
		if (result == ANSWER_YES) {
			paint(MESSAGES[CORRECT], DELAY_MSG);
			increaseMetric(index[picker].c_str(), FIELD_VOTES);
		} else if (result == ANSWER_NO) {
			paint(MESSAGES[WRONG], DELAY_MSG);
		} else {
			paint(MESSAGES[TIMEOUT], DELAY_MSG);
		}

		// Record interaction data.
		saveInteraction(atoi(fortuneId.c_str()), category, result, sensor, version, randomChoice);
	} else printDebug(fbData.errorReason());
	fbData.clear();
}

/**
 * Get the question from stash.
 * Handle end of question tree.
 * Display and handle answer.
 */
void askQuestion (String id, unsigned int version) {
	if (!NOCHROME) play(APPEAR_FRAMES, 6);
	// Grab data from cached global and show.
	Question question;
	bool questionFound = false;
	for (uint8_t i = 0; i < sizeof(Q_LIST) / sizeof(Q_LIST[0]); i++) {
		question = Q_LIST[i];
		if (question.name == id) {
			printDebug(question.text);
			if (!NOCHROME) play(APPEAR_FRAMES, 6);
			txtToScreen(question.text, DELAY_NONE, 0);
			questionFound = true;
			break;
		}
		delay(0);
	}
	// End of the question tree.
	if (!questionFound) {
		// @todo Get sensor reading.
		double sensor = 123.456;
		fetchFortune(id, sensor, version, QUESTION_TIMEOUT);
		// @todo Handle no fortune.
		return;
	}

	int result = ask(QUESTION_TIMEOUT);
	String next;
	// Go to the next step.
	if (result == ANSWER_YES) {
		paint(MESSAGES[PICK_YES], DELAY_QUICK_MSG);
		next = question.nextYes;
	} else if (result == ANSWER_NO) {
		paint(MESSAGES[PICK_NO], DELAY_QUICK_MSG);
		next = question.nextNo;
	} else {
		paint(MESSAGES[RANDOM], DELAY_MSG);
		// Mark interactions with random selection.
		randomChoice = true;
		next = (random(9) % 2) ? question.nextYes : question.nextNo;
	}
	askQuestion(next, question.version);
}

/**
 * Handle kickoff of interaction.
 */
void coin () {
	// Greeting routine.
	printDebug("Coin! **********");
	if (!NOCHROME) {
		play(WAKE_FRAMES, 19);
		paint(MESSAGES[GREET1], DELAY_MSG);
		play(APPEAR_FRAMES, 6);
		paint(MESSAGES[GREET2], DELAY_QUICK_MSG);
		play(APPEAR_FRAMES, 6);
		paint(MESSAGES[GREET3], DELAY_QUICK_MSG);
	} else {
		paint(MESSAGES[FREEBIE], DELAY_MSG);
	}
	// Start question tree.
	randomChoice = false;
	String next = FIRST_QUESTION_ID;
	askQuestion(next, 0);
}

/**
 * Listen for coins and nudge animation.
 */
void sleep () {
	timer++;
	if (timer == DELAY_SLEEP) {
		paint(SLEEP_FRAMES[sleepFrame], DELAY_NONE);
		sleepFrame = (sleepFrame < 3) ? sleepFrame + 1 : 0;
		timer = 0;
	}
	delay(10);
}


// STANDARD...

void setup (void) {
	lcd.begin(20, 4);
	pinMode(TRIGGER_PIN, INPUT);
	pinMode(BTN1_PULLUP, INPUT_PULLUP);
	pinMode(BTN2_PULLUP, INPUT_PULLUP);
	if (DEBUG) Serial.begin(1000000);
	randomSeed(analogRead(0));

	// On-the-fly alternate modes.
	if (digitalRead(BTN1_PULLUP) == LOW) {
		NOCHROME = true;
	}
	if (digitalRead(BTN2_PULLUP) == LOW) {
		FREEPLAY = true;
	}

	if (!NOCHROME) paint(MESSAGES[BOOT], DELAY_QUICK_MSG);
	else {
		lcd.setCursor(0, 1);
		lcd.print("      8(*__*)8");
	}

	if (connect()) {
		if (TESTING) {
			printDebug("Running fortune tests...");
			fetchFortune ("heart", 123.00, 2, FAST_TIMEOUT);
			fetchFortune ("fate", 123.00, 2, FAST_TIMEOUT);
			fetchFortune ("health", 123.00, 2, FAST_TIMEOUT);
			fetchFortune ("fortune", 123.00, 2, FAST_TIMEOUT);
		}
		fetchQuestions();
		printDebug("Ready.");
		paint(SLEEP_FRAMES[0], DELAY_NONE);
	}
}

void loop (void) {
	trigger = digitalRead(TRIGGER_PIN);
	if (trigger == HIGH || (FREEPLAY && digitalRead(BTN2_PULLUP) == LOW)) {
		coin();
	} else sleep();
}
