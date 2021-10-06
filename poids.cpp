#include "mbed.h"
#include "HX711.h"
 
Serial pc(USBTX,USBRX);         // Déclaration du port série USB
 
HX711 Balance(D5,D6);       // Déclaration de l'objet HX711
 
int main() 
{
    pc.printf("\nDebut Enregistrement\n");
    
    long valeurHx;
    long valeurHxTare ;
    double poids;
    valeurHxTare = Balance.getValue(); 
   
    while(1){
         wait_ms(200);                                                      
        valeurHx= Balance.getValue();   
        poids = ((double)valeurHx-(double)valeurHxTare)/11500;  // Conversion de la valeur de l'ADC en grammes
        poids = (-0.50)*(poids/0.875); // ajuster en kg
       
        pc.printf("weight is %4.2f \r\n",poids);
        
        }
    
}
 