#include <Wire.h>
#include <GSM.h>
#include <ArduinoHttpClient.h>
#include<TimeLib.h>

int Led1=13; //Led for the insect trap
int Pump1=12;//Pumping motor for the solar water heater
int Pump2=11;//Pumping motor for the hot water supply of the nest
int Pump3=10;//Pumping motor for the re supply of used water
int Pump4=7;//Pumping motor feeds
int pump5=4;
int drinker=9;//Control for the water supply to the drinker
int Tempsensor=A0;//nest temperature sensor
int LDR=A1;// for the Light sensitivity
int IR1=8;//Sensing the level of feeds;
const int targetHour = 7;
// Define server details

const char* serverAddress = "http://example.com"; // Replace with your server URL
const int serverPort = 80;

// Initialize GSM
GSM gsmAccess(true);
GSMScanner scannerNetworks;
GSMModem modem;

 //Initialize HttpClient
HttpClient client = HttpClient(modem, serverAddress,serverPort);

void setup() {
  // put your setup code here, to run once:
pinMode(Led1,OUTPUT);
pinMode(Pump1,OUTPUT);
pinMode(Pump2,OUTPUT);
pinMode(Pump4,OUTPUT)
pinMode(Pump3,OUTPUT);
pinMode(drinker,OUTPUT);
Serial.begin(9600);
pinMode(LDR, INPUT);
pinMode(IR1, INPUT);
pinMode(Tempsensor, INPUT);

  // Start GSM connection
connectGSM();
 
 
}

void loop() {
  //temperature sensing
int tempsense=analogRead(Tempsensor);
float temperature = (((tempsense/1023.0)*5.0)-0.5)*100.0;
if(temperature<=18)
{
digitalWrite(Pump2,HIGH);
delay(700);
 
digitalWrite(Pump2,LOW);
delay(700);}
else
{
if(temperature>=24)
{
  digitalWrite(Pump4,HIGH);
delay(700);
   digitalWrite(Pump4,LOW);
delay(700);
}}
 int threshold1 = 400;
//LDR Light intensity sensing
int LDRsense=analogRead(LDR);

if(LDRsense>threshold1 )
{
  digitalWrite(Pump1,HIGH);
delay(1000);
digitalWrite(Pump1,LOW);
delay(20000);
if(LDRsense<threshold1)
{
  digitalWrite(Pump1,HIGH);
delay(10000);
digitalWrite(Pump1,LOW);
delay(1000);
}
}

//Feeds Level

int IRsense=digitalRead(IR1);

if(IRsense==HIGH)
{
  int sensorData=20;
sendDataToInternet(sensorData);
delay(1000);
}

// Check the time and switch the feeds
  checkAndSwitchfeeds();
  // Wait for some time before the next iteration
  delay(60000); // Delay for 1 minute
}

void connectGSM() {
  Serial.println("Connecting to GSM...");

  if (gsmAccess.begin() == GSM_READY) {
    Serial.println("GSM connected");
  } else {
    Serial.println("GSM connection failed");
    while (true); // Stop here if GSM connection fails
  }
}

void checkAndSwitchfeeds() {
  int currentHour = hour();

  if (currentHour == targetHour) {
    digitalWrite(Pump4, HIGH); // Switch on feeds motor
    delay(1000);
  } else {
    digitalWrite(Pump4, LOW); // Switch off feeds motor
      delay(1000);
  }
}
  
void sendDataToInternet(float sensorData)
{
  Serial.println("Sending data to the internet");

  // Construct the data to be sent
  String postData = "sensor=" + String(sensorData);
  // Send HTTP POST request
  client.beginRequest();
  client.post("/path/to/your/api"); // Replace with the actual path to your API
  client.sendHeader("Content-Type", "application/x-www-form-urlencoded");
  client.sendHeader("Content-Length", postData.length());
  client.sendHeader("Connection", "close");
  client.beginBody();
  client.print(postData);
  client.endRequest();

  // Read and print the server response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("HTTP Status Code: ");
  Serial.println(statusCode);
  Serial.print("Server Response: ");
  Serial.println(response);
}

