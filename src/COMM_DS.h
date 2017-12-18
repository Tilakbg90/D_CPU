/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   COMM_DS
    Target MCU          :
    Compiler            :
    Author              :
    Date                :
    Company Name        :
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
