#include "NokaSmart.h"
NokaUART_Slave Slave;
String data = "";
String data_send = "T10H40C31";
unsigned long time_req = 0;
int change_req = 0;
void setup() {
  Slave.Start();
}

void loop() {
  data = Slave.Get_Data();
  if (data!="NULL") Parser_data(data); 
  if (!Slave.RequestMasterPort && change_req!=3) {
    if (millis() -  time_req > 2000) {
    dooble_request();
    time_req = millis();
    change_req++;
    //if (change_req==3) Serial.println("Req error");
    }
  }
}

void Parser_data(String datas) {
  switch(datas[0]) {
    case CS: Handler_critical(datas); break;
    case REQ: Handler_request(datas); break;
    case SCE: Connect(datas); break;
    case ERM: Handler_errors(datas); break;
    case RES: Handler_responses(datas); break;
  }
 
}

void Handler_request(String request){ // обработчик запросов
  switch(request[1]) {
    case TELEMETRY:
      //Slave.Send_Byte_Data(data_send,TELEMETRY);
      Slave.RequestMasterPort = false;
      change_req=0;
      break;
    case CONet: Serial.write("Config$");break;
}

}

void Handler_critical(String critical){ // обработчик критического сигнала
  Serial.write("critical!!!$");
}

void Connect (String Session){ // установление подключения
  Serial.write("Session$");
}

void Handler_errors(String error){ // обработчик критического сигнала
  Serial.write("Error$");
}

void Handler_responses(String response) { // обработчик ответов
  switch(response[1]) {
    case TELEMETRY: 
      switch (response[2]){
        case 0x4F: Slave.RequestMasterPort=true; 
        //Serial.println("Req OK");
        break;
      }
}
}

void dooble_request(){
  Slave.Send_Byte_Data(data_send,TELEMETRY);
}
