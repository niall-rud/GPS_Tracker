// Base workspace gps data saved to array
 #include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <String.h>
SoftwareSerial mySerial(10, 11); // TX to 10 RX to 11
SoftwareSerial gprsSerial(7,8);
TinyGPS gps;

int motionLED=5;
int GPS_LED=4;
int GSM_LED=3;
  float coordinates[2]={};
  int gpsData[6]={};
void printFloat(double f, int digits = 2);
void printFloatSMS(double f, int digits=2);
int check=0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

Serial.println("Motion Check begin");
pinMode(motionLED, OUTPUT);
pinMode(GPS_LED, OUTPUT);
pinMode(GSM_LED, OUTPUT);
}

int GSMcheck=0;

void loop() {
  Serial.println("Checking Motion");
  check =  DetectMotion(check, GSMcheck);
  Serial.println("Motion Detected");
  if (check>0){
    digitalWrite(motionLED, HIGH);
  mySerial.begin(9600);
  bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update
  while (millis() - start < 5000) {
    if (mySerial.available()) {
      char c = mySerial.read();
      // Serial.print(c);  // uncomment to see raw GPS data
      if (gps.encode(c)) {
            newdata = true;
            // break;  // uncomment to print new data immediately!
          }
        }
      }
    if (newdata){
    GetGPS(coordinates,gpsData);
    
    if (coordinates[0]==0)
    {
    {
      Serial.println("In while");
      GetGPS(coordinates, gpsData);
    Serial.println("Coordinates:");Serial.print(coordinates[0]);
    }
    }
    digitalWrite(GPS_LED, HIGH);
    Serial.print("Latitude:\t");printFloat(coordinates[0],5);Serial.println();
    Serial.print("Longitude:\t");printFloat(coordinates[1],5);Serial.println();
     }
     for (int i=0; i<3;i++)
     {
    
      Serial.println(gpsData[i]);
     }
     mySerial.end();
    GSMcheck=SendTextMessage(coordinates, gpsData, check);
    digitalWrite(GSM_LED, HIGH);
    delay(5000);
    }
lightsOff();
delay(20000);
flash();
}


void GetGPS(float coordinates[],int gpsData[]){
  
//  int gpsData[6]={};
 // float coordinates[2]={};
    float flat,flon;
  int year;
  byte month, day, hour, minute, second;
  gps.f_get_position(&flat, &flon);
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second);
   day=static_cast<int>(day);
   month=static_cast<int>(month);
   year=static_cast<int>(year);
   hour=static_cast<int>(hour);
   minute=static_cast<int>(minute);
   second=static_cast<int>(second);
//   float coordinates[2]={flat, flon};
 // gpsData[0]=flat; gpsData[1]=flon; 
 // gpsData[0]=day; gpsData[1]=month; gpsData[2]=year;
 gpsData[0]=hour; gpsData[1]=minute;
   gpsData[2]=second;
   coordinates[0]=flat;coordinates[1]=flon;
  //, flon, day, month, year, hour, minute, second};
  // may need to redeclare here check later "float gpsData[8]= ..."
int  GSMcheck=2;
  return ;//flat;
  }

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}

int DetectMotion (int check, int GSMcheck)
{

  const int xpin=A0;
  const int ypin=A1;
  const int zpin=A2;
  const int tolerance=30;

char x_ref[2]={};char y_ref[2]={};char z_ref[2]={};

  for (int i=0; i<2;i++)
  {
  x_ref[i]=analogRead(xpin);
  y_ref[i]=analogRead(ypin);
  z_ref[i]=analogRead(zpin);
  delay(500);
  }
int j=1;
//Serial.print(x_ref[j]);Serial.print(x_ref[j+1]);
  if (abs(x_ref[j-1]-x_ref[j]) > tolerance)
  {
    check=1;
  }
   else if (abs(y_ref[j-1]-y_ref[j]) > tolerance)
  {
      check=2;
  }
   else if (abs(z_ref[j-1]-z_ref[j]) > tolerance)
  {
      check=3;
  }
  else
  {
      check=0;
  }
  if(GSMcheck==0 && check>0)
  {
    check=5;
    return check;
  }
 else if (GSMcheck > 0 && check>0)
  {
    check=400;
    return check;
    }
else if (GSMcheck>0 && check==0)
{
  check=1000;
  return check;
}
  return check;
}

int SendTextMessage(float coordinates[], int gpsData[], int check)
{
  gprsSerial.begin(19200);
  if (check<6)
  {

//might actually set the gsm pin high here so to save battery
  Serial.println("Sending Text...");
  gprsSerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  // send sms message, the phone number needs to include the country code e.g. if a U.S. phone number such as (540) 898-5543 then the string must be:
  // +15408985543
  gprsSerial.println("AT+CMGS = \"+353857602302\"");
  delay(100);
  gprsSerial.println("Your bicycle has been moved. \nThese are its current coordinates."); //the content of the message*
 gprsSerial.print("Longitude:");printFloatSMS(coordinates[0], 5);gprsSerial.println("");
  gprsSerial.print("Lattitude:");
 printFloatSMS(coordinates[1], 5);gprsSerial.println("");
  gprsSerial.print("At this time:");
  gprsSerial.print(gpsData[0]); gprsSerial.print(":");
   gprsSerial.print(gpsData[1]);gprsSerial.print(":");
   gprsSerial.println(gpsData[2]);gprsSerial.println(""); 
  delay(200);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
  delay(100);
  GSMcheck=10;
  }
  else if( check>5 && check<500){

//might actually set the gsm pin high here so to save battery
  Serial.println("Sending Text...");
  gprsSerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  // send sms message, the phone number needs to include the country code e.g. if a U.S. phone number such as (540) 898-5543 then the string must be:
  // +15408985543
  gprsSerial.println("AT+CMGS = \"+353857602302\"");
  delay(100);
  gprsSerial.println("Your bicycle is still moving."); //the content of the message*
 gprsSerial.print("Longitude:");printFloatSMS(coordinates[0], 5);gprsSerial.println("");
  gprsSerial.print("Lattitude:");
 printFloatSMS(coordinates[1], 5);gprsSerial.println("");
  gprsSerial.print("Time:");
  gprsSerial.print(gpsData[0]); gprsSerial.print(":");
   gprsSerial.print(gpsData[1]);gprsSerial.print(":");
   gprsSerial.println(gpsData[2]);gprsSerial.println(""); 
  delay(200);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
  delay(100);
  GSMcheck=10;
  }
  
   else 
  {
     Serial.println("Sending Text...");
  gprsSerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  // send sms message, the phone number needs to include the country code e.g. if a U.S. phone number such as (540) 898-5543 then the string must be:
  // +15408985543
  gprsSerial.println("AT+CMGS = \"+353857602302\"");
  delay(100);
  gprsSerial.println("Your bicycle has stopped moving"); //the content of the message*
 gprsSerial.print("Longitude:");printFloatSMS(coordinates[0], 5);gprsSerial.println("");
  gprsSerial.print("Lattitude:");
 printFloatSMS(coordinates[1], 5);gprsSerial.println("");
  gprsSerial.print("At this time:");
  gprsSerial.print(gpsData[0]); gprsSerial.print(":");
   gprsSerial.print(gpsData[1]);gprsSerial.print(":");
   gprsSerial.println(gpsData[2]);gprsSerial.println(""); 
  delay(200);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
  delay(100);
  GSMcheck=0;
    flash();
  }
   if (gprsSerial.available()){ // if the shield has something to say
    Serial.write(gprsSerial.read()); // display the output of the shield
  }
  gprsSerial.end();
 digitalWrite(4, HIGH);
  return GSMcheck;
}

void printFloatSMS(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) {
     gprsSerial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  gprsSerial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    gprsSerial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    int toPrint = int(remainder);
    gprsSerial.print(toPrint);
    remainder -= toPrint;
  }
}
void lightsOff()
{
  digitalWrite(3,LOW);
    digitalWrite(4,LOW);
      digitalWrite(5,LOW);
}
void flash()
{
    digitalWrite(3,HIGH);
    digitalWrite(4,HIGH);
    digitalWrite(5,HIGH);
    delay(500);
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);
     digitalWrite(3,HIGH);
    digitalWrite(4,HIGH);
    digitalWrite(5,HIGH);
    delay(500);
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);
}

