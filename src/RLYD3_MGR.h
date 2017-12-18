/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   RLYD3_MGR
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
#ifndef _RLYD3_MGR_H_
#define _RLYD3_MGR_H_

#define MAX_OVERFLOW_COUNTS                 (10000)

/* Time delay to clear the local count of 3D track system */
#define LOCAL_COUNT_CLEARING_TIMEOUT    (60)

extern void Initialise_Relay_D3_Mgr(void);                  //Defined in RLYD3_MGR.c
extern void Update_Relay_D3_State(void);                    //Defined in RLYD3_MGR.c
extern void Set_Relay_D3_DAC_Defective(void);               //Defined in RLYD3_MGR.c
extern void Process_D3_Local_AxleCount (UINT16,UINT16);     //Defined in RLYD3_MGR.c
extern void Process_D3_Local_Direction(BYTE,UINT16);        //Defined in RLYD3_MGR.c
#endif
