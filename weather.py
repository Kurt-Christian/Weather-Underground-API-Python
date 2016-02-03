import urllib2
import json
import time

#Makes a call to the WU API for hourly weather data.
#You must request and use your own WU API key, which is free through signup.
#Be aware that there are limitations for all strata of accounts with WU API requests.
#Free accounts are limited to 10 requests per hour and 500 per day.
#Will determine your location based on your IP address, see WU API documentation for finding other locations
def getLaterData():
    urlLater = urllib2.urlopen('http://api.wunderground.com/api/INSERT_YOUR_OWN_WU_KEY_HERE/hourly/q/autoip.json')
    json_stringLater = urlLater.read()
    dataLater = json.loads(json_stringLater)
    urlLater.close()
    return dataLater

#Makes a call to the WU API for current weather data
def getCurrentData():
    urlCurrent = urllib2.urlopen('http://api.wunderground.com/api/INSERT_YOUR_OWN_WU_KEY_HERE/conditions/q/autoip.json')
    json_stringCurrent = urlCurrent.read()
    dataCurrent = json.loads(json_stringCurrent)
    return dataCurrent
    
#Makes a call to WU API for current alert data
def getAlertData():
    urlAlert = urllib2.urlopen('http://api.wunderground.com/api/INSERT_YOUR_OWN_WU_KEY_HERE/alerts/q/autoip.json')
    json_stringAlert = urlAlert.read()
    dataAlert = json.loads(json_stringAlert)
    return dataAlert

def current():
        #gets current data from def
        dataCurrent = getCurrentData()
        currentTemp = dataCurrent['current_observation']['temp_f']
        #rounding
        currentTemp = int(round(currentTemp))
        currentSkyActivity = dataCurrent['current_observation']['icon'].upper() #or can use 'icon' instead of 'weather'
        alert = alerts()
        print 'Current temperature: ', currentTemp , "F"
        print 'Current conditions: ', currentSkyActivity
        options[currentSkyActivity]()
        alertOptions[alert]()
        print
        
def laterPlus2():
        #gets hourly data from def
        dataLater = getLaterData()
        currentPlus2Temp = dataLater['hourly_forecast'][2]['temp']['english']              
        laterPlus2SkyActivity = dataLater['hourly_forecast'][2]['icon'].upper()
        alert = alerts()
        print 'In 2 hours temperature: ', currentPlus2Temp, 'F'
        print 'In 2 hours conditions: ', laterPlus2SkyActivity
        options[laterPlus2SkyActivity]()
        alertOptions[alert]()
        print
        
def laterPlus4():
        #gets hourly data from def
        dataLater = getLaterData()
        currentPlus4Temp = dataLater['hourly_forecast'][4]['temp']['english'] 
        currentPlus4SkyActivity = dataLater['hourly_forecast'][4]['icon'].upper()
        alert = alerts()
        print 'In 4 hours temperature: ', currentPlus4Temp, 'F'
        print 'In 4 hours conditions: ', currentPlus4SkyActivity
        options[currentPlus4SkyActivity]()
        alertOptions[alert]()
        print
        
        
def laterPlus6():  
        #gets hourly data from def
        dataLater = getLaterData()
        currentPlus6Temp = dataLater['hourly_forecast'][6]['temp']['english']
        currentPlus6SkyActivity = dataLater['hourly_forecast'][6]['icon'].upper()
        alert = alerts()
        print 'In 6 hours temperature: ', currentPlus6Temp, 'F'
        print 'In 6 hours conditions: ', currentPlus6SkyActivity
        options[currentPlus6SkyActivity]()
        alertOptions[alert]()  
        print   
   
    
##### Get Alerts ######
#this will get weather alerts such as severe thunderstorms, winter weather, winds, etc
#I did not add all severe weather conditions, if you want to add more, lookup the strings which will
#be output by the JSON (look for 'type') and add them to the dictionary 'alertOptions'
def alerts():
        dataAlert = getAlertData()
        try:
            currentAlert = dataAlert['alerts'][0]['type']
        except:
                currentAlert = 'noAlert'
        return currentAlert
#######################

#for now I am using print statements to simulate the actions which the arduino should be taking 
#with the processes which are running
def sunIcon():
    #light up sun icon with arduino software
    print 'sun icon light up'
def sunCloudIcon():
    #light up sun/cloud icon with arduino software
    print 'sun cloud icon light up'
def cloudIcon():
    #light up cloud icon with arduino software
    print 'cloud icon light up'
def lightngingCloudIcon():
    #light up lightning cloud icon with arduino software
    print 'lightning cloud icon light up'
def rainCloudIcon():
    #light up cloud with rain icon with arduino software
    print 'rain cloud icon light up'
def snowIcon():
    #light up snow icon with arduino software
    print 'snow icon light up'
def clearMoonIcon():
    #light up clear moon icon with arduino software
    print 'clear moon icon light up'
def windyIcon():
    #light up windy icon with arduino software
    print 'windy icon light up'
def cloudyMoonIcon():
    #light up cloudy moon icon with arduino software
    print 'cloudy moon icon light up'
    
########## Alert def ########## 
def weatherAlert():
    print "Weather alert! Exclamation mark on board illuminated!"
    
def noWeatherAlert():
    print 'No weather alert at this time'
###############################

#keyword strings which will be returned by JSON requests for current and later sky conditions
options = {'CLEAR' : sunIcon,
           'SUNNY' : sunIcon,
           'OVERCAST' : sunCloudIcon,
           'PARTLY CLOUDY' : sunCloudIcon,
           'MOSTLYSUNNY' : sunCloudIcon,
           'PARTLYSUNNY' : sunCloudIcon,
           'PARTLYCLOUDY' : sunCloudIcon,
           'Scattered Clouds' : sunCloudIcon,
           'MOSTLYCLOUDY' : cloudIcon,
           'OVERCAST' : cloudIcon,
           'MOSTLY CLOUDY' : cloudIcon,
           'MOSTLYCLOUDY' : cloudIcon,
           'CLOUDY' : cloudIcon,
           'FOG' : cloudIcon,
           'CHANCERAIN' : rainCloudIcon,
           'RAIN' : rainCloudIcon,
           }

#various alert keyword strings which will be returned by JSON calls using 'type'
alertOptions = {
               'TOR' : weatherAlert, #tornado alert
               'TOW' : weatherAlert, #tornado watch
               'WRN' : weatherAlert, #severe t-storm warning
               'SEW' : weatherAlert, #severe t-storm watch
               'WIN' : weatherAlert, #winter weather advisory
               'FLO' : weatherAlert, #flood warning
               'WAT' : weatherAlert, #flood watch / statement
               'WND' : weatherAlert, #high wind advisory
               'SVR' : weatherAlert, #severe weather statement
               'FOG' : weatherAlert, #dense fog
               'SPE' : weatherAlert, #special weather statement
               'PUB' : weatherAlert, #public information statement
               'noAlert' : noWeatherAlert
               }

#infinite loop to run on arduino until process is ended by user
while(1):
    current()
    laterPlus2()
    laterPlus4()
    laterPlus6()
    time.sleep(3600)
