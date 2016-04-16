#include <SoftwareSerial.h>
 
#define DEBUG true
 #define trigpin 8
#define echopin 9
#define lmotorf 4
#define lmotorb 5
#define rmotorf 7
#define rmotorb 6
#define en1 11
#define en2 12
#define esp_pin 13
SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
void straight()
{
  digitalWrite(lmotorf,1);
  digitalWrite(lmotorb,0);
  digitalWrite(rmotorf,1);
  digitalWrite(rmotorb,0);
  
}
void right()
{
  digitalWrite(lmotorf,0);
  digitalWrite(lmotorb,1);
  digitalWrite(rmotorf,1);
  digitalWrite(rmotorb,0);
}
void left()
{
  digitalWrite(lmotorf,1);
  digitalWrite(lmotorb,0);
  digitalWrite(rmotorf,0);
  digitalWrite(rmotorb,1);
}
void back()
{
   digitalWrite(lmotorf,0);
  digitalWrite(lmotorb,1);
  digitalWrite(rmotorf,0);
  digitalWrite(rmotorb,1);
}
void _stop()
{
   digitalWrite(lmotorf,1);
  digitalWrite(lmotorb,1);
  digitalWrite(rmotorf,1);
  digitalWrite(rmotorb,1);
}

void setup()
{
  Serial.begin(115200);
  esp8266.begin(115200); // your esp's baud rate might be different
  
  pinMode(trigpin,OUTPUT);
  pinMode(echopin,INPUT);
   
  sendCommand("AT+RST\r\n",2000,DEBUG); // reset module
  sendCommand("AT+CWMODE=1\r\n",1000,DEBUG); // configure as access point
  sendCommand("AT+CWJAP=\"Faisal@CSE\",\"faisal35\"\r\n",3000,DEBUG);
  delay(10000);
  sendCommand("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendCommand("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendCommand("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80

  pinMode(en1,OUTPUT);
  pinMode(en2,OUTPUT);
  pinMode(esp_pin,OUTPUT);
  digitalWrite(esp_pin,HIGH);
  digitalWrite(en1,HIGH);
  digitalWrite(en2,HIGH);
  Serial.println("Server Ready");
}
int incomingByte; 
int ultra()
{
   float duration,distance;
  digitalWrite(trigpin,HIGH);
  delay(10);
  digitalWrite(trigpin,LOW);
  duration=pulseIn(echopin,HIGH);
  distance=((duration/2)/30);
  return distance;
}
int connectionId=0;
void loop()
{
  String content;
  int dis=ultra();
   if(esp8266.find("IPD,"))
    {
          
      connectionId = esp8266.read()-48;
  if (esp8266.available()>=0) {
   
      while(esp8266.available())
      {
        incomingByte = esp8266.read();
      }
    
    // if it's a capital H (ASCII 72), turn on the LED:
    if (incomingByte == 'U') {
      straight();
      Serial.println("UP");
  
    }
    if (incomingByte == 'D') {
      back();
      Serial.println("DOWN");
    }
    if (incomingByte == 'L') {
      left();
      Serial.println("LEFT");
    }
    if (incomingByte == 'R') {
     right();
      Serial.println("RIGHT");
    }
    if (incomingByte == '1') {
     
      Serial.println("1");
    }
    if (incomingByte == '2') {
     
      Serial.println("2");
    }
    if (incomingByte == '3') {
     
      Serial.println("3");
    }
    if (incomingByte == '4') {
     
      Serial.println("4");
    }
    if (incomingByte == 'M') {

      Serial.println("M");
    }
    if (incomingByte == 'S')
    {
      _stop();
      Serial.println("S");
    }
   
  }
 
    }
    if((dis>=1)&&(dis<=20))
    {
      content="v";
    
      sendHTTPResponse(connectionId,content);
        Serial.println(dis);
    }

}
 
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    int dataSize = command.length();
    char data[dataSize];
    command.toCharArray(data,dataSize);
           
    esp8266.write(data,dataSize); // send the read character to the esp8266

    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
   
    
    return response;
}
 
/*
* Name: sendHTTPResponse
* Description: Function that sends HTTP 200, HTML UTF-8 response
*/
void sendHTTPResponse(int connectionId, String content)
{
     
     // build HTTP response
     String httpResponse;
     String httpHeader="";
     httpResponse =content+"\r\n"; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
     sendCIPData(connectionId,httpResponse);
}
 
/*
* Name: sendCIPDATA
* Description: sends a CIPSEND=<connectionId>,<data> command
*
*/
void sendCIPData(int connectionId, String data)
{
   String cipSend = "AT+CIPSEND=";
   cipSend += connectionId;
   cipSend += ",";
   cipSend +=data.length();
   cipSend +="\r\n";
   sendCommand(cipSend,10,DEBUG);
   sendData(data,10,DEBUG);
}
 
/*
* Name: sendCommand
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendCommand(String command, const int timeout, boolean debug)
{
    String response = "";
           
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
   
    
    return response;
}
 
