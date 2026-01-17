
void DrawLines()
{
  u8g2_display.clearBuffer();					// clear the internal memory
 // u8g2_display.drawFrame(0,13,125,49);
  // Top Horizontal line
  //u8g2_display.drawLine(0, 18 , 64, 18);  
   // Vertical Cross Wire
  u8g2_display.drawLine(64, 1 , 64, 64);
  //Horizontal Cross wire
  u8g2_display.drawLine(1,32 , 127, 32);
  // Draw second Horizontal line
  u8g2_display.drawLine(1,48 , 127, 48);
}
void DisplaySecondScreenOnOLED128x64Display(
                                float totalBikeDistance)
{
    char buffer[20];

    u8g2_display.clearBuffer();
    u8g2_display.sendBuffer();	
    DrawLines();
    u8g2_display.setFont( u8g2_font_profont11_mr );
    u8g2_display.setCursor(4, 43);
    u8g2_display.print("KM:");
    u8g2_display.setCursor(39, 43);
    sprintf(buffer,"%0.2f",totalBikeDistance);
    u8g2_display.print(buffer);	// write something to the internal memory
    u8g2_display.sendBuffer();	
    //DEBUG_PRINTF("Second Screen\n");
}
void DisplayRPMAndSpeedOnOLED128x64Display(float speed, 
                                int cadence, 
                                float tripDistance)

{
  String  currentTime,formattedDate ;
  int len; 
  char    deviceTime[20] ;
  char buffer[20];
  int hours, minutes, seconds, balance ;
  uint32_t  currentUnixTime, diff  ;
  DateTime presentTimeNow ;

  //Serial.printf("DisplayOnOLED128x64Display started \n");
  
  GetCurrentTime(deviceTime) ;

  //Serial.printf("DisplayOnOLED128x64Display: gTripDuration = %d\n",tripDuration);
 
    presentTimeNow = GetRTCNow();
    currentUnixTime = presentTimeNow.unixtime() ;
    
      hours = 0 ;
      minutes = 0 ; 
      seconds = 0 ;
      balance  = 0 ;

  if (cadence != 0)
  {
      diff = currentUnixTime - gPreviousUnixTimeWhenDisplayWasUpdated ;
      gtripDuration = gtripDuration + diff ;
      
  }
  gPreviousUnixTimeWhenDisplayWasUpdated = currentUnixTime ;
  hours = gtripDuration /3600 ;
  balance = gtripDuration % 3600 ;
  minutes = balance / 60 ;
  seconds  = balance % 60 ;

  DrawLines();


  // Display Cadence
  //u8g2_display.setFont( u8g2_font_7x13B_tf);
  //u8g2_display.setCursor(4, 12);
  //u8g2_display.print("RPM");	// write something to the internal memory
  //u8g2_display.setFont( u8g2_font_utopia24_tn);    
  u8g2_display.setFont(u8g2_font_lubB12_tn);
  u8g2_display.setCursor(24, 30);
  sprintf(buffer,"%d",cadence);
  u8g2_display.print(buffer);

  // Display Speed
  //u8g2_display.setFont( u8g2_font_7x13B_tf);
  //u8g2_display.setCursor(66, 12);
  u8g2_display.setCursor(78, 30);
  //u8g2_display.setFont( u8g2_font_utopia24_tn);  
  //u8g2_display.setFont(u8g2_font_lubB12_tn);  
  sprintf(buffer,"%.2f", speed) ;
  u8g2_display.print(buffer);

  // Distance Display
  //u8g2_display.setFont( u8g2_font_7x13B_tf);
  u8g2_display.setFont( u8g2_font_profont11_mr );
  u8g2_display.setCursor(4, 43);
  u8g2_display.print("KM:");
  u8g2_display.setCursor(39, 43);
  sprintf(buffer,"%0.2f",tripDistance);
  u8g2_display.print(buffer);	// write something to the internal memory

  
  u8g2_display.setFont( u8g2_font_profont11_mr );
  u8g2_display.setCursor(4, 58);
  
  sprintf(buffer, "P:%d,S:%d",gCadenceSensorActive,gSpeedSensorActive);

  u8g2_display.print(buffer);

  //Display Trip Duration
  u8g2_display.setCursor(72, 43);
  sprintf(buffer,"%d:%d:%d",hours,minutes,seconds);
  u8g2_display.print(buffer);
  
  //Current Time
  //u8g2_display.setFont( u8g2_font_7x13B_tf);
  u8g2_display.setFont( u8g2_font_profont11_mr );
  u8g2_display.setCursor(68, 60);
  u8g2_display.print(deviceTime);
  u8g2_display.sendBuffer();			
  //Serial.printf("DisplayOnOLED128x64Display exit \n");
}
void SetupDisplay()
{
  char buff[10];
  
  //display.begin();
  u8g2_display.begin();
  u8g2_display.clearBuffer();
  //u8g2_display.setFont( u8g2_font_7x13B_tf);
  u8g2_display.setFont( u8g2_font_profont11_mr );
  DEBUG_PRINTF("Display Started");

  
}
