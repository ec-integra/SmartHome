// библиотека для работы программного Serial
#include <SoftwareSerial.h>
 
// создаём объект для работы с программным Serial
// и передаём ему пины TX и RX
SoftwareSerial mySerial(8, 9);
 
// serial-порт к которому подключён Wi-Fi модуль
#define WIFI_SERIAL    mySerial

String INPUT_MSG="";
byte Data[4]= {1,69,32,40};
byte bytes=0;

void setup()
{
  // открываем последовательный порт для мониторинга действий в программе
  // и передаём скорость 9600 бод
  Serial.begin(9600);
  while (!Serial) {
  // ждём, пока не откроется монитор последовательного порта
  // для того, чтобы отследить все события в программе
  }
  Serial.print("Serial init OK\r\n");
  // открываем Serial-соединение с Wi-Fi модулем на скорости 115200 бод
  WIFI_SERIAL.begin(9600);
}
 
void loop()
{
    while (Serial.available()>0){
    char c=Serial.read();
    if (c=='\n' || c=='$' || c=='/'){
      delay(500);
      //Serial.println(INPUT_MSG);
      Begin();
      INPUT_MSG="";
    }
    else {
      INPUT_MSG +=c;
    }
    
}
}

void Begin() {
  if (INPUT_MSG.startsWith("G:")){
    INPUT_MSG.replace("G:","");
    bytes = sizeof(Data);
    sendATCommand("AT+CIPSEND=" + String (bytes), true);
    WIFI_SERIAL.println(Data[0],HEX);

    sendATCommand("AT+CIPSEND=" + String (bytes), true);
    WIFI_SERIAL.println(Data[1],HEX);

    sendATCommand("AT+CIPSEND=" + String (bytes), true);
    WIFI_SERIAL.println(Data[2],HEX);

    sendATCommand("AT+CIPSEND=" + String (bytes), true);
    WIFI_SERIAL.println(Data[3],HEX);
   }
   sendATCommand(INPUT_MSG,true);
}

void sendATCommand(String cmd,bool wait) {
  long _timeout = millis() + 6500;
  
  String INPUT_SIM="";                            // Переменная для хранения результата
  String Resp = "";
  //Serial.println(cmd);                          // Дублируем команду в монитор порта
  WIFI_SERIAL.println(cmd);                          // Отправляем команду модулю
  while (!WIFI_SERIAL.available() && millis() < _timeout)  {} // Ждем ответа 3 секунды, если пришел ответ или наступил таймаут, то...
  
  if (wait){
 
    while (millis() < _timeout && Resp.indexOf("OK")<0 && Resp.indexOf("ERROR")<0) { // Ждем ответа 3 секунды , собираем данные
    if (WIFI_SERIAL.available()){ // Если пришло что-то записываем
    INPUT_SIM = WIFI_SERIAL.readString();
     if (INPUT_SIM.startsWith(cmd)) {  // Убираем из ответа дублирующуюся команду
      INPUT_SIM = INPUT_SIM.substring(INPUT_SIM.indexOf("\r", cmd.length()) + 2);
    }
    INPUT_SIM.replace("\n"," ");
    Resp = Resp+INPUT_SIM;
    
  }
  }

}
else {
  if (WIFI_SERIAL.available()){ // Если пришло что-то записываем
    INPUT_SIM = WIFI_SERIAL.readString();
    INPUT_SIM.replace("\n"," ");
    Resp = Resp+INPUT_SIM;
}
}
  Serial.println (Resp);
}
