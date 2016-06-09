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
#include "PCD8544/PCD8544.h"
#include "ClickButton/ClickButton.h"
//https://github.com/stevecooley/beatseqr-software/tree/master/arduino_code/beatseqr_arduino_firmware_4_experimental
#include <avr/sleep.h>
#include <avr/power.h>
static PCD8544 lcd;
bool on = true; //Check if powered
bool inv = false; //Screen inverted
bool puvol = false; //Decide if you want to see volts or puffs
//False == puffs / time
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
const float wireres = 0.5; //Resistance of wires. For ohmmeter calibration.
const float vin = 8.4; //Max. supply voltage
const float vbord = 6; //Max. discharge voltage

int mode = 0; //0 - VV/VW, 1 - BYPASS
int cnt = 0; //How many times button was pressed
long int tempnow; //First click
long int tempend; //Last click
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
const unsigned char ohm[] = {
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
  pinMode(pot, INPUT);
  pinMode(volt, INPUT);
  pinMode(2, INPUT);
  pinMode(mosfet, OUTPUT);
  pinMode(ohmmet, INPUT);
  lcd.begin();
  lcd.createChar(0, ohm);
  lcd.createChar(1, full);
  lcd.createChar(2, hi);
  lcd.createChar(3, med);
  lcd.createChar(4, low);
  lcd.createChar(5, empty);
  fire.debounceTime = 10;
  fire.multiclickTime = 400;
  fire.longClickTime = 200;
  on = true;
  lcd.clear();
  lcd.drawBitmap(splashscreen, 84, 48);
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  res = gainres(ohmmet, wireres);
  vbat = gainvbat(volt, r1, r2);
  if (mode == 0) {
    duty = gainduty(pot);
    rms = gainrms(vbat, duty);
  } else if (mode == 1) {
    duty = 255;
    rms = vbat;
  }
  printstate(res, vbat, duty, rms, mode, puffs, pufftime);
}
void loop() {
  //fire.Update();
  res = gainres(ohmmet, wireres);
  vbat = gainvbat(volt, r1, r2);
  if (mode == 0) {
    duty = gainduty(pot);
    rms = gainrms(vbat, duty);
  } else if (mode == 1) {
    duty = 255;
    rms = vbat;
  }

  fire.Update();
  cnt = fire.clicks;
  if (cnt != 0) {
    if (cnt == 6) serv(baud, res, vbat, duty, rms, mode, puffs, pufftime);
    if (cnt == 5) {
      on = false;
      power();
    }
    if (cnt == 4) {
      switch (inv) {
        case 1: inv = 0; break;
        case 0: inv = 1; break;
      }
      lcd.setInverse(inv);
    }
    if (cnt == 3) {
      switch (mode) {
        case 1: mode = 0; break;
        case 0: mode = 1; break;
      }
    }
    if (cnt == -1 && fire.depressed == 1) go(mosfet, duty, 2);
    cnt = 0;
  }
  printstate(res, vbat, duty, rms, mode, puffs, pufftime);
}

void printstate(float restmp, float vbatmp, float dutmp, float rmstmp, int modtmp, int pufftmp, float pufftmp2) {
  cbat = ((vbatmp - vbord) * 100);
  if (cbat < 0) cbat = 0;
  lcd.setCursor(0, 0);
  lcd.println("   BOXduino");
  lcd.setCursor(0, 1);
  if (modtmp == 0) lcd.print("Regulated mode");
  if (modtmp == 1) lcd.print(" Bypass mode! ");
  lcd.setCursor(0, 2);
  lcd.print("Watts: ");
  float watts = 0;
  watts = (rmstmp * rmstmp) / restmp; //P=U^2/R
  lcd.print(watts, 0);
  lcd.print("W  ");
  lcd.setCursor(0, 3);
  lcd.print("Coil: ");
  lcd.print(restmp, 2);
  lcd.write(0);
  lcd.setCursor(0, 4);
  lcd.print("Voltage: ");
  lcd.print(rmstmp, 2);
  lcd.print("V");
  lcd.setCursor(0, 5);
  lcd.print(pufftmp);
  lcd.print("/");
  lcd.print(pufftmp2, 2);
  lcd.print(" ");
  lcd.print(cbat, 0);
  lcd.print("%");
  lcd.setCursor(79, 5);
  if (cbat <= 100 && cbat >= 95) lcd.write(1);
  if (cbat < 95 && cbat >= 70) lcd.write(2);
  if (cbat < 70 && cbat >= 40) lcd.write(3);
  if (cbat < 40 && cbat >= 15) lcd.write(4);
  if (cbat < 15) lcd.write(5);

}
void serv(int tmpbaud, float restmp, float vbatmp, float dutmp, float rmstmp, int modtmp, int pufftmp, float pufftmp2) {
  Serial.begin(tmpbaud);
  Serial.println("BOXduino service mode");
  Serial.print("Resistance: ");
  Serial.print(restmp, 2);
  Serial.print("ohm\n");
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
  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
  Serial.end();
}

//MOSFET port, duty cycle, fire button port
void go(int mostmp, float dutmp, int fitmp) {
  float czas = millis();
  float licz = 0;
  analogWrite(mostmp, dutmp);
  lcd.setCursor(0, 4);
  lcd.clearLine();
  while (digitalRead(fitmp) != 0 && licz < 9.97) { //It's not rocket science
    licz = (millis() - czas) / 1000;
    lcd.setCursor(33, 4);
    lcd.print(licz, 2);
  }
  analogWrite(mostmp, 0);
  pufftime += licz;
  delay(200);
  if (licz >= 9.97) {
    lcd.setCursor(33, 4);
    lcd.print("10.00");
    lcd.clearLine();
    lcd.print("  Time's up!  ");
    while (digitalRead(fitmp) == 1) {}
    delay(2500);
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
//Ohmmeter port, resistance of connection wires
//Average of 5 measurements to avoid too big values
float gainres(int ohmtmp, float wirerestmp) {
  float tmp = 0, val, volts, resistance;
  for (int a = 0; a < 5; a++) {
    tmp += ((analogRead(ohmtmp) * 5) / 1023.0) / 0.125; //Change 0.125 if needed (1.25V/R1)
  }
  resistance = tmp / 5;
  //resistance -= wirerestmp;
  if (resistance > 9.99) resistance = 9.99;
  if (resistance < 0) resistance = 0.00;
  return (resistance);
}
//Shutdown
void power() {
  attachInterrupt(2, poweron, HIGH);
  do {
    lcd.stop();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode();
    long int teemp = millis();
    while (digitalRead(2) == HIGH) {
      if ((millis() - teemp) >= 2500) on = true;
    }
    if (on == true) lcd.setPower(true);
  } while (on == false);
  detachInterrupt(2);
}
void poweron() {
  detachInterrupt(2);
}
