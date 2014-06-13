
/////////////////////////////////////////////////////////////////////////////
// marelab modbus dimmer modul                                             //                                            
// This modul is controlled over modbus via Bluetooth RS232 interface      //
// The modul is a client that can be also configured over Modbus           //
/////////////////////////////////////////////////////////////////////////////

#include <SimpleModbusSlave.h>
#include <EEPROMAnything.h>
#include <HC05.h>


/*
 * Change the BT device name
 *
 * This works with a Bluetooth terminal. The user is prompted for a new
 * name which is then written to HC-05.
 *
 */
#include <Arduino.h>
#include <EEPROM.h>


/*
* MARELAB BLUETOOTH COMMUINICATUION
* 
* To change the the module communication speed, PIN & Name the following 
* procedure is used. 
* 1.) Put a jumper on HC05_PROGRAMMING_JUMPER
* 2.) Power UP the MARELAB-BTDIM
* 3.) write via ModBus speed, PIN & Name to the defined ModBus Registers
* 
*/
#define HC05_RX_PIN 1                   // Connection Pin of the AVR used Software Serial
#define HC05_TX_PIN 2                   // Connection Pin of the AVR used Software Serial
#define HC05_PROGRAMMING_PIN 3          // AVR Output PIN for Programming with AT COMMANDS
#define HC05_PROGRAMMING_JUMPER 4       // AVR INPUT Pin that sets the Module into AT programming modus
#define HC05_MODUL_NAME "MARELAB-BTDIM" // Modul Name = HC05_MODUL_NAME + DeviceNameNum

//////////////// registers of your modbus slave ///////////////////
// These Registers defined by the enum are the modbus registers  //
// the server writes and reads out of them to control the        //
// board                                                         //
///////////////////////////////////////////////////////////////////
enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  DIM1,
  DIM2,
  DIM3,
  DIM4,
  DIM5,
  DIM6,
  DIM7,
  DIM8,
  DIM9,
  DIM10,
  DIM11,
  DIM12,
  DIM13,
  DIM14,
  DIM15,
  DIM16,
  DIM1_ADR, 
  DIM2_ADR,
  DIM3_ADR,
  DIM4_ADR,
  DIM5_ADR,
  DIM6_ADR,
  DIM7_ADR,
  DIM8_ADR,
  DIM9_ADR,
  DIM10_ADR,
  DIM11_ADR,
  DIM12_ADR,
  DIM13_ADR,
  DIM14_ADR,
  DIM15_ADR,
  DIM16_ADR,
  DIM_CHANNEL_COUNT,
  BLUETOOTH_STATE,
  PEERING_PIN,
  DEVICE_NAME_NR,
  DEVICE_BAUD,
  DEVICE_UFR,
  // leave this one
  TOTAL_REGS_SIZE 
  // total number of registers for function 3 and 16 share the same register array
};

unsigned int ModBudRegs[TOTAL_REGS_SIZE]; // function 3 and 16 register array



/*
 * Configure this sketch to work with either a software or a hardware
 * serial port, as configured in HC05.h
 */
#ifdef HC05_SOFTWARE_SERIAL
  #include <SoftwareSerial.h>
  HC05 myHC05 = HC05(3, 2, 4, 5);  // cmd, state, rx, tx
  #else
  HC05 myHC05 = HC05(3, 2);  // cmd, state
#endif

/*
 * See the ITeadStudio HC-05 datasheet for a full list of commands.
 */
String AT_NAME("AT+NAME=");
String AT_PIN("AT+PSWD=");


struct config_t
{
    int     PeeringPin;      //Peering pin for Bluetooth 
    uint8_t DeviceNameNum;   //Namber that gets added to the DeviceName
    uint8_t Baudrate;        //Baudrate of Bluetooth interface
    uint8_t State;           //State of Modul   0=FirstRun 1=Normal passthrou 2=NewParameterMode
} CONFIG;

//Sending Command in Command Mode to the Bluetooth Device
boolean SendAT(String command, String parameter)
{
    char buffer[32];
    String newName;
    
    newName = command + parameter;
    newName.toCharArray(buffer, 32);
    
    // make sure there is no pending output to interfere with commands
    myHC05.flush();

    // The name change command takes extra time.
    // 1000ms is large enough, but arbitrary.
    if (myHC05.cmd(buffer,1000))
    {
      myHC05.println("OK");
      return true;
    }
    else
    {
      myHC05.println("cmd failed...");
      return false;
    }
  return false;
}


// Soft Reset of the Sketch
void softReset(){
  asm volatile ("  jmp 0");
}

void setup()
{
  int errortrigger = 0;
  char buffer[32];
  char numstr[5];
  String newName;
  
  EEPROM_readAnything(0, CONFIG);
  myHC05.findBaud();
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // THIS IS THE PRE BOOT STATE IT CHECKS IF THE MODUL IS BOOTED NORMAL AS RS232 Bluetooth Brdige //
  // or the Bluetooth interface has to get reconfigured                                           //
  // The reconfiguration is a two state process first the new values are received the the         //
  // booting the modul using the new values                                                       //
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  // Check for ConfigState of Bluetooth Modul
  if (CONFIG.State == 0)        // First Run, the Device have to be initilized to default parameters
  {
    CONFIG.PeeringPin      = 1234;
    CONFIG.DeviceNameNum   = 0;
    CONFIG.Baudrate        = 0;
    CONFIG.State           = 1;
    
    //Setting the default DeviceName
    sprintf(numstr, "%d", CONFIG.DeviceNameNum);
    String number = numstr;
    String modulename = HC05_MODUL_NAME;
    newName = modulename + number;
    SendAT (AT_NAME,newName);
    
    //Setting the new DevicePin
    sprintf(numstr, "%d", CONFIG.PeeringPin);
    newName = numstr;
    SendAT (AT_PIN,newName);
     
    myHC05.cmd("AT+DISC", 1000);  // Disoneting Device 
    
    // Write Config Eeprom
    EEPROM_writeAnything(0,CONFIG);
  }
  else if (CONFIG.State == 1)  // Normal passthrou
  {
    
  }
  else if (CONFIG.State == 2)  // New Parameters to set
  {
  }
}

void setDimmer(int dimvalue, uint8_t channel)
{
  
}


void loop(){
  char buffer[32];
  size_t recvd = 0;
  bool waiting = true;
  String newName;

  myHC05.println("");
  myHC05.print("New name? ");

  // Use a timeout that will give reasonablly quick response to the user.
  myHC05.setTimeout(100);
  while (waiting)
  {
    if (myHC05.available())
    {
      recvd = myHC05.readBytes(buffer, 32);
      for (size_t i = 0; i < recvd; i++)
      {
        if (buffer[i] != '\n')
        {
          newName += buffer[i];
          myHC05.print(buffer[i]);
        }
        else
        {
          myHC05.println(" ");
          waiting = false;
          break;
        }
      }
    }
    delay(100);
  }

  newName.toCharArray(buffer, 32);
  //newName = NewNameCmd + newName;
  newName.toCharArray(buffer, 32);

  // make sure there is no pending output to interfere with commands
  myHC05.flush();

  // The name change command takes extra time.
  // 1000ms is large enough, but arbitrary.
  if (myHC05.cmd(buffer,1000))
  {
    myHC05.println("Name changed.");
    myHC05.println("Reconnect or rescan to see the result.");
    myHC05.println("Disconnecting...");
    myHC05.flush();
    myHC05.cmd("AT+DISC", 1000);
  }
  else
  {
    myHC05.println("Name NOT changed.");
  }
  
  // Send a count to the port just to indicate activity.
  // This will appear after the connection is re-established, or
  // immediately if the name change command fails.
  for (uint8_t i = 0; true; i++)
  {
    if (i == 0)
    {
      myHC05.println("");
    }
    myHC05.print(i);
    myHC05.print('\r');
    delay(1000);
  }
  
  // Main Loop For RS232 BRIDGE mode
  while (-1)
  {
    if (ModBudRegs[BLUETOOTH_STATE]== 2)  // New Parameters to config BlueTooth Bridge is set over ModBus
    {
      CONFIG.PeeringPin      = ModBudRegs[PEERING_PIN];
      CONFIG.DeviceNameNum   = ModBudRegs[DEVICE_NAME_NR];
      CONFIG.Baudrate        = ModBudRegs[DEVICE_BAUD];
      CONFIG.State           = 2;
      EEPROM_writeAnything(0, CONFIG);
      softReset();                                  // Doing a soft reset and restarting the sketch
    }
    else if  (ModBudRegs[BLUETOOTH_STATE]== 1)      // Normal operation lets get the dim values and set them to the I2C Bus
    {
      for (int ch = 0; ch < ModBudRegs[DIM_CHANNEL_COUNT]; ch++)
      {
        // ModBudRegs[ch] ;  // Dim Value
        // ModBudRegs[16+ch]; // Dim Channel
        setDimmer(ModBudRegs[ch] , (uint8_t) ModBudRegs[16+ch]);
      }
    }
    
    
  }
}

