
import controlP5.*;
import processing.serial.*;
Serial NokaSmartMasterPort;
ControlP5 Fronted;
int port_num;
int time_wait=0;

int i = 0;

int Packet_Wifi = 0;
int Packet_Radio = 0;

boolean SensWifi = false;
boolean MOD_Wifi = false;
boolean SensRadio = false;
boolean MOD_Radio = false;

boolean Session_Wifi = false;
boolean Session_Radio = false;

boolean Connect = false;


 
String COM[] = {"H0S0", "H1S0", "H1WS1", "E1WM", "E1WM", "E1WM", "H0S0", "D1WDT26H68C12SV", "H0RS1", "D0RDT10H21C50SV", "D1WDT30H40C100SV", "D0RDT15H68C100SV", "E1WI", "E0RM", "E1WM","D1WDT26H68C12SV"};


int COLOR_ERRORW = 255;
int COLOR_ERRORR = 255;
//TELEMETRY = 0x44 
//SCE = 0x48; 
//ERM = 0x45;

void setup() {
  String comPorts[] = Serial.list();
  colorMode(HSB, 255, 255, 255);
  Fronted = new ControlP5(this);

  size(1000, 400);
  //surface.setlocation();
  surface.setTitle("NokaSmart");
  printArray(comPorts);

  Fronted.addScrollableList("COM")
    .addItems(comPorts)
    .setPosition(10, 10)
    .setSize(110, 100)
    .setBarHeight(30)
    .setItemHeight(30)
    .setCaptionLabel("COM PORTS")
    .setColorActive(#FFC4C4)
    .setColorForeground(#F7A0F2)
    .setColorBackground(#DFEFF0)
    .setColorCaptionLabel(0)
    .setColorValueLabel(0)
    .setFont(createFont("Algerian", 15))
    .getCaptionLabel().align(CENTER,CENTER)
    ;
    
 //Fronted.getController("COM").getValueLabel().align(CENTER,CENTER);
  
  Fronted.addButton("Refresh")
    .setValue(0)
    .setCaptionLabel("   Refresh \n com ports")
    .setPosition(150, 100)
    .setSize(100, 40)
    .setFont(createFont("Algerian", 15))
    .setColorForeground(#F7A0F2)
    .setColorBackground(#DFEFF0)
    .setColorCaptionLabel(0)
    .setColorValueLabel(0)
    .getCaptionLabel().align(CENTER,TOP)
    ;
    

  Fronted.addToggle("OnOff")
    .setPosition(150, 10)
    .setSize(100, 40)
    .setColorActive(#1FFF03) 
    .setColorForeground(#FEFF03)
    .setColorBackground(#FF0505)
    .setCaptionLabel("DISCONNECT")
    .setColorCaptionLabel(0)
    .getCaptionLabel().alignX(CENTER)
    .setFont(createFont("Algerian", 15))
    ;  


  Fronted.addSlider("TempoR").lock()
    .setPosition(630, 70)
    .setSize(20, 100)
    .setRange(0, 50)
    .setCaptionLabel("Temp")
    .setColorLabel(#0513FA) 
    .setColorValue(#93038A) 
    .setColorValueLabel(#93038A)
    .getCaptionLabel().alignX(CENTER)
    .setFont(createFont("Algerian", 15))
    ;
  Fronted.addSlider("HumR").lock()
    .setPosition(710, 70)
    .setSize(20, 100)
    .setRange(20, 90)
    .setCaptionLabel("Hum")
    .setColorLabel(#0513FA) 
    .setColorValue(#93038A) 
    .setColorValueLabel(#93038A)
    .getCaptionLabel().alignX(CENTER)
    .setFont(createFont("Algerian", 15))
    ;
  Fronted.addSlider("CoR").lock()
    .setPosition(790, 70)
    .setSize(20, 100)
    .setRange(0, 100)
    .setCaptionLabel("Co2")
    .setColorLabel(#0513FA) 
    .setColorValue(#93038A) 
    .setColorValueLabel(#93038A)
    .getCaptionLabel().alignX(CENTER)
    .setFont(createFont("Algerian", 15))
    ;

  Fronted.addSlider("TempoW").lock()
    .setPosition(350, 70)
    .setSize(20, 100)
    .setRange(0, 50)
    .setColorLabel(#0513FA) 
    .setColorValue(#93038A) 
    .setColorValueLabel(#93038A)
    .setCaptionLabel("Temp")
    .getCaptionLabel().alignX(CENTER)
    .setFont(createFont("Algerian", 15))
    ;
  Fronted.addSlider("HumW").lock()
    .setPosition(430, 70)
    .setSize(20, 100)
    .setRange(20, 90)
    .setColorLabel(#0513FA) 
    .setColorValue(#93038A) 
    .setColorValueLabel(#93038A)
    .setCaptionLabel("Hum")
    .getCaptionLabel().alignX(CENTER)
    .setFont(createFont("Algerian", 15))
    ;

  Fronted.addSlider("CoW").lock()
    .setPosition(510, 70)
    .setSize(20, 100)
    .setRange(0, 100)
    .setColorLabel(#0513FA) 
    .setColorValue(#93038A) 
    .setColorValueLabel(#93038A)
    //.setColorForeground(Noka)
    .setCaptionLabel("Co2")
    .getCaptionLabel().alignX(CENTER)
    .setFont(createFont("Algerian", 15))
    ;

  Fronted.getController("TempoW").getValueLabel().setFont(createFont("Algerian", 15));
  Fronted.getController("HumW").getValueLabel().setFont(createFont("Algerian", 15));
  Fronted.getController("CoW").getValueLabel().setFont(createFont("Algerian", 15));
  Fronted.getController("TempoR").getValueLabel().setFont(createFont("Algerian", 15));
  Fronted.getController("HumR").getValueLabel().setFont(createFont("Algerian", 15));
  Fronted.getController("CoR").getValueLabel().setFont(createFont("Algerian", 15));

  Fronted.addTextlabel("ModuleWifi")
    .setText("Wi-Fi - ")
    .setPosition(385, 10)
    .setColorValue(#0513FA)
    .setFont(createFont("Algerian", 25))
    ;

  Fronted.addTextlabel("ModuleWifi_on/off")
    .setText(" OFF")
    .setPosition(460, 10)
    .setColorValue(#FC0000)
    .setFont(createFont("Algerian", 25))
    ;
    
    
  Fronted.addTextlabel("PacketWifi")
    .setText("Incoming Packets:" + Packet_Wifi)
    .setPosition(375, 40)
    .setColorValue(#676F00)
    .setFont(createFont("Algerian", 15))
    ;
  Fronted.addTextlabel("ModuleRadio")
    .setText("Radio - ")
    .setPosition(665, 10)
    .setColorValue(#0513FA)
    .setFont(createFont("Algerian", 25))
    ;
  Fronted.addTextlabel("ModuleRadio_on/off")
    .setText(" OFF")
    .setPosition(755, 10)
    .setColorValue(#FC0000)
    .setFont(createFont("Algerian", 25))
    ;
    
  Fronted.addTextlabel("PacketRadio")
    .setText("Incoming Packets:" + Packet_Radio)
    .setPosition(655, 40)
    .setColorValue(#676F00)
    .setFont(createFont("Algerian", 15))
    ;

  Fronted.addButton("Errors_WIFI").lock()
    .setVisible(false)
    //.setLabel("SENSORS ERROR\nMODULE ERROR")
    .setFont(createFont("Algerian", 20))
    .setColorLabel(#BCCEBB)
    .setPosition(350, 230)
    .setSize(200, 150)
    .getCaptionLabel().align(CENTER, CENTER)
    ;

  Fronted.addButton("Errors_Radio").lock()
    .setVisible(false)
    //.setLabel("SENSORS ERROR")
    .setFont(createFont("Algerian", 20))
    .setColorLabel(#BCCEBB)
    .setPosition(630, 230)
    .setSize(200, 150)
    .getCaptionLabel().align(CENTER, CENTER)
    ;
}


void Refresh() {
  String comPorts[] = Serial.list();
  if (comPorts.length <1) Fronted.getController("COM").setCaptionLabel("COM PORTS");
  Fronted.get(ScrollableList.class, "COM").setItems(comPorts);
  printArray(comPorts);
}

void COM(int port) {
  port_num = port;
}

void OnOff(boolean Flag) {
  if (Flag) {
    Connect = true;
    Fronted.get(Toggle.class, "OnOff").setCaptionLabel("CONNECT");
    NokaSmartMasterPort = new Serial(this, Serial.list()[port_num], 9600);
    NokaSmartMasterPort.write("O\n");
  } else {
    Connect = false;
    Fronted.get(Toggle.class, "OnOff").setCaptionLabel("DISCONNECT");
    NokaSmartMasterPort.write("C\n");
    NokaSmartMasterPort.stop();
  }
}


void Start() {
  NokaSmartMasterPort.write("G\n");
}

void draw() {
  background(#A0CBF7);
  fill(#06CB9C);
  rect(320, 10, 265, 200, 10);
  rect(600, 10, 265, 200, 10);
  Error_Ses_front();

  println(mouseX, mouseY);
  if (NokaSmartMasterPort != null) {
    if (NokaSmartMasterPort.available() > 0) {
      String data_master = NokaSmartMasterPort.readStringUntil('\n');
      data_master = data_master.trim();
      Handler_data(data_master);
    }
  }
}


void Error_Ses_front() {
  
  if (!Connect) {
    
    Fronted.get(Slider.class, "TempoW").setValue(0); 
    Fronted.get(Slider.class, "TempoW").setColorForeground(color(0,255, 255));
    Fronted.get(Slider.class, "HumW").setValue(0); 
    Fronted.get(Slider.class, "HumW").setColorForeground(color(0,255, 255));
    Fronted.get(Slider.class, "CoW").setValue(0); 
    Fronted.get(Slider.class, "CoW").setColorForeground(color(0,255, 255));
    Fronted.get(Slider.class, "TempoR").setValue(0); 
    Fronted.get(Slider.class, "TempoR").setColorForeground(color(0,255, 255));
    Fronted.get(Slider.class, "HumR").setValue(0); 
    Fronted.get(Slider.class, "HumR").setColorForeground(color(0,255, 255));
    Fronted.get(Slider.class, "CoR").setValue(0); 
    Fronted.get(Slider.class, "CoR").setColorForeground(color(0,255, 255));
    Packet_Radio = 0;
    Packet_Wifi = 0;
    Fronted.get(Textlabel.class, "PacketWifi").setText("Incoming Packets:" + Packet_Wifi);
    Fronted.get(Textlabel.class, "PacketRadio").setText("Incoming Packets:" + Packet_Radio);
    
  }
  
  if (Session_Wifi) {

    Fronted.get(Textlabel.class, "ModuleWifi_on/off").setText(" ON"); 
    Fronted.get(Textlabel.class, "ModuleWifi_on/off").setColorValue(#00FC00);

    if (SensWifi && MOD_Wifi) {
      Fronted.get(Button.class, "Errors_WIFI").setVisible(false);
      //Fronted.get(Button.class, "Errors_WIFI").setColorBackground(color(255, 65, 65));
    } else {
      Fronted.get(Button.class, "Errors_WIFI").setColorBackground(color(255, COLOR_ERRORW, COLOR_ERRORW));
      COLOR_ERRORW-=3;
      if (COLOR_ERRORW<80) COLOR_ERRORW = 255; 
      Fronted.get(Button.class, "Errors_WIFI").setVisible(true);

      if (!MOD_Wifi && SensWifi) {
        Fronted.get(Button.class, "Errors_WIFI").setLabel("MODULE ERROR");
      }

      if (!SensWifi && MOD_Wifi) {
        Fronted.get(Button.class, "Errors_WIFI").setLabel("SENSORS ERROR");
      }
      if (!MOD_Wifi && !SensWifi) {
        Fronted.get(Button.class, "Errors_WIFI").setLabel("SENSORS ERROR\nMODULE ERROR");
      }
    }
  }

  if (!Session_Wifi) {
    Fronted.get(Textlabel.class, "ModuleWifi_on/off").setText(" OFF"); 
    Fronted.get(Textlabel.class, "ModuleWifi_on/off").setColorValue(#FC0000);
  }

  if (Session_Radio) {

    Fronted.get(Textlabel.class, "ModuleRadio_on/off").setText(" ON"); 
    Fronted.get(Textlabel.class, "ModuleRadio_on/off").setColorValue(#00FC00);

    if (SensRadio && MOD_Radio) {
      Fronted.get(Button.class, "Errors_Radio").setVisible(false);
    } else {
      Fronted.get(Button.class, "Errors_Radio").setColorBackground(color(255, COLOR_ERRORR, COLOR_ERRORR));
      COLOR_ERRORR-=3;
      if (COLOR_ERRORR<80) COLOR_ERRORR = 255; 
      Fronted.get(Button.class, "Errors_Radio").setVisible(true);

      if (!MOD_Radio && SensRadio) {
        Fronted.get(Button.class, "Errors_Radio").setLabel("MODULE ERROR");
      }

      if (!SensRadio && MOD_Radio) {
        Fronted.get(Button.class, "Errors_Radio").setLabel("SENSORS ERROR");
      }
      if (!MOD_Radio && !SensRadio) {
        Fronted.get(Button.class, "Errors_Radio").setLabel("SENSORS ERROR\nMODULE ERROR");
      }
    }
  }

  if (!Session_Radio) {
    Fronted.get(Textlabel.class, "ModuleRadio_on/off").setText(" OFF"); 
    Fronted.get(Textlabel.class, "ModuleRadio_on/off").setColorValue(#FC0000);
  }
}

void Handler_data(String data) {
  switch(data.charAt(0)) {
  case 0x44: //TELEMETRY
    Handler_Telemetrys(data);
    break; 
  case 0x45: //ERM
    Handler_Errors(data);
    break; 
  case 0x48: //SCE
    Handler_Sessions(data);
    break;
  default: 
    break;
  }
}

void Handler_Telemetrys(String data) {
  int num_Slave_port = int(data.substring(data.indexOf('D')+1, data.indexOf('D')+2));
  int Temp = int(data.substring(data.indexOf('T')+1, data.indexOf('H')));
  int Hum = int(data.substring(data.indexOf('H')+1, data.indexOf('C')));
  int Co = int(data.substring(data.indexOf('C')+1, data.indexOf('S')));
  char Name = data.charAt(2);
  color TempCol = color(map(Temp, 0, 40, 0, 255), 255, 255);
  color HumCol = color(map(Hum, 20, 60, 0, 255), 255, 255);
  color CoCol = color(map(Co, 0, 70, 0, 255), 255, 255);

  if (Name == 'W') {
    Fronted.get(Slider.class, "TempoW").setValue(Temp); 
    Fronted.get(Slider.class, "TempoW").setColorForeground(TempCol);
    Fronted.get(Slider.class, "HumW").setValue(Hum); 
    Fronted.get(Slider.class, "HumW").setColorForeground(HumCol);
    Fronted.get(Slider.class, "CoW").setValue(Co); 
    Fronted.get(Slider.class, "CoW").setColorForeground(CoCol);
    SensWifi = true;
    MOD_Wifi = true;
    Packet_Wifi++;
    Fronted.get(Textlabel.class, "PacketWifi").setText("Incoming Packets:" + Packet_Wifi); 
  }

  if (Name == 'R') {
    Fronted.get(Slider.class, "TempoR").setValue(Temp); 
    Fronted.get(Slider.class, "TempoR").setColorForeground(TempCol);
    Fronted.get(Slider.class, "HumR").setValue(Hum); 
    Fronted.get(Slider.class, "HumR").setColorForeground(HumCol);
    Fronted.get(Slider.class, "CoR").setValue(Co); 
    Fronted.get(Slider.class, "CoR").setColorForeground(CoCol);
    SensRadio = true;
    MOD_Radio = true;
    Packet_Radio++;
    Fronted.get(Textlabel.class, "PacketRadio").setText("Incoming Packets:" + Packet_Radio);
  }
}

void Handler_Sessions(String data) {
  int num_Slave_port = int(data.substring(data.indexOf('H')+1, data.indexOf('S')));
  int Status=0;
  char Name=0;
  if (data.length() == 4) Status = char (data.charAt(3));
  if (data.length() == 5) {
    Status = char (data.charAt(4));
    Name = char(data.charAt(2));
  }

  if (Status == '1') {
    if (Name == 'W'){
      Session_Wifi = true;
      MOD_Wifi = true;
    }
    if (Name == 'R') {
      Session_Radio = true;
      MOD_Radio = true;
    }
  }
}

void Handler_Errors(String data) {
  int num_Slave_port = int(data.substring(data.indexOf('E')+1, data.indexOf('E')+2));
  char Name = char(data.charAt(2));
  char Module_off = char(data.charAt(3)); 
  if (Name == 'W') {
    if (Module_off == 'M') SensWifi = false;
    if (Module_off == 'I') MOD_Wifi = false;
  }
  if (Name == 'R') {
    if (Module_off == 'M') SensRadio = false;
    if (Module_off == 'I') MOD_Radio = false;
  }
}
