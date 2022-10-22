  #include <SoftwareSerial.h>
String INPUT_MSG="";
String Name = "Iskra";
bool HI=true;
unsigned long TIME;
int request_hi = 5000;
SoftwareSerial Iskra(10, 11); // RX, TX
void setup() {
  HI=true;
  Iskra.begin(9600);
  Serial.begin(9600);
  pinMode(8,OUTPUT);
}

void loop() {
  if (HI==true) {
    Hello();
  }
  while (Iskra.available()>0){
    char c=Iskra.read();
    if (c=='\n' || c=='$' || c=='/'){ // Ожидание конца строки
      Begin();
      INPUT_MSG="";
    }
    else {
      INPUT_MSG +=c;
    }
    
}
}

void Begin(){
  if (INPUT_MSG.startsWith("command:")){ // проверка начала сообщения
    Serial.println(INPUT_MSG);
    INPUT_MSG.replace("command:",""); // инкапсюляция - удаление заголовка 
    if (INPUT_MSG.endsWith("On LED")){
      digitalWrite(8,HIGH); // включение лед
      Serial.println("Led On!");
    }
    if (INPUT_MSG.endsWith("Off LED")){
      digitalWrite(8,LOW); // выключение лед
      Serial.println("Led Off!");
    }
  }
  if (INPUT_MSG.startsWith("name:")){
    INPUT_MSG.replace("name:"," ");
    if(INPUT_MSG.endsWith("OK")){
      HI=false;
      Serial.println("Name save is Successful!");
    }
    if(INPUT_MSG.endsWith("ERROR")){
      HI=true;
    }
    //Serial.print("Him name is ");
    //Serial.println(INPUT_MSG);
    //Iskra.write(Transfer(INPUT_MSG));
    //Serial.println(Transfer(INPUT_MSG)); 
  }
  else{
    Serial.println(INPUT_MSG);
  }
}

char* Transfer(String OUT_MSG){ // * - вернуть массив функция переводит строку в массив, чтобы отправлять другой ардуинке
  OUT_MSG = OUT_MSG + "$"; // $ - стоп символ
  OUT_MSG.trim(); // удаление пробелов до и после стрки
  char *rezult = new char[OUT_MSG.length()]; // создание массива для перевод строки в символьный массив для передачи по виртуальному юарт
  rezult = (OUT_MSG).c_str(); // перевод строки в символьный массив для передачи по виртуальному юарт
  //Serial.println(request);
  return rezult;
  
}

void Hello(){ // запрос на запоминание имени устройства
  if( millis() - TIME > request_hi) {
    Name = "name:" + Name;
    TIME = millis();
    Iskra.write(Transfer(Name));
    }
}
