#include <SoftwareSerial.h>
#include "NokaSmart.h"
NokaUART_Master Master;
String data = "";
String response = "";
char TEL[] = "RD$";
char SES[] = "H$";
char ci[] = "!$";
byte NumSoftPort = 0;
bool time_check = false;
SoftwareSerial SoftPorts[2] = {SoftwareSerial(8, 9), SoftwareSerial(10, 11)};
char SoftPorts_name[2] = {0, 0};


void setup() {
  SoftPorts[0].begin(9600);
  SoftPorts[1].begin(9600);
  Master.Start();
  //Serial.println("GO");
  if (Serial) Serial.println("Master ok!");
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    switch (c) {
      case 'O': Master.RunWork = true; break;
      case 'C': Master.RunWork = false; SoftPorts_name[0] = 0; SoftPorts_name[1] = 0; break;
    }
    //Serial.println("DATA IN");
  }
 

  if (Master.RunWork) {
    NumSoftPort = round (millis() / 1000) % 2;

    response = listen_port(NumSoftPort);
    if (response != "") Parser_data(response);
    if (Master.RunningSoftPort != NumSoftPort) {
      if (check_session_slave(NumSoftPort)) SoftPorts[NumSoftPort].write(TEL);
      else {
        SoftPorts[NumSoftPort].write(SES);
        Master.Send_Byte_Data(String(NumSoftPort) + "S" + "0",SCE);
      }
      //if (!time_check) {
      //SoftPorts[NumSoftPort].write(SES);
      //time_check = true;
      //}
      //SoftPorts[NumSoftPort].write(TEL);
    }
    Master.RunningSoftPort = NumSoftPort;
  }
}

String listen_port (int i) {
  if (Master.RequestSoftPort) {
    data = "";
    Master.RequestSoftPort = false;
  }
  SoftPorts[i].listen();
  if (SoftPorts[i].overflow()) {
    Serial.println("OVERFLOW: " + String(i));
  }
  //Serial.println("Listen: " + String(i));
  if (SoftPorts[i].available()) {
    char c;
    while (SoftPorts[i].available() > 0) {
      c = SoftPorts[i].read();
      if (c == '\n' || c == '$' || c == '/') {
        Master.RequestSoftPort = true;
        return data;
        //_request = true;
        //return (_InMSG); // Возвращает значение String

      }
      else {
        data += c;
      }
    }
  }
  return "";
}


void Parser_data(String datas) {
  switch (datas[0]) {
    case CS: Handler_critical(datas); break;
    case REQ: Handler_request(datas); break;
    case SCE: Connect(datas); break;
    case ERM: Handler_errors(datas); break;
  }
}

void Handler_request(String request) { // обработчик запросов
  switch (request[1]) {
    case TELEMETRY: Handler_telemetry(request); break;
    case CONet: break;
  }
}

void Handler_critical(String critical) { // обработчик критического сигнала
  switch (critical[1]) {
    case 'T': break;
    case 'H': break;
    case 'C': break;
  }
}

void Connect (String Session) { // установление подключения
  SoftPorts_name[NumSoftPort] = Session[1];
  Master.Send_Byte_Data(String(NumSoftPort) + String(SoftPorts_name[NumSoftPort]) + "S" + "1",SCE);
  SoftPorts[NumSoftPort].write(TEL);
}

void Handler_errors(String error) { // обработчик критического сигнала
  Master.Send_Byte_Data(String(NumSoftPort) + String(SoftPorts_name[NumSoftPort]) + error[1],ERM);
}

void Handler_telemetry(String telemetry) {
  String tempo = telemetry.substring(telemetry.indexOf('T') + 1, telemetry.indexOf('H'));
  String humidity = telemetry.substring(telemetry.indexOf('H') + 1, telemetry.indexOf('C'));
  String co = telemetry.substring(telemetry.indexOf('C') + 1, telemetry.indexOf('S'));
  if (CHECK_SUM(telemetry)) {
    Ok_Response(NumSoftPort);
    //Serial.print("From:");
    //Serial.print(Master.RunningSoftPort);
    telemetry.replace("R", "");
    //Serial.println("; Temp: " + tempo + " Hum: " + humidity + " CO2: " + co);
    Master.Send_Byte_Data(String(NumSoftPort) + String(SoftPorts_name[NumSoftPort]) + telemetry, TELEMETRY);
  }
}

bool CHECK_SUM(String TEM) {
  String data_tem = TEM.substring(TEM.indexOf('T'), TEM.indexOf('S'));
  String CHECK_SUM_REQ_S = TEM.substring(TEM.indexOf('S') + 1);
  char CHECK_SUM_REQ = CHECK_SUM_REQ_S[0];
  int SUM = 0;
  for (int i = 0; i < data_tem.length(); i++) {
    //Serial.println(data_tem[i],HEX);
    SUM ^= data_tem[i];
  }
  //Serial.print("SUM:");
  //Serial.println(SUM);
  if (SUM == CHECK_SUM_REQ) return true;
  else return false;
}

void Ok_Response(int i) {
  //Serial.print("OK request");
  char response[4];
  response[0] = RES;
  response[1] = TELEMETRY;
  response[2] = 0x4F;
  response[3] = '$';
  SoftPorts[i].write(response, 4);
}

bool check_session_slave(byte i) {
  if (SoftPorts_name[i] != 0) return true;
  else return false;
}
