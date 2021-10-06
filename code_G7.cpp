#include "mbed.h"
#include "DS1820.h"
#include "DHT.h"
#include "DavisAnemometer.h"
#include "HX711.h"
#include "fft.h"

DS1820 ds1820(D2); // OK
Serial pc(USBTX, USBRX); //  OK
Serial sigfox(D1,D0); // pas OK
static DavisAnemometer anemometer(A1 /* wind direction */, D6 /* wind speed */); // OK
DHT dht22(D3, DHT22); // pas OK
HX711 Balance(D5,D6);       // Déclaration de l'objet HX711 

int main(){

    pc.printf("\r\n--Commencer--\r\n");
    
    //sonde
    float temp = 0; //temperature de la sonde
    int resultat = 0;
    int erreur=0;
    
    //DHT22
    float humidite = 0, tempDHT22 = 0;
    anemometer.enable();
    
    //sigfox.printf("ESSAI");

    //poids
    long valeurHx;
    long valeurHxTare ;
    double poids;
    valeurHxTare = Balance.getValue(); 
    
    //fft
    AnalogIn sound_sensor(A0);
    Serial pc(USBTX,USBRX);
    Ticker ticker;
    std::complex<float> samples_array[samples_nb];
    int sample_count = 0;
    int compute_fft = 0;
    void readSample() {
      if(!compute_fft) {
        samples_array[sample_count] = std::complex<float>(sound_sensor.read());
        sample_count += 1;

        if(sample_count >= samples_nb) {
            sample_count = 0;
            compute_fft = 1;
        }
      }
    }
    void fft(){
        if(compute_fft) {
            ticker.detach();    // disable interupt
            
            // compute FFT
            FFT(samples_array);

            // find the first harmonic
            int H1_index = 0;

            for(int i = 1; i < samples_nb/2; ++i) {
                samples_array[i] = std::norm(samples_array[i]);
                if(samples_array[i].real() > samples_array[H1_index].real()) {
                    H1_index = i;
                }
            }
            
            // calculate frequency and amplitude
            float H1_freq = (float)H1_index * (float)sampling_freq / (float)samples_nb;
            float H1_ampl = std::abs(20*std::log10(samples_array[H1_index].real()) - 50);
            
            // print the results
            printf("Harmonic frequency : %f ; harmonic amplitude %f\n\r", H1_freq, H1_ampl);
     
            compute_fft = 0;
            
            // re enable interrupt
            ticker.attach(&readSample, 1.0/sampling_freq);
        }
    }
    

    while(1){
        //ThisThread::sleep_for(2000);
        erreur = dht22.readData(); 
            
        if(!(ds1820.begin() && erreur == 0)){
            printf("ERREUR DS1820 ou DHT22\r\n");
        }
        ds1820.startConversion();
        //wait(10);
        resultat = ds1820.read(temp);
        tempDHT22 = dht22.ReadTemperature(CELCIUS); // lire temperature par DHT22
        humidite= dht22.ReadHumidity(); // lire humidite par DHT22
        switch (resultat) {
                  case 0:                 // pas d'erreur -> 'temp' comporte valeur de temperature
                      pc.printf("tempSonde = %3.1f%cC ,tempDHT22: %f%cC , humidite: %f \r\n", temp, 176,tempDHT22,176,humidite);
                     // sigfox.printf("AT$SF=%02x%02x%02x\r\n",tempDHT22,humidite,(int)temp); //envoyer le message a backend sigfox

                    break;
  
                  case 1:                 // pas de capteur present -> 'temp' n'est pas mis a jour
                      pc.printf("pas de capteur present\n\r");
                      break;
  
                  case 2:                 // erreur de CRC  -> 'temp' n'est pas mis a jour
                      pc.printf("erreur de CRC\r\n");
        }
        
        
        
        
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

        valeurHx= Balance.getValue();   
        poids = ((double)valeurHx-(double)valeurHxTare)/11500;  // Conversion de la valeur de l'ADC en grammes
        poids = (-1.0)*(poids/1.88); // ajuster en kg   

        fft();


        sigfox.printf("AT$SF=%02x%02x%02x%02x\r\n",tempDHT22,humidite,(int)temp,anemometer.readWindSpeed());
        //humidite= dht22.ReadHumidity();
        //tempDHT22 = dht22.ReadTemperature(CELCIUS); 
        // pc.printf("tempDHT22=%3.1f /humidite= %3.1f \r\n",tempDHT22,humidite); 
        pc.printf("weight is %4.2f \r\n",poids); 
        pc.printf("\r");              
        
        wait(2);
        
    }
    
    
    
    
    
    
}