 /*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   RESET
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
    Functions       :   void Initialise_SF_Reset_Monitor(void)
                        void Initialise_CF_Reset_Monitor(void)
                        void Initialise_EF_Reset_Monitor(void)
                        void Initialise_DE_Reset_Monitor(void)
                        void Start_SF_Reset_Monitor(void)
                        void Start_CF_Reset_Monitor(void)
                        void Start_EF_Reset_Monitor(void)
                        void Start_DE_Reset_Monitor(void)
                        void Update_SF_Reset_Monitor_State(void)
                        void SF_Reset_Sequence_Complete(void)
                        void SF_Wait_for_Local_Reset(void)
                        void Update_CF_Reset_Monitor_State(void)
                        void CF_DS_Reset_Sequence_Complete(void)
F                       void CF_US_Reset_Sequence_Complete(void)
                        void CF_Wait_for_Local_Reset(void)
                        void Process_CF_DS_Reset_Command(void);
                        void Process_CF_US_Reset_Command(void);
                        void Process_D3_Reset_Command(void);
                        void Update_EF_Reset_Monitor_State(void)
                        void EF_Reset_Sequence_Complete(void)
                        void EF_Wait_for_Local_Reset(void)
                        void Update_DE_Reset_Monitor_State(void)
                        void DE_Reset_Sequence_Complete(void)
                        void DE_Wait_for_Local_Reset(void)
                        void Update_Reset_Button_State(void);
                        void Update_Reset_Button2_State(void);
                        void Clear_Reset_Info(void)
                        void Clear_Reset_State(void)
                        void Clear_Local_Reset_Flag(void)
                        void Decrement_Reset_msTmr(void)
                        void Decrement_Reset_300sTmr(void)
                        void Decrement_Reset_300sTmr2(void)
                        BYTE Get_SF_Reset_State(void)
                        BYTE Get_SF_DS1_Reset_State(void)
                        BYTE Get_SF_DS2_Reset_State(void)
                        BYTE Get_CF_DS_Reset_State(void)
                        BYTE Get_CF_DS1_Reset_State(void)
                        BYTE Get_CF_DS2_Reset_State(void)
                        BYTE Get_CF_US_Reset_State(void)
                        BYTE Get_CF_US1_Reset_State(void)
                        BYTE Get_CF_US2_Reset_State(void)
                        BYTE Get_EF_Reset_State(void)
                        BYTE Get_EF_US1_Reset_State(void)
                        BYTE Get_EF_US2_Reset_State(void)
                        BYTE Get_DE_Reset_State(void)
                        void Post_DS1_has_been_Reset(BYTE uchData)
                        void Post_DS2_has_been_Reset(BYTE uchData)
                        void Post_US1_has_been_Reset(BYTE uchData)
                        void Post_US2_has_been_Reset(BYTE uchData)
                        void Post_Peer_CPU_Reset(BYTE uchData)
                        void Post_Peer_CPU_Reset1(BYTE uchData)
                        void Update_US_Section_Remote_Reset(void)
                        void Update_US_Section_Remote_Preparatory(void)
                        void Update_DS_Section_Remote_Reset(void)
                        void Update_DS_Section_Remote_Preparatory(void)
                        BYTE Update_System_Mode(void)
                        void Clear_US_Remote_Reset_Flag(void)
                        void Clear_DS_Remote_Reset_Flag(void)
                        BYTE Update_US_Section_Mode(BYTE us_remote_mode,BYTE us_local_mode)
                        BYTE Update_DS_Section_Mode(BYTE ds_remote_mode,BYTE ds_local_mode)
*****************************************************************************/
#include <xc.h>
//#include <sys.h>

#include "COMMON.h"
#include "RESET.h"

extern  /*near*/  dip_switch_info_t DIP_Switch_Info;
extern  /*near*/  dac_status_t Status;                    /* from dac_main.c */
reset_info_t Reset_Info  __attribute__ ((persistent));
reset_button_info_t Reset_Button_Info;
reset_button_info_t Reset_Button2_Info;
extern  ds_section_mode DS_Section_Mode;             /*from DAC_MAIN.c*/
extern  us_section_mode US_Section_Mode;             /*from DAC_MAIN.c*/


const BYTE uchDAC_Status_Table[NO_OF_REMOTE_UNIT_STATES][NO_OF_LOCAL_UNIT_STATES] = {
{ZERO_MODE, ZERO_MODE,                         ERROR_RESET_APPLIED_AT_LOCAL_UNIT, ZERO_MODE,                   ZERO_MODE,                   ZERO_MODE,                      SECTION_ERROR_AT_BOTH_UNITS},
{ZERO_MODE, WAITING_FOR_RESET_AT_BOTH_UNITS,   RESET_APPLIED_AT_LOCAL_UNIT,       ZERO_MODE,                   ZERO_MODE,                   ZERO_MODE,                      ERROR_REMOTE_UNIT_WAITING_FOR_RESET},
{ZERO_MODE, RESET_APPLIED_AT_REMOTE_UNIT,      RESET_APPLIED_AT_BOTH_UNITS,       ERROR_RESET_APPLIED_AT_REMOTE_UNIT,ZERO_MODE,             ZERO_MODE,                      ERROR_RESET_APPLIED_AT_REMOTE_UNIT},
{ZERO_MODE, ZERO_MODE,                         ZERO_MODE,                         SECTION_WAIT_FOR_PILOT_TRAIN,ZERO_MODE,                   ZERO_MODE,                      SECTION_ERROR_AT_BOTH_UNITS},
{ZERO_MODE, ZERO_MODE,                         ZERO_MODE,                         ZERO_MODE,                   SECTION_CLEAR_AT_BOTH_UNITS, ZERO_MODE,                      SECTION_ERROR_AT_BOTH_UNITS},
{ZERO_MODE, ZERO_MODE,                         ZERO_MODE,                         ZERO_MODE,                   ZERO_MODE,                   SECTION_OCCUPIED_AT_BOTH_UNITS, SECTION_ERROR_AT_BOTH_UNITS},
{ZERO_MODE, ERROR_LOCAL_UNIT_WAITING_FOR_RESET,ERROR_RESET_APPLIED_AT_LOCAL_UNIT, ZERO_MODE,                   ZERO_MODE,                   ZERO_MODE,                      SECTION_ERROR_AT_BOTH_UNITS},
{ZERO_MODE, WAITING_FOR_RESET_AT_BOTH_UNITS   ,RESET_APPLIED_AT_LOCAL_UNIT       ,SECTION_WAIT_FOR_PILOT_TRAIN,SECTION_CLEAR_AT_BOTH_UNITS, SECTION_OCCUPIED_AT_BOTH_UNITS, SECTION_ERROR_AT_BOTH_UNITS},
};

void SF_Wait_for_Local_Reset(void);
void CF_Wait_for_Local_Reset(void);
void EF_Wait_for_Local_Reset(void);

void SF_Reset_Sequence_Complete(void);
void CF_DS_Reset_Sequence_Complete(void);
void CF_US_Reset_Sequence_Complete(void);
void Process_CF_DS_Reset_Command(void);
void Process_CF_US_Reset_Command(void);
void Process_D3_Reset_Command(void);
void Process_D4_Reset_Command(void);
void EF_Reset_Sequence_Complete(void);
void Update_Reset_Button_State(void);
void Update_Reset_Button2_State(void);
void LCWS_Wait_for_Local_Reset(void);
void Update_LCWS_Reset_Monitor_State(void);
extern BOOL Reset_Allowed_For_LCWS(void);
extern BOOL Reset_Allowed_For_DE(void);

void Initialise_LCWS_Reset_Monitor(void);
void Start_LCWS_Reset_Monitor(void);
void Initialise_DE_Reset_Monitor(void);
void Start_DE_Reset_Monitor(void);
void Update_DE_Reset_Monitor_State(void);
void DE_Wait_for_Local_Reset(void);


/*********************************************************************
Component name      :RESET
Module Name         :void Initialise_SF_Reset_Monitor(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initialise SF reset monitor,update checksum



Design Requirements     :

Interfaces
    Calls           :   

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()

Input Variables         Name                                    Type
    Global          :   SF_WAIT_FOR_RESET                       Enumerator
                        RESET_BUTTON_POLLING_NOT_STARTED        Enumerator
                        WAITING_FOR_RESET_MODE                  Enumerator

    Local           :   None

Output Variables        Name                                            Type
    Global          :   Reset_Button_Info.State                         Enumerator
                        Reset_Button_Info.Timeout_ms                    UINT16
                        Reset_Info.SF.Flags.DS1_has_been_Reset          Bit
                        Reset_Info.SF.Flags.DS2_has_been_Reset          Bit
                        Reset_Info.SF.Flags.Peer_CPU_has_been_Reset     Bit
                        DS_Section_Mode.Flags.Remote_Preparatory_State  Bit
                        DS_Section_Mode.Flags.Remote_Reset_Applied      Bit
                        DS_Section_Mode.Local_Unit                      BYTE
                        DS_Section_Mode.Remote_Unit                     BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                       Value
                            FALSE                       0
                            TRUE                        1

References          :

Derived Requirements:



Algorithm           :1.Initialize the SF system where in the system waits for reset signal
                     2.For the SF system to reset it is necessary for the remote unit to
                       reset, hence set a seperate flags to check the reset state of remote unit
                            down stream CPU1,
                            down stream CPU2 &
                            local unit peer CPU
                     3.Initialise the Down stream section reset, preparatory flags for the remote unit.
                     4.Intialise the modes of the local and remote units of the down section.
                     


************************************************************************/
void Initialise_SF_Reset_Monitor(void)
{
    Reset_Button2_Info.State = (BYTE) RESET_BUTTON_POLLING_NOT_STARTED;
    Reset_Button2_Info.Timeout_ms = 0;
    Reset_Info.SF.Flags.DS1_has_been_Reset = FALSE;
    Reset_Info.SF.Flags.DS2_has_been_Reset = FALSE;
    Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
    DS_Section_Mode.Flags.Remote_Reset_Applied =  FALSE;
    DS_Section_Mode.Flags.Remote_Preparatory_State =  TRUE;
    DS_Section_Mode.Local_Unit = (BYTE)WAITING_FOR_RESET_MODE;
    DS_Section_Mode.Remote_Unit = (BYTE)WAITING_FOR_RESET_MODE;
}


void Initialise_LCWS_Reset_Monitor(void)
{
    Reset_Button2_Info.State = (BYTE) RESET_BUTTON_POLLING_NOT_STARTED;
    Reset_Button2_Info.Timeout_ms = 0;
    Reset_Info.SF.Flags.DS1_has_been_Reset = FALSE;
    Reset_Info.SF.Flags.DS2_has_been_Reset = FALSE;
    Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
    DS_Section_Mode.Flags.Remote_Reset_Applied =  FALSE;
    DS_Section_Mode.Flags.Remote_Preparatory_State =  TRUE;
    DS_Section_Mode.Local_Unit = (BYTE)WAITING_FOR_RESET_MODE;
    DS_Section_Mode.Remote_Unit = (BYTE)WAITING_FOR_RESET_MODE;
}


void Initialise_DE_Reset_Monitor(void)
{
    Reset_Button2_Info.State = (BYTE) RESET_BUTTON_POLLING_NOT_STARTED;
    Reset_Button2_Info.Timeout_ms = 0;
    Reset_Info.SF.Flags.DS1_has_been_Reset = FALSE;
    Reset_Info.SF.Flags.DS2_has_been_Reset = FALSE;
    Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
    DS_Section_Mode.Flags.Remote_Reset_Applied =  FALSE;
    DS_Section_Mode.Flags.Remote_Preparatory_State =  TRUE;
    DS_Section_Mode.Local_Unit = (BYTE)WAITING_FOR_RESET_MODE;
    DS_Section_Mode.Remote_Unit = (BYTE)WAITING_FOR_RESET_MODE;
}

/*********************************************************************
Component name      :RESET
Module Name         :void Initialise_CF_Reset_Monitor(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initialise CF reset monitor,update checksum


Design Requirements     :

Interfaces
    Calls           :   

    Called by       :   DAC_MAIN.c  -   Start_CF_Reset_Sequence()

Input Variables         Name                                    Type
    Global          :   CF_WAIT_FOR_RESET                       Enumerator
                        RESET_BUTTON_POLLING_NOT_STARTED        Enumerator
                        WAITING_FOR_RESET_MODE                  Enumerator
                        DP3_MODE                                Enumerator
                        DAC_UNIT_TYPE_CF                        Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator


    Local           :   None

Output Variables        Name                                            Type
    Global          :   Reset_Button_Info.State                         Enumerator
                        Reset_Button_Info.Timeout_ms                    UINT16
                        Reset_Info.CF.Flags.DS1_has_been_Reset          Bit
                        Reset_Info.CF.Flags.DS2_has_been_Reset          Bit
                        Reset_Info.CF.Flags.US1_has_been_Reset          Bit
                        Reset_Info.CF.Flags.US2_has_been_Reset          Bit
                        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset     Bit
                        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1    Bit
                        Reset_Info.CF.Flags.Local_Reset1_done           Bit
                        Reset_Info.CF.Flags.Local_Reset2_done           Bit
                        DS_Section_Mode.Flags.Remote_Preparatory_State  Bit
                        DS_Section_Mode.Flags.Remote_Reset_Applied      Bit
                        DS_Section_Mode.Local_Unit                      BYTE
                        DS_Section_Mode.Remote_Unit                     BYTE
                        US_Section_Mode.Flags.Remote_Preparatory_State  Bit
                        US_Section_Mode.Flags.Remote_Reset_Applied      Bit
                        US_Section_Mode.Local_Unit                      BYTE
                        US_Section_Mode.Remote_Unit                     BYTE

    Local           :   None

Signal Variables


Macro definitions used:     Macro                       Value
                            FALSE                       0
                            TRUE                        1

References          :

Derived Requirements:
Algorithm           :1.Initialize the CF system where in the system waits for reset signal
                     2.For the CF system to reset it is necessary for the remote units to
                       reset, hence set a seperate flags to check the reset state of remote units
                            down stream CPU1,
                            down stream CPU2,
                            Up stream CPU1,
                            Up stream CPU2
                            Peer CPU reset
                            Peer CPU reset2
                            local unit reset1 &
                            local unit reset2
                     3.Initialise the Down stream and Upstream section reset, preparatory flags for the remote unit.
                     4.Intialise the modes of the local and remote units of the down and up section.
                     

************************************************************************/
void Initialise_CF_Reset_Monitor(void)
{
    Reset_Button_Info.State  = (BYTE) RESET_BUTTON_POLLING_NOT_STARTED;
    Reset_Button2_Info.State = (BYTE) RESET_BUTTON_POLLING_NOT_STARTED;
    Reset_Button_Info.Timeout_ms = 0;
    Reset_Button2_Info.Timeout_ms = 0;
    Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
    Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
    Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
    Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
    Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
    Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
    DS_Section_Mode.Flags.Remote_Reset_Applied =  FALSE;
    DS_Section_Mode.Flags.Remote_Preparatory_State =  TRUE;
    US_Section_Mode.Flags.Remote_Reset_Applied =  FALSE;
    US_Section_Mode.Flags.Remote_Preparatory_State =  TRUE;
    DS_Section_Mode.Local_Unit = (BYTE)WAITING_FOR_RESET_MODE;
    if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A ||
       DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B ||
       DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
        {
         DS_Section_Mode.Remote_Unit = (BYTE)DP3_MODE;
        }
    else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A ||
       DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B ||
       DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C ||
       DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
        {
         DS_Section_Mode.Remote_Unit = (BYTE)DP3_MODE;
        }
    else
        {
         DS_Section_Mode.Remote_Unit = (BYTE)WAITING_FOR_RESET_MODE;
        }
    US_Section_Mode.Local_Unit = (BYTE)WAITING_FOR_RESET_MODE;
    US_Section_Mode.Remote_Unit = (BYTE)WAITING_FOR_RESET_MODE;

}
/*********************************************************************
Component name      :RESET
Module Name         :void Initialise_EF_Reset_Monitor(void)
Created By          :
Date Created        :

Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initialise EF reset monitor,update checksum


Design Requirements     :
Interfaces
    Calls           :   

    Called by       :   DAC_MAIN.c  -   Start_EF_Reset_Sequence()

Input Variables         Name                                    Type
    Global          :   EF_WAIT_FOR_RESET                       Enumerator
                        RESET_BUTTON_POLLING_NOT_STARTED        Enumerator
                        WAITING_FOR_RESET_MODE                  Enumerator

    Local           :   None

Output Variables        Name                                        Type
    Global          :   Reset_Button_Info.State                     Enumerator
                        Reset_Button_Info.Timeout_ms                UINT16
                        Reset_Info.EF.Flags.US1_has_been_Reset      Bit
                        Reset_Info.EF.Flags.US2_has_been_Reset      Bit
                        Reset_Info.EF.Flags.Peer_CPU_has_been_Reset Bit
                        US_Section_Mode.Flags.Remote_Preparatory_State  Bit
                        US_Section_Mode.Flags.Remote_Reset_Applied      Bit
                        US_Section_Mode.Local_Unit                      BYTE
                        US_Section_Mode.Remote_Unit                     BYTE

    Local           :   None

Signal Variables


Macro definitions used:     Macro                       Value
                            FALSE                       0
                            TRUE                        1

References          :

Derived Requirements:
Algorithm           :1.Initialize the EF system where in the system waits for reset signal
                     2.For the EF system to reset it is necessary for the remote unit to
                       reset, hence set a seperate flags to check the reset state of remote unit
                            Up stream CPU1,
                            Up stream CPU2 &
                            local unit peer CPU
                     3.Initialise the Upstream section reset, preparatory flags for the remote unit.
                     4.Intialise the modes of the local and remote units of the up section.
                     
************************************************************************/
void Initialise_EF_Reset_Monitor(void)
{
    Reset_Button_Info.State = (BYTE) RESET_BUTTON_POLLING_NOT_STARTED;
    Reset_Button_Info.Timeout_ms = 0;
    Reset_Info.EF.Flags.US1_has_been_Reset = FALSE;
    Reset_Info.EF.Flags.US2_has_been_Reset = FALSE;
    Reset_Info.EF.Flags.Peer_CPU_has_been_Reset = FALSE;
    US_Section_Mode.Flags.Remote_Reset_Applied =  FALSE;
    US_Section_Mode.Flags.Remote_Preparatory_State =  TRUE;
    US_Section_Mode.Local_Unit = WAITING_FOR_RESET_MODE;
    US_Section_Mode.Remote_Unit = WAITING_FOR_RESET_MODE;
}


/*********************************************************************
Component name      :RESET
Module Name         :void Start_SF_Reset_Monitor(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start reset button polling for SF

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()

Input Variables         Name                                Type
    Global          :   Status.Flags.Local_Reset_Done       Bit
                        WAIT_FOR_RESET_BUTTON_PRESS         Enumerator

    Local           :   None

Output Variables        Name                                Type
    Global          :   Reset_Button_Info.State             Enumerator
                        Reset_Button_Info.Timeout_300s      UINT16
                        Reset_Button2_Info.Timeout_300s     UINT16

    Local           :   None

Signal Variables


Macro definitions used:     Macro                            Value
                            SM_RESET_PENDING                  0
                            COOPERATIVE_RESET_MAX_TIMEOUT     6000


References          :

Derived Requirements:
Algorithm           :1.Load the reset button and button2 300s timer values
                     2.If the local reset flag is not set, start polling of
                     reset button by making the system wait for the reset button to
                     be pressed
************************************************************************/
void Start_SF_Reset_Monitor(void)
{
   Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
    if (Status.Flags.Local_Reset_Done2 == SM_RESET_PENDING)
    {
     Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS; /* Start reset button polling */
    }

}


void Start_LCWS_Reset_Monitor(void)
{
   Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
    if (Status.Flags.Local_Reset_Done2 == SM_RESET_PENDING)
    {
     Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS; /* Start reset button polling */
    }

}



void Start_DE_Reset_Monitor(void)
{
   Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
    if (Status.Flags.Local_Reset_Done2 == SM_RESET_PENDING)
    {
     Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS; /* Start reset button polling */
    }

}
/*********************************************************************
Component name      :RESET
Module Name         :void Start_CF_Reset_Monitor(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24

                            2           01              67              10-12-05         Refer ATR No.67


Abstract            :Start reset button polling for CF


Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Start_CF_Reset_Sequence()

Input Variables         Name                                Type
    Global          :   Status.Flags.Local_Reset_Done       Bit
                        Status.Flags.Local_Reset_Done2      Bit
                        WAIT_FOR_RESET_BUTTON_PRESS         Enumerator

    Local           :   None

Output Variables        Name                                Type
    Global          :   Reset_Button_Info.State             Enumerator
                        Reset_Button2_Info.State            Enumerator
                        Reset_Button_Info.Timeout_300s      UINT16
                        Reset_Button2_Info.Timeout_300s     UINT16

    Local           :   None

Signal Variables


Macro definitions used:     Macro                            Value
                            SM_RESET_PENDING                  0
                            COOPERATIVE_RESET_MAX_TIMEOUT     6000


References          :

Derived Requirements:
Algorithm           :1.Load the reset button and button2 300s timer values
                     2.If the local reset flag of both the units is not set, start polling of
                     reset button by making the system wait for the reset button to
                     be pressed
************************************************************************/
void Start_CF_Reset_Monitor(void)
{
    Reset_Button_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
    Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
    if (Status.Flags.Local_Reset_Done == SM_RESET_PENDING ||
        Status.Flags.Local_Reset_Done2 == SM_RESET_PENDING)
    {
        Reset_Button_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;   /* Start reset button polling */
        Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;  /* Start reset button polling */
    }
 }
/*********************************************************************
Component name      :RESET
Module Name         :void Start_EF_Reset_Monitor(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24


Abstract            :Start reset button polling for EF

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Start_EF_Reset_Sequence()

Input Variables         Name                                Type
    Global          :   Status.Flags.Local_Reset_Done       Bit
                        WAIT_FOR_RESET_BUTTON_PRESS         Enumerator

    Local           :   None

Output Variables        Name                                Type
    Global          :   Reset_Button_Info.State             Enumerator
                        Reset_Button_Info.Timeout_300s      UINT16
                        Reset_Button2_Info.Timeout_300s     UINT16

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            SM_RESET_PENDING                0
                            COOPERATIVE_RESET_MAX_TIMEOUT   6000


References          :
Derived Requirements:
Algorithm           :1.Load the reset button and button2 300s timer values
                     2.If the local reset flag is not set, start polling of
                     reset button by making the system wait for the reset button to
                     be pressed
************************************************************************/
void Start_EF_Reset_Monitor(void)
{
    Reset_Button_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
    if (Status.Flags.Local_Reset_Done == SM_RESET_PENDING)
    {
        Reset_Button_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;   /* Start reset button polling */
    }
}


/*********************************************************************
Component name      :RESET
Module Name         :void Update_SF_Reset_Monitor_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Update_Reset_Button_State, SF_Wait_for_Local_Reset and update the checksum


Design Requirements     :


Interfaces
    Calls           :   
                        RESET.c -   Update_Reset_Button_State()
                        RESET.c -   SF_Wait_for_Local_Reset()
                        RESET.c -   SF_Reset_Sequence_Complete()
                        RESET.c -   Clear_Reset_Info()

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_SF()

Input Variables         Name                                        Type
    Global          :   Reset_Info.SF.State                         Enumerator
                        SF_WAIT_FOR_RESET                           Enumerator
                        SF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        Reset_Info.SF.Flags.DS1_has_been_Reset      Bit
                        Reset_Info.SF.Flags.DS2_has_been_Reset      Bit
                        SF_WAIT_FOR_PEER_TO_RESET                   Enumerator
                        SF_WAIT_FOR_DS2_TO_RESET                    Enumerator
                        SF_WAIT_FOR_DS1_TO_RESET                    Enumerator
                        SF_RESET_SEQUENCE_COMPLETED                 Enumerator
                        WAITING_FOR_RESET_MODE                      Enumerator
                        Reset_Info.SF.Flags.Peer_CPU_has_been_Reset Bit
                        Reset_Button_Info.Timeout_300s              UINT16
                        Reset_Button2_Info.Timeout_300s             UINT16

    Local           :   None

Output Variables        Name                                            Type
    Global          :   Reset_Info.SF.State                             Enumerator
                        Status.Flags.Local_Reset_Done                   Bit
                        Status.Flags.Local_Reset_Done2                  Bit
                        DS_Section_Mode.Local_Unit                      BYTE
                        DS_Section_Mode.Flags.Remote_Reset_Applied      Bit
                        DS_Section_Mode.Flags.Remote_Preparatory_State  Bit
                        Reset_Button_Info.Timeout_300s                  UINT16
                        Reset_Button2_Info.Timeout_300s                 UINT16


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            SM_HAS_RESETTED_SYSTEM          1
                            TIMEOUT_EVENT                   0
                            COOPERATIVE_RESET_MAX_TIMEOUT   6000
                            FALSE                           0
                            TRUE                            1

References          :
Derived Requirements:
Algorithm           :1.Update the reset button state, to know the state of the reset button as to the
                       reset button is pressed or stuck or not pressed
                     2.Update the states of reset monitoring sequence in SF mode locally
                     3.If downstream 5min timeout has completed after the reset aplied at either local or remote unit,
                        clear the reset applied earlier,change the reset info state,update the downstream local unit status,
                        intialise the downstream remote reset and remote preparatory flags and load the 5min reset timer with values
                        then call persist_variable function inorder to update the change in the persistant variable.
                     4.If the SF system is still waiting for reset than just return
                     5.If SF system is waiting for DS to reset
                            If both down stream CPU1 & down stream CPU2 has to be reset then the system shall wait
                            for the peer to reset
                            If only down stream CPU1 as been reset than wait for down stream CPU2 to reset
                            If only down stream CPU2 as been reset than wait for down stream CPU1 to reset
                     6.If the SF state is waiting for down stream DS1 to reset, than reset the down stream DS1, wait
                       for peer CPU to reset 
                     7.If the SF state is waiting for down stream DS2 to reset, then reset the down stream DS2, wait
                       for peer CPU to reset
                     8.If the SF state is wait for the peer to reset then reset the peer CPU & with this reset sequence for
                       SF will be completed
************************************************************************/
void Update_LCWS_Reset_Monitor_State(void)
{
    Update_Reset_Button2_State();
    LCWS_Wait_for_Local_Reset();
     if(Reset_Button2_Info.Timeout_300s == TIMEOUT_EVENT)
        {
             Clear_Reset_Info();
             Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
             Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_RESET;
             DS_Section_Mode.Local_Unit = WAITING_FOR_RESET_MODE;
             DS_Section_Mode.Flags.Remote_Reset_Applied = FALSE;
             DS_Section_Mode.Flags.Remote_Preparatory_State = TRUE;
             Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
             return;
          }

    switch (Reset_Info.SF.DS_State)
    {
        case SF_WAIT_FOR_RESET:
        case SF_WAIT_FOR_DS_TO_RESET:
        case SF_WAIT_FOR_DS1_TO_RESET:
        case SF_WAIT_FOR_DS2_TO_RESET:
            break;
//        case SF_WAIT_FOR_DS_TO_RESET:
//            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM &&
//                Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
//                
//                break;
//            }
//            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_DS2_TO_RESET;
//                
//                break;
//            }
//            if (Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_DS1_TO_RESET;
//                
//            }
//            break;
//        case SF_WAIT_FOR_DS1_TO_RESET:
//            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
//                
//            }
//            break;
//        case SF_WAIT_FOR_DS2_TO_RESET:
//            if (Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
//                
//            }
//            break;
        case SF_WAIT_FOR_PEER_TO_RESET:
            if (Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 == SM_HAS_RESETTED_SYSTEM)
            {
                SF_Reset_Sequence_Complete();
            }
                break;
        case SF_RESET_SEQUENCE_COMPLETED:
                break;
    }
}


void Update_DE_Reset_Monitor_State(void)
{
    Update_Reset_Button2_State();
    DE_Wait_for_Local_Reset();
     if(Reset_Button2_Info.Timeout_300s == TIMEOUT_EVENT)
        {
             Clear_Reset_Info();
             Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
             Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_RESET;
             DS_Section_Mode.Local_Unit = WAITING_FOR_RESET_MODE;
             DS_Section_Mode.Flags.Remote_Reset_Applied = FALSE;
             DS_Section_Mode.Flags.Remote_Preparatory_State = TRUE;
             Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
             
             return;
          }

    switch (Reset_Info.SF.DS_State)
    {
        case SF_WAIT_FOR_RESET:
        case SF_WAIT_FOR_DS_TO_RESET:
        case SF_WAIT_FOR_DS1_TO_RESET:
        case SF_WAIT_FOR_DS2_TO_RESET:
            break;
//        case SF_WAIT_FOR_DS_TO_RESET:
//            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM &&
//                Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
//                
//                break;
//            }
//            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_DS2_TO_RESET;
//                
//                break;
//            }
//            if (Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_DS1_TO_RESET;
//                
//            }
//            break;
//        case SF_WAIT_FOR_DS1_TO_RESET:
//            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
//                
//            }
//            break;
//        case SF_WAIT_FOR_DS2_TO_RESET:
//            if (Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
//            {
//                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
//                
//            }
//            break;
        case SF_WAIT_FOR_PEER_TO_RESET:
            if (Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 == SM_HAS_RESETTED_SYSTEM)
            {
                SF_Reset_Sequence_Complete();
            }
                break;
        case SF_RESET_SEQUENCE_COMPLETED:
                break;
    }
}

/*********************************************************************
Component name      :RESET
Module Name         :void Update_SF_Reset_Monitor_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Update_Reset_Button_State, SF_Wait_for_Local_Reset and update the checksum


Design Requirements     :


Interfaces
    Calls           :   
                        RESET.c -   Update_Reset_Button_State()
                        RESET.c -   SF_Wait_for_Local_Reset()
                        RESET.c -   SF_Reset_Sequence_Complete()
                        RESET.c -   Clear_Reset_Info()

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_SF()

Input Variables         Name                                        Type
    Global          :   Reset_Info.SF.State                         Enumerator
                        SF_WAIT_FOR_RESET                           Enumerator
                        SF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        Reset_Info.SF.Flags.DS1_has_been_Reset      Bit
                        Reset_Info.SF.Flags.DS2_has_been_Reset      Bit
                        SF_WAIT_FOR_PEER_TO_RESET                   Enumerator
                        SF_WAIT_FOR_DS2_TO_RESET                    Enumerator
                        SF_WAIT_FOR_DS1_TO_RESET                    Enumerator
                        SF_RESET_SEQUENCE_COMPLETED                 Enumerator
                        WAITING_FOR_RESET_MODE                      Enumerator
                        Reset_Info.SF.Flags.Peer_CPU_has_been_Reset Bit
                        Reset_Button_Info.Timeout_300s              UINT16
                        Reset_Button2_Info.Timeout_300s             UINT16

    Local           :   None

Output Variables        Name                                            Type
    Global          :   Reset_Info.SF.State                             Enumerator
                        Status.Flags.Local_Reset_Done                   Bit
                        Status.Flags.Local_Reset_Done2                  Bit
                        DS_Section_Mode.Local_Unit                      BYTE
                        DS_Section_Mode.Flags.Remote_Reset_Applied      Bit
                        DS_Section_Mode.Flags.Remote_Preparatory_State  Bit
                        Reset_Button_Info.Timeout_300s                  UINT16
                        Reset_Button2_Info.Timeout_300s                 UINT16


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            SM_HAS_RESETTED_SYSTEM          1
                            TIMEOUT_EVENT                   0
                            COOPERATIVE_RESET_MAX_TIMEOUT   6000
                            FALSE                           0
                            TRUE                            1

References          :
Derived Requirements:
Algorithm           :1.Update the reset button state, to know the state of the reset button as to the
                       reset button is pressed or stuck or not pressed
                     2.Update the states of reset monitoring sequence in SF mode locally
                     3.If downstream 5min timeout has completed after the reset aplied at either local or remote unit,
                        clear the reset applied earlier,change the reset info state,update the downstream local unit status,
                        intialise the downstream remote reset and remote preparatory flags and load the 5min reset timer with values
                        then call persist_variable function inorder to update the change in the persistant variable.
                     4.If the SF system is still waiting for reset than just return
                     5.If SF system is waiting for DS to reset
                            If both down stream CPU1 & down stream CPU2 has to be reset then the system shall wait
                            for the peer to reset
                            If only down stream CPU1 as been reset than wait for down stream CPU2 to reset
                            If only down stream CPU2 as been reset than wait for down stream CPU1 to reset
                     6.If the SF state is waiting for down stream DS1 to reset, than reset the down stream DS1, wait
                       for peer CPU to reset 
                     7.If the SF state is waiting for down stream DS2 to reset, then reset the down stream DS2, wait
                       for peer CPU to reset 
                     8.If the SF state is wait for the peer to reset then reset the peer CPU & with this reset sequence for
                       SF will be completed
************************************************************************/
void Update_SF_Reset_Monitor_State(void)
{
    Update_Reset_Button2_State();
    SF_Wait_for_Local_Reset();
     if(Reset_Button2_Info.Timeout_300s == TIMEOUT_EVENT)
        {
             Clear_Reset_Info();
             Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
             Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_RESET;
             DS_Section_Mode.Local_Unit = WAITING_FOR_RESET_MODE;
             DS_Section_Mode.Flags.Remote_Reset_Applied = FALSE;
             DS_Section_Mode.Flags.Remote_Preparatory_State = TRUE;
             Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
             
             return;
          }

    switch (Reset_Info.SF.DS_State)
    {
        case SF_WAIT_FOR_RESET:
            break;
        case SF_WAIT_FOR_DS_TO_RESET:
            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM &&
                Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
                
                break;
            }
            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_DS2_TO_RESET;
                
                break;
            }
            if (Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_DS1_TO_RESET;
                
            }
            break;
        case SF_WAIT_FOR_DS1_TO_RESET:
            if (Reset_Info.SF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
                
            }
            break;
        case SF_WAIT_FOR_DS2_TO_RESET:
            if (Reset_Info.SF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
                
            }
            break;
        case SF_WAIT_FOR_PEER_TO_RESET:
            if (Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 == SM_HAS_RESETTED_SYSTEM)
            {
                SF_Reset_Sequence_Complete();
            }
                break;
        case SF_RESET_SEQUENCE_COMPLETED:
                break;
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :void SF_Reset_Sequence_Complete(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Indicate that the reset sequence is completed for SF

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   

    Called by       :   RESET.c -   Update_SF_Reset_Monitor_State()

Input Variables         Name                            Type
    Global          :   SF_RESET_SEQUENCE_COMPLETED     Enumerator

    Local           :   None

Output Variables        Name                            Type
    Global          :   Reset_Info.SF.State             Enumerator

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :
Derived Requirements:
************************************************************************/
void SF_Reset_Sequence_Complete(void)
{
 Reset_Info.SF.DS_State = (BYTE) SF_RESET_SEQUENCE_COMPLETED;
         
}


/*********************************************************************
Component name      :RESET
Module Name         :void SF_Wait_for_Local_Reset(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --



Abstract            :Accept preparatory reset, update the Check sum,Reset the system


Design Requirements     :
Interfaces
    Calls           :   RLYB_MGR.c          -   Reset_Allowed_For_DS()
                        
                        RESET()             - compiler defined function

    Called by       :   RESET.c -   Update_SF_Reset_Monitor_State()

Input Variables         Name                                    Type
    Global          :   Reset_Button_Info.State                 Enumerator
                        RESET_BUTTON_PRESS_ACCEPTED             Enumerator
                        WAIT_FOR_RESET_BUTTON_PRESS             Enumerator
                        SF_WAIT_FOR_DS_TO_RESET                 Enumerator
                        SF_WAIT_FOR_RESET                       Enumerator
                        RESET_APPLIED_MODE                      Enumerator

    Local           :   None

Output Variables        Name                                    Type
    Global          :   Reset_Info.SF.State                     Enumerator
                        Reset_Info.SF.Flags.DS1_has_been_Reset  Bit
                        Reset_Info.SF.Flags.DS2_has_been_Reset  Bit
                        Status.Flags.Local_Reset_Done           Bit
                        DS_Section_Mode.Local_Unit              BYTE

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                           0
                            SM_HAS_RESETTED_SYSTEM          1

References          :
Derived Requirements:

Algorithm           :For SF system if reset button for the local system is pressed, update the reset state for two conditions
                        1.During run time, check for DS state, update both down stream DS1 & DS2 state & reset the system
                        2.During start up local system has already been reset hence check for DS to reset
************************************************************************/
void LCWS_Wait_for_Local_Reset(void)
{
    if (Reset_Button2_Info.State == RESET_BUTTON_PRESS_ACCEPTED)
        {
           Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;      /* Start reset button polling */


           if (Reset_Allowed_For_LCWS())
                {
                /* Preparatory reset at Runtime */
                  Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
                  // Reset_Info.SF.Flags.DS1_has_been_Reset = FALSE;
                  // Reset_Info.SF.Flags.DS2_has_been_Reset = FALSE;
                  Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
                         
                  {__asm__ volatile ("reset");} // RESET();             /* Reset the System */
                  return;
                 }
             if(Reset_Info.SF.DS_State == SF_WAIT_FOR_RESET)
                {
                    /* Preparatory reset at start-up */
                  Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
                  Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
                  DS_Section_Mode.Local_Unit = (BYTE)RESET_APPLIED_MODE;
                         
                 }
            }
}



void DE_Wait_for_Local_Reset(void)
{
    if (Reset_Button2_Info.State == RESET_BUTTON_PRESS_ACCEPTED)
        {
           Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;      /* Start reset button polling */


           if (Reset_Allowed_For_DE())
                {
                /* Preparatory reset at Runtime */
                  Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
                  // Reset_Info.SF.Flags.DS1_has_been_Reset = FALSE;
                  // Reset_Info.SF.Flags.DS2_has_been_Reset = FALSE;
                  Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
                         
                  {__asm__ volatile ("reset");} // RESET();             /* Reset the System */
                  return;
                 }
             if(Reset_Info.SF.DS_State == SF_WAIT_FOR_RESET)
                {
                    /* Preparatory reset at start-up */
                  Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
                  Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_PEER_TO_RESET;
                  DS_Section_Mode.Local_Unit = (BYTE)RESET_APPLIED_MODE;
                         
                 }
            }
}
/*********************************************************************
Component name      :RESET
Module Name         :void SF_Wait_for_Local_Reset(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --



Abstract            :Accept preparatory reset, update the Check sum,Reset the system


Design Requirements     :
Interfaces
    Calls           :   RLYB_MGR.c          -   Reset_Allowed_For_DS()
                        
                        RESET()             - compiler defined function

    Called by       :   RESET.c -   Update_SF_Reset_Monitor_State()

Input Variables         Name                                    Type
    Global          :   Reset_Button_Info.State                 Enumerator
                        RESET_BUTTON_PRESS_ACCEPTED             Enumerator
                        WAIT_FOR_RESET_BUTTON_PRESS             Enumerator
                        SF_WAIT_FOR_DS_TO_RESET                 Enumerator
                        SF_WAIT_FOR_RESET                       Enumerator
                        RESET_APPLIED_MODE                      Enumerator

    Local           :   None

Output Variables        Name                                    Type
    Global          :   Reset_Info.SF.State                     Enumerator
                        Reset_Info.SF.Flags.DS1_has_been_Reset  Bit
                        Reset_Info.SF.Flags.DS2_has_been_Reset  Bit
                        Status.Flags.Local_Reset_Done           Bit
                        DS_Section_Mode.Local_Unit              BYTE

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                           0
                            SM_HAS_RESETTED_SYSTEM          1

References          :
Derived Requirements:

Algorithm           :For SF system if reset button for the local system is pressed, update the reset state for two conditions
                        1.During run time, check for DS state, update both down stream DS1 & DS2 state & reset the system
                        2.During start up local system has already been reset hence check for DS to reset
************************************************************************/
void SF_Wait_for_Local_Reset(void)
{
    if (Reset_Button2_Info.State == RESET_BUTTON_PRESS_ACCEPTED)
        {
           Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;      /* Start reset button polling */


           if (Reset_Allowed_For_DS())
                {
                /* Preparatory reset at Runtime */
                  Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_DS_TO_RESET;
                  Reset_Info.SF.Flags.DS1_has_been_Reset = FALSE;
                  Reset_Info.SF.Flags.DS2_has_been_Reset = FALSE;
                  Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
                         
                  {__asm__ volatile ("reset");} // RESET();             /* Reset the System */
                  return;
                 }
             if(Reset_Info.SF.DS_State == SF_WAIT_FOR_RESET)
                {
                    /* Preparatory reset at start-up */
                  Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
                  Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_DS_TO_RESET;
                  DS_Section_Mode.Local_Unit = (BYTE)RESET_APPLIED_MODE;
                         
                 }
            }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Update_CF_Reset_Monitor_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24

                            2           01              67              10-12-05        Refer ATR No.67
                            3           4               80              28-12-05        Refer ATR 80

Abstract            :Update_Reset_Button_State,CF_Wait_for_Local_Reset,Update the Checksum



Design Requirements     :

Interfaces
    Calls           :   RESET.c -   Update_Reset_Button_State()
                        RESET.c -   CF_Wait_for_Local_Reset()
                        RESET.c -   Process_CF_Reset_Command()
                        RESET.c -   CF_DS_Reset_Sequence_Complete()
                        
                        RESET.c -   CF_US_Reset_Sequence_Complete()
                        RESET.c -   CF_Reset_Sequence_Complete()

    Called by       :   DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables         Name                                            Type
    Global          :   DIP_Switch_Info.Flags.Remote_Reset_for_IBS      Bit
                        Reset_Info.CF.Flags.US1_has_been_Reset          Bit
                        Reset_Info.CF.Flags.US2_has_been_Reset          Bit
                        Reset_Info.CF.Flags.DS1_has_been_Reset          Bit
                        Reset_Info.CF.Flags.DS2_has_been_Reset          Bit
                        Reset_Info.CF.State                             Enumerator
                        CF_WAIT_FOR_RESET                               Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                         Enumerator
                        CF_WAIT_FOR_DS2_TO_RESET                        Enumerator
                        CF_WAIT_FOR_DS1_TO_RESET                        Enumerator
                        CF_WAIT_FOR_US_TO_RESET                         Enumerator
                        CF_WAIT_FOR_US2_TO_RESET                        Enumerator
                        CF_WAIT_FOR_US1_TO_RESET                        Enumerator
                        WAITING_FOR_RESET_MODE                          Enumerator
                        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset     Bit
                        Reset_Button_Info.Timeout_300s                  UINT16
                        Reset_Button2_Info.Timeout_300s                 UINT16


    Local           :   None

Output Variables        Name                                            Type
    Global          :   Reset_Info.CF.State                             Enumerator
                        Reset_Info.CF.Flags.DS1_has_been_Reset          Bit
                        Reset_Info.CF.Flags.DS2_has_been_Reset          Bit
                        Status.Flags.Local_Reset_Done                   Bit
                        Reset_Info.CF.Flags.US1_has_been_Reset          Bit
                        Reset_Info.CF.Flags.US2_has_been_Reset          Bit
                        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset     Bit
                        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1    Bit
                        Reset_Button_Info.Timeout_300s                  UINT16
                        Reset_Button2_Info.Timeout_300s                 UINT16


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            TRUE                            1
                            FALSE                           0
                            SM_HAS_RESETTED_SYSTEM          1
                            TIMEOUT_EVENT                   0
                            SM_RESET_PENDING                0
                            COOPERATIVE_RESET_MAX_TIMEOUT   6000

References          :
Derived Requirements:
Algorithm           :1.In CF there will be  two reset box, so Update the reset button state of both the two buttons,
                      to know the state of the reset button as to the reset button is pressed or stuck or not pressed
                     2.Update the states of reset monitoring sequence in CF mode locally
                     3.If downstream 5min timeout has completed after the reset aplied at either local or remote unit,
                        clear the reset applied earlier,change the reset info state,update the downstream local unit status,
                        intialise the downstream remote reset and remote preparatory flags and load the 5min reset timer with values
                        then call persist_variable function inorder to update the change in the persistant variable.
                        If upstream 5min timeout has completed after the reset aplied at either local or remote unit,
                        clear the reset applied earlier,change the reset info state,update the upstream local unit status,
                        intialise the upstream remote reset and remote preparatory flags and load the 5min reset timer with values
                        then call persist_variable function inorder to update the change in the persistant variable.
                     4.If the system is configured to IBS configuration reset at CF can be done by applying
                       reset to both the remote units hence down stream DS1, down stream DS2, up stream US1 & up stream
                       US2 has to be reset than only reset process for CF can be started
                     5.If CF system is waiting for DS to reset
                            If both down stream CPU1 & down stream CPU2 has to be reset then the DS reset sequence
                            is completed
                            If only down stream CPU1 as been reset then wait for down stream CPU2 to reset
                            If only down stream CPU2 as been reset then wait for down stream CPU1 to reset
                     6.If CF system is waiting for US to reset
                            If both up stream CPU1 & up stream CPU2 has to be reset then the US reset sequence
                            is completed
                            If only up stream CPU1 as been reset than wait for up stream CPU2 to reset
                            If only up stream CPU2 as been reset then wait for up stream CPU1 to reset
                     7.If the CF state is wait to down stream DS1 to reset then reset the down stream DS1 with this CF down
                       stream sequence is completed
                     8.If the CF state is wait to down stream DS2 to reset then reset the down stream DS2 with this CF down
                       stream sequence is completed
                     9.If the CF state is wait to up stream DS1 to reset then reset the down stream DS1 with this CF down
                       stream sequence is completed
                     10.If the CF state is wait to up stream DS2 to reset then reset the down stream DS2 with this CF down
                       stream sequence is completed
                     11.If the CF state is wait for the peer to reset then reset the peer CPU & with this reset sequence for
                       CF will be completed
************************************************************************/
void Update_CF_Reset_Monitor_State(void)
{
    Update_Reset_Button_State();
    Update_Reset_Button2_State();
    CF_Wait_for_Local_Reset();
    if(Reset_Button_Info.Timeout_300s == TIMEOUT_EVENT)
        {
             Status.Flags.Local_Reset_Done  = SM_RESET_PENDING;
             Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
             Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
             Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
             Reset_Info.CF.DS_State = (BYTE) CF_DS_WAIT_FOR_RESET;
             DS_Section_Mode.Flags.Remote_Reset_Applied = FALSE;
             DS_Section_Mode.Flags.Remote_Preparatory_State = TRUE;
             DS_Section_Mode.Local_Unit = WAITING_FOR_RESET_MODE;
             Reset_Button_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
             
          }
    if(Reset_Button2_Info.Timeout_300s == TIMEOUT_EVENT)
        {
             Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
             Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
             Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
             Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
             Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_RESET;
             US_Section_Mode.Flags.Remote_Reset_Applied = FALSE;
             US_Section_Mode.Flags.Remote_Preparatory_State = TRUE;
             US_Section_Mode.Local_Unit = WAITING_FOR_RESET_MODE;
             Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
             
           }
    switch (Reset_Info.CF.DS_State)
    {
        case CF_DS_WAIT_FOR_RESET:
            break;
        case CF_WAIT_FOR_DS_TO_RESET:
            if (Reset_Info.CF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM &&
                Reset_Info.CF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                CF_DS_Reset_Sequence_Complete();
                break;
            }
            if (Reset_Info.CF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.DS_State = (BYTE) CF_WAIT_FOR_DS2_TO_RESET;
                
                break;
            }
            if (Reset_Info.CF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.DS_State = (BYTE) CF_WAIT_FOR_DS1_TO_RESET;
                
            }
            break;
        case CF_WAIT_FOR_DS1_TO_RESET:
            if (Reset_Info.CF.Flags.DS1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.DS_State = (BYTE) CF_DS_WAIT_FOR_PEER_TO_RESET;
                
            }
            break;
        case CF_WAIT_FOR_DS2_TO_RESET:
            if (Reset_Info.CF.Flags.DS2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.DS_State = (BYTE) CF_DS_WAIT_FOR_PEER_TO_RESET;
                
            }
            break;
        case CF_DS_WAIT_FOR_PEER_TO_RESET:
            if (Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1 == SM_HAS_RESETTED_SYSTEM )
            {
                CF_DS_Reset_Sequence_Complete();
            }
             break;
        case CF_DS_RESET_SEQUENCE_COMPLETED:
            break;
      }

    switch(Reset_Info.CF.US_State)
      {
        case CF_US_WAIT_FOR_RESET:
            break;
        case CF_WAIT_FOR_US_TO_RESET:
            if (Reset_Info.CF.Flags.US1_has_been_Reset == SM_HAS_RESETTED_SYSTEM &&
                Reset_Info.CF.Flags.US2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_PEER_TO_RESET;
                
                break;
            }
            if (Reset_Info.CF.Flags.US1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.US_State = (BYTE) CF_WAIT_FOR_US2_TO_RESET;
                
                break;
            }
            if (Reset_Info.CF.Flags.US2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.US_State = (BYTE) CF_WAIT_FOR_US1_TO_RESET;
                
            }
            break;
        case CF_WAIT_FOR_US1_TO_RESET:
            if (Reset_Info.CF.Flags.US1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_PEER_TO_RESET;
                
                break;
            }
            break;
        case CF_WAIT_FOR_US2_TO_RESET:
            if (Reset_Info.CF.Flags.US2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_PEER_TO_RESET;
                
                break;
            }
            break;
        case CF_US_WAIT_FOR_PEER_TO_RESET:
            if(Reset_Info.CF.Flags.Peer_CPU_has_been_Reset == SM_HAS_RESETTED_SYSTEM )
            {
                CF_US_Reset_Sequence_Complete();
            }
             break;
        case CF_US_RESET_SEQUENCE_COMPLETED:
            break;
     }
}
/*********************************************************************
Component name      :RESET
Module Name         :void CF_DS_Reset_Sequence_Complete(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24

                            3           4               80              28-12-05        Refer ATR 80

Abstract            :Once the CF down stream sequence is completed than check for
                     CF up stream to be completed, call the function persist validate()
                     as there will be change in persistant variable

Allocated Requirements  :

Design Requirements     :
Interfaces
    Calls           :   
                        RESTORE.c   -   Clear_DS_Checksum_Info()

    Called by       :   RESET.c -   Update_CF_Reset_Monitor_State()

Input Variables         Name                            Type
    Global          :   CF_WAIT_FOR_US_TO_RESET         Enumerator
                        WAIT_FOR_PILOT_TRAIN_MODE       Enumerator

    Local           :   None

Output Variables        Name                            Type
    Global          :   Reset_Info.CF.State             Enumerator
                        DS_Section_Mode.Local_Unit      BYTE
                        Reset_Button_Info.Timeout_300s  UINT16

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            COOPERATIVE_RESET_MAX_TIMEOUT   6000

References          :
Derived Requirements:
************************************************************************/
void CF_DS_Reset_Sequence_Complete(void)
{
  Reset_Info.CF.DS_State = (BYTE) CF_DS_RESET_SEQUENCE_COMPLETED;
  Reset_Button2_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
   if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_CF ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_SF ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_EF)
     {
       Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_COMPLETED;
       Start_Relay_B_Mgr();         /* from rlyb_mgr.c */
       Start_DS_Axle_Counting();    /* Start Axle Counting */
       DS_Section_Mode.Local_Unit = WAIT_FOR_PILOT_TRAIN_MODE;
     }
   Clear_DS_Checksum_Info();
             
}
/*********************************************************************
Component name      :RESET
Module Name         :void CF_US_Reset_Sequence_Complete(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24

                            2       13002               64              21-11-05        Refer ATR 64
                            3           4               80              28-12-05        Refer ATR 80


Abstract            :Update the Center fed reset state and update the Checksum

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   
                        RESTORE.c   -   Clear_US_Checksum_Info()

    Called by       :   RESET.c -   Update_CF_Reset_Monitor_State()

Input Variables         Name                            Type
    Global          :   CF_WAIT_FOR_PEER_TO_RESET       Enumerator
                        WAIT_FOR_PILOT_TRAIN_MODE       Enumerator

    Local           :   None

Output Variables        Name                            Type
    Global          :   Reset_Info.CF.State             Enumerator
                        Reset_Button2_Info.Timeout_300s UINT16
                        US_Section_Mode.Local_Unit      BYTE

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            COOPERATIVE_RESET_MAX_TIMEOUT   6000

References          :
Derived Requirements:
Algorithm           :Once the CF up stream sequence is completed than check for
                     peer CPU to reset, call the function persist validate() as
                     there is change in persistant variable
************************************************************************/
void CF_US_Reset_Sequence_Complete(void)
{
    Reset_Info.CF.US_State = (BYTE) CF_US_RESET_SEQUENCE_COMPLETED;
    Reset_Button_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
    if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_CF ||
       DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_SF ||
       DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_EF)
     {
       Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_COMPLETED;
       Start_Relay_A_Mgr();         /* from rlya_mgr.c */
       Start_US_Axle_Counting();    /* Start Axle Counting */
       US_Section_Mode.Local_Unit = WAIT_FOR_PILOT_TRAIN_MODE;
     }
    Clear_US_Checksum_Info();
             
}

/*********************************************************************
Component name      :RESET
Module Name         :void CF_Wait_for_Local_Reset(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24
                            2           2-5             5               7-12-05         Refer ATR 5
                            2           01              67              10-12-05        Refer ATR No.67
                            3           4               80              28-12-05        Refer ATR 80

Abstract            :For CF configuration when the local system is been reset system waits for the
                     remote unit to reset with this it supports for co-operative reset, hence update the
                     remote unit reset also (both DS & US)

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RESET.c -   Process_CF_Reset_Command()

    Called by       :   RESET.c -   Update_CF_Reset_Monitor_State()

Input Variables         Name                            Type
    Global          :   Reset_Button_Info.State         Enumerator
                        Reset_Button2_Info.State        Enumerator
                        RESET_BUTTON_PRESS_ACCEPTED     Enumerator
                        WAIT_FOR_RESET_BUTTON_PRESS     Enumerator

    Local           :   None

Output Variables        Name                            Type
    Global          :   Reset_Button_Info.State         Enumerator
                        Reset_Button2_Info.State        Enumerator

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:
************************************************************************/
void CF_Wait_for_Local_Reset(void)
{
    if(Reset_Button2_Info.State  == RESET_BUTTON_PRESS_ACCEPTED)
      {
        if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A ||
           DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B ||
           DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
        {
         Process_D3_Reset_Command();
        }
        else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A ||
           DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B ||
           DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C ||
           DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
        {
         Process_D4_Reset_Command();
        }
        else
        {
         Process_CF_DS_Reset_Command();
        }
        Reset_Button2_Info.State  = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS; /* Start reset button polling */
      }
    if(Reset_Button_Info.State == RESET_BUTTON_PRESS_ACCEPTED)
    {
      if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_CF ||
         DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_SF ||
         DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_EF)
       {
        Process_CF_US_Reset_Command();
        Reset_Button_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;   /* Start reset button polling */
        }
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Process_D3_Reset_Command(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            3           4               80              28-12-05        Refer ATR 80


Abstract            :Get the status of both relay A and B,If defective,reset the system and update the checksum

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RLYD3_MGR.c -   Reset_Allowed_For_D3()
                        
                        RESET()             - compiler defined function
                        RLYA_MGR.c  -   Get_Relay_A_State()
                        RLYB_MGR.c  -   Get_Relay_B_State()
                        RLYB_MGR.c  -   Reset_Allowed_For_DS()
                        RLYA_MGR.c  -   Reset_Allowed_For_US()
                        RLYB_MGR.c  -   Set_Relay_B_Reset_State()
                        RLYA_MGR.c  -   Set_Relay_A_Reset_State()

    Called by       :   RESET.c -   Update_CF_Reset_Monitor_State()
                        RESET.c -   CF_Wait_for_Local_Reset()

Input Variables         Name                                        Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                   DAC_Unit_Type
                        DAC_UNIT_TYPE_CF                            Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        CF_WAIT_FOR_RESET                           Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        DAC_DEFECTIVE                               Enumerator
                        RESET_APPLIED_MODE                          Enumerator
                        DP3_MODE                                    Enumerator

    Local           :   uch_A_State                                 BYTE
                        uch_B_State                                 BYTE

Output Variables        Name                                        Type
    Global          :   Reset_Info.CF.Flags.DS1_has_been_Reset      Bit
                        Reset_Info.CF.Flags.DS2_has_been_Reset      Bit
                        Reset_Info.CF.Flags.US1_has_been_Reset      Bit
                        Reset_Info.CF.Flags.US2_has_been_Reset      Bit
                        Reset_Info.CF.State                         Enumerator
                        Status.Flags.Local_Reset_Done               Bit
                        DS_Section_Mode.Local_Unit                  BYTE
                        DS_Section_Mode.Remote_Unit                 BYTE


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                           0
                            SM_HAS_RESETTED_SYSTEM          1


References          :

Derived Requirements:

Algorithm           :1. If the system is configured for 3D configuration & reset is accepted,
                        if the CF system is waiting for reset then during start up time reset
                        the system & wait for DS to reset, call the function persist validate
                        to update the checksum.
                     2. If the reset is allowed for 3D then clear the down stream DS1, down stream
                        DS2, up stream US1 & up stream US2 reset flags and wait for DS to reset and
                        call the function persist validate() to update the checksum.
                     3. Check for the reset button state, if it is accepted, set the local reset done
                        flag call the function persist validate() to update the checksum.
                     4. Check for the reset button2 state, if it is accepted, set the local reset2 done
                        flag call the function persist validate() to update the checksum.
                     5. If the CF system is waiting for reset then during start up time reset
                        the system & wait for DS to reset, call the function persist validate
                        to update the checksum.
                     6. During run time get both the vital relays(A & B) state to know the state of the
                        system at remote units
                     7. If both the remote units at US & DS are defective then reset is accepted hence reset
                        down stream DS1, down stream DS2, up stream US1 & up stream US2,local reset1 and local
                        reset2 flag and call the persist validate function to update the check sum & reset
                        the CF  system
                     8. If reset is allowed for both DS & US then reset is accepted hence reset
                        down stream DS1, down stream DS2, up stream US1 & up stream US2, local reset1 and local
                        reset2 flag, call the persist validate function to update the check sum & reset the CF system
                     9. If local reset1 flag and the reset is allowed for DS, then reset only downstream
                         DS1 & down stream DS2 else set local reset flag as waiting for reset and clear the
                        local reset1 flag and call the persist validate function to update the check sum
                    10. If local reset2 flag and the reset is allowed for US, then reset only upstream
                        US1 & up stream US2 else set local reset flag as waiting for reset and clear the
                        local reset2 flag and call the persist validate function to update the check sum
************************************************************************/

void Process_D3_Reset_Command(void)
{
  /* 3D Configuration */
   if(Reset_Info.CF.US_State == CF_US_WAIT_FOR_RESET &&
      Reset_Info.CF.DS_State == CF_DS_WAIT_FOR_RESET )
     {
        Status.Flags.Local_Reset_Done = SM_HAS_RESETTED_SYSTEM;
        Status.Flags.Local_Reset_Done2= SM_HAS_RESETTED_SYSTEM;
        Reset_Info.CF.DS_State = (BYTE) CF_WAIT_FOR_DS_TO_RESET;
        Reset_Info.CF.US_State = (BYTE) CF_WAIT_FOR_US_TO_RESET;
        
        DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
        DS_Section_Mode.Remote_Unit = DP3_MODE;
        return;
     }
   if(Reset_Allowed_For_D3())
     {
        Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
        Reset_Info.CF.DS_State = (BYTE)CF_WAIT_FOR_DS_TO_RESET;
        Reset_Info.CF.US_State = (BYTE)CF_WAIT_FOR_US_TO_RESET;
              
        {__asm__ volatile ("reset");} // RESET();               /* Reset the System */
     }
}

/*********************************************************************
Component name      :RESET
Module Name         :void Process_D4_Reset_Command(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            3           4               80              28-12-05        Refer ATR 80


Abstract            :Get the status of both relay A and B,If defective,reset the system and update the checksum

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RLYD3_MGR.c -   Reset_Allowed_For_D3()
                        
                        RESET()             - compiler defined function
                        RLYA_MGR.c  -   Get_Relay_A_State()
                        RLYB_MGR.c  -   Get_Relay_B_State()
                        RLYB_MGR.c  -   Reset_Allowed_For_DS()
                        RLYA_MGR.c  -   Reset_Allowed_For_US()
                        RLYB_MGR.c  -   Set_Relay_B_Reset_State()
                        RLYA_MGR.c  -   Set_Relay_A_Reset_State()

    Called by       :   RESET.c -   Update_CF_Reset_Monitor_State()
                        RESET.c -   CF_Wait_for_Local_Reset()

Input Variables         Name                                        Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                   DAC_Unit_Type
                        DAC_UNIT_TYPE_CF                            Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        CF_WAIT_FOR_RESET                           Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        DAC_DEFECTIVE                               Enumerator
                        RESET_APPLIED_MODE                          Enumerator
                        DP3_MODE                                    Enumerator

    Local           :   uch_A_State                                 BYTE
                        uch_B_State                                 BYTE

Output Variables        Name                                        Type
    Global          :   Reset_Info.CF.Flags.DS1_has_been_Reset      Bit
                        Reset_Info.CF.Flags.DS2_has_been_Reset      Bit
                        Reset_Info.CF.Flags.US1_has_been_Reset      Bit
                        Reset_Info.CF.Flags.US2_has_been_Reset      Bit
                        Reset_Info.CF.State                         Enumerator
                        Status.Flags.Local_Reset_Done               Bit
                        DS_Section_Mode.Local_Unit                  BYTE
                        DS_Section_Mode.Remote_Unit                 BYTE


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                           0
                            SM_HAS_RESETTED_SYSTEM          1


References          :

Derived Requirements:

Algorithm           :1. If the system is configured for 3D configuration & reset is accepted,
                        if the CF system is waiting for reset then during start up time reset
                        the system & wait for DS to reset, call the function persist validate
                        to update the checksum.
                     2. If the reset is allowed for 3D then clear the down stream DS1, down stream
                        DS2, up stream US1 & up stream US2 reset flags and wait for DS to reset and
                        call the function persist validate() to update the checksum.
                     3. Check for the reset button state, if it is accepted, set the local reset done
                        flag call the function persist validate() to update the checksum.
                     4. Check for the reset button2 state, if it is accepted, set the local reset2 done
                        flag call the function persist validate() to update the checksum.
                     5. If the CF system is waiting for reset then during start up time reset
                        the system & wait for DS to reset, call the function persist validate
                        to update the checksum.
                     6. During run time get both the vital relays(A & B) state to know the state of the
                        system at remote units
                     7. If both the remote units at US & DS are defective then reset is accepted hence reset
                        down stream DS1, down stream DS2, up stream US1 & up stream US2,local reset1 and local
                        reset2 flag and call the persist validate function to update the check sum & reset
                        the CF  system
                     8. If reset is allowed for both DS & US then reset is accepted hence reset
                        down stream DS1, down stream DS2, up stream US1 & up stream US2, local reset1 and local
                        reset2 flag, call the persist validate function to update the check sum & reset the CF system
                     9. If local reset1 flag and the reset is allowed for DS, then reset only downstream
                         DS1 & down stream DS2 else set local reset flag as waiting for reset and clear the
                        local reset1 flag and call the persist validate function to update the check sum
                    10. If local reset2 flag and the reset is allowed for US, then reset only upstream
                        US1 & up stream US2 else set local reset flag as waiting for reset and clear the
                        local reset2 flag and call the persist validate function to update the check sum
************************************************************************/

void Process_D4_Reset_Command(void)
{
  /* 3D Configuration */
   if(Reset_Info.CF.US_State == CF_US_WAIT_FOR_RESET &&
      Reset_Info.CF.DS_State == CF_DS_WAIT_FOR_RESET )
     {
        Status.Flags.Local_Reset_Done = SM_HAS_RESETTED_SYSTEM;
        Status.Flags.Local_Reset_Done2= SM_HAS_RESETTED_SYSTEM;
        Reset_Info.CF.DS_State = (BYTE) CF_WAIT_FOR_DS_TO_RESET;
        Reset_Info.CF.US_State = (BYTE) CF_WAIT_FOR_US_TO_RESET;
        
        DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
        DS_Section_Mode.Remote_Unit = DP3_MODE;
        return;
     }
   if(Reset_Allowed_For_D4())
     {
        Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
        Reset_Info.CF.DS_State = (BYTE)CF_WAIT_FOR_DS_TO_RESET;
        Reset_Info.CF.US_State = (BYTE)CF_WAIT_FOR_US_TO_RESET;
              
        {__asm__ volatile ("reset");} // RESET();               /* Reset the System */
     }
}

/*********************************************************************
Component name      :RESET
Module Name         :void Process_CF_DS_Reset_Command(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                        3           4               80              28-12-05        Refer ATR 80



Abstract            :Get the status of both relay A and B,If defective,reset the system and update the checksum

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RLYD3_MGR.c -   Reset_Allowed_For_D3()
                        
                        RESET()             - compiler defined function
                        RLYA_MGR.c  -   Get_Relay_A_State()
                        RLYB_MGR.c  -   Get_Relay_B_State()
                        RLYB_MGR.c  -   Reset_Allowed_For_DS()
                        RLYA_MGR.c  -   Reset_Allowed_For_US()
                        RLYB_MGR.c  -   Set_Relay_B_Reset_State()
                        RLYA_MGR.c  -   Set_Relay_A_Reset_State()

    Called by       :   RESET.c -   Update_CF_Reset_Monitor_State()
                        RESET.c -   CF_Wait_for_Local_Reset()

Input Variables         Name                                        Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                   DAC_Unit_Type
                        DAC_UNIT_TYPE_CF                            Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        CF_WAIT_FOR_RESET                           Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        DAC_DEFECTIVE                               Enumerator
                        RESET_APPLIED_MODE                          Enumerator

    Local           :   uch_A_State                                 BYTE
                        uch_B_State                                 BYTE

Output Variables        Name                                        Type
    Global          :   Reset_Info.CF.Flags.DS1_has_been_Reset      Bit
                        Reset_Info.CF.Flags.DS2_has_been_Reset      Bit
                        Reset_Info.CF.Flags.US1_has_been_Reset      Bit
                        Reset_Info.CF.Flags.US2_has_been_Reset      Bit
                        Reset_Info.CF.State                         Enumerator
                        Status.Flags.Local_Reset_Done               Bit
                        DS_Section_Mode.Local_Unit                  BYTE
                        Status.Flags.Preparatory_Reset_DS           Bit


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                           0
                            SM_HAS_RESETTED_SYSTEM          1
                            PREPARATORY_RESET_PENDING       0


References          :

Derived Requirements:

Algorithm           :1. If the system is configured for 3D configuration & reset is accepted,
                        if the CF system is waiting for reset then during start up time reset
                        the system & wait for DS to reset, call the function persist validate
                        to update the checksum.
                     2. If the reset is allowed for 3D then clear the down stream DS1, down stream
                        DS2, up stream US1 & up stream US2 reset flags and wait for DS to reset and
                        call the function persist validate() to update the checksum.
                     3. Check for the reset button state, if it is accepted, set the local reset done
                        flag call the function persist validate() to update the checksum.
                     4. Check for the reset button2 state, if it is accepted, set the local reset2 done
                        flag call the function persist validate() to update the checksum.
                     5. If the CF system is waiting for reset then during start up time reset
                        the system & wait for DS to reset, call the function persist validate
                        to update the checksum.
                     6. During run time get both the vital relays(A & B) state to know the state of the
                        system at remote units
                     7. If both the remote units at US & DS are defective then reset is accepted hence reset
                        down stream DS1, down stream DS2, up stream US1 & up stream US2,local reset1 and local
                        reset2 flag and call the persist validate function to update the check sum & reset
                        the CF  system
                     8. If reset is allowed for both DS & US then reset is accepted hence reset
                        down stream DS1, down stream DS2, up stream US1 & up stream US2, local reset1 and local
                        reset2 flag, call the persist validate function to update the check sum & reset the CF system
                     9. If local reset1 flag and the reset is allowed for DS, then reset only downstream
                         DS1 & down stream DS2 else set local reset flag as waiting for reset and clear the
                        local reset1 flag and call the persist validate function to update the check sum
                    10. If local reset2 flag and the reset is allowed for US, then reset only upstream
                        US1 & up stream US2 else set local reset flag as waiting for reset and clear the
                        local reset2 flag and call the persist validate function to update the check sum
************************************************************************/

void Process_CF_DS_Reset_Command(void)
{
   BYTE uch_A_State,uch_B_State;

    if(Reset_Info.CF.DS_State == CF_DS_WAIT_FOR_RESET)
       {
        Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
        Reset_Info.CF.DS_State = (BYTE) CF_WAIT_FOR_DS_TO_RESET;
        DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
        
        return;
       }

    /* reset at run-time */
    uch_A_State = Get_Relay_A_State();
    uch_B_State = Get_Relay_B_State();
    if(uch_A_State == DAC_DEFECTIVE && uch_B_State == DAC_DEFECTIVE)
       {
          /*reset is accepted */
          Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
          Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
          Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
          Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
          Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
          Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1= FALSE;
          Reset_Info.CF.DS_State =(BYTE) CF_WAIT_FOR_DS_TO_RESET;
          Reset_Info.CF.US_State =(BYTE) CF_US_WAIT_FOR_RESET;
                   
          {__asm__ volatile ("reset");} // RESET();                   /* Reset the System */
          return;
       }
   if(Reset_Allowed_For_DS())
       {
        Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
        Reset_Info.CF.DS_State = (BYTE) CF_WAIT_FOR_DS_TO_RESET;
        Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
        Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING;
        DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
        Set_Relay_B_Reset_State();
                 
       }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Process_CF_US_Reset_Command(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            3           4               80              28-12-05        Refer ATR 80


Abstract            :Get the status of both relay A and B,If defective,reset the system and update the checksum

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RLYD3_MGR.c -   Reset_Allowed_For_D3()
                        
                        RESET()             - compiler defined function
                        RLYA_MGR.c  -   Get_Relay_A_State()
                        RLYB_MGR.c  -   Get_Relay_B_State()
                        RLYB_MGR.c  -   Reset_Allowed_For_DS()
                        RLYA_MGR.c  -   Reset_Allowed_For_US()
                        RLYB_MGR.c  -   Set_Relay_B_Reset_State()
                        RLYA_MGR.c  -   Set_Relay_A_Reset_State()

    Called by       :   RESET.c -   Update_CF_Reset_Monitor_State()
                        RESET.c -   CF_Wait_for_Local_Reset()

Input Variables         Name                                        Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                   DAC_Unit_Type
                        DAC_UNIT_TYPE_CF                            Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        CF_WAIT_FOR_RESET                           Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                     Enumerator
                        DAC_DEFECTIVE                               Enumerator
                        RESET_APPLIED_MODE                          Enumerator

    Local           :   uch_A_State                                 BYTE
                        uch_B_State                                 BYTE

Output Variables        Name                                        Type
    Global          :   Reset_Info.CF.Flags.DS1_has_been_Reset      Bit
                        Reset_Info.CF.Flags.DS2_has_been_Reset      Bit
                        Reset_Info.CF.Flags.US1_has_been_Reset      Bit
                        Reset_Info.CF.Flags.US2_has_been_Reset      Bit
                        Reset_Info.CF.State                         Enumerator
                        Status.Flags.Local_Reset_Done               Bit
                        US_Section_Mode.Local_Unit                  BYTE
                        Status.Flags.Preparatory_Reset_US           Bit


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                           0
                            SM_HAS_RESETTED_SYSTEM          1
                            PREPARATORY_RESET_PENDING       0


References          :

Derived Requirements:

Algorithm           :1. If the system is configured for 3D configuration & reset is accepted,
                        if the CF system is waiting for reset then during start up time reset
                        the system & wait for DS to reset, call the function persist validate
                        to update the checksum.
                     2. If the reset is allowed for 3D then clear the down stream DS1, down stream
                        DS2, up stream US1 & up stream US2 reset flags and wait for DS to reset and
                        call the function persist validate() to update the checksum.
                     3. Check for the reset button state, if it is accepted, set the local reset done
                        flag call the function persist validate() to update the checksum.
                     4. Check for the reset button2 state, if it is accepted, set the local reset2 done
                        flag call the function persist validate() to update the checksum.
                     5. If the CF system is waiting for reset then during start up time reset
                        the system & wait for DS to reset, call the function persist validate
                        to update the checksum.
                     6. During run time get both the vital relays(A & B) state to know the state of the
                        system at remote units
                     7. If both the remote units at US & DS are defective then reset is accepted hence reset
                        down stream DS1, down stream DS2, up stream US1 & up stream US2,local reset1 and local
                        reset2 flag and call the persist validate function to update the check sum & reset
                        the CF  system
                     8. If reset is allowed for both DS & US then reset is accepted hence reset
                        down stream DS1, down stream DS2, up stream US1 & up stream US2, local reset1 and local
                        reset2 flag, call the persist validate function to update the check sum & reset the CF system
                     9. If local reset1 flag and the reset is allowed for DS, then reset only downstream
                         DS1 & down stream DS2 else set local reset flag as waiting for reset and clear the
                        local reset1 flag and call the persist validate function to update the check sum
                    10. If local reset2 flag and the reset is allowed for US, then reset only upstream
                        US1 & up stream US2 else set local reset flag as waiting for reset and clear the
                        local reset2 flag and call the persist validate function to update the check sum
************************************************************************/

void Process_CF_US_Reset_Command(void)
{
   BYTE uch_A_State,uch_B_State;

    if(Reset_Info.CF.US_State == CF_US_WAIT_FOR_RESET)
       {
          Status.Flags.Local_Reset_Done = SM_HAS_RESETTED_SYSTEM;
          Reset_Info.CF.US_State = (BYTE) CF_WAIT_FOR_US_TO_RESET;
                 
          US_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
          return;
       }
    /* reset at run-time */
    uch_A_State = Get_Relay_A_State();
    uch_B_State = Get_Relay_B_State();
    if(uch_A_State == DAC_DEFECTIVE && uch_B_State == DAC_DEFECTIVE)
       {
         /*reset is accepted  */
           Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
           Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
           Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
           Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
           Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
           Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1= FALSE;
           Reset_Info.CF.US_State = (BYTE)CF_WAIT_FOR_US_TO_RESET;
           Reset_Info.CF.DS_State = (BYTE)CF_DS_WAIT_FOR_RESET;
                 
           {__asm__ volatile ("reset");} // RESET();            /* Reset the System */
           return;
        }
   if(Reset_Allowed_For_US())
    {
        Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
        Reset_Info.CF.US_State = (BYTE)CF_WAIT_FOR_US_TO_RESET;
        Status.Flags.Local_Reset_Done = SM_HAS_RESETTED_SYSTEM;
        Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
        US_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
        Set_Relay_A_Reset_State();
                 
    }
}

/*********************************************************************
Component name      :RESET
Module Name         :void Update_EF_Reset_Monitor_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Call Update_Reset_Button_State(),EF_Wait_for_Local_Reset(),
                     After accepting the reset update the Checksum


Design Requirements     :

Interfaces
    Calls           :   RESET.c -   Update_Reset_Button_State()
                        RESET.c -   EF_Wait_for_Local_Reset()
                        
                        RESET.c -   EF_Reset_Sequence_Complete()

    Called by       :   DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()

Input Variables         Name                                        Type
    Global          :   Reset_Info.EF.State                         Enumerator
                        EF_WAIT_FOR_RESET                           Enumerator
                        EF_WAIT_FOR_US_TO_RESET                     Enumerator
                        Reset_Info.EF.Flags.US1_has_been_Reset      Bit
                        Reset_Info.EF.Flags.US2_has_been_Reset      Bit
                        EF_WAIT_FOR_PEER_TO_RESET                   Enumerator
                        EF_WAIT_FOR_US2_TO_RESET                    Enumerator
                        EF_WAIT_FOR_US1_TO_RESET                    Enumerator
                        Reset_Info.EF.Flags.Peer_CPU_has_been_Reset Bit
                        EF_RESET_SEQUENCE_COMPLETED                 Enumerator
                        WAITING_FOR_RESET_MODE                      Enumerator
                        Reset_Button_Info.Timeout_300s              UINT16
                        Reset_Button2_Info.Timeout_300s             UINT16

    Local           :   None

Output Variables        Name                                            Type
    Global          :
                        Reset_Info.EF.State                             Enumerator
                        US_Section_Mode.Flags.Remote_Reset_Applied      Bit
                        US_Section_Mode.Flags.Remote_Preparatory_State  Bit
                        US_Section_Mode.Local_Unit                      BYTE
                        Reset_Button_Info.Timeout_300s                  UINT16
                        Reset_Button2_Info.Timeout_300s                 UINT16

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            SM_HAS_RESETTED_SYSTEM          1
                            TIMEOUT_EVENT                   0
                            COOPERATIVE_RESET_MAX_TIMEOUT   6000
                            TRUE                            1
                            FALSE                           0

References          :

Derived Requirements

Algorithm           :1.Update the reset button state, to know the state of the reset button as to the
                       reset button is pressed or stuck or not pressed
                     2.Update the states of reset monitoring sequence in EF mode locally
                     3.If 5min timeout has completed after the reset aplied at either local or remote unit,
                        clear the reset applied earlier,change the reset info state,update the upstream local unit status,
                        intialise the upstream remote reset and remote preparatory flags and load the 5min reset timer with values
                        then call persist_variable function inorder to update the change in the persistant variable.
                     4.If the EF system is still waiting for reset than just return
                     5.If EF system is waiting for US to reset
                            If both up stream CPU1 & up stream CPU2 has to be reset then the system shall wait
                            for the peer to reset
                            If only up stream CPU1 as been reset than wait for up stream CPU2 to reset
                            If only up stream CPU2 as been reset than wait for up stream CPU1 to reset
                     6.If the EF state is waiting for up stream US1 to reset, than reset the up stream US1, wait
                       for peer CPU to reset 
                     7.If the EF state is waiting for up stream US2 to reset, then reset the up stream US2, wait
                       for peer CPU to reset 
                     8.If the EF state is wait for the peer to reset then reset the peer CPU & with this reset sequence for
                       EF will be completed
************************************************************************/
void Update_EF_Reset_Monitor_State(void)
{
    Update_Reset_Button_State();
    EF_Wait_for_Local_Reset();
     if(Reset_Button_Info.Timeout_300s == TIMEOUT_EVENT )
        {
             Clear_Reset_Info();
             Status.Flags.Local_Reset_Done  = SM_RESET_PENDING;
             Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_RESET;
             US_Section_Mode.Flags.Remote_Reset_Applied = FALSE;
             US_Section_Mode.Flags.Remote_Preparatory_State = TRUE;
             US_Section_Mode.Local_Unit = WAITING_FOR_RESET_MODE;
             Reset_Button_Info.Timeout_300s = COOPERATIVE_RESET_MAX_TIMEOUT;
                     
             return;
          }

    switch (Reset_Info.EF.US_State)
    {
        case EF_WAIT_FOR_RESET:
            break;
        case EF_WAIT_FOR_US_TO_RESET:
            if (Reset_Info.EF.Flags.US1_has_been_Reset == SM_HAS_RESETTED_SYSTEM &&
                Reset_Info.EF.Flags.US2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_PEER_TO_RESET;
                
                break;
            }
            if (Reset_Info.EF.Flags.US1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_US2_TO_RESET;
                
                break;
            }
            if (Reset_Info.EF.Flags.US2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_US1_TO_RESET;
                
            }
            break;
        case EF_WAIT_FOR_US1_TO_RESET:
            if (Reset_Info.EF.Flags.US1_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_PEER_TO_RESET;
                
            }
            break;
        case EF_WAIT_FOR_US2_TO_RESET:
            if (Reset_Info.EF.Flags.US2_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_PEER_TO_RESET;
                
            }
            break;
        case EF_WAIT_FOR_PEER_TO_RESET:
            if (Reset_Info.EF.Flags.Peer_CPU_has_been_Reset == SM_HAS_RESETTED_SYSTEM)
            {
                EF_Reset_Sequence_Complete();
            }
            break;
        case EF_RESET_SEQUENCE_COMPLETED:
            break;
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :void EF_Reset_Sequence_Complete(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --
                            2           1               24              9-12-05         Refer ATR 24
                            3           4               80              28-12-05        Refer ATR 80

Abstract            :Update reset state for End Fed and the Checksum

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   

    Called by       :   RESET.c -   Update_EF_Reset_Monitor_State()

Input Variables         Name                                Type
    Global          :   EF_RESET_SEQUENCE_COMPLETED         Enumerator

    Local           :   None

Output Variables        Name                                Type
    Global          :   Reset_Info.EF.State                 Enumerator

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None


References          :

Derived Requirements
Algorithm           :Indicate that the reset sequence is completed for EF
************************************************************************/
void EF_Reset_Sequence_Complete(void)
{
  Reset_Info.EF.US_State = (BYTE) EF_RESET_SEQUENCE_COMPLETED;
         
}

/*********************************************************************
Component name      :RESET
Module Name         :void EF_Wait_for_Local_Reset(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --



Abstract            :Start reset button polling.Preparatory reset at
                     Runtime.Reset the system and update the checksum

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RLYA_MGR.c          -   Reset_Allowed_For_US()
                        
                        RESET()             - compiler defined function

    Called by       :   RESET.c -   Update_EF_Reset_Monitor_State()

Input Variables         Name                                    Type
    Global          :   Reset_Button_Info.State                 Enumerator
                        RESET_BUTTON_PRESS_ACCEPTED             Enumerator
                        WAIT_FOR_RESET_BUTTON_PRESS             Enumerator
                        EF_WAIT_FOR_US_TO_RESET                 Enumerator
                        EF_WAIT_FOR_RESET                       Enumerator
                        RESET_APPLIED_MODE                      Enumerator

    Local           :   None

Output Variables        Name                                    Type
    Global          :   Reset_Info.EF.State                     Enumerator
                        Reset_Info.EF.Flags.US1_has_been_Reset  Bit
                        Reset_Info.EF.Flags.US2_has_been_Reset  Bit
                        Status.Flags.Local_Reset_Done           Bit
                        Status.Flags.Local_Reset_Done2          Bit
                        US_Section_Mode.Local_Unit              BYTE

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                           0
                            SM_HAS_RESETTED_SYSTEM          1

References          :
Derived Requirements:
Algorithm           :For EF system if reset button for the local system is pressed, update the reset state for two conditions
                        1.During run time, check for US state, update both up stream US1 & US2 state
                        2.During start up local system has already been reset hence check for US to reset
************************************************************************/
void EF_Wait_for_Local_Reset(void)
{
    if (Reset_Button_Info.State == RESET_BUTTON_PRESS_ACCEPTED)
    {
        Reset_Button_Info.State  = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;    /* Start reset button polling */
        if (Reset_Allowed_For_US())
        {
            /* Preparatory reset at Runtime */
            Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_US_TO_RESET;
            /* Preparatory reset is accepted only once */
            Reset_Info.EF.Flags.US1_has_been_Reset = FALSE;
            Reset_Info.EF.Flags.US2_has_been_Reset = FALSE;
            Reset_Info.EF.Flags.Peer_CPU_has_been_Reset = FALSE;
                 /* Update CheckSum  */
            {__asm__ volatile ("reset");} // RESET();               /* Reset the System */
            return;
        }
        if(Reset_Info.EF.US_State == EF_WAIT_FOR_RESET)
        {
            /* Preparatory reset at start-up */
            Status.Flags.Local_Reset_Done  = SM_HAS_RESETTED_SYSTEM;
            Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_US_TO_RESET;
            US_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
            
        }
    }
}

/*********************************************************************
Component name      :RESET
Module Name         :void Update_Reset_Button_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --



Abstract            :Update the reset button state




Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   RESET.c -   Update_SF_Reset_Monitor_State()
                        RESET.c -   Update_CF_Reset_Monitor_State()
                        RESET.c -   Update_EF_Reset_Monitor_State()
                        RESET.c -   Update_DE_Reset_Monitor_State()

Input Variables         Name                            Type
    Global          :   Reset_Button_Info.State         Enumerator
                        WAIT_FOR_RESET_BUTTON_PRESS     Enumerator
                        RESET_BUTTON_PRESSED_MIN_WAIT   Enumerator
                        RESET_BUTTON_PRESSED_MAX_WAIT   Enumerator
                        RESET_BUTTON_PRESS_ACCEPTED     Enumerator
                        RESET_BUTTON_STUCK              Enumerator

    Local           :

Output Variables        Name                            Type
    Global          :   Reset_Button_Info.State         Enumerator
                        Reset_Button_Info.Timeout_ms    UINT16

    Local           :   None
Signal Variables

                                A1_RST_BUTTON_PORT                  Input                   RD3
                                A2_RST_BUTTON_PORT                  Input                   RD4

Macro definitions used:     Macro                           Value
                            RESET_BUTTON_MIN_TIMEOUT        500
                            TIMEOUT_EVENT                   0
                            RESET_BUTTON_MAX_TIMEOUT        9000
                            FALSE                           0
                            TRUE                            1


References          :
Derived Requirements:
Algorithm           :1.check whether the reset button is pressed or not pressed,if pressed wait for minimum
                       time out of around 3second to accept that the reset button is pressed
                     2.Reset button can be kept pressed upto maximum time out of around 9seconds beyond this
                       software will consider that reset button is stuck
************************************************************************/
void Update_Reset_Button_State(void)
{
    switch (Reset_Button_Info.State)
    {
        case WAIT_FOR_RESET_BUTTON_PRESS:
            if (A1_RST_BUTTON_PORT == FALSE && A2_RST_BUTTON_PORT == TRUE)
            {
                Reset_Button_Info.State = (BYTE) RESET_BUTTON_PRESSED_MIN_WAIT;
                Reset_Button_Info.Timeout_ms = RESET_BUTTON_MIN_TIMEOUT;
            }
            break;
        case RESET_BUTTON_PRESSED_MIN_WAIT:
            if (Reset_Button_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Reset_Button_Info.State = (BYTE) RESET_BUTTON_PRESSED_MAX_WAIT;
                Reset_Button_Info.Timeout_ms = RESET_BUTTON_MAX_TIMEOUT;
                break;
            }
            if (A1_RST_BUTTON_PORT == TRUE && A2_RST_BUTTON_PORT == FALSE)
            {
                Reset_Button_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;
                Reset_Button_Info.Timeout_ms = 0;
            }
            break;
        case RESET_BUTTON_PRESSED_MAX_WAIT:
            if (A1_RST_BUTTON_PORT == TRUE && A2_RST_BUTTON_PORT == FALSE)
            {
                Reset_Button_Info.State = (BYTE) RESET_BUTTON_PRESS_ACCEPTED;
                Reset_Button_Info.Timeout_ms = 0;
                break;
            }
            if (Reset_Button_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Reset_Button_Info.State = (BYTE) RESET_BUTTON_STUCK;
            }
            break;
        case RESET_BUTTON_PRESS_ACCEPTED:
                /*
                 * Remain in this state until ??_Wait_for_Local_Reset function/s
                 * changes Reset_Button_Info.State.
                 */
            break;
        case RESET_BUTTON_STUCK:
            if (A1_RST_BUTTON_PORT == TRUE && A2_RST_BUTTON_PORT == FALSE)
            {
                Reset_Button_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;
                Reset_Button_Info.Timeout_ms = 0;
            }
            break;
        default:
            Reset_Button_Info.State = (BYTE) RESET_BUTTON_STUCK;
            Reset_Button_Info.Timeout_ms = 0;
            break;
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Update_Reset_Button2_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract                :Update the reset button state

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   RESET.c -   Update_CF_Reset_Monitor_State()

    Input Variables         Name                            Type
    Global          :   Reset_Button2_Info.State            Enumerator
                        WAIT_FOR_RESET_BUTTON_PRESS     Enumerator
                        RESET_BUTTON_PRESSED_MIN_WAIT   Enumerator
                        RESET_BUTTON_PRESSED_MAX_WAIT   Enumerator
                        RESET_BUTTON_PRESS_ACCEPTED     Enumerator
                        RESET_BUTTON_STUCK              Enumerator

    Local           :

Output Variables        Name                            Type
    Global          :   Reset_Button2_Info.State        Enumerator
                        Reset_Button2_Info.Timeout_ms   UINT16

    Local           :   None
Signal Variables

                                B1_RST_BUTTON_PORT              Input                   RD5
                                B2_RST_BUTTON_PORT              Input                   RD6

Macro definitions used:     Macro                           Value
                            RESET_BUTTON_MIN_TIMEOUT        500
                            TIMEOUT_EVENT                   0
                            RESET_BUTTON_MAX_TIMEOUT        9000
                            FALSE                           0
                            TRUE                            1


References          :
Derived Requirements:
Algorithm           :1.check whether the reset button is pressed or not pressed,if pressed wait for minimum
                       time out of around 3second to accept that the reset button is pressed
                     2.Reset button can be kept pressed upto maximum time out of around 9seconds beyond this
                       software will consider that reset button is stuck
************************************************************************/
void Update_Reset_Button2_State(void)
{
    switch (Reset_Button2_Info.State)
    {
        case WAIT_FOR_RESET_BUTTON_PRESS:
            if (B1_RST_BUTTON_PORT == FALSE && B2_RST_BUTTON_PORT == TRUE)
            {
                Reset_Button2_Info.State = (BYTE) RESET_BUTTON_PRESSED_MIN_WAIT;
                Reset_Button2_Info.Timeout_ms = RESET_BUTTON_MIN_TIMEOUT;
            }
            break;
        case RESET_BUTTON_PRESSED_MIN_WAIT:
            if (Reset_Button2_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Reset_Button2_Info.State = (BYTE) RESET_BUTTON_PRESSED_MAX_WAIT;
                Reset_Button2_Info.Timeout_ms = RESET_BUTTON_MAX_TIMEOUT;
                break;
            }
            if (B1_RST_BUTTON_PORT == TRUE && B2_RST_BUTTON_PORT == FALSE)
            {
                Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;
                Reset_Button2_Info.Timeout_ms = 0;
            }
            break;
        case RESET_BUTTON_PRESSED_MAX_WAIT:
            if (B1_RST_BUTTON_PORT == TRUE && B2_RST_BUTTON_PORT == FALSE)
            {
                Reset_Button2_Info.State = (BYTE) RESET_BUTTON_PRESS_ACCEPTED;
                Reset_Button2_Info.Timeout_ms = 0;
                break;
            }
            if (Reset_Button2_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Reset_Button2_Info.State = (BYTE) RESET_BUTTON_STUCK;
            }
            break;
        case RESET_BUTTON_PRESS_ACCEPTED:
                /*
                 * Remain in this state until ??_Wait_for_Local_Reset function/s
                 * changes Reset_Button_Info.State.
                 */
            break;
        case RESET_BUTTON_STUCK:
            if (B1_RST_BUTTON_PORT == TRUE && B2_RST_BUTTON_PORT == FALSE)
            {
                Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;
                Reset_Button2_Info.Timeout_ms = 0;
            }
            break;
        default:
            Reset_Button2_Info.State = RESET_BUTTON_STUCK;
            Reset_Button2_Info.Timeout_ms = 0;
            break;
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Clear_Reset_Info(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24
                            3           4               80              28-12-05        Refer ATR 80

Abstract            :Depending on configuration clear the reset info flag
                     and update the checksum.Initialize reset button Timeout to
                     zero and assign reset button state to wait for reset button
                     to press.

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   


    Called by       :   RLYA_MGR.c  -   Start_Relay_A_Mgr()
                        RLYB_MGR.c  -   Start_Relay_B_Mgr()
                        RLYD3_MGR.c -   Start_Relay_D3_Mgr()
                        RLYDE_MGR.c -   Start_Relay_DE_Mgr()

Input Variables         Name                                        Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                   DAC_Unit_Type
                        DAC_UNIT_TYPE_SF                            Enumerator
                        DAC_UNIT_TYPE_CF                            Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_EF                            Enumerator
                        DAC_UNIT_TYPE_DE                            Enumerator
                        WAIT_FOR_RESET_BUTTON_PRESS                 Enumerator

    Local           :   None

Output Variables        Name                                            Type
    Global          :   Reset_Info.SF.Flags.DS1_has_been_Reset          Bit
                        Reset_Info.SF.Flags.DS2_has_been_Reset          Bit
                        Reset_Info.SF.Flags.Peer_CPU_has_been_Reset     Bit
                        Reset_Info.CF.Flags.DS1_has_been_Reset          Bit
                        Reset_Info.CF.Flags.DS2_has_been_Reset          Bit
                        Reset_Info.CF.Flags.US1_has_been_Reset          Bit
                        Reset_Info.CF.Flags.US2_has_been_Reset          Bit
                        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset     Bit
                        Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1    Bit
                        Reset_Info.CF.Flags.Local_Reset1_done           Bit
                        Reset_Info.CF.Flags.Local_Reset2_done           Bit
                        Reset_Info.EF.Flags.US1_has_been_Reset          Bit
                        Reset_Info.EF.Flags.US2_has_been_Reset          Bit
                        Reset_Info.EF.Flags.Peer_CPU_has_been_Reset     Bit
                        Reset_Info.DE.Flags.Peer_CPU_has_been_Reset     Bit
                        Reset_Button_Info.Timeout_ms                    UINT16
                        Reset_Button_Info.State                         Enumerator

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                           0


References          :

Derived Requirements:

Algorithm           :1.If the DAC unit type is SF,then it involves only down stream communication hence
                       reset down stream CPU1, down stream CPU2, peer CPU & call the library function persist validate()
                       as there is change in persistant variable
                     2.If the DAC unit type is CF,then it involves both up stream & down stream communication
                       hence reset down stream CPU1, down stream CPU2, up stream CPU1, up stream CPU2, peer CPU reset
                       reset1, local reset1 done, local reset2 done & call the library function persist validate()
                       as there is change in the persistant variable
                     3.If the DAC unit type is EF, then it involves only up stream communication hence reset up stream CPU1,
                       up stream CPU2, peer CPU & call the function persist validate() as there is change in the persistant
                       variable
                     4.If the DAC unit type is DE, then it involves no communication hence reset only the peer CPU & call the
                       function persist validate() as there is change in the persistant variable
************************************************************************/
void Clear_Reset_Info(void)
{
    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_SF:
            Reset_Info.SF.Flags.DS1_has_been_Reset = FALSE;
            Reset_Info.SF.Flags.DS2_has_been_Reset = FALSE;
            Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
            
            break;
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
            Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1= FALSE;
            
            break;
        case DAC_UNIT_TYPE_D3_A:
            Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1= FALSE;
            
            break;
        case DAC_UNIT_TYPE_D3_B:
            Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1= FALSE;
            
            break;
        case DAC_UNIT_TYPE_D3_C:
            Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1= FALSE;
            
            break;
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            Reset_Info.CF.Flags.DS1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.DS2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US1_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.US2_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = FALSE;
            Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1= FALSE;
            
            break;
        case DAC_UNIT_TYPE_EF:
            Reset_Info.EF.Flags.US1_has_been_Reset = FALSE;
            Reset_Info.EF.Flags.US2_has_been_Reset = FALSE;
            Reset_Info.EF.Flags.Peer_CPU_has_been_Reset = FALSE;
            
            break;
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
            Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
            
            break;
        case DAC_UNIT_TYPE_DE:
            Reset_Info.SF.Flags.Peer_CPU_has_been_Reset1 = FALSE;
            
            break;
    }
    Reset_Button_Info.Timeout_ms = 0;
    Reset_Button_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;   /* Start reset button polling */
    Reset_Button2_Info.Timeout_ms = 0;
    Reset_Button2_Info.State = (BYTE) WAIT_FOR_RESET_BUTTON_PRESS;  /* Start reset button polling */

}
/*********************************************************************
Component name      :RESET
Module Name         :void Clear_Local_Reset_Flag(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24


Abstract            :Clear the local reset flag

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   RELAYMGR.c  -   Declare_DAC_Defective()
                        RELAYMGR.c  -   Declare_DAC_Defective_DS()
                        RLYB_MGR.c  -   Update_Relay_B_State()
                        RLYDE_MGR.c -   Update_Relay_DE_State()

Input Variables         Name                                Type
    Global          :   None

    Local           :   None

Output Variables        Name                                Type
    Global          :   Status.Flags.Local_Reset_Done       Bit


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            SM_RESET_PENDING                0

References          :

Derived Requirements:

Algorithm           :1.Set the local reset done status flag as LOW
************************************************************************/
void Clear_Local_Reset_Flag(void)
{
    Status.Flags.Local_Reset_Done = SM_RESET_PENDING;
}
/*********************************************************************
Component name      :RESET
Module Name         :void Clear_Local_Reset2_Flag(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Clear the local reset2 flag

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   RELAYMGR.c  -   Declare_DAC_Defective()
                        RELAYMGR.c  -   Declare_DAC_Defective_US()
                        RLYA_MGR.c  -   Update_Relay_A_State()


Input Variables         Name                                Type
    Global          :   None

    Local           :   None

Output Variables        Name                                Type
    Global          :  Status.Flags.Local_Reset_Done2       Bit

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            SM_RESET_PENDING                0

References          :

Derived Requirements:

Algorithm           :1.Set the local reset done2 status flag as LOW
************************************************************************/
void Clear_Local_Reset2_Flag(void)
{
    Status.Flags.Local_Reset_Done2= SM_RESET_PENDING;
 }
/*********************************************************************
Component name      :RESET
Module Name         :void Decrement_Reset_msTmr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24


Abstract            :Decremnet the reset timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   NIL

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_DE_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_DE()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables         Name                                Type
    Global          :   Reset_Button_Info.Timeout_ms        UINT16
                        Reset_Button2_Info.Timeout_ms       UINT16

    Local           :   None

Output Variables        Name                                Type
    Global          :   Reset_Button_Info.Timeout_ms        UINT16
                        Reset_Button2_Info.Timeout_ms       UINT16

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None


References          :
Derived Requirements:

Algorithm           :1.Check the reset button timer value is greater than zero.
                     2.If it is greater than zero decrement the value by 1
                     3.Check the reset button2 timer value is greater than zero.
                     4.If it is greater than zero decrement the value by 1
************************************************************************/
void Decrement_Reset_msTmr(void)
{
    if (Reset_Button_Info.Timeout_ms > 0)
    {
        Reset_Button_Info.Timeout_ms = Reset_Button_Info.Timeout_ms - 1;
    }
    if(Reset_Button2_Info.Timeout_ms > 0)
    {
       Reset_Button2_Info.Timeout_ms = Reset_Button2_Info.Timeout_ms - 1;
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Decrement_Reset_300sTmr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Decreament the reset timer for clearing the reset after 5minutes timeout

Allocated Requirements  :
Design Requirements     :

Interfaces
    Calls           :   NIL

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()

Input Variables         Name                                Type
    Global          :   Reset_Button_Info.Timeout_300s      UINT16

    Local           :   None

Output Variables        Name                                Type
    Global          :   Reset_Button_Info.Timeout_300s      UINT16


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None


References          :
Derived Requirements:

Algorithm           :1.Check the reset button 300ms timer value is greater than zero.
                     2.If it is greater than zero decrement the value by 1
************************************************************************/
void Decrement_Reset_300sTmr(void)
{
    if (Reset_Button_Info.Timeout_300s > 0)
    {
        Reset_Button_Info.Timeout_300s = Reset_Button_Info.Timeout_300s - 1;
    }

}
/*********************************************************************
Component name      :RESET
Module Name         :void Decrement_Reset_300sTmr2(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Decreament the reset timer for clearing the reset after 5minutes timeout

Allocated Requirements  :
Design Requirements     :

Interfaces
    Calls           :   NIL

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()

Input Variables         Name                                Type
    Global          :   Reset_Button2_Info.Timeout_300s     UINT16

    Local           :   None

Output Variables        Name                                Type
    Global          :   Reset_Button2_Info.Timeout_300s     UINT16


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None


References          :
Derived Requirements:

Algorithm           :1.Check the reset button2 300ms timer value is greater than zero.
                     2.If it is greater than zero decrement the value by 1
************************************************************************/
void Decrement_Reset_300sTmr2(void)
{

    if(Reset_Button2_Info.Timeout_300s > 0)
    {
       Reset_Button2_Info.Timeout_300s = Reset_Button2_Info.Timeout_300s - 1;
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_SF_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --
                            2           1               24              9-12-05         Refer ATR 24
                            3           4               80              28-12-05        Refer ATR 80

Abstract            :Get the SF reset state to monitor the system

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_SF_Reset_Sequence ()


Input Variables             Name                            Type
    Global          :   Reset_Info.SF.State                 Enumerator

    Local           :   uchState                            BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Load the Centre fed downstream reset state value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_SF_Reset_State(void)
{
    BYTE uchState;

    uchState = (BYTE) Reset_Info.SF.DS_State;

    return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_SF_DS1_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --


Abstract            :Get the Down stream CPU1 reset state to start the 5min reset timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_SF_Reset_Sequence ()



Input Variables             Name                                Type
    Global          :   Reset_Info.SF.Flags.DS1_has_been_Reset  Bit

    Local           :   uchState                            BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Load the Centre fed downstream CPU1 reset status flag  value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_SF_DS1_Reset_State(void)
{
  BYTE uchState;

  uchState = (BYTE) Reset_Info.SF.Flags.DS1_has_been_Reset;
  return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_SF_DS2_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --


Abstract            :Get the Down stream CPU2 reset state to start the 5min reset timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_SF_Reset_Sequence ()



Input Variables             Name                                Type
    Global          :   Reset_Info.SF.Flags.DS2_has_been_Reset  Bit

    Local           :   uchState                            BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Load the Centre fed downstream CPU2 reset status flag  value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_SF_DS2_Reset_State(void)
{
  BYTE uchState;

  uchState = (BYTE) Reset_Info.SF.Flags.DS2_has_been_Reset;
  return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_CF_US_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                        2           1               24              9-12-05         Refer ATR 24
                        3           4               80              28-12-05        Refer ATR 80




Abstract            :Get the CF US reset state to monitor the system

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_CF_US_Reset_Sequence ()
                        RLYA_MGR.c - Update_Relay_A_State()


Input Variables             Name                            Type
    Global          :   Reset_Info.CF.US_State                  Enumerator

    Local           :   uchState                            BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None


References          :
Derived Requirements:

Algorithm           :1.Load the Centre fed upstream reset state value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_CF_US_Reset_State(void)
{
    BYTE uchState;

    uchState = (BYTE) Reset_Info.CF.US_State;
    return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_CF_US1_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --


Abstract            :Get the Up stream CPU1 reset state to start the 5min reset timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_EF_Reset_Sequence ()
                        DAC_MAIN.c - Control_DAC_Type_CF()


Input Variables             Name                                Type
    Global          :   Reset_Info.CF.Flags.US1_has_been_Reset  Bit

    Local           :   uchState                            BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Load the Centre fed upstream CPU1 reset status flag  value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_CF_US1_Reset_State(void)
{
  BYTE uchState;

  uchState = (BYTE) Reset_Info.CF.Flags.US1_has_been_Reset;
  return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_CF_US2_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --


Abstract            :Get the Up stream CPU2 reset state to start the 5min reset timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_EF_Reset_Sequence ()
                        DAC_MAIN.c - Control_DAC_Type_CF()


Input Variables             Name                                Type
    Global          :   Reset_Info.CF.Flags.US2_has_been_Reset  Bit

    Local           :   uchState                            BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Load the Centre fed upstream CPU2 reset status flag  value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_CF_US2_Reset_State(void)
{
  BYTE uchState;

  uchState = (BYTE) Reset_Info.CF.Flags.US2_has_been_Reset;
  return (uchState);
}

/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_CF_DS_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                        2           1               24              9-12-05         Refer ATR 24
                        3           4               80              28-12-05        Refer ATR 80


Abstract            :Get the CF DS reset state to monitor the system

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_CF_DS_Reset_Sequence ()
                        RLYB_MGR.c - Update_Relay_B_State()


Input Variables             Name                            Type
    Global          :   Reset_Info.CF.DS_State                  Enumerator

    Local           :   uchState                            BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None


References          :
Derived Requirements:

Algorithm           :1.Load the Centre fed downstream reset state value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_CF_DS_Reset_State(void)
{
    BYTE uchState;

    uchState = (BYTE) Reset_Info.CF.DS_State;
    return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_CF_DS1_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --


Abstract            :Get the Down stream CPU1 reset state to start the 5min reset timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_CF_Reset_Sequence ()
                        DAC_MAIN.c - Control_DAC_Type_CF()


Input Variables             Name                                Type
    Global          :   Reset_Info.CF.Flags.DS1_has_been_Reset  Bit

    Local           :   uchState                                BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Load the Centre fed downstream CPU1 reset status flag  value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_CF_DS1_Reset_State(void)
{
  BYTE uchState;

  uchState = (BYTE) Reset_Info.CF.Flags.DS1_has_been_Reset;
  return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_CF_DS2_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --


Abstract            :Get the Down stream CPU2 reset state to start the 5min reset timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_CF_Reset_Sequence ()
                        DAC_MAIN.c - Control_DAC_Type_CF()


Input Variables             Name                                Type
    Global          :   Reset_Info.CF.Flags.DS2_has_been_Reset  Bit

    Local           :   uchState                                BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Load the Centre fed downstream CPU2 reset status flag  value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_CF_DS2_Reset_State(void)
{
  BYTE uchState;

  uchState = (BYTE) Reset_Info.CF.Flags.DS2_has_been_Reset;
  return (uchState);
}

/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_EF_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                        2           1               24              9-12-05         Refer ATR 24
                        3           4               80              28-12-05        Refer ATR 80



Abstract            :Get the EF reset state to monitor the system

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_EF_Reset_Sequence ()


Input Variables             Name                            Type
    Global          :   Reset_Info.EF.State                 Enumerator

    Local           :   uchState                            BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :
Derived Requirements:

Algorithm           :1.Load the End fed reset state value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_EF_Reset_State(void)
{
    BYTE uchState;

    uchState = (BYTE) Reset_Info.EF.US_State;
    return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_EF_US1_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Get the Up stream CPU1 reset state to start the reset 5minutes timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_EF_Reset_Sequence ()


Input Variables             Name                                    Type
    Global          :   Reset_Info.EF.Flags.US1_has_been_Reset      Bit

    Local           :   uchState                                    BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :
Derived Requirements:

Algorithm           :1.Load the End fed CPU1 reset status flag  value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_EF_US1_Reset_State(void)
{
  BYTE uchState;

  uchState = (BYTE) Reset_Info.EF.Flags.US1_has_been_Reset;
  return (uchState);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Get_EF_US2_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Get the Up stream CPU2 reset state to start the reset 5minutes timer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c - Start_EF_Reset_Sequence ()


Input Variables             Name                                    Type
    Global          :   Reset_Info.EF.Flags.US2_has_been_Reset      Bit

    Local           :   uchState                                    BYTE

Output Variables            Name                            Type
    Global          :       None

    Local           :   uchState                            BYTE
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :
Derived Requirements:

Algorithm           :1.Load the End fed CPU2 reset status flag value to the local variable uchState
                     2.Return the uchState value
************************************************************************/
BYTE Get_EF_US2_Reset_State(void)
{
  BYTE uchState;

  uchState = (BYTE) Reset_Info.EF.Flags.US2_has_been_Reset;
  return (uchState);
}


/*********************************************************************
Component name      :RESET
Module Name         :void Post_DS1_has_been_Reset(BYTE uchData)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24

                            2           1,4             10              8-12-05         Refer ATR 10


Abstract            :If a unit type is CF, system involves co-operative reset
                     hence check the reset flag of down stream CPU1, call the function
                     persist validate() to update the checksum as there is change in persistant
                     variable

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           : 

    Called by       :   COMM_DS - Process_DS_Reset_Info_Message()
                        COMM_DS - Process_DS_Axle_Count_Message()

Input Variables         Name                                        Type
    Global          :   None

    Local           :   uchData                                     BYTE

Output Variables        Name                                        Type
    Global          :   Reset_Info.CF.Flags.DS1_has_been_Reset      Bit

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :
Derived Requirements:

Algorithm           :1.Update the Downstream CPU1 reset status
                     2.Call the persist_variable function as there is a change in the persistant variable
************************************************************************/
void Post_DS1_has_been_Reset(BYTE uchData)
{
    Reset_Info.CF.Flags.DS1_has_been_Reset = uchData;
    
}
/*********************************************************************
Component name      :RESET
Module Name         :void Post_DS2_has_been_Reset(BYTE uchData)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24


Abstract            :If a unit type is CF, system involves co-operative reset
                     hence check the reset flag of down stream CPU2, call the function
                     persist validate() to update the checksum as there is change in persistant
                     variable

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           : 

    Called by       :   COMM_DS - Process_DS_Reset_Info_Message
                        COMM_DS - Process_DS_Axle_Count_Message


Input Variables         Name                                        Type
    Global          :   None

    Local           :   uchData                                     BYTE

Output Variables        Name                                        Type
    Global          :   Reset_Info.CF.Flags.DS2_has_been_Reset      Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            None


References          :
Derived Requirements:

Algorithm           :1.Update the Downstream CPU2 reset status
                     2.Call the persist_variable function as there is a change in the persistant variable
************************************************************************/
void Post_DS2_has_been_Reset(BYTE uchData)
{
    Reset_Info.CF.Flags.DS2_has_been_Reset = uchData;
    
}
/*********************************************************************
Component name      :RESET
Module Name         :void Post_US1_has_been_Reset(BYTE uchData)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24


Abstract            :If a unit type is CF, system involves co-operative reset
                     hence check the reset flag of up stream CPU1, call the function
                     persist validate() to update the checksum as there is change in persistant
                     variable

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   persist_validate() - Complier defined function

    called by       :   COMM_US.c   -   Process_US_Reset_Info_Message()
                        COMM_US.c   -   Process_US_Axle_Count_Message()

Input Variables         Name                                        Type
    Global          :   None

    Local           :   uchData                                     BYTE

Output Variables        Name                                    Type
    Global          :   Reset_Info.CF.Flags.US1_has_been_Reset  Bit

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :
Derived Requirements:

Algorithm           :1.Update the Upstream CPU1 reset status
                     2.Call the persist_variable function as there is a change in the persistant variable
************************************************************************/
void Post_US1_has_been_Reset(BYTE uchData)
{
    Reset_Info.CF.Flags.US1_has_been_Reset = uchData;
    
}
/*********************************************************************
Component name      :RESET
Module Name         :void Post_US2_has_been_Reset(BYTE uchData)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --

                            2           1               24              9-12-05         Refer ATR 24

Abstract            :If a unit type is CF, system involves co-operative reset
                     hence check the reset flag of up stream CPU2, call the function
                     persist validate() to update the checksum as there is change in persistant
                     variable

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   persist_validate() - Complier defined function

    Called by       :   COMM_US.c   -   Process_US_Reset_Info_Message()
                        COMM_US.c   -   Process_US_Axle_Count_Message()

Input Variables         Name                                        Type
    Global          :   None

    Local           :   uchData                                     BYTE

Output Variables        Name                                        Type
    Global          :   Reset_Info.CF.Flags.US2_has_been_Reset      Bit

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Update the Upstream CPU2 reset status
                     2.Call the persist_variable function as there is a change in the persistant variable
************************************************************************/
void Post_US2_has_been_Reset(BYTE uchData)
{
    Reset_Info.CF.Flags.US2_has_been_Reset = uchData;
    
}
/*********************************************************************
Component name      :RESET
Module Name         :void Post_Peer_CPU_Reset(BYTE uchData)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --
                            2           1               24              9-12-05         Refer ATR 24

                            2           01              67              10-12-05        Refer ATR No.67

Abstract            :In unit type CF,reset sequence is completed by resetting the peer CPU
                     hence check the reset flag of peer CPU, call the function persist validate
                     to update the checksum as there is change in persistant variable

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   persist_validate() - Complier defined function

    Called by       :   COMM_SM.c   -   Process_Interprocess_Message()

Input Variables         Name                                        Type
    Global          :   None

    Local           :   uchData                                         BYTE

Output Variables        Name                                            Type
    Global          :   Reset_Info.CF.Flags.Peer_CPU_has_been_Reset     Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.update the peer CPU reset status
                     2.Call the persist_variable function as there is a change in the persistant variable
************************************************************************/
void Post_Peer_CPU_Reset(BYTE uchData)
{
    Reset_Info.CF.Flags.Peer_CPU_has_been_Reset = uchData;
    
}
/*********************************************************************
Component name      :RESET
Module Name         :void Post_Peer_CPU_Reset1(BYTE uchData)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :In unit type CF,reset sequence is completed by resetting the peer CPU
                     hence check the reset flag of peer CPU, call the function persist validate
                     to update the checksum as there is change in persistant variable

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   persist_validate() - Complier defined function

    Called by       :   COMM_SM.c   -   Process_Interprocess_Message()

Input Variables         Name                                        Type
    Global          :   None

    Local           :   uchData                                         BYTE

Output Variables        Name                                            Type
    Global          :   Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1    Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.update the peer CPU reset1 status
                     2.Call the persist_variable function as there is a change in the persistant variable
************************************************************************/
void Post_Peer_CPU_Reset1(BYTE uchData)
{
    Reset_Info.CF.Flags.Peer_CPU_has_been_Reset1 = uchData;
    
}


/*********************************************************************
Component name      :RESET
Module Name         :void Update_DS_Section_Remote_Reset(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the Down stream reset status

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :

    Called by       :   COMM_DS.c   -   Process_DS_Reset_Info_Message()
                        COMM_DS.c   -   Process_DS_Axle_Count_Message()

Input Variables         Name                                            Type
    Global          :   DS_Section_Mode.Flags.Remote_Reset_Applied       Bit
                        RESET_APPLIED_MODE                               Enumerator

    Local           :   uchData                                         BYTE

Output Variables        Name                                            Type
    Global          :   DS_Section_Mode.Remote_Unit                     BYTE
                        DS_Section_Mode.Flags.Remote_Reset_Applied       Bit
                        DS_Section_Mode.Flags.Remote_Preparatory_State   Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                             0
                            SET_HIGH                          1
                            SET_LOW                           0

References          :

Derived Requirements:

Algorithm           :1.Check the Downstream section remote reset flag
                     2.If the flag value is zero then update the downstream section remote unit state as
                        reset applied mode,set the remote reset applied flag as 1 and set the remote preparatory
                        flag as 0 inorder to update the preparatory mode
************************************************************************/
void Update_DS_Section_Remote_Reset(void)
{
   if(DS_Section_Mode.Flags.Remote_Reset_Applied == FALSE)
      {
        DS_Section_Mode.Remote_Unit = (BYTE)RESET_APPLIED_MODE;
        DS_Section_Mode.Flags.Remote_Reset_Applied = SET_HIGH;
        DS_Section_Mode.Flags.Remote_Preparatory_State = SET_LOW;
      }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Update_DS_Section_Remote_Preparatory(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the Down stream Preparatory reset status

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :

    Called by       :   COMM_DS.c   -   Process_DS_Axle_Count_Message()

Input Variables         Name                                            Type
    Global          :   DIP_Switch_Info.Flags.ATC_Enabled                Bit
                        WAIT_FOR_PILOT_TRAIN_MODE                        Enumerator

    Local           :   uchData                                         BYTE

Output Variables        Name                                            Type
    Global          :   DS_Section_Mode.Remote_Unit                     BYTE
                        DS_Section_Mode.Flags.Remote_Preparatory_State   Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            TRUE                              1
                            SET_HIGH                          1


References          :

Derived Requirements:

Algorithm           :1.Check the flag ATC enabled
                     2.If the flag value is not equal to 1 then load the Downstream section remote unit status as
                        wait for pilot train mode.
                     3.Set the flag remote preparatory state used for updating the remote unit preparatory state
************************************************************************/
void Update_DS_Section_Remote_Preparatory(void)
{
   if(DIP_Switch_Info.Flags.ATC_Enabled != TRUE)
    {
     DS_Section_Mode.Remote_Unit = (BYTE)WAIT_FOR_PILOT_TRAIN_MODE;
     DS_Section_Mode.Flags.Remote_Preparatory_State = SET_HIGH;
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Update_US_Section_Remote_Reset(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the Up stream reset status

Allocated Requirements  :
Design Requirements     :

Interfaces
    Calls           :

    Called by       :   COMM_US.c   -   Process_US_Reset_Info_Message()
                        COMM_US.c   -   Process_US_Axle_Count_Message()

Input Variables         Name                                            Type
    Global          :   US_Section_Mode.Flags.Remote_Reset_Applied       Bit
                        RESET_APPLIED_MODE                               Enumerator

    Local           :   uchData                                         BYTE

Output Variables        Name                                            Type
    Global          :   US_Section_Mode.Remote_Unit                     BYTE
                        US_Section_Mode.Flags.Remote_Reset_Applied       Bit
                        US_Section_Mode.Flags.Remote_Preparatory_State   Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            FALSE                             0
                            SET_HIGH                          1
                            SET_LOW                           0

References          :

Derived Requirements:

Algorithm           :1.Check the Upstream section remote reset flag
                     2.If the flag value is zero then update the Upstream section remote unit state as
                        reset applied mode,set the remote reset applied flag as 1 and set the remote preparatory
                        flag as 0 inorder to update the preparatory mode
************************************************************************/
void Update_US_Section_Remote_Reset(void)
{
   if(US_Section_Mode.Flags.Remote_Reset_Applied == FALSE)
      {
        US_Section_Mode.Remote_Unit = (BYTE)RESET_APPLIED_MODE;
        US_Section_Mode.Flags.Remote_Reset_Applied = SET_HIGH;
        US_Section_Mode.Flags.Remote_Preparatory_State = SET_LOW;
      }
}
/*********************************************************************
Component name      :RESET
Module Name         :void Update_US_Section_Remote_Preparatory(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the UP stream Preparatory reset status

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :

    Called by       :   COMM_US.c   -   Process_US_Axle_Count_Message()

Input Variables         Name                                            Type
    Global          :   DIP_Switch_Info.Flags.ATC_Enabled                Bit
                        WAIT_FOR_PILOT_TRAIN_MODE                        Enumerator

    Local           :   uchData                                         BYTE

Output Variables        Name                                            Type
    Global          :   US_Section_Mode.Remote_Unit                     BYTE
                        US_Section_Mode.Flags.Remote_Preparatory_State   Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            TRUE                              1
                            SET_HIGH                          1


References          :

Derived Requirements:

Algorithm           :1.Check the flag ATC enabled
                     2.If the flag value is not equal to 1 then load the Upstream section remote unit status as
                        wait for pilot train mode.
                     3.Set the flag remote preparatory state used for updating the remote unit preparatory state
************************************************************************/
void Update_US_Section_Remote_Preparatory(void)
{
   if(DIP_Switch_Info.Flags.ATC_Enabled != TRUE)
    {
     US_Section_Mode.Remote_Unit = (BYTE)WAIT_FOR_PILOT_TRAIN_MODE;
     US_Section_Mode.Flags.Remote_Preparatory_State = SET_HIGH;
    }
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Update_System_Mode(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the mode of local and remote unit to send to SMCPU through SPI

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RESET.c -   Update_DS_Section_Mode(DS_Section_Mode.Remote_Unit,DS_Section_Mode.Local_Unit)
                        RESET.c -   Update_US_Section_Mode(US_Section_Mode.Remote_Unit,US_Section_Mode.Local_Unit)

    Called by       :   COMM_SM.c   -   Build_Interprocess_Reset_Info_Message()
                        COMM_SM.c   -   Build_Interprocess_Post_Reset_Message()

Input Variables         Name                                            Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                        Enumerator
                        DAC_UNIT_TYPE_SF                                 Enumerator
                        DAC_UNIT_TYPE_EF                                 Enumerator
                        DAC_UNIT_TYPE_CF                                 Enumerator
                        DAC_UNIT_TYPE_D3_A                               Enumerator
                        DAC_UNIT_TYPE_D3_B                               Enumerator
                        DAC_UNIT_TYPE_D3_C                               Enumerator
                        WAIT_FOR_PILOT_TRAIN_MODE                        Enumerator

    Local           :   Mode                                            BYTE
                        DS_Mode_Byte                                    BYTE
                        US_Mode_Byte                                    BYTE

Output Variables        Name                                            Type
    Global          :   US_Section_Mode.Remote_Unit                     BYTE
                        US_Section_Mode.Flags.Remote_Preparatory_State   Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            TRUE                              1
                            SET_HIGH                          1


References          :

Derived Requirements:

Algorithm           :1.Check the unit type
                     2.If the unit type is start fed Update downstream section status by reading the table DAC status
                        and load the value into the lower nibble of the byte Mode.
                     3.If the unit type is end fed Update upstream section status by reading the table DAC status
                        and load the value into the higher nibble of the byte Mode.
                     4..If the unit type is centre fed Update upstream section status by reading the table DAC status
                        and load the value into the higher nibble of the byte Mode.
                        If the unit type is centre fed Update downstream section status by reading the table DAC status
                        and load the value into the lower nibble of the byte Mode.
                     5.If the unit type is UNIT A or B or C update the downstream section status by reading the table DAC status
                        and load the value into the lower nibble of the byte Mode.
************************************************************************/

BYTE Update_System_Mode(void)
{
   BYTE Mode = 0;
   BYTE DS_Mode_Byte = 0;
   BYTE US_Mode_Byte = 0;

    switch (DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_SF:
              DS_Mode_Byte = Update_DS_Section_Mode(DS_Section_Mode.Remote_Unit,DS_Section_Mode.Local_Unit);
              Mode = (BYTE)(Mode | DS_Mode_Byte);
             break;

       case DAC_UNIT_TYPE_EF:
             US_Mode_Byte = Update_US_Section_Mode(US_Section_Mode.Remote_Unit,US_Section_Mode.Local_Unit);
             US_Mode_Byte = (BYTE)(US_Mode_Byte << 4);
             Mode = (BYTE)(Mode | US_Mode_Byte);
            break;
       case DAC_UNIT_TYPE_CF:
       case DAC_UNIT_TYPE_3D_SF:
       case DAC_UNIT_TYPE_3D_EF:
        case DAC_UNIT_TYPE_D3_A:
        case DAC_UNIT_TYPE_D3_B:
        case DAC_UNIT_TYPE_D3_C:
            DS_Mode_Byte = Update_DS_Section_Mode(DS_Section_Mode.Remote_Unit,DS_Section_Mode.Local_Unit);
            US_Mode_Byte = Update_US_Section_Mode(US_Section_Mode.Remote_Unit,US_Section_Mode.Local_Unit);
            US_Mode_Byte = (BYTE)(US_Mode_Byte << 4);
            Mode = (BYTE)(Mode | US_Mode_Byte);
            Mode = (BYTE)(Mode | DS_Mode_Byte);
            break;
       /*case DAC_UNIT_TYPE_D3_A:
            DS_Mode_Byte = Update_DS_Section_Mode(DS_Section_Mode.Remote_Unit,DS_Section_Mode.Local_Unit);
            Mode = (BYTE)(Mode | DS_Mode_Byte);
           break;
        case DAC_UNIT_TYPE_D3_B:
            DS_Mode_Byte = Update_DS_Section_Mode(DS_Section_Mode.Remote_Unit,DS_Section_Mode.Local_Unit);
            Mode = (BYTE)(Mode | DS_Mode_Byte);
           break;
        case DAC_UNIT_TYPE_D3_C:
            DS_Mode_Byte = Update_DS_Section_Mode(DS_Section_Mode.Remote_Unit,DS_Section_Mode.Local_Unit);
            Mode = (BYTE)(Mode | DS_Mode_Byte);
           break;
        */
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            DS_Mode_Byte = Update_DS_Section_Mode(DS_Section_Mode.Remote_Unit,DS_Section_Mode.Local_Unit);
            Mode = (BYTE)(Mode | DS_Mode_Byte);
           break;
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
              DS_Mode_Byte = Update_DS_Section_Mode(DS_Section_Mode.Local_Unit,DS_Section_Mode.Local_Unit); /* There is no remote unit */
              Mode = (BYTE)(Mode | DS_Mode_Byte);
             break;
        case DAC_UNIT_TYPE_DE:
              DS_Mode_Byte = Update_DS_Section_Mode(DS_Section_Mode.Local_Unit,DS_Section_Mode.Local_Unit); /* There is no remote unit */
              Mode = (BYTE)(Mode | DS_Mode_Byte);
             break;
       default:
           break;
   }
  return(Mode);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Update_DS_Section_Mode(BYTE ds_remote_mode,BYTE ds_local_mode)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the mode of Down stream local and remote unit from the uchDAC_Status_Table
Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   RESET.c -   Update_System_Mode()

Input Variables         Name                                            Type
    Global          : uchDAC_Status_Table                               BYTE

    Local           :   uchMode                                         BYTE
                        ds_remote_mode                                  BYTE
                        ds_local_mode                                   BYTE


Output Variables        Name                                            Type
    Global          :   None

    Local           :   uchMode                                         BYTE
Signal Variables


Macro definitions used:     Macro                           Value

References          :

Derived Requirements:

Algorithm           :1.Update the Downstream Section status by reading the DAC status table
                     2.Return the readed value inorder to send it to SMCPU through SPI
************************************************************************/

BYTE Update_DS_Section_Mode(BYTE ds_remote_mode,BYTE ds_local_mode)
{
  BYTE uchMode = 0;
  if(ds_remote_mode <= NO_OF_REMOTE_UNIT_STATES && ds_local_mode <= NO_OF_LOCAL_UNIT_STATES)
    {
     uchMode = uchDAC_Status_Table[ds_remote_mode][ds_local_mode];
    }
  return(uchMode);
}
/*********************************************************************
Component name      :RESET
Module Name         :BYTE Update_US_Section_Mode(BYTE ds_remote_mode,BYTE ds_local_mode)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the mode of Up stream local and remote unit from the uchDAC_Status_Table
Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   RESET.c -   Update_System_Mode()

Input Variables         Name                                            Type
    Global          : uchDAC_Status_Table                               BYTE

    Local           :   uchMode                                         BYTE
                        us_remote_mode                                  BYTE
                        us_local_mode                                   BYTE


Output Variables        Name                                            Type
    Global          :   None

    Local           :   uchMode                                         BYTE
Signal Variables


Macro definitions used:     Macro                           Value

References          :

Derived Requirements:

Algorithm           :1.Update the Upstream Section status by reading the DAC status table
                     2.Return the readed value inorder to send it to SMCPU through SPI
************************************************************************/
BYTE Update_US_Section_Mode(BYTE us_remote_mode,BYTE us_local_mode)
{
  BYTE uchMode = 0;
   if(us_remote_mode <= NO_OF_REMOTE_UNIT_STATES && us_local_mode <= NO_OF_LOCAL_UNIT_STATES)
    {
     uchMode = uchDAC_Status_Table[us_remote_mode][us_local_mode];
    }
  return(uchMode);
}
/*********************************************************************
Component name      :RESET
Module Name         :void Clear_DS_Remote_Reset_Flag(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the flag setted for Down stream remote reset updation
Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   RELAYMGR.c  -   Declare_DAC_Defective()
                        RELAYMGR.c  -   Declare_DAC_Defective_DS()

Input Variables         Name                                            Type
    Global          :   None

    Local           :   None


Output Variables        Name                                            Type
    Global          :   DS_Section_Mode.Flags.Remote_Reset_Applied      Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            SET_LOW                           0
References          :

Derived Requirements:

Algorithm           :1.Set the Downstream section remote reset flag as LOW
************************************************************************/
void Clear_DS_Remote_Reset_Flag(void)
{
    DS_Section_Mode.Flags.Remote_Reset_Applied = SET_LOW;
}
/*********************************************************************
Component name      :RESET
Module Name         :void Clear_US_Remote_Reset_Flag(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the flag setted for Up stream remote reset updation
Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   RELAYMGR.c  -   Declare_DAC_Defective()
                        RELAYMGR.c  -   Declare_DAC_Defective_US()

Input Variables         Name                                            Type
    Global          :   None

    Local           :   None


Output Variables        Name                                            Type
    Global          :   US_Section_Mode.Flags.Remote_Reset_Applied      Bit

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            SET_LOW                           0
References          :

Derived Requirements:

Algorithm           :1.Set the Upstream section remote reset flag as LOW
************************************************************************/
void Clear_US_Remote_Reset_Flag(void)
{
    US_Section_Mode.Flags.Remote_Reset_Applied = SET_LOW;
}
/*********************************************************************
Component name      :RESET
Module Name         :void Clear_Reset_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear all the reset status

Design Requirements     :

Interfaces
    Calls           :persist_validate() - compiler defined function
                     RESET.c    -   Clear_Reset_Info()

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_D3_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()

Input Variables         Name                                            Type
    Global          :   SF_WAIT_FOR_RESET                               Enumerator
                        DAC_UNIT_TYPE_SF                                Enumerator
                        DAC_UNIT_TYPE_CF                                Enumerator
                        DAC_UNIT_TYPE_EF                                Enumerator
                        DAC_UNIT_TYPE_D3_A                              Enumerator
                        DAC_UNIT_TYPE_D3_B                              Enumerator
                        DAC_UNIT_TYPE_D3_C                              Enumerator
                        CF_DS_WAIT_FOR_RESET                            Enumerator
                        CF_US_WAIT_FOR_RESET                            Enumerator
                        EF_WAIT_FOR_RESET                               Enumerator


    Local           :   None


Output Variables        Name                                            Type
    Global          :   Status.Flags.Local_Reset_Done                   Bit
                        Status.Flags.Local_Reset_Done2                  Bit
                        Reset_Info.SF.DS_State                          BYTE
                        Reset_Info.CF.DS_State                          BYTE
                        Reset_Info.CF.US_State                          BYTE
                        Reset_Info.EF.US_State                          BYTE

    Local           :   None
Signal Variables


Macro definitions used:     Macro                           Value
                            SM_RESET_PENDING                  0
References          :

Derived Requirements:

Algorithm           :1.Check the unit type
                     2.Clear the local reset status flags
                     3.Load the reset info state
                     4.Call the function persist validate() as there is change in the persistant
                       variable
                     5.Call the function clear reset info to clear all the reset status flags
************************************************************************/
void Clear_Reset_State(void)
{
    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
        case DAC_UNIT_TYPE_SF:
            Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
            Reset_Info.SF.DS_State = (BYTE) SF_WAIT_FOR_RESET;
            
            Clear_Reset_Info();
            break;
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
            Status.Flags.Local_Reset_Done  = SM_RESET_PENDING;
            Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
            Reset_Info.CF.DS_State = (BYTE) CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_RESET;
            
            Clear_Reset_Info();
            break;
        case DAC_UNIT_TYPE_D3_A:
            Status.Flags.Local_Reset_Done  = SM_RESET_PENDING;
            Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
            Reset_Info.CF.DS_State = (BYTE) CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_RESET;
            
            Clear_Reset_Info();
            break;
        case DAC_UNIT_TYPE_D3_B:
            Status.Flags.Local_Reset_Done  = SM_RESET_PENDING;
            Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
            Reset_Info.CF.DS_State = (BYTE) CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_RESET;
            
            Clear_Reset_Info();
            break;
        case DAC_UNIT_TYPE_D3_C:
            Status.Flags.Local_Reset_Done  = SM_RESET_PENDING;
            Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
            Reset_Info.CF.DS_State = (BYTE) CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_RESET;
            
            Clear_Reset_Info();
            break;
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            Status.Flags.Local_Reset_Done  = SM_RESET_PENDING;
            Status.Flags.Local_Reset_Done2 = SM_RESET_PENDING;
            Reset_Info.CF.DS_State = (BYTE) CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State = (BYTE) CF_US_WAIT_FOR_RESET;
            
            Clear_Reset_Info();
            break;
        case DAC_UNIT_TYPE_EF:
            Status.Flags.Local_Reset_Done  = SM_RESET_PENDING;
            Reset_Info.EF.US_State = (BYTE) EF_WAIT_FOR_RESET;
            
            Clear_Reset_Info();
            break;
        default:
            break;
     }

}

