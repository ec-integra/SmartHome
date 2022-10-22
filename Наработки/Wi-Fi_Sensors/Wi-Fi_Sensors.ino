#include <TroykaDHT.h>
#include <TroykaMQ.h>

int analogMQ7 = A0;
#define SENSORS_PIN A4

int val;
int      ackData[4]; 
unsigned long TIME_SEND = 0;

DHT Sensors(SENSORS_PIN, DHT11);
//MQ7 mq7(PIN_MQ2);


void setup() {
  Sensors.begin();  
  Serial.begin(9600);
  //mq7.cycleHeat();

}

void loop() {
  Values();

  if (millis() - TIME_SEND > 500) {
      TIME_SEND = millis();
      for (int i=0;i<4;i++) {
        Serial.println(ackData[i]);
      }
    }
  
}

void Values() {
  Sensors.read();
  val=analogRead(analogMQ7);
  val = map(val, 0, 1000, 0, 100);
  if (val < 65) ackData[0]=0;
  else ackData[0]=1; 
  ackData[1]=Sensors.getTemperatureC();
  ackData[2]=Sensors.getHumidity();
  ackData[3]=val;
}
