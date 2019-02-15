#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/led_dd"

int toggle = 0;

int main(void){
  int dev;
  int retval;

  printf("[APP] led Device Driver open start\n");
  dev = open(DEVICE_NAME, O_RDWR);
  if(dev < 0){
    printf("[APP] led Device Driver open error\n");
    return -1;
  }
  printf("[APP] led Device Driver open success\n");

  while(1){
    if(toggle){
      ioctl(dev,0,1);
      printf("[APP] turn on led\n");
    }
    else{
      ioctl(dev,0,0);
      printf("[APP] turn off led\n");
    }
    toggle = !toggle;
    sleep(1);
  }

  printf("[APP] led Device Driver close start\n");
  close(dev);
  printf("[APP] led Device Driver close end\n");
  return 0;
}
