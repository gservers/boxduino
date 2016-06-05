/*  Arduino controlled box mod by Marek Ledworowski.
    If you want to use my code for your own box mod,
    please contact me first at fotelpl@gmail.com

    I'm not responsible for dead MCU's, burnt PC's,
    smoked cigarettes or electrocutions.
    This sketch is in early dev stage, so there is no
    warranty it won't drink all your milk and steal
    your car.

    TL;DR - You are responsible for this. Not me.
            Do not redistribute without my permission.
*/
#include <ClickButton.h>
//https://github.com/stevecooley/beatseqr-software/tree/master/arduino_code/beatseqr_arduino_firmware_4_experimental
#include <avr/sleep.h>
#include <avr/power.h>
#include "PCD8544/PCD8544.h"
static PCD8544 lcd;
bool on = true; //Check if powered
const int mosfet = 6; //PWM output to N-FET
ClickButton fire(2, HIGH); //Define fire button as object
const int pot = A0; //Potentiometer
const int volt = A1; // Vin voltmeter
const int ohmmet = A2; //Ohmmeter
const int bouncedelay = 50; //Short press
const int holddelay = 100; //Long press
const int baud = 9600; //Serial baudrate

const float r1 = 1000000.0; //R1 of Vin voltmeter
const float r2 = 100000.0; //R2 Vin voltmeter
const float wireres = 0.21; //Resistance of wires. For ohmmeter calibration.
const float vin = 8.4; //Max. supply voltage
const float vbord = 6; //Max. discharge voltage

int mode = 0; //0 - VV/VW, 1 - BYPASS
int cnt = 0; //How many times button was pressed
long int tempnow; //First click
long int tempend; //Last click

float state = 0; //% of potentiometer scale
float rms = 0;  //RMS voltage
float duty = 0; //duty cycle (0-255)
float vbat = 0; //Vin voltage
float cbat = 0; //Baterry %
float res = 0; //Resistance

static const byte full[] = {
  B11111000,
  B11111100,
  B11111100,
  B11111100,
  B11111000,
};
static const byte hi[] = {
  B11111000,
  B11110100,
  B11110100,
  B11110100,
  B11111000,
};
static const byte med[] = {
  B11111000,
  B11100100,
  B11100100,
  B11100100,
  B11111000,
};
static const byte low[] = {
  B11111000,
  B11000100,
  B11000100,
  B11000100,
  B11111000,
};
static const byte empty[] = {
  B11111000,
  B10001100,
  B10110100,
  B11000100,
  B11111000,
};
static const byte ohm[] = {
  B10110000,
  B11001000,
  B00001000,
  B11001000,
  B10110000,
};
void setup() {
  pinMode(pot, INPUT);
  pinMode(volt, INPUT);
  pinMode(2, INPUT);
  pinMode(mosfet, OUTPUT);
  pinMode(ohmmet, INPUT);
  //lcd.createChar(0, full);
  //lcd.createChar(1, hi);
  //lcd.createChar(2, med);
  //lcd.createChar(3, low);
  //lcd.createChar(4, empty);
  //lcd.createChar(5, ohm);
  lcd.begin(84, 48);
  lcd.createChar(0, ohm);
  lcd.createChar(1, full);
  lcd.createChar(2, hi);
  lcd.createChar(3, med);
  lcd.createChar(4, low);
  lcd.createChar(5, empty);
  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.write(1);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
  fire.debounceTime = 30;
  fire.multiclickTime = 300;
  fire.longClickTime = 500;
  on = true;
}
void loop() {
  duty = gainduty(pot); //This needs to be refreshed every time
  rms = gainrms(vbat, duty);
  res = gainres(ohmmet, wireres);
  vbat = gainvbat(volt, r1, r2);
  fire.Update();
  cnt = fire.clicks;
  switch (cnt) {
    case -1: if (mode == 0) watt(mosfet, duty, 2);
      if (mode == 1) bypass(mosfet, 2);
      delay(50);
      break;
    case 3: if (mode == 1) mode = 0;
      else if (mode == 0) mode = 1;
      break;
    case 5: on = false; power(); break;
    case 6: printstate(baud, res, vbat, duty, rms, mode); break;
  }
  cnt = 0;
}

void printstate(int tmpbaud, float restmp, float vbatmp, float dutmp, float rmstmp, int modtmp) {
  //Serial.begin(tmpbaud);
  Serial.begin(tmpbaud);
  Serial.println("BOXduino service mode");
  Serial.print("Resistance: ");
  Serial.print(restmp, 2);
  Serial.print("ohm\n");
  Serial.print("Supply voltage: ");
  Serial.print(vbatmp, 1);
  Serial.print("V\n");
  Serial.print("Mode: ");
  if(mode == 0) Serial.print("Regulated\n");
  if(mode == 1) Serial.print("Bypass\n");
  Serial.print("RMS voltage: ");
  Serial.print(rmstmp, 1);
  Serial.print("V\n");
  Serial.print("Digital duty cycle: ");
  Serial.print(dutmp, 0);
  Serial.print("/255\n");
  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
  Serial.end();
}
//Saving this function for later use
/*void updatebat(float tmpvbat) {
  float cbat = ((tmpvbat - vbord) * 100) / vin; //wypisanie na ekranie
  if (cbat < 0) cbat = 0;
  if (cbat == 100) lcd.setCursor(10, 1);
  if (cbat < 100 && cbat >= 10) lcd.setCursor(11, 1);
  if (cbat < 10) lcd.setCursor(12, 1);
  if (cbat <= 100 && cbat >= 95) lcd.write(byte(0));
  if (cbat < 95 && cbat >= 70) lcd.write(byte(1));
  if (cbat < 70 && cbat >= 40) lcd.write(byte(2));
  if (cbat < 40 && cbat >= 15) lcd.write(byte(3));
  if (cbat < 15) lcd.write(byte(4));
  }*/
//MOSFET port, duty cycle, fire button port
void watt(int mostmp, float dutmp, int fitmp) {
  float czas = millis();
  float licz = 0;
  analogWrite(mostmp, dutmp);
  while (digitalRead(fitmp) != 0 && licz < 10)
    licz = (millis() - czas) / 1000;
  analogWrite(mostmp, 0.0);
  delay(100);
  if (licz >= 10) {
    while (digitalRead(fitmp) == 1) {}
    delay(2500);
  }
}
//MOSFET port, fire button port
void bypass(int mostmp, int fitmp) {
  float czas = millis();
  float licz = 0;
  analogWrite(mostmp, 1023);
  while (digitalRead(fitmp) != 0 && licz < 10)
    licz = (millis() - czas) / 1000;
  analogWrite(mostmp, 0.0);
  delay(100);
  if (licz >= 10) {
    while (digitalRead(fitmp) == 1) {};
    delay(2500);
  }
}
//Vin voltmeter port, R1, R2
float gainvbat(int batmp, float rtmp1, float rtmp2) {
  float value = analogRead(batmp);
  float vout = (value * 5.0) / 1024.0;
  float vbatmp = vout / (rtmp2 / (rtmp1 + rtmp2));
  return (vbatmp);
}
//Potentiometer port
int gainduty(float potmp) {
  float dutmp = analogRead(potmp);
  dutmp /= 4;
  dutmp = round(dutmp);
  if (dutmp >= 256) dutmp = 255;
  return (dutmp);
}
//Vin voltage, duty cycle
float gainrms(float vbatmp, float dutmp) {
  float statmp = (dutmp * 100) / 255; //Stan w %
  float rmstmp = vbatmp * sqrt(statmp / 100); //RMS
  return (rmstmp);
}
//Ohmmeter port, resistance of connection wires
//Average of 5 measurements to avoid too big values
float gainres(int ohmtmp, float wirerestmp) {
  float tmp = 0, val, volts, resistance;
  for (int a = 0; a < 5; a++) {
    val = analogRead(ohmtmp);
    volts = (val * 5) / 1023.0;
    resistance = volts / 0.125; //Change 0.125 if needed
    resistance -= wirerestmp;
    tmp += resistance;
  }
  resistance = tmp / 5;
  if (resistance > 9.99) resistance = 9.99;
  if (resistance < 0) resistance = 0;
  return (resistance);
}
//Shutdown
void power() {
  attachInterrupt(2, poweron, HIGH);
  do {
    analogWrite(bcg, 0);
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode();
    long int teemp = millis();
    while (digitalRead(2) == HIGH) {
      if ((millis() - teemp) >= 2500) on = true;
    }
    if (on == true) analogWrite(bcg, 64);
  } while (on == false);
}
//Apparently this little guy works well
void poweron() {
  detachInterrupt(2);
}
