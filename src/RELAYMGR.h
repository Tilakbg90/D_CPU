/*****************************************************************************
    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   RELAYMGR
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
extern void Update_FDP_Relay_A_Counts(void);        //Defined in RELAYMGR.c
extern void Update_Relay_B_Counts(void);            //Defined in RELAYMGR.c
extern void Update_Relay_D3_Counts(void);           //Defined in RELAYMGR.c
extern void Update_Relay_D4_Counts(void);           //Defined in RELAYMGR.c
extern void Update_3S_Relay_Counts(void);           //Defined in RELAYMGR.c
extern void Update_3S_Relay_A_Counts(void);         //Defined in RELAYMGR.c
extern void Update_3S_Relay_B_Counts(void);         //Defined in RELAYMGR.c


extern void Update_SF_Track_Status(void);           //Defined in RELAYMGR.c
extern void Update_CF_Track_Status(void);           //Defined in RELAYMGR.c
extern void Update_EF_Track_Status(void);           //Defined in RELAYMGR.c

extern void A_Sec_Led_drive(Led_State);
extern void B_Sec_Led_drive(Led_State);

#endif
