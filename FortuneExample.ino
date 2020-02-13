#include <LiquidCrystal.h>

// Display screen pins.
const int rs = D5,
	en = D0,
	d4 = D1,
	d5 = D2,
	d6 = D3,
	d7 = D4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Use pin for coin trigger.
const int TRIGGER_PIN = D6;

// List of fortunes.
const int NUM_FORTUNES = 5;
// Filling all characters for each 
const String FORTUNES[][4] = {{
    	"A hunch is just     ",
    	"creativity trying to",
    	"tell you something. "
  	}, {
		"A soft voice may be ",
		"awfully persuasive. "
  	}, {
		"Adventure can be    ",
		"real happiness.     "
	}, {
		"At the touch of love",
		"everyone becomes a  ",
		"poet."
	}, {
		"A friend is a       ",
		"present you give    ",
		"yourself.           "
	}};

const int DELAY = 6000;

void setup(void) {
	// Define screen size.
	lcd.begin(20, 4);
	// Allows debug monitoring.
	Serial.begin(115200);
	// Enabled true random.
	randomSeed(analogRead(0));
	// Allow using trigger pin.
	pinMode(TRIGGER_PIN, INPUT);
}
 
void loop(void) {
	const int trigger = digitalRead(TRIGGER_PIN);
	Serial.print("TRIGGER: ");
	Serial.println(trigger);

	// When pin gets triggered, show a fortune.
	if (trigger == HIGH) {
		// Pick random fortune.
		lcd.display();
		showFortune(random(NUM_FORTUNES));
	} else {
		// Reset the display.
		lcd.noDisplay();
		lcd.clear();
	}
}

void showFortune(int f) {
	// Pretend to compute the fortune.
	for (uint8_t s=0; s < 10; s++) {
		lcd.setCursor(0, 1);
		lcd.print("  * * THINKING * *  ");
		delay(250);
		lcd.clear();
		delay(250);
	}

	// Debugging.
	Serial.print("Printing fortune: ");
	Serial.println(f);

	// Print all lines of the fortune.
	uint8_t lines = sizeof(FORTUNES[f]) / sizeof(FORTUNES[f][0]);
	for (uint8_t i=0; i < lines; i++) {
		lcd.setCursor(0, i);
		lcd.print(FORTUNES[f][i]);
	}
	delay(DELAY);
}
