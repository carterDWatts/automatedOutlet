#include <ESP8266HTTPClient.h>
#include<ESP8266WiFi.h>

boolean powered = true;
int powerPin = 5;

void setup(){
  Serial.begin(115200);
  WiFi.begin("ORBI24","jaggedcarrot123");  // add ssid and password here

  while(WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.println("Waiting for connection");
  }
  
  Serial.println("Connected...");
  delay(1000);
  if (WiFi.status() ==WL_CONNECTED){
    Serial.println("Wi-Fi Connected!");
  }
  delay(1000);
  Serial.println("Sending message to server espcomm");
  delay(1000);
  int res=sendmessage("Hi,Server");
  delay(1000);
  if (res==1){
    Serial.println("Send Successfully");
  } else{
    Serial.println("Error on Server side or client side.");
  }

  pinMode(powerPin, OUTPUT);
  
}

void loop(){
  
  String data=receivelastmessage();
  int response=validatemessage(data);
  
  if (response==1){
    
    Serial.print("Message Received from client : ");
    Serial.println(data);
    handleresponse(data);
  }

  if(data == "1"){
      Serial.println("Turning on");
      powered = true;
  }else if(data == "0"){
      powered = false;
      Serial.println("Turning off");
  }else{
      Serial.print("Neither 1 or 0 is the content of the most recent line");
  }

  if(powered){
    digitalWrite(powerPin, HIGH);  
    Serial.println("set to HIGH");  
  }else{
    digitalWrite(powerPin, LOW);
    Serial.println("set to LOW");
  }
  
}

String receivelastmessage(){
  
  String lastmessage="";
  if(WiFi.status()==WL_CONNECTED){
    
    HTTPClient http;
    String url="http://carterwatts.xyz/datastorage.txt";
    http.begin(url);
    http.addHeader("Content-Type","text/plain");
    int httpCode=http.GET();
    String data=http.getString();    
    lastmessage=getlastline(data);
    Serial.println(lastmessage);
    http.end();
  }else{
    lastmessage="";
  }
  return lastmessage;
}

String getlastline(String str){
  
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

int validatemessage(String message){
  
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

void handleresponse(String message){
  
  Serial.println("Sending reply to client.");
  String mes="";
  for (int i=7;i<message.length();i++){    
    mes=mes+message[i];
  }
  
  if (mes=="hello,server"){
      
    int response=sendmessage("server:hello,client");
    if (response==1) {      
      Serial.println("Replied Successfully.");
    }else{      
      Serial.println("Error! Check Internet Connection or Server Error!");
    }
  }
}

int sendmessage(String d){
  
  int sres;
  int net;
  
  if (WiFi.status()==WL_CONNECTED){
    
    HTTPClient http;
    String url="http://carterwatts.xyz/writefile.php?data="+d;
    http.begin(url);
    http.addHeader("Content-Type","text/plain");
    int httpCode=http.GET();
    String payload=http.getString();
    Serial.println("While sending I received this from server : "+payload);
    
    if (payload=="SUCCESS. Data written in 00file."){      
      sres=1;
    }else{      
      sres=0;
    }
    
    http.end();
    net=1;
    
  }else{    
    Serial.println("Internet Problem!");
    net=0;
  }
  
  return (net && sres);
}
