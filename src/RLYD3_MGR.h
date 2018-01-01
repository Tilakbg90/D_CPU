/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RLYD3_MGR
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
#ifndef _RLYD3_MGR_H_
#define _RLYD3_MGR_H_

#define MAX_OVERFLOW_COUNTS_3                 (10000)

/* Time delay to clear the local count of 3D track system */
#define LOCAL_COUNT_CLEARING_TIMEOUT_3    (60)

extern void Initialise_Relay_D3_Mgr(void);                  //Defined in RLYD3_MGR.c
extern void Update_Relay_D3_State(void);                    //Defined in RLYD3_MGR.c
extern void Process_D3_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);     //Defined in RLYD3_MGR.c
extern void Process_D3_Local_Direction(BYTE uchDirection,UINT16 uiAxleCount);        //Defined in RLYD3_MGR.c
#endif
