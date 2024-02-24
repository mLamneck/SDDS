# SDDS (Self-Describing-Data-Structure)
A simple and lighweight C++ library to write event driven processes with self generating user interfaces.


## Why to use this library
In our opinion one of the most annoying things in software development is that one have to spend more time to provide ways to interact with the software compared to what's the program actually doing.The time needed to develop and test things like the following is massive.

* communication protocols to control the software via
  * serial
  * web
  * udp
  * displays
* save parameters to non-volatile memory

The purpose of this library is to get rid of all that and focus on what's really important, the functionality that has to be implemented. This is done by completely separating the business logic and provide standard interfaces. This way all the points mentioned above have to be done only once in a generic way for all of our projects.   

We want to mention, that there are other libraries trying to achieve the same, but they require a lot of setup and the declaration and usage of variables is fairly complex. Another goal of this library is to keep it as simple as possible. We want to declare and use variables like we are used to in regular C++, but gain the benefits on the other hand for free.


## Installation

### Arduino
Clone this repository in your library folder and you are ready to go. Open Arduino studio IDE and goto file->examples->sdds->serialComm

### PlatformIO
Add the github link to this repository as a lib dependency to your platformio.ini like so:
```
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    https://github.com/mLamneck/SDDS.git
```

## Example for this documentation

The topic addressed by this document is fairly abstract. That's why we provide a simple example that will be used along the whole document to show the connection to a real-world scenario. For this purpose, we want to use an advanced led blinking code with the following features:
1. Turn the boards led on/off.
2. Enable/Disable blinking of the led.
3. Adjust the time the led is on/off when blinking.

We use the serial console to trigger/control features. On ESP based boards it's possible to use the smartphone as well.
	
## Coding the example
Let's start by defining the datastructure for our example. We need the following.
| Name           	| Possible Values  	|
|:-------------		|:-----			|
| ledSwitch 	  	| ON,OFF 		|
| blinkSwitch 	  	| ON,OFF 		|
| onTime/offTime  	| 100-10000 		|

Let's code it right away:
### Defining a structure
```C++
#include "uTypedef.h"
#include "uMultask.h"

ENUM(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
  public:
    sdds_struct(
        sdds_var(TonOffState,ledSwitch);
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

### Timers and the full component

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

void setup(){
}

void loop(){
  TtaskHandler::handleEvents();
}

```

There are some things to note here
* There's not need to move the Tled code into a separate file. We think it's a good practice move components to their own files.
* We nested the led component in another class called userStruct. This is not necessary. We could instead just declare Tled userStruct. Usually you will have multiple components and one root structure collecting it. But it's completely up to you.
* The declaration of TserialPlainCommHandler makes the structure accessible over serial communication. And that's the beauty of it. You don't have to touch your Led component to make it available. And it doesn't matter if you add/remove variables to your led or if you add more components.
* The call to TtaskHandler::handleEvents() in the loop is necessary to run the event handler.


