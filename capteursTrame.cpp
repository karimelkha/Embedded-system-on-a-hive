#include "mbed.h"
#include "DS1820.h"
#include "DHT.h"
#include "DavisAnemometer.h"
#include "HX711.h"

DS1820 ds1820(D2); // OK
Serial pc(USBTX, USBRX); //  OK
Serial sigfox(D1,D0); // pas OK
static DavisAnemometer anemometer(A1 /* wind direction */,D4 /* wind speed */); // OK
HX711 Balance(D5,D6);       // Déclaration de l'objet HX711

DHT dht22(D3, DHT22); // pas OK

int main(){
    
    float temp = 0; //temperature de la sonde
    int temp_form;
    int temp_sonde_form;
    int humid_form;
    int vitesse_form;
    int direction_form;
    int poids_form;
    int resultat = 0;
    int erreur=0;
    int trame1;
    int trame2;
    int trame3;
    long valeurHx;
    long valeurTare;
    float poids;
    float vitesse;
	int flag = 0x0;
    
    
    float humidite = 0, tempDHT22 = 0;
    anemometer.enable();
    valeurTare = Balance.getValue();    
    pc.printf("\r\n--Commencer--\r\n");
    
    while(1){
        //ThisThread::sleep_for(2000);
        erreur = dht22.readData(); 
            
        if(!(ds1820.begin() && erreur == 0)){
            pc.printf("ERREUR DS1820 ou DHT22\r\n");
        }
        ds1820.startConversion();
        //wait(10);
        resultat = ds1820.read(temp);
        temp_sonde_form=(int)((temp*10)-300); // température de la sonde formalisée pour etre envoyée sur Sigfox
       
        
        tempDHT22 = dht22.ReadTemperature(CELCIUS); // lire temperature par DHT22
        temp_form=(int)((tempDHT22*10)-300);// température du DHT22 formalisée pour etre envoyée sur Sigfox
        humidite= dht22.ReadHumidity(); // lire humidite par DHT22
        humid_form=(int) humidite;
        
        switch (resultat) {
                  case 0:                 // pas d'erreur -> 'temp' comporte valeur de temperature
                     // pc.printf("tempSonde = %3.1f%cC ,tempDHT22: %f%cC , humidite: %f \r\n", temp, 176,tempDHT22,176,humidite);
                       pc.printf("tempSonde = %d%cC ,tempDHT22: %d%cC , humidite: %d \r\n", temp_sonde_form, 176,temp_form,176,humid_form);
                     // sigfox.printf("AT$SF=%02x%02x%02x\r\n",tempDHT22,humidite,(int)temp); //envoyer le message a backend sigfox

                    break;
  
                  case 1:                 // pas de capteur present -> 'temp' n'est pas mis a jour
                      pc.printf("pas de capteur present\n\r");
                      break;
  
                  case 2:                 // erreur de CRC  -> 'temp' n'est pas mis a jour
                      pc.printf("erreur de CRC\r\n");
        }
        
        
        
        vitesse=anemometer.readWindSpeed();
        if(anemometer.readWindDirection() <=309 && anemometer.readWindDirection() >=306){
                direction_form=0;   
                pc.printf("La direction est Nord\r\n numero envoye sur Sigfox: %d\r\n [speed] %.2f km/h\r\n",direction_form,vitesse); 
                

            }
        else if(anemometer.readWindDirection() <=328 && anemometer.readWindDirection() >=326){
                direction_form=1;
                pc.printf("La direction est Sud\r\n [speed] %.2f km/h\r\n",vitesse); 
               
            }
        else if(anemometer.readWindDirection() <=295 && anemometer.readWindDirection() >=291){
                direction_form=2;
                pc.printf("La direction est Ouest\r\n [speed] %.2f km/h\r\n",vitesse); 
                
            }
            
         else if(anemometer.readWindDirection() <=324 && anemometer.readWindDirection() >=321){
                direction_form=3;
                pc.printf("La direction est Est\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
         else if(anemometer.readWindDirection() <=262 && anemometer.readWindDirection() >=259){
                direction_form=4;
                pc.printf("La direction est Nord-Ouest\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
         else if(anemometer.readWindDirection() <=277 && anemometer.readWindDirection() >=274){
                direction_form=5;
                pc.printf("La direction est Sud-Ouest\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
            
         else if(anemometer.readWindDirection() <=302 && anemometer.readWindDirection() >=298){
                direction_form=6;
                pc.printf("La direction est Sud-Est\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
         else if(anemometer.readWindDirection() <=285 && anemometer.readWindDirection() >=283){
                direction_form=7;
                pc.printf("La direction est Nord-Est\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
        
        valeurHx= Balance.getValue();   
        poids = ((double)valeurHx-(double)valeurTare)/11500;  // Conversion de la valeur de l'ADC en grammes
        poids = (-0.50)*(poids/0.875)+0.02; // ajuster en kg
       poids_form= (int)((poids*10));
       
        pc.printf("weight is %4.2f \r\n",poids);
        
        pc.printf("----------------------------\r\n");
        
        trame1=(trame1 & 0x0)| // trame a 0
		(temp_form)| // 10bits de temperature DHT22
		(humid_form<<10)| // humidite sur 7 bits
		(temp_sonde_form<<17)| // temperature sonde sur 8 bits
        (direction_form<<25)| // direction sur 3 bits
		(flag<<28); // flag sur 3 bits
		
		trame2=(trame2 & 0x0)|
		(poids_form);
		
		
		
       //sigfox.printf("AT$SF=%02x%02x%02x%02x%02x\r\n",(int)tempDHT22,(int)humidite,(int)temp,(int)anemometer.readWindSpeed(),(int) poids);

        //humidite= dht22.ReadHumidity();
        //tempDHT22 = dht22.ReadTemperature(CELCIUS); 
       // pc.printf("tempDHT22=%3.1f /humidite= %3.1f \r\n",tempDHT22,humidite);                
        
        wait(1);
        
    }
}
