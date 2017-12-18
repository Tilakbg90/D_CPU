/*****************************************************************************
    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   RLYA_MGR
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
#ifndef _RLYA_MGR_H_
#define _RLYA_MGR_H_


extern void Initialise_Relay_A_Mgr(void);                       //Defined in RLYA_MGR.c
extern void Update_Relay_A_State(void);                         //Defined in RLYA_MGR.c
extern void Process_Relay_A_Local_AxleCount(UINT16,UINT16);     //Defined in RLYA_MGR.c
extern void Process_Relay_A_Local_Direction(BYTE,UINT16);       //Defined in RLYA_MGR.c
extern void Update_3S_Relay_A_State(void);                      //Defined in RLYA_MGR.c
#endif
