# SDDS (Self-Describing-Data-Structure)
A simple and lighweight C++ library to write event driven processes with self generating user interfaces.

## Table of contents
- [Why to use this library](#why-to-use-this-library)
- [Installation](#installation)
  - [Arduino](#arduino)
  - [PlatformIO](#platformio)
- [Example for this documentation](#example-for-this-documentation)
- [Coding the example](#coding-the-example)
  - [Defining the structure](#defining-the-structure)
  - [Reacting to state changes](#reacting-to-state-changes)
  - [Introducing Timers](#introducing-timers)
  - [Putting it all together](#putting-it-all-together)
- [Testing the Example](#testing-the-example)
  - [Build and Upload the code](#build-and-upload-the-code)
  - [Full Example Code](#full-example-code)
  - [Explore Serial Spike](#explore-serial-spike)
  - [Request the type description](#request-the-type-description)
  - [Subscribe to change notification](#subscribe-to-change-notification)
  - [Set values](#set-values)
- [Documentation](#documentation)
  - [The Data Structure](#the-data-structure)
  - [Spikes](#spikes)
     - [Plain protocol](#plain-protocol)
     - [Serial Spike](#serial-spike)
     - [Web Spike](#web-spike)
     - [Udp Spike](#udp-spike)


## Why to use this library
In our opinion one of the most annoying things in software development is that one have to spend more time to provide ways to interact with the software compared to what's the program actually doing. The time needed to develop and test things like the following is massive.

* communication protocols to control the software via
  * serial
  * web
  * udp
  * displays
* save parameters to non-volatile memory

The purpose of this library is to get rid of all that and focus on what's really important, the functionality that has to be implemented. This is done by completely separating the business logic and provide standard interfaces. This way all the points mentioned above have to be done only once in a generic way for all of our projects in what we call [Spikes](#spikes).   

We want to mention, that there are other libraries trying to achieve the same, but they require a lot of setup and the declaration and usage of variables is fairly complex. Another goal of this library is to keep it as simple as possible. We want to declare and use variables like we are used to in regular C++, but gain the benefits on the other hand for free.


## Installation

### Arduino
Clone this repository in your library folder and you are ready to go. If you want to use ESP-only features like [web spikes](#webspike), you also need to install the [Esp Extension](https://github.com/mLamneck/SDDS_ESP_Extension).

### PlatformIO
Add the github link to this repository as a lib dependency to your platformio.ini like in the following example.
```
[env:myEnv]
platform = ...
board = ...
framework = arduino
lib_deps = https://github.com/mLamneck/SDDS.git
```
If you want to use ESP-only features like [web spikes](#webspike) you want to add the [Esp Extension](https://github.com/mLamneck/SDDS_ESP_Extension) instead. This will automatically add the SDDS core library.
```
[env:myEspEnv]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = https://github.com/mLamneck/SDDS_ESP_Extension
```

## Example for this documentation

The topic addressed by this document is fairly abstract. That's why we provide a simple example that will be used along the whole document to show the connection to a real-world scenario. For this purpose, we want to use an advanced led blinking code with the following features:
1. Turn the boards led on/off.
2. Enable/Disable blinking of the led.
3. Adjust the time the led is on/off when blinking.

We use the serial console to trigger/control features. On ESP based boards it's possible to use the smartphone as well.
	
## Coding the example
If you like to have a look at the final implementation jump to the [Testing the Example](#testing-the-example) section right away. 

Otherwise let's start by defining the datastructure for our example. We need the following.
| Name           	| Possible Values  	|
|:-------------		|:-----			|
| ledSwitch 	  	| ON,OFF 		|
| blinkSwitch 	  	| ON,OFF 		|
| onTime/offTime  	| 100-10000 		|

### Defining the structure
```C++
#include "uTypedef.h"
#include "uMultask.h"

ENUM(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
  public:
    sdds_struct(
        sdds_var(TonOffState,ledSwitch,sdds::opt::saveval);
        sdds_var(TonOffState,blinkSwitch,sdds::opt::saveval);
        sdds_var(Tuint16,onTime,sdds::opt::saveval,500);  //flag for eeprom save and give a default value	
        sdds_var(Tuint16,offTime,sdds::opt::saveval,500);
    )
    Tled(){
        pinMode(LED_BUILTIN, OUTPUT);

        //process logic goes here
    }
};
```
We start with the necessary includes followed by an enum definition. Next, we define the necessary process variables for our example by deriving a class from TmenuHandle provided by the lib. Note the only special thing here, is the use of sdds_struct and sdds_var. Don't care about it, it will always look like this. Just note ssds_var has 4 parameters while the last 2 are optional.

`sdds_var(dtype,name[,option,defaulValue])`
1. Data Type
2. name (to be used it the code)
3. option (readonly, saveval, showhex, ...)
4. default value

We will proceed with the logic for the led handling in the constructor of this class.

### Reacting to state changes
So far we have defined our data structure. For the moment just assume that it would be possible to change the value of the variable ledSwitch with a smartphone. We need a way to react to that change, i.e. turn the led on or off. Of course we could periodically check the value but we don't like polling do we? There's a much better way to do it:
```C++
...
Tled(){
  ...
  on(ledSwitch){
    if (ledSwitch == TonOffState::dtype::ON) digitalWrite(LED_BUILTIN,1);
    else digitalWrite(LED_BUILTIN,0);
  };
}
```
This can by read like this: if the value of ledSwitch is written, execute the code in curly braces and this code finally turns the led on/off depending on the value of ledSwitch.

### Introducing Timers

Now we have a basic setup and we can switch the led, but in order to implement blinking of the led we need a mechanism for the timing. Without further doing let's have a look at the final code:

```C++
#include "uTypedef.h"
#include "uMultask.h"

sdds_enum(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
    Ttimer timer;	//this is added
    public:
      sdds_struct(
          sdds_var(TonOffState,ledSwitch,sdds::opt::saveval);
          sdds_var(TonOffState,blinkSwitch,sdds::opt::saveval);
          sdds_var(Tuint16,onTime,sdds::opt::saveval,500);
          sdds_var(Tuint16,offTime,sdds::opt::saveval,500);
      )
      Tled(){
          pinMode(LED_BUILTIN, OUTPUT);

          on(ledSwitch){
              if (ledSwitch == TonOffState::dtype::ON) digitalWrite(LED_BUILTIN,1);
              else digitalWrite(LED_BUILTIN,0);
          };

          // code from here is new
          on(blinkSwitch){
              if (blinkSwitch == TonOffState::dtype::ON) timer.start(0);
              else timer.stop();
          };

          on(timer){
              if (ledSwitch == TonOffState::dtype::ON){
                  ledSwitch = TonOffState::dtype::OFF;
                  timer.start(offTime);
              } 
              else {
                  ledSwitch = TonOffState::dtype::ON;
                  timer.start(onTime);
              }
          };
      }
};
```

Note the definition of the Ttimer at the top of the class. Just like we did with ledSwitch, we defined an event handler for our blinkSwitch. If the switch is on it just starts the timer with the value of 0 which basically means elapse as soon as you can and if the switch is off, we just stop the timer. And how do we react to the elapsed timer? Just like we do with all state changes: on(timer){...}. Here we just toggle the led and start the timer with the according value.

### Putting it all together

Now that we have a fully functional component, probably in a separate file we can use it for example in an Arduino Sketch:

```C++
#include "uLed.h"

class TuserStruct : public TmenuHandle{
    public:
      sdds_struct(
          sdds_var(Tled,led);
      )
      TuserStruct(){
      }
} userStruct;

//make available through serial communication
#include "uPlainCommHandlerArduino.h"
TserialPlainCommHandler serialHandler(userStruct);

//make it available for Websockets
#include "uWebCommHandler.h"
TwebCommHandler webHandler(userStruct);

void setup(){
}

void loop(){
  TtaskHandler::handleEvents();
}

```

There are some things to note here:
* There's not need to move the Tled code into a separate file. We think it's a good practice to move components to their own files.
* We nested the led component in another class called userStruct. This is not necessary. We could instead just declare Tled userStruct. Usually you will have multiple components and one root structure collecting it. But it's completely up to you.
* The call to TtaskHandler::handleEvents() in the loop is necessary to run the event handler.
* The declaration of TserialPlainCommHandler makes the structure accessible over serial communication. And that's the beauty of it. You don't have to touch your Led component to make it available. And it doesn't matter if you add/remove variables to your led or if you add more components.
* The declaration TwebCommHandler is only available for ESP32/ESP8266 and will provide a website with a generic user interface. If you are using an ESP, add your WiFi startup code in the setup, otherwise comment out the lines for webSocket based handling.

## Testing the Example
It's time to finally play around and have fun...

### Build and Upload the code
If you are using Arduino IDE, you can just open the example. 

```File->Examples->SDDS->Led```
### Full Example Code
You can also copy and paste the following complete example. Note the settings on top and adjust to your preference.

```C++
//set to 1 on ESP boards if you want to use the webSpike
//but make sure to have SDDS_ESP_EXTENSION library installed from here https://github.com/mLamneck/SDDS_ESP_Extension
#define USE_WEB_SPIKE 0
#define WLAN_SSID "ENTER_YOUR_SSID_HERE"
#define WLAN_PW "ENTER_YOUR_PASSWORD_HERE"
#define HOSTNAME "myEspWithSDDS"
#define WLAN_AP 0   //to be set if you want to use it as access point instead of connecting to your wifi

#include "uTypedef.h"
#include "uMultask.h"

sdds_enum(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
    Ttimer timer;	//this is added
    public:
        sdds_struct(
            sdds_var(TonOffState,ledSwitch,sdds::opt::saveval);
            sdds_var(TonOffState,blinkSwitch,sdds::opt::saveval);
            sdds_var(Tuint16,onTime,sdds::opt::saveval,500);
            sdds_var(Tuint16,offTime,sdds::opt::saveval,500);
        )
        Tled(){
            pinMode(LED_BUILTIN, OUTPUT);

            on(ledSwitch){
                if (ledSwitch == TonOffState::dtype::ON) digitalWrite(LED_BUILTIN,1);
                else digitalWrite(LED_BUILTIN,0);
            };

            // code from here is new
            on(blinkSwitch){
                if (blinkSwitch == TonOffState::dtype::ON) timer.start(0);
                else timer.stop();
            };

            on(timer){
                if (ledSwitch == TonOffState::dtype::ON){
                    ledSwitch = TonOffState::dtype::OFF;
                    timer.start(offTime);
                } 
                else {
                    ledSwitch = TonOffState::dtype::ON;
                    timer.start(onTime);
                }
            };
        }
};

class TuserStruct : public TmenuHandle{
    public:
      sdds_struct(
          sdds_var(Tled,led);
      )
      TuserStruct(){
      }
} userStruct;

//make available through serial communication
#include "uSerialSpike.h"
TserialSpike serialHandler(userStruct,115200);

#if USE_WEB_SPIKE == 1
//make it available for Websockets
  #include "uWebSpike.h"
  TwebSpike webSpike(userStruct);
#endif

void setup(){
  #if USE_WEB_SPIKE == 1
    WiFi.setHostname(HOSTNAME);
    #if WLAN_AP == 0
      WiFi.begin(WLAN_SSID, WLAN_PW);
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
      }
      // Print ESP Local IP Address
      Serial.println(WiFi.localIP());
    #else
      WiFi.mode(WIFI_AP);
      WiFi.softAP(WLAN_SSID, WLAN_PW);
      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);
    #endif
  #endif
}

void loop(){
  TtaskHandler::handleEvents();
}

```

### Explore Serial Spike
The serial spike uses the [Plain protocol](#plain-protocol) specified in the documentation. Let's try it with the following steps.

1. Build and upload the code to your board.
2. Open the Serial Monitor (default baudrate 115200)
   
#### Request the type description
3. Send the command "T"
   * The response will look like the following. It's a full descpription of the datastructure you have declared within your code, inluding options, the current value, possible values for enums, ... This information can be used by a software to build a generic user interface like showcased with the webSpike for ESP-based boards. For now let's continue to explore the fundamentals in the serial monitor.
     
     ```t [{"type":66,"opt":0,"name":"led","value":[{"type":1,"opt":0,"name":"ledSwitch","value":"OFF","enums":["OFF","ON"]},...```

#### Subscribe to change notification

4. Send the command "L 1 led"
   * This command is used to subscribe to the led structure, so that whenever a value changes we get a notification in the serial console. The initial response contains basically all values from our Tled structure:

     ```l 1 0 OFF,OFF,500,500,```

#### Set values
6. Use the command "led.ledSwitch=0/1" or "led.ledSwitch=OFF/ON" to turn the led off/on.
   * Because we have previously subscribed to get change notifications in step 4, we receive a notification as a result. If we have skipped step 4 we wouldn't get any response, but the led would still turn on/off.

     ```
     l 1 0 OFF,
     l 1 0 ON,
     ```
     
7. Use the command "led.blinkSwitch=0/1" or "led.blinkSwitch=OFF/ON" to enable the automatic led toggle.
    * Again because of our subscription initiated in step 4, we first get a change notification of the blinkSwitch and the ledSwitch in the first line of the response and later on we get a notifications every time the led turns on/off.

      ```
      l 1 0 OFF,ON,
      l 1 0 ON,
      l 1 0 OFF,
      l 1 0 ON,
      l 1 0 OFF,
      ```
 
 8. If you are using an ESP board and you have enabled the WebSpike and your ESP is connected to the same WiFi as your laptop or smartphone, open your web browser and navigate to http://myEspWithSDDS (if the right column shown is "NULL" you propably have to refresh the page). You have to click on the "***>***" twice to get your Tled structure. If you are using your ESP as Access Point, you probably have to use http://ip instead of http://myEspWithSDDS. The ip you should use is printed in the serial monitor on startup.
    
    ![image](https://github.com/mLamneck/SDDS/assets/32937082/eb41a075-5ed6-4196-8592-094ed3d7b655)
    
    ![image](https://github.com/mLamneck/SDDS/assets/32937082/e62e3b24-6aec-489c-b156-fe8082607fa2)
    
    ![image](https://github.com/mLamneck/SDDS/assets/32937082/5c995e0b-26c8-460c-81ba-12116949edef)

    To change a value click into the right column and enter or choose a new value. Note that you now have 2 active spikes, the Serial Spike and the Web Spike. You can set values in one of them and see the changes reflected in the other one. More information about [Web Spikes](#web-spike) will be available in the documentaion. 
    
      
 8. Feel free play with led.onTime/offTime as you like...
 9. You can also try to unsubscribe from notifications with the command "U 1" and try if the set commands still work.

## Documentation
to be done...
### The Data Structure
In the middle of SDDS there is a global self describing datastructure organized in a hierachy called the tree. This tree is the only interface needed. You have to change your mindset from api driven interfaces, where you have a set of functions and methods you need to call. In sdds everything is done by manipulating variables. Typically you organize your programm into components. A component is a sub data structure within the global tree combined with some functionality. Our [Tled](#introducing-timers) class from the Example chapter is such a component. It's responsible for managing the boards led state. It's the master of the led if you want so. Instead of calling a function to turn the led on/off or let it periodically toggle (which would be complicated, unless you'd use some library like freeRtos to do it in a task), you change the value of the led fields: 
```
led.blinkSwitch=1
```
On the other hand you can always react to changes of variables in the tree by using the statement 
```
on(led.blinkSwitch){ 
	//some code executed on state change
};
```
Another example which is more focused on producing data would be a readout of the adc. It could probably look like this:
```C++
class Tadc : public TmenuHandle{
    Ttimer timer;
    public:
        sdds_struct(
            sdds_var(Tuint16,value,sdds::opt::readonly);
            sdds_var(Tuint8,pin,sdds::opt::saveval);
            sdds_var(Tuint16,readInterval,sdds::opt::saveval,100);
        )
        Tadc(int _pin){
	    on(pin){
		pinMode(pin, INPUT);
		timer.start(0);
            };

            on(timer){
              value = analogRead(_pin); 
              timer.start(readInterval); 
            };
        }
};
```
The beauty of this is that this code completely seperated from the rest of you code. It's responsible for one thing, to read out the adc with a given, adjustable interval. You don't care who get's notified of a change in the adc value nor who is setting the readInterval. There could be 10 clients subscribed due to different communication channels or none. And if nobody's interested in the adc value the line
```
value = analogRead(_pin); 
```
just set's the value in memory instead of sending it useless to the serial console or some websocket, udp or whatever. The library takes care for you. Imagin it like a secretary. You give the datastructure and say: Hey look this is my structure. Please provide it for everbody's interested in it and please notify me if somebody is changing the value of the variable "pin". The variable "value" is readonly, so nobody from outside of this machine is allowed to change it. The variables "pin" and "readInterval" should be stored to a non-volatile memory if a save is triggered. Now some code on the same machine could do something like this.
```C++
  ...
  //within another component
  on(adc0.value){
    if (adc0.value > 2000){
	//turn off something here.
    }
  }; 
```
This is an example of a component on the local machine beeing a client and reacting to state changes. For clients outside of the machine we use what we call [Spikes](#spikes) illustrated in the documentation.

### Spikes
to be done
#### Plain protocol
to be done
#### Serial Spike
to be done
#### Web Spike
to be done
#### Udp Spike
to be done

