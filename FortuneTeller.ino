#include <LiquidCrystal.h>
#include "FirebaseESP8266.h"
#include "StringSplitter.h"
#include "animation.h"
#include "messages.h"

const int rs = D5,
		  en = D0,
		  d4 = D1,
		  d5 = D2,
		  d6 = D3,
		  d7 = D4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const uint8_t TRIGGER_PIN = D6,
			  BTN1_PULLUP = D7,
			  BTN2_PULLUP = D8;

#define FIREBASE_HOST "fortunecalibrator.firebaseio.com"
#define FIREBASE_AUTH "pwYQO2x4KMEGA9cPXC2VoOV7tJwH7yrPVHuDnUAc"
String WIFI_SSID[] = {
  "FBIsurveillanceVan-2.4G",
  "WingCommander"
};
String WIFI_PASSWORD[] = {
  "camstamp",
  "whatdidyousay"
};
uint8_t wifiTry = 0;
boolean offline = false;
FirebaseData fbData;
FirebaseJson fbJson;

uint8_t sleepFrame = 0;
uint32_t timer = 0;


// CONFIG.

#define WIDTH 20
#define HEIGHT 4

uint16_t DELAY_MSG = 4000,
   DELAY_ANIMATION = 250,
	   DELAY_SLEEP = 50000;


// UTILITIY.

typedef struct {
	uint8_t id;
	String name;
	String text;
	String nextNo;
	String nextYes;
} Question;

typedef struct {
	String text;
	String category;
	unsigned int votes;
} Fortune;

Question Q_LIST[3];

void message (char msg[4][WIDTH+1]) {
	paint(msg, DELAY_MSG);
}

void play (char frames[][4][WIDTH+1], uint8_t count) {
	for (uint8_t f = 0; f < count; f++) {
		paint(frames[f], DELAY_ANIMATION);
	}
}

void paint (char screen[4][WIDTH+1], int wait) {
	lcd.noDisplay();
	lcd.clear();
	for (uint8_t i = 0; i < sizeof(screen); i++) {
		lcd.setCursor(0, i);
		lcd.print(String(screen[i]));
	}
	lcd.display();
	delay(wait);
}

void txtToScreen (String msg, int row) {
	lcd.clear();
	lcd.setCursor(0, row);
	lcd.print(msg);
	delay(DELAY_MSG);
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

void event (int fortune, int category, boolean accurate, String heartbeat) {
	Serial.println("SAVING...");
	fbJson.clear();
	fbJson.set("fortune", fortune);
	fbJson.set("category", category);
	fbJson.set("accurate", accurate);
	fbJson.set("heartbeat", heartbeat);
	fbJson.set("version", 1);

	if (Firebase.pushJSON(fbData, String("/interactions/"), fbJson)) {
		Serial.println("SAVED");
	} else Serial.println("SAVE ERROR");
}

void connect (void) {
	lcd.display();
	for (uint8_t i = 0; i < 2; i++) {
		WiFi.begin(WIFI_SSID[i], WIFI_PASSWORD[i]);

		Serial.print("Connecting to: ");
		Serial.println(WIFI_SSID[i]);
		txtToScreen("Connecting to:", 1);
		lcd.setCursor(0, 2);
		lcd.print(WIFI_SSID[i].substring(0, 20));

		while (WiFi.status() != WL_CONNECTED) {
			Serial.print(" .");
			delay(2000);
			if (wifiTry == 3) {
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
			txtToScreen("Connected:", 1);
			String wifi = WiFi.localIP().toString();
			txtToScreen(wifi, 2);
			break;
		}
		else {
			Serial.println("Unable to connect.");
			txtToScreen("Unable to connect.", 1);
		}
	}

  	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("Unable to connect.");
    	message(MESSAGES[2]);
		offline = true;
  	} else {
		Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
		Firebase.reconnectWiFi(true);
	}
}


// APPLICATION.

String getField (String key) {
	if (Firebase.getShallowData(fbData, key)) {
		if (fbData.dataType() == "string") {
			String text = fbData.stringData();
			return text;
		}
	} else Serial.println(fbData.errorReason());
}

Question getQuestion (uint8_t i, String name, String &key) {
	Question question = { i, name,
		getField(key + "/text"),
		getField(key + "/nextYes"),
		getField(key + "/nextNo")
	};
	return question;
}

void fetchQuestions () {
	txtToScreen("Fetching questions.", 1);

	String key = "/questions";
  	if (Firebase.getShallowData(fbData, key)) {
		FirebaseJson json;
		size_t count = json.iteratorBegin(fbData.jsonString().c_str());
		String _key, _val;
		int _type = 0;
		for (size_t i = 0; i < count; i++) {
			json.iteratorGet(i, _type, _key, _val);
			if (_val == "true") {
				Question question = getQuestion(i, _key, key + "/" + _key);
				Q_LIST[i] = question;
			}
		}
		json.iteratorEnd();
    }
  	else Serial.println(fbData.errorReason());
}

void askQuestion (String *id) {
	Question question;
	uint8_t len = sizeof(Q_LIST) / sizeof(Q_LIST[0]);
	for (uint8_t i = 0; i < len; i++) {
		question = Q_LIST[i];
		if (question.name == *id) {
			Serial.println(question.text);
			txtToScreen(question.text, 1);
			break;
		}
	}

	// Receive a vote.
	int sensorValue = digitalRead(2);

	// Decide if done.

	// Ask another question.
}

int showFortune () {
	int f = random(sizeof(FORTUNES[0]) / sizeof(FORTUNES));
	message(FORTUNES[f]);
	return f;
}

void coin () {
	// Greeting routine.
	Serial.println("TRIGGER");
	play(WAKE_FRAMES, 19);
	message(MESSAGES[GREET1]);
	play(APPEAR_FRAMES, 6);
	message(MESSAGES[GREET2]);
	play(APPEAR_FRAMES, 6);

	String next = "first";
	askQuestion(&next);

	next = "concrete";
	askQuestion(&next);

	next = "abstract";
	askQuestion(&next);

	int f = showFortune();
	play(APPEAR_FRAMES, 6);

	if (!offline) {
		// Ask for accuracy.

		// Record interaction.
		event(f, 2, false, "555.555");
	}
}

void sleep () {
	timer++;
	if (timer == DELAY_SLEEP) {
		paint(SLEEP_FRAMES[sleepFrame], 0);
		sleepFrame = (sleepFrame < 3) ? sleepFrame + 1 : 0;
		timer = 0;
	}
}


// STANDARD.

void setup (void) {
	lcd.begin(20, 4);
	pinMode(TRIGGER_PIN, INPUT);
	pinMode(BTN1_PULLUP, INPUT_PULLUP);
	pinMode(BTN2_PULLUP, INPUT_PULLUP);
	Serial.begin(1000000);
	message(MESSAGES[BOOT]);
	connect();
	fetchQuestions();
	randomSeed(analogRead(0));
}

void loop (void) {
	const int trigger = digitalRead(TRIGGER_PIN);
	if (trigger == HIGH) coin();
	else sleep();
}






        // FirebaseJson &json = fbData.jsonObject();
		// String jsonStr;
		// json.first.toString(jsonStr);
		// Serial.println(jsonStr);

        // size_t len = json.iteratorBegin();
        // String key, value = "";
        // int type = 0;
        // for (size_t i = 0; i < len; i++) {
        //     json.iteratorGet(i, type, key, value);
        //     Serial.print(i);
        //     Serial.print(", ");
        //     Serial.print("Type: ");
        //     Serial.print(type);
        //     if (type == JSON_OBJECT) {
        //         Serial.print(", Key: ");
        //         Serial.print(key);
        //     }
        //     Serial.print(", Value: ");
        //     Serial.println(value);
        // }
        // json.iteratorEnd();

		// FirebaseJsonArray &questionList = fbData.jsonArray();
		// for (size_t i = 0; i < questionList.size(); i++) {
		// 	FirebaseJsonData &jsonData = fbData.jsonData();
		// 	questionList.get(jsonData, i);
		// 	fbJson.get(jsonData, "text");

		// 	//Serial.println(jsonData.stringValue);
		// }

	// } else Serial.println("FETCH ERROR");


void getNode (String &key) {
	if (Firebase.getShallowData(fbData, key)) {
		if (fbData.dataType() != "json") {
			Serial.print(key);
			Serial.print("->");

			if (fbData.dataType() == "int")
				Serial.println(fbData.intData());
			else if (fbData.dataType() == "float")
				Serial.println(fbData.floatData(), 5);
			else if (fbData.dataType() == "double")
				printf("%.9lf\n", fbData.doubleData());
			else if (fbData.dataType() == "boolean")
				Serial.println(fbData.boolData() == 1 ? "true" : "false");
			else if (fbData.dataType() == "string")
				Serial.println(fbData.stringData());
			else if (fbData.dataType() == "null")
				Serial.println("null");
		}
	}
	else Serial.println(fbData.errorReason());
}
