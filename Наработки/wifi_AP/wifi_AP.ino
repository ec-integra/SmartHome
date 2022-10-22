#include <SoftwareSerial.h>
String Data_wifi = "";
//int coint_byte =0;
//int last_byte =0;
int clients = 0;
byte Data[4];
SoftwareSerial WIFI_AP(8, 9);


// Конфиг точки доступа
String AT = "AT";
String CWMODE = "AT+CWMODE_CUR=2";
String CREAT_AP = "AT+CWSAP_CUR=\"Noka_AP\",\"123456789\",10,3";
String CIPMUX = "AT+CIPMUX=1";
String CIPSERVER = "AT+CIPSERVER=1,333";

String command[5] = {AT,CWMODE,CREAT_AP,CIPMUX,CIPSERVER};


 
void setup()
{
  Serial.begin(9600);
  WIFI_AP.begin(9600);
  config_AP ();
}
 
void loop()
{
  // если приходят данные из Wi-Fi модуля - отправим их в порт компьютера
  parser_data();
  // если приходят данные из компьютера - отправим их в Wi-Fi модуль
  if (Serial.available()) {
    WIFI_AP.write(Serial.read());
  }
}

void config_AP (){
  Serial.println("Start config AP");
  int i=0;
  int firstOK=0;
  int Success=0;
  String request=""; 
  while(i<5){
    WIFI_AP.println(command[i]);
    while (!WIFI_AP.available()) {}
    request=WIFI_AP.readString();
    request.replace("\r","");
    request.replace("\n\n",":");
    //request.replace("\n","");
    firstOK = request.indexOf("OK");
    if (firstOK>0) Success++;
    i++;
  Serial.println(request);
  }
  if (Success==5) Serial.println("AP create! TCP-server up!");
  }

/*
void parser_data () {
  if (WIFI_AP.available()>0) {
    char c = WIFI_AP.read();
    Data_wifi+=c;
    coint_byte++;
    if (last_byte != coint_byte) {
    last_byte = coint_byte;
    }
  }

  
  
    Serial.println(Data_wifi);
      Data_wifi="";
    //else if (c!=13) Data_wifi+=c;
    
  }
  //Serial.println(Data_wifi);
//}
  */
/*
void parser_data () {
  bool data = false;
  if (WIFI_AP.available()) {
    data=true;
    char c = WIFI_AP.read();
    if (c!=13){
      Data_wifi += c;
    coint_byte++;
    }
  }
  if (!data) {
  if (coint_byte!=0){
    Serial.println(Data_wifi);
    Data_wifi = "";
    coint_byte=0;
  }
  }
}
*/

void parser_data () {
 if (WIFI_AP.available()) {
  Data_wifi = WIFI_AP.readString();
  Data_wifi.replace("\r","");
  Data_wifi.replace("\n\n",":");
  Serial.println(Data_wifi);
  if (Data_wifi.indexOf("IPD")>0){
    Serial.println("New Data!");
    receive_data(Data_wifi);
  }
  if (Data_wifi.indexOf("CONNECT")>0){
    //Data_wifi.replace(",CONNECT","");
    //Data_wifi.replace("\n","");
    clients++;
    Serial.print("New client!");
  }
  if (Data_wifi.indexOf("CLOSED")>0 || Data_wifi.startsWith("CLOSED")){
    Serial.println("Client off");
  }
 }
}

void send_data(byte id_client, String data){
  byte bytes=0;
  String request=""; 
  bytes=data.length() ;
  WIFI_AP.println("AT+CIPSEND=" + String(id_client)+","+String (bytes));
  while (!WIFI_AP.available()) {}
  if (WIFI_AP.available()) {
  WIFI_AP.println(data);
  request = WIFI_AP.readString();
  //Serial.println(request);
  while (request.indexOf("SEND OK")<0){}
  if (request.indexOf("SEND OK")>0) Serial.println("Send ok");
  }
}

void receive_data(String message){
  String id="";
  String datas="";
  message.replace("+IPD,","");
  message.replace("\n","");
  message.replace("\r","");
  id= message.substring(0, message.indexOf(","));
  datas = message.substring(message.indexOf(":")+1);
  Serial.print("ID:");
  Serial.println(id);
  Serial.print("Data:");
  Serial.println(datas);
  if (datas.startsWith("TEL:")) parser_TELEMETRY(datas);
  if (datas.startsWith("com:")) parser_COMMANDS(datas,id.toInt());
}

void parser_TELEMETRY (String TELEMETRY) {
  TELEMETRY.replace("TEL:","");
  String Sign = TELEMETRY.substring(TELEMETRY.indexOf("S")+1,TELEMETRY.indexOf("T"));
  String Temp = TELEMETRY.substring(TELEMETRY.indexOf("T")+1,TELEMETRY.indexOf("H"));
  String Hum = TELEMETRY.substring(TELEMETRY.indexOf("H")+1,TELEMETRY.indexOf("G"));
  String Gas = TELEMETRY.substring(TELEMETRY.indexOf("G")+1);

  Serial.print("Sign");
  Serial.println(Sign);
  Serial.print("Temp");
  Serial.println(Temp);
  Serial.print("Hum");
  Serial.println(Hum);
  Serial.print("Gas");
  Serial.println(Gas);
}

void parser_COMMANDS (String command, byte id_client) {
  command.replace("com:","");
  if (command.indexOf("200")>0 || command.startsWith("200")){
    Serial.println("Client already!");
    //send_data(id_client,"TEL:");
  }  
}
