/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   COMM_US
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
extern void Setup_FDP_COM1BaudRate(void);   /* defined in COMM1_FDP.c */

#endif

