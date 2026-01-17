void ReadPersistantDataFromSPIFFS()
{
    File jsonFile ;
  
/*
   {
      "TotalDistance": 100000.1,
      "TripDuration"  : 4000,
      LogSequence"  :1111
   }
*/

   const size_t capacity = JSON_OBJECT_SIZE(1) + 40;

   DynamicJsonDocument doc(capacity);
   DEBUG_PRINTF("Reading Persistant Data File\n");
   jsonFile = LittleFS.open(JSON_PERSISTANT_FILE_NAME, FILE_READ);
  
   if (jsonFile == NULL)
   {     
     gBikeDistance = 0  ;
     gtripDuration  = 0 ;
     DEBUG_PRINTF("Unable to open %s, default values assumed",JSON_PERSISTANT_FILE_NAME);
     return ;
   }

   deserializeJson(doc, jsonFile);

   float TotalDistance = doc["TotalDistance"]; 
   int TripDuration    = doc["TripDuration"]; // 4000
   int LogSequenceNo   = doc["RecordSequence"]; // 1111

   // Convert Km to meters as our distance calculation is in meters.
   gBikeDistance= TotalDistance ;
   
   gBikeDistanceInMeters = TotalDistance *1000.0 ;
   
   gtripDuration  = TripDuration ;
   gRecordFileSequenceNo  = LogSequenceNo ;
   gRecordFileSequenceNo++ ; 
   DEBUG_PRINTF("Reading Persistant Data File Done\n");
   jsonFile.close();
   return ;
}

void WritePersistantDataToSPIFFS()
{
    File jsonFile ; 
    StaticJsonDocument<256> doc;
    char JsonStr[250];
    //DEBUG_PRINTF("WritePersistantDataToSPIFFS done \n");

    jsonFile = LittleFS.open(JSON_PERSISTANT_FILE_NAME, FILE_WRITE);
    
    if (jsonFile == NULL)
    {
      DEBUG_PRINTF("Unable to open %s",JSON_PERSISTANT_FILE_NAME);
      return ;
    }
    
    doc["TotalDistance"] = gBikeDistance ;
    doc["TripDuration"] = gtripDuration ;

    serializeJson(doc, JsonStr);
    jsonFile.printf("%s\n", JsonStr);
    //DEBUG_PRINTF("WritePersistantDataToSPIFFS done \n");
    jsonFile.close();
    return ;
}



void CreateRecordFile()
{
  File    recordFile ;
  int     len;  
  char    currentDateTime[30] ;
  char    deviceDate[15];
  char    deviceTime[10];

  //DEBUG_PRINTF("CreateRecordFile: 1. gRTCReady = %d\n", gRTCReady);
  if (gRTCReady == true)
  {
      GetCurrentDate(deviceDate) ;
      //DEBUG_PRINTF("CreateRecordFile: 2.Extracted Time from RTC \n");
      GetCurrentTime(deviceTime) ;
      //DEBUG_PRINTF("CreateRecordFile: 3.Extracted Date from RTC \n");
      sprintf(currentDateTime,"%s_%s",deviceDate,deviceTime);
      
      sprintf(gRecordFileName,"/record_%s.csv",currentDateTime);
  }
  else
  {
     sprintf(gRecordFileName, "/noclock-%d.csv",gRecordFileSequenceNo);
  }

  // Create header for the record file 
  recordFile =  LittleFS.open(gRecordFileName, FILE_WRITE);
  recordFile.printf("Time , RPM , Speed , Distance, Date : %s\n",deviceDate);
  recordFile.close(); 
  
}



void ReadConfigValuesFromSPIFFS()
{
  File jsonFile ;
  const size_t capacity = JSON_OBJECT_SIZE(8) + 240;
  DynamicJsonDocument doc(capacity);

  //const char* json = "{\"ssid1\":\"xxxxxxxxxxxxxxxxxxxx\",\"password1\":\"xxxxxxxxxxxxxxxxxxxx\",\"ssid2\":\"xxxxxxxxxxxxxxxxxxxx\",\"password2\":\"xxxxxxxxxxxxxxxxxxxx\",\"ssid3\":\"xxxxxxxxxxxxxxxxxxxx\",\"password3\":\"xxxxxxxxxxxxxxx\",\"wheelDiameter\":85.99,\"devicename\":\"xxxxxxxxxxxxxx\"}";
  
  jsonFile = LittleFS.open(JSON_CONFIG_FILE_NAME, FILE_READ);
  
  if (jsonFile == NULL)
  {
     DEBUG_PRINTF("Unable to open %s",JSON_CONFIG_FILE_NAME);
     return ;
  }
  
  deserializeJson(doc, jsonFile);
  
  const char* ssid1 = doc["ssid1"]; // "xxxxxxxxxxxxxxxxxxxx"
  const char* password1 = doc["password1"]; // "xxxxxxxxxxxxxxxxxxxx"
  const char* ssid2 = doc["ssid2"]; // "xxxxxxxxxxxxxxxxxxxx"
  const char* password2 = doc["password2"]; // "xxxxxxxxxxxxxxxxxxxx"
  const char* ssid3 = doc["ssid3"]; // "xxxxxxxxxxxxxxxxxxxx"
  const char* password3 = doc["password3"]; // "xxxxxxxxxxxxxxx"
  float wheelDiameter = doc["wheelDiameter"]; // 85.99
  const char* devicename = doc["devicename"]; // "xxxxxxxxxxxxxx"
  jsonFile.close();
    DEBUG_PRINTF("-*--Reading for SPIFF startss \n");
  DEBUG_PRINTF("%s \n", ssid1);
  strcpy(ConfigData.ssid1,ssid1);
  strcpy(ConfigData.password1,password1);
  
  strcpy(ConfigData.ssid2,ssid2);
  strcpy(ConfigData.password2,password2);

  strcpy(ConfigData.ssid3,ssid3);
  strcpy(ConfigData.password3,password3);
  DEBUG_PRINTF("%s \n", ssid3);
  ConfigData.wheelCirumference = (3.14 * wheelDiameter)/100 ;// in meters
  DEBUG_PRINTF("%s \n", devicename);
  strcpy(ConfigData.wifiDeviceName,devicename);

//TODO Items for config file 
  //strcpy(ConfigData.mqttServer, "platform.i2otlabs.com");
                                
  //ConfigData.mqttPort = 1883 ;
  
  strcpy(ConfigData.userName,"girish_kumar"); // TODO Read from JSON file
  //sprintf(ConfigData.dataRequestTopic,"MASTER\/WORKOUTDATA\/%s",ConfigData.userName);
  //strcpy(ConfigData.mqttUser,"dataone");
  //strcpy(ConfigData.mqttPassword,"onedata");
  ConfigData.TrainerMode = true ;
}

void WriteToRecordFile(int cadence, struct speedDistance SpeedAndDistance, GPSDataStructure GPSData)
{
  int     Power ;
  File    recordFile ;
  
  String  currentTime,formattedDate ;
  char    deviceTime[20] ;
  int     len ;
  //DEBUG_PRINTF("WriteToRecordFile: filename = ,%s\n",gRecordFileName);
  if (LittleFS.exists(gRecordFileName))
     recordFile =  LittleFS.open(gRecordFileName, FILE_APPEND);
  else
     recordFile =  LittleFS.open(gRecordFileName, FILE_WRITE);
  //DEBUG_PRINTF("WriteToRecordFile: 1\n");
  if (recordFile == NULL)
  {
    DEBUG_PRINTF("Unable to open record file,%s\n",gRecordFileName);
    return ;
  }
  //DEBUG_PRINTF("WriteToRecordFile: 2\n");
  //Power = ComputePower(SpeedAndDistance.Speed) ;
  //DEBUG_PRINTF("WriteToRecordFile: 3 \n");
  //DEBUG_PRINTF("WriteToRecordFile gWifiConnection = %d \n",gWifiConnection);
  
  if (gWifiConnection == true)
  {
     //currentTime = timeClient.getFormattedTime() ; 
     //len = currentTime.length() ;
     //currentTime.toCharArray(deviceTime,len+1) ;
     GetCurrentTime(deviceTime) ;
     //DEBUG_PRINTF("WriteToRecordFile: Writing to record file %s\n", deviceTime) ;
     DEBUG_PRINTF("WriteToLogFile: Speed = %0.1f, Distance = %0.1f\n",SpeedAndDistance.Speed,SpeedAndDistance.distanceKM);
     recordFile.printf("%s,%d,%0.1f,%0.1f\n",
                             deviceTime,cadence,SpeedAndDistance.Speed,
                             SpeedAndDistance.distanceKM);
  }  
  else
  {
  
     recordFile.printf("%d,%d,%f,%f,%f,%d,%f,%d,%d,%d,%f\n",
                        timeCounter,cadence,
                        GPSData.Latitude,
                        GPSData.Longitude,
                        SpeedAndDistance.distanceKM,
                        GPSData.speedInKmH,
                        GPSData.noOfSatellites,
                        GPSData.altitudeInMeters,
                        GPSData.hours,
                        GPSData.minutes,
                        GPSData.seconds,
                        Power);
     timeCounter++ ;
  }
  WritePersistantDataToSPIFFS();                
  recordFile.flush();
  recordFile.close();
}

void DisplayConfigValues()
{
   DEBUG_PRINTF("ssid1 %s \n",ConfigData.ssid1);
   DEBUG_PRINTF("Password %s \n", ConfigData.password1);

   DEBUG_PRINTF("ssid2 %s \n",ConfigData.ssid2);
   DEBUG_PRINTF("Password2 %s \n", ConfigData.password2);

   DEBUG_PRINTF("ssid3 %s \n",ConfigData.ssid3);
   DEBUG_PRINTF("Password3 %s \n", ConfigData.password3);

   DEBUG_PRINTF("Wheel Circumference = %f\n", ConfigData.wheelCirumference);
   DEBUG_PRINTF("Device name = %s ", ConfigData.wifiDeviceName);
}
