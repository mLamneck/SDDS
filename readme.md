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
        sdds_var(TonOffState,ledSwitch);
	sdds_var(TonOffState,blinkSwitch);
        sdds_var(Tuint16,onTime,sdds::opt::saveval,500);  //flag for eeprom save and give a default value	
        sdds_var(Tuint16,offTime,sdds::opt::saveval,500);
    )
    public:
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

ENUM(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
    Ttimer timer;	//this is added
    public:
        sdds_struct(
            sdds_var(TonOffState,ledSwitch);
            sdds_var(TonOffState,blinkSwitch);
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
    public:
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
* There's not need to move the Tled code into a separate file. We think it's a good practice move components to their own files.
* We nested the led component in another class called userStruct. This is not necessary. We could instead just declare Tled userStruct. Usually you will have multiple components and one root structure collecting it. But it's completely up to you.
* The call to TtaskHandler::handleEvents() in the loop is necessary to run the event handler.
* The declaration of TserialPlainCommHandler makes the structure accessible over serial communication. And that's the beauty of it. You don't have to touch your Led component to make it available. And it doesn't matter if you add/remove variables to your led or if you add more components.
* The declaration TwebCommHandler is only available for ESP32/ESP8266 and will provide a website with a generic user interface. If you are using an ESP, add your WiFi startup code in the setup, oterhwise comment out the lines for webSocket based handling.

## Testing the Example
It's time to finally play around and have fun...

### Build and Upload the code
If you are using Arduino IDE, just can just open the example. 

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

ENUM(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
    Ttimer timer;	//this is added
    public:
        sdds_struct(
            sdds_var(TonOffState,ledSwitch);
            sdds_var(TonOffState,blinkSwitch)
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
    public:
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
   * This command is used to subscribe to the led structure, so that whenever a value changes we get a notification in the serial console. The intial response contains basically all values from our Tled structure:

     ```l 1 0 OFF,OFF,500,500,```

#### Set values
6. Use the command "led.ledSwitch=0/1" or "led.ledSwitch=OFF/ON" to turn the led off/on.
   * Because we have previously subscribed to get change notification in step 4, we receive a notification as a result. If we have skipped step 4 we wouldn't get any response, but the led would still turn on/off.

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
 
 8. If you are using an ESP board and you have enabled the WebSpike and your ESP is connected to the same WiFi as your laptop or smartphone, open your web browser and navigate to http://myEspWithSDDS (if the right column shown is "NULL" you propably have to refresh the page). You have to click on the "***>***" twice to get your Tled structure.
    
    ![image](https://github.com/mLamneck/SDDS/assets/32937082/eb41a075-5ed6-4196-8592-094ed3d7b655)
    
    ![image](https://github.com/mLamneck/SDDS/assets/32937082/e62e3b24-6aec-489c-b156-fe8082607fa2)
    
    ![image](https://github.com/mLamneck/SDDS/assets/32937082/5c995e0b-26c8-460c-81ba-12116949edef)

    To change a value click into the right column and enter or choose a new value. Note that you now have 2 active spikes, the Serial Spike and the Web Spike. You can set values in one of them and see the changes reflected in the other one. More information about [Web Spikes](#web-spike) will be available in the documentaion. 
    
      
 8. Feel free play with led.onTime/offTime as you like...
 9. You can also try to unsubscribe from notifications with the command "U 1" and try if the set commands still work.

## Documentation
to be done...
### The Data Structure
to be done...
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

