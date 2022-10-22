#include <SoftwareSerial.h>
#include "NokaSmart.h"
NokaUART_Slave Slave;

SoftwareSerial WIFI_AP(8, 9);
void setup() {
  Slave.Start();
  WIFI_AP.begin(9600);
  Slave.Name = 'W';
  while (!WIFI_AP) {};
    if (WIFI_AP){
    config_AP ();
    delay(500);
  }

}
String data = "";
String PARCE_DATA = "";
String SSID_AP = "Noka_AP";

String data_send = "";
int change_req = 0;
unsigned long time_req = 0;
unsigned long time_wait = 0;

bool AP_FULL_UP = false;
bool Session = false;

void loop() {
  data = Slave.Get_Data();
    if (data!="NULL")  {
      //Serial.println("REQ!");
      Parser_data(data);
    }
  if(AP_FULL_UP){
    Parser_pakets ();
    //Parser_pakets();
    if (Session) Work_Slave_Wifi();
  }
  else {
    while (WIFI_AP.available()>0) {
    WIFI_AP.read(); }
    config_AP ();
    delay(500);
  }
}


bool Parser_WIFI_Serial_com (String COM,int wait = 3000) {
  time_wait = millis();
  PARCE_DATA = "";
  WIFI_AP.print(COM);
  while(true && (millis() - time_wait < wait)) {
    if (WIFI_AP.available()) {
      char c = WIFI_AP.read();
      //Serial.write(c); 
      if (PARCE_DATA.endsWith("OK")) {
        //Serial.println(PARCE_DATA);
        return true;
      }
      else if (PARCE_DATA.endsWith("ERROR") || PARCE_DATA.endsWith("FAIL")) {
        //Serial.println(PARCE_DATA);
        //Slave.Send_Byte_Data("I",ERM,'I');
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
  //Slave.Send_Byte_Data("I",ERM,'I');
  return false;
  //Serial.println("ERROR WIFI");
  
}

void config_AP () {
  if(Parser_WIFI_Serial_com("AT\r\n")) {
      //Serial.println("AT-OK");
      if(Parser_WIFI_Serial_com("AT+CWMODE_CUR=2\r\n")){
        //Serial.println("MOD-OK");
        if(Parser_WIFI_Serial_com("AT+CWSAP_CUR=\"" + SSID_AP +"\",\"123456789\",10,3\r\n")){
          //Serial.println("AP UP!");
          if(Parser_WIFI_Serial_com("AT+CIPMUX=1\r\n")){
            if(Parser_WIFI_Serial_com("AT+CIPSERVER=1,333\r\n")){
              //Serial.println("Let's go!");
              //Slave.Send_Byte_Data("H",SCE);
              AP_FULL_UP = true;
            }
          }
        }
    }
  }
}

void Parser_pakets () {
  if (WIFI_AP.available()) {
      char c = WIFI_AP.read();
      if (c=='\n') {
        //Serial.println(PARCE_DATA);
        Parser_data_pakets(PARCE_DATA);
        PARCE_DATA = "";
      }
      else {
        PARCE_DATA+=c;
      }
    
  }
}

void Parser_data_pakets (String DATA) {
  if (DATA.indexOf("PD,") > 0) {
    Slave.NewData = true;
    data_send = DATA.substring(DATA.indexOf(":T")+1,DATA.indexOf("$"));
    //Serial.println(data_send); 
  }
}


void Handler_request(String request){ // обработчик запросов
  Session = Parser_WIFI_Serial_com("AT\r\n",2000);
  switch(request[1]) {
    case TELEMETRY:
      if (Session) {
      //Serial.println("RE DATA");
      //Slave.Send_Byte_Data(data_send,TELEMETRY);
      Slave.RequestMasterPort = false;
      change_req=0; 
      }
      else Slave.Send_Byte_Data("I",ERM,'I');
      break;
}
}

void dooble_request(){
  Slave.Send_Byte_Data(data_send,TELEMETRY);
  //Serial.println("NOK");
}

void Handler_responses(String response) { // обработчик ответов
  switch(response[1]) {
    case TELEMETRY: 
      switch (response[2]){
        case 0x4F: 
        Slave.RequestMasterPort=true;
        Slave.NewData = false; 
        //Serial.println("Req OK");
        break;
      }
}
}

void Handler_Session() {
  Session = Parser_WIFI_Serial_com("AT\r\n",1000);
  if (Session)Slave.Send_Byte_Data("H",SCE);
  else Slave.Send_Byte_Data("I",ERM,'I');
  
}

void Parser_data(String datas) {
  //Serial.println(datas[0]);
  switch(datas[0]) {
    case REQ: Handler_request(datas); break;
    case RES: Handler_responses(datas); break;
    case SCE: Handler_Session(); break;
  }
}

void Work_Slave_Wifi() {
  if (!Slave.RequestMasterPort && change_req!=3) {
    if (millis() -  time_req > 500) {
      switch (Slave.NewData) {
      case true: dooble_request();break;
      case false: Slave.Send_Byte_Data("M",ERM,'M');break;
      }
    
    time_req = millis();
    change_req++;
    //if (change_req==3) Serial.println("Req error");
    }
  }
}

