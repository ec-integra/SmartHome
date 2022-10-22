#include <SoftwareSerial.h>
String INPUT_MSG="";
SoftwareSerial Arduino(10, 11); // RX, TX
void setup() {
  Arduino.begin(9600);
  Serial.begin(9600);
  Arduino.write("name: ERROR$");

}

void loop() {
  Wait_msg();
  if (Serial.available()){
    Arduino.write(Serial.read());
  }
}

void Wait_msg(){
  while (Arduino.available()>0){
    char c=Arduino.read();
    if (c=='\n' || c=='$' || c=='/'){
      Begin();
      Serial.println(INPUT_MSG);
      INPUT_MSG="";
    }
    else {
      INPUT_MSG +=c;
    }
    
}
}

void Begin(){
  if (INPUT_MSG.startsWith("name:")){
    INPUT_MSG.replace("name:"," ");
    INPUT_MSG.trim();
    Arduino.write("name: OK$");
    Serial.print("Ok name: ");
    Serial.println(INPUT_MSG);
  }
}
