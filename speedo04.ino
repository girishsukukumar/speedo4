
/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *  ESP8266 Arduino example
 MAX30003 Intergration Pending 
 */
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <SPI.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <WiFiMulti.h>

#include <WiFiMulti.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266FtpServer.h>
#include <NTPClient.h>
#include "time.h"
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "EEPROM.h"
#include <WebSerial.h>

#include <RTClib.h>

#define DEBOUNCETIME 30
#define WEBSERVER_PORT 80
#define JSON_CONFIG_FILE_NAME "/config.json" 
#define JSON_PERSISTANT_FILE_NAME "/persistant.json"
#define SSID_NAME_LEN 20 
#define SSID_PASSWD_LEN 20
#define NAME_LEN 40
#define FTP_USER_NAME "apollo11"
#define FTP_PASSWORD  "eagle"
#define MAX_LIMIT_OF_CONNETION_FAILURE 5
#define RX2 26
#define TX2 27


#define CORE_ONE 1
#define CORE_ZERO 0 

#define MQTT_FINGER_PRINT_LENGTH    150
#define JSON_STRING_LENGTH          350
#define PASSWORD_NAME_LENGTH         20
#define USERNAME_NAME_LENGTH         20
#define GEN_NAME_LEN                 20
#define HOST_NAME_LENGTH             40
#define   DEBUG 1 
//#define  NETWORK_DEBUG 1

// *******************Enable and disable serial print **************
#ifdef DEBUG
#define DEBUG_PRINTLN(x)      Serial.println (x)
#define DEBUG_PRINT(x)        Serial.print (x)
#define DEBUG_PRINTF(f,...)   Serial.printf(f,##__VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(f,...)
#endif


#ifdef NETWORK_DEBUG
    //#define    DEBUG_PRINTF(f,...)  Debug.printf(f,##__VA_ARGS__)
    #define    DEBUG_PRINTLN(x)  WebSerial.println(x);

#endif

// *******************Enable and disable serial print **************

// *******************Declarations needed for GPS *********************
#include <TinyGPS++.h>
#define GPS_BAUD 9600 
HardwareSerial gpsSerial(2);
TinyGPSPlus gps ;
SemaphoreHandle_t rtc_mutex = NULL; 
RTC_DS3231 rtc;

typedef struct GPSDataStructure
{
  float Latitude;
  float Longitude ;
  int   hours ;
  int   minutes ;
  int   seconds ;
  float altitudeInMeters ;
  float speedInKmH ;
  int   noOfSatellites ; 
  bool  GPSActiveFlag ;
}  ;
// *********************End Declarations needed for GPS****************************

typedef struct configData 
{
  char     ssid1[SSID_NAME_LEN]   ;
  char     password1[SSID_PASSWD_LEN]  ;
  char     ssid2[SSID_NAME_LEN] ;
  char     password2[SSID_PASSWD_LEN] ;
  char     ssid3[SSID_NAME_LEN] ;
  char     password3[SSID_PASSWD_LEN] ;
  float    wheelCirumference ;
  char     wifiDeviceName[NAME_LEN] ;  
  char     userName[NAME_LEN];

  bool     TrainerMode ;
};

typedef struct speedDistance
{
  float Speed ;
  float distanceKM ;
} ;

void WriteToRecordFile(int cadence, struct speedDistance, GPSDataStructure GPSData);
void ReadConfigValuesFromSPIFFS();
void DisplayConfigValues();
void setupWebHandler();

int            gtripDuration =0;
bool           gCadenceSensorActive = false ;
bool           gSpeedSensorActive = false ;

long           gRecordFileSequenceNo ;
String         gHotSpotIP ;
uint32_t       gPreviousUnixTimeWhenDisplayWasUpdated ;
WiFiMulti      wifiMulti;          //  Create  an  instance  of  the ESP32WiFiMulti 
struct         configData ConfigData;
const byte     CADENCE_PIN = 18  ;
const byte     SPEED_PIN   = 19  ;
FtpServer      ftpSrv; 
WebServer      webServer(WEBSERVER_PORT);
AsyncWebServer DebugServer(100);

WiFiUDP        ntpUDP;
NTPClient      timeClient(ntpUDP);
int            msg_queue_len = 10;
char           pulse = '1' ;
HardwareSerial SerialGPS(1);

BaseType_t xHigherPriorityTaskWoken = pdFALSE;
static     QueueHandle_t CADENCE_MSG_QUEUE ;
static     QueueHandle_t SPEED_MSG_QUEUE ;
static     QueueHandle_t CADENCE_TO_DISPLAY_MODULE ;
static     QueueHandle_t SPEED_TO_DISPLAY_MODULE ;
static     QueueHandle_t SPEED_TO_WEBSERVER  ;
static     QueueHandle_t CADENCE_TO_WEBSERVER ;

portMUX_TYPE cadenceMux = portMUX_INITIALIZER_UNLOCKED;
volatile int      numberOfCadenceInterrupts = 0;
volatile bool     cadencePinLastState;
volatile uint32_t cadenceDebounceTimeout = 0;

portMUX_TYPE speedMux = portMUX_INITIALIZER_UNLOCKED;

volatile int      numberOfSpeedInterrupts = 0;
volatile bool     speedPinLastState;
volatile uint32_t speedDebounceTimeout = 0;
char              gRecordFileName[NAME_LEN]; 
#if 0
//                     1    2    3    4    5   6    7    8   9    10
//-------------------------------------------------------------------
int PowerTable[60] = { 0,   0,   0,   1,   3,  7,  10,  15,  21,  25, //0
                      30,   35, 40,  45,  50, 55,  65,  70,  75,  80, //1
                      90,  100,105, 115, 125,135, 145, 155, 165, 180, //2
                     190,  210,220, 240, 250,265, 280, 300, 320, 330, //3
                     345,  360,380, 410, 430,450,480,  500, 530, 550, //4
                     575,  610,640, 675, 720,750,790,  825, 860, 900};//5 

#endif 

int    gCounter       = 0 ;
float  gTripDistance  = 0 ;
float  gBikeDistance  = 0.0 ;
float  gBikeDistanceInMeters = 0.0;
bool   gWifiConnection = false ;
bool   gRTCReady       = false ;
int    timeCounter = 0 ; // To be used to track time, in case wifi is not avalable

/*
Hall Effect Sensor Used A3144
https://www.circuits-diy.com/wp-content/uploads/2019/12/a3144-magnetic-hall-effect-sensor.png
  1   2   3
/--  --  --\
|   A3144   |
|___________|
1 => Vcc +5v
2=> GND
3=> DATA
*/

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Adafruit_SSD1306 display(0x3c, 21, 22); 
//Adafruit_SSD1306 display(0x3c, 21, 22); 

//Trying Nokia display
//Adafruit_PCD8544 display = Adafruit_PCD8544(18,   23,   19,   5,   14);

// **** New GPIO Configuration to support the ESP32 wiring tp Support SPI display****
/* Reference :Perplexity 
Prompt : You are an expert in ESP32 microcontrollers
Can I use any GPIO pin for interfacing ESP32 to SPI based LCD display

Response
You can use most ESP32 GPIO pins for interfacing with SPI-based LCD displays, 
as the SPI bus can be configured to run on almost any available GPIO pin due 
to the ESP32’s pin multiplexing capability. 

The ESP32 has multiple SPI interfaces (HSPI, VSPI), and you can map them to
most GPIOs, except for certain input-only or reserved pins.

Avoid pins used for flash or reserved functions unless you are experienced, 
as this can cause boot or operational issues.

The ESP32 has some GPIO pins that are unsafe to use or are input only, which you should avoid for SPI or other general GPIO purposes:

GPIO6 to GPIO11 (GPIO6, 7, 8, 9, 10, 11) are used internally for the flash memory interface and should never be used for SPI or other I/O as it will cause malfunctions.

GPIO12 is a special pin that controls the flash voltage level during boot and can brick the device if misconfigured.

GPIO34 to GPIO39 are input-only pins and cannot be used as SPI outputs (MOSI, CLK, or CS).

GPIO0, GPIO2, GPIO4, GPIO5, GPIO15 are strapping pins influencing boot mode and should be used 
carefully.

Some pins can have pullup/down or special functions during boot (e.g., GPIO0, GPIO2, GPIO15) 
that may cause 
unintended behavior if used for output without consideration.

For SPI, avoid using GPIO6-11, GPIO12 for flash stability reasons, and do not use GPIO34-39 
for MOSI, CLK, or CS since they are input only.
*/

//Adafruit_PCD8544 display = Adafruit_PCD8544(26,   25,   33,   12,   14);
//int contrastValue = 45; // Default Contrast Value
//const int adcPin = 34;
//int adcValue = 0;
//int BackLightPIN = 27 ;

/**********************************************************************
 Following is for WAVESHARE 2.42-inch display SSD1309
 **********************************************************************
2.42-inch OLED module with an embedded SSD1309 driver chip and supports SPI/I2C communication. 
2.42-inch OLED is from Waveshare - resolution 128 × 64 pixels
OPERATING VOLTAGE: 3.3V / 5V
COMMUNICATION INTERFACE: 4-wire SPI (default) / I2C
DRIVER CHIP: SSD1309
RESOLUTION: 128 × 64 pixels
DISPLAY SIZE: 55.01 × 27.49mm
PIXEL SIZE: 0.4 × 0.4mm
MODULE SIZE: 61.50 × 39.50mm
https://www.waveshare.com/wiki/2.42inch_OLED_Module

Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
This above library is used, for coding the display control


*/

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI u8g2_display(U8G2_R0, 
                                             26,  // CLK
                                             25,  // DIN
                                             32,  // CS
                                             33,  // DC
                                             14); // RESET 

DateTime GetRTCNow()
{
  DateTime presentTimeNow ;
  presentTimeNow  = DateTime(2025, 01, 01, 01, 01, 01);
  //DEBUG_PRINTF("GetRTCNow: About to take Mutex\n");
  if (xSemaphoreTake(rtc_mutex, portMAX_DELAY) == pdTRUE) 
  {
     //DEBUG_PRINTF("GetRTCNow: Took Mutex about the call rtc.now\n");
     presentTimeNow = rtc.now();
     //DEBUG_PRINTF("GetRTCNow: Returned from rtc.now\n");
     xSemaphoreGive(rtc_mutex); 
     //DEBUG_PRINTF("GetRTCNow: Mutex released \n");
    return presentTimeNow ;
  }
  return presentTimeNow ; // Return default value which is 1-Jan-2025 01:00:00 AM
}
void GetCurrentDate(char *currentDate)
{
  DateTime presentTimeNow ;

   char dateStr[15]; 
   //DEBUG_PRINTF("DEBUG: GetCurrentDate API Called\n");
   
   presentTimeNow = GetRTCNow();
   
   //DEBUG_PRINTF("DEBUG: RTC Time extracted \n") ;

  // Getting each time field in individual variables
  // And adding a leading zero when needed;
  
  String yearStr = String(presentTimeNow.year());
  String monthStr =  String(presentTimeNow.month()) ;
  String dayStr = String(presentTimeNow.day()) ;

  String formattedTime = dayStr +"-"+ monthStr + "-" + yearStr ;
 //formattedTime.toCharArray(dateStr, 9) ;
  //DEBUG_PRINTF("DEBUG: GetCurrentDate 1 \n") ;
  sprintf(dateStr,"%s",formattedTime.c_str());
  //DEBUG_PRINTF("DEBUG: GetCurrentDate 2 \n") ;
  strcpy(currentDate,dateStr);
  //DEBUG_PRINTF("GetCurrentDate > : %s\n",currentDate);
  return ;

}

void GetCurrentTime(char *currentTime)
{
   
   char TimeStr[10]; 
   
   DateTime presentTimeNow ;
   //DEBUG_PRINTF("DEBUG: GetCurrentTime API Called\n");
   presentTimeNow = GetRTCNow();
   //DEBUG_PRINTF("DEBUG: RTC Time extracted \n") ;
  // Getting each time field in individual variables
  // And adding a leading zero when needed;
  String hourStr = (presentTimeNow.hour() < 10 ? "0" : "") + String(presentTimeNow.hour(), DEC); 
  String minuteStr = (presentTimeNow.minute() < 10 ? "0" : "") + String(presentTimeNow.minute(), DEC);
  String secondStr = (presentTimeNow.second() < 10 ? "0" : "") + String(presentTimeNow.second(), DEC);
  String formattedTime = hourStr + ":" + minuteStr + ":" + secondStr;
  formattedTime.toCharArray(TimeStr, 10) ;
  strcpy(currentTime,TimeStr);
  //DEBUG_PRINTF("DEBUG: GetCurrentTime %s\n", currentTime);
  //DEBUG_PRINTF("%d, %d, %d\n",presentTimeNow.hour(),presentTimeNow.minute(),presentTimeNow.second() );
  return ;
}

void setupRTC()
{
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  DEBUG_PRINTF("RTC initalized\n");
  
  gRTCReady = true; 

  if (rtc.lostPower()) 
   {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  else
  {
    DEBUG_PRINTF("RTC battery OK\n");
  }
 
}

void ICACHE_RAM_ATTR SPEED_PIN_Handler()
{
  portENTER_CRITICAL_ISR(&speedMux);
  numberOfSpeedInterrupts++;
  gSpeedSensorActive =  true ;
//  DEBUG_PRINTLN(numberOfSpeedInterrupts);
  speedPinLastState = digitalRead(SPEED_PIN);
  speedDebounceTimeout = xTaskGetTickCount(); //version of millis() that works from interrupt
  
  portEXIT_CRITICAL_ISR(&speedMux);
}



void ICACHE_RAM_ATTR CADENCE_PIN_Handler()
{
  portENTER_CRITICAL_ISR(&cadenceMux);
  numberOfCadenceInterrupts++;
  gCadenceSensorActive = true ;
  cadencePinLastState = digitalRead(CADENCE_PIN);
  cadenceDebounceTimeout = xTaskGetTickCount(); //version of millis() that works from interrupt
  portEXIT_CRITICAL_ISR(&cadenceMux);
}

bool ConnectToWifi()
{
  int count ;
  char ssid[40];
  char ipAddr[25];
  String IPaddress;
  DEBUG_PRINTF("ConnectToWifi: begin\n");
  DEBUG_PRINTF("ConnectToWifi: Hostname: %s\n",ConfigData.wifiDeviceName);

  WiFi.setHostname(ConfigData.wifiDeviceName);
  wifiMulti.addAP(ConfigData.ssid1, ConfigData.password1);   
  wifiMulti.addAP(ConfigData.ssid2, ConfigData.password2);    
  wifiMulti.addAP(ConfigData.ssid3, ConfigData.password3);    
  count  = 0 ;
  //Serial.println("XXXX1");

  while  (wifiMulti.run()  !=  WL_CONNECTED) 
  { 
    //  Wait  for the Wi-Fi to  connect:  scan  for Wi-Fi networks, and connect to  the strongest of  the networks  above       
    delay(1000);        
    DEBUG_PRINTF("*");    
    count++ ;
    if (count > 40)
    {
       return false ;  
    }
  }   
  delay(5000);
  //Serial.println("XXXX3");


  DEBUG_PRINTF("\n");   
  strcpy(ssid,WiFi.SSID().c_str() );
  IPaddress = WiFi.localIP().toString();
  strcpy(ipAddr,IPaddress.c_str());
  DEBUG_PRINTF("Connected to  ");   
  DEBUG_PRINTF("%s\n",ssid);         
  DEBUG_PRINTF("IP  address: %s ",ipAddr);   
  DEBUG_PRINTLN(); 

   // Set mDNS
  if (!MDNS.begin(ConfigData.wifiDeviceName)) 
  {
    Serial.println("Error setting up MDNS responder!");
     u8g2_display.clearBuffer();
     u8g2_display.sendBuffer();
     u8g2_display.drawStr(1,40,"Error with MDNS");
     u8g2_display.drawStr(1,50,"Device may work");
     delay(10000);
  }

  // Add service to MDNS-SD
  MDNS.addService("_http", "_tcp", 80);
  Serial.println("mDNS responder started. Access your ESP32 at http://" + String(ConfigData.wifiDeviceName) + ".local");

  u8g2_display.clearBuffer();
  u8g2_display.sendBuffer();
  u8g2_display.drawStr(1,40,ssid);
  u8g2_display.drawStr(1,50,ipAddr);
  u8g2_display.sendBuffer();

  WiFi.softAPdisconnect (true);   //Disable the Access point mode.
  Serial.println("ConnectToWifi: End");
 
  return true ;
}


#if 0
int ComputePower(float s)
{
  /* 
   *  PowerTable is specific to Jet Black Pro fluid trainer
   *  This can be extract from the specification of any trainer
   *  Provided the manufacture publishes it
   *  The power table for Jet Black trainer can be found at 
   *  https://www.jetblackcycling.com/indoor-cycle-trainers-and-accessories/z1-pro-fluid-trainer/
   *  https://www.jetblackcycling.com/wp-content/uploads/2020/03/jetblack-cycling-Fluid-Resistance-1536x697.png
   *  Alternatively an equation for Kurt Kinetic Trainer can be found at 
   *  https://www.cycling-inform.com/how-to-use-a-cheap-25-bike-computer-to-measure-power-with-a-kurt-kinetic-trainer
   *  This formula can be tuned  for other trainer too.
   *  
  */
  
  int idx, power ;
  idx = round(s);
  if (idx >= 60)
  {
    power = PowerTable[59];
  }
  else if (idx > 0)
  {
     power = PowerTable[idx-1] ;
  }
  else if (idx == 0)
  {
    power = 0 ;
  }
  else
     power = -1 ; // Error will be reported in record file
  
  //DEBUG_PRINTF("Compute Power %f, %d, %d \n", s, idx, power);
      
  return power ;
}
#endif 
void DISPLAY_TASK(void *parameters)
{
   int      cadence  = 0 ;
   struct   speedDistance speedAndDistance ;
   bool     flag = false ;
   int      zeroCadenceWaterMark =0  ;
   int      currentTime = 0 ;
   float    timeFrame ;
   
   int     LastSpeedComputedTime=0 ;
   float   speedInMeterPerSec ;
   float   distanceTravelledinTimeFrame;
   int     lastRecordLoggingTime ;
   char    buff[10];
   GPSDataStructure GPSData ;

    //WebSerial.printf("Display Thread Started\n");

   u8g2_display.clearBuffer();
   

   DisplayRPMAndSpeedOnOLED128x64Display(speedAndDistance.Speed,
                                 cadence,
                                 gBikeDistance
                                );

   currentTime = millis();
   lastRecordLoggingTime = millis();

   while(1)
   {    
    currentTime = millis();
    if (xQueueReceive(CADENCE_TO_DISPLAY_MODULE, &cadence, 0) == pdTRUE) 
    {
       currentTime = millis();
       timeFrame = (currentTime - LastSpeedComputedTime)/1000 ;
    
       
       if (cadence == 0)
       {
          // Count the number of time Cadence is zero
          zeroCadenceWaterMark++ ;
       }
       else
       {
          zeroCadenceWaterMark = 0 ;
       }

       if (zeroCadenceWaterMark <= 2)
       {
          // Do not write to record file, in case cadence remains at zero 
          // for more than 2 minutes.

          if ((currentTime - lastRecordLoggingTime) >= 15000)
          {
             //DEBUG_PRINTF("DISPLAY_TASK Log: Speed = %0.1f\n",speedAndDistance.Speed);
              WriteToRecordFile(cadence, speedAndDistance, GPSData);
              lastRecordLoggingTime = millis();            
          }
          
       }
    }
    if (xQueueReceive(SPEED_TO_DISPLAY_MODULE, &speedAndDistance, 0) == pdTRUE) 
    {
      /*---------------------------------------------------------------------------
           if (timeFrame >= 5.0)
           {               
            ------------------------------------------------
              1. Convert Speed to meter/sec from Kh/hr
              2. Compute delta distanceTravelled Speed X timeFrame
              3. Add delta to gTripDistance
              4. KM travelled = gTripDistance/1000
              -------------------------------------------------
            
              speedInMeterPerSec = speedAndDistance.Speed ;
              distanceTravelledinTimeFrame = speedInMeterPerSec * timeFrame ;
              gTripDistance =gTripDistance+distanceTravelledinTimeFrame ;
              speedAndDistance.distanceKM= gTripDistance/1000 ;
           }
      ------------------------------------------------------------------------------ */
    }
    //DEBUG_PRINTF("DISPLAY_TASK Speed = %0.1f, Distance %0.1f\n",speedAndDistance.Speed,speedAndDistance.distanceKM);
    
    DisplayRPMAndSpeedOnOLED128x64Display(speedAndDistance.Speed,
                                 cadence,
                                 speedAndDistance.distanceKM
                                );
      
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    DisplaySecondScreenOnOLED128x64Display(gBikeDistance);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
   }
}
void COUNT_CADENCE(void *parameters) 
{
  int cadenceTicks =0;
  char item = 'c';
  int currentTime  ;
  int LastCadenceComputedTime =0;
  int RPM ;
  int timerVal;
  int diff ;
  bool persistantDataWritten = false ;
  uint32_t previousUnitTime, currentUnixTime ;
  DateTime presentTimeNow ;

  presentTimeNow = GetRTCNow();
  currentUnixTime = presentTimeNow.unixtime() ;
  previousUnitTime = currentUnixTime ;
  while(true)
  {
    if (xQueueReceive(CADENCE_MSG_QUEUE, (void *)&item, 0) == pdTRUE) 
    {
      cadenceTicks++ ;     
    }
    currentTime = millis(); 
    diff = currentTime - LastCadenceComputedTime ;
    
    if (diff >= 60000)
    {
       // We are calculating cadence everyone minute or 60 secs
       // The value displayed will the true RPM for the last one minute and
       // not a calculated RPM
       RPM = cadenceTicks;
   
       xQueueSend(CADENCE_TO_DISPLAY_MODULE, (void*) &RPM,0);
       xQueueSend(CADENCE_TO_WEBSERVER, (void*) &RPM,  0);
       cadenceTicks = 0 ;                     
       LastCadenceComputedTime = currentTime ;   
    }
    if (cadenceTicks == 0)
    {
         //PEdelling stopped
         // We keep track of internal time even when bike is stopped
         // how ever gTripDuration variable will be updated only
         // when bike is moving
     
         //DEBUG_PRINTF("cadenceTick = %d %d \n", cadenceTicks,diff);
         gCadenceSensorActive = false;
         if (diff >= 30000)
         {
            //When the cycle stops we will  update the persistant data file
           //DEBUG_PRINTF("cadenceTick = %d \n", cadenceTicks);
            if (persistantDataWritten == false)
            {
               //DEBUG_PRINTF("Calling WritePersistantDataToSPIFFS\n");
               WritePersistantDataToSPIFFS();
               persistantDataWritten = true ;
               // Persistant data need to written only once if the 
               // pedal stops for more that 2 mins 
            }
         }
    }       
  }  
}

void COUNT_SPEED(void *parameters) 
{
  int    speedTicks =0;
  float  distanceTravelled = 0 ;
  char   item  = 'c';
  int    currentTime = 0 ;
  float  timeFrame ;
  //float  Speed  ;
  int    LastSpeedComputedTime=0 ;

  float  DistanceKM     = 0 ;
  int    gTotalDistance = 0 ;
  struct speedDistance speedAndDistance ;
  //ConfigData.wheelCirumference = 2.19  ; //meter had coded for testing to beremoved
  while (true)
  {
     if (xQueueReceive(SPEED_MSG_QUEUE, (void *)&item, 0) == pdTRUE) 
     {
       speedTicks++ ;
     }  
     if (speedTicks == 0)
     {
      gSpeedSensorActive =false ;
     }
     currentTime = millis();
     timeFrame = (currentTime - LastSpeedComputedTime)/1000 ;
     if (timeFrame >= 5.0)
     {
         distanceTravelled =  (speedTicks * ConfigData.wheelCirumference);
         LastSpeedComputedTime = currentTime ;
         speedAndDistance.Speed  = distanceTravelled/timeFrame ;

         speedAndDistance.Speed  = distanceTravelled / timeFrame ; // This is meters/per sec
         //speedAndDistance.Speed  = speedAndDistance.Speed * 3.6 ; // convert m/sec into Km/hr 
 
         gTripDistance =gTripDistance+distanceTravelled ;
         speedAndDistance.distanceKM= gTripDistance/1000 ;
         //DEBUG_PRINTF("Distance = %f", speedAndDistance.distanceKM);
         
         
         if (gTripDistance > 1000.0)
         {
            if (speedAndDistance.Speed > 0)
            {
               gBikeDistanceInMeters = gBikeDistanceInMeters + distanceTravelled;

               // Convert the distance in KM
               gBikeDistance =  gBikeDistanceInMeters/1000 ;      

            }
         }
         
         speedTicks = 0 ;
         // Make structure and send it across ro WEBSERVER
         xQueueSend(SPEED_TO_WEBSERVER, (void*) &speedAndDistance,  0);
         xQueueSend(SPEED_TO_DISPLAY_MODULE, (void*) &speedAndDistance , 0);         
     }
    
  }
}
void ConfigureAsAccessPoint()
{
   IPAddress local_IP(192,168,4,4);
   IPAddress gateway(192,168,5,5);
   IPAddress subnet(255,255,255,0);

   WiFi.softAP(ConfigData.wifiDeviceName, "123456789");  //Start HOTspot removing password will disable security
   DEBUG_PRINTF("Wifi.softAP completed ");
   WiFi.softAPConfig(local_IP, gateway, subnet);
   IPAddress myIP = WiFi.softAPIP(); //Get IP address
   gHotSpotIP = myIP.toString() ; 
   //display.clearDisplay();
   u8g2_display.clearBuffer();
   u8g2_display.drawStr(1,0,"WiFi Hotspot");
   u8g2_display.drawStr(1,4,gHotSpotIP.c_str());  //extract the string for display
   u8g2_display.sendBuffer();

    // Set mDNS
  if (!MDNS.begin(ConfigData.wifiDeviceName)) 
  {
    Serial.println("Error setting up MDNS responder!");
     u8g2_display.clearBuffer();
     u8g2_display.sendBuffer();
     u8g2_display.drawStr(1,40,"Error with MDNS");
     u8g2_display.drawStr(1,50,"Device may work");
     delay(10000);
  }

  // Add service to MDNS-SD
  MDNS.addService("_http", "_tcp", 80);
  Serial.println("mDNS responder started. Access your ESP32 at http://" + String(ConfigData.wifiDeviceName) + ".local");
  vTaskDelay(5000);

}

void setup() 
{
   struct GPSDataStructure GPSData ;
   struct speedDistance speedAndDistance ;
   int GMTOffset = 19800; //sec
   char deviceDate[12];
   char deviceTime[10];
   char buffer[50];
   DateTime presentTimeNow ;
   uint32_t  currentUnixTime  ;

   gBikeDistance = 0.0;
   Serial.begin(115200);
   delay(1000);
   rtc_mutex = xSemaphoreCreateMutex();

   if (LittleFS.begin() ==  false)
   {
    Serial.println("LittleFS mount failed - formating");
   }
  




   Serial.printf("Starting the display");
   SetupDisplay();
   Serial.printf("Display ready");
   setupRTC();
   delay(1000);
   
  

  presentTimeNow = GetRTCNow();
  // We are updating the following Global variable
  // as gPreviousUnixTimeWhenDisplayWasUpdated is used
  // for calcuating the actual duration for cycling, that is duration for which cycle was pedalled 
  // this is automatically remove the time during which breaks are taken
  
  gPreviousUnixTimeWhenDisplayWasUpdated = presentTimeNow.unixtime() ;
   ReadConfigValuesFromSPIFFS();
   
   DisplayConfigValues();
   CADENCE_MSG_QUEUE = xQueueCreate(msg_queue_len, sizeof(char));   
   SPEED_MSG_QUEUE = xQueueCreate(msg_queue_len, sizeof(char));

   SPEED_TO_DISPLAY_MODULE  = xQueueCreate(msg_queue_len, sizeof(struct speedDistance));
   CADENCE_TO_DISPLAY_MODULE  = xQueueCreate(msg_queue_len, sizeof(int));

   SPEED_TO_WEBSERVER  = xQueueCreate(msg_queue_len, sizeof(struct speedDistance));
   CADENCE_TO_WEBSERVER  = xQueueCreate(msg_queue_len, sizeof(int));
   
   
   pinMode(CADENCE_PIN, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(CADENCE_PIN), CADENCE_PIN_Handler, RISING);

   pinMode(SPEED_PIN, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(SPEED_PIN), SPEED_PIN_Handler, RISING);
  // We start by connecting to a WiFi network
  ConfigData.TrainerMode = true ;

   // Start RTC  hard I2C interface
   Serial.printf("Starting the RTC");
 
  u8g2_display.clearBuffer();
  u8g2_display.drawStr(1,30,"Connecting to Wifi...");
  u8g2_display.sendBuffer();
  
  /* A check is needed here in case of trainer where GPS is not required */
 //SetUPConnectionToGPS();

  if (ConnectToWifi() == true)
  {

     gWifiConnection = true ;
     
  }
  else
  {
       u8g2_display.drawStr(1,10,"Wifi not available..");
       u8g2_display.drawStr(1,20,"Access point mode...");
       DEBUG_PRINTF("WifiSetup: failed \n");
       ConfigureAsAccessPoint(); 
       u8g2_display.drawStr(1,30,"IP: 192.168.4.4");
       delay(6000);
  }
   InitGPSDataStruct(&GPSData); //Set all values to ZERO 
  CreateRecordFile();
  DEBUG_PRINTF("Create Record file %s\n",gRecordFileName);
  speedAndDistance.distanceKM = 0 ;
  speedAndDistance.Speed = 0 ;
  //DEBUG_PRINTF("1>");
  WriteToRecordFile(0 , speedAndDistance, GPSData);
  //DEBUG_PRINTF("2>");
  //DEBUG_PRINTF("First write  to  Record file successful\n");
  u8g2_display.drawStr(1,62,"Connected to Wifi");
  u8g2_display.sendBuffer();	
  delay(5000);
  //WebSerial.begin(&DebugServer);
  //DebugServer.begin();
  //WebSerial.println("WebSerial Started");

  setupWebHandler();
  ftpSrv.begin(FTP_USER_NAME,FTP_PASSWORD);
  ReadPersistantDataFromSPIFFS();
  DEBUG_PRINTF(" gBikeDistance= %f\n",gBikeDistance);
  
  u8g2_display.clearBuffer();
  GetCurrentDate(deviceDate) ;
  GetCurrentTime(deviceTime) ;
  
  sprintf(buffer,"Date: %s ",deviceDate);
  u8g2_display.drawStr(3,32,buffer);
  
  sprintf(buffer,"Time: %s ",deviceTime);
  u8g2_display.drawStr(3,42,buffer);
  
  sprintf(buffer,"Usage: %d km", round(gBikeDistance));
  u8g2_display.drawStr(3,52,buffer);

  u8g2_display.drawStr(1,62,"Sensors:Ready");
  u8g2_display.sendBuffer();	
  delay(6000);
  DEBUG_PRINTF("About to create threads\n");
  //WebSerial.printf("About to create threads\n");

  xTaskCreatePinnedToCore(COUNT_CADENCE,
                          "Track Cadence",
                          4096,
                          NULL,
                          1,
                          NULL,
                          1);

   xTaskCreatePinnedToCore(COUNT_SPEED,
                          "Track Speed",
                          2048,
                          NULL,
                          1,
                          NULL,
                          1);
                          
  xTaskCreatePinnedToCore(DISPLAY_TASK,
                          "Display Task",
                          4096,
                          NULL,
                          1,
                          NULL,
                          1);
   xTaskCreatePinnedToCore(MANAGE_DEBOUNCE_CADENCE_PIN,
                          "Manage Debouce Cadence Task",
                          2048,
                          NULL,
                          1,
                          NULL,
                          1);
                      
 xTaskCreatePinnedToCore(MANAGE_DEBOUNCE_SPEED_PIN,
                          "Manage Debouce Speed Task",
                          2048,
                          NULL,
                          1,
                          NULL,
                          1);

 

  // DEBUG_PRINTF("%s,%s\n",F(__DATE__), F(__TIME__));
}

void MANAGE_DEBOUNCE_CADENCE_PIN(void *parameters)
{
  uint32_t saveDebounceTimeout;
  bool     saveLastState;
  int      save;

  while (1) 
  {

    portENTER_CRITICAL_ISR(&cadenceMux); // so that value of numberOfButtonInterrupts,l astState are atomic - Critical Section
       save  = numberOfCadenceInterrupts;
       saveDebounceTimeout = cadenceDebounceTimeout;
       saveLastState  = cadencePinLastState;
    portEXIT_CRITICAL_ISR(&cadenceMux); // end of Critical Section

    bool currentState = digitalRead(CADENCE_PIN);

    // This is the critical IF statement
    // if Interrupt Has triggered AND Button Pin is in same state AND the debounce time has expired THEN you have the button push!
    //
    if ((save != 0) //interrupt has triggered 
         && (currentState == saveLastState) // pin is still in the same state as when intr triggered
         && (millis() - saveDebounceTimeout > DEBOUNCETIME ))
    { 
      // and it has been low for at least DEBOUNCETIME, then valid keypress
      
      if (currentState == HIGH)
      {
        //DEBUG_PRINTF("Cadence tick high\n");        
        xQueueSend(CADENCE_MSG_QUEUE, &pulse, 0 );
      }
      else
      {
        //Serial.printf("Cadence tick low %d\n",save);        
      }
      
      //Serial.printf("Button Interrupt Triggered %d times, current State=%u, time since last trigger %dms\n", save, currentState, millis() - saveDebounceTimeout);
      
      portENTER_CRITICAL_ISR(&cadenceMux); // can't change it unless, atomic - Critical section
      numberOfCadenceInterrupts = 0; // acknowledge keypress and reset interrupt counter
      portEXIT_CRITICAL_ISR(&cadenceMux);
      //Serial.printf("Out of critical section");

      vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);

  }
}

void MANAGE_DEBOUNCE_SPEED_PIN(void *parameters)
{
  uint32_t saveDebounceTimeout;
  bool     saveLastState;
  int      save;

  while (1) 
  {
    portENTER_CRITICAL_ISR(&speedMux); // so that value of numberOfButtonInterrupts,l astState are atomic - Critical Section
       save  = numberOfSpeedInterrupts;
       saveDebounceTimeout = speedDebounceTimeout;
       saveLastState  = speedPinLastState;
    portEXIT_CRITICAL_ISR(&speedMux); // end of Critical Section

    bool currentState = digitalRead(SPEED_PIN);

    // This is the critical IF statement
    // if Interrupt Has triggered AND Button Pin is in same state AND the debounce time has expired THEN you have the button push!
    //
    if ((save != 0) //interrupt has triggered 
         && (currentState == saveLastState) // pin is still in the same state as when intr triggered
         && (millis() - saveDebounceTimeout > DEBOUNCETIME ))
    { 
      // and it has been low for at least DEBOUNCETIME, then valid keypress
      
      if (currentState == HIGH)
      {
        //DEBUG_PRINTLN(" Speed tick high");        
        xQueueSend( SPEED_MSG_QUEUE, &pulse, 0 );
      }
      else
      {
       
      }
      
      //Serial.printf("Button Interrupt Triggered %d times, current State=%u, time since last trigger %dms\n", save, currentState, millis() - saveDebounceTimeout);
      
      portENTER_CRITICAL_ISR(&speedMux); // can't change it unless, atomic - Critical section
      numberOfSpeedInterrupts = 0; // acknowledge keypress and reset interrupt counter
      portEXIT_CRITICAL_ISR(&speedMux);
      //Serial.printf("Out of critical section");

      vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);

  }
  
}

void loop() 
{
  webServer.handleClient();
  //ftpSrv.handleFTP(); 
  yield();
}
