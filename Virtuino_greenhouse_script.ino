/* VirtuinoSE example: Enable or disable the led of pin 13
 * Supported boards: Arduino Uno or Nano + ESP8266 module 
 * Created by Ilias Lamprou
 * Updated Oct 27 2018
 * More examples: http://virtuino.com/index.php/virtuino-se/20-virtuino-se-code-examples
 */
// ------ VirtuinoSE V Memory Table
//  V13 memory -> Pin 13   digital output Relay1

#define WATER_SENSOR_PIN 6
#define DHT_PIN 8   //good
#define LED_PIN 9   //good
#define FAN_PIN 12
#define PUMP_PIN 10

//--------------- USER SETTINGS ---------------------------------------
const char* ssid = "UPC2274608-2Ghz";        // WIFI network SSID
const char* password = "T3pmjwdekxm5";       // WIFI network PASSWORD

//#define VIRTUINO_KEY  "1234"                // Virtuino KEY, only the requests that include this key will be acceptable
                                            // disable the line if you don't want to use a key                          

#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <dht.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);      // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin

const char* serverIP = "192.168.0.200";        
int port =8000;       

unsigned long timeStrored=0;
boolean debug=true;

#define V_memory_count 32         // the size of V memory. You can change it to a number <256)


float V_memory[V_memory_count];

int   LED_STATE = 1;
int   FAN_STATE = 0;
int   PUMP_STATE = 0;


LiquidCrystal_PCF8574 lcd(0x27);
dht DHT;
float temp;
float hum;
float water;
int displayPage = 0;
//============================================================== connectToWiFiNetwork
void connectToWiFiNetwork(){
    Serial.println("Connecting to "+String(ssid));
    clearESP_buffer(1000);
    espSerial.println("AT+GMR");
    waitForResponse("OK",1000);
    
    espSerial.println("AT+CWMODE=1");  // configure as client
    waitForResponse("OK",1000);
    
    //--- connect
    espSerial.print("AT+CWJAP=\"");
    espSerial.print(ssid);
    espSerial.print("\",\"");
    espSerial.print(password);
    espSerial.println("\"");
    waitForResponse("OK",10000);

    espSerial.print("AT+CIPSTA=\"");
    espSerial.print(serverIP);
    espSerial.println("\"");   
    waitForResponse("OK",5000);

    espSerial.println("AT+CIPSTA?");
    waitForResponse("OK",3000); 
    
    espSerial.println("AT+CIFSR");           // get ip address
    waitForResponse("OK",1000);
    
    espSerial.println("AT+CIPMUX=1");         // configure for multiple connections   
    waitForResponse("OK",1000);
    
    espSerial.print("AT+CIPSERVER=1,");
    espSerial.println(port);
    waitForResponse("OK",1000);
   
}


//============================================================== setup
//==============================================================
void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);       // You have to set the baud rate of the ESP8266 to 9600 before 
  
  connectToWiFiNetwork();      //enable this line to connect the module your wifi network 

  Wire.begin();
  Wire.beginTransmission(0x27); //Your LCD Address
  lcd.begin(16, 2); // initialize the lcd
  lcd.setBacklight(255);
  
  pinMode(LED_PIN,OUTPUT);
  pinMode(PUMP_PIN,OUTPUT);
  pinMode(FAN_PIN,OUTPUT);
   
  pinMode(DHT_PIN,INPUT);
  initState();
   //turn on light
  
   
  }

//============================================================== onCommandReceived
//==============================================================
/* This function is called every time VirtuinoSE app sends a request to server 
 * The 'variableIndex' is the V memory of Virtuino app
 * The 'valueAsText' is the value that has sent from the app
 * If the 'valueAsText" is ? you have to return the value that you want to be displayed on Virtuino panel
 */
 
String onCommandReceived(int variableIndex, String valueAsText){
  if (debug) Serial.println("New command: variableIndex="+String(variableIndex)+"   valueAsText="+valueAsText);
   
  //--------   enable or disable the LED
  if (variableIndex==22){                     // in this example Virtuino V13 memory contains the state of pin 13
      if (valueAsText=="?") {
        return digitalRead(LED_PIN) == HIGH ? String(0):String(1) ;       // return the state of pin 13 as text
      }
       else if (valueAsText=="0") digitalWrite(LED_PIN,HIGH);          // change the state of pin 13 as text
        else if (valueAsText=="1") digitalWrite(LED_PIN,LOW);
   }
    //--------   enable or disable the PUMP
   if (variableIndex==23){                     // in this example Virtuino V13 memory contains the state of pin 13
      if (valueAsText=="?") {
        return digitalRead(PUMP_PIN) == HIGH ? String(0):String(1) ;      
      }
       else if (valueAsText=="0") digitalWrite(PUMP_PIN,HIGH);          
        else if (valueAsText=="1") digitalWrite(PUMP_PIN,LOW);
   }
    //--------   enable or disable the FAN
   if (variableIndex==24){                     // in this example Virtuino V13 memory contains the state of pin 13
      if (valueAsText=="?") {
        return digitalRead(FAN_PIN) == HIGH ? String(0):String(1) ;      // return the state of pin 13 as text
      }
       else if (valueAsText=="0") digitalWrite(FAN_PIN,HIGH);          // change the state of pin 13 as text
        else if (valueAsText=="1") digitalWrite(FAN_PIN,LOW);
   }

   //--------read temperature
  if (variableIndex==20){
      if (valueAsText=="?") {
        return String(temp);       
      }
   }
   //--------read humidity
   if (variableIndex==21){
      if (valueAsText=="?") {
        return String(hum);       
      }
   }
   //--------read water
   if (variableIndex==25){
      if (valueAsText=="?") {
        return String(water);       
      }
   }
 
  return "";
}


//============================================================== loop
//==============================================================
void loop() {
  virtuinoRun();
  //--- enter your loop code below
  if (millis()-timeStrored>5000) {       // read sensor value every 5 seconds. Avoid to use delay on your code
      water = analogRead(A0);       // copy the A0 value V4
      Serial.println("Water level:"+ String(water));
      int readData = DHT.read11(DHT_PIN); 
      if (DHT.temperature > 0 && DHT.humidity){
        temp = float(DHT.temperature); 
        hum = float(DHT.humidity);
        if(displayPage == 0){
          displayTemp();
          displayPage = 1;
        }else{
          displayWater();
          displayPage = 0;
        }
        
      }
      
      
      timeStrored=millis();               // On Virtuino panel add value displays or instrument  to virtual memories V2,V3,V4
  }
  
 

}


void initState(){
  if(LED_STATE==1){
    digitalWrite(LED_PIN,LOW);
    Serial.println("Turning led on");
  }
  else{
    digitalWrite(LED_PIN,HIGH);
    Serial.println("Turning led off");
  }
  if(FAN_STATE==1){
    digitalWrite(FAN_PIN,LOW);
    Serial.println("Turning fan on");
  }
  else{
    digitalWrite(FAN_PIN,HIGH);
    Serial.println("Turning fan off");
  }
  if(PUMP_STATE==1){
    digitalWrite(PUMP_PIN,LOW);
    Serial.println("Turning pump on");
  }
  else{
    digitalWrite(PUMP_PIN,HIGH);
    Serial.println("Turning pump off");
  }
  
}

void displayWater(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water level: ");
  lcd.print(water); //Displays the integer bits of temperature;
}
void displayTemp(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp :   ");
  lcd.print(temp); //Displays the integer bits of temperature;
  lcd.print((char)223);
  lcd.println("C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(hum);
  lcd.println("%");
}


//================== VirtuinoSE Library (included in the sketch)=============
//=============================================================================
//=============================================================================

/* Only the next two functions are nedded to communicate with Virtuino 
 * Avoid to make changes to the code below 
 */


#define WRONG_KEY_MESSAGE  "Wrong Key"  
#define WELLCOME_MESSAGE  "Hello Virtuino"  

//============================================================== virtuinoRun
//==============================================================
void virtuinoRun(){
     if(espSerial.available()){
       delay(1);
       String espBuffer = espSerial.readStringUntil('\r');
          if (debug) Serial.print(espBuffer);
          int pos = espBuffer.indexOf("+IPD,");
          if (pos>=0){                                        // check for GET command from Virtuino app
              clearESP_buffer(100);                           // can be higher than 100
              int connectionId = espBuffer.charAt(pos+5)-48;  // get connection ID
              pos = espBuffer.indexOf("GET /");
              if (pos!=-1){                                   // We have a GET message
                   espBuffer.remove(0,pos+5);                 // Clear the characters GET / 
                   int k= espBuffer.indexOf(" HTTP/");       
                   if (k>0) espBuffer.remove(k);                       // Clear the tail of the line
                   String responseData = getResponseData(&espBuffer);  // The espBuffer contains the Virtuino data
                                                                       
                    // Create HTTP Header and data
                    String httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: ";
                    httpResponse += responseData.length();
                    httpResponse += "\r\nConnection: close\r\n\r\n";
                    httpResponse += responseData;
                    httpResponse += (" ");
                    //---Send response to Virtuino SE
                    String cipSend = "AT+CIPSEND=";
                    cipSend += connectionId;
                    cipSend += ",";
                    cipSend += httpResponse.length();
                    cipSend += "\r\n";
                    for (int i=0;i<cipSend.length();i++) espSerial.write(cipSend.charAt(i));
                    if (waitForResponse(">",2000)) {
                        for (int i=0;i<httpResponse.length();i++) {
                            espSerial.write(httpResponse.charAt(i));
                            if (debug) Serial.print(httpResponse.charAt(i));
                        }
                    }
              }
          }
     }

}

//============================================================== prepareResponse
//==============================================================
String getResponseData(String* commands){
   
   //---check the KEY
   #ifdef VIRTUINO_KEY 
      String virtuinoKey="";
      int keyPosStart = commands->indexOf("&KEY=");
      if (keyPosStart!=-1) {
        int keyPosEnd = commands->indexOf('&',keyPosStart+5);
        if (keyPosEnd!=-1) virtuinoKey= commands->substring(keyPosStart+5,keyPosEnd);
        else virtuinoKey= commands->substring(keyPosStart);
      }
      if (!virtuinoKey.equals(VIRTUINO_KEY)) return WRONG_KEY_MESSAGE;
   #endif
   
  //--- read all virtuino commands from request data
  String commandResponse="";
  boolean checkNext=true;
  int cStart=-1;
  while (checkNext){
    cStart=commands->indexOf('&',cStart+1);
    if (cStart!=-1) {
      int cEnd=commands->indexOf('&',cStart+1);
      String oneCommand="";
      if (cEnd<0) oneCommand=commands->substring(cStart+1); 
      else oneCommand=commands->substring(cStart+1,cEnd); 
      if (debug) Serial.println("cmd: "+oneCommand);
      char commandType = oneCommand.charAt(0);
      if (commandType=='C') return "&CNT="+String(WELLCOME_MESSAGE);               // Virtuino sends the command &C=? to check the connection status   
      else if ((commandType=='V') || (commandType=='T')) {
        int equalCharPosition = oneCommand.indexOf('=');
        int variableIndex  = oneCommand.substring(1,equalCharPosition).toInt(); 
        String valueString = oneCommand.substring(equalCharPosition+1); 
        String returnedValue = onCommandReceived(variableIndex,valueString);  
       if (returnedValue.length()>0) {
          commandResponse+='&';
          commandResponse+=commandType;
          commandResponse+=String(variableIndex);
          commandResponse+='=';
          commandResponse+=returnedValue;
        }
      }
    }
    else checkNext=false;   // the last command has checked
  }
  if (debug) Serial.println("Response="+commandResponse );
   if (commandResponse.length()==0) commandResponse="OK";       
  return commandResponse;
}

//======================================================================================== clearESP_buffer
//========================================================================================
void clearESP_buffer(int timeout){
    long t=millis();
    char c;
    while (t+timeout>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (debug) Serial.print(c);
      }
    }
   }

//======================================================================================== waitForResponse
//========================================================================================
boolean waitForResponse(String target1,  int timeout){
    String data;
    char a;
    unsigned long start = millis();
    boolean rValue=false;
    while (millis() - start < timeout) {
        while(espSerial.available() > 0) {
            a = espSerial.read();
            if (debug) Serial.print(a);
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            rValue=true;
            break;
        } 
    }
    return rValue;
}