#include <LiquidCrystal.h>
#include "FirebaseESP8266.h"
#include "utilities.h"

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
FirebaseJson json;

const int rs = D5,
	en = D0,
	d4 = D1,
	d5 = D2,
	d6 = D3,
	d7 = D4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define WIDTH 20
#define HEIGHT 4

const int TRIGGER_PIN = D6;

uint8_t sleepFrame = 0;
uint32_t timer = 0;
const char *SLEEP_FRAMES[][4][20] = {{
		"COIN                ",
		"                    ",
		"       _==z=_       ",
		"     8( u..u )8     "
	}, {
		"              PLEASE",
		"          z         ",
		"       _====_       ",
		"     8( u..u )8     "
	}, {
		"                    ",
		"         zZ         ",
		"       _====_       ",
		"     8( u..u )8     "
	}, {
		"        zZz         ",
		"                    ",
		"       _====_       ",
		"     8( u..u )8     "
	}};

const char *WAKE_FRAMES[][4][20] = {{
		"                    ",
		"                    ",
		"       _====_       ",
		"     8( o..- )8     "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"     8( 0..- )8     "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"     8( -..o )8     "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"     8( -..O )8     "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"   q 8( o..0 )8 p   "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"   q 8( O..O )8 p   "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"   q 8( 0..0 )8 p   "
	}, {
		"                    ",
		"      . .  . .      ",
		"   .   _====_   .   ",
		" . q 8( O..O )8 p . "
	}, {
		"         .  .       ",
		"      `      '      ",
		"   `   _====_   '   ",
		" - q 8( O..O )8 p - "
	}, {
		"     `        '     ",
		"  `              '  ",
		"       _====_       ",
		"-  q 8( O..O )8 p  -"
	}, {
		" `                ' ",
		"                    ",
		"       _====_       ",
		"   q 8( 0..0 )8 p   "
	}, {
		"                   '",
		"                    ",
		"       _====_       ",
		"   q 8( O..O )8 p   "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"   q 8( O..- )8 p   "
	}, {
		"                    ",
		"           wink     ",
		"       _====_       ",
		"   q 8( O..- )8 p   "
	}, {
		"                    ",
		"      . .  . .      ",
		"   _ - _====_ - _   ",
		" _ q 8( O..O )8 p _ "
	}, {
		"        .  .        ",
		"     `        '     ",
		"  . `  _====_  ' .  ",
		" . q 8( O..O )8 p . "
	}, {
		"    .          .    ",
		"   .            .   ",
		" -                - ",
		" -  q  _====_  p   -"
	}, {
		"   `            '   ",
		"  -              -  ",
		"`                  '",
		"-    q _====_ p    -"
	}, {
		".                  .",
		"                    ",
		"                    ",
		"      q      p      "
  }};

const char *APPEAR_FRAMES[][4][20] = {{
		"                    ",
		"                    ",
		"                    ",
		"      q      p      "
	}, {
		"                    ",
		"                    ",
		"                    ",
		"     q _====_ p     "
	}, {
		"                    ",
		"                    ",
		"                    ",
		"    q  _====_  p    "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"   q 8( O..O )8 p   "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"  q  8( -..O )8  p  "
	}, {
		"                    ",
		"                    ",
		"       _====_       ",
		"   q 8( O..- )8 p   "
  }};

const char *QUESTIONS[][4][20] = {{
		"                    ",
		"   Can you reason   ",
		"   your way out of  "
		"   this one?        ",
		"                    "
	}, {
		"                    ",
		"   Is there love    ",
		"   in this story?   ",
		"                    "
	}, {
		"                    ",
		"  health vs wealth  ",
		"                    ",
		"                    "
	}};

const char *FORTUNES[][4][20] = {{
    "A hunch is just     ",
    "creativity trying to",
    "tell you something. ",
		"                    "
  }, {
		"A soft voice may be ",
		"awfully persuasive. ",
		"                    ",
		"                    "
  	}, {
		"Adventure can be    ",
		"real happiness.     ",
		"                    ",
		"                    "
	}, {
		"At the touch of love",
		"everyone becomes a  ",
		"poet.               ",
		"                    "
	}, {
		"A friend is a       ",
		"present you give    ",
		"yourself.           ",
		"                    "
	}};

#define GREET1 0
#define GREET2 1
const char *MESSAGES[][4][20] = {{
		"                    ",
		"  Contemplate your  ",
		"  uncertainty...    ",
		"                    "
	}, {
		"                    ",
		"   actually,        ",
		"       perhaps...   ",
		"                    "
	}, {
		"                    ",
		"    WiFi Error :(   ",
		"                    ",
		"                    "
	}};

//Frames frames;

int DELAY_MSG = 4000,
    DELAY_ANIMATION = 250,
		DELAY_SLEEP = 50000;


// UTILITY.

Utils utils;

void message (const char *msg[4][20]) {
	paint(msg, DELAY_MSG);
}

void play (const char *frames[][4][20], uint8_t count) {
  for (uint8_t f = 0; f < count; f++) {
		paint(frames[f], DELAY_ANIMATION);
  }
}

void paint (const char *screen[4][20], int wait) {
	lcd.noDisplay();
	lcd.clear();

	for (uint8_t i = 0; i < sizeof(screen); i++) {
		lcd.setCursor(0, i);
		lcd.print(String(screen[0][i]));
		// Serial.println(String(screen[0][i]));
	}

	lcd.display();
	delay(wait);
}

void event (int fortune, int category, boolean accurate, String heartbeat) {
	Serial.println("SAVING...");
	json.clear();
	json.set("fortune", fortune);
	json.set("category", category);
	json.set("accurate", accurate);
	json.set("heartbeat", heartbeat);
	json.set("version", 1);

	if (Firebase.pushJSON(fbData, String("/interactions/"), json)) {
		Serial.println("SAVED");
	} else Serial.println("SAVE ERROR");
}


		//for (size_t i = 0; i < list.size(); i++) {
			// Convert to JSON.
// 			FirebaseJsonData &jsonData = fbData.jsonData();
// 			list.get(jsonData, i);
// 			if (jsonData.typeNum == JSON_OBJECT) {
// 				Serial.println(jsonData.stringValue);
// 				return jsonData;
// 			}
// 			else Serial.println('WRONG TYPE');
// 		}

// 	} else Serial.println(fbData.errorReason());
// }



void connect (void) {
	for (uint8_t i = 0; i < 2; i++) {
		WiFi.begin(WIFI_SSID[i], WIFI_PASSWORD[i]);
		Serial.println();
		Serial.print("Connecting to: ");
		Serial.print(WIFI_SSID[i]);

		while (WiFi.status() != WL_CONNECTED) {
			Serial.print(" .");
			delay(2000);
			if (wifiTry == 3) {
				WiFi.disconnect();
				wifiTry = 0;
				break;
			}
			else wifiTry += 1; 
		}

		if (WiFi.status() == WL_CONNECTED) {
			Serial.println();
			Serial.print("Connected: ");
			Serial.println(WiFi.localIP());
			break;
		}
		else {
			Serial.println();
			Serial.print("Unable to connect.");
		}
	}

  if (WiFi.status() != WL_CONNECTED) {
    message(MESSAGES[2]);
		offline = true;
  } else {
		Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
	}
}


// APPLICATION.

void coin () {
	Serial.println("TRIGGER");
	play(WAKE_FRAMES, 19);
	message(MESSAGES[GREET1]);
	play(APPEAR_FRAMES, 6);
	message(MESSAGES[GREET2]);
	play(APPEAR_FRAMES, 6);

	// Ask seires of questions.
	String next, category;
	if (next = askQuestion("first")) {
		// Learn fortune category.
		//category = askQuestion(next);

		int f = showFortune();
		play(APPEAR_FRAMES, 6);

		if (!offline) {
			// Ask for accuracy.

			// Record interaction.
			event(f, 2, false, "555.555");
		}
	}
}

void fetchQuestions () {
	if (Firebase.get(fbData, String("/questions"))) {
		FirebaseJsonArray &questions = fbData.jsonArray();
		FirebaseJsonData &jsonData = fbData.jsonData();
		questions.get(jsonData, 0);
		Serial.println(jsonData.stringValue);
	}
}

String askQuestion (String id) {
	Serial.println("ASKING QUESTION");
	return "abstract";
}

int showFortune () {
	int f = random(sizeof(FORTUNES[0]) / sizeof(FORTUNES));
	message(FORTUNES[f]);
	return f;
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
  	lcd.display();
	randomSeed(analogRead(0));
	pinMode(TRIGGER_PIN, INPUT);
	Serial.begin(1000000);
	connect();
	fetchQuestions();
}

void loop (void) {
	const int trigger = digitalRead(TRIGGER_PIN);
	if (trigger == HIGH) coin();
	else sleep();
}
