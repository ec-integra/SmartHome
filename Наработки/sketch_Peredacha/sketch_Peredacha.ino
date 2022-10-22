#include <SPI.h>                                               // Подключаем библиотеку для работы с шиной SPI.
#include <nRF24L01.h>                                          // Подключаем файл настроек из библиотеки RF24.
#include <RF24.h>                                              // Подключаем библиотеку для работы с nRF24L01+.
RF24     radio(9, 10);                                         // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов модуля (CE, SS)
int      myData;                                            // Объявляем массив для хранения и передачи данных.
int      ackData[4];                                           // Объявляем массив для получения данных из пакета подтверждения приёма (до 32 байт включительно).
unsigned long stoped;                                                               //
bool Get_Data = false;
void setup(){                                                  //
    Serial.begin(9600);
    Serial.println("Work!");
    radio.begin           ();                                  // Инициируем работу модуля nRF24L01+.
    radio.setChannel      (60);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
    radio.setDataRate     (RF24_1MBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
    radio.setPALevel      (RF24_PA_MAX);                       // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
    radio.enableAckPayload();                                  // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
//  radio.enableDynamicPayloads();                             // Разрешить динамически изменяемый размер блока данных на всех трубах.
    radio.openWritingPipe (0xAABBCCDD11LL);                    // Открываем трубу с адресом 0xAABBCCDD11 для передачи данных (передатчик может одновременно вещать только по одной трубе).
    radio.openReadingPipe(1,0xAABBCCDD22LL);
    radio.startListening  (); 
}                                                              //
                                                               //
void loop(){
  /*if (Get_Data == false) {
    if (millis()-stoped>5000) {
      radio.stopListening  ();
      stoped=millis();
    if(radio.write(&myData, sizeof(myData))){
      radio.startListening  (); 
      //Serial.println("Data go");
      }
    else Serial.println("Data NOT");
    // Отправляем данные из массива myData указывая сколько байт массива мы хотим отправить.                           //
    }  */                                          // Устанавливаем задержку на 50 мс. В этом скетче нет смысла слать данные чаще.
if(Serial.read()=='G'){
  radio.stopListening  ();
  if(radio.write(&myData, sizeof(myData))){radio.startListening();Serial.println("Data go WITH G");}
}
  //}
if(radio.available() ){                       // Если в буфере имеются принятые данные из пакета подтверждения приёма, то ...
        Get_Data = true;
        radio.read(&ackData, sizeof(ackData));                 // Читаем данные из буфера в массив ackData указывая сколько всего байт может поместиться в массив.
        Serial.println(millis());
        Serial.println(ackData[0]);
        Serial.println(ackData[1]);
        Serial.println(ackData[2]);
        Serial.println(ackData[3]);
        Serial.println("-----");
        radio.writeAckPayload (1, &myData, sizeof(myData));
    }
 else Get_Data=false;                               
}
