void DisplayserverIndex()
{
     File  file ;
     size_t  sent;
 
     if (LittleFS.exists("/serveridx.html"))  
     { 
        file =LittleFS.open("/serveridx.html",  "r");
        sent =webServer.streamFile(file, "text/html");  
        file.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File Sensor.html not found </H1> </HTML>");
     }


}
void ChangeDetails()
{
      File  file ;
     size_t  sent;
 
     if (LittleFS.exists("/config.html"))  
     { 
        file =LittleFS.open("/config.html",  "r");
        sent =webServer.streamFile(file, "text/html");  
        file.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File loginIndex.html not found </H1> </HTML>");
     }
 
}
void FileUpload()
{
     File  file ;
     size_t  sent;
 
     if (LittleFS.exists("/upload.html"))  
     { 
        file =LittleFS.open("/upload.html",  "r");
        sent =webServer.streamFile(file, "text/html");  
        file.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File loginIndex.html not found </H1> </HTML>");
     }
  
}

void RebootDevice()
{
    ESP.restart();
}
void DisplayLoginIndex()
{
//    webServer.sendHeader("Connection", "close");
//    webServer.send(200, "text/html", loginIndex);
     File  file ;
     size_t  sent;
 
     if (LittleFS.exists("/loginIndex.html"))  
     { 
        file =LittleFS.open("/loginIndex.html",  "r");
        sent =webServer.streamFile(file, "text/html");  
        file.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File loginIndex.html not found </H1> </HTML>");
     }

}
void DisplayDashboard()
{
     File  file ;
     size_t  sent;
 
     if (LittleFS.exists("/sensor.html"))  
     { 
        file =LittleFS.open("/sensor.html",  "r");
        sent =webServer.streamFile(file, "text/html");  
        file.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File Sensor.html not found </H1> </HTML>");
     }

}
void RealTimeGraph()
{
     File  file ;
     size_t  sent;
 
     if (LittleFS.exists("/livechart.html"))  
     { 
        file =LittleFS.open("/livechart.html",  "r");
        sent =webServer.streamFile(file, "text/html");  
        file.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File livechart.html not found </H1> </HTML>");
     }
 
}
void DisplayGaugeDisplay()
{
     File  file ;
     size_t  sent;
 
     if (LittleFS.exists("/gauge.html"))  
     { 
        file =LittleFS.open("/gauge.html",  "r");
        sent =webServer.streamFile(file, "text/html");  
        file.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File gauge.html not found </H1> </HTML>");
     }

}
void CopyTemplateToRecordhtml()
{
  File inFile, outFile ;
  inFile =LittleFS.open("/recordhead.html",  "r");
  outFile =LittleFS.open("/records.html",  "w");
  while(inFile.available())
  {
    outFile.write(inFile.read());
  }
  inFile.close();
  outFile.close();
}
void SendFileToBrowser()
{
  File    filePtr ;
  String  selectedRecordFileNameStr, recordFile ;
  char    SelectedFileName[50], header[100];
  int     len, sent;
 
  if (webServer.hasArg("fileName"))
  {
     selectedRecordFileNameStr = webServer.arg("fileName") ;
     len = selectedRecordFileNameStr.length();
     sprintf(SelectedFileName, "\//%s",selectedRecordFileNameStr.c_str());
     //selectedRecordFileNameStr.toCharArray(SelectedFileName,49); 
     //sprintf(SelectedFileName,"%s",SelectedFileName);
     
     if (LittleFS.exists(SelectedFileName))
     { 
      
        filePtr = LittleFS.open(SelectedFileName,  "r");
        
        recordFile = selectedRecordFileNameStr.substring(1); // Removing the / from the beginning of file name
        len = recordFile.length();
        recordFile.toCharArray(SelectedFileName,len+1); 
        sprintf(header,"filename=\"%s\"",SelectedFileName);
        webServer.sendHeader("Content-Disposition",header);
        sent = webServer.streamFile(filePtr, "application/text");  
        filePtr.close();
        webServer.send(200, "text/html", "<HTML> File sent </HTML>");   
     }
     else
     {
        char error[100];
        sprintf(error,"<HTML> <H1> File %s not found </H1> </HTML>",SelectedFileName);
        webServer.sendHeader("Connection", "close");
        webServer.send(200, "text/html", error);
     }
    
  }
  else
  {
        char error[100];
        sprintf(error,"<HTML> <H1> Argument received does not have fileName  </H1> </HTML>");
        webServer.sendHeader("Connection", "close");
        webServer.send(200, "text/html", error);
  }
}
void CopyTopToGraphHtml()
{
  File inFile, outFile ;
  inFile =LittleFS.open("/graph_1.html",  "r");
  outFile =LittleFS.open("/graph.html",  "w");
  while(inFile.available())
  {
    outFile.write(inFile.read());
  }
  inFile.close();
  outFile.close();  
}
void CopyBottomToGraphHtml()
{
  File inFile, outFile ;
  inFile =LittleFS.open("/graph_2.html",  "r");
  outFile =LittleFS.open("/graph.html",  FILE_APPEND);
  while(inFile.available())
  {
    outFile.write(inFile.read());
  }
  inFile.close();
  outFile.close();
  DEBUG_PRINTF("CopyBottomToGraphHtml\n");

}
int WriteValuesToVariables(File readFilePtr, File writeFilePtr,  int field)
{
}
void ShowGraph()
{
    File    recordFilePtr ,graphFilePtr ;
    String  selectedRecordFileNameStr, recordFile ;
    char    SelectedFileName[40], fileNameForScanning[40];
    int     len, sent;
    String  oneLineStr ;
    char    oneLine[35] ; 
    char    timeStamp[10];
    int     rpm ;
    float   Speed ;
    float   distance ;
    int     power ;
    char    lineToBeWritten[40];
    String  LineToBeWrittenStr ;
    //String  currentTime, currentDate,dateStamp;
    char    startTime[20],StartDate[20] ;
    int     splitT;
    int     i,count =0;
    bool    firstLineRead = false ;
    int     Day, Month, Year ;
  if (webServer.hasArg("fileName"))
  {
     selectedRecordFileNameStr = webServer.arg("fileName") ;
     len = selectedRecordFileNameStr.length();
     selectedRecordFileNameStr.toCharArray(SelectedFileName,len+1); 
     
     if (LittleFS.exists(SelectedFileName))
     { 
        strcpy(fileNameForScanning,SelectedFileName);
        len = strlen(fileNameForScanning);
        for(i = 0 ; i < len ; i++)
        {
           if ((fileNameForScanning[i] == '-') || (fileNameForScanning[i] == ':'))
           {
               fileNameForScanning[i] = ' ' ;
           }
           
        }
        sscanf(fileNameForScanning,"/rec %d %d %d %d %d %d.csv",&Year, &Month,&Day, &len,&len,&len);
        
        len = 0 ;
        CopyTopToGraphHtml() ;
        graphFilePtr  = LittleFS.open("/graph.html",  FILE_APPEND);
        if (graphFilePtr == NULL)
        {
          DEBUG_PRINTF("Unable to open graph.html is append mode\n");
        }
        recordFilePtr = LittleFS.open(SelectedFileName,  "r");
        sprintf(lineToBeWritten,"['Time', 'RPM', 'Speed' , 'Power']");
        graphFilePtr.printf("%s",lineToBeWritten);
        
        while(recordFilePtr.available())
        {
          int i ;
          graphFilePtr.write(',');
          graphFilePtr.write('\n');
          oneLineStr = recordFilePtr.readStringUntil('\n'); 
          oneLineStr.replace(',',' '); // Replace comma with space so that sscanf() will work smoothly
          len = oneLineStr.length(); 
          oneLineStr.toCharArray(oneLine,len+1);
          
          i = sscanf(oneLine,"%s %d %f %f %d",timeStamp, &rpm, &Speed, &distance, &power);
          if (firstLineRead == false)
          {
            strcpy(startTime,timeStamp); //The time stamp of the first record show the work out start time
            firstLineRead = true ;
          }

         
          sprintf(lineToBeWritten, "['%s', %d,%f,%d]", timeStamp, rpm, Speed, power);
          
          LineToBeWrittenStr = lineToBeWritten ;
          
          graphFilePtr.printf("%s",lineToBeWritten);

          count++ ;
          
        }
        graphFilePtr.printf("]);\n");
        graphFilePtr.printf("var options = {\n");
        graphFilePtr.printf("title: 'Date:%d-%d-%d, Time of start: %s, Duration %d minutes, Distance %.2f Km',\n",Day,Month,Year,startTime, count,distance);
        
        recordFilePtr.close();
        graphFilePtr.close();
        CopyBottomToGraphHtml();
        
        graphFilePtr  = LittleFS.open("/graph.html",  "r");
        sent = webServer.streamFile(graphFilePtr, "text/html");          
        graphFilePtr.close();
     }
     else
     {
        char error[100];
        sprintf(error,"<HTML> <H1> File %s not found </H1> </HTML>",SelectedFileName);
        webServer.sendHeader("Connection", "close");
        webServer.send(200, "text/html", error);
     }
  }
  else
  {
        char error[100];
        sprintf(error,"<HTML> <H1> Argument received does not have fileName  </H1> </HTML>");
        webServer.sendHeader("Connection", "close");
        webServer.send(200, "text/html", error);
  }
  
}
int CountNoOfRecords(String fileNameString)
{
  File csvFile ;
  char fileName[40];
  int  count = 0 ;
  sprintf(fileName, "/%s",fileNameString.c_str());
  //DEBUG_PRINTF("DEBUG: CountNoOfRecords function entry ->%s\n",fileName);
  
  csvFile =LittleFS.open(fileName,  FILE_READ);
  //DEBUG_PRINTF("csvFile.available %d \n",csvFile.available());
  while(csvFile.available())
  {
      //DEBUG_PRINTF("Count = %d\n",count);
     csvFile.readStringUntil('\n'); 
     count++ ;
  }
  csvFile.close() ;
  //DEBUG_PRINTF("DEBUG: CountNoOfRecords function exit\n");
  return count ;
}

void DisplayRecordsForAnalysis()
{
     File    htmlFile, rootDir,recordPtr ;
     size_t  sent;
     char    radioButtonTag[100];
     String  fileNameStr ;

     CopyTemplateToRecordhtml();
     rootDir = LittleFS.open("/");
     htmlFile =LittleFS.open("/records.html",  FILE_APPEND);
     htmlFile.print("<H3> <CENTER>Select a file for analysing </H3>\n");
     htmlFile.print("<form  action=\"/ShowGraph\" method=\"POST\">");
     htmlFile.print(" <TABLE border=\"5\"> ");
     htmlFile.print(" <TR><th>Workout Record </th> <th>Workout Duration</th> <th>Selection</th> </TR>");
     recordPtr = rootDir.openNextFile();

     while(recordPtr)
     {
      recordPtr = rootDir.openNextFile();   
      fileNameStr = recordPtr.name();
      if (fileNameStr.endsWith(".csv") == true)
      {
        int noOfRecords;
        htmlFile.print("<TR>");
        htmlFile.print("<TD>");
        htmlFile.print(recordPtr.name());
        htmlFile.print("</TD>");
        noOfRecords = CountNoOfRecords(recordPtr.name());
        htmlFile.print("<TD>");
        htmlFile.print(noOfRecords);
        htmlFile.print(" Minutes");
        htmlFile.print("</TD>");
        htmlFile.print("<TD>");
        sprintf(radioButtonTag, "<input type=\"radio\" id=\"");
        htmlFile.print(radioButtonTag);
        htmlFile.print(recordPtr.name()); 
        htmlFile.print("\""); 
        htmlFile.print(" name=\"fileName\"");
        htmlFile.print(" value=\"");
        htmlFile.print(recordPtr.name());
        htmlFile.print("\">");        
        htmlFile.print("</TD>");
        htmlFile.print("</TR>\n");
      }
      
     }
  
     htmlFile.printf("</TABLE>\n");
     htmlFile.printf("<input type=\"submit\" value=\"Submit\">");
     htmlFile.printf("</form>");
     htmlFile.printf("</CENTER></HTML>\n");
     htmlFile.close();
     
     if (LittleFS.exists("/records.html"))  
     { 
        htmlFile =LittleFS.open("/records.html",  "r");
        sent =webServer.streamFile(htmlFile, "text/html");  
        htmlFile.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File records.htm; not found </H1> </HTML>");
     }
  
}
void ListRecords()
{
     File    htmlFile, rootDir,recordPtr ;
     size_t  sent;
     char    radioButtonTag[100];
     String  fileNameStr ;
     int     noOfRecords ;

     CopyTemplateToRecordhtml();
     rootDir = LittleFS.open("/");
     htmlFile =LittleFS.open("/records.html",  FILE_APPEND);
     htmlFile.print("<H3> <CENTER>Select a work out record to Download </H3>\n");
     htmlFile.print("<form  action=\"/ViewCSVFile\" method=\"POST\">");
     htmlFile.print(" <TABLE border=\"5\"> ");
     htmlFile.print(" <TR><th>Workout Record </th> <th>Workout Duration</th> <th>Selection</th> </TR>");
     recordPtr = rootDir.openNextFile();

     while(recordPtr)
     {
      recordPtr = rootDir.openNextFile();   
      fileNameStr = recordPtr.name();
      if (fileNameStr.endsWith(".csv") == true)
      {
        htmlFile.print("<TR>");
        htmlFile.print("<TD>");
        htmlFile.print(recordPtr.name());
        htmlFile.print("</TD>");
        
        noOfRecords = CountNoOfRecords(recordPtr.name());
        htmlFile.print("<TD>");
        htmlFile.print(noOfRecords);
        htmlFile.print(" Minutes");
        htmlFile.print("</TD>");
                
        htmlFile.print("<TD>");
        sprintf(radioButtonTag, "<input type=\"radio\" id=\"");
        htmlFile.print(radioButtonTag);
        htmlFile.print(recordPtr.name()); 
        htmlFile.print("\""); 
        htmlFile.print(" name=\"fileName\"");
        htmlFile.print(" value=\"");
        htmlFile.print(recordPtr.name());
        htmlFile.print("\">");        
        htmlFile.print("</TD>");
        htmlFile.print("</TR>\n");
      }
      
     }
  

     htmlFile.printf("</TABLE>\n");
     htmlFile.printf("<input type=\"submit\" value=\"Submit\">");
     htmlFile.printf("</form>");
     htmlFile.printf("</CENTER> </HTML>\n");
     htmlFile.close();
     
     if (LittleFS.exists("/records.html"))  
     { 
        htmlFile =LittleFS.open("/records.html",  "r");
        sent =webServer.streamFile(htmlFile, "text/html");  
        htmlFile.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File records.htm; not found </H1> </HTML>");
     }

}
void DeleteRecordFile()
{
  String  selectedRecordFileNameStr ;
  char fileName[40];
  //char message[90];
  if (webServer.hasArg("fileName"))
  {
     selectedRecordFileNameStr = webServer.arg("fileName") ;
     sprintf(fileName, "\/%s",selectedRecordFileNameStr.c_str());
     DEBUG_PRINTF("DeleteRecordFile: File to be deleted %s",fileName);
     if (LittleFS.exists(fileName))
     { 
        LittleFS.remove(fileName);
        webServer.sendHeader("Connection", "close");
        //sprintf(message,"<HTML> <body style=\"background-color:black;\"> <H1> Record %s Deleted </H1></HTML>",fileName);
        webServer.send(200, "text/html", "<HTML> <body style=\"background-color:powderblue;\"> <H1> Record Deleted</H1></HTML>");
     }
  } 
  else
  {
        webServer.sendHeader("Connection", "close");
        //sprintf(message,"<HTML> <body style=\"background-color:black;\"> <H1> Record %s NOT FOUND </H1></HTML>",fileName);
        webServer.send(200, "text/html", "<HTML> <body style=\"background-color:powderblue;\"> <H1> Record NOT FOUND</H1></HTML>");
    
  }
  
}

void DeleteEmptyRecords()
{
     File    htmlFile, rootDir,recordPtr ;
     size_t  sent;
     char    radioButtonTag[100];
     String  fileNameStr ;
     int     noOfRecords ;
     char    fileName[50];
     char    message[100] ;
     File    outFile ;


     int index =0 ;
     int noOfFilesMarkedForDeletion  = 0 ;
     DEBUG_PRINTF("DeleteEmptyRecords Entry\n"); 

     outFile =LittleFS.open("/zerofiles.txt",  FILE_WRITE);
     
     rootDir = LittleFS.open("/");
     recordPtr = rootDir.openNextFile();
     index = 0 ;
     while(recordPtr)
     {
      recordPtr = rootDir.openNextFile();
      
      fileNameStr = recordPtr.name();

      if (fileNameStr.endsWith(".csv") == true)
      {
        noOfRecords = CountNoOfRecords(recordPtr.name());   
        
        if (noOfRecords == 0)
        {
             
             fileNameStr =  recordPtr.name() ;
             sprintf(fileName, "/%s", fileNameStr.c_str());
             //DEBUG_PRINTF("DeleteEmptyRecords %s idenfied \n",fileName); 
             outFile.print(fileName) ;
             outFile.print('\n') ;
             //DEBUG_PRINTF("DeleteEmptyRecords %s added to list at %d \n",fileName,index); 
             index++ ;
             //LittleFS.remove(fileName);   
        }
      }
     }
     outFile.close(); 
     noOfFilesMarkedForDeletion = index ;
  
     recordPtr.close(); 
     //DEBUG_PRINTF("DEBUG: DeleteEmptyRecords : %d files marked for deletion \n ",
     //               noOfFilesMarkedForDeletion);
     outFile =LittleFS.open("/zerofiles.txt",  FILE_READ);
     //DEBUG_PRINTF("DEBUG: DeleteEmptyRecords : 1\n");
     while(outFile.available())
     {
        //DEBUG_PRINTF("DEBUG: DeleteEmptyRecords : 2\n");
        fileNameStr = outFile.readStringUntil('\n');
        //DEBUG_PRINTF("DEBUG: DeleteEmptyRecords : 3\n"); 
        strcpy(fileName,fileNameStr.c_str()) ;
        //DEBUG_PRINTF("DeleteEmptyRecords about to delete : %s\n", fileName);
        LittleFS.remove(fileName);
        //DEBUG_PRINTF("DeleteEmptyRecords :Deleted : %s\n", fileName);
      
      }
      outFile.close() ;
      //DEBUG_PRINTF("DEBUG: DeleteEmptyRecords : 4\n");
     htmlFile =LittleFS.open("/recordDelete.html",  FILE_WRITE);
     sprintf(message,"<HTML> <H1> <CENTER> %d Empty Records deleted </H1> </HTML>\n", noOfFilesMarkedForDeletion) ;
     htmlFile.print(message);
     htmlFile.close();
     if (LittleFS.exists("/recordDelete.html"))  
     { 
        htmlFile =LittleFS.open("/recordDelete.html",  "r");
        sent =webServer.streamFile(htmlFile, "text/html");  
        htmlFile.close();
        LittleFS.remove("/recordDelete.html");
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File recordDelete.html not found </H1> </HTML>");
     }  
     DEBUG_PRINTF("DeleteEmptyRecords: Exit\n"); 
}
void DeleteRecords()
{
     File    htmlFile, rootDir,recordPtr ;
     size_t  sent;
     char    radioButtonTag[100];
     String  fileNameStr ;
     int     noOfRecords ;
     
     CopyTemplateToRecordhtml();
     rootDir = LittleFS.open("/");
     htmlFile =LittleFS.open("/records.html",  FILE_APPEND);
     htmlFile.print("<H3> <CENTER>Select a workout record to Delete, only one deletion at a time</H3>\n");
     htmlFile.print("<TR> <TD> </TD>  <TD> <form  action=\"/DeleteEmptyRecords\" method=\"POST\"> <button type=\"submit\">Delete  Empty Records</button></form></TD></TR>");
     htmlFile.print("<form  action=\"/DeleteCSVFile\" method=\"POST\">");
     htmlFile.print(" <TABLE border=\"5\"> ");
     htmlFile.print(" <TR><th>Workout Record </th> <th>Workout Duration</th> <th>Selection</th> </TR>");
     recordPtr = rootDir.openNextFile();
     while(recordPtr)
     {
      recordPtr = rootDir.openNextFile();
      
      fileNameStr = recordPtr.name();
      if (fileNameStr.endsWith(".csv") == true)
      {
        htmlFile.print("<TR>");
        htmlFile.print("<TD>");
        htmlFile.print(recordPtr.name());
        htmlFile.print("</TD>");

        noOfRecords = CountNoOfRecords(recordPtr.name());        
        htmlFile.print("<TD>");
        htmlFile.print(noOfRecords);
        htmlFile.print(" Minutes");
        htmlFile.print("</TD>");        
        
        htmlFile.print("<TD>");
        sprintf(radioButtonTag, "<input type=\"radio\" id=\"");
        htmlFile.print(radioButtonTag);
        htmlFile.print(recordPtr.name()); 
        htmlFile.print("\""); 
        htmlFile.print(" name=\"fileName\"");
        htmlFile.print(" value=\"");
        htmlFile.print(recordPtr.name());
        htmlFile.print("\">");        
        htmlFile.print("</TD>");
        htmlFile.print("</TR>\n");
      }
      
     }
     
     htmlFile.printf("</TABLE>\n");
     htmlFile.printf("<input type=\"submit\" value=\"Delete Record\">");
     htmlFile.print("</form>");
     htmlFile.printf("</CENTER> </HTML>\n");
     htmlFile.close();
     if (LittleFS.exists("/records.html"))  
     { 
        htmlFile =LittleFS.open("/records.html",  "r");
        sent =webServer.streamFile(htmlFile, "text/html");  
        htmlFile.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File Sensor.html not found </H1> </HTML>");
     } 
}
void PostDetails()
{
  int   hours   = 0 ;
  int   seconds = 0 ;
  int   minutes = 0 ;
  char  durationStr[15]; 
  const size_t capacity = JSON_OBJECT_SIZE(18);
  DynamicJsonDocument doc(capacity);
  static float SpeedData ;
  static int   RPMData ;
  float  gTripDistance  = 0 ;
  float  gDistanceKM    = 0 ;
  int    gTotalDistance = 0 ;

    
  char jsonString[250];
  int gSpeed ;
  float  gRoomTemp      = 0.0 ;
  
  int    gPulseRate     = 0 ;
  float  gBodyTempInCelius = 0.0 ;
  float  gRoomHumidity  = 0.0 ;
  int    gSatellites    = 0 ;
  struct speedDistance speedAndDistance ;
  sprintf(durationStr,"%d:%d:%d",hours, minutes,seconds); 
  
  //Serial.println("Post Details");
  
  if (xQueueReceive(SPEED_TO_WEBSERVER, (void *)&speedAndDistance, 0) == pdTRUE) 
  {
     gSpeed = round(speedAndDistance.Speed);
  }

  if (xQueueReceive(CADENCE_TO_WEBSERVER, (void *)&RPMData, 0) != pdTRUE) 
  {
    
  }
   
  doc["Speed"]         = round(gSpeed);
  doc["Cadence"]       = round(RPMData);
  doc["Distance"]      = round(speedAndDistance.distanceKM);
  doc["TotalDistance"] = round(gTotalDistance);
  doc["Pulse"]         = round(gPulseRate);
 
  
  if (gSpeed > 0.0)
  {
      doc["Status"] = "Moving";
  }
  else 
  {
    doc["Status"] = "Stopped";
  }
  gCounter++;    
  doc["Duration"]          = durationStr;
  doc["RoomTemperature"]   = gRoomTemp ;
  doc["Power"]             =     0 ;        // ComputePower(gSpeed);
  doc["RoomHumidity"]      = round(gRoomHumidity) ;
  doc["AverageSpeed"]      = 0.0;
  doc["RestDuration"]      = 0.0;
  doc["Satellites"]        = gSatellites ;
  doc["GPSTimeSet"]        = false;
  doc["KeepAlive"]          = gCounter ;
  serializeJson(doc, jsonString);
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "json", jsonString);
  //Serial.println(jsonString);
}

void UpdateConfigJson()
{
  // To update config.json
}

void ResetTrip()
{
 //  gtripDuration =0 ;
 //  CreateNewRecordFile();
}

void SetDeviceTime()
{
      File  file ;
     size_t  sent;
 
     if (LittleFS.exists("/settime.html"))  
     { 
        file =LittleFS.open("/settime.html",  "r");
        sent =webServer.streamFile(file, "text/html");  
        file.close();
     }
     else
     {
         webServer.sendHeader("Connection", "close");
         webServer.send(200, "text/html", "<HTML> <H1> File settime.html not found </H1> </HTML>");
     }
}

void SendRealTimeData()
{
    StaticJsonDocument<256> doc;
    char JsonStr[250];
    int     len;
    String  currentTime, currentDate,dateStamp;
    char    deviceTime[20],deviceDate[20] ;
    int     splitT;
    if (gWifiConnection == true)
    {
      currentDate = timeClient.getFormattedDate(); 

      splitT = currentDate.indexOf("T");
      dateStamp = currentDate.substring(0, splitT);
      len = dateStamp.length();   
      currentDate.toCharArray(deviceDate,len+1);
   
      currentTime = timeClient.getFormattedTime(); 
      len = currentTime.length();
      currentTime.toCharArray(deviceTime,len+1);
    
     
      doc["time"] = deviceTime ;
      doc["value"] = (random(0,9)/100) +1.1;

      serializeJson(doc, JsonStr);
      webServer.sendHeader("Connection", "close");
      webServer.send(200, "text/event-stream", JsonStr);
      DEBUG_PRINTF("%s \n", JsonStr);
    }
    DEBUG_PRINTF("SendRealTimeData\n");

}
void SetTimeOnDevice()
{
   String  deviceDateStr;
   String  deviceTimeStr;
   char  DeviceTime[9];
   char  DeviceDate[12];
   int  day, month, year ;
   int  hour, minutes, seconds;
  DEBUG_PRINTF("SetTimeOnDevice\n");
  if (webServer.hasArg("deviceDate"))
  {
     deviceDateStr = webServer.arg("deviceDate") ;
     deviceDateStr.toCharArray(DeviceDate,12);
     sscanf(DeviceDate, "%d-%d-%d", &day, &month, &year);
  }

  if (webServer.hasArg("deviceTime"))
  {
     deviceTimeStr = webServer.arg("deviceTime") ;
     deviceTimeStr.toCharArray(DeviceTime,9);
     sscanf(DeviceTime, "%d:%d:%d", &hour, &minutes, &seconds);
  }
  DEBUG_PRINTF("%s, %s",DeviceDate,DeviceTime);

  rtc.adjust(DateTime(year,month,day,hour,minutes,seconds));
  
  DEBUG_PRINTF("SetTimeOnDevice\n");
  webServer.sendHeader("Connection", "close");
  //sprintf(message,"<HTML> <body style=\"background-color:black;\"> <H1> Record %s Deleted </H1></HTML>",fileName);
  webServer.send(200, "text/html", "<HTML> <body style=\"background-color:powderblue;\"> <H1> Date and Time Configured</H1></HTML>");
}

void setupWebHandler()
{
   /*return index page which is stored in serverIndex */
  
  webServer.on("/",                 HTTP_GET,   DisplayLoginIndex);
  webServer.on("/serverIndex",      HTTP_GET,   DisplayserverIndex);
  webServer.on("/dashboard",        HTTP_POST,  DisplayDashboard);
  webServer.on("/speed",            HTTP_POST,  PostDetails);
  webServer.on("/ConfigPage",       HTTP_POST,  ChangeDetails);
  webServer.on("/Fileupload",       HTTP_POST,  FileUpload);
  webServer.on("/rebootDevice",     HTTP_POST,  RebootDevice);
  webServer.on("/updateConfigJson", HTTP_POST,  UpdateConfigJson);
  webServer.on("/ListRecords",      HTTP_POST,  ListRecords);
  webServer.on("/resettrip",        HTTP_POST,  ResetTrip);
  webServer.on("/DeleteRecords",    HTTP_POST,  DeleteRecords);
  webServer.on("/gaugeDisplay",     HTTP_POST,  DisplayGaugeDisplay);
  webServer.on("/ViewCSVFile",      HTTP_POST,  SendFileToBrowser);
  webServer.on("/DeleteCSVFile",    HTTP_POST,  DeleteRecordFile);
  webServer.on("/Analysis",         HTTP_POST,  DisplayRecordsForAnalysis);
  webServer.on("/ShowGraph",        HTTP_POST,  ShowGraph);
  webServer.on("/RealTime",         HTTP_POST,  RealTimeGraph);
  webServer.on("/chart-data",       HTTP_GET,  SendRealTimeData);
  webServer.on("/SetTime",         HTTP_POST,  SetDeviceTime);
  webServer.on("/SetTimeOnDevice", HTTP_POST,  SetTimeOnDevice);
  webServer.on("/DeleteEmptyRecords",    HTTP_POST,  DeleteEmptyRecords);
  
  
  
  
  /*handling uploading firmware file */
  webServer.on("/update", HTTP_POST, []() {
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = webServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
      //DEBUG_PRINTF("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        //DEBUG_PRINTF("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  webServer.begin();
  Serial.println("WebServer Setup Completed");
}
