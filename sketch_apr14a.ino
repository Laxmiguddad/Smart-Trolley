#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define RED_LED 2
#define GREEN_LED 3
#define BUZZER 4
#define BUTTON 5
 
 
byte kurkureUID[4] = {0x3, 0x9D, 0x2A, 0x31};
byte spriteUID[4]  = {0x26, 0x88, 0x44, 0x3F};

// Prices
int kurkurePrice = 10;
int spritePrice = 20;

// Counters
int kurkureCount = 0;
int spriteCount = 0;
int total = 0;

bool compareUID(byte *a, byte *b) {
for (byte i = 0; i < 4; i++) {
if (a[i] != b[i]) return false;
}
return true;
}

void setup() {
Serial.begin(9600);
SPI.begin();
rfid.PCD_Init();

lcd.init();
lcd.backlight();

pinMode(RED_LED, OUTPUT);
pinMode(GREEN_LED, OUTPUT);
pinMode(BUZZER, OUTPUT);
pinMode(BUTTON, INPUT_PULLUP);

lcd.print("Smart Trolley");
delay(1500);
lcd.clear();
lcd.print("Scan Item...");
}

void loop() {

bool removeMode = (digitalRead(BUTTON) == LOW);

if (removeMode) {
lcd.setCursor(0,3);
lcd.print("REMOVE MODE     ");
}

if (!rfid.PICC_IsNewCardPresent()) return;
if (!rfid.PICC_ReadCardSerial()) return;

lcd.clear();

// 🛒 KURKURE
if (compareUID(rfid.uid.uidByte, kurkureUID)) {

if (!removeMode) {
  kurkureCount++;
  total += kurkurePrice;
  lcd.print("Kurkure Added");
} else {
  if (kurkureCount > 0) {
    kurkureCount--;
    total -= kurkurePrice;
    lcd.print("Kurkure Removed");
  } else {
    lcd.print("None to Remove");
  }
}

}

// 🛒 SPRITE
else if (compareUID(rfid.uid.uidByte, spriteUID)) {

if (!removeMode) {
  spriteCount++;
  total += spritePrice;
  lcd.print("Sprite Added");
} else {
  if (spriteCount > 0) {
    spriteCount--;
    total -= spritePrice;
    lcd.print("Sprite Removed");
  } else {
    lcd.print("None to Remove");
  }
}

}

else {
lcd.print("Unknown Item");
digitalWrite(RED_LED, HIGH);
delay(1000);
digitalWrite(RED_LED, LOW);
return;
}

// Feedback
digitalWrite(GREEN_LED, HIGH);
tone(BUZZER, 1000);
delay(300);
digitalWrite(GREEN_LED, LOW);
noTone(BUZZER);

// Display bill
lcd.setCursor(0,1);
lcd.print("K:");
lcd.print(kurkureCount);
lcd.print(" S:");
lcd.print(spriteCount);

lcd.setCursor(0,2);
lcd.print("Total: Rs ");
lcd.print(total);

delay(2000);

lcd.clear();
lcd.print("Scan Item...");
}
