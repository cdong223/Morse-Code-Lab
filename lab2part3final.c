/*
 * File:   Part3.c
 * Author: Claire Dong, Zohair Zia
 *
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
#include <string.h>
#define _XTAL_FREQ 4000000

long int total_time, time, t0, t1;  //variables used for measuring pulse widths
int display_active = 0;             //conditional for TMR1 interrupt
int count = 0;                      //track spaces vs. pauses
int decode_flag = 0;                //flag for allowing decoding to occur
int calculate_flag = 0;             //flag for assigning dots and dashes to character array
char letter[5] = "00000";           //character array, to be decoded for each symbol after assigning sequence of dots/dashes
int index = 0;
char d = 'd';
char D = 'D';
char e = 'e';
char table[16][5] = {               //lookup table used for decoding symbol
                    "DDDDD", //0
                    "dDDDD", //1
                    "ddDDD", //2
                    "dddDD", //3
                    "ddddD", //4 
                    "ddddd", //5 
                    "Ddddd", //6
                    "DDddd", //7
                    "DDDdd", //8 
                    "DDDDd", //9
                    "dD000", //A
                    "Dddd0", //B
                    "DdDd0", //C 
                    "Ddd00", //D
                    "d0000", //E 
                    "ddDd0"  //F
};

void display(int x){ //function to display symbol
    PORTAbits.RA0 = 1;
    switch(x){
        case 0:
            PORTCbits.RC0 = 0;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
            break;
        case 1:
            PORTCbits.RC0 = 0;
            PORTAbits.RA2 = 0;
            PORTAbits.RA3 = 0;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 0;
            PORTCbits.RC7 = 0;
             break;
        case 2:
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 0;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 0;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
             break;
        case 3:
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 0;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 0;
             break;
        case 4:
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 0;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 0;
            PORTCbits.RC7 = 0;
             break;
        case 5:
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 0;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 0;
             break;
        case 6:
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 0;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
             break;
        case 7:
            PORTCbits.RC0 = 0;
            PORTAbits.RA2 = 0;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 0;
            PORTCbits.RC7 = 0;
             break;
        case 8:
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
             break;
        case 9:
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 0;
            PORTCbits.RC7 = 0;
             break;
        case 10: //A
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 0;
            PORTCbits.RC7 = 1;
            break;
        case 11: //B
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 0;
            PORTCbits.RC3 = 0;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
            break;
        case 12: //C
            PORTCbits.RC0 = 0;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 0;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 0;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
            break;
        case 13: //D
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 0;
            PORTAbits.RA3 = 0;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 1;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
            break;
        case 14: //E
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 0;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 0;
            PORTCbits.RC6 = 1;
            PORTCbits.RC7 = 1;
            break;
        case 15: //F
            PORTCbits.RC0 = 1;
            PORTAbits.RA2 = 1;
            PORTAbits.RA3 = 1;
            PORTCbits.RC3 = 0;
            PORTCbits.RC4 = 0;
            PORTCbits.RC5 = 0;
            PORTCbits.RC6 = 0;
            PORTCbits.RC7 = 1;
            break;
        case 16: //error
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
    __delay_ms(2000);           //display for 2 seconds
    PORTCbits.RC0 = 0;          //disable display
    PORTAbits.RA2 = 0;
    PORTAbits.RA3 = 0;
    PORTCbits.RC3 = 0;
    PORTCbits.RC4 = 0;
    PORTCbits.RC5 = 0;
    PORTCbits.RC6 = 0;
    PORTCbits.RC7 = 0;
    PORTAbits.RA0 = 0;
    memset(letter, '0', 5);     //clear character array for next input
    index = 0;                  //reset variables and clear any interrupt flags triggered during display
    TMR1 = 0;
    TMR1IF = 0;
    CCP1IF = 0;
    CCP2IF = 0;
}

void decode(char input[5]){     //decode function for going through lookup table
   for (int i = 0 ; i < 16 ; i++) {
       for (int j = 0 ; j < 5 ; j++) {
           if (input[j] != table[i][j]) break;
           if (j == 4) {
               display(i);
               return;
           }
       }
   }
   display(16);
}

void calculate(long int time){  //function for assigning dot/dash sequence of input to character array
    
    if(time > 30000 && time < 200000) letter[index] = d; //dot
    else if(time > 200000 && time < 400000) letter[index] = D; //dash
    else letter[index] = e; //error
    index = index + 1;

}

void main(void){        //initialize registers for I/O, interrupts, and time config
    ANSEL = 0x00;       //PORTA = I/O
    ANSELH = 0x00;      //PORTB = I/O
    TRISA = 0x00;       //output
    TRISB = 0x01;       //RB0 = input
    TRISC = 0x06;       //RC1(CCP2) and RC2(CCP1) set to input
    TMR1 = 0;           //start timer at 0
    INTCON = 0xC0;      //enable peripheral/global interrupt
    T1CON = 0x01;       //off, 1MHz, PS 1:1, always counts when on
    CCP1CON = 0x05;     //CCP1 = rising edge
    CCP2CON = 0x04;     //CCP2 = falling edge
    PIE1 = 0x05;        //enable CCP1IE and TMR1IE
    PIE2 = 0x01;        //enable CCP2IE
    PIR1 = 0x00;        //set CCP1IF and TMR1IF to 0
    PIR2 = 0x00;        //set CCP2IF to 0

    while(1){           //handle decoding in main, do important variable assignments in ISR
        if(decode_flag == 1){
            decode(letter);
            decode_flag = 0;
            
            
        }
        if(calculate_flag == 1){
            calculate(total_time);
            calculate_flag = 0;
        }
    }
}

void __interrupt() isr(void){
  if(CCP1IF == 1){ //triggered on rising edge
    t0 = CCPR1;   //capture rising edge time
    display_active = 0;
    count = 0;
    CCP1IF = 0;
  }
  if(TMR1IF == 1){ //increment time by 2^16 to keep track of overflows
    if(display_active == 0) time = time + 65536;
    else{ //when display_active = 1, any overflows occurring are used to detect pause
      count = count + 1;
      if(count == 7) decode_flag = 1; //once pause detected, ready to decode
      //else TMR1 = 15535; //overflow at t=50 when low
    }
    TMR1IF = 0;     //reset timer interrupt and value
    TMR1 = 0;
  }
  if(CCP2IF == 1){ //triggered on falling edge
    t1 = CCPR2 - 400;    //capture falling edge time, offset by -400 microseconds to account for delay in latching due to processing occurring when pulse is high
    TMR1 = 58752;
    total_time = time + t1 - t0;
    calculate_flag = 1;     //ready to assign dot/dash sequence to character array
    time = 0; 
    display_active = 1;     //ready to display
    CCP2IF = 0;
  }
}
