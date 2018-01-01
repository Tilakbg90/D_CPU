/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    COMM_US
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
#ifndef _COMM_US_H_
#define _COMM_US_H_

/************Input signals to microcontroller from the modem card ***********/

    /*  Carrier Detect signal to check for the carrier generation */

#define MODEM_A_CD                  PORTFbits.RF5   // CD1A/CD2A

    /* Channel status signal */

#define MODEM_A_CHANNEL_STATUS      PORTGbits.RG0   // PSP0A1

/************Input signals to microcontroller from the modem card ***********/

/********** Output signal from  microcontroller to modem card  *************/

    /* Request to Send signal for transmission */

#define MODEM_A_M0                  LATFbits.LATF8  // RS1A

#define MODEM_A_M1                  LATFbits.LATF4   // CS1A

    /* Channel control signal  */

#define MODEM_A_CHANNEL_CONTROL     LATGbits.LATG1  // PSP1A1

/********** Output signal from  microcontroller to modem card  *************/

extern void SetupCOM1BaudRate(void);    /* defined in COMM_US.c */


#endif

