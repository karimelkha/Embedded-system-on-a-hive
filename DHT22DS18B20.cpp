#include "mbed.h"
#include "DS1820.h"
#include "DHT.h"

  //serial du module wisol sigfox et PC
  Serial pc(USBTX, USBRX);
  Serial sigfox(D1,D0);

  //Capteur de temperature/humidite et sonde
  DHT dht22(D3, DHT22);
  DS1820      ds1820(D4); 

  //verification par LED
  DigitalOut  led(LED1);
 

  
  int main()
  {
    int erreur = 0;
    int humidite = 0, tempDHT22 = 0; //temperature et humidite de DHT22
    float       temp = 0; //temperature du sonde
    int         resultat = 0; //verifier le statut de ds18b20
      
      pc.printf("\r\n--Commencer--\r\n");
      erreur = dht22.readData(); 
    
      if (ds1820.begin() && erreur == 0) {
          while (1) {
              ds1820.startConversion();     // commencer a converser
              //ThisThread::sleep_for(1000); // laisser DS1820 complete la conversion
              wait(10);
              resultat = ds1820.read(temp); // lire temperature par ds1820 et lancer cyclic redundancy check (CRC)
              tempDHT22 = dht22.ReadTemperature(CELCIUS); // lire temperature par DHT22
              humidite= dht22.ReadHumidity(); // lire humidite par DHT22
              switch (resultat) {
                  case 0:                 // pas d'erreur -> 'temp' comporte valeur de temperature
                      //pc.printf("temp = %3.1f%cC\r\n", temp, 176);
                      sigfox.printf("AT$SF=%02x%02x%02x\r\n",c,h,(int)temp); //envoyer le message a backend sigfox

                    break;
  
                  case 1:                 // pas de capteur present -> 'temp' n'est pas mis a jour
                      pc.printf("pas de capteur present\n\r");
                      break;
  
                  case 2:                 // erreur de CRC  -> 'temp' n'est pas mis a jour
                      pc.printf("erreur de CRC\r\n");
              }
  
              led = !led; // verification par LED
          }
      }
      else
         pc.printf("erreur de connection ds18b20 ou DHT22!\r\n");
        
  }
