/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RESTORE
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
#ifndef _RESTORE_H_
#define _RESTORE_H_
/*
 *      Restoration routines
 */
/************************** Out put Ports******************************************/
#define CS11                LATAbits.LATA4
#define CS12                LATF1
#define CS13                LATF2
#define CS14                LATF3
#define CS15                LATJ3
#define CS16                LATE2
#define CS17                LATG0
/*CS11 – CS14 are Chip select signal.
When CS11=0, CS12=1, CS13=1, CS14=1, CS15=1 and CS16=1. Dip Switch DSw1 to DSw4 of S1 is read.
When CS11=1, CS12=0, CS13=1, CS14=1, CS15=1 and CS16=1. Dip Switch DSw5 to DSw8 of S1 is read.
When CS11=1, CS12=1, CS13=0, CS14=1, CS15=1 and CS16=1. Dip Switch DSw1 to DSw4 of S2 is read.
When CS11=1, CS12=1, CS13=1, CS14=0, CS15=1 and CS16=1. Dip Switch DSw5 to DSw8 of S2 is read.
When CS11=1, CS12=1, CS13=1, CS14=1, CS15=0 and CS16=1. Dip Switch DSw1 to DSw4 of S3 is read.
When CS11=1, CS12=1, CS13=1, CS14=1, CS15=1 and CS16=0. Dip Switch DSw5 to DSw8 of S3 is read. */

//#define CARD_SEL1         LATF4
//#define CARD_SEL2         LATF5
//#define CARD_SEL3         LATF6

/*CARD_SEL1, CARD_SEL2,CARD_SEL3 are the card detection signals which are fed
to an 8 input Multiplexed IC.Depending on the inputs given, particular card
will get detected for its presence.*/

/************************** Out put Ports******************************************/

/* ID Locations from where CRC-32 CheckSum will be read */


#define All_BOARD_PRESENT   0xFF        //Macro for all board presence
#endif
