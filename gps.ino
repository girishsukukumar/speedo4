void AdjustTime(int *Hr, int *Minutes, int *Sec)
{
  int seconds ;
  int hours ;
  int minutes ;
  int balance ;

  seconds = *Hr *60 * 60  + *Minutes * 60  + *Sec ;
  seconds = seconds + 19800 ; //GMT correction
  hours =  seconds/3600 ;
  balance =  seconds % 3600 ;
  minutes = balance /  60 ;
  balance = balance % 60 ;
  seconds = balance ; 
  

  *Hr = hours ;
  *Minutes = minutes ;
  *Sec = seconds ;
  return; 
}
void SetUPConnectionToGPS()
{

   // Serial0 is for Serial.println() so we cannot use
   // Serial1 is for GSM module 
   // So we are using Serial2 for GPS

   //--------------------- IMPORTANT 20-Feb-2025 ---------------------
   // Defining the PINS for gpsSerial which Serial2
   //RX_ GPIO = 2
   //TX_GPIO = 4
  
   //TX of GPS is connected to RX which is GPIO 2 of ESP32
   //RX of GPS is connected to TX which is GPIO 4 of ESP32
   // Start Serial 2 with the defined RX and TX pins and a baud rate of 9600
   gpsSerial.begin(GPS_BAUD, SERIAL_8N1, 2, 4);
   Serial.println("Serial 2 started at 9600 baud rate");
   //-----------------------------------------------------

} 

void InitGPSDataStruct(GPSDataStructure *gpsData)
{
  gpsData->Latitude = 0.0;
  gpsData->Longitude =0.0;
  gpsData->hours = 0 ;
  gpsData->minutes = 0;
  gpsData->seconds = 0 ;
  gpsData->altitudeInMeters = 0.0 ;
  gpsData->speedInKmH =0.0 ;
  gpsData->noOfSatellites  = 0;
  gpsData->GPSActiveFlag = false ;
  return;
}
struct GPSDataStructure ReadGPSData()
{
  GPSDataStructure GPSData ;
  
    while (gpsSerial.available() > 0) 
    {
      gps.encode(gpsSerial.read());
    }
   //DisplayData("L");
    if (gps.location.isUpdated()) 
    {
      //DisplayData("Yes-U");
      
      Serial.print("LAT: ");
      Serial.println(gps.location.lat(), 6);
      GPSData.Latitude = gps.location.lat() ;
      
      //Serial.print("LONG: "); 
      //Serial.println(gps.location.lng(), 6);
      GPSData.Longitude = gps.location.lng();
      
      //Serial.print("SPEED (km/h) = "); 
      //Serial.println(gps.speed.kmph()); 
      GPSData.speedInKmH = gps.speed.kmph();
      
      //Serial.print("ALT (min)= "); 
      //Serial.println(gps.altitude.meters());
      GPSData.altitudeInMeters = gps.altitude.meters() ;
    
      //Serial.print("HDOP = "); 
      //Serial.println(gps.hdop.value() / 100.0); 
      Serial.print("Satellites = "); 
      Serial.println(gps.satellites.value()); 
      GPSData.noOfSatellites = gps.satellites.value() ;
      Serial.print("Time in UTC: ");
      Serial.println(String(gps.date.year()) + "/" + String(gps.date.month()) + "/" + String(gps.date.day()) + "," + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()));
      
      int hr    = gps.time.hour() ;
      int minutes  = gps.time.minute() ;
      int sec  = gps.time.second() ;
      
      //Serial.printf("%d, %d, %d\n", hr, minutes, sec);
      
      AdjustTime(&hr, &minutes,&sec);
      GPSData.hours    = hr  ;
      GPSData.minutes  = minutes ;
      GPSData.seconds  = sec ;
      GPSData.GPSActiveFlag = true ;
    }
    else
    {

      InitGPSDataStruct(&GPSData);
      //DisplayData("No-U");
    }
    return GPSData ;
}
