#include "mbed.h"
#include "DavisAnemometer.h"

DigitalOut  led(LED2);
static DavisAnemometer anemometer(A0 /* wind direction */, D4 /* wind speed */);
int main()
{
    anemometer.enable();
    while (1) {
        if(anemometer.readWindDirection() <=309 && anemometer.readWindDirection() >=306){
                printf("La direction est Nord\r\n [speed] %.2f km/h\r\n",anemometer.readWindSpeed());          
            }
        else if(anemometer.readWindDirection() <=328 && anemometer.readWindDirection() >=326){
                printf("La direction est Sud\r\n [speed] %.2f km/h\r\n",anemometer.readWindSpeed()); 
            }
        else if(anemometer.readWindDirection() <=295 && anemometer.readWindDirection() >=291){
            
                printf("La direction est Ouest\r\n [speed] %.2f km/h\r\n",anemometer.readWindSpeed()); 
            }
            
         else if(anemometer.readWindDirection() <=324 && anemometer.readWindDirection() >=321){
            
                printf("La direction est Est\r\n [speed] %.2f km/h\r\n",anemometer.readWindSpeed()); 
            }
         else if(anemometer.readWindDirection() <=262 && anemometer.readWindDirection() >=259){
            
                printf("La direction est Nord-Ouest\r\n [speed] %.2f km/h\r\n",anemometer.readWindSpeed()); 
            }
         else if(anemometer.readWindDirection() <=277 && anemometer.readWindDirection() >=274){
        
                printf("La direction est Sud-Ouest\r\n [speed] %.2f km/h\r\n",anemometer.readWindSpeed()); 
            }
            
         else if(anemometer.readWindDirection() <=302 && anemometer.readWindDirection() >=298){
            
                printf("La direction est Sud-Est\r\n [speed] %.2f km/h\r\n",anemometer.readWindSpeed()); 
            }
         else if(anemometer.readWindDirection() <=285 && anemometer.readWindDirection() >=283){
            
                printf("La direction est Nord-Est\r\n [speed] %.2f km/h\r\n",anemometer.readWindSpeed()); 
          
            }
        
        led = !led;
        wait(0.5);
    }
}