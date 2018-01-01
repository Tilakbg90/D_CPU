/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    COMM_DS
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
#ifndef _COMM_DS_H_
#define _COMM_DS_H_

/************Input signals to microcontroller from the modem card ***********/

    /*  Carrier Detect signal to check for the carrier generation */

#define MODEM_B_CD                  PORTEbits.RE9   //CD1B/CD2B Signal on Schematics

    /* Channel status signal */

#define MODEM_B_CHANNEL_STATUS      PORTFbits.RF0   // PSP0A2 Signal on Schematics

/************Input signal to microcontroller from the modem card **********/


/********** Output signal from  microcontroller to modem card  *************/

    /*  Request to Send signal for transmission */

#define MODEM_B_M0                  LATAbits.LATA14 //RS1B Signal on Schematics

#define MODEM_B_M1                  LATEbits.LATE8 //PORTEbits.RE8   // CS1B Signal On Schematics

    /* Channel control signal */

#define MODEM_B_CHANNEL_CONTROL     LATFbits.LATF1 // PSP1A2 Signal on Schematics

/********** Output signal from  microcontroller to modem card  *************/

extern void SetupCOM2BaudRate(void); /* Defined in COMM_DS.c */

#endif
