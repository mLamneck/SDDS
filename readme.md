# SDDS (Self-Describing-Data-Structure)
A lightweight, dependency-free C++ library to write event-driven processes with self-generating user interfaces.

## Table of contents
- [Why use this library](#why-use-this-library)
- [Installation](#installation)
  - [Arduino](#arduino)
  - [PlatformIO](#platformio)
- [Supported platforms](#supported-platforms)
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
  - [The Data Structure (Tree)](#the-data-structure)
  - [Declaring variables](#declaring-variables)
  - [Calling functions with SDDS variables](#calling-functions-with-sdds-variables)
  - [Calling functions with SDDS enums](#calling-funtions-with-sdds-enums)
  - [Data types](#data-types)
    - [Primitve types](#primitives)
    - [Time](#ttime)    
    - [Enums](#enums)
    - [Structs](#structs)
  - [Parameter Save](#parameter-save)
  - [Spikes](#spikes)
     - [Plain protocol](#plain-protocol)
     - [Serial Spike](#serial-spike)
     - [Web Spike](#web-spike)
     - [UDP Spike](#udp-spike)
- [How to contribute](#how-to-contribute)
  - [Write Documentation](#write-documentation)
  - [Help to organize the repository](#help-to-organization-of-the-repository)
  - [Code new Spikes](#code-new-spikes)
  - [Develop components](#develop-components-for-our-public-component-library)
  - [Test and feedback](#test-and-give-feedback)


## Why use this library
In our opinion, one of the most annoying things in software development is that one has to spend more time providing ways to interact with the software compared to what the program actually does. The time needed to develop and test things like the following is massive:

* Communication protocols to control the software via:
  * Serial
  * Web
  * UDP
  * Displays
* Save parameters to non-volatile memory

The purpose of this library is to get rid of all that and focus on what's really important: the functionality that has to be implemented. This is done by completely separating the business logic and providing standard interfaces. This way, all the points mentioned above have to be done only once in a generic way for all of our projects in what we call [Spikes](#spikes). If you are not sure what we are talking about, check out our [Coding a full-featured WiFi Manager in 120 lines](https://github.com/mLamneck/SDDS_ESP_Extension?tab=readme-ov-file#coding-a-full-featured-wifi-manager-in-120-lines) where we showcase how you can tremendously speed up your development process with SDDS.

Another goal of this library is to keep it as simple as possible. We want to declare and use variables like we are used to in regular C++, but gain the benefits on the other hand for free.


## Installation

### Arduino
Clone this repository into your library folder, and you are ready to go. If you want to use ESP-only features like [web spikes](#web-spike), you also need to install the [ESP Extension](https://github.com/mLamneck/SDDS_ESP_Extension).


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

## Supported platforms

The library is intended to be highly scalable, i.e., capable of running with minimal RAM and/or ROM, as well as on more powerful platforms like the ESP32 or different TEENSY boards. It's also intended to be independent of Arduino. So basically, it's possible to use it within Espressif IDF or STM32Cube, but we have not done that so far, and there would certainly be some adjustments needed. We will address this on demand. So far, the library has been tested on the following boards, using the Arduino platform:

- Teensy 3.2
- ESP32
  - ESP-WROOM-32
  - WEMOS LOLIN32 Lite
- ESP8266
  - LOLIN D1 mini
  - LOLIN D1 mini lite

We would appreciate feedback to further extend this list.

### Known issues

At the moment, we do not support AVR CPUs because of AVR-GCC's inability to capture variables in lambda functions. We will work on this issue, but it may lead to slight changes in syntax when using AVR CPUs. Nevertheless, there will be a syntax that works for all platforms. Perhaps we can implement the missing feature on our own using some type-traits magic.

## Example for this documentation

The topic addressed by this document is fairly abstract. That's why we provide a simple example that will be used throughout the document to show the connection to a real-world scenario. For this purpose, we want to use an advanced LED blinking code with the following features:
1. Turn the board's LED on/off.
2. Enable/disable blinking of the LED.
3. Adjust the time the LED is on/off when blinking.
4. Save settings to non-volatile memory to be available again after restarting the board.

We use the serial console to trigger/control features. On ESP-based boards, it's possible to use the smartphone as well.
    
## Coding the example
If you'd like to have a look at the final implementation, jump to the [Testing the Example](#testing-the-example) section right away.

Otherwise, let's start by defining the data structure for our example. We need the following:

| Name           	  | Possible Values  	|
|:-------------		  |:-----			        |
| ledSwitch 	  	  | ON, OFF 		      |
| blinkSwitch 	  	| ON, OFF 	      	|
| onTime/offTime  	| 100-10000 		    |

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
We start with the necessary includes followed by an enum definition. Next, we define the necessary process variables for our example by deriving a class from `TmenuHandle` provided by the library. Note the only special thing here is the use of `sdds_struct` and `sdds_var`. Don't care about it; it will always look like this. Just note `sdds_var` has 4 parameters while the last 2 are optional.

`sdds_var(dtype,name[,option,defaulValue])`
1. Data Type
2. name (to be used it the code)
3. option (readonly, saveval, showhex, ...)
4. default value

We will proceed with the logic for the LED handling in the constructor of this class.

### Reacting to state changes
So far, we have defined our data structure. For the moment, just assume that it would be possible to change the value of the variable `ledSwitch` with a smartphone. We need a way to react to that change, i.e., turn the LED on or off. Of course, we could periodically check the value, but we don't like polling, do we? There's a much better way to do it:
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
This can be read like this: if the value of `ledSwitch` is written, execute the code in curly braces, and this code finally turns the LED on/off depending on the value of `ledSwitch`.

### Introducing Timers

Now we have a basic setup and we can switch the LED, but in order to implement blinking of the LED, we need a mechanism for timing. Without further ado, let's have a look at the final code:

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

Note the definition of the `Ttimer` at the top of the class. Just like we did with `ledSwitch`, we defined an event handler for our `blinkSwitch`. If the switch is on, it just starts the timer with the value of 0, which basically means elapse as soon as you can. If the switch is off, we just stop the timer. And how do we react to the elapsed timer? Just like we do with all state changes: `on(timer){...}`. Here we just toggle the LED and start the timer with the corresponding value.

### Putting it all together

Now that we have a fully functional component, we can use it, for example, in an Arduino Sketch:

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

* We nested the LED component in another class called `userStruct`. This is not necessary. We could instead just declare `Tled led` and replace all occurrences of `userStruct` with `led`. Usually, you will have multiple components and one root structure collecting them. But it's completely up to you.
* Another reason to nest the `Tled` component is that we've implemented the parameter save right away. We promised to implement a parameter save, right? That's done by simply including the `TparamSaveMenu` somewhere in your tree. You will see it in action in the testing section.
* The call to `TtaskHandler::handleEvents()` in the loop is necessary to run the event handler.
* The declaration of `TserialSpike` makes the structure accessible over serial communication. And that's the beauty of it. You don't have to touch your LED component to make it available. And it doesn't matter if you add/remove variables to your LED or if you add more components. The same holds true for the parameter save.

## Testing the Example
It's time to finally play around and have fun...

### Build and Upload the Code
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
              if (ledSwitch == TonOffState::dtype::ON) digitalWrite(LED_BUILTIN,0);
              else digitalWrite(LED_BUILTIN,1);
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
  public:
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
The serial spike uses the [Plain protocol](#plain-protocol) specified in the documentation to populate the data structure we've built over the default serial. We want to point out here that the following section is not how it's supposed to be used. It's made to have software running on the host machine to provide a user interface using the protocol to read/write the data like showcased [here](https://github.com/mLamneck/SDDS_ESP_Extension?tab=readme-ov-file#introducing-the-user-interface) for ESP boards. There is a fully featured software for serial communication, but unfortunately, it's not available for the public. It still makes sense to check out the following instructions to get an idea of how spikes work under the hood. So let's walk through...

1. Build and upload the code to your board.
2. Open the Serial Monitor (default baudrate 115200).
   
#### Request the type description
3. Send the command ```T```.
    * The response will look like the following. It's a full description of the data structure you have declared within your code, including options, the current value, possible values for enums, ... This information can be used by software to build a generic user interface like showcased with the webSpike for ESP-based boards. For now, let's continue to explore the fundamentals in the serial monitor.
     
       ```t 0 [{"type":66,"opt":0,"name":"led","value":[{"type":49,"opt":0,"name":"ledSwitch","value":"OFF","enums":["OFF","ON"]},...```
    
    Without specifying a path after the ```T```, you get the whole data structure. You can also ask more specifically, i.e., ```T led```, which will give you the description of the component we've created.

#### Subscribe to change notification

4. Send the command ```L 1 led```.
   * This command is used to subscribe to the LED structure, so that whenever a value changes, we get a notification in the serial console. The initial response contains basically all values from our `Tled` structure:

     ```l 1 0 ["OFF","OFF",500,500]```

#### Set Values
5. Use the command ```led.ledSwitch=0/1``` or ```led.ledSwitch=OFF/ON``` to turn the LED off/on.
   * Because we have previously subscribed to get change notifications in step 4, we receive a notification as a result. If we have skipped step 4, we wouldn't get any response, but the LED would still turn on/off.

     ```
     l 1 0 ["OFF"]
     l 1 0 ["ON"]
     ```
     
6. Use the command ```led.blinkSwitch=0/1``` or ```led.blinkSwitch=OFF/ON``` to enable the automatic LED toggle.
    * Again because of our subscription initiated in step 4, we first get a change notification of the blinkSwitch and the ledSwitch in the first line of the response and later on we get notifications every time the LED turns on/off.

      ```
      l 1 0 ["OFF","ON"]
      l 1 0 ["ON"]
      l 1 0 ["OFF"]
      l 1 0 ["ON"]
      l 1 0 ["OFF"]
      ```

7. Feel free to play with `led.onTime`/`offTime` as you like...
8. You can also try to unsubscribe from notifications with the command "U 1" and try if the set commands still work.

#### Save Parameters
9. Let's try to save parameters. Turn on the periodic blinking of the LED. Enter the command ```L 2 params``` to enable change notifications for the params menu. This is not necessary but in order to see a response we better do it. Enter ```params.action=save``` in the serial console. The response will look something like the following. The first ```___``` is the action variable which you have set to ```save``` with your command. This triggers the save of the menu and when the command is done, it is set back to ```___``` by the `TparamSaveMenu` component. The second ```___``` means no error occurred and the third value is the number of bytes that have been saved to the non-volatile memory, in this case 8 (2x1 byte for the ledSwitch/blinkSwitch, 2x2 bytes for on/offTime, and 2 bytes for internal management). Read more about [Parameter Saving](#parameter-save) in the Documentation. The response will look like this:
 ```
l 2 0 ["___","___",8]
 ```
 
## Documentation

As always, when time is a limiting factor, you keep things like documentation and unit testing short in order to be productive. However, the best library is useless without proper documentation. We do our best in the following chapters to address the most important things. Regarding unit tests, there are a few for critical functionalities, and we hope to find some developers with experience in this field to help us do a better job.

### The Data Structure
In the middle of SDDS, there is a global self-describing data structure organized in a hierarchy called the tree. This tree is the only interface needed. You have to change your mindset from API-driven interfaces, where you have a set of functions and methods you need to call. In SDDS, everything is done by manipulating variables. Typically, you organize your program into components. A component is a sub data structure within the global tree combined with some functionality. Our [Tled](#introducing-timers) class from the Example chapter is such a component. It's responsible for managing the board's LED state. It's the master of the LED if you want. Instead of calling a function to turn the LED on/off or let it periodically toggle (which would be complicated unless you'd use some library like FreeRTOS to do it in a task), you change the value of the LED fields: 
```
led.blinkSwitch=1
```
On the other hand, you can always react to changes of variables in the tree by using the statement.
```C++
on(led.blinkSwitch){ 
	//some code executed on state change
};
```
Another example which is more focused on producing data would be a readout of the ADC. It could probably look like this:
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
The beauty of this is that this code is completely separated from the rest of your code. It's responsible for one thing: to read out the ADC with a given, adjustable interval. You don't care who gets notified of a change in the ADC value nor who is setting the readInterval. There could be 10 clients subscribed due to different communication channels or none. And if nobody's interested in the ADC value, the line
```C++
value = analogRead(_pin); 
```
just sets the value in memory instead of sending it uselessly to the serial console or some websocket, UDP, or whatever. The library takes care of it for you. Imagine it like a secretary. You give the data structure and say: "Hey look, this is my structure. Please provide it for everybody interested in it and please notify me if somebody is changing the value of the variable 'pin'." The variable "value" is read-only, so nobody from outside of this machine is allowed to change it. The variables "pin" and "readInterval" should be stored to a non-volatile memory if a save is triggered. Now some code on the same machine could do something like this.
```C++
  ...
  //within another component
  on(adc0.value){
    if (adc0.value > 2000){
	//turn off something here.
    }
  }; 
```
This is an example of a component on the local machine being a client and reacting to state changes. For clients outside of the machine, we use what we call [Spikes](#spikes) illustrated in the documentation.

### Declaring Variables

In SDDS, variables are declared using `sdds_var(dtype, name[, option, defaultValue])` inside a `sdds_struct(...)` block inside a class derived from `TmenuHandle`, i.e.
```C++
class TuserStruct : public TmenuHandle{
    sdds_struct(
        sdds_var(Tuint8,cnt);
        sdds_var(Tfloat32,someValue);
        //...
    )
};
```
You will always group variables that belong together in such a structure and add some functionality to form a component. The macro `sdds_var` has four parameters, while the last two are optional.

1. Data Type (A list of available types can be found [here](#data-types))
2. name (to be used in the code)
3. option (readonly, saveval, showhex, ... use `sdds::opt::` for autocompletion; multiple options can be combined with a logical OR, or preferably with `sdds_joinOpt(opt1,opt2,...)`)
4. default value

### Calling Functions with SDDS Variables

When passing variables declared with `sdds_var(...)`, there is nothing special to take care of. You can pass a variable by value or reference like you would do in regular C++. However, there are some things to note. Consider the following code:
```C++
void incByValue(Tuint8 _val){
    _val++;
}

void incByValueMoreEfficient(Tuint8::dtype _val){
    _val++;
}

void incByRef(Tuint8& _val){
    _val++;
}

class TuserStruct : public TmenuHandle{
    public:
        sdds_struct(
            sdds_var(Tuint8,cnt);
        )
        TuserStruct(){
            on(cnt){
                debug::log(cnt.to_string().c_str());
            };
            
            //this will log 1
            cnt = 1;

            //this won't log anything
            incByValue(cnt);
            incByValueMoreEfficient(cnt);

            //this will log 2
            incByRef(cnt);
        }
} userStruct;
```
Here we have declared one variable called `cnt` of type `uint8`. We install an event handler (`on`) to log any write access. We first set it to `1` and then pass the variable to 3 different functions. All functions just increment the given value by 1. The first 2 functions are:
- `incByValue(Tuint8 _val)`
- `incByValueMoreEfficient(Tuint8::dtype _val)`

They receive the variable `_val` by value (a copy of it). The main difference between the two is that the first one receives a copy of an object which controls the access to the underlying datatype, i.e., uint8, while the latter one takes the underlying data type uint8 directly. It's not much of a difference. In the first case, a copy constructor has to be called, which just makes a copy of the underlying data type. However, it uses more stack because it has to create a copy of the object `cnt`, which is not necessary here. So if you want to have it most efficient, use the latter one, but it's not much of a difference. Instead of `Tuint8::dtype`, you could use `uint8_t`, but it's neat to use it like this.

### Calling Funtions with SDDS Enums

The usage of enums as function arguments is basically the same. But there is more to say. Again, consider the following code. Note the line:

`_enum = _enum == TonOff::e::off ? TonOff::e::on : TonOff::e::off;`

This line is just toggling the enum's value if you are not familiar with this C++ syntax.
```C++
sdds_enum(on,off) TonOff;

void changeEnumByVal(TonOff _enum){
    _enum = _enum == TonOff::e::off ? TonOff::e::on : TonOff::e::off;
}

void changeEnumByValMoreEfficient(TonOff::dtype _enum){
    _enum = _enum == TonOff::e::off ? TonOff::e::on : TonOff::e::off;
}

void changeEnumByRef(TonOff& _enum){
    _enum = _enum == TonOff::e::off ? TonOff::e::on : TonOff::e::off;
}

void passEnumConstAndLog(TonOff::dtype _enum){
    debug::log(TonOff::c_str(_enum));
}

class TuserStruct : public TmenuHandle{
    public:
        sdds_struct(
            sdds_var(TonOff,mySwitch)
        )
        TuserStruct(){
            on(mySwitch){
                debug::log(mySwitch.c_str());
            };

            //this will log "on"
            mySwitch = TonOff::e::on;

            //won't lof something
            changeEnumByVal(mySwitch);
            changeEnumByValMoreEfficient(mySwitch);

            //this will log "off"
            changeEnumByRef(mySwitch);

            //this will log "off" again
            passEnumConstAndLog(mySwitch);
        }
} userStruct;
```
Note how handy it is to log the state of the enum as readable text, i.e., "on" or "off," instead of 0 or 1. This would be impossible with a built-in C++ enum. The usage of the first 3 functions:

- `changeEnumByVal(mySwith)`
- `changeEnumByValMoreEfficient(mySwitch)`
- `changeEnumByRef(mySwitch)`

is equivalent to the functions described for the more primitive values [here](#calling-functions-with-sdds-variables). However, you might come across the need to pass an enum constant to a function and log its value in readable text as well. The problem is that in this case, the function receives a pure C++ class enum, and as described earlier, it's impossible to get the string representation out of an enum's value in C++. However, because we created a SDDS enum class, we can use it to do the translation:
```C++
void passEnumConstAndLog(TonOff::dtype _enum){
    debug::log(TonOff::c_str(_enum));
}
```
The `TonOff:dtype` in the parameter definition translates into the underlying datatype: 

```C++
enum class internalName {off,on}
```
However, the `TonOff` type we have declared with `sdds_enum` is smart enough to translate values back to their string representation using its class method.

```C++ 
TonOff::c_str(_enum)
```

### Data Types
SDDS provides primitive types as well as composed types and enums.

#### Primitives
We support the usual primitive data types as displayed in the following table. The TypeID field is what you see as the type fields in the response of the ```T``` command.

| Type      | TypeID/hex  | TypeID/dec  | Min         | Max         |
| -         | :-:         | :-:         | -----------:| -----------:|
| Tuint8    | 0x01        | 1           | 0           | 255         |
| Tuint16   | 0x02        | 2           | 0           | 65535       |
| Tuint32   | 0x04        | 4           | 0           | 4294967295  |
| Tint8     | 0x11        | 17          | -128        | 127         |
| Tint16    | 0x12        | 18          |-32768       | 32767       |
| Tint32    | 0x14        | 20          |-2147483648  | 2147483647  |
| Ffloat32  | 0x24        | 36          |             |             |


#### Other Types

| Type      | TypeID/hex  | TypeID/dec  |
| -         | :-:         | :-:         |
| Ttime     | 0x06        | 6           |
| Tstring   | 0x81        | 129         |
| Tstruct   | 0x42        | 66          |
| Enum      | 0x31        | 49          |

#### Ttime
to be done... 

#### Strings
There's not much to say about strings. Under the hood, a dynamic string representation is used. However, the parameter save is limited to a 255-byte length.

#### Enums
If you don't know about enums, it is basically an integer type that has a more readable representation. This is, for example, useful for status variables.

```C++
sdds_enum(on,off) TonOffState;
...
sdds_struct(
  sdds_var(TonOffState,mySwitch)
)
```
this declaration is the equivalent to C++ builtin `class enum` like
```C++
enum class TonOffState1 {on,off};
struct{
  TonOffState1 mySwitch;
};
```
However, it is much more powerful because of the capability to translate the numeric value back into its string representation, which is impossible in native C++. Read more about the usage of enums [here](#calling-functions-with-sdds-enums). The only thing to take further note of here is that you have to use `TonOffState::e::on/off` instead of just `TonOffState::on/off` for a regular C++ class enum.

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
In most applications, you will find the need to store some of the parameters you are using to a non-volatile memory to be available after a restart. In SDDS, this is quite simple. Just specify the `saveval` when declaring a variable and include the built-in `TparamSaveMenu` somewhere in your [tree](#the-data-structure), and you are ready to go. This way, whenever you trigger a parameter save (`params.action=save`), all parameters you have flagged for save are stored and automatically reloaded after a power-up. A full example can be found in the [Putting it all together section](#putting-it-all-together).

#### Changes in Structure
You can always add/remove variables or restructure your components. Just be aware that you probably lose your saved values after you change the tree. You can use the `T` command to retrieve the current state of your tree before you flash a new version to have a copy of your old version.

Values from the tree are saved binary so when you change something, this can lead to weird values when loading. Therefore, we are calculating a checksum of the tree and only execute a param load when the stored checksum matches the one of your tree. To build the checksum, we only use the types and not the names. So it's possible to change names without losing your config.

Parameters will not be loaded under the following circumstances:
* the type of a saved variable has changed
* the order of saved variables has changed
* variables flagged with `saveval` are added
* variables flagged with `saveval` are removed

Parameters will still load even if you:
* change the name of a saved variable
* add variables not flagged for save
* remove variables not flagged for save

#### Size in the Non-volatile Memory
The size required for the parameter save is basically the binary size of all the variables that have to be saved plus 2 bytes for the checksum and a version number.

After a save/load of parameters, you will find the used space in the size variable of the `TsaveParamsMenu`.

### Spikes

As mentioned before, in the middle of SDDS is the [tree](#the-data-structure). An interface that can populate this data structure over a communication channel like *serial* or a *websocket* is what we call a spike. It needs 2 things:
* a protocol
* a communication channel

#### Plain Protocol
The plain protocol is a human-readable format. All commands are terminated with a `\n`. At the moment, this is the only protocol we are using, but there will be a binary protocol as well in the future.

The most obvious commands are to simply read and write values.
```C++
  led.blinkSwitch         //replies with led.blinkSwitch=ON/OFF
  led.blinkSwitch?        //replies with led.blinkSwitch=ON/OFF
  led.blinkSwitch=ON      //set the value. No reply
  led.blinkSwitch=1       //set the value. No replpy
```

For more advanced usage, we need the following:
 * retrieve type information
 * (un)subscribe to change notifications
 * error handling

Find a list of all available commands in the following table.

| Type            | Func | Port | Data           | Example |
|-----------------|------|------|----------------|---------|
| Type request    | T    |  %d+ | Path           | T 11 led |
| Type answer     | t    |  %d+ | JSON           | T 11 [{"type": 1, "opt": 0, "value":1},...] |
| Link request    | L    |  %d+ | Path           | L 33 led |
| Link answer     | l    |  %d+ | First \| JSON  | l 33 0 [1,1.4,"stringVal",...] |
| Unlink request  | U    |  %d+ | -              | U 3 |
| Unlink answer   | u    |  %d+ | -              | u 3 |
| Error           | E    |  %d+ | Error string   | E 5 invPort |
| Boot notify     | B    |  0   | -              |

You can find the error codes in the [source file](src/uPlainCommErrors.h).


#### Serial Spike

The Serial Spike uses the [plain protocol](#plain-protocol) to publish the data over the default serial.

#### Web Spike

The Web Spike does actually 2 things:
* It uses the [plain protocol](#plain-protocol) to publish data over websockets.
* It provides a website with a generic UI to interact with the data.
Web Spikes are only available on ESP platforms with the [SDDS_ESP_EXTENSION](https://github.com/mLamneck/SDDS_ESP_Extension).

#### UDP Spike

At the moment not implemented for ESP. We use it for debugging when running SDDS on Windows with MinGW.

# How to Contribute

Our vision is to form a community with people of the same mindset. If you find this library helpful and you want to contribute, you can do so in many ways:

## Write Documentation

Writing documentation requires a lot of time and effort. This is a good place to start contributing. Even beginners are welcome to help us out. It's always good to have parts of the documentation done by users who are not involved in the development. They can write the documentation from their perspective and probably help others better understand the concepts.

## Help to organization of the Repository

This is our first Open Source project. We have no experience with stuff like:
- Automated tests 
- Workflows
- Actions

So, we are glad if you want to help us out to streamline our development process.

## Code new Spikes

Coding a [Spike](#spikes) is usually straightforward, especially if you can use an existing protocol like the [plain protocol](#plain-protocol). There's a lot that can be done:

- `mqttSpike`
- `tcpSpike`
- `displaySpikes`

just to name a few.

## Develop Components for Our Public Component Library

We are planning to have a separate repository for libraries with components based on and to be used with SDDS. A few examples of what a component could be:
- PID Controller
- Filter Components
- Device drivers
    - Temperature Sensors
    - Motor Controllers
    - ...

Developing components is usually straightforward, so even beginners can contribute with their work. For example, if you have coded a SDDS component to publish the data of a temperature sensor like the **DS18B20**, you could publish that for others.

## Test and give feedback

The more people testing stuff, the better. Good feedback like "If I hit the upper arrow key at a 30-degree angle with the spoon, the program crashes" is always welcome to help us eliminate bugs. It also helps to have some feedback on how the library performs on different platforms.

