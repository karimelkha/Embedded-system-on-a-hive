#include "mbed.h"
#include <math.h> 
#include <cstdio>
#include <cmath>
#include <complex>
#include <iostream>
#include <valarray>
#define Pai  3.1415

typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;
    

void fftSpectre(int nbEch,Complex test[]);

AnalogIn micro(A0);
Serial pc(USBTX, USBRX);


int i = 0;
char*str;
char ch[36];

float Log2( float n )  
{   
    return log( n ) / log( 2.0 );  
} 

void fft(CArray &x)
{
    // Transformée de Fourier discrète
    unsigned int N = x.size(), k = N, n;
    float thetaT = Pai / N;
    Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
    while (k > 1)
    {
        n = k;
        k >>= 1;
        phiT = phiT * phiT;
        T = 1.0L;
        for (unsigned int l = 0; l < k; l++)
        {
            for (unsigned int a = l; a < N; a += n)
            {
                unsigned int b = a + k;
                Complex t = x[a] - x[b];
                x[a] += x[b];
                x[b] = t * T;
            }
            T *= phiT;
        }
    }
    // Decimate
    unsigned int m = (unsigned int) Log2(N);
    for (unsigned int a = 0; a < N; a++)
    {
        unsigned int b = a;
        // Reverse bits
        b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
        b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
        b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
        b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if (b > a)
        {
            Complex t = x[a];
            x[a] = x[b];
            x[b] = t;
        }
    }
}

const int nbEch = 256; //nombre d'échantillons
const int frqEch = 40000;
Complex test[nbEch];
int cpt = 0;

void lecture(){
    
    double a = micro.read();
    //pc.printf("\r\n %f ", a);
    test[cpt] = a;
    cpt++;
    if (cpt == nbEch) {
        cpt = 0;
        fftSpectre(nbEch,test);
    }
    
}

void fftSpectre(int nbEch,Complex test[]) {
    CArray data(test, nbEch);
    float VectFreq[nbEch/2];
 
    // forward fft
    fft(data);
    int indice = 0;
    for (int nb = 1; nb <nbEch/2 ; nb++){
        data[nb] = (norm(data[nb]));
        VectFreq[nb] =((float)nb)*(float)frqEch/((float)(nbEch)) ;
        //pc.printf("\r\n%.f Hz --> %f",VectFreq[nb],data[nb].real());
        if (data[nb].real() > data[indice].real()){
            indice = nb;
        }
    }
        
    wait(0.1);           
    pc.printf("\rFrequence dominante = %d Hz avec A = %f  ", (int)VectFreq[indice], data[indice].real());
    int freqDom = VectFreq[indice]; // Fréquence dominante
    
    char buffer[15];

    
}

Ticker flipper;


int main() {    
    
    
    flipper.attach(&lecture, 1.0/frqEch);
    
  
    while(1){
        
    }
}