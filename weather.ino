#include <Bridge.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

char currentTempC[2] = "";
char currentSkyIconC[8] = "";
char currentAlertC[2] = "";
char currentHourC[2] = "";

char laterPlus2TempC[2] = "";
char laterPlus2SkyIconC[8] = "";
char laterPlus2AlertC[2] = "";
char laterPlus2HourC[2] = "";

char laterPlus4TempC[2] = "";
char laterPlus4SkyIconC[8] = "";
char laterPlus4AlertC[2] = "";
char laterPlus4HourC[2] = "";

char dummyVarC[2] = ""; // did this to fix bug with data spilling into variable below from varibale above

char laterPlus6TempC[2] = "";
char laterPlus6SkyIconC[8] = "";
char laterPlus6AlertC[2] = "";
char laterPlus6HourC[2] = "";


int count = 0;
boolean gateKeeper = true;
int timeCycle = 36;  //37 - 1, because N - 1, where N is the amount of time in minutes to wait until getting new weather data.
                         //30 minute wait because each 'weather page' will display for 15 seconds and 
                         //there are 4 'weather pages', therefore 4pages * 15sec = 60 seconds = 1 minute
                         //we want program to refresh data after 36 minutes /or/ 36 one minute cycles
                         
void setup() 
{
  memset(currentTempC,       0, 2);
  memset(currentSkyIconC,    0, 8);
  memset(currentAlertC,      0, 2);
  memset(currentHourC,       0, 2);
  
  memset(laterPlus2TempC,    0, 2);
  memset(laterPlus2SkyIconC, 0, 8);
  memset(laterPlus2AlertC,   0, 2);
  memset(laterPlus2HourC,    0, 2);
  
  memset(laterPlus4TempC,    0, 2);
  memset(laterPlus4SkyIconC, 0, 8);
  memset(laterPlus4AlertC,   0, 2);
  memset(laterPlus4HourC,    0, 2);
  
  memset(dummyVarC, 0, 2); //dummy variable
  
  memset(laterPlus6TempC,    0, 2);
  memset(laterPlus6SkyIconC, 0, 8);
  memset(laterPlus6AlertC,   0 , 2);
  memset(laterPlus6HourC,    0, 2);
  
  pinMode(6, OUTPUT);
  Bridge.begin();
  strip.begin();
  strip.show();      // Initialize all pixels to 'off'
  
  delay(70000);      // wait 70 secs for linux to boot and connect to wifi
  
  //launches the python script on the linux side
  Process myProcess; //initialize with constructor
  myProcess.runShellCommandAsynchronously("/root/weatherpane.py");
  //
  
  delay(10000); //wait for10 secs for python to post data before C code begins requesting it
}

void loop() 
{
  while (true)
  {
      getAllData();
      Serial.println("just refreshed weather data");
      gateKeeper = true;
      count = 0;
      while(gateKeeper)
      {
        //current weather page (15 secs)
        currentWeatherPage(atoi(currentTempC), atoi(currentSkyIconC), atoi(currentAlertC), atoi(currentHourC));
        //plus 2 hours weather page (15 seconds)
        laterPlus2WeatherPage(atoi(laterPlus2TempC), atoi(laterPlus2SkyIconC), atoi(laterPlus2AlertC), atoi(laterPlus2HourC));
        //plus 4 hours weather page (15 secs)
        laterPlus4WeatherPage(atoi(laterPlus4TempC), atoi(laterPlus4SkyIconC), atoi(laterPlus4AlertC), atoi(laterPlus4HourC));
        //plus 6 hours weather page (15 secs)
        laterPlus6WeatherPage(atoi(laterPlus6TempC), atoi(laterPlus6SkyIconC), atoi(laterPlus6AlertC), atoi(laterPlus6HourC));
        count++;
        if (count == timeCycle)
        {
            gateKeeper = false;
            Serial.println("gateKeeper turned to false, referesh data from python");
         }
       }
  }
}

//get all data from linino side (python)
void getAllData()
{
  //getting current info
  Bridge.get("currentTemp",         currentTempC, 2);
  Bridge.get("currentSkyIcon",      currentSkyIconC, 8);
  Bridge.get("currentAlert",        currentAlertC, 2);
  Bridge.get("currentHour",         currentHourC, 2);
  
  //getting current + 2 hours
  Bridge.get("laterPlus2Temp",      laterPlus2TempC, 2);
  Bridge.get("laterPlus2SkyIcon",   laterPlus2SkyIconC, 8);
  Bridge.get("laterPlus2Alert",     laterPlus2AlertC, 2);
  Bridge.get("laterPlus2HourC",     laterPlus2HourC, 2);
  
  //getting current + 4 hours
  Bridge.get("laterPlus4temp",      laterPlus4TempC, 2);
  Bridge.get("laterPlus4SkyIcon",   laterPlus4SkyIconC, 8);
  Bridge.get("laterPlus4Alert",     laterPlus4AlertC, 2);
  Bridge.get("laterPlus4Hour",      laterPlus4HourC, 2);
  
  //getting current + 6 hours
  Bridge.get("laterPlus6Temp",      laterPlus6TempC, 2);
  Bridge.get("laterPlus6SkyIcon",   laterPlus6SkyIconC, 8);
  Bridge.get("laterPlus6Alert",     laterPlus6AlertC, 2);
  Bridge.get("laterPlus6Hour",      laterPlus6HourC, 2);  
}


//method for current weather "page" to be displayed for 15 seconds
void currentWeatherPage(int currentTemp, int currentSkyIcon, int currentAlert, int currentHour)
{
  //this will turn the N O W icon on on the weather board
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 0, 0);
  
  int currentTensTemp = 0;
  int currentOnesTemp = 0;
  currentTensTemp = lightTensTemp(currentTemp);
  currentOnesTemp = lightOnesTemp(currentTemp);
  
  //lighting up the sky icons
  turnOnSkyIcon(currentSkyIcon, currentHour);
  //lighting up the alert icon
  if (currentAlert == 4748)
  {
    strip.setPixelColor(47, 0, 0, 255);
    strip.setPixelColor(48, 0, 0, 255);
    strip.show();
  }
  //show this "page" and led arrangement for 15 seconds (15,000) milliseconds
  delay(15000);
  // TURN OFF //
  //turning off this LED display before moving onto the next "page" of weather
  //
  strip.setPixelColor(currentTensTemp   , 0, 0, 0);
  strip.show();
  strip.setPixelColor(currentOnesTemp   , 0, 0, 0);
  strip.show();
  //turn off currentSkyIcon
  turnOffSkyIcon(currentSkyIcon, currentHour);
  //turn off currentAlert
  if (currentAlert == 4748)
  {
    strip.setPixelColor(47, 0, 0, 0);
    strip.setPixelColor(48, 0, 0, 0);
    strip.show();
  }
}

//plus 2 weather page
void laterPlus2WeatherPage(int plus2Temp, int plus2SkyIcon, int plus2Alert, int plus2Hour)
{
  //this will turn the N O W on 
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 0, 0);
  strip.setPixelColor(3, 255, 0, 0); // setting the +2 illuminated
  //dividing by 10 in order to get tens place to light up
  //need to check if variable currentTemp is affected
  
  int plus2TensTemp = 0;
  int plus2OnesTemp = 0;
  plus2TensTemp = lightTensTemp(plus2Temp);
  plus2OnesTemp = lightOnesTemp(plus2Temp);
  
  //lighting up the sky icons
  turnOnSkyIcon(plus2SkyIcon, plus2Hour);
  //lighting up the alert icon
  if (plus2Alert == 4748)
  {
    strip.setPixelColor(plus2Alert, strip.Color(0,0,255));
    strip.show();
  }
  //show this "page" and led arrangement for 15 seconds (15,000) milliseconds
  delay(15000);
  // TURN OFF
  //turning off this LED display before moving onto the next "page" of weather
  //
  strip.setPixelColor(plus2TensTemp   , 0, 0, 0);
  strip.show();
  strip.setPixelColor(plus2OnesTemp   , 0, 0, 0);
  strip.show();
  //turn off sky icon
  turnOffSkyIcon(plus2SkyIcon, plus2Hour);
  //turn off alert
  if (plus2Alert == 4748)
  {
    strip.setPixelColor(47, 0, 0, 0);
    strip.setPixelColor(48, 0, 0, 0);
    strip.show();
  }
  //turning off the +2
  strip.setPixelColor(3 , 0, 0, 0);
  strip.show();
}

//plus4 weather page
void laterPlus4WeatherPage(int plus4Temp, int plus4SkyIcon, int plus4Alert, int plus4Hour)
{
  //this will turn the N O W on 
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 0, 0);
  strip.setPixelColor(4, 255, 0, 0); //illuminate the +4  
  //dividing by 10 in order to get tens place to light up
  //need to check if variable currentTemp is affected
  
  int plus4TensTemp = 0;
  int plus4OnesTemp = 0;
  plus4TensTemp = lightTensTemp(plus4Temp);
  plus4OnesTemp = lightOnesTemp(plus4Temp);
  
  //lighting up the sky icons
  turnOnSkyIcon(plus4SkyIcon, plus4Hour);
  
  //lighting up the alert icon
  if (plus4Alert == 4748)
  {
    strip.setPixelColor(47, strip.Color(0,0,255));
    strip.setPixelColor(48, strip.Color(0,0,255));
    strip.show();
  }
  //show this "page" and led arrangement for 15 seconds (15,000) milliseconds
  delay(15000);
  //TURN OFF
  //turning off this LED display before moving onto the next "page" of weather
  //
  strip.setPixelColor(plus4TensTemp   , 0, 0, 0);
  strip.show();
  strip.setPixelColor(plus4OnesTemp   , 0, 0, 0);
  strip.show();
  //turn off skyicons
  turnOffSkyIcon(plus4SkyIcon, plus4Hour);
  //turn off alert
  if(plus4Alert == 4748)
  {
    strip.setPixelColor(47, strip.Color(0,0,0));
    strip.setPixelColor(48, strip.Color(0,0,0));
    strip.show();
  }
  //turn off +4
  strip.setPixelColor(4 , 0, 0, 0);
  strip.show();
}



//plus 6 weather page
void laterPlus6WeatherPage(int plus6Temp, int plus6SkyIcon, int plus6Alert, int plus6Hour)
{
  //this will turn the N O W on 
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 0, 0);
  strip.setPixelColor(5, 255, 0, 0); //illuminate the +6
  //dividing by 10 in order to get tens place to light up
  //need to check if variable currentTemp is affected
  
  
  int plus6TensTemp = 0;
  int plus6OnesTemp = 0;
  plus6TensTemp = lightTensTemp(plus6Temp);
  plus6OnesTemp = lightOnesTemp(plus6Temp);
  
  
  //lighting up the sky icons
  turnOnSkyIcon(plus6SkyIcon, plus6Hour);
  //lighting up the alert icon
  if (plus6Alert == 4748)
  {
    strip.setPixelColor(47, strip.Color(0,0,255));
    strip.setPixelColor(48, strip.Color(0,0,255));
    strip.show();
  }
  //show this "page" and led arrangement for 15 seconds (15,000) milliseconds
  delay(15000);
  //turning off this LED display before moving onto the next "page" of weather
  strip.setPixelColor(plus6TensTemp   , 0, 0, 0);
  strip.show();
  strip.setPixelColor(plus6OnesTemp   , 0, 0, 0);
  strip.show();
  turnOffSkyIcon(plus6SkyIcon, plus6Hour);
  //turn off alert
  if (plus6Alert == 4748)
  {
    strip.setPixelColor(47, strip.Color(0,0,0));
    strip.setPixelColor(48, strip.Color(0,0,0));
    strip.show();
  }
  //turn off +6
  strip.setPixelColor(5               , 0, 0, 0);
  strip.show();
}

//turns the current weather icons on depending on sky conditions
void turnOnSkyIcon(int skyIconIntParam, int hour)
{
  switch(skyIconIntParam)
  {
    //sunny
    case 343536:    if (hour > 18 || hour < 7) //moony
                    {
                      strip.setPixelColor(26, 0, 0, 255);
                      strip.setPixelColor(27, 0, 0, 255);
                      strip.show();
                    }
                    else //sunny
                    {
                      strip.setPixelColor(34, 0, 0, 255);
                      strip.setPixelColor(35, 0, 0, 255);
                      strip.setPixelColor(36, 0, 0, 255);
                      strip.show();
                    }
                    break;
    //cloudy    
    case 313233:    strip.setPixelColor(31, 0, 0, 255);
                    strip.setPixelColor(32, 0, 0, 255);
                    strip.setPixelColor(33, 0, 0, 255);
                    strip.show();
                    break;
    //cloud/sun
    case 282930:    if (hour > 18 || hour < 7) //cloud/moon
                    {
                      strip.setPixelColor(40, 0, 0, 255);
                      strip.setPixelColor(41, 0, 0, 255);
                      strip.setPixelColor(42, 0, 0, 255);
                      strip.show();
                    }
                    else //cloud/sun
                    {
                      strip.setPixelColor(28, 0, 0, 255);
                      strip.setPixelColor(29, 0, 0, 255);
                      strip.setPixelColor(30, 0, 0, 255);
                      strip.show();
                    }
                    break;
    //rain w/ cloud
    case 373839:    strip.setPixelColor(37, 0, 0, 255);
                    strip.setPixelColor(38, 0, 0, 255);
                    strip.setPixelColor(39, 0, 0, 255);
                    strip.show();
                    break;
    //thunderstorm cloud rain
    case 43444546:  strip.setPixelColor(43, 0, 0, 255);
                    strip.setPixelColor(44, 0, 0, 255);
                    strip.setPixelColor(45, 0, 0, 255);
                    strip.setPixelColor(46, 0, 0, 255);
                    strip.show();
                    break;
    //snow w/ cloud
    case 49505152:  strip.setPixelColor(49, 0, 0, 255);
                    strip.setPixelColor(50, 0, 0, 255);
                    strip.setPixelColor(51, 0, 0, 255);
                    strip.setPixelColor(52, 0, 0, 255);
                    strip.show();
                    break;
  }
}

//turns off the sky conditions 
void turnOffSkyIcon(int skyIconIntParam, int hour)
{
  switch(skyIconIntParam)
  {
    //sunny
    case 343536:    if (hour > 18 || hour < 7) //moony
                    {
                      strip.setPixelColor(26, 0, 0, 0);
                      strip.setPixelColor(27, 0, 0, 0);
                      strip.show();
                    }
                    else //sunny
                    {
                      strip.setPixelColor(34, 0, 0, 0);
                      strip.setPixelColor(35, 0, 0, 0);
                      strip.setPixelColor(36, 0, 0, 0);
                      strip.show();
                    }
                    break;
    //cloudy    
    case 313233:    strip.setPixelColor(31, 0, 0, 0);
                    strip.setPixelColor(32, 0, 0, 0);
                    strip.setPixelColor(33, 0, 0, 0);
                    strip.show();
                    break;
    //cloud/sun
    case 282930:    if (hour > 18 || hour < 7) //cloud/moon
                    {
                      strip.setPixelColor(40, 0, 0, 0);
                      strip.setPixelColor(41, 0, 0, 0);
                      strip.setPixelColor(42, 0, 0, 0);
                      strip.show();
                    }
                    else //cloud/sun
                    {
                      strip.setPixelColor(28, 0, 0, 0);
                      strip.setPixelColor(29, 0, 0, 0);
                      strip.setPixelColor(30, 0, 0, 0);
                      strip.show();
                    }
                    break;
    //rain w/ cloud
    case 373839:    strip.setPixelColor(37, 0, 0, 0);
                    strip.setPixelColor(38, 0, 0, 0);
                    strip.setPixelColor(39, 0, 0, 0);
                    strip.show();
                    break;
    //thunderstorm cloud rain
    case 43444546:  strip.setPixelColor(43, 0, 0, 0);
                    strip.setPixelColor(44, 0, 0, 0);
                    strip.setPixelColor(45, 0, 0, 0);
                    strip.setPixelColor(46, 0, 0, 0);
                    strip.show();
                    break;
    //snow w/ cloud
    case 49505152:  strip.setPixelColor(49, 0, 0, 0);
                    strip.setPixelColor(50, 0, 0, 0);
                    strip.setPixelColor(51, 0, 0, 0);
                    strip.setPixelColor(52, 0, 0, 0);
                    strip.show();
                    break;
  }
}

//lights the temperature on the tens place on the weather board
int lightTensTemp(int tempParam)
{
 int tensTemp = 0;
  tensTemp = tempParam / 10;
  switch (tensTemp)
  {
    case 0:  strip.setPixelColor(15, 0, 0, 255);
             strip.show();
             tensTemp =  15;
             break;
             
    case 1:  strip.setPixelColor(14, 0, 0, 255);
             strip.show();
             tensTemp = 14;
             break;
             
    case 2:  strip.setPixelColor(13, 0, 0, 255);
             strip.show();
             tensTemp = 13;
             break;
             
    case 3:  strip.setPixelColor(12, 0, 0, 255);
             strip.show();
             tensTemp = 12;
             break;
             
    case 4:  strip.setPixelColor(11, 0, 0, 255);
             strip.show();
             tensTemp = 11;
             break;
             
    case 5:  strip.setPixelColor(10, 0, 0, 255);
             strip.show();
             tensTemp = 10;
             break;
             
    case 6:  strip.setPixelColor(9, 0, 0, 255);
             strip.show();
             tensTemp = 9;
             break;
             
    case 7:  strip.setPixelColor(8, 0, 0, 255);
             strip.show();
             tensTemp = 8;
             break;
             
    case 8:  strip.setPixelColor(7, 0, 0, 255);
             strip.show();
             tensTemp = 7;
             break;
             
    case 9:  strip.setPixelColor(6, 0, 0, 255);
             strip.show();
             tensTemp = 6;
             break;
  } 
  return tensTemp;
}

//lights the ones temp on the weather board
int lightOnesTemp(int tempParam)
{
  int onesTemp = 0;
  onesTemp = tempParam % 10;
  switch (onesTemp)
  {
    case 0:  strip.setPixelColor(16, 0, 0, 255);
             strip.show();
             onesTemp = 16;
             break;
             
    case 1:  strip.setPixelColor(17, 0, 0, 255);
             strip.show();
             onesTemp = 17;
             break;
             
    case 2:  strip.setPixelColor(18, 0, 0, 255);
             strip.show();
             onesTemp = 18;
             break;
             
    case 3:  strip.setPixelColor(19, 0, 0, 255);
             strip.show();
             onesTemp = 19;
             break;
             
    case 4:  strip.setPixelColor(20, 0, 0, 255);
             strip.show();
             onesTemp = 20;
             break;
             
    case 5:  strip.setPixelColor(21, 0, 0, 255);
             strip.show();
             onesTemp = 21;
             break;
             
    case 6:  strip.setPixelColor(22, 0, 0, 255);
             strip.show();
             onesTemp = 22;
             break;
             
    case 7:  strip.setPixelColor(23, 0, 0, 255);
             strip.show();
             onesTemp = 23;
             break;
             
    case 8:  strip.setPixelColor(24, 0, 0, 255);
             strip.show();
             onesTemp = 24;
             break;
             
    case 9:  strip.setPixelColor(25, 0, 0, 255);
             strip.show();
             onesTemp = 25;
             break;
  }
  return onesTemp;
}

