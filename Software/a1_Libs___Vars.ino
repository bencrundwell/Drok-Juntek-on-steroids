// *** libraries***
//#define _DISABLE_TLS_
#include <Wire.h>          // Libs for I2C
#include <ArduinoOTA.h>    // from Library
#include <WiFi.h>
#include <WiFiUdp.h>
#include "time.h"
#include <FS.h>
#include <ThingerESP32.h>    // from Library
#include <ThingerConsole.h>  //Hmm... does that work?
#include <EEPROM.h>
#include <Button2.h>     // from Library
#include <ADS1115_WE.h>  // from Library (Wollewald)

#define UDP_TX_PACKET_MAX_SIZE 128 //increase UDP size
#define DST_MN        60               
#define GMT_OFFSET_SEC 3600 * TZ       
#define DAYLIGHT_OFFSET_SEC 60 * DST_MN

#ifdef BOARD_IS_WEMOS 
#include "SSD1306Wire.h"  // from https://github.com/ThingPulse/esp8266-oled-ssd1306/
#define OLED_W 128
#define OLED_H 64
#define OLED_SCL   5  // D1 GPIO5 for I2C (Wire) System Clock
#define OLED_SDA   4  // D2 GPIO4 for I2C (Wire) System Data
#define OLED_RST   0  //    GPIO0
#define ADC_V      36  
#define ADC_I      39  
#define ADC_P      38 
#define ADS_V      ADS1115_COMP_0_GND
#define ADS_I      ADS1115_COMP_1_GND
#define ADS_P      ADS1115_COMP_2_GND
#define TFT_BL     0  // dummy
 
#define PWM_V      12 // Pin SD0  (Vpwm)
#define PWM_I      14 // Pin SD1  (Ipwm) 
#endif

#ifdef BOARD_IS_TTGO  
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#define TFT_W 160
#define TFT_H 128
#define OLED_SCL     22  // GPIO22 for I2C (Wire) System Clock
#define OLED_SDA     21  // GPIO21 for I2C (Wire) System Data
#define TFT_BL       4  // Display backlight control pin
#define SCL          22  // D1 GPIO22 for I2C (Wire) System Clock
#define SDA          21  // D2 GPIO21 for I2C (Wire) System Data
#define RST          0   //    GPIO0
#define MOSI         23  //    GPIO for SPI Master Out
#define MISO         19  //    GPIO for SPI Master In
#define SCLK         18  //    GPIO for SPI System Clock
#define BUTTON_1     35
#define BUTTON_2     0
#define ADC_V        36  
#define ADC_I        39  
#define ADC_P        38
#define ADS_V        ADS1115_COMP_0_GND
#define ADS_I        ADS1115_COMP_1_GND
#define ADS P        ADS1115_COMP_2_GND
    
#define PWM_V        15  // Pin GPIO15  (Vpwm)
#define PWM_I        13  // Pin GPIO13  (Ipwm) 
#endif

#define RecvPin   15 // Pin for IR Receiver

//***Variables for Time***
tm*        timeinfo;                 //localtime returns a pointer to a tm struct static int Second;
time_t     Epoch;
time_t     now;
byte Second;
long SecondOfDay;
byte GracePause;
long MillisMem;
byte Minute;
byte Hour;
byte Day;
byte Month;
unsigned int Year;
byte Weekday;
char DayName[12];
char MonthName[12];
char Time[10];
char Date[12];
byte RunMin;
byte RunHour;
String Runtime;
String Message;

byte slice;
boolean Each6S;
boolean NewMinute;
boolean MinuteExpiring;
boolean NewHour;
boolean HourExpiring;
boolean NewDay;
boolean DayExpiring;


// ***Variables for Menu***
byte    inbyte;
byte    displayPage;
byte    displaySubPage;
byte    serialPage;
byte    serialPeriodicity;
boolean serialEvent;
boolean triglEvent;
boolean coarse = true;

static IPAddress ip;
byte wifiConnectCounter;

struct dashboard {
// Measures
  float Vout = 0;
  float Iout = 0;
  float Wout ;
  float Vin = 0;
  float Iin = 0;
  float Vset ;
  float Iset ;
  float Vday ;
  float Iday ;
  float Wday ;
} dashboard;
unsigned char dashboard_punning[sizeof(dashboard)];  //  Array of characters as image of the structure for udp xmit/rcv

struct persistance {
  long  Runseconds;
  float Whout ;
  float Ahout ;
  float initial_voltage;  
} persistance;
char persistance_punning[sizeof(persistance)];

float load_internal_resistance;
float source_internal_resistance;
float delta_current;
float delta_voltage;
float currentInt;
int   nCurrent;
float CycleVInt;
float CycleIInt;
float CycleWInt;
int   nCycle; 
boolean cycleRST;

// Statistics
float Vh[31];
float Ah[31];
float Wh[31];

unsigned int ADC_VoutRaw;
unsigned int ADC_IoutRaw;
unsigned int ADC_VinRaw;
float converted_VoutRaw;
float converted_IoutRaw;
float converted_VinRaw;
int lastADC_Vout;
int lastADC_Iout;
int lastADC_Vin;

// Injection PWM output values
unsigned int PWM_Vset;
unsigned int PWM_Cset;

//*** Buffers ***
static char charbuff[80];    //Char buffer for many functions

//*** Aliases ***
#define Console0 Serial  // Reports 1
#define Console1 Serial  // Reports 2
#define Console2 Serial  // Menu
#define Console3 Serial  // Errors
#define Console4 Serial  // Boot messages
