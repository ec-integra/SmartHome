#include <SPI.h>                                               // Подключаем библиотеку для работы с шиной SPI.
#include <nRF24L01.h>                                          // Подключаем файл настроек из библиотеки RF24.
#include <RF24.h>                                              // Подключаем библиотеку для работы с nRF24L01+.
#include "NokaSmart.h"
NokaUART_Slave Slave;

RF24     radio(9, 10);                                         // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов модуля (CE, SS)
int      myData;                                            // Объявляем массив для хранения и передачи данных.w
int      ackData[4]; 

bool ConnectModRadio = false;

bool SES_CON = false;
String data = "";
String data_send = "";
unsigned long time_req = 0;

int change_req = 0;
void setup() {
  Slave.Start();
  Slave.Name = 'R';
  Config_Radio();
}

void loop() {
  Parser_Radio();
  
  data = Slave.Get_Data();
  if (data!="NULL") Parser_data(data);
  
  if (SES_CON) { 
  if (!Slave.RequestMasterPort && change_req!=3) {
    if (millis() -  time_req > 500) {
      //Serial.println("OK!!!");
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
      //Serial.println("REQ!");
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
  //Serial.println(SES_CON);
  Slave.Send_Byte_Data("H",SCE);
  SES_CON = true;
  //Serial.println(SES_CON);
}

void Handler_errors(String error){ // обработчик критического сигнала
  Serial.write("Error$");
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

void dooble_request(){
  Slave.Send_Byte_Data(data_send,TELEMETRY);
}

void Config_Radio() {
    radio.begin           ();                                  // Инициируем работу модуля nRF24L01+.
    radio.setChannel      (70);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
    radio.setDataRate     (RF24_1MBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
    radio.setPALevel      (RF24_PA_MAX);                       // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
    radio.enableAckPayload();                                  // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
//  radio.enableDynamicPayloads();                             // Разрешить динамически изменяемый размер блока данных на всех трубах.
    radio.openWritingPipe (0xAABBCCDD11LL);                    // Открываем трубу с адресом 0xAABBCCDD11 для передачи данных (передатчик может одновременно вещать только по одной трубе).
    radio.openReadingPipe(1,0xAABBCCDD22LL);
    radio.startListening  (); 
}

void Parser_Radio() {
  //radio.startListening  ();
  if(radio.available() ){                       // Если в буфере имеются принятые данные из пакета подтверждения приёма, то ...
        data_send = "";
        Slave.NewData = true;
        radio.read(&ackData, sizeof(ackData));                 // Читаем данные из буфера в массив ackData указывая сколько всего байт может поместиться в массив.
        //Serial.println(millis());
        //Serial.println(ackData[0]);
        //Serial.println(ackData[1]);
        //Serial.println(ackData[2]);
        //Serial.println(ackData[3]);
        //Serial.println("-----");
        radio.writeAckPayload (1, &myData, sizeof(myData));
        //Slave.NewData = false;
        data_send = 'T' + String(ackData[1]) + 'H' + String(ackData[2]) + 'C' + String(ackData[3]);
        //Serial.println(data_send);
    }                               
}

