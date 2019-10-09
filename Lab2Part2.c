/*
 * File:   main.c
 * Author: cjd53
 *
 * Created on September 30, 2019, 1:17 PM
 */

#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <htc.h>
#include <pic.h>
#include <math.h>
#define _XTAL_FREQ 4000000

//tie RB0 to RC1 and RC2


long int time, t0, t1;
int display_active; 

void display(int x){
    PORTAbits.RA0 = 1;
    switch(x){
        case 1: //dot
            PORTCbits.RC0 = 0;
            PORTAbits.RA2 = 0; 
            PORTAbits.RA3 = 0;
            PORTCbits.RC3 = 0;
            PORTCbits.RC4 = 1;
            PORTCbits.RC5 = 0;
            PORTCbits.RC6 = 0;
            PORTCbits.RC7 = 0;
            break;
        case 2: //dash
            PORTCbits.RC0 = 0;
            PORTAbits.RA2 = 0; 
            PORTAbits.RA3 = 0;
            PORTCbits.RC3 = 0;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 0;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 0;
            break;
        case 3: //error
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1; 
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 0;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
            break;
    }
    __delay_ms(2000);  
    
    PORTCbits.RC0 = 0;
    PORTAbits.RA2 = 0; 
    PORTAbits.RA3 = 0;
    PORTCbits.RC3 = 0;
    PORTCbits.RC4 = 0;
    PORTCbits.RC5 = 0;
    PORTCbits.RC6 = 0;
    PORTCbits.RC7 = 0;
    PORTAbits.RA0 = 0; 
    TMR1IF = 0;
    CCP1IF = 0;
    CCP2IF = 0;
}

void main(void){
    ANSEL = 0x00; //PORTA = I/O
    ANSELH = 0x00; //PORTB = I/O
    TRISA = 0x00; //output
    TRISB = 0x01; //RB0 = input
    TRISC = 0x06; //RC1(CCP2) and RC2(CCP1) set to input
    time = 0;
    INTCON = 0xC0; //enable peripheral/global interrupt
    T1CON = 0x01; //off, 1MHz, PS 1:1, always counts when on
    CCP1CON = 0x05; //CCP1 = rising edge
    CCP2CON = 0x04; //CCP2 = falling edge
    TMR1 = 0; //start timer at 0
    PIE1 = 0x05; //enable CCP1IE and TMR1IE
    PIE2 = 0x01; //enable CCP2IE
    PIR1 = 0x00; //set CCP1IF and TMR1IF to 0
    PIR2 = 0x00; //set CCP2IF to 0

  while(1){
  }
}

void __interrupt() isr(void){
  if(CCP1IF == 1){ //triggered on rising edge
    //T1CON = 0x01; //timer on
    //TMR1 = 0; //start timer at 0
    t0 = time + CCPR1;
    CCP1IF = 0;
  }
  if(TMR1IF == 1){ //timer overflows, add 65535 us to time
    time = time + 0x10000 - 1;
    TMR1IF = 0;
  }
  if(CCP2IF == 1){ //triggered on falling edge
    //T1CON = 0x00; //timer off
    t1 = time + CCPR2; //add current TMR1 value to time
    //TMR1 = 0; //start timer at 0
    CCP2IF = 0;
    time = t1 - t0; //subtract starting time 
    
    if(time < 200000) display(1);
    else if(time > 200000 && time < 400000) display(2);  
    else display(3);
    
    time = 0;
    t0 = 0;
    t1 = 0;
  }
}