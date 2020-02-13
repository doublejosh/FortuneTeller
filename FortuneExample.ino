#include <LiquidCrystal.h>

const int DELAY = 5000;

const int rs = D5,
          en = D0,
          d4 = D1,
          d5 = D2,
          d6 = D3,
          d7 = D4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const String fortunes[][4] = {{
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
int numFortunes = 5;

void setup(void) {
  Serial.begin(115200);
  lcd.begin(20, 4);
  randomSeed(analogRead(0));
}
 
void loop(void) {
  int f = random(numFortunes);
  int fortuneLength = sizeof(fortunes[f]) / sizeof(fortunes[f][0]);

  lcd.noDisplay();
  lcd.clear();
  Serial.print("Printing fortune: ");
  Serial.println(f);
  delay(DELAY/2);

  for (uint8_t i=0; i < fortuneLength - 1; i++) {
    lcd.setCursor(0, i);
    lcd.print(fortunes[f][i]);
  }
  lcd.display();
  delay(DELAY);
}
