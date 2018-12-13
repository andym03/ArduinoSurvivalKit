#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include <EEPROM.h>

#define UPDATE_TIME 10000

SoftwareSerial gpsSerial(11, 10); //RX=pin 10, TX=pin 11
SoftwareSerial bluetoothSerial(18, 19); // RX, TX
int BluetoothData; // the data given from Computer
int address = 0;
unsigned long timer;
double dummyCount = 0;

double latitudes[569] = {0};
double longitudes[569] = {0};

TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data
void setup()
{
  Serial.begin(9600);//This opens up communications to the Serial monitor in the Arduino IDE
  Serial1.begin(9600); //Serial1 for bluetooth communication
  gpsSerial.begin(9600);//This opens up communications to the GPS
  bluetoothSerial.begin(9600);
  initStorage();
  timer = millis();
}

void loop()
{
  bluetoothLoop();
  //gpsLoop();
  writeGps(12.1234 + dummyCount, 12.1234 + dummyCount);
  dummyCount++;
  //delay(1000);
}

void bluetoothLoop() {
  printStorage();
  delay(500);
}

void gpsLoop() {
  while(gpsSerial.available())//While there are characters to come from the GPS
  {
    gps.encode(gpsSerial.read());//This feeds the serial NMEA data into the library one char at a time
  }
  if(gps.location.isUpdated())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {
      if(millis()-timer > UPDATE_TIME)  //check if it's time to do a temp sensor sample
        {
          writeGps(gps.location.lat(), gps.location.lng());
          timer = millis();
          Serial.println("Wrote GPS Data");
        }
    //Get the latest info from the gps object which it derived from the data sent by the GPS unit
    Serial.println("Satellite Count:");
    Serial.println(gps.satellites.value());
    Serial.println("Latitude:");
    Serial.println(gps.location.lat(), 6);
    Serial.println("Longitude:");
    Serial.println(gps.location.lng(), 6);
    Serial.println("Speed MPH:");
    Serial.println(gps.speed.mph());
    Serial.println("Altitude Feet:");
    Serial.println(gps.altitude.feet());
    Serial.println("");
    Serial1.println("test");
  }
}

void printStorage()
{
  Serial1.println("SENDING GPS"); 
  for (int i = 0 ; i < address ; i++) {
      Serial1.println(latitudes[i]);
      delay(100);                 
      Serial1.println(longitudes[i]);  
  }
  Serial1.println("FINISHED GPS"); 
  if (address == 569) {
      for (int i = 0 ; i < 569 ; i++) {
        latitudes[i] = 0;                      //write 0 to address i
        longitudes[i] = 0;    
      }
    Serial.println("EEPROM erased");
    address = 0;                             //reset address counter
  }
}

void initStorage()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    if(EEPROM.read(i) != 0)                     //skip already "empty" addresses
    {
      EEPROM.write(i, 0);                       //write 0 to address i
    }
  }
  Serial.println("EEPROM erased");
  address = 0;                                  //reset address counter
}

void writeGps(double latitude, double longitude)
{
  Serial.print("Count ");
  Serial.print(address);
  Serial.print("\n");
  if (address == 569) {
    printStorage();
  }
  latitudes[address] = latitude;
  longitudes[address] = longitude;
  address++;

}
/*
void writeGps(double latitude, double longitude)
{
  EEPROM.put(address, latitude);         //write value to current address counter address
  EEPROM.put(address, longitude);        //write value to current address counter address
 
  Serial.print("Sensor value stored at address ");
  Serial.println(address);

  address += 2*sizeof(double);
  if(address == EEPROM.length())  //check if address counter has reached the end of EEPROM
  {
    address = 0;              //if yes: reset address counter
  }
}*/
