/*  Arduino controlled box mod by Marek Ledworowski.
    I'm not responsible for dead MCU's, burnt PC's,
    smoked cigarettes or electrocutions.
    I'm not responsible for this sketch, even if it
    poops on your lawn - You let him do it.

    Please don't sell it as Yours, but feel free to use
    it WITHOUT financial benefits.
    Any questions? Contact me at fotelpl@gmail.com

    To do:
      - lock for too low/high resistance
      - lock for too high power
*/

#include "libraries/PCD8544/PCD8544.h" //LCD Library
#include "libraries/ClickButton/ClickButton.h" //Fire button driver
#include <avr/sleep.h> //Sleeping lib
#include <avr/power.h> //^up
static PCD8544 lcd;
bool manual = true; //true for LM317 circiut, false for manual setting
bool on = true; //Check if powered
bool inv = false; //Screen inverted
bool lock = false; //Autofire
const int baud = 9600; //Serial baudrate
const int maxwatt = 100; //Max watts

ClickButton fire(2, HIGH); //Define fire button as object
const int pfet = 9; //Trigger (+) channel of P-FET
const int ohmmetpower = 8; //Ohmmeter power supply
const int pot = A0; //Potentiometer
const int volt = A1; // Vin voltmeter
const int ohmmet = A2; //Ohmmeter output

const int probes = 15; //How many resistance samples will be taken
const float r1 = 1000000.0; //R1 of Vin voltmeter
const float r2 = 100000.0; //R2 Vin voltmeter
const float vin = 8.4; //Max. supply voltage
const float vbord = 6; //Max. discharge voltage
const float ohmcal = 0.08; //Resistance of connections, just put copper wire between pins.
//Check the readings and change value if needed

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
float minres = 0; //Minimal resistance
int watts = 0; //Watts

const int timeout = 10; //Fire limit
const int dim = 10; //Disable display limit

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
  lcd.begin();
  lcd.setContrast(65); //We want to see something, right?
  lcd.drawBitmap(splashscreen, 84, 48);
  setPwmFrequency(9, 1); //31372.55Hz, 256 cycle length (0-255)
  pinMode(pot, INPUT); //Potentiometer
  pinMode(volt, INPUT); //Voltmeter
  pinMode(2, INPUT); //Button
  pinMode(pfet, OUTPUT); //P-FET transistor out
  pinMode(ohmmetpower, OUTPUT); //Ohmmeter power supply
  pinMode(ohmmet, INPUT); //Ohmmeter
  prepchar(); //Send custom chars to display's RAM
  fire.debounceTime = 10; //Single click
  fire.multiclickTime = 400; //Time for many clicks
  fire.longClickTime = 200; //Long click timer
  on = true; //Set this flag to enable screen and stuff
  digitalWrite(pfet, HIGH); //Make sure it won't fire without us
  digitalWrite(ohmmetpower, LOW); //Disable ohmmeter
  vbat = gainvbat(volt, r1, r2); //Performing initial calculations
  minres = vbat * vbat / maxwatt;
  switch (mode) {
    case 0: //VV/VW
      duty = gainduty(pot); rms = gainrms(vbat, duty);
      watts = round(rms * rms / res); break;
    case 1: //Bypass
      duty = 255; rms = vbat; break;
  } //As far, we're finished
  delay(1500);
  lcd.clear();
  if (manual == false)
    res = gainres(ohmmetpower, ohmmet, probes); //LM317 is present so it checks resistance
  else res = setohm(pot); //No LM317 = no automatic updates
  lcd.setCursor(0, 0);
  printstate(vbat, duty, rms, mode, res);
  lastpress = millis();
}

void loop() {
  fire.Update();
  vbat = gainvbat(volt, r1, r2);
  switch (mode) {
    case 0:
      duty = gainduty(pot); rms = gainrms(vbat, duty);
      watts = round(rms * rms / res); break;
    case 1:
      duty = 255; rms = vbat;
      watts = round(rms * rms / res); break;
  }

  lcd.setCursor(30, 2);
  lcd.print(watts);
  lcd.print("W  ");
  lcd.setCursor(30, 3);
  lcd.print(rms, 1);
  lcd.print("V");
  lcd.setCursor(30, 4);
  if (res > 9.99) res = 9.99;
  if (res < 0.03) res = 0.00;
  lcd.print(res, 2);
  lcd.write(0);

  duty = abs(duty - 255); //P-FET transistor reacts to low state soooo...
  //The value must be inverted
  fire.Update();
  cnt = fire.clicks;
  if (cnt != 0) lastpress = millis();
  switch (cnt) {
    case -1:
      if (digitalRead(2) == 1 && lock == false && res > minres) {
        printstate(vbat, duty, rms, mode, res);
        go(pfet, duty, 2);
        printstate(vbat, duty, rms, mode, res);
      }
      if (digitalRead(2) == 1 && lock == false && res < minres) {
        lcd.setCursor(0, 3);
        lcd.print(" Too low ohm!");
        while (digitalRead(2) == HIGH) {}
        lcd.clearLine();
      }
      break;
    case 1:
      if (lock == true) {
        lcd.begin();
        lcd.setContrast(65);
      }
      else if (manual == false) res = gainres(ohmmetpower, ohmmet, probes);
      lock = false;
      prepchar();
      printstate(vbat, duty, rms, mode, res); break;
    case 2:
      lcd.clear();
      if (/*manual == true && */analogRead(pot) <= 5) res = setohm(pot);
      else {
        lcd.setCursor(0, 0);
        lcd.print("Min ohm: ");
        lcd.print(minres);
        lcd.write(0);
        lcd.setCursor(0, 1);
        lcd.print("Max Watts:");
        lcd.print(maxwatt);
        lcd.print("W");
        lcd.setCursor(0, 2);
        lcd.print("Puffs: ");
        lcd.print(puffs);
        lcd.setCursor(0, 3);
        lcd.print("PuffTime:");
        lcd.print(pufftime, 2);
        lcd.setCursor(0, 5);
        if (manual == true)lcd.print("    Manual");
        else lcd.print("  Automatic");
        delay(2000);
      }
      lcd.clear();
      printstate(vbat, duty, rms, mode, res); break;
    case 3:
      if (mode == 1) {
        mode = 0;
        duty = gainduty(pot);
        rms = gainrms(vbat, duty);
        watts = round(rms * rms / res);
      } else {
        mode = 1;
        duty = 255;
        rms = vbat;
      }
      printstate(vbat, duty, rms, mode, res); break;
    case 4:
      if (inv == 1) inv = 0; else inv = 1;
      lcd.setInverse(inv); break;
    case 5:
      on = false; power();
      printstate(vbat, duty, rms, mode, res); break;
    case 6:
      serv(baud, vbat, duty, rms, mode, puffs, pufftime, res); break;
  }
  if (cnt != 0) lastpress = millis();
  if ((millis() - lastpress) >= (dim * 1000) && cnt == 0 && lock == false) {
    lcd.stop();
    lock = true;
  }
}
//MOSFET port, duty cycle, fire button port
void go(int pftmp, float dutmp, int fitmp) {
  float czas = millis();
  float licz = 0;
  lcd.setCursor(0, 4);
  lcd.clearLine();
  analogWrite(pfet, dutmp);
  while (digitalRead(fitmp) != 0 && licz <= timeout) { //It's not rocket science
    licz = (millis() - czas) / 1000;
    lcd.setCursor(30, 4);
    lcd.print(licz, 2);
  }
  digitalWrite(pfet, HIGH);
  delay(5);
  if (licz >= timeout) {
    lcd.setCursor(33, 4);
    delay(45);
    lcd.print("10.00");
    lcd.clearLine();
    lcd.print("  Time's up!  ");
    while (digitalRead(fitmp) == 1) {}
    delay(2500);
    lcd.clearLine();
    licz = timeout;
  }
  pufftime += licz;
  lcd.setCursor(0, 4);
  lcd.clearLine();
  puffs = puffs + 1;
}
//Potentiometer port
int gainduty(float potmp) {
  float dutmp = analogRead(potmp);
  Serial.print(duty);
  Serial.print(" | ");
  dutmp = map(dutmp, 0, 1023, 0, 255);
  Serial.println(duty);
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
  lcd.setCursor(0, 0);
  lcd.print("Set Resistance");
  lcd.setCursor(0, 2);
  lcd.print("Min ohm: ");
  lcd.print(minres);
  lcd.write(0);
  lcd.setCursor(0, 3);
  lcd.print("Max Watts:");
  lcd.print(maxwatt);
  lcd.print("W");
  lcd.setCursor(0, 4);
  lcd.print("Puffs: ");
  lcd.println(puffs);
  lcd.setCursor(0, 5);
  lcd.print("PuffTime:");
  lcd.println(pufftime, 1);
  while (digitalRead(2) != true) {
    lcd.setCursor(0, 0);
    lcd.print("Set Resistance");
    tempohm = analogRead(potmp);
    tempohm /= 1023.0; //9.99 ohm max
    tempohm *= 999;
    tempohm = ceil(tempohm);
    tempohm /= 100;
    lcd.setCursor(30, 1);
    lcd.print(tempohm);
    lcd.write(0);
  }
  lcd.clear();
  return (tempohm);
}
//Ohmmeter power supply pin, ohmmeter reading pin, how many probes
//Remember to DISABLE P-FET transistor and/or N-FET!
float gainres(int ohmsuptmp, int ohmmetmp, int probetmp) {
  float tempres = 0;
  float tp = 0;
  digitalWrite(ohmsuptmp, HIGH);
  delayMicroseconds(10);
  for (int a = 0; a < probes; a++) {
    tp = analogRead(ohmmetmp);
    tp = tp * 5 / 1023;
    tp = tp / 0.125;
    tempres += tp;
  }
  digitalWrite(ohmsuptmp, LOW);
  tempres /= probetmp;
  tempres -= ohmcal;
  return (tempres);
}
//Shutdown
void power() {
  digitalWrite(pfet, HIGH);
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
    if (on == true) lcd.setPower(true); //Do something here to show it's working again
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
  Serial.println("ohm");
  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
  Serial.end();
}
void printstate(float vbatmp, float dutmp, float rmstmp, int modtmp, float restmp) {
  cbat = ((vbatmp - vbord) * 100);
  if (cbat < 0) cbat = 0;
  lcd.setCursor(0, 0);
  lcd.print("   BOXduino");
  lcd.setCursor(0, 1);
  if (modtmp == 0) lcd.print("Regulated mode");
  if (modtmp == 1) lcd.print(" Bypass mode! ");
  lcd.setCursor(0, 2);
  lcd.print(cbat, 0);
  lcd.print("% ");
  lcd.setCursor(30, 2);
  int watts = round(rms * rms / res);
  lcd.print(watts);
  lcd.print("W   ");
  lcd.setCursor(79, 2);
  if (cbat <= 100 && cbat >= 95) lcd.write(1);
  if (cbat < 95 && cbat >= 70) lcd.write(2);
  if (cbat < 70 && cbat >= 40) lcd.write(3);
  if (cbat < 40 && cbat >= 15) lcd.write(4);
  if (cbat < 15) lcd.write(5);
  lcd.setCursor(30, 3);
  lcd.print(rmstmp, 1);
  lcd.print("V");
  lcd.setCursor(30, 4);
  if (restmp > 9.99) restmp = 9.99;
  if (restmp < 0.03) restmp = 0.00;
  lcd.print(restmp, 2);
  lcd.write(0);
}
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
