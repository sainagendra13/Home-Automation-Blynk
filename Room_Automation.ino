#include "utils.h"
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEthernet.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

float temp = 0.0;
int lastAnalogValue = -1;

bool heaterOn = false;
bool coolerOn = false;

int tanklevel = 0;
bool fillOn = false;
bool useOn = false;

BLYNK_CONNECTED()
{
  heaterOn = false;
  coolerOn = false;
  fillOn = false;
  useOn = false;

  Blynk.virtualWrite(V1, 0);   // Temperature gauge
  Blynk.virtualWrite(V2, 0);   // Cooler button
  Blynk.virtualWrite(V3, 0);   // Heater button
  Blynk.virtualWrite(V4, 0);   // Volume gauge
  Blynk.virtualWrite(V5, 0);   // Use button
  Blynk.virtualWrite(V6, 0);   // Fill button

  lcd.setCursor(6, 0);
  lcd.print("           ");
   lcd.setCursor(6, 1);
  lcd.print("           ");
}

BLYNK_WRITE(V3) {
  int requestedState = param.asInt();
  // Logic: Only allow turning ON if temperature is below the limit
  if (requestedState == 1 && temp > 35.0) {
    Blynk.virtualWrite(V3, 0); // Force button back to OFF
    Blynk.virtualWrite(V7, "REJECTED: Temp already above 35 limit!\n");
    heaterOn = false;
  } else {
    heaterOn = requestedState;
    if (heaterOn) {
      coolerOn = false;
      Blynk.virtualWrite(V2, 0);
      lcd.setCursor(6, 0);
      lcd.print(" HE_TR_ON ");
    } else {
      lcd.setCursor(6, 0);
      lcd.print(" HE_TR_OFF");
    }
  }
}

BLYNK_WRITE(V2) {
  int requestedState = param.asInt();
  if (requestedState == 1 && temp < 10.0) {
    Blynk.virtualWrite(V2, 0); // Force button back to OFF in app
    Blynk.virtualWrite(V7, "REJECTED: Temp already below 10 limit!\n");
    coolerOn = false;
  } else {
    coolerOn = requestedState;
    if (coolerOn) {
      heaterOn = false;
      Blynk.virtualWrite(V3, 0); // Force Heater button OFF
      Blynk.virtualWrite(V7, "Cooler: ON && HEATER OFF\n");
      lcd.setCursor(6, 0);
      lcd.print(" CO_LR_ON ");
    } else {
      Blynk.virtualWrite(V7, "Cooler: OFF\n");
      lcd.setCursor(6, 0);
      lcd.print(" CO_LR_OFF"); 
    }
  }
}


void readAnalogOutput()
{
  while (Serial.available() > 0) Serial.read();

  // 2. REQUEST DATA: Send the 0x30 command
  Serial.write(0x30);
  
  // 3. WAIT WITH TIMEOUT: Don't read until exactly 2 bytes are ready
  unsigned long start = millis();
  while (Serial.available() < 2 && (millis() - start < 150));

  if (Serial.available() >= 2) {
    byte valorh = Serial.read(); 
    byte valorl = Serial.read();
    
    int newLevel = (valorh << 8) | valorl;

    if (newLevel >= 0 && newLevel <= 3000) {
       tanklevel = newLevel;
       
       // Update Blynk and LCD only if valid
       Blynk.virtualWrite(V8, tanklevel);
       lcd.setCursor(2, 1);
       lcd.print("    "); // Clear old digits
       lcd.setCursor(2, 1);
       lcd.print(tanklevel);
    }
  } 
}

void waterInlet(bool flag)
{
  Serial.write(0x00);
  Serial.write(flag ? 0x01 : 0x00); 
  if(flag) {
    lcd.setCursor(6, 1);
    lcd.print(" IN_FL_ON ");
    Blynk.virtualWrite(V7, flag ? "INLET: ON\n" : "INLET: OFF\n");
  }
  else {
    lcd.setCursor(6, 1);
    lcd.print(" IN_FL_OFF");
  }

}

void waterOutlet(bool flag)
{
  Serial.write(0x01);
  Serial.write(flag ? 0x01 : 0x00); 
  if(flag) {
    lcd.setCursor(6, 1);
    lcd.print(" OT_FL_ON ");
    Blynk.virtualWrite(V7, flag ? "OUTLET: ON\n" : "OUTLET: OFF\n");
  }
  else {
    lcd.setCursor(6, 1);
    lcd.print(" OT_FL_OFF");
  }
}

BLYNK_WRITE(V6) {
  fillOn = param.asInt();
  if (fillOn) {
    waterInlet(true);
  } else {
    waterInlet(false);
  }
}

BLYNK_WRITE(V5) {
  useOn = param.asInt();
  if (useOn) {
    waterOutlet(true);
  } else {
    waterOutlet(false);
  }
}


void tankControl()
{
  readAnalogOutput();

    if (tanklevel < 50 && fillOn == false) {
    fillOn = true;
    waterInlet(true);
    Blynk.virtualWrite(V6, 1); // Turn Inlet Switch ON in App
    Blynk.virtualWrite(V7, "CRITICAL LOW: AUTO-FILL START\n");
  }

  // SAFETY: AUTO-STOP FILLING
  if (tanklevel >= 3000 && fillOn == true) {
    fillOn = false;
    waterInlet(false);
    Blynk.virtualWrite(V6, 0); // Turn Inlet Switch OFF in App
    Blynk.virtualWrite(V7, "TANK FULL: AUTO-STOP\n");
  }

  // This will turn off the "Use" button ONLY if the tank hits 0
  if (tanklevel <= 0 && useOn == true) {
    useOn = false;
    waterOutlet(false);
    Blynk.virtualWrite(V5, 0); // Turn Use Switch OFF in App
    Blynk.virtualWrite(V7, "TANK EMPTY: OUTLET CLOSED\n");
  }
}

void checkSensor() {
  static float offset=0.0;
  static bool heaterLimitTriggered = false;
  static bool coolerLimitTriggered = false;

    int currentAnalog = analogRead(TEMP);

    float realtemp = ((currentAnalog * 5.0) * 100) / 1024.0;

 /*  Serial.print("Temperature: ");
    Serial.println(realtemp); */

  if (heaterOn && (realtemp + offset) < 35.1) {
    offset += 0.04;
  }

  if (coolerOn && (realtemp + offset) > 9.9) {
    offset -= 0.04;
  }
  temp = realtemp + offset;

  if (temp > 35 && !heaterLimitTriggered) {
    heaterOn = false;
    Blynk.virtualWrite(V3, 0);
    lcd.setCursor(6, 0);
    lcd.print(" HE_TR_OFF");
    Blynk.virtualWrite(V7, "Heater: OFF\n");
    Blynk.virtualWrite(V7, "Temp exceeded Max Limit 35 deg\n");
    heaterLimitTriggered = true;
  }

  if (temp <= 34.5) {
    heaterLimitTriggered = false;
  }

  if (temp < 10 && !coolerLimitTriggered) {
    coolerOn = false;
    Blynk.virtualWrite(V2, 0);
    lcd.setCursor(6, 0);
    lcd.print(" CO_LR_OFF");
    Blynk.virtualWrite(V7, "Cooler: OFF\n");
    Blynk.virtualWrite(V7, "Temp below Min Limit 10 deg\n");
    coolerLimitTriggered = true;
  }

  if (temp > 10.5) {
    coolerLimitTriggered = false;
  }

  digitalWrite(HEATER, heaterOn ? HIGH : LOW);
  digitalWrite(COOLER, coolerOn ? HIGH : LOW);

  lcd.setCursor(2, 0);
  lcd.print(temp, 2);

  Blynk.virtualWrite(V1, temp);
}

void setup() {

  lcd.init();
   Serial.begin(19200); 
  Blynk.begin(BLYNK_AUTH_TOKEN);

  pinMode(TEMP, INPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(COOLER, OUTPUT);

  timer.setInterval(1000L, checkSensor);
  timer.setInterval(1100L, tankControl);
  lcd.setCursor(0,0);
  lcd.print("T=");
  lcd.setCursor(0,1);
  lcd.print("V=");

}

void loop() {
  Blynk.run();
  timer.run();
}