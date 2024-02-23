Installation:
	copy sdds in the Arduino library folder (for me it is C:\Users\mark.lamneck\Documents\Arduino\libraries)
	open arduino studio IDE and goto file->examples->sdds->serialComm

tests with vbus:
	open testSDDS from the vbus directory
	adjust the comport in the script to the one of your Arduino. Make sure it's not blocked by the Arduino IDE
	hit compile and have fun
	the script makes a copy of the datastructure defined in the arduino code and keeps it in sync
	this way you can use all the vbus components to visualize/set the variables from your code
	
	
Quick Explainations:

	The goal is to have a self-describing data structure (sdds) which provides standardized methods to read/write the data. 
	For example store all values with an option sdds::opt::saveval to eeprom (not implemented yet) or set a value over serial 
	console (userStruct.floatVal=11.4 would set the value to 11.4, userStruct.floatVal would read it). We wan't to get rid
	of all the code neccessary to read/write values from remote or to keep it in sync. On the other hand we want to use
	it like pure C++ if possible. We don't want to learn or worry much about the framework if possible. So we try
	to keep the interface as simple as possible:		

	I. In order to convert a C++ class to a sdds there a 3 steps to do:
		1. Derive your class from TmenuHandle
		2. wrap all variables that needs to be self-describing in a sdds_struct()
		3. declare variables with sdds_var(type,name[,opt,initialValue])	//parameter 3 and 4 are optional

		Example and comparison between plain C++ and sdds:
			Plain C++ code							with sdds
			
			class TuserStruct{						class TuserStruct : public TmenuHandle{
														sdds_struct(
				uint8_t Fuint8=10;							sdds_var(Tuint8,Fuint8,0,10);		//with initial value			
				double floatVal;							sdds_var(Tfloat32,floatVal);
														)
			} userStruct;							} userStruct;

			It's not much of a change right!? 
			
	
		To make it accessible over Serial the following code is required (Magic happens in the first 2 lines, the code in the loop is just to read the data from
		the serial, I will do this somewhen in TplainCommHandler):
				
			TserialStream sStream(115200);
			TplainCommHandler plainComm(userStruct, sStream);


			String buffer = "";
			void loop() {
			  while (Serial.available()) {
				char inChar = (char)Serial.read();
				if (inChar == '\n') {
				  plainComm.handleMessage(buffer.c_str());
				  buffer="";
				}
				else{
				  buffer += inChar;
				}
			  }
			  
			  //THIS LINE IS VERY IMPORTANT, IT HAS TO BE EXECUTED FREQUENTLY
			  TtaskHandler::handleEvents();			
			}
			
		Note that the same would work with UDP or any other communication channel. The only thing that must be changed it the sStream object (Providing 3 or 4 methods to send data). 
		And the code for receiving, which is typically a callback from some framework. In this callback you would simply call plainComm.handleMessage.

	II. But how do we know when a value is set over network? Let's say we have a heater and we change the setpoint. Of course we want to do something in that exact moment. There is an easy
		way to do that. Taking the example from above. We implement a constructor and in that constructor we place the code:
	
		class TuserStruct{
			sdds_struct(
				sdds_var(Tuint8,Fuint8,0,10);		//with initial value	
				sdds_var(Tfloat32,floatVal);
			)
			public:
				TuserStruct(){		//regular c++ constructor, nothing special
				
					//this block is the magic, it get's executed whenever floatVal is set
					//over network or just by assigning some value to it from the code (floatVal=4).
					on(floatVal){
						Fuint8 = Fuint8 + 1			//increment Fuint8 on every "change" of floatVal
					};
				
				}
		} userStruct;
	
	
	III. What if you want to do something every second or at some time in the future? This is where Ttimer comes into play:
		1. instantiate a Ttimer within you class but outside of sdds_struct
		2. start the timer
		3. use on statement to do something on elapse
			
			class TuserStruct{
				sdds_struct(
					sdds_var(Tuint8,Fuint8,0,10);		//with initial value	
					sdds_var(Tfloat32,floatVal);
				)
				Ttimer timer;
				public:
					TuserStruct(){		//regular c++ constructor, nothing special
						
						timer.start(1000);		//first start of the timer
						
						//code to be executed on timer
						on(timer){				
							timer.start(1000); //restart if neccessary
							//...
						};
					}
			} userStruct;
			
	
	
News (06.11.2023):
	I've implemented the datatype time which is not featurerich at the moment. But basically it can be used to enter some date/times
	and use it in the code. The underlying datatime is a timeval structure. So if you want to make use of it, you have to look for
	the C++ functions. Following is from the sdds example. Ttime::dyte is equal to timeval from the c lib. In the example I'm incrementing
	the time by 1 second in a time which runs every second. This will be much better supported in the future.
      
	  //increment time by one second
        Ttime::dtype t = time;
        t.tv_sec += 1;
        time = t;
