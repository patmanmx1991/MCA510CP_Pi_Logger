#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <wiringSerial.h>
#include <string>
#include <iostream>
#include <array>
#include <unistd.h>
#include "command.hpp"
#include <iostream>
#include <fstream>
#include <fstream>

#include <wiringSerial.h>
#include <string>
using namespace std;

int OpenSerial(){
  int fd;
  if((fd=serialOpen("/dev/ttyS0",115200))<0){
    fprintf(stderr,"Unable to open serial device: %s\n",strerror(errno));
    return -1;
  }
  return fd;
}

std::string GetResponse(int fd){
  usleep(0.25E6);
  std::string res = "";
  while (serialDataAvail(fd)){
    char c = serialGetchar(fd);
    res += c;
  }
  return res;
}

std::string SetGPS(int fd){
  
  serialFlush(fd);
  serialPrintf(fd,"AT+CGPS=1,1\r\n");
  return GetResponse(fd);
  
}

std::string GetGPS(int fd){
  serialFlush(fd);
  serialPrintf(fd,"AT+CGPSINFO\r\n");

  std::string res =  GetResponse(fd);
  if (res.length() == 0) return "ERROR";
  std::string token = res.substr( res.find("+CGPSINFO: ")+11, res.length()-1);
  
  return token.substr(0, token.find("OK"));
}

int execute(std::string cmd, std::string& output) {
    const int bufsize=128;
    std::array<char, bufsize> buffer;
    
    auto pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");

    size_t count;
    do {
        if ((count = fread(buffer.data(), 1, bufsize, pipe)) > 0) {
            output.insert(output.end(), std::begin(buffer), std::next(std::begin(buffer), count));
        }
    } while(count > 0);

    return pclose(pipe);
}    

std::string GetEnviro(){
  std::string data;
//  return "";
  execute("python3 /home/gammapi/getenviro.py", data);
  return data;
}

int lld;
int hv;
int gain;

void GetConfig(){
  std::ifstream file;
  file.open("/home/gammapi/lld");
  while (file >> lld) {};
  file.close();
  file.open("/home/gammapi/hv");
  while (file >> hv) {};
  file.close();
  file.open("/home/gammapi/gain");
  while (file >> gain) {};
  file.close();
}

uint16_t histogram_last[1024];
uint16_t histogram_current[1024];

int main(int argc, char argv[]){
  USBGammaSpec usb = USBGammaSpec(0);
  USBGammaSpec usb2 = USBGammaSpec(1);
  
  GetConfig();

  long runtag = time(NULL);
  std::string tag = std::to_string(runtag);

  usb.HIDOpen("/dev/hidraw0");
  usb2.HIDOpen("/dev/hidraw1");

  usb.SetConfig(hv,gain,lld);
  usb2.SetConfig(hv,gain,lld);

  usb.StopRun();
  usb2.StopRun();

  int run = 1;
  int epoch = 0;

  int gpsfd = OpenSerial();
  SetGPS(gpsfd);
  long times2 = 0;
  long times1 = 0;
  usb.StartRun();
  usb2.StartRun();

  while(true){
    epoch += 1;

    times1 = times2; //time(NULL);
    std::string gpsinfo1 = GetGPS(gpsfd);
    if (gpsinfo1.length() == 0) gpsinfo1 = GetGPS(gpsfd); 

    if (epoch % 300 == 0){
      run += 1;

      usb.StopRun();
      usb2.StopRun();

      usb.HIDClose();
      usb2.HIDClose();

      GetConfig();

      usb.HIDOpen();
      usb2.HIDOpen();

      usb.SetConfig(hv,gain,lld);
      usb2.SetConfig(hv,gain,lld);

      epoch = 1;
      usb.StartRun();
      usb2.StartRun();

      for (int bin = 0; bin < 1024; bin++){
        histogram_last[bin] = 0; 
        histogram_current[bin] = 0;
      }
      
      times1 = time(NULL);
    } else {
    
    }

    sleep(1);
    
    usb.GetReadings();
    usb2.GetReadings();

    long times = time(NULL);
    times2 = time(NULL);
    std::string gpsinfo2 = GetGPS(gpsfd);
    if (gpsinfo2.length() == 0) gpsinfo2 = GetGPS(gpsfd); 

    string enviro = GetEnviro();
    std::cout << enviro << std::endl;
    std::cout << gpsinfo1 << std::endl;

    std::ofstream myfile;
    std::string runss = std::to_string(run);
    std::string epochss = std::to_string(epoch);
    
    int n_zero = 8;
    int npadruns = n_zero;
    if (runss.length() > 0) npadruns = (n_zero - runss.length());
  
    int npadepochs = n_zero;
    if (epochss.length() > 0) npadepochs = (n_zero - epochss.length());

    std::string runs = std::string(npadruns, '0') + runss; //std::to_string(run);
    std::string epochs = std::string(npadepochs, '0') + epochss;// std::to_string(epoch);
    
    std::string filename = "./gammapi_st_" + tag + "_run_" + runs + "_epoch_" + epochs + ".dat";
    myfile.open( filename.c_str() );
    myfile << "run " << run << std::endl;
    myfile << "epoch " << epoch << std::endl;
    myfile << "enviro " << enviro << std::endl; 
    myfile << "unix " << times << std::endl;
    myfile << "ts1 " << times1 << std::endl;
    myfile << "ts2 " << times2 << std::endl;

    int potassium_counts = 0;
    int thorium_counts = 0;
    int uranium_counts = 0;
    int total_counts = 0;
    int cosmic_counts = 0;
    for(int j = 0; j < 1020; j++){
      int c = usb.GetCounts(j);
      int c2 = usb2.GetCounts(j);
      histogram_current[j] = c - histogram_last[j];
      histogram_last[j] = c;
      myfile << j << " " << c << " " << histogram_current[j] << " " << c2 << std::endl;

     if (j > 460 && j < 545) potassium_counts += histogram_current[j];
     if (j > 560 && j < 645) thorium_counts += histogram_current[j];
     if (j > 820 && j < 970) uranium_counts += histogram_current[j];
     if (j > 200 && j < 970) total_counts += histogram_current[j];
     if (j > 1020) cosmic_counts += histogram_current[j];
    }
    myfile << "potac " << potassium_counts << std::endl;
    myfile << "thorc " << thorium_counts << std::endl;
    myfile << "uranc " << uranium_counts << std::endl;
    myfile << "totac " << total_counts << std::endl;
    myfile << "cosmc " << cosmic_counts << std::endl;

    myfile << "gps1 " << gpsinfo1 << std::endl;
    myfile << "gps2 " << gpsinfo2 << std::endl;

    myfile.close();
  }
}
