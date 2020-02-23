#include <LiquidCrystal.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <math.h>
#include "utilities.h"
#include "animation.h"
#include "messages.h"

typedef struct {
	uint8_t id;
	String name;
	String nextYes;
	String nextNo;
	String text;
} Question;

typedef struct {
	unsigned int id;
	String text;
} Fortune;

FirebaseData fbData;
FirebaseJson fbJson;
DynamicJsonDocument jsonDoc(JSON_DOC_BYTES);
Question Q_LIST[QUESTION_LIST_LENGTH];
uint8_t wifiTry = 0, sleepFrame = 0;
uint32_t timer = 1;
boolean offline = false;
uint16_t trigger;

LiquidCrystal lcd(
	displayRS, displayEN, displayD4,
	displayD5, displayD6, displayD7
);


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
	// char screen[HEIGHT][WIDTH+1];

	// const char* arr = {};
	// //char* buffer[WIDTH+1] = {};
	// for (uint8_t i=0; i<HEIGHT; i++) {
	// 	String slice = msg.substring(i*WIDTH, (i*WIDTH)+WIDTH);
	// 	arr = slice.c_str();
	// 	for (uint8_t c=0; c<WIDTH; c++) {
	// 		screen[i][c] = arr[c];
	// 		// slice += "\0";
	// 		// buffer = slice.c_str(WIDTH);
	// 		//slice.toCharArray(buffer, WIDTH);
	// 		//screen[i] = slice.c_str();
	// 		//Serial.println(sizeof(screen[i]));
	// 	}
	// 	arr = "";
	// }
	// paint(screen, DELAY_MSG);
}

/**
 * Handle wifi connection.
 */
bool connect (void) {
	for (uint8_t i = 0; i < 2; i++) {
		WiFi.begin(WIFI_SSID[i], WIFI_PASSWORD[i]);

		Serial.print("Connecting: ");
		Serial.println(WIFI_SSID[i]);
		if (!NOCHROME) txtToScreen("Connecting: " + WIFI_SSID[i].substring(0, 8), DELAY_MSG, 1);

		while (WiFi.status() != WL_CONNECTED) {
			Serial.print(" .");
			delay(WIFI_RETRY_DELAY);
			if (wifiTry == 4) {
				WiFi.disconnect();
				Serial.println();
				wifiTry = 0;
				break;
			}
			else wifiTry += 1; 
		}

		if (WiFi.status() == WL_CONNECTED) {
			Serial.println("Connected: ");
			Serial.println(WiFi.localIP());
			if (!NOCHROME) txtToScreen("Connected.", DELAY_QUICK_MSG, 1);
			String wifi = WiFi.localIP().toString();
			if (!NOCHROME) txtToScreen(wifi, DELAY_MSG, 2);
			break;
		}
		else {
			Serial.println("Unable to connect.");
			txtToScreen("Unable to connect.", DELAY_MSG, 1);
		}
	}

  	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("Unable to connect.");
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
 * Retrieve fields within JSON data.
 */
String getField (String key) {
	if (Firebase.getShallowData(fbData, key)) {
		if (fbData.dataType() == "string") {
			String text = fbData.stringData();
			return text;
		}
	} else Serial.println(fbData.errorReason());
}

/**
 * Get cached data for specific question.
 */
Question getQuestion (uint8_t i, String name, String &key) {
	Question question = { i, name,
		getField(key + "/nextYes"),
		getField(key + "/nextNo"),
		getField(key + "/text")
	};
	Serial.println(fbData.jsonString());
	return question;
}

/**
 * Get list of questions online and stash for later.
 */
void fetchQuestions () {
	if (!NOCHROME) txtToScreen("Fetching questions.", DELAY_QUICK_MSG, 1);
	String path = "/questions";
	if (Firebase.getShallowData(fbData, path)) {

		FirebaseJson json;
		size_t lineCount = json.iteratorBegin(fbData.jsonString().c_str());
		String _key, _val;
		int _type = 0;
		for (size_t i = 0; i < lineCount; i++) {
			json.iteratorGet(i, _type, _key, _val);
			if (_val == "true") {
				Question question = getQuestion(i, _key, path + "/" + _key);
				Q_LIST[i] = question;
			}
			yield();
		}
		json.iteratorEnd();
	}
	else Serial.println(fbData.errorReason());
}

/**
 * Boost the vote of a fortune.
 */
void increaseCount(String fortuneId, String field) {
	int votes;
	String path = "/fortunes/" + fortuneId + "/" + field;
	if (Firebase.getInt(fbData, path)) {
		votes = fbData.intData();
		Serial.print("Vote Count: ");
		Serial.println(votes);
		// Update value.
		if (Firebase.setInt(fbData, path, votes+1)) {
			print("Votes updated.");
		} else Serial.println(fbData.errorReason());
	} else Serial.println(fbData.errorReason());
}

/**
 * Wait and collect a yes no answer from user.
 */
#define ANSWER_YES 1
#define ANSWER_NO 0
#define ANSWER_SKIP -1
int8_t ask () {
	// Time boundary on question.
	int seconds;
	unsigned long startTime = millis(),
				currentTime = millis();
	while (currentTime - startTime <= QUESTION_TIMEOUT) {
		currentTime = millis();
		uint8_t seconds = (currentTime - startTime) / 1000;
		lcd.setCursor(18, 3);
		lcd.print("  ");
		lcd.setCursor(18, 3);
		lcd.print(String((QUESTION_TIMEOUT / 1000) - seconds));
		// Accept the answer.
		bool voteYes = (digitalRead(BTN2_PULLUP) == LOW);
		if (voteYes || (digitalRead(BTN1_PULLUP) == LOW)) {
			return (voteYes) ? ANSWER_YES : ANSWER_NO;
		}
		delay(200);
	}
	return ANSWER_SKIP;
}

/**
 * Organize fortunes into indexed list, to pick randomly, etc.
 */
JsonObject buildFortuneIndex(String jsonStr, String index[]) {
	// Parse JSON from string.
	DeserializationError e = deserializeJson(jsonDoc, jsonStr);
	if (e.code() == DeserializationError::NoMemory) {
		Serial.print(F("Not enough JSON memory."));
	}
	// Create index of ids.
	JsonObject listObj = jsonDoc.as<JsonObject>();
	uint8_t id = 0;
	for (JsonObject::iterator i=listObj.begin(); i!=listObj.end(); ++i) {
		index[id] = String(i->key().c_str());
		id++;
		// Keep the ESP watchdog happy (avoid soft reset).
		delay(0);
	}
	return listObj;
}

/**
 * Save interaction online.
 */
void saveInteraction (int fortune, String category, boolean accurate, String heartbeat) {
	Serial.println("SAVING...");
	fbJson.clear();
	fbJson.set("fortune", fortune);
	fbJson.set("category", category);
	fbJson.set("accurate", accurate);
	fbJson.set("heartbeat", heartbeat);
	fbJson.set("version", 1);

	if (!NOCHROME) paint(MESSAGES[SAVE], DELAY_MSG);
	if (Firebase.pushJSON(fbData, String("/interactions/"), fbJson)) {
		Serial.println("SAVED");
		if (!NOCHROME) paint(MESSAGES[FUTURE], DELAY_MSG);
	} else Serial.println("SAVE ERROR");
}

/**
 * Get fortune online based on chosen category.
 */
void fetchFortune (const String category) {
	if (!NOCHROME) paint(MESSAGES[FETCHING], DELAY_MSG);
	print("Fortune category: " + category);
	// Fetch relevant fortunes.
	QueryFilter query;
	query.orderBy("category");
	query.equalTo(category);
	query.limitToFirst(10);
	String path = "/fortunes";
	String index[FORTUNE_INDEX_MAX];
	if (Firebase.getJSON(fbData, path, query)) {
		FirebaseJson &json = fbData.jsonObject();
		String jsonStr;
		json.toString(jsonStr, true);

		// Pick from the list.
		JsonObject listObj = buildFortuneIndex(jsonStr, index);
		uint8_t picker = random(0, listObj.size());
		const char* fortune = listObj[index[picker]]["text"];
		Serial.println(fortune);
		txtToScreen(fortune, DELAY_FORTUNE, 0);
		increaseCount(index[picker].c_str(), "shown");

		// Ask for accuracy.
		paint(MESSAGES[ACCURATE], DELAY_NONE);
		int result = ask();
		if (result == ANSWER_YES) {
			paint(MESSAGES[CORRECT], DELAY_MSG);
			increaseCount(index[picker].c_str(), "votes");
		} else if (result == ANSWER_NO) {
			paint(MESSAGES[WRONG], DELAY_MSG);
		} else {
			paint(MESSAGES[TIMEOUT], DELAY_MSG);
		}

		// Record interaction data.
		unsigned int fortuneId = atoi(index[picker].c_str());
		saveInteraction(fortuneId, category, result, "555.555");

	} else Serial.println(fbData.errorReason());

	// @todo Handle failures.
}

/**
 * Get the questioned requested from cache.
 * Display and provide time to answer.
 * Handle end of question tree.
 */
void askQuestion (String id) {
	// Grab data from cached global and show.
	Question question;
	bool questionFound = false;
	for (uint8_t i = 0; i < sizeof(Q_LIST) / sizeof(Q_LIST[0]); i++) {
		question = Q_LIST[i];
		if (question.name == id) {
			Serial.println(question.text);
			if (!NOCHROME) play(APPEAR_FRAMES, 6);
			txtToScreen(question.text, DELAY_NONE, 0);
			questionFound = true;
			break;
		}
	}
	// End of the question tree.
	if (!questionFound) {
		// @todo Get sensor reading.
		fetchFortune(id);
		// @todo Handle no fortune.
		return;
	}

	int result = ask();
	// Go to the next step.
	if (result == ANSWER_YES) {
		txtToScreen("You picked YES.", DELAY_QUICK_MSG, 1);
		askQuestion(question.nextYes);
	} else if (result == ANSWER_NO) {
		txtToScreen("You picked NO.", DELAY_QUICK_MSG, 1);
		askQuestion(question.nextNo);
	} else {
		paint(MESSAGES[RANDOM], DELAY_MSG);
		askQuestion((random(9) % 2) ? question.nextYes : question.nextNo);
	}
}

/**
 * Handle kickoff of interaction.
 */
void coin () {
	// Greeting routine.
	Serial.println("Coin!");
	if (!NOCHROME) {
		play(WAKE_FRAMES, 19);
		paint(MESSAGES[GREET1], DELAY_MSG);
		play(APPEAR_FRAMES, 6);
		paint(MESSAGES[GREET2], DELAY_QUICK_MSG);
		play(APPEAR_FRAMES, 6);
		paint(MESSAGES[GREET3], DELAY_QUICK_MSG);
	} else {
		txtToScreen("   A freebie, ok.   ", DELAY_QUICK_MSG, 1);
	}
	// Start question tree.
	String next = FIRST_QUESTION_ID;
	askQuestion(next);
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
	Serial.begin(1000000);
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
		fetchQuestions();
		paint(SLEEP_FRAMES[0], DELAY_NONE);
	}
}

void loop (void) {
	trigger = digitalRead(TRIGGER_PIN);
	if (trigger == HIGH || (FREEPLAY && digitalRead(BTN2_PULLUP) == LOW)) {
		coin();
	} else sleep();
}
