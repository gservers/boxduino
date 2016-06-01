/*  Arduino controlled box mod by Marek Ledworowski
    Before committing changes please contact me first
    at fotelpl@gmail.com .
    I'm not responsible for dead MCU's, burnt lungs
    and all stuff that may be dangerous or sad.
    Version 1.0 - Getting all parts and stuff
*/
#include <avr/sleep.h>
#include <avr/power.h>
bool on = true; //Stan wlaczenia
const int fire = 2; //Przycisk
const int mosfet = 6; //Wyjscie (PWM)
const int pot = A0; //Potencjometr
const int volt = A1; //Woltomierz Vin
const int ohmmet = A2; //Omomierz

int mode = 0; //0 - VV/VW, 1 - BYPASS
int bouncedelay = 100;
int holddelay = 500;

long int tempnow; //Poczatek klikania
long int tempend; //Koniec klikania

const float r1 = 1000000.0; //R1 woltomierza Vin
const float r2 = 100000.0; //R2 woltomierza Vin

float state = 0; //Wychylenie w %
float rms = 0;  //Napięcie RMS
float duty = 0; //Dlugosc cyklu 0-255
float vbat = 0; //Napiecie zasilania
float vin = 8.4; //Napiecie referencyjne zasilania
float vbord = 6; //Granic rozladowania
float cbat = 0; //% naladowania baterii
float res = 0; //omomierz
float light = 256.0; //Jasnosc podswietlenia
byte full[8] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
};
byte hi[8] = {
  B00000,
  B01110,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
};
byte med[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B00000,
};
byte low[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B00000,
};
byte empty[8] = {
  B00000,
  B01110,
  B11001,
  B10101,
  B10101,
  B10011,
  B11111,
  B00000,
};
byte ohm[8] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B10001,
  B10001,
  B01010,
  B11011,
};
void setup() {
  pinMode(pot, INPUT);
  pinMode(volt, INPUT);
  pinMode(fire, INPUT);
  pinMode(mosfet, OUTPUT);
  pinMode(ohmmet, INPUT);
  Serial.begin(19200);
  //lcd.createChar(0, full);
  //lcd.createChar(1, hi);
  //lcd.createChar(2, med);
  //lcd.createChar(3, low);
  //lcd.createChar(4, empty);
  //lcd.createChar(5, ohm);
  vbat = gainvbat(volt, r1, r2);
  duty = gainduty(pot);
  rms = gainrms(vbat, duty);
}
void loop() {
  vbat = gainvbat(volt, r1, r2);
  duty = gainduty(pot);
  rms = gainrms(vbat, duty);
  res = gainres(ohmmet);
  res -= 0.21; //For breadboard only. After soldering will be changed.
  if(res > 9.99) res = 9.99;
  if(res < 0) res = 0;
  Serial.println(res);
  /*if (rms < 0.1 && digitalRead(fire) == HIGH ) {
    tempnow = millis();
    while (digitalRead(fire) == HIGH) {}
    tempend = millis();
    if ((tempend - tempnow) >= holddelay) {
      on = false;
      power();
    } else if ((tempend - tempnow) < bouncedelay) {
      if (mode == 1) mode = 0;
      else mode = 1;
    }
    } else if (digitalRead(fire) == HIGH && rms >= 0.1) {
    tempnow = millis();
    while (millis() - tempnow < bouncedelay && digitalRead(fire) == HIGH) {}
    if (mode == 0 && digitalRead(fire) == HIGH) watt(mosfet, duty, fire);
    if (mode == 1 && digitalRead(fire) == HIGH) bypass(mosfet, fire);
    }*/
  delay(10);
}

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

void watt(int mostmp, float dutmp, int fitmp) { //mosfet, duty, fire
  float czas = millis();
  float licz = 0;
  analogWrite(mostmp, dutmp);
  while (digitalRead(fitmp) != 0 && licz < 10)
    licz = (millis() - czas) / 1000;
  analogWrite(mostmp, 0.0);
  if (licz >= 10) {
    while (digitalRead(fitmp) == 1) {}
    delay(2500);
  }
}

void bypass(int mostmp, int fitmp) { //mosfet, fire
  float czas = millis();
  float licz = 0;
  analogWrite(mostmp, 1023);
  while (digitalRead(fitmp) != 0 && licz < 10)
    licz = (millis() - czas) / 1000;
  analogWrite(mostmp, 0.0);
  if (licz >= 10) {
    while (digitalRead(fitmp) == 1) {};
    delay(2500);
  }
}

float gainvbat(int batmp, float rtmp1, float rtmp2) {
  float value = analogRead(batmp);
  float vout = (value * 5.0) / 1024.0;
  float vbatmp = vout / (rtmp2 / (rtmp1 + rtmp2));
  return (vbatmp);
}

int gainduty(float potmp) {
  float dutmp = analogRead(potmp);
  dutmp /= 4;
  dutmp = round(dutmp);
  if (dutmp >= 256) dutmp = 255;
  return (dutmp);
}

float gainrms(float vbatmp, float dutmp) {
  float statmp = (dutmp * 100) / 255; //Stan w %
  float rmstmp = vbatmp * sqrt(statmp / 100); //RMS
  return (rmstmp);
}

float gainres(int ohmtmp) {
  int val = analogRead(ohmtmp);
  float volts = (val * 5) / 1023.0 ;
  float resistance = volts * 8; //Set for 10ohm Vadj resistor for LM317
  //float resistance = volts / 0.125; //Change 0.125 if needed
  return (resistance);
}

void power() {
  attachInterrupt(fire, poweron, HIGH);
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sleep_mode();
  while (on == false && digitalRead(fire) == LOW) {}
  if (on == false) poweron();
  sleep_disable();
}

void poweron() {
  detachInterrupt(fire);
  long int teemp = millis();
  while (digitalRead(fire) == HIGH) {}
  if ((millis() - teemp) >= 2000) on = true;
  power();
}
