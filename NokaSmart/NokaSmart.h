#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>

//Константы протоколов

#define TELEMETRY 0x44 // Отправка данных с датчиков
#define SCE 0x48 // Начало/конец сессии сети
#define CS 0x21 // Экстренный сигнал (Утечка газа, сигнализация)
#define CONet 0x43 // Конфиг сети 
#define ERM 0x45 // Протокол ошибок
#define REQ 0x52 // Сделать запрос
#define RES 0x41 // Отправить ответ

///////////////////

class NokaUART_Slave {
	public:
	NokaUART_Slave(int bod = 9600){ // скорость в бодах
		_bod = bod;
	};
	bool NewData = false;
	bool RequestMasterPort = true;
	char Name;
	
	String Get_Data(); // Функция получения данных из сериал порта
	void Start(); // Serial.begin
	void Print_Data(); // Просто вывод входязих данных для тестов
	void Send_Byte_Data(String Data, byte protocol, byte Mod = 0); // Отправка байтов данных
	//bool response(byte protocol,char *Data)
	
	private:
	bool _request = false;
	int _bod;
	String _InMSG="";
	String _PrintMSG="";
};


class NokaUART_Master {
	public:
	NokaUART_Master(int bod = 9600){ // скорость в бодах
		_bod = bod;
	};
	bool RequestSoftPort = false;
	byte RunningSoftPort  = 255;
	bool RunWork = false;
	void Start(); // Serial.begin
	void Send_Byte_Data(String _Data, byte protocol); // Serial.begin
	
	
	private:
	String _InMSG="";
	int _bod;
	bool _request = false;
	
};

