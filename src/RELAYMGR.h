/*****************************************************************************
    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RELAYMGR
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
#ifndef _RELAYMGR_H_
#define _RELAYMGR_H_

/***************Output signal to drive the LEDs ****************************/


#define SECTION_A_LED_OCCUPIED_PORT          LATBbits.LATB2
#define SECTION_B_LED_OCCUPIED_PORT          LATDbits.LATD12

#define SECTION_A_LED_CLEAR_PORT            LATCbits.LATC14
#define SECTION_B_LED_CLEAR_PORT            LATDbits.LATD4

/***************Output signal to drive the LEDs ****************************/

extern void Initialise_Relay_Mgr(void);             //Defined in RELAYMGR.c
extern void Update_Relay_A_Counts(void);            //Defined in RELAYMGR.c
extern void Update_Relay_B_Counts(void);            //Defined in RELAYMGR.c
extern void Update_Relay_D3_Counts(void);           //Defined in RELAYMGR.c
extern void Update_Relay_D4_Counts(void);           //Defined in RELAYMGR.c
extern void Update_3S_Relay_Counts(void);           //Defined in RELAYMGR.c
extern void Update_3S_Relay_A_Counts(void);         //Defined in RELAYMGR.c
extern void Update_3S_Relay_B_Counts(void);         //Defined in RELAYMGR.c


extern void Update_SF_Track_Status(void);           //Defined in RELAYMGR.c
extern void Update_CF_Track_Status(void);           //Defined in RELAYMGR.c
extern void Update_EF_Track_Status(void);           //Defined in RELAYMGR.c

extern void A_Sec_Led_drive(Led_State Led_status);
extern void B_Sec_Led_drive(Led_State Led_status);

#endif
