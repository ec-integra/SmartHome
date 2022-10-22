#include <SoftwareSerial.h>
#include <TroykaDHT.h>
#include <TroykaMQ.h>

int analogMQ7 = A0;
#define SENSORS_PIN A4

int val;
int DATA[4]; 
bool UP_CONNECT = false;
unsigned long TIME_SEND = 0;
DHT Sensors(SENSORS_PIN, DHT11);

SoftwareSerial WIFI_Client(8, 9);

String SSID_AP = "Noka_AP";

// Конфиг клиента
String AT = "AT";
String CWMODE = "AT+CWMODE_CUR=1";
String CONNECT_TO_AP = "AT+CWJAP_CUR=\"" + SSID_AP + "\",\"123456789\"";
String STATUS = "AT+CIPSTATUS";
String SEARCH_AP = "AT+CWLAP";
String CHECK_IP_AP = "AT+CIPSTA_CUR?";

String command[2] = {AT,CWMODE};

 
void setup()
{
  Sensors.begin();
  Serial.begin(9600);
  WIFI_Client.begin(9600);
  config_Client ();
  
}
 
void loop()
{
  Values();
  // если приходят данные из Wi-Fi модуля - отправим их в порт компьютера
  parser_data();
  // если приходят данные из компьютера - отправим их в Wi-Fi модуль
  if (Serial.available()) {
    Serial.read();
    //char P = Serial.read();
    //if (P == 'Y') {send_data(DATA);}
    //else WIFI_Client.write(Serial.read());
    send_data(DATA);
  }
  if (UP_CONNECT) {
    if (millis() - TIME_SEND > 2000) {
      TIME_SEND = millis();
      send_data(DATA);
    }
  }
}

void config_Client (){
  bool TCP = false;
  bool Status = false;
  bool connecting = false;
  int i=0;
  int firstOK=0;
  int Success=0;
  String request=""; 
  while(i<2){
    WIFI_Client.println(command[i]);
    while (!WIFI_Client.available()) {}
    request=WIFI_Client.readString();
    request.replace("\r","");
    request.replace("\n\n",":");
    firstOK = request.indexOf("OK");
    if (firstOK>0) Success++;
    i++;
  //Serial.println(request);
  }
  if (Success==2){
    Serial.println("Success config");
    Serial.println("Search AP...");
    if (up_search_AP()){
      Serial.println("AP is found!");
      Serial.println("Start connect to AP");
      connecting = connect_to_AP ();
      Status = check_status (false);
      if (connecting && Status) {
        Serial.println("Success connect");
        TCP = open_TCP();
      }
      else Serial.println("Error connect");
  }
  }
  else {
    Serial.println("Error");
  }
  if (TCP) {
    //Serial.println("Can send data!");
    First_proto();
    //Serial.println(IP_AP ());
  }
  else Serial.println("No open TCP");
  
  }

 bool connect_to_AP () {
  bool connecting = false;
  String request=""; 
  int firstOK=0;
  WIFI_Client.println(CONNECT_TO_AP);
  while (!WIFI_Client.available()) {}
  while (firstOK<=0){
  request=WIFI_Client.readString();
  request.replace("\r","");
  request.replace("\n\n",":");
  firstOK = request.indexOf("OK");
  }
  if (firstOK>0) connecting = true;
  return connecting;
  }

  bool check_status (bool TCP) {
    bool Status = false;
    String request=""; 
    int firstOK=0;
    WIFI_Client.println(STATUS);
    while (!WIFI_Client.available()) {}
    while (firstOK<=0){
    request=WIFI_Client.readString();
    request.replace("\r","");
    request.replace("\n\n",":");
    firstOK = request.indexOf("OK");
    }
    
    if (firstOK>0) {
      if ((request.indexOf("STATUS:2")>0)&& (!TCP)) Status = true;
        
      if ((request.indexOf("STATUS:3") > 0) && TCP) Status = true;
       
    }
  return Status; 
  }

bool open_TCP () {
  String IPaddr_AP = "";
  IPaddr_AP = IP_AP();
  String CONNECT_TCP = "AT+CIPSTART=\"TCP\",\"" + IPaddr_AP + "\",333";
  bool status_TCP = false;
  bool TCP = false;
  String request=""; 
  int firstOK=0;
  WIFI_Client.println(CONNECT_TCP);
  while (!WIFI_Client.available()) {}
  while (firstOK<=0){
  request=WIFI_Client.readString();
  request.replace("\r","");
  request.replace("\n\n",":");
  firstOK = request.indexOf("OK");
  }
  if (firstOK>0) {
    TCP = true;
    status_TCP = check_status(TCP);
  }
  return status_TCP;
}


bool up_search_AP () {
  int i=0;
  unsigned Time=0;
  bool finding = false;
  while (!finding) {
  if (millis()-Time>1500){
  i++;
  Time = millis();
  Serial.print("...search: ");
  Serial.println(i);
  finding = search_AP();
  }
  }
  return finding;
}

bool search_AP(){
  bool finding = false;
  String request = "";
    WIFI_Client.println(SEARCH_AP);
    while (!WIFI_Client.available()) {}
    while (!request.endsWith("\nOK\r\n")) {
      if (WIFI_Client.available()){
      request = WIFI_Client.readString();
      //Serial.println(request);
      }
    }
    if (request.indexOf(SSID_AP)>0) {
    finding = true;
    //Serial.println("AP find!");
    }
    //else Serial.println("Search...");
  return finding;
}

String IP_AP (){
  String request = "";
  String IP ="";
  WIFI_Client.println(CHECK_IP_AP);
  while (!WIFI_Client.available()) {}
  if (WIFI_Client.available()){
      request = WIFI_Client.readString();  
      }
  request.replace("\n","");
  request.replace("\r","");
  IP = request.substring(request.indexOf("+CIPSTA_CUR:gateway:\""),request.indexOf("\"+CIPSTA_CUR:netmask:"));
  IP.replace("+CIPSTA_CUR:gateway:\"","");
  return IP;
}

void parser_data () {
 String Data_wifi = ""; 
 if (WIFI_Client.available()) {
  Data_wifi = WIFI_Client.readString();
  Data_wifi.replace("\r","");
  Data_wifi.replace("\n\n","");
  //Serial.print("IN: ");
  Serial.println(Data_wifi);
  if (Data_wifi.indexOf("IPD")>0){
    Serial.println("New Data!");
    receive_data(Data_wifi);
  }
  if (Data_wifi.indexOf("CLOSED")>0 || Data_wifi.startsWith("CLOSED")){
    Serial.println("Server off");
  }
 }
}

void receive_data(String message){
  String datas="";
  message.replace("+IPD,","");
  message.replace("\n","");
  message.replace("\r","");
  datas = message.substring(message.indexOf(":")+1);
  //Serial.print("Data:");
  //Serial.println(datas);
  if (datas.indexOf("TEL:")>0 || datas.startsWith("TEL:")){
    send_data(DATA);
  }
}

void send_data(int data[]){
  String Data_send = Create_data(data);
  byte bytes=0;
  String request=""; 
  bytes=Data_send.length() ;
  WIFI_Client.println("AT+CIPSEND=" + String (bytes));
  while (!WIFI_Client.available()) {}
  if (WIFI_Client.available()) {
  WIFI_Client.println(Data_send);
  request = WIFI_Client.readString();
  //Serial.println(request);
  while (request.indexOf("SEND OK")<0){}
  if (request.indexOf("SEND OK")>0) Serial.println("Send ok");
  }
}

String Create_data (int data[]) {
  String Data_send = "TEL:";
  for (int i=0;i<4;i++){
    switch (i) {
    case 0: Data_send = Data_send + "S" + String (data[i]); break;
    case 1: Data_send = Data_send + "T" + String (data[i]);break;
    case 2: Data_send = Data_send + "H" + String (data[i]);break;
    case 3: Data_send = Data_send + "G" + String (data[i]);break;
    
    }
  }
  Data_send.replace("\n","");
  Data_send.replace("\r","");
  return Data_send;
}

void First_proto() {
  String Data_send = "com:200";
  byte bytes=0;
  String request=""; 
  bytes=Data_send.length() ;
  WIFI_Client.println("AT+CIPSEND=" + String (bytes));
  while (!WIFI_Client.available()) {}
  if (WIFI_Client.available()) {
  WIFI_Client.println(Data_send);
  //request = WIFI_Client.readString();
  //Serial.println(request);
  //while (request.indexOf("SEND OK")<0){}
  //if (request.indexOf("SEND OK")>0) Serial.println("Send ok");
  //}
  UP_CONNECT = true;
}
}

void Values() {
  Sensors.read();
  val=analogRead(analogMQ7);
  val = map(val, 0, 1000, 0, 100);
  if (val < 65) DATA[0]=0;
  else DATA[0]=1; 
  DATA[1]=Sensors.getTemperatureC();
  DATA[2]=Sensors.getHumidity();
  DATA[3]=val;
}
