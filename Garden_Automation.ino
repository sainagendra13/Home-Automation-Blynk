#define BLYNK_TEMPLATE_ID "TMPL3PVq_BQDS"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "Your_Auth_Token_Here"
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

#define LDR A0
#define LED 4
/*************************************************************************************
By varying Slider which measures LDR resistance in Kilo-ohms. Above 54k-ohm value in slider,
LED will glow.

*************************************************************************************/
char auth[]=BLYNK_AUTH_TOKEN;
BlynkTimer timer;
void setup() {
  Serial.begin(9600);
  pinMode(LDR, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Blynk.begin(auth);
  timer.setInterval(5000L, checkSensor);
}

void checkSensor()
{
  int var = analogRead(LDR);
  delay(500);
  int threshold = 400;
  Serial.println(" ");
  Serial.print("LDR Value: ");
  Serial.println(var);
  if (var < threshold)
    {
      digitalWrite(LED, HIGH);
      Blynk.virtualWrite(V0, "ON");
    }
  else
  {
    digitalWrite(LED, LOW);
    Blynk.virtualWrite(V0, "OFF");
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
