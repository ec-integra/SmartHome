String response = "AT+CIPSTA_CUR?+CIPSTA_CUR:ip:\"192.168.0.108\"+CIPSTA_CUR:gateway:\"192.168.0.1\"+CIPSTA_CUR:netmask:\"255.255.255.0\"OK";
String IP = "";
void setup() {
  Serial.begin(9600);
  IP = response.substring(response.indexOf("gateway:\"") + 9,response.indexOf("\"+CIPSTA_CUR:net"));
  Serial.println(IP);
  Serial.println(IP.length());

}

void loop() {
  // put your main code here, to run repeatedly:

}
