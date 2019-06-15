#include <Wire.h>
#include <ESP8266WiFi.h>

#define DIST_S 1600*58.2
#define IDX_SIZE 100
#define UNIT_NUM 2  //the number of sensor

const char* server2 = "3.16.161.61";
const char* MY_SSID = "myWifi";
const char* MY_PWD = "myPassword";


int vib =D0;
int echoPin = D1;
int trigPin = D2;
int echoPin2 = D3;
int trigPin2 = D4;
int echoPin3 = D5;
int trigPin3 = D6;
int echoPin4 = D7;
int trigPin4 = D8;

String C_id="C01";              
String T_id="T01"; 
String ID[4] ; 
String ID[0] = C_id+ T_id+ "S01";   
String ID[1] = C_id+ T_id+ "S02";   
String ID[2] = C_id+ T_id+ "S03";   
String ID[3] = C_id+ T_id+ "S04";            
   
int idx;

String len_payload[4];
String vib_payload[4];

void setup() {
    idx=0;
    for(int i=0;i<UNIT_NUM;i++){
      len_payload[i]="";
      vib_payload[i]="";
    }
    connectWifi();//connecting WIFI
    pinMode(vib, INPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    pinMode(trigPin3, OUTPUT);
    pinMode(echoPin3, INPUT);
    pinMode(trigPin4, OUTPUT);
    pinMode(echoPin4, INPUT);
    Serial.begin(9600);
}

void loop() {
  double distance[4];
  long measurement =TP_init();
  
  //get distance
  distance[0]=get_dis(echoPin,trigPin);
  distance[1]=get_dis(echoPin2,trigPin2);
  distance[2]=get_dis(echoPin3,trigPin3);
  distance[3]=get_dis(echoPin4,trigPin4);
 
 //print result 
 for(int i=0;i<UNIT_NUM;i++){
  Serial.print(distance[i]);
  Serial.println("cm");
  Serial.print("measurment = ");
  Serial.println(measurement);
 }
 if(idx != IDX_SIZE-1){
  for(int i=0;i<UNIT_NUM;i++){ 
    len_payload[i]+=String(distance[i])+",";
    vib_payload[i]+=String(measurement)+",";
  }
 }
 else{
   for(int i=0;i<UNIT_NUM;i++){
     len_payload[i]+=String(distance[i]);
     vib_payload[i]+=String(measurement);
   }
 }
 
   //sending the data to server
  if(idx==IDX_SIZE-1){
    for(int i=0; i<UNIT_NUM;i++){
          sendSV(ID[i], len_payload[i], vib_payload[i])
    }
    idx=0;
  }
  else
    idx++;
  delay(100);
}

float get_dis(int echo ,int trig){
  float duration;
  
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
   
  duration = pulseIn(echo, HIGH, DIST_S); 
  return ((float)(340 * duration) / 10000) / 2;
}

long TP_init(){
  delay(10);
  long measurement=pulseIn (vib, HIGH);
  return measurement;
}

void sendSV(String ID, String len_payload, String vib_payload){  
   WiFiClient client;
     if (client.connect(server2, 3000)) { 
   Serial.println("SV WiFi Client connected ");
   
   String getheader = "GET /data?ID="+ String(ID) 
   +"&len="+ String(len_payload)
   +"&vib="+ String(vib_payload)
   +"&state="+ String(2) 
   +" HTTP/1.1";
   client.println(getheader);
   client.println("User-Agent: ESP8266 ");  
   client.print("Host: 3.16.161.61\n");
   client.print("Connection: close\n");
   client.print("\n\n");
   delay(1000);
  }
  client.stop();
}

void connectWifi(){
  Serial.println();
  Serial.print("connecting to WiFi: ");
  Serial.println(MY_SSID);
  WiFi.begin(MY_SSID,MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n Got WiFi, IP address: ");
  Serial.println(WiFi.localIP());   
}
