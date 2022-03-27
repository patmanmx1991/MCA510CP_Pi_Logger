#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include<unistd.h>
#include <wiringSerial.h>
#include <string>

int OpenSerial(){
  int fd;
  if((fd=serialOpen("/dev/ttyS0",115200))<0){
    fprintf(stderr,"Unable to open serial device: %s\n",strerror(errno));
    return -1;
  }
  return fd;
}

std::string GetResponse(int fd){
  usleep(0.5E6);
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
  return GetResponse(fd);
}

int main ()
{
  int fd = OpenSerial();
  std::cout << SetGPS(fd) << std::endl;
  for (int i = 0; i < 30; i++){
    std::cout << GetGPS(fd) << std::endl;
  }


/*
  if((fd=serialOpen("/dev/ttyS0",115200))<0){
    fprintf(stderr,"Unable to open serial device: %s\n",strerror(errno));
    return 1;
  }

  serialPrintf(fd, "AT\r\n");
  usleep(1E6);
  std::cout << "Waiting" << std::endl;
  std::string res = "";

  while (serialDataAvail(fd)){
    
    char c = serialGetchar(fd);
    res += c;
//    std::cout << c;
  }
  std::cout << res << std::endl;

//  for (;;){
//    putchar(serialGetchar(fd));
//    fflush(stdout);
//  }
*/
}
