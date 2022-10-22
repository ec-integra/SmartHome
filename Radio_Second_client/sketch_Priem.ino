#include <Wire.h>
#include <Adafruit_Sensor.h>

#include <TroykaDHT.h>
#include <SPI.h>                                              // Подключаем библиотеку для работы с шиной SPI.
#include <nRF24L01.h>                                          // Подключаем файл настроек из библиотеки RF24.
#include <RF24.h> 
#define SENSORS_PIN 4
DHT Sensors(SENSORS_PIN, DHT11);

RF24     radio(9, 10);                                         // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов модуля (CE, SS).
int      myData;                                            // Объявляем массив для приёма и хранения данных (до 32 байт включительно).
int      ackData[4];                                           // Объявляем массив для передачи данных в пакете подтверждения приёма (до 32 байт включительно).
int val;
int analogMQ7 = A7;
unsigned long TIME_SEND = 0;
void setup(){
    Sensors.begin();  
    Serial.begin(9600);
    radio.begin();                                             // Инициируем работу nRF24L01+
    radio.setChannel      (70);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
    radio.setDataRate     (RF24_1MBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
    radio.setPALevel      (RF24_PA_MAX);                       // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
    radio.enableAckPayload();                                  // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
//  radio.enableDynamicPayloads();                             // Разрешить динамически изменяемый размер блока данных на всех трубах.
    radio.openWritingPipe(0xAABBCCDD22LL);
    //radio.openReadingPipe (1, 0xAABBCCDD11LL);                 // Открываем 1 трубу с адресом 0xAABBCCDD11, для приема данных.
    //radio.startListening  ();                                  // Включаем приемник, начинаем прослушивать открытые трубы.
    //radio.writeAckPayload (1, &ackData, sizeof(ackData) );     // Помещаем данные всего массива ackData в буфер FIFO. Как только будут получены любые данные от передатчика на 1 трубе, то данные из буфера FIFO будут отправлены этому передатчику вместе с пакетом подтверждения приёма его данных.
}                                                              // В модуле имеется 3 буфера FIFO, значит в них одновременно может находиться до трёх разных или одинаковых данных для ответа по одной или разным трубам.
                                                               // После отправки данных из буфера FIFO к передатчику, соответствующий буфер очищается и способен принять новые данные для отправки.
void loop(){
    Values();
    if (millis() - TIME_SEND > 1000) {
      TIME_SEND = millis();
      
      radio.write(&ackData, sizeof(ackData));
      
      //for (int i=0;i<4;i++) {
        //Serial.println(ackData[i]);
      //}
    }
    
    // Если все 3 буфера FIFO уже заполнены, то функция writeAckPayload() будет проигнорирована.
}

void Values() {
  Sensors.read();
  val=analogRead(analogMQ7);
        if (val<=1000) val=val*100/1000;
        else val=100;
        if (val < 65) ackData[0]=0;
        else ackData[0]=1; 
        ackData[1]=Sensors.getTemperatureC();
        ackData[2]=Sensors.getHumidity();
        ackData[3]=val;
}

