/*
 * File:   main-ServoUltra.c
 * Author: Moustafa Eid
 *
 * Created on June 8, 2024, 3:05 PM
 */


#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <pic16f877a.h>
#include "config-ServoUltra.h"
#define _XTAL_FREQ 4000000

uint16_t UltrasonicValue = 0;
uint8_t counter = 0;
uint8_t intcounter = 0;

void TMR2_init(void);
void CCP1_init(void);
void Ultrasonic_init(void);
void Ultrasonic_Read(void);

void main(void) {
    Ultrasonic_init();
    CCP1_init();
    TMR2_init();
    CCPR1 = (UltrasonicValue/2) + (0x3E8) + (0xB1DF); //Based on min and max distances 2~40 cm
    while(1){
        RC2 = 1;

    }
    return;
}


void TMR2_init(void){
    //---{Configure Timer2 pre-scaler to 1:1}---
    T2CKPS0 = 0;
    T2CKPS1 = 0;
    // Enable TMR2 Interrupt
    TMR2IE = 1;
    TMR2IF = 0;
    TMR2 = 0;
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

void Ultrasonic_init(void){
    TRISD0 = 0; //Trigger
    RD0 = 0;
    TRISD1 = 1; //Echo
}

void Ultrasonic_Read(void){
    
    RD0 = 1;
    __delay_us(10);
    RD0 = 0;
    while(!RD1); // Wait For The Echo Pulse From The Sensor
    TMR2ON = 1;
    while(RD1){
        if(TMR2IF){
            counter ++;
            TMR2IF = 0;
            TMR2 = 0;
        }
    } // Wait Until The Pulse Ends
    TMR2ON = 0;
    
}


void __interrupt() ISR (void){
        
    
    if(CCP1IF){
        RC2 = 0;
        intcounter ++;
        Ultrasonic_Read();
        UltrasonicValue = counter * 255;
        if(UltrasonicValue >= 2500)
            UltrasonicValue = 2500;
        while(!TMR1IF);
        TMR1 = 45535;
        if(intcounter > 5){
            CCPR1 = (UltrasonicValue/2) + (0x3E8) + (0xB1DF);
            intcounter = 0;}
        else{
            CCPR1 = CCPR1;
        }
        counter = 0;
        CCP1IF = 0;
        TMR1IF = 0;
        
    }

}