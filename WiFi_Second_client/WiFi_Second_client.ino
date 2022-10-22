#include <SoftwareSerial.h>
#include <TroykaDHT.h>
#include <TroykaMQ.h>

//int DATA[4] = {1,2,3,4};
unsigned long TIMER_DATA = 0;
String PARCE_DATA = "";
String Data_Send = "";

bool ModOk = false;
bool Searching_AP = false;
bool FOUND_AP = false;
bool CONNECT_to_AP = false;
bool CONNECT_on_TCP = false;
bool CONNECT_Full = false;
unsigned long time_wait = 0;

byte analogMQ7 = A0;
byte SENSORS_PIN = 2;
DHT Sensors(SENSORS_PIN, DHT11);

SoftwareSerial WIFI_Client(8, 9);

String IP_ADDR_AP = "";
String SSID_AP = "Noka_AP";

void setup()
{
  //Sensors.begin(); 
  Serial.begin(9600);
  WIFI_Client.begin(9600);
  Serial.println("COMFIG");
  WIFI_Client.read();
  while (!WIFI_Client) {};
    if (WIFI_Client){
    config_Client ();
    delay(500);
  }
  
}


void loop(){
  if (!ModOk) {
    while (WIFI_Client.available()>0) {
    WIFI_Client.read(); }
    config_Client ();
    delay(500);
  }
  if (!CONNECT_Full) {
    if (Searching_AP)Search_AP (); 
    if (FOUND_AP && !CONNECT_to_AP) Connect_to_AP ();
    if (CONNECT_to_AP && !CONNECT_on_TCP) Open_TCP_on_AP ();
    if (CONNECT_on_TCP) Check_status();
  }

  else {
    Values();
    if (WIFI_Client.available()) {
    Serial.write(WIFI_Client.read());
  }
    if (millis() - TIMER_DATA > 1000) {
    TIMER_DATA = millis();
    Send_Data();
    //Serial.println(TIMER_DATA);
    }
  }


    
  /*if (Serial.available()) {
    char com = Serial.read();
    switch (com) {
      case 'A': WIFI_Client.write("AT+CIPSEND=4\r\n");break;
      case 'S':WIFI_Client.write("noka\r\n");break; 
      //case 'W':Parser_WIFI_Serial_com("AT+CIPSTART=?\r\n");break; 
    }
  }*/
}




bool Parser_WIFI_Serial_com (String COM) {
  time_wait = millis();
  PARCE_DATA = "";
  WIFI_Client.print(COM);
  while(true && (millis() - time_wait < 15000)) {
    if (WIFI_Client.available()) {
      char c = WIFI_Client.read();
      Serial.write(c); 
      if (PARCE_DATA.endsWith("OK")) {
        //Serial.println(PARCE_DATA);
        return true;
      }
      else if (PARCE_DATA.endsWith("ERROR") || PARCE_DATA.endsWith("FAIL")) {
        //Serial.println(PARCE_DATA);
        return false;
      }
      else {
        switch (c) {
          case '\n':break;
          case '\r':break;
          default: PARCE_DATA+=c;break;
      }
      }
    }
  }
  Serial.println("ERROR WIFI");
}

void Search_AP (){
  if (Parser_WIFI_Serial_com ("AT+CWLAP=\""+SSID_AP+"\"\r\n")) {
    if (PARCE_DATA.startsWith("AT+CWLAP")) {
      if (PARCE_DATA.length() > 29) {
      Serial.println("FIND AP");
      Searching_AP = false;
      FOUND_AP = true;
      }
      else {Searching_AP = true; Serial.println("Search AP");}
    }
  }
}

void config_Client () {
  if(Parser_WIFI_Serial_com("AT\r\n")) {
      Serial.println("AT-OK");
      if(Parser_WIFI_Serial_com("AT+CWMODE_CUR=1\r\n")){
        Serial.println("MOD-OK");
        ModOk = true;
        Searching_AP = true;
    }
  }
}

void Connect_to_AP () {
  if(Parser_WIFI_Serial_com("AT+CWJAP_CUR=\"" + SSID_AP +"\",\"123456789\"\r\n")) {
    Serial.println("AP-OK");
    CONNECT_to_AP = true;
  }
  else {
    Serial.println("AP-NOT");
    CONNECT_to_AP = false;
  }
}


void Open_TCP_on_AP () {
  if (Parser_WIFI_Serial_com("AT+CIPSTA_CUR?\r\n")) {
    IP_ADDR_AP = PARCE_DATA.substring(PARCE_DATA.indexOf("gateway:\"") + 9,PARCE_DATA.indexOf("\"+CIPSTA_CUR:net"));
    Serial.println("IP_AP: " + IP_ADDR_AP);
    if (Parser_WIFI_Serial_com("AT+CIPSTART=\"TCP\",\"" + IP_ADDR_AP + "\",333\r\n")) {
      CONNECT_on_TCP = true;
    }
  }
 }

void Check_status () {
  if (Parser_WIFI_Serial_com("AT+CIPSTATUS\r\n")) {
    if (PARCE_DATA.indexOf("STATUS:3")) {
      CONNECT_Full = true; 
      Serial.println("GO DATA!");
    }
  }
  
}

bool Send_Data() {
  byte bytes_data = Data_Send.length();
    if (Parser_WIFI_Serial_com("AT+CIPSEND=" + String(bytes_data) + "\r\n")) {
      if (Parser_WIFI_Serial_com(Data_Send + "\n")) return true;
    }
  }

void Values() {
  Sensors.read();
  int val=analogRead(analogMQ7);
  byte Tempo = Sensors.getTemperatureC();
  byte Hum = Sensors.getHumidity();
  val = map(val, 0, 1000, 0, 100); 

  Data_Send = 'T' + String(Tempo) + 'H' + String(Hum) + 'C' + String(val) + "$";

 
}
