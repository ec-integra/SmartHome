#include "NokaSmart.h"
#include <SoftwareSerial.h>
// Slave's методы

void NokaUART_Slave::Start() { // Начало работы Serial порта
	Serial.begin(_bod);
	Serial.println("Slave ON!");}

String NokaUART_Slave::Get_Data() { // Парсер данных из сериал. В IDE 
	if (_request) { // необходимо проверка на NULL - значение. Пример: if (data!="NULL") {Serial.println(data);}
	_InMSG = ""; 
	_request = false;
	}
	if (Serial.available()) {
		char c;
		while (Serial.available()>0){
			c=Serial.read();
			if (c=='\n' || c=='$' || c=='/'){
				  //Serial.println (_InMSG);
				  _request = true;
				  return (_InMSG); // Возвращает значение String 
				  
			}
			else {
			  _InMSG +=c;
			}
		}
	}
	return "NULL"; // Возвращает значение String 
	}
	
	
void NokaUART_Slave::Print_Data() { // Используется просто для просмотра вывода/данных
	_PrintMSG = Get_Data(); // автоматизация, без проверки в IDE
	if (_PrintMSG != "NULL") Serial.println(_PrintMSG);
}

void NokaUART_Slave::Send_Byte_Data(String DataIn, byte protocol, byte Mod = 0) { // Отправка строки данных (байтов)
	char Data1[3] = {};
	switch(protocol) {
		case SCE:
			Data1[0] = SCE;
			Data1[1] = Name;
			Data1[2] = '$';
			Serial.write(Data1,sizeof(Data1));
			break;
			
		case ERM:
			//Serial.println("NOKA!");
			Data1[0] = ERM;
			Data1[1] = Mod;
			Data1[2] = '$';
			Serial.write(Data1,sizeof(Data1));
			break;
			
		case TELEMETRY:
			//Serial.println(DataIn.length());
			int CHECK_SUM = 0;
			char Data[DataIn.length()+5]={};
			Data[0] = REQ;
			Data[1] = TELEMETRY;
			for (int i=0;i<DataIn.length();i++) {
				Data[i+2] = DataIn[i];
				CHECK_SUM ^= DataIn[i];
			}
			Data[DataIn.length()+2] = 'S';
			Data[DataIn.length()+3] = CHECK_SUM;
			Data[DataIn.length()+4] = '$';
			Serial.write(Data,sizeof(Data));
			break;
}
}
/////////////////////
/////////////////////

// Master's методы
void NokaUART_Master::Start() { // Начало работы Serial порта
	Serial.begin(_bod);
	Serial.println("Master ON!");}
	

void NokaUART_Master::Send_Byte_Data(String _Data, byte protocol){
	char Data[_Data.length()+2]={};
	Data[0] = protocol;
	for (int i=0;i<_Data.length();i++) {
				Data[i+1] = _Data[i];
			}
	Data[_Data.length()+1] = '\n';
	
	Serial.write(Data,sizeof(Data)); 
}