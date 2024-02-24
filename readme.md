# SDDS (Self-Describing-Data-Structure)
A simple and lighweight C++ library to write event driven processes with self generating user interfaces.


## Installation

### Arduino
Clone this repository in your library folder and you are ready to go. Open arduino studio IDE and goto file->examples->sdds->serialComm

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

## Why to use this library

The core of this library introcudes a concept of self describing data structures that provides standardized methods to acccess it. The purpose is to get rid of all the code neccessary to build a transport layer necessary to control or paramerize the written software from remote. Process variables can be flagged to be stored in a permament storage and will can loaded on the next startup.

There are other libraries doing that, but they require a lot of setup and the declaration and usage of variables is complex. The goal of this library is to keep it as simple as possible. We want to declare and use variables like we are used to in regular C++, but gain the benefits on the other hand for free.

## Example for this documentation

The topic adressed by this document is fairly abstract. That's why we provide a simple example that will be used a long the whole document to show the connection to a real world scenario. For this purpose we want to use an advanced led blinking code with the following features:
1. Turn the boards led on/off.
2. Enable/Disable blinking of the led.
3. Adjust the time the led is on/off when blinking.

We use the serial console to trigger/control features. On ESP based boards it's possible to use the smartphone as well.

	
## Coding the example
Let's start by defining the datastructure for our example. We need the following.
| Name           	| Possible Values  	|
|:-------------		|:-----			|
| Led Switch 	  	| ON,OFF 		|
| Blink Switch 	  	| ON,OFF 		|
| OnTime/OffTime  	| 100-10000 		|

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
We start with the necessary includes followed by a enum definition. Next we define the necessary process variables for our example by deriving a class from TmenuHandle provided by the lib. Note the only special thing here, is the use of sdds_struct and sdds_var. Don't care about it, it will always look like this. Just note ssds_var has 4 parameters while the last 2 are optional.

`sdds_var(dtype,name[,option,defaulValue])`
1. Data Type
2. name (to be used it the code)
3. option (readonly, saveval, showhes, ...)
4. default value

We will proceed with the logic for the led handling in the constructor of this class.

### Coding the logic
The following code goes into the constructor of Tled.




### Components
A component is a datastructure plus logic responsible for a specific task. It does not depend on the rest of your code and if it's used via serial console, smartphone or any other communication channel. It can be outsourced into an include file and can be used in several projects and platforms. In a real world scenario you will have a lot of such components all together forming a hierachy of components.

