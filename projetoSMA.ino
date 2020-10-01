
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>


uint8_t mac[6]  = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(172, 16, 0, 100);
IPAddress server(172, 16, 0, 2);

#define VIN 5 // V power voltage
#define R 1000 //ohm resistance value
#define CLIENT_ID "grupo11SMA_ecb2d2f4"

bool startsend = HIGH;// flag for sending at startup
int pinoLed = 13;
int pinoBuz = 10;
int pinoSensorLuz = A0; //LDR
int valorLuz = 0;
int pinoSensorTemp = A2;
float valorTemp = 0;
bool valorBuz = true;
bool statusLed = false;
float seno;
int frequencia;


long previousMillis;

EthernetClient ethClient;
PubSubClient mqttClient;

void ligarSirene(){
  for(int x=0;x<180;x++){
      //converte graus para radiando e depois obtém o valor do seno
      seno=(sin(x*3.1416/180));
      //gera uma frequência a partir do valor do seno
      frequencia = 2000+(int(seno*1000));
//      Serial.print("Freq: ");
//      Serial.println(frequencia);
      tone(pinoBuz,frequencia);
      delay(2);
    }
}

int sensorRawToPhys(int raw){
  // Conversion rule
  float Vout = float(raw) * (VIN / float(1023));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int phys=500/(RLDR/1000); // Conversion resitance to lumen
  return phys;
}

void sendData() {
  char msgBuffer[20];
  valorLuz = analogRead(pinoSensorLuz);
  float l = sensorRawToPhys(valorLuz);
  float t = analogRead(pinoSensorTemp)*(5.0/1023.0)*10;
  bool led = digitalRead(pinoLed);
  bool buzzy = digitalRead(pinoBuz);
//  
//  Serial.print("Temperature: ");
//  Serial.print(t);
//  Serial.println("oC");
  Serial.print("Lumen: ");
  Serial.print(l);
  Serial.println("lm");
//  Serial.print("Led: ");
//  Serial.println(led);
//  Serial.print("Buzz: ");
//  Serial.println(buzzy ? "ON" : "OFF");
  
  
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //sets serial port for communication
  pinMode(pinoLed,OUTPUT);
  pinMode(pinoBuz,OUTPUT);

//   // setup ethernet communication using DHCP
//  if (Ethernet.begin(mac) == 0) {
//    //Serial.println(F("Unable to configure Ethernet using DHCP"));
//    for (;;);
//  }
//
//  Serial.println(F("Ethernet configured via DHCP"));
//  Serial.print("IP address: ");
//  Serial.println(Ethernet.localIP());
//  Serial.println();
 //convert ip Array into String
  

}

void loop() {
  // put your main code here, to run repeatedly:
  valorLuz = analogRead(pinoSensorLuz);
  valorTemp = analogRead(pinoSensorTemp)*(5.0/1023.0)*10;
  if(valorLuz<240)
  {                
    digitalWrite(pinoLed,HIGH);
    valorBuz = true;
  }
  else
  {                    
    digitalWrite(pinoLed,LOW);
    valorBuz = false;
  }
  
  if (valorBuz == true){
    ligarSirene();
    
  }
  else{
      tone(pinoBuz,0);
  }
//  Serial.print("luz: ");
//  Serial.print(valorLuz);
//  Serial.print("\n");
//  Serial.print("temp: ");
//  Serial.print(valorTemp);
//  Serial.print("\n");
  sendData();
  delay(100); 

}
