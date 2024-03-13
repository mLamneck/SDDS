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
  - [Save Parameters](#save-parameters)
- [Documentation](#documentation)
  - [The Data Structure](#the-data-structure)
  - [Parameter Save](#parameter-save)
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

The purpose of this library is to get rid of all that and focus on what's really important, the functionality that has to be implemented. This is done by completely separating the business logic and provide standard interfaces. This way all the points mentioned above have to be done only once in a generic way for all of our projects in what we call [Spikes](#spikes). If you are not sure what we are talking about, check out or [Coding a full-featured WiFi Manager in 120 lines](#https://github.com/mLamneck/SDDS_ESP_Extension?tab=readme-ov-file#coding-a-full-featured-wifi-manager-in-120-lines) where we showcase how you can tremendously speed up your developement process with SDDS.

Another goal of this library is to keep it as simple as possible. We want to declare and use variables like we are used to in regular C++, but gain the benefits on the other hand for free.


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
4. Save settings to non-volatile memory to be available again after restarting the board

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
        sdds_var(TonOffState,ledSwitch,sdds::opt::saveval)
        sdds_var(TonOffState,blinkSwitch,sdds::opt::saveval)
        sdds_var(Tuint16,onTime,sdds::opt::saveval,500)  //flag for eeprom save and give a default value	
        sdds_var(Tuint16,offTime,sdds::opt::saveval,500)
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
          sdds_var(TonOffState,ledSwitch,sdds::opt::saveval)
          sdds_var(TonOffState,blinkSwitch,sdds::opt::saveval)
          sdds_var(Tuint16,onTime,sdds::opt::saveval,500)
          sdds_var(Tuint16,offTime,sdds::opt::saveval,500)
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

Now that we have a fully functional component, we can use it for example in an Arduino Sketch:

```C++
#include "uTypedef.h"
#include "uMultask.h"
#include "uParamSave.h"

//...
//your Tled component
//...

class TuserStruct : public TmenuHandle{
    sdds_struct(
        sdds_var(Tled,led)
        sdds_var(TparamSaveMenu,params)
    )
    TuserStruct(){
        //you application code goes here... 
    }
} userStruct;

#include "uSerialSpike.h"
TserialSpike serialHandler(userStruct,115200);

void setup(){

}

void loop(){
  TtaskHandler::handleEvents();
}
```

There are some things to note here:

* We nested the led component in another class called userStruct. This is not necessary. We could instead just declare ```Tled led``` and replace all occurrences of userStruct with led. Usually you will have multiple components and one root structure collecting it. But it's completely up to you. 
* Another reason to nest the Tled component is that we've implemented the parameter save right away. We promised to implement a parameter save right? That's done by simply including the ```TparamSaveMenu``` somewhere in you tree. You will see it in action in the testing section.
* The call to TtaskHandler::handleEvents() in the loop is necessary to run the event handler.
* The declaration of TserialSpike makes the structure accessible over serial communication. And that's the beauty of it. You don't have to touch your Led component to make it available. And it doesn't matter if you add/remove variables to your led or if you add more components. Same holds true for the parameter save.

Notes for ESP devices:
* The declaration TwebSpike is available for ESP32/ESP8266 and will provide a website with a generic user interface.
* We have also included a WifiManager. Read more about it In the [SDDS_ESP_EXTENSION](https://github.com/mLamneck/SDDS_ESP_Extension) repository. But basically it let you configure the ssid and password to connect to your local network by creating an Access Point with the website.

## Testing the Example
It's time to finally play around and have fun...

### Build and Upload the code
If you are using Arduino IDE, you can just open the example. 

```File->Examples->SDDS->Led```
### Full Example Code
You can also copy and paste the following complete example.

```C++
#include "uTypedef.h"
#include "uMultask.h"
#include "uParamSave.h"

sdds_enum(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
    Ttimer timer;
    public:
      sdds_struct(
          sdds_var(TonOffState,ledSwitch,sdds::opt::saveval)
          sdds_var(TonOffState,blinkSwitch,sdds::opt::saveval)
          sdds_var(Tuint16,onTime,sdds::opt::saveval,500)
          sdds_var(Tuint16,offTime,sdds::opt::saveval,500)
      )
      Tled(){
          pinMode(LED_BUILTIN, OUTPUT);

          on(ledSwitch){
              if (ledSwitch == TonOffState::dtype::ON) digitalWrite(LED_BUILTIN,1);
              else digitalWrite(LED_BUILTIN,0);
          };

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
    sdds_struct(
        sdds_var(Tled,led)
        sdds_var(TparamSaveMenu,params)
    )
    TuserStruct(){
        //you application code goes here... 
    }
} userStruct;

#include "uSerialSpike.h"
TserialSpike serialHandler(userStruct,115200);

void setup(){

}

void loop(){
  TtaskHandler::handleEvents();
}
```

### Explore Serial Spike
The serial spike uses the [Plain protocol](#plain-protocol) specified in the documentation to populate the datastructure we've build over the default serial. We want to point out here that the following section is not how it's supposed to be used. It's made to have a software running on the host machine to provide a userinterface using the protocol to read/write the data like showcased [here](https://github.com/mLamneck/SDDS_ESP_Extension?tab=readme-ov-file#introducing-the-user-interface) for ESP boards. There is a full featured software for serial communicatio, but unfortunatelay it's not available for public. It still make sense to checkout the following instructions to get an idea how spikes word under the hood. So let's walk through...

1. Build and upload the code to your board.
2. Open the Serial Monitor (default baudrate 115200)
   
#### Request the type description
3. Send the command "T"
    * The response will look like the following. It's a full descpription of the datastructure you have declared within your code, inluding options, the current value, possible values for enums, ... This information can be used by a software to build a generic user interface like showcased with the webSpike for ESP-based boards. For now let's continue to explore the fundamentals in the serial monitor.
     
       ```t 0 [{"type":66,"opt":0,"name":"led","value":[{"type":49,"opt":0,"name":"ledSwitch","value":"OFF","enums":["OFF","ON"]},...```
    
    Without specifying a path after the ```T``` you get the whole datastructure. You can also ask more specific i.e. ```T led``` which will give you the description of the component we've created.

#### Subscribe to change notification

4. Send the command "L 1 led"
   * This command is used to subscribe to the led structure, so that whenever a value changes we get a notification in the serial console. The initial response contains basically all values from our Tled structure:

     ```l 1 0 ["OFF","OFF",500,500]```

#### Set values
6. Use the command "led.ledSwitch=0/1" or "led.ledSwitch=OFF/ON" to turn the led off/on.
   * Because we have previously subscribed to get change notifications in step 4, we receive a notification as a result. If we have skipped step 4 we wouldn't get any response, but the led would still turn on/off.

     ```
     l 1 0 ["OFF"]
     l 1 0 ["ON"]
     ```
     
7. Use the command ```led.blinkSwitch=0/1``` or ```led.blinkSwitch=OFF/ON``` to enable the automatic led toggle.
    * Again because of our subscription initiated in step 4, we first get a change notification of the blinkSwitch and the ledSwitch in the first line of the response and later on we get a notifications every time the led turns on/off.

      ```
      l 1 0 ["OFF","ON"]
      l 1 0 ["ON"]
      l 1 0 ["OFF"]
      l 1 0 ["ON"]
      l 1 0 ["OFF"]
      ```

8. Feel free play with led.onTime/offTime as you like...
9. You can also try to unsubscribe from notifications with the command "U 1" and try if the set commands still work.

 #### Save Parameters
 10. Let's try to save parameters. Turn on the periodic blinking of the led. Enter the ```L 2 params``` to enable change notifications for the params menu. This is not necessary but in order to see a response we better do it. Enter ```params.action=save``` in the serial console. The response will look something like the following. The first ```___``` is the action variable which you have set to ```save``` with your command. This triggeres the save of the menu and when the command is done, it is set back to ```___``` by the ```TparamSaveMenu``` component. The second ```___``` means no error occured and the third value, is the number of bytes that have been save to the non-volatile memory, in this case 8 (2x1 byte for the ledSwitch/blinkSwitch, 2x2 bytes for on/offTime, and 2 bytes for internal management). Read more about [Parameter Saving](#parameter-save) in the Documentation. The response will look like this:
 ```
l 2 0 ["___","___",8]
 ```
 
## Documentation

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
            sdds_var(Tuint16,value,sdds::opt::readonly)
            sdds_var(Tuint8,pin,sdds::opt::saveval)
            sdds_var(Tuint16,readInterval,sdds::opt::saveval,100)
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

### Data Types
Sdds provides primtive types as well as composed types and enums.

#### Primitives
We support the usual primitve datatypes like displayed in the following table. The TypeID field is what you see as the type fields in the resonse of ```T``` command.

| Type      | TypeID/hex  | TypeID/dec  | Min         | Max         |
| -         | :-:         | :-:         | -----------:| -----------:|
| Tuint8    | 0x01        | 1           | 0           | 255         |
| Tuint16   | 0x02        | 2           | 0           | 65535       |
| Tuint32   | 0x04        | 4           | 4294967295  |             |
| Tint8     | 0x11        | 17          | -128        | 127         |
| Tint16    | 0x12        | 18          |-32768       | 32767       |
| Tint32    | 0x14        | 20          |-2147483648  | 2147483647  |
| Ffloat32  | 0x24        | 36          |             |             |

Not all operators are implemented yet. 
```C++
class TmyStruct : public TmenuHandle{
    sdds_struct(
        sdds_var(Tuint8,cnt);
    )
    TmyStruct(){
      cnt = 5;    //works
      cnt++;      //not implemented yet, use cnt=cnt+1
      cnt+=2;     //not implemented yet, use cnt=cnt+2
      //...
    }
};

```
It's not a problem to implement these things but not done at the moment.

#### Other Types

| Type      | TypeID/hex  | TypeID/dec  |
| -         | :-:         | :-:         |
| Ttime     | 0x06        | 6           |
| Tstring   | 0x81        | 129         |
| Tstruct   | 0x42        | 66          |
| Enum      | 0x31        | 49          |


#### Strings
There's not much to say about strings. Under the hood a dynamic string representation is used. The parameter save however is limited to 255 byte length.

#### Enums
If you don't know about enums, it is basically an integer type that has a more readable representation. This is for example useful for status variables.

```C++
sdds_enum(on,off) TonOffState;
...
sdds_struct(
  sdds_var(TonOffState,switch,sdds::opt::saveval,TonOffState::e::on)
)
```

toDo: There's a lot to write about enums and it could be a library on their own...

#### Structs
Structs are a collection of primitive values bundled together. Structs can be nested to form the [tree](#the-data-structure). You can also derive a struct from a base struct.

```C++
//declare a struct
class TmyStruct : public TmenuHandle{
    sdds_struct(
        sdds_var(Tuint8,cnt);
        sdds_var(Tuint32,max);
    )
};

//nest within another struct
class TrootStruct : public TmenuHandle{
    sdds_struct(
        sdds_var(TmyStruct,myStruct);
    )
};

//derive from TmyStruct and add a time variable
class TderivedStruct : public TmyStruct{
    sdds_struct(
        sdds_var(Ttime,time);
    )
};
```

### Parameter Save
In most application you will find the need to store some of the parameters you are using to a non-volatile memory to be available after a restart. In sdds this is quite simple. Just specify the ```saveval``` when declaring a variable end include the builtin ```TparamSaveMenu``` somewhere in your [tree](#the-data-structure) and you are ready to go. This way, whenever you trigger a parameter save (```params.action=save```), all parameters you have flagged for save are stored and automatically reloaded after a power up. A full example can be found in [Putting it all together section](#putting-it-all-together).

#### Changing in structure
You can always add/remove variables or restructure your components. Just be aware that you probably loose your saved values after you change the tree. You can use the ```T``` command to rertieve the current state of your tree before you flash a new version to have a copy of you old version. 

Values from the tree are saved binary so when you change something, this can lead to weird values when loading. Therefore we are calculating a checksum of the tree and only execute a param load when the stored checksum matches the one of your tree. To build the checksum we only use the types and not the names. So it's possible to change names without loosing your config. 

Parameters will not be loaded under the following circumstances.
* the type of a saved variable has changed
* the order of saved variables have changed
* variables flagged with saveval are added
* variables flagged with saveval are removed

Parameters will still load even if you
* change the name of a saved variable
* add variables not flagged fo save
* remove variables not flagged for save

#### Size in the non-volatile memory
The size required for the parameter save is basically the binary size of all the variables that have to be saved plus 2 bytes for the checksum and a version number.

After a save/load of parameters you will find the used space in the size variable of the ```TsaveParamsMenu```.


### Spikes

Like mentioned before, in the middle of ssds is the [tree](#the-data-structure). An interface that can populate this data structure over a communication channel like *serial* or a *websocket*  is what we call a spike. It needs 2 things.
* a protocol
* a communication channel

#### Plain protocol
The plain protocol is human readable format. All commands are terminated with a ```\n```. At the moment this is the only protocol we are using but there will be a binary protocol as well in the future.

The most obvious commands are to simply read and write values.
```C++
  led.blinkSwitch         //replies with led.blinkSwitch=ON/OFF
  led.blinkSwitch?        //replies with led.blinkSwitch=ON/OFF
  led.blinkSwitch=ON      //set the value. No reply
  led.blinkSwitch=1       //set the value. No replpy
```

For more advanced usage we need the following:
 * retrieve type information
 * (un)subscribe to change notifications
 * error handling

Find a list of all available cmds in the following table.

| type            | func | port | data           | example
|-                |:-----|:----:| --             | -
| type request    | T    |  %d+ | path           | T 11 led
| type answer     | t    |  %d+ | json           | T 11 [{"type": 1, "opt": 0, "value":1},...]
| link request    | L    |  %d+ | path           | L 33 led
| link answer     | l    |  %d+ | first \| json  | l 33 0 [1,1.4,"stringVal",...]
| unlink request  | U    |  %d+ |-               | U 3
| unlink answer   | u    |  %d+ |-               | u 3
| error           | E    |  %d+ | err string     | E 5 invPort
| boot notify     | B    |  0   | -              |

You can find the error codes in the [source file](src/uPlainCommErrors.h).


#### Serial Spike

The Serial Spike uses the [plain protocol](#plain-protocol) to publish the data over the default serial.

#### Web Spike

The Web Spike does actually 2 things.
* It uses the [plain protocol](#plain-protocol) to publish data over websockets
* It provides a website with a generic ui to interact with the data.
Web Spikes are only available on ESP platforms with the [SDDS_ESP_EXTENSION](https://github.com/mLamneck/SDDS_ESP_Extension).

#### Udp Spike

At the moment not implemented for ESP. We use it for debugging when running sdds on windows with mingw.

