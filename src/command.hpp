
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MAX_STR 255
#define MCAChannels 1024
#define InBufferSize    (MCAChannels*2)+1 //see device firmware
#define OutBufferSize   65                //The firmware buffer size + 1

class USBGammaSpec {
public:

  // ***********************************************
  // LOCAL ITEMS
  // ***********************************************
  unsigned char buf[65];
  wchar_t wstr[MAX_STR];

  char16_t DeviceCommand = 'S';
  unsigned short HighVoltage = 550;
  unsigned short Gain = 1000;
  unsigned short LLD = 40;
  char16_t Input = 1;
  char16_t PMTgain = 12;
  char16_t Output1 = 1;
  char16_t Output2 = 1;

  uint8_t OUTBuffer[OutBufferSize];
  uint8_t INBuffer[InBufferSize];
  
  //  uint8_t OUTBuffer[OutBufferSize];  //USB out buffer. Allocate a memory buffer equal to the OUT endpoint size + 1
  //  uint8_t INBuffer[InBufferSize];    //USB in buffer. Allocate a memory buffer equal to the IN endpoint size + 1

  hid_device *hid_handle;

  // ***********************************************
  // HID HANDLER
  // ***********************************************
  bool HIDOpen(std::string pathname="/dev/hidraw0"){

int res;
        unsigned char buf[256];
        //#define MAX_STR 255
        //wchar_t wstr[MAX_STR];
        hid_device *handle;
        int i;

        struct hid_device_info *devs, *cur_dev;
        
        if (hid_init())
                return -1;

        devs = hid_enumerate(0x0, 0x0);
        cur_dev = devs; 
        while (cur_dev) {
                printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
                printf("\n");
                printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
                printf("  Product:      %ls\n", cur_dev->product_string);
                printf("  Release:      %hx\n", cur_dev->release_number);
                printf("  Interface:    %d\n",  cur_dev->interface_number);
                printf("\n");
                cur_dev = cur_dev->next;
        }
        hid_free_enumeration(devs);


    // Initialize the hidapi library
//        handle = hid_open(0x4d8, 0x3f, NULL);
        handle = hid_open_path(pathname.c_str());
        if (!handle) {
                printf("unable to open device\n");
                return 1;
        }

        // Read the Manufacturer String
        wstr[0] = 0x0000;
        res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
        if (res < 0)
                printf("Unable to read manufacturer string\n");
        printf("Manufacturer String: %ls\n", wstr);

        this->hid_handle = handle;

std::cout << "OPENED" << std::endl;

 // Read the Manufacturer String
/*
        wstr[0] = 0x0000;
        res = hid_get_manufacturer_string(hid_handle, wstr, MAX_STR);
        if (res < 0)
                printf("Unable to read manufacturer string\n");
        printf("Manufacturer String: %ls\n", wstr);
*/

/*
        // Read the Product String
        wstr[0] = 0x0000;
        res = hid_get_product_string(hid_handle, wstr, MAX_STR);
        if (res < 0)
                printf("Unable to read product string\n");
        printf("Product String: %ls\n", wstr);

        // Read the Serial Number String
        wstr[0] = 0x0000;
        res = hid_get_serial_number_string(hid_handle, wstr, MAX_STR);
        if (res < 0)
                printf("Unable to read serial number string\n");
        printf("Serial Number String: (%d) %ls", wstr[0], wstr);
        printf("\n");

*/
/*
        std::cout << "GETTING STRING" << std::endl;
  	// Read the Manufacturer String
  	res = hid_get_manufacturer_string(hid_handle, wstr, MAX_STR);
  	wprintf(L"Manufacturer String: %s\n", wstr);

  	// Read the Product String
  	res = hid_get_product_string(hid_handle, wstr, MAX_STR);
  	wprintf(L"Product String: %s\n", wstr);

  	// Read the Serial Number String
  	res = hid_get_serial_number_string(hid_handle, wstr, MAX_STR);
  	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

  	// Read Indexed String 1
  	res = hid_get_indexed_string(hid_handle, 1, wstr, MAX_STR);
  	wprintf(L"Indexed String 1: %s\n", wstr);
*/


    

    return (res == 0);
  }

  bool HIDClose(){
    int res = hid_exit();
    return (res == 0);
  }

  void HIDResponse(const char val){
    for (int i = 0; i < OutBufferSize; i++){
//      Serial.print(OUTBuffer[i]);
      std::cout << int(OUTBuffer[i]);
    }
std::cout << std::endl;

    if (hid_write(this->hid_handle, OUTBuffer, OutBufferSize)){
      hid_read(this->hid_handle, INBuffer, InBufferSize);
    }
  }

  // ***********************************************
  // BUFFER IO
  // ***********************************************
  void SetupBufferCommand(const char val){
    //std::cout << "SETTING DEVICE COMMAND " << DeviceCommand << std::endl;
    DeviceCommand = (char16_t)val;
    OUTBuffer[0] = 0;
    OUTBuffer[1] = val;
    for(int j=2; j<OutBufferSize; j++) {
      //std::cout << "FILLING BUFFER " << j << std::endl;
      OUTBuffer[j] = 0x00;
    }
    PrintOutBuffer();
  }

  // ***********************************************
  // GAMMA SPEC COMMANDS
  // ***********************************************
  void PrintOutBuffer(){
    return;
  for (int j=0; j < 20; j++){
    char data1 = OUTBuffer[j*2];
    char data2 = OUTBuffer[j*2+1];

    char16_t val = 0;
    val |= data2;
    val |= data1 << 8;

    std::cout << " BUFFER " << j << " : " << val << " : [" << data1 << "," << data2 << "]" << std::endl;
  }
}
  
  void SetHV(unsigned short value)
  {
    HighVoltage = value;

    union setdata
    {
        struct set
        {
  	uint8_t empty;
  	uint8_t DeviceCommand;
  	unsigned short HighVoltage;
        } data;
        uint8_t dataChar [4];
    } TxSettings;

    TxSettings.data.empty = 0;
    TxSettings.data.DeviceCommand = 'H';//DeviceCommand;
    TxSettings.data.HighVoltage   = HighVoltage;

    SetupBufferCommand('H');

    OUTBuffer[2] = HighVoltage >> 8;
    OUTBuffer[3] = HighVoltage >> 0;

    for(int j=0; j<4; j++) {
      OUTBuffer[j] = TxSettings.dataChar[j];
    }

    PrintOutBuffer();
    HIDResponse('H');
  }

  void StartRun(void)
  {
      union setdata
      {
          struct set
          {
              char16_t DeviceCommand;
              unsigned short HighVoltage;
              unsigned short Gain;
              unsigned short LLD;
              char16_t Input;
              char16_t PMTgain;
              char16_t Output1;
              char16_t Output2;
          } data;
          uint8_t dataChar [11];
      } TxSettings;

      TxSettings.data.DeviceCommand = 'R'; //DeviceCommand;
      TxSettings.data.HighVoltage = HighVoltage;
      TxSettings.data.Gain = Gain;
      TxSettings.data.LLD = LLD;
      TxSettings.data.Input = Input;
      TxSettings.data.PMTgain = PMTgain;
      TxSettings.data.Output1 = Output1;
      TxSettings.data.Output2 = Output2;

      SetupBufferCommand('R');
      for(int j=2; j<14; j++) OUTBuffer[j] = TxSettings.dataChar[j-2];

      PrintOutBuffer();
gettimeofday(&reading_timestamp, NULL);
//      clock_gettime(CLOCK_REALTIME, &gettime_now);
       std::cout << "startign run " << (reading_timestamp.tv_usec) << std::endl;
      HIDResponse('R');
  }
//unsigned long time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

  //struct timespec gettime_now;
  struct timeval reading_timestamp;
  struct timeval last_reading_timestamp;
  void GetReadings(void)
  {
    //std::cout << "-----------------------" << std::endl;
    //std::cout << "SETTING UP BUGGER " << this << std::endl;
     SetupBufferCommand('G');

      last_reading_timestamp = reading_timestamp;
//      clock_gettime(CLOCK_REALTIME, &gettime_now);
      gettimeofday(&reading_timestamp, NULL);
//      reading_timestamp = gettime_now.tv_sec*1E6 + gettime_now.tv_usec;
      std::cout << "getting readings " << (reading_timestamp.tv_usec) << " " << (reading_timestamp.tv_usec-last_reading_timestamp.tv_usec) <<  " " << (last_reading_timestamp).tv_usec << std::endl;
      //      std::cout << "RUNNING GET COMMAND " << this << std::endl;
   
      HIDResponse('G');
  }
  //long GetReadingTimestamp(void){
  //
  //}
  long GetReadingExposure(void){
//    std::cout<< "Exposure - " << reading_timestamp << " " << last_reading_timestamp << " " << reading_timestamp-last_reading_timestamp << std::endl;
    return reading_timestamp.tv_usec-last_reading_timestamp.tv_usec;
  }

  void StopRun(void)
  {
      union setdata
      {
          struct set
          {
              char16_t DeviceCommand;
              unsigned short HighVoltage;
              unsigned short Gain;
              unsigned short LLD;
              char16_t Input;
              char16_t PMTgain;
              char16_t Output1;
              char16_t Output2;
          } data;
          uint8_t dataChar [11];
      } TxSettings;

      TxSettings.data.DeviceCommand = 'S'; //DeviceCommand;
      TxSettings.data.HighVoltage = HighVoltage;
      TxSettings.data.Gain = Gain;
      TxSettings.data.LLD = LLD;
      TxSettings.data.Input = Input;
      TxSettings.data.PMTgain = PMTgain;
      TxSettings.data.Output1 = Output1;
      TxSettings.data.Output2 = Output2;

      SetupBufferCommand('S');
      for(int j=2; j<14; j++) OUTBuffer[j] = TxSettings.dataChar[j-2];
      HIDResponse('S');
  }

  void SetGain(unsigned short value)
  {
      Gain = value;

      union setdata
      {
          struct set
          {
              char16_t DeviceCommand;
              unsigned short Gain;
          } data;
          uint8_t dataChar [4];
      } TxSettings;

      TxSettings.data.DeviceCommand = 'A'; //DeviceCommand;
      TxSettings.data.Gain = Gain;

      SetupBufferCommand('A');

      OUTBuffer[2] = Gain >> 8;
      OUTBuffer[3] = Gain >> 0;


      for(int j=2; j<6; j++) OUTBuffer[j] = TxSettings.dataChar[j-2];
      PrintOutBuffer();
      HIDResponse('A');
  }//SetGain

  void SetLLD(unsigned short value)
  {
      LLD = value;

      union setdata
      {
          struct set
          {
              char16_t DeviceCommand;
              unsigned short LLD;
          } data;
          uint8_t dataChar [4];
      } TxSettings;

      TxSettings.data.DeviceCommand = 'L'; //DeviceCommand;
      TxSettings.data.LLD = LLD;

      SetupBufferCommand('L');
      OUTBuffer[2] = LLD >> 8;
      OUTBuffer[3] = LLD >> 0;

      for(int j=2; j<6; j++) OUTBuffer[j] = TxSettings.dataChar[j-2];
      HIDResponse('L');
  }//SetLLD

  void SetUserOuts(char16_t out1, char16_t out2)
  {
    Output1 = out1;
    Output2 = out2;

    union setdata
    {
        struct set
        {
            char16_t DeviceCommand;
            char16_t Output1;                 //User output 1
            char16_t Output2;                 //User output 2
        } data;
        uint8_t dataChar [4];
    } TxSettings;

    TxSettings.data.DeviceCommand = DeviceCommand;
    TxSettings.data.Output1 = Output1;
    TxSettings.data.Output2 = Output2;

    SetupBufferCommand('O');
    for(int j=2; j<6; j++) OUTBuffer[j] = TxSettings.dataChar[j-2];
    HIDResponse('O');
  }//SetUserOuts

  void SetInput(char16_t in)
  {
      Input = in;
      union setdata
      {
          struct set
          {
              char16_t DeviceCommand;
              char16_t Input;
          } data;
          uint8_t dataChar [2];
      } TxSettings;

      TxSettings.data.DeviceCommand = DeviceCommand;
      TxSettings.data.Input = Input;

      SetupBufferCommand('I');
      for(int j=2; j<4; j++) OUTBuffer[j] = TxSettings.dataChar[j-2];
      HIDResponse('I');

  }//SetInput


  // ***********************************************
  // EXTERNAL COMMANDS
  // ***********************************************
public:
  bool valid;
  USBGammaSpec( unsigned short deviceid ){


    unsigned short hv=550;
    unsigned short gain=1000;
    unsigned short lld=251;
    char16_t in = 1;
    char16_t pmtgain = 12;
    char16_t out1 = 1;
    char16_t out2 = 1;
    valid = false;

    std::string devicename = "/dev/hidraw0";
    if (deviceid == 0) devicename = "/dev/hidraw0";
    if (deviceid == 1) devicename = "/dev/hidraw1";
    if (deviceid == 2) devicename = "/dev/hidraw2";
    if (deviceid == 3) devicename = "/dev/hidraw3";
    valid = HIDOpen(devicename);

    SetConfig( hv, gain, lld, in, pmtgain, out1, out2 );
  }

  void SetConfig( unsigned short hv=550,
                  unsigned short gain=1000,
                  unsigned short lld=25,
                  char16_t in = 1,
                  char16_t pmtgain = 12,
                  char16_t out1 = 1,
                  char16_t out2 = 1 ){
    HighVoltage = hv;
    Gain = gain;
    LLD = lld;
    Input = in;
    Output1 = out1;
    Output2 = out2;

    std::cout << "Gamma Settings : "
              << "[HV " << hv << "] "
              << "[Gain " << gain << "] "
              << "[LLD " << lld << "] "
              << "[Input " << in << "] "
              << "[Output1 " << out1 << "] "
              << "[Output2 " << out2 << "]" << std::endl;

    std::cout << "ID " << this << std::endl;
      
    SetHV(HighVoltage);
    SetGain(Gain);
    SetLLD(LLD);
    SetInput(Input);
    SetUserOuts(Output1, Output2);
  };

  uint16_t GetCounts(int j){
    char data1 = INBuffer[j*2+3];
    char data2 = INBuffer[j*2+3+1];
    
    uint16_t value = 0;
    value = uint8_t(data2);
    value |= uint8_t(data1) << 8;

    return value;
  }
  
  ~USBGammaSpec(){
    SetHV(0);
    HIDClose();
  }

};
