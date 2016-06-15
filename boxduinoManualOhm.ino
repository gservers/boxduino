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
    You need to set resistance manually.
    Turn potentiometer to low position and short press
    fire button. Select resistance with potentiometer
    and press button again.
    Only in regulated mode!
*/
#include "PCD8544/PCD8544.h"
#include "ClickButton/ClickButton.h"
//https://github.com/stevecooley/beatseqr-software/tree/master/arduino_code/beatseqr_arduino_firmware_4_experimental
#include <avr/sleep.h>
#include <avr/power.h>
static PCD8544 lcd;
bool on = true; //Check if powered
bool inv = false; //Screen inverted
bool lock = false; //Autofire
const int nfet = 6; //PWM output to N-FET
const int pfet = 9; //Trigger (+) channel of P-FET
ClickButton fire(2, HIGH); //Define fire button as object
const int pot = A0; //Potentiometer
const int volt = A1; // Vin voltmeter
const int bouncedelay = 50; //Short press
const int holddelay = 100; //Long press
const int timeout = 10; //Fire limit
const int dim = 10; //Disable display limit
const int baud = 9600; //Serial baudrate

const float r1 = 1000000.0; //R1 of Vin voltmeter
const float r2 = 100000.0; //R2 Vin voltmeter
const float vin = 8.4; //Max. supply voltage
const float vbord = 6; //Max. discharge voltage

int mode = 0; //0 - VV/VW, 1 - BYPASS
int cnt = 0; //How many times button was pressed
long int tempnow; //First click
long int tempend; //Last click
long int lastpress = 0; //Last button press
unsigned int puffs = 0; //Total puffs
float pufftime = 0; //Total puff time

float state = 0; //% of potentiometer scale
float rms = 0;  //RMS voltage
float duty = 0; //duty cycle (0-255)
float vbat = 0; //Vin voltage
float cbat = 0; //Baterry %
float res = 0; //Resistance

const unsigned char full[] = {
  0xFE, 0xFF, 0xFF, 0xFF, 0xFE
};
const unsigned char hi[] = {
  0xFE, 0xFD, 0xFD, 0xFD, 0xFE
};
const unsigned char med[] = {
  0xFE, 0xF1, 0xF1, 0xF1, 0xFE
};
const unsigned char low[] = {
  0xFE, 0xC1, 0xC1, 0xC1, 0xFE
};
const unsigned char empty[] = {
  0xFE, 0x87, 0x99, 0xE1, 0xFE
};
const unsigned char ohm[] = { //saving for later use
  0x58, 0x64, 0x04, 0x64, 0x58
};
const unsigned char splashscreen[] = {
  0x00, 0x00, 0x00, 0xFE, 0x0C, 0x30, 0x60, 0x30, 0x0C, 0xFE, 0x00, 0x80, 0x40, 0x40, 0x40, 0xC0, 0x00, 0x00, 0xC0, 0x80, 0x40, 0x40, 0x00, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x80, 0x40, 0x40, 0x40, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x02, 0x80, 0x40, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x40, 0x78, 0x7C, 0x78, 0x40, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x03, 0xF4, 0x14, 0x14, 0x13, 0xE4, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x03, 0x05, 0x05, 0x05, 0x00, 0x00, 0x03, 0x04, 0x04, 0x04, 0x00, 0x03, 0xF4, 0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0x84, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x0F, 0x05, 0x0F, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x21, 0x21, 0x21, 0x22, 0x1C, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x3F, 0x00, 0x1E, 0x20, 0x20, 0x20, 0x1E, 0x20, 0x00, 0x3E, 0x00, 0x3E, 0x04, 0x02, 0x02, 0x3C, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
void setup() {
  Serial.begin(9600);
  //TCCR0B = TCCR0B & 0b11111000 | 0x01; //Set 61kHz PWM whick is 64 times faster than normal
  pinMode(pot, INPUT);
  pinMode(volt, INPUT);
  pinMode(2, INPUT);
  pinMode(nfet, OUTPUT);
  pinMode(pfet, OUTPUT);
  digitalWrite(pfet, HIGH);
  lcd.begin();
  prepchar();
  fire.debounceTime = 10;
  fire.multiclickTime = 400;
  fire.longClickTime = 200;
  on = true;
  lcd.clear();
  lcd.drawBitmap(splashscreen, 84, 48);
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  vbat = gainvbat(volt, r1, r2);
  switch (mode) {
    case 0:
      duty = gainduty(pot); rms = gainrms(vbat, duty); break;
    case 1:
      duty = 255; rms = vbat; break;
  }
  printstate(vbat, duty, rms, mode, puffs, pufftime, res);
  lastpress = millis();
}

void loop() {
  fire.Update();
  vbat = gainvbat(volt, r1, r2);
  switch (mode) {
    case 0:
      duty = gainduty(pot); rms = gainrms(vbat, duty); break;
    case 1:
      duty = 255; rms = vbat; break;
  }
  fire.Update();
  cnt = fire.clicks;
  if (cnt != 0) lastpress = millis();
  switch (cnt) {
    //default: printstate(vbat, duty, rms, mode, puffs, pufftime, res); break;
    case -1:
      if (digitalRead(2) == 1 && lock == false) {
        printstate(vbat, duty, rms, mode, puffs, pufftime, res);
        go(pfet, nfet, duty, 2);
        printstate(vbat, duty, rms, mode, puffs, pufftime, res);
      } break;
    case 1:
      if (lock == true) lcd.begin();
      lock = false;
      prepchar();
      if (lock == false && rms <= 0.2) {
        lcd.clear();
        lcd.print(" Ohm Set Mode");
        res = setohm(pot);
        delay(100);
        lcd.clear();
      }
      printstate(vbat, duty, rms, mode, puffs, pufftime, res); break;
    case 3:
      if (mode == 1) {
        mode = 0;
        duty = gainduty(pot);
        rms = gainrms(vbat, duty);
      } else {
        mode = 1;
        duty = 255;
        rms = vbat;
      }
      printstate(vbat, duty, rms, mode, puffs, pufftime, res); break;
    case 4:
      if (inv == 1) inv = 0; else inv = 1;
      lcd.setInverse(inv); break;
    case 5:
      on = false; power();
      printstate(vbat, duty, rms, mode, puffs, pufftime, res); break;
    case 6:
      serv(baud, vbat, duty, rms, mode, puffs, pufftime, res); break;
  }
  if (cnt != 0) lastpress = millis();
  if ((millis() - lastpress) >= (dim * 1000) && cnt == 0 && lock == false) {
    lcd.stop();
    lock = true;
  }
}

void printstate(float vbatmp, float dutmp, float rmstmp, int modtmp, int pufftmp, float pufftmp2, float restmp) {
  cbat = ((vbatmp - vbord) * 100);
  if (cbat < 0) cbat = 0;
  lcd.setCursor(0, 0);
  lcd.println("   BOXduino");
  lcd.setCursor(0, 1);
  if (modtmp == 0) lcd.print("Regulated mode");
  if (modtmp == 1) lcd.print(" Bypass mode! ");
  lcd.setCursor(0, 2);
  lcd.print("Voltage: ");
  lcd.print(rmstmp, 2);
  lcd.print("V");
  lcd.setCursor(0, 3);
  lcd.print("Puffs: ");
  lcd.println(pufftmp);
  lcd.setCursor(0, 4);
  lcd.print("PuffTime:");
  lcd.println(pufftmp2, 2);
  lcd.setCursor(0, 5);
  //lcd.print(cbat, 0);
  lcd.print("100% ");
  lcd.setCursor(30,5);
  lcd.print(restmp);
  lcd.write(0);
  lcd.setCursor(79, 5);
  if (cbat <= 100 && cbat >= 95) lcd.write(1);
  if (cbat < 95 && cbat >= 70) lcd.write(2);
  if (cbat < 70 && cbat >= 40) lcd.write(3);
  if (cbat < 40 && cbat >= 15) lcd.write(4);
  if (cbat < 15) lcd.write(5);

}
//MOSFET port, duty cycle, fire button port
void go(int pftmp, int nftmp, float dutmp, int fitmp) {
  float czas = millis();
  float licz = 0;
  analogWrite(nftmp, dutmp);
  lcd.setCursor(0, 4);
  lcd.clearLine();
  digitalWrite(pfet, LOW);
  while (digitalRead(fitmp) != 0 && licz < (timeout + 0.01)) { //It's not rocket science
    licz = (millis() - czas) / 1000;
    lcd.setCursor(33, 4);
    lcd.print(licz, 2);
  }
  analogWrite(nftmp, 0);
  digitalWrite(pfet, HIGH);
  pufftime += licz;
  delay(50);
  if (licz >= 10.00) {
    lcd.setCursor(33, 4);
    lcd.print("10.00");
    lcd.clearLine();
    lcd.print("  Time's up!  ");
    while (digitalRead(fitmp) == 1) {}
    delay(2500);
    lcd.clearLine();
  }
  lcd.clearLine();
  puffs = puffs + 1;
}
//Potentiometer port
int gainduty(float potmp) {
  float dutmp = round((analogRead(potmp)) / 4);
  if (dutmp >= 256) dutmp = 255;
  return (dutmp);
}
//Vin voltmeter port, R1, R2
float gainvbat(int batmp, float rtmp1, float rtmp2) {
  float vbatmp = ((analogRead(batmp) * 5.0) / 1024.0) / (rtmp2 / (rtmp1 + rtmp2));
  return (vbatmp);
}

//Vin voltage, duty cycle
float gainrms(float vbatmp, float dutmp) {
  float statmp = (dutmp * 100) / 255; //Stan w %
  float rmstmp = vbatmp * sqrt(statmp / 100); //RMS
  return (rmstmp);
}

float setohm(int potmp) { //If you know resistance of coil
  float tempohm;
  while (digitalRead(2) != true) {
    tempohm = analogRead(potmp);
    tempohm /= 1023.0; //3 ohm max
    tempohm *= 3;
    lcd.setCursor(30, 2);
    lcd.print(tempohm);
  }
  lcd.clear();
  return(tempohm);
}

//Shutdown
void power() {
  digitalWrite(pfet, HIGH);
  digitalWrite(nfet, LOW);
  do {
    attachInterrupt(2, poweron, HIGH);
    lcd.stop();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode();
    sleep_disable();
    long int teemp = millis();
    while (digitalRead(2) == HIGH) {
      if ((millis() - teemp) >= 2500) on = true;
    }
    detachInterrupt(2);
    if (on == true) lcd.setPower(true);
  } while (on == false);
}
void poweron() {
  detachInterrupt(2);
}
void prepchar() {
  lcd.createChar(0, ohm);
  lcd.createChar(1, full);
  lcd.createChar(2, hi);
  lcd.createChar(3, med);
  lcd.createChar(4, low);
  lcd.createChar(5, empty);
}
void serv(int tmpbaud, float vbatmp, float dutmp, float rmstmp, int modtmp, int pufftmp, float pufftmp2, float restmp) {
  Serial.begin(tmpbaud);
  Serial.println("BOXduino service mode");
  Serial.print("Supply voltage: ");
  Serial.print(vbatmp, 1);
  Serial.print("V\n");
  Serial.print("Mode: ");
  if (mode == 0) Serial.print("Regulated\n");
  if (mode == 1) Serial.print("Bypass\n");
  Serial.print("RMS voltage: ");
  Serial.print(rmstmp, 1);
  Serial.print("V\n");
  Serial.print("Digital duty cycle: ");
  Serial.print(dutmp, 0);
  Serial.print("/255\n");
  Serial.print("Puffs: ");
  Serial.println(pufftmp);
  Serial.print("Puff time: ");
  Serial.print(pufftmp2, 2);
  Serial.println("s");
  Serial.print("Resistance: ");
  Serial.print(restmp);
  Serial.println("ohm\n");
  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
  Serial.end();
}