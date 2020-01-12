#include <ESP8266HTTPClient.h>
#include<ESP8266WiFi.h>

boolean powered = true;
int powerPin = 5;

const char* ssid = "ORBI24";
const char* password = "jaggedcarrot123";
//const char* ssid = "HelloThere";
//const char* password = "generalkenobi";

const char* host = "carterwatts.com";

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);  // add ssid and password here

  while(WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.println("Waiting for connection");
  }

  if (sendMessage("starting")){
    Serial.println("Send Successful");
  } else{
    Serial.println("Error sending");
  }

  pinMode(powerPin, OUTPUT);
  
}

void loop(){
 
  if(WiFi.status() !=3){
    Serial.println("no connection");
    connectWiFi();
    return;
  }

  String data=receivelastMessage();
  int response=validateMessage(data);
  
  if (response==1){
    Serial.print("Data Received from client : ");
    Serial.println(data);
    handleResponse(data);
  }

  if(data == "1"){
      Serial.println("Turning on");
      powered = true;
  }else if(data == "0"){
      powered = false;
      Serial.println("Turning off");
  }else{
      Serial.println("No usable data");
  }

  if(powered){
    digitalWrite(powerPin, HIGH);  
    Serial.println("set to HIGH");  
  }else{
    digitalWrite(powerPin, LOW);
    Serial.println("set to LOW");
  }
  
}

int connectWiFi(){

  Serial.println("["+String(millis())+"] ConnectWifi started");
  WiFi.begin(ssid, password);
 
  delay(2000);
  Serial.print("Wifi stats code: ");
  Serial.println(WiFi.status());
 
  Serial.println("["+String(millis())+"] connectWifi complete");  
  return WiFi.status();
  
}

String receivelastMessage(){
  
  String lastMessage="";
  if(WiFi.status()==WL_CONNECTED){
    
    HTTPClient http;
    String url="http://carterwatts.com/datastorage.txt";
    http.begin(url);
    http.addHeader("Content-Type","text/plain");
    int httpCode=http.GET();
    String data=http.getString();    
    lastMessage=getLastLine(data);
    Serial.println(lastMessage);
    http.end();
  }else{
    lastMessage="";
  }
  return lastMessage;
}

String getLastLine(String str){
  
  String s="";
  int len=str.length();
  for (int i=len-2;i>=0;i--){
    
    if (str[i]=='\n'){      
      break;
    }else{
      s=s+str[i];
    }
  }
  
  String rs="";
  for (int i=s.length()-1;i>=0;i--){    
    rs=rs+s[i];
  }
  
  return rs;
}

int validateMessage(String message){
  
  String sender="";  
  for (int i=0;i<message.length();i++){
    if (message[i]==':'){
      break;
    }else{      
      sender=sender+message[i];
    }
  }
  if (sender=="client"){    
    return 1;
  }
  else{    
    return 0;
  }
}

void handleResponse(String message){
  
  Serial.println("Sending reply to client");
  String mes="";
  for (int i=7;i<message.length();i++){    
    mes=mes+message[i];
  }
  
  if (mes=="hello,server"){
      
    int response=sendMessage("server:hello,client");
    if (response==1) {      
      Serial.println("Replied Successfully.");
    }else{      
      Serial.println("Error");
    }
  }
}

int sendMessage(String d){
  
  int sres;
  int net;
  
  if (WiFi.status()==WL_CONNECTED){
    
    HTTPClient http;
    String url="http://carterwatts.com/writefile.php?data="+d;
    http.begin(url);
    http.addHeader("Content-Type","text/plain");
    int httpCode=http.GET();
    String payload=http.getString();
    Serial.println("While sending, received this from server : "+payload);
    
    if (payload=="Success, data written to file."){      
      sres=1;
    }else{      
      sres=0;
    }
    
    http.end();
    net=1;
    
  }else{    
    Serial.println("Could not send, no connection");
    net=0;
  }
  
  return (net && sres);
}
