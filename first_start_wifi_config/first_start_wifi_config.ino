// библиотека для работы программного Serial
#include <SoftwareSerial.h>
 
// создаём объект для работы с программным Serial
// и передаём ему пины TX и RX
SoftwareSerial mySerial(8, 9);
 
// serial-порт к которому подключён Wi-Fi модуль
#define WIFI_SERIAL    mySerial

void setup() {
  Serial.begin(9600);
  WIFI_SERIAL.begin(115200);
  WIFI_SERIAL.println("AT");
  WIFI_SERIAL.println("AT+UART_DEF=9600,8,1,0,0");

}

void loop() {
  // put your main code here, to run repeatedly:

}
