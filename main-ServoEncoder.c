/*
 * File:   main-ServoEncoder.c
 * Author: Moustafa Eid
 *
 * Created on June 8, 2024, 3:05 PM
 */


#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <pic16f877a.h>
#include "config-ServoEncoder.h"
#define _XTAL_FREQ 4000000

signed int counter = 0;
uint8_t OutputA_LState;

void CCP1_init(void);
void Encoder_init(void);

void main(void) {
    Encoder_init();
    CCP1_init();
    CCPR1 = counter + (0x41A) + (0xB1DF); //Based on min and max distances 2~40 cm
    while(1){
        RC2 = 1;

    }
    return;
}


void CCP1_init(void){
    //---{Setting RC2 Pin as Output "Direction"}---
    TRISC2 = 0;
    // Set The Timer1 Register Value. To start counting from 45535
    TMR1 = 45535;
    //---{Configure CCP1 module to operate in Compare mode}--- 
    CCP1M0 = 0;
    CCP1M1 = 1;
    CCP1M2 = 0;
    CCP1M3 = 1;
    //---{Configure Timer1 pre-scaler to 1:1}---
    T1CKPS0 = 0;
    T1CKPS1 = 0;
    //---{Select Local Clock as TMR1 clock source}---
    TMR1CS = 0;
    //---{Turn on Timer 1}---
    TMR1ON = 1;
    // Enable CCP1 Interrupt
    CCP1IE = 1;
    PEIE = 1;
    GIE = 1;
    // Enable TMR1 Interrupt
    TMR1IE = 1;
}

void Encoder_init(void){
    TRISD0 = 1; //Output A of Encoder
    TRISD1 = 1; //Output B of Encoder
    OutputA_LState = RD0;
}


void __interrupt() ISR (void){
        
    
    if(CCP1IF){
        RC2 = 0;
        while(!TMR1IF){
            if(RD0 != OutputA_LState){
                if(RD1 != RD0){
                    counter-=50;
                }
                else
                    counter+=50;
        }
            OutputA_LState = RD0;
        }
        if(counter > 1000)
            counter = 1000;
        if(counter < 0)
            counter = 0;
        TMR1 = 45535;
        CCPR1 = counter + (0x41A) + (0xB1DF); 
        CCP1IF = 0;
        TMR1IF = 0;
        
    }

}