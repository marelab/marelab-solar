#include <Event.h>
#include <Timer.h>

//This code has intentionally has been written to be overly lengthy and includes unnecessary steps.
//Many parts of this code can be truncated. This code was written to be easy to understand.
//Code efficiency was not considered. Modify this code as you see fit.
//This code will output data to the Arduino serial monitor. Type commands into the Arduino serial monitor to control the pH circuit.
//set the var Arduino_only to equal 1 to watch the Arduino take over control of the pH circuit.
#include <math.h>
#include <SoftwareSerial.h>    //we have to include the SoftwareSerial library, or else we can't use it.
#include <Wire.h>              //For the I2C slave functionality


#define rrx 8                  //define what pin rx is going to be.
#define ttx 9                  //define what pin Tx is going to be.


#define I2CADRPIN0 4
#define I2CADRPIN1 5
#define I2CADRPIN2 6
#define BAUDRATE_PH 38400
#define BAUDRATE_COM 9600

#define I2CADRESSOFFEST 40			// The I2C Adress of this slave Device
int		I2CADRESS = 0;				// I2C Adress variable part 0-7
byte	I2C_OUT = 0;				// Control Flag that copys Serial output to I2C as answer
byte    SELECT_I2C_REGISTER = 0;	// The register that is readed by a Master read call

union phI2C { float phwert; byte b[sizeof(float)]; }; // I2C Output Registers
phI2C PHI2C;

Timer timer;						// Reads the PH every n seconds;
int   timer_id;						// ID of the Timer;
byte  PH_AUTO_MODE = 0;             // sets PH mode AUTO = 1; 
byte  PH_READ_INTERVAL = 20;		// Intervall time of PH Reads;

SoftwareSerial myserial(rrx, ttx);	//define how the soft serial port is going to work.

char ph_data[20];					//we make a 20 byte character array to hold incoming data from the pH.
char computerdata[20];				//we make a 20 byte character array to hold incoming data from a pc/mac/other.
byte received_from_computer=0;		//we need to know how many characters have been received.
byte received_from_sensor=0;		//we need to know how many characters have been received.
byte arduino_only=0;				//if you would like to operate the pH Circuit with the Arduino only and not use the serial monitor to send it commands set this to 1. The data will still come out on the serial monitor, so you can see it working.
byte startup=0;						//used to make sure the Arduino takes over control of the pH Circuit properly.
float ph=0;							//used to hold a floating point number that is the pH.
byte bph_int;
byte bph_frac;
byte string_received=0;            //used to identify when we have received a string from the pH circuit.

char * val1, *val2;                   // PH strings of tokenizer
byte phVal1,phVal2;

///////////////////////////////////////////////////////////////////////////////////////
//here are some functions you might find useful
//these functions are not enabled

void s7_cal(){					//calibrate to a pH of 7
myserial.print("s\r");}			//send the "s" command to calibrate to a pH of 7.00


void f4_cal(){					//calibrate to a pH of 4
myserial.print("f\r");}			//send the "f" command to calibrate to a pH of 4.00


void t10_cal(){					//calibrate to a pH of 10.00
myserial.print("t\r");}			//send the "t" command to calibrate to a pH of 10.00


void phFactoryDefault(){		//factory defaults the pH circuit
myserial.print("X\r");}			//send the "X" command to factory reset the device


void read_info(){				//get device info
myserial.print("I\r");}			//send the "I" command to query the information


void phSetLEDs(byte enabled)	//turn the LEDs on or off
{
	if(enabled)					//if enabled is > 0
	myserial.print("L1\r");		//the LED's will turn ON
	else						//if enabled is 0
	myserial.print("L0\r");		//the LED's will turn OFF
}


void ReadPh()
{
	myserial.print("r\r");     //read ph once.
}

// Gets the I2C Adress for the device
// the Adress is I2CADRESSOFFEST + JUMPERS (I2CADRPIN0,I2CADRPIN1,I2CADRPIN2)
void Get_I2C_Adress()
{
	pinMode(I2CADRPIN0, INPUT);      // Adr Bit 0
	pinMode(I2CADRPIN1, INPUT);      // Adr Bit 1
	pinMode(I2CADRPIN2, INPUT);      // Adr Bit 2
	if (digitalRead(I2CADRPIN0) == 1)
	I2CADRESS = 1;
	if (digitalRead(I2CADRPIN1) == 1)
	I2CADRESS = I2CADRESS + 2;
	if (digitalRead(I2CADRPIN1) == 1)
	I2CADRESS = I2CADRESS + 4;
	I2CADRESS = I2CADRESSOFFEST + I2CADRESS;
}

//////////////////////////////////////////////////////////////////////////////////
void setup(){
	int val=0;
	
	// Setting pins for I2C Adress to read that is configured over these Jumpers
	Get_I2C_Adress();					// Get the I2C Adress for the device
	Wire.begin(I2CADRESS);				// join I2C bus with address #4
	Wire.onReceive(receiveEvent);		// register event for Recv from master
	Wire.onRequest(sendEvent);          // register event for sending to master

	//////////////////////////////////////////////////////////////////
	
	Serial.begin(BAUDRATE_COM);         //enable the hardware serial port
	myserial.begin(BAUDRATE_PH);		//enable the software serial port

	// Init the PH device
	myserial.print("e\r");				//take the pH Circuit out of continues mode.
	delay(50);							//on start up sometimes the first command is missed.
	myserial.print("e\r");				//so, let’s send it twice.
	delay(50);							//a short delay after the pH Circuit was taken out of continues mode is used to make sure we don’t over load it with commands.
	startup=1;							//startup is completed, let's not do this again during normal operation.
	if (PH_AUTO_MODE)
		timer_id = timer.every((PH_READ_INTERVAL*1000), ReadPh); // Timer for PH reading
}


// Commands the I2C client understands
// recv 1 - Get PH once       sets device to PH once and sends PH
// recv 3 - CAL 7             sends back one Byte a 0 for OK and 10 for false
// recv 4 - CAL 4             sends back one Byte a 0 for OK and 10 for false
// recv 5 - CAL 10            sends back one Byte a 0 for OK and 10 for false
// recv 6 - Factory Default   sends back one Byte a 0 for OK and 10 for false
// recv 7 - GetInfo           sends back one Byte a 0 for OK and 10 for false
// recv 9 - AutoPH            switchs to AUTO mode that reads PH every 

// I2C event receiver
void receiveEvent(int howMany)
{
	char c;
	byte start = 0;
	int x;
	while(1 <= Wire.available()) // loop through all but the last
	{
		if (start==0){ // first BYTE is COMMAND 
			x = Wire.read();
			start ++;
		}
		else
		{
			c = Wire.read(); // receive byte as a character
			//Serial.print(c); // print the character
		}
		
	}

	
	if (x == 1)
	{
		Serial.println("I2C get PH");
		myserial.print("r\r");     //read ph once.
		
	}
	if (x == 2)
	{
		Serial.println("I2C continuous PH");
		myserial.print("c\r");     //continuous read ph .
		
	}
	else if (x == 3)
	{
		Serial.println("I2C PH cal7");
		s7_cal();
	}
	else if (x == 4)
	{
		Serial.println("I2C PH cal4");
		f4_cal();
	}
	else if (x == 5)
	{
		Serial.println("I2C PH cal10");
		t10_cal();
		
	}
	else if (x == 6)
	{
		Serial.println("I2C FactoryDefault");
		phFactoryDefault();
		
	}
	else if (x == 7)
	{
		Serial.println("I2C ph version");
		read_info();
	}
	else if (x == 8)
	{
		Serial.println("I2C end continuous mode go quite");
		myserial.print("e\r");			//send the "I" command to query the information
	}
	else if (x == 9)		// sets AUTO PH ON/OFF only relevant for RS232
	{
		SetAutoPh();
	}
	else if (x == 20)		// sets Time Value for AUTO PH in sec. 1-255
	{
		PH_READ_INTERVAL = (byte) c;
		if (PH_AUTO_MODE)
		{
			timer.stop(timer_id);
			timer_id = timer.every((PH_READ_INTERVAL*1000), ReadPh); // Timer for PH reading
			Serial.print("PH AUTO ON INTERVAL SEC.: ");
			Serial.println((PH_READ_INTERVAL));
		}else
		{
			Serial.print("PH INTERVAL SEC.: ");
			Serial.println((PH_READ_INTERVAL));
		}
	}
	else if (x == 21)		// sets Time Value for AUTO PH in sec. 1-255
	{
		SELECT_I2C_REGISTER = 5;
	}
	// Register for PH Values
	else if (x == 10){ SELECT_I2C_REGISTER = 0; }
	else if (x == 11){ SELECT_I2C_REGISTER = 1; }
	else if (x == 12){ SELECT_I2C_REGISTER = 2; }
	else if (x == 13){ SELECT_I2C_REGISTER = 3; }
	
}

// callback for sending I2C data back to Master
void sendEvent(){
	if ((SELECT_I2C_REGISTER >= 0) && (SELECT_I2C_REGISTER <= 3))  // PH Value Return 4 Bytes
	{
		Wire.write(PHI2C.b[SELECT_I2C_REGISTER]);
		Serial.println("PH send");
	}
	if ( SELECT_I2C_REGISTER == 5)								  // PH_TIMER RETURN
	{
		Wire.write(PH_READ_INTERVAL);
		Serial.println("PH TIMER send");
	}
}


void PrintSerialMenu(){
	Serial.println("1 - Get PH once");
	Serial.println("2 - Get PH continues");
	Serial.println("3 - CAL 7");
	Serial.println("4 - CAL 4");
	Serial.println("5 - CAL 10");
	Serial.println("6 - Factory Default");
	Serial.println("7 - GetInfo");
	Serial.println("8 - Disp. Menu");
	Serial.println("9 - Auto PH");
	Serial.println("Input :");
}

// Sets the PH Reading AUTO ON/OFF
void SetAutoPh(){
	if (PH_AUTO_MODE)
	{
		Serial.println("PH AUTO OFF");
		PH_AUTO_MODE = 0;
		timer.stop(timer_id);
	}
	else
	{
		Serial.print("PH AUTO ON INTERVAL: ");
		Serial.println((PH_READ_INTERVAL*1000));
		PH_AUTO_MODE = 1;
		timer_id = timer.every((PH_READ_INTERVAL*1000), ReadPh); // Timer for PH reading
	}
}


void loop(){
	
	if(myserial.available() > 0){										//if we see that the pH Circuit has sent a character.
		received_from_sensor=myserial.readBytesUntil(13,ph_data,20);	//we read the data sent from pH Circuit until we see a <CR>. We also count how many character have been received.
		ph_data[received_from_sensor]=0;								// we add a 0 to the spot in the array just after the last character we received. This will stop us from transmitting incorrect data that may have been left in the buffer.
		string_received=1;												// a flag used when the Arduino is controlling the pH Circuit to let us know that a complete string has been received.
		ph=atof(ph_data);
		PHI2C.phwert = ph;												// Convert PH float to I2C PH Register
		Serial.print("S: ");
		Serial.println(ph_data);										// lets transmit that data received from the pH Circuit to the serial monitor.
		Serial.print("F: ");
		Serial.println(ph);												// lets transmit that data received from the pH Circuit to the serial monitor.
	}
	
	
	if (Serial.available()){
		received_from_computer=Serial.readBytesUntil(13,computerdata,20); //we read the data sent from the serial monitor(pc/mac/other) until we see a <CR>. We also count how many characters have been received.
		computerdata[received_from_computer]=0; //we add a 0 to the spot in the array just after the last character we received.. This will stop us from transmitting incorrect data that may have been left in the buffer.
		//Serial.println(computerdata[received_from_computer-1]);
		
		// Checking what to do
		if (computerdata[received_from_computer-1] == '1')
		{
			//Serial.println("1 - Get PH once");
			myserial.print("e\r");     //take the pH Circuit out of continues mode.
			delay (50);
			myserial.print("r\r");     //read ph once.
			//PrintSerialMenu();
			
		}
		else if (computerdata[received_from_computer-1] == '2')
		{
			//Serial.println("2 - Get PH continues");
			myserial.print("c\r");     //read ph once.
		}
		else if (computerdata[received_from_computer-1] == '3')
		{
			Serial.println("3 - CAL 7");
			s7_cal();
			//PrintSerialMenu();
		}
		else if (computerdata[received_from_computer-1] == '4')
		{
			Serial.println("4 - CAL 4");
			f4_cal();
			//PrintSerialMenu();
		}
		else if (computerdata[received_from_computer-1] == '5')
		{
			Serial.println("5 - CAL 10");
			t10_cal();
			//PrintSerialMenu();
		}
		if (computerdata[received_from_computer-1] == '6')
		{
			//Serial.println("6 - Factory Default");
			phFactoryDefault();
			//PrintSerialMenu();
		}
		else if (computerdata[received_from_computer-1] == '7')
		{
			//Serial.println("7 - GetInfo");
			read_info();
			//PrintSerialMenu();
		}
		else if (computerdata[received_from_computer-1] == '8')
		{
			//Serial.println("7 - GetInfo");
			PrintSerialMenu();
		}
		else if (computerdata[received_from_computer-1] == '9') // Set AutoPH Toggle
		{
			SetAutoPh();
		}
	}
	
	if (PH_AUTO_MODE)
		timer.update();
}







 


 


