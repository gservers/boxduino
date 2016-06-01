/*  Arduino controlled box mod by Marek Ledworowski.
    If you want to use my code for your own box mod,
    please contact me first at fotelpl@gmail.com

    I'm not responsible for dead MCU's, burnt PC's,
    smoked cigarettes or electrocutions.
    This sketch is in early dev stage, so there is no
    warranty it won't drink all your milk and steal
    your car.

    TL;DR - You are responsible for your mistakes. Not me.
*/
#include <avr/sleep.h>
#include <avr/power.h>
bool on = true; //Check if powered
const int fire = 2; //Fire button
const int mosfet = 6; //PWM output to N-FET
const int bcg = 3; //PCD8544 backlight
const int pot = A0; //Potentiometer
const int volt = A1; // Vin voltmeter
const int ohmmet = A2; //Ohmmeter
const int bouncedelay = 50; //Short press
const int holddelay = 100; //Long press

const float r1 = 1000000.0; //R1 of Vin voltmeter
const float r2 = 100000.0; //R2 Vin voltmeter
const float wireres = 0.21; //Resistance of wires. Breadboard only.
const float vin = 8.4; //Max. supply voltage
const float vbord = 6; //Max. discharge voltage

int mode = 0; //0 - VV/VW, 1 - BYPASS
long int tempnow; //First click
long int tempend; //Last click

float state = 0; //% of potentiometer scale
float rms = 0;  //RMS voltage
float duty = 0; //duty cycle (0-255)
float vbat = 0; //Vin voltage
float cbat = 0; //Baterry %
float res = 0; //Resistance

byte full[8] = { //Saving them, may be useful later
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
  Serial.begin(9600); //Just for debug
  pinMode(pot, INPUT);
  pinMode(volt, INPUT);
  pinMode(fire, INPUT);
  pinMode(mosfet, OUTPUT);
  pinMode(ohmmet, INPUT);
  pinMode(bcg, OUTPUT);
  //lcd.createChar(0, full);
  //lcd.createChar(1, hi);
  //lcd.createChar(2, med);
  //lcd.createChar(3, low);
  //lcd.createChar(4, empty);
  //lcd.createChar(5, ohm);
  analogWrite(bcg, 48); //Not nescessary
  res = gainres(ohmmet, wireres); //This data dosen't need to be
  vbat = gainvbat(volt, r1, r2);  //refreshed very frequently
}
void loop() {
  duty = gainduty(pot); //This needs to be refreshed every time
  rms = gainrms(vbat, duty);

  if (rms < 0.1 && digitalRead(fire) == HIGH ) { //f***** up s***
    tempnow = millis();
    while (digitalRead(fire) == HIGH) {}
    tempend = millis();
    if ((tempend - tempnow) >= holddelay) {
      on = false;
      power();
      res = gainres(ohmmet, wireres);
      vbat = gainvbat(volt, r1, r2);
    } else if ((tempend - tempnow) <= bouncedelay) {
      if (mode == 1) mode = 0;
      else mode = 1;
    }
  } else if (digitalRead(fire) == HIGH && rms >= 0.1) {
    tempnow = millis();
    while (millis() - tempnow >= holddelay && digitalRead(fire) == HIGH) {}
    if (mode == 0 && digitalRead(fire) == HIGH) watt(mosfet, duty, fire);
    if (mode == 1 && digitalRead(fire) == HIGH) bypass(mosfet, fire);
  }
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
    if (resistance > 9.99) resistance = 9.99;
    if (resistance < 0) resistance = 0;
    tmp += resistance;
  }
  resistance = tmp / 5;
  return (resistance);
}
//Shutdown
void power() {
  attachInterrupt(fire, poweron, HIGH);
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sleep_mode();
  while (on == false && digitalRead(fire) == LOW) {}
  if (on == false) poweron();
  sleep_disable();
}
//Apparently this little guy works well
void poweron() {
  detachInterrupt(fire);
  long int teemp = millis();
  while (digitalRead(fire) == HIGH) {}
  if ((millis() - teemp) >= 2000) on = true;
  power();
}
