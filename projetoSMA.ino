
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>


uint8_t mac[6]  = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 100);
IPAddress server(172, 16, 0, 200);

#define VIN 5 // V power voltage
#define R 1000 //ohm resistance value
#define CLIENT_ID "grupo11SMA_ecb2d2f4"

bool startsend = HIGH;// flag for sending at startup
int pinoLed = 7;
int pinoBuz = 10;
int pinoSensorLuz = A0; //LDR
int valorLuz = 0;
int pinoSensorTemp = A2;
float valorTemp = 0;
bool valorBuz = true;
bool statusLed = false;
float seno;
int frequencia;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
//  String strPayload = String((char*)payload);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");//MQTT_BROKER
}

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



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //sets serial port for communication
  pinMode(pinoLed,OUTPUT);
  pinMode(pinoBuz,OUTPUT);

//  Eth settings
//  Ethernet.begin(mac, ip);
// setup ethernet communication using DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Unable to configure Ethernet using DHCP"));
    for (;;);
  }

  // setup mqtt client
  mqttClient.setClient(ethClient);
//   mqttClient.setServer("test.mosquitto.org", 1883);//use public broker
  mqttClient.setServer( "mqtt.eclipse.org", 1883); // or local broker
  //Serial.println(F("MQTT client configured"));
  mqttClient.setCallback(callback);


//  mqtt settings
  Serial.println(mqttClient.connect(CLIENT_ID));
  if (mqttClient.connect(CLIENT_ID)) {
    mqttClient.publish("ufpe/cin/in1116/2020/grupo11","Daniel Candido e Flaviano Dias");
//    mqttClient.subscribe("ufpe/cin/in1116/2020/grupo11/actuators/led");
//    mqttClient.subscribe("ufpe/cin/in1116/2020/grupo11/actuators/horn");
  }
  

}


void postData(){
  if (mqttClient.connect(CLIENT_ID)) {
    float t = analogRead(pinoSensorTemp)*(5.0/1023.0)*10;
    String tekst = "{ value:";
    String Sum;
    Sum = tekst + " " + String(t) + " }";
//    Serial.println(Sum);
    Serial.println(mqttClient.publish("ufpe/cin/in1116/2020/sensor/temperature", Sum.c_str()));
  }
  else{
    Serial.println(mqttClient.state());
    
  }
}

void test(){
  valorLuz = analogRead(pinoSensorLuz);
  valorTemp = analogRead(pinoSensorTemp)*(5.0/1023.0)*10;
  if(valorLuz>400)
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
}

void sendData() {
  char msgBuffer[20];
  valorLuz = analogRead(pinoSensorLuz);
  float l = sensorRawToPhys(valorLuz);
  float t = analogRead(pinoSensorTemp)*(5.0/1023.0)*10;
  bool led = digitalRead(pinoLed);
  bool buzzy = digitalRead(pinoBuz);
  char data[5];
  
//  
//  mqttClient.publish("ufpe/cin/in1116/2020/grupo11/sensor/ldr", dtostrf(l,4,2,data));
//  mqttClient.publish("ufpe/cin/in1116/2020/grupo11/sensor/temp", dtostrf(t,4,2,data));
//  mqttClient.publish("ufpe/cin/in1116/2020/grupo11/actuator/led/status", led == HIGH ? "ON" : "OFF");
//  mqttClient.publish("ufpe/cin/in1116/2020/grupo11/actuator/horn/status", buzzy ? "ON" : "OFF");

  
  
//  Serial.print("Temperature: ");
//  Serial.print(t);
//  Serial.println("oC");
//  Serial.print("Lumen: ");
//  Serial.print(l);
//  Serial.println("lm");
//  Serial.print("Led: ");
//  Serial.println(led == true ? "ON" : "OFF");
//  Serial.print("Buzz: ");
//  Serial.println(buzzy ? "ON" : "OFF");
  
//  postData();
  
}
void loop() {
  // put your main code here, to run repeatedly:
//  test();
  float t = analogRead(pinoSensorTemp)*(5.0/1023.0)*10;
  String tekst = "{\"value\":";
  String Sum;
  Sum = tekst + " " + String(t) + "}";
  Serial.println(Sum.c_str());
  delay(1500); 
  mqttClient.publish("ufpe/cin/in1116/2020/sensor/temperature", Sum.c_str());
  mqttClient.loop();
  
  

}
