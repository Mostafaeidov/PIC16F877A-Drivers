/*
 * File:   main-ADC1.c
 * Author: Moustafa Eid
 *
 * Created on June 8, 2024, 3:05 PM
 */


#include <xc.h>
#include <stdint.h>
#include <pic16f877a.h>
#include "config-ServoMotor.h"
#define _XTAL_FREQ 4000000

void TMR0_init(void);
void CCP1_init(void);
void ADC_init(void);
uint16_t ADC_Read(uint8_t);

void main(void) {
    ADC_init();
    CCP1_init();
    CCPR1 = ADC_Read(0)+(0x3E8)+(0xB1DF);
    while(1){
        RC2 = 1;}
    return;
}


void TMR0_init(void){
    OPTION_REG = 0x04; //Set a prescaler of 32
    TMR0IE = 1;
    PEIE = 1;
    GIE = 1;
    TMR0IF = 0;
    TMR0 = 0;
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

void ADC_init(void){
    /*1. Configure the A/D module:
        Configure analog pins/voltage reference and digital I/O (ADCON1)
        Select A/D input channel (ADCON0)
        Select A/D conversion clock (ADCON0)
        Turn on A/D module (ADCON0)*/
    ADCON0 = 0x41;
    ADCON1 = 0xC0;
}

uint16_t ADC_Read(uint8_t ANC){
    if(ANC<0 || ANC>7)
    {return 0;}
    ADCON0 &= 0x11000101;
    ADCON0 |= (ANC<<3);
    __delay_us(30); //3. Wait the required acquisition time.
    GO_DONE = 1; //4. Start conversion: Set GO/DONE bit (ADCON0)
    while(ADCON0bits.GO_DONE); /*5. Wait for A/D conversion to complete by either:
                                                        Polling for the GO/DONE bit to be cleared (interrupts disabled); 
                                                        OR Waiting for the A/D interrupt.*/
    
    return ADRESL + (ADRESH<<8); //6. Read A/D Result register pair (ADRESH:ADRESL), clear bit ADIF if required.

     
/*2. Configure A/D interrupt (if desired):
       Clear ADIF bit
       Set ADIE bit
       Set PEIE bit
       Set GIE bit
7. For the next conversion, go to step 1 or step 2 as required. 
   The A/D conversion time per bit is defined as TAD.*/
    
}

void __interrupt() ISR (void){
    if(CCP1IF){
        RC2 = 0;
        while(!TMR1IF);
        TMR1 = 45535;
        CCPR1 = ADC_Read(0)+(0x3E8)+(0xB1DF);
        CCP1IF = 0;
        TMR1IF = 0;
    }
    

}