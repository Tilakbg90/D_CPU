/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RLYB_MGR
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
#ifndef _RLYB_MGR_H_
#define _RLYB_MGR_H_

extern void Initialise_Relay_B_Mgr(void);                   //Defined in RLYB_MGR.c
extern void Update_Relay_B_State(void);                     //Defined in RLYB_MGR.c
extern void Process_Relay_B_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount); //Defined in RLYB_MGR.c
extern void Process_Relay_B_Local_Direction(BYTE uchDirection,UINT16 uiAxleCount);   //Defined in RLYB_MGR.c
extern void Update_3S_Relay_B_State(void);                  //Defined in RLYB_MGR.c

#endif
