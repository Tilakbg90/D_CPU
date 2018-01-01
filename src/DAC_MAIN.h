/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    DAC_MAIN
    Target MCU          :    PIC24FJ256GB210
    Compiler            :    XC16 V1.31
    Author              :    S Venkata Krishna
    Date                :    15/12/2017
    Company Name        :    Insys Digital Systems Private Limited, Bangalore
    Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
*****************************************************************************/
#ifndef _DAC_MAIN_H_
#define _DAC_MAIN_H_

/* Port Allocation Table and it's Default Values and Directions
    PORTA   Port allocation -   Direction - Default Value   PORTB   Port allocation - Direction -Default Value
PORT RA0 -  PD1 DET           -    1    -     0             PORT RB0- PULSE1_SUP    -    1    -     0
PORT RA1 -  PD2 DET           -    1    -     0             PORT RB1- PULSE2_SUP    -    1    -     0
PORT RA2 -  SMCPU DET         -    1    -     0             PORT RB2- PSP1A         -    0    -     0
PORT RA3 -  COM DET           -    1    -     0             PORT RB3- PSP0A         -    1    -     0
PORT RA4 -  RD1 DET           -    1    -     0             PORT RB4- PULSE1_M      -    1    -     0
PORT RA5 -  RD2 DET           -    1    -     0             PORT RB5- PULSE2_M      -    1    -     0
PORT RA6 -  Osc1 Pin          -    0    -     0             PORT RB6- ICD           -    0    -     0
PORT RA7 -  Osc2 Pin          -    0    -     0             PORT RB7- ICD           -    1    -     0


    PORTC   Port allocation -   Direction - Default Value   PORTD   Port allocation - Direction -Default Value
PORT RC0 -  PREP2_DRV         -    0    -     0             PORT RD0- VR1.FB2         -    1    -     0
PORT RC1 -  VR1 PWM-EN        -    0    -     0             PORT RD1- VR2.FB1         -    1    -     0
PORT RC2 -  VR2.CKFB         -     1    -     0             PORT RD2- VR2.FB2         -    1    -     0
PORT RC3 -  SCK of SPI        -    1    -     0             PORT RD3- RSTA1           -    1    -     1
PORT RC4 -  SDI of SPI        -    1    -     0             PORT RD4- RSTA2           -    1    -     0
PORT RC5 -  SDO of SPI        -    0    -     0             PORT RD5- RSTB1           -    1    -     1
PORT RC6 -  Tx of USART 1     -    0    -     0             PORT RD6- RSTB2           -    1    -     0
PORT RC7 -  RX of USART 1     -    1    -     0             PORT RD7- PREP1.FB1       -    1    -     0


    PORTE   Port allocation -   Direction - Default Value   PORTF   Port allocation - Direction -Default Value
PORT RE0 -  VR1.FB1           -    1    -     0             PORT RF0- CS11            -    0    -     1
PORT RE1 -  VR1.CKFB          -    1    -     0             PORT RF1- CS12            -    0    -     1
PORT RE2 -  CS16              -    0    -     1             PORT RF2- CS13            -    0    -     1
PORT RE3 -  PULSE1_S          -    1    -     0             PORT RF3- CS14            -    0    -     1
PORT RE4 -  PULSE2_S          -    1    -     0             PORT RF4- PREP1.FB2       -    1    -     0
PORT RE5 -  COM1_M0           -    0    -     1             PORT RF5- PREP2.FB1       -    1    -     0
PORT RE6 -  COM1_CD           -    1    -     0             PORT RF6- PREP2.FB2       -    1    -     0
PORT RE7 -  COM1_M1           -    0    -     1             PORT RF7-   SS1           -    1    -     1



PORTG   Port allocation -   Direction - Default Value   PORTH   Port allocation - Direction -Default Value
PORT RG0 -  No Connection     -    0    -     0       PORT RH0- LCD- RS           -    0    -     0
PORT RG1 -  Tx of USART 2     -    0    -     0       PORT RH1- LCD Detect        -    1    -     0
PORT RG2 -  Rx of USART 2     -    1    -     0       PORT RH2- Lcd Data Enable   -    0    -     0
PORT RG3 -  VR2 PWM-EN        -    0    -     0       PORT RH3- Lcd Strobe        -    0    -     0
PORT RG4 -  PREP1_DRV         -    0    -     0       PORT RH4- Lcd Data          -    0    -     0
PORT RG5 -  MCLR pin          -    0    -     0       PORT RH5- Lcd Clock         -    0    -     0
                                                      PORT RH6- PSP0B             -    1    -     0
                                                      PORT RH7- PSP1B             -    0    -     0

PORTJ   Port allocation -   Direction - Default Value
PORT RJ0 -  COM2_M0           -    0    -     1
PORT RJ1 -  COM2_CD           -    1    -     0
PORT RJ2 -  COM2_M1           -    0    -     1
PORT RJ3 -  CS15              -    0    -     1
PORT RJ4 -  SW_AD1            -    1    -     0
PORT RJ5 -  SW_AD2            -    1    -     0
PORT RJ6 -  SW_AD3            -    1    -     0
PORT RJ7 -  SW_AD4            -    1    -     0

*/

/******* Default values to be loaded on to I/O PORTS on Power-up*************/
#define PORT_A_DEFAULT  0b00000000
#define PORT_B_DEFAULT  0b00000000
#define PORT_C_DEFAULT  0b00000000
#define PORT_D_DEFAULT  0b00101000
#define PORT_E_DEFAULT  0b00000100
#define PORT_F_DEFAULT  0b10001111
#define PORT_G_DEFAULT  0b00000001
#define PORT_H_DEFAULT  0b00000000
#define PORT_J_DEFAULT  0b00001000
/********* Default values to be loaded on to I/O PORTS on Power-up**********/


/***Default PORT Directions to be loaded on to DIRECTION REGISERS on Power-up***/

#define PORT_A_DIRECTION    0b00111111
#define PORT_B_DIRECTION    0b00111011
#define PORT_C_DIRECTION    0b10011100
#define PORT_D_DIRECTION    0b11111111
#define PORT_E_DIRECTION    0b01011011
#define PORT_F_DIRECTION    0b11110000
#define PORT_G_DIRECTION    0b00000100
#define PORT_H_DIRECTION    0b01000010
#define PORT_J_DIRECTION    0b11110010

/********* Default PORT Directions to be loaded on to DIRECTION REGISERS on Power-up*********/

/********************************Timer configuration************************************/
    /* Set as 16-bit Counter, 1:1 Pre-Scaler RA4 -PD1 MAIN PULSE*/
#define TIMER0_CONFIG       0b00101000

    /* Set as 16-bit Counter, 1:1 Pre-Scaler RC0 -PD2 MAIN PULSE*/
#define TIMER1_CONFIG       0b10000110

    /* Set Pre-Scaler 1:4 and Post-Scaler to 1 */
#define TIMER2_CONFIG       0b00000001

    /* 40-MHz, Set as 16-bit Timer,   1:8 Pre-Scaler */
#define TIMER3_CONFIG       0b10110000

    /* 40-MHz, Set as 1:16 Pre-Scaler,1:16 Post-Scaler */
#define TIMER4_CONFIG       0b01111010

    /* 50 mSec, 3035 - 40MHz */
#define TIMER3_SETPOINT     (3035)

    /* 1 mSec , 39   - 40MHz */
#define TIMER4_SETPOINT     (39)
/********************************Timer configuration************************************/
#endif
