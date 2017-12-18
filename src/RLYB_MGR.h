/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   RLYB_MGR
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
#ifndef _RLYB_MGR_H_
#define _RLYB_MGR_H_

extern void Initialise_Relay_B_Mgr(void);                   //Defined in RLYB_MGR.c
extern void Update_Relay_B_State(void);                     //Defined in RLYB_MGR.c
extern void Process_Relay_B_Local_AxleCount(UINT16,UINT16); //Defined in RLYB_MGR.c
extern void Process_Relay_B_Local_Direction(BYTE,UINT16);   //Defined in RLYB_MGR.c
extern void Update_3S_Relay_B_State(void);                  //Defined in RLYB_MGR.c

#endif
