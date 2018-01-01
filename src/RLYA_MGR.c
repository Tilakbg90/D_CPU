/******************************************************************************
    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RLYA_MGR
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
    Functions   :   void Initialise_Relay_A_Mgr(void)
                    void Start_Relay_A_Mgr(void)
                    void Set_Relay_A_Reset_State(void)
                    void Update_Relay_A_State(void)
                    BOOL Reset_Allowed_For_US()
                    void Process_Relay_A_Local_Direction(BYTE uchDirection,UINT16 uiAxleCount)
                    void Process_Relay_A_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void  Process_Peer_Relay_A_Direction( BYTE uchDirection)
                    void Process_Peer_Relay_A_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void Update_Next_Block_State(bitadrb_t SrcAdr, BYTE Next_Block_Status)
                    void Update_A_US1_OUT_Count(UINT16 uiCount)
                    void Update_A_US2_OUT_Count(UINT16 uiCount)
                    void Update_A_US1_IN_Count(UINT16 uiCount)
                    void Update_A_US2_IN_Count(UINT16 uiCount)
                    void Update_A_LU1_Fwd_Count(UINT16 uiCount)
                    void Update_A_LU2_Fwd_Count(UINT16 uiCount)
                    void Update_A_LU1_Rev_Count(UINT16 uiCount)
                    void Update_A_LU2_Rev_Count(UINT16 uiCount)
                    void Update_A_Local_AxleCount(UINT16 uiCount)
                    void Update_A_US1_AxleCount(UINT16 uiCount)
                    void Update_A_US2_AxleCount(UINT16 uiCount)
                    void Update_A_US1_Direction(BYTE Direction)
                    void Update_A_US2_Direction(BYTE Direction)
                    void Update_A_LU1_Direction(BYTE Direction)
                    void Update_A_LU2_Direction(BYTE Direction)
                    void Update_ATC_Remote1_Relay_A_State(BYTE Remote_state)
                    void Update_ATC_Remote2_Relay_A_State(BYTE Remote_state)
                    void Set_Relay_A_DAC_Defective(void)
                    BYTE Get_Relay_A_State(void)
                    BYTE Get_Relay_A_ATC_State(void)
                    void Clear_ATC_Local_Relay_A_State(void)
                    void Clear_Relay_A_Counts(void)
*******************************************************************************/
#include "COMMON.h"
#include "RELAYDRV.h"
#include "RLYA_MGR.h"
#include "RESET.h"
#include "SYS_MON.h"
#include "ERROR.h"

extern  /*near*/  dac_status_t Status;                    /* from dac_main.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from dac_main.c */


extern us_section_mode US_Section_Mode;             /*from DAC_MAIN.c*/

relay_a_info_t Relay_A_Info;

void Clear_Relay_A_Counts(void);

void Update_A_LU1_Fwd_Count  (UINT16 uiCount);
void Update_A_LU2_Fwd_Count  (UINT16 uiCount);
void Update_A_LU1_Rev_Count  (UINT16 uiCount);
void Update_A_LU2_Rev_Count  (UINT16 uiCount);
void Update_A_Local_AxleCount(UINT16 uiCount);
void Update_A_LU1_Direction(BYTE Direction);
void Update_A_LU2_Direction(BYTE Direction);
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Initialise_Relay_A_Mgr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initailise the Relay A Manager

Allocated Requirements: (SSDAC_SWRS_0032)

Design Requirements:	SSDAC_DR_5083

Interfaces
    Calls           :   None

    Called by       :   RELAYMGR.C-Initialise_Relay_Mgr()


Input Variables                     Name                Type
    Global          :   RELAY_MANAGER_NOT_STARTED       Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_A_Info.State              Enumerator

    Local           :   None

Signal Variables

                                        None                        None                None

Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Initialise the relay A state as RELAY MANAGER NOT STARTED
*******************************************************************************/
void Initialise_Relay_A_Mgr(void)
{
    Relay_A_Info.State = RELAY_MANAGER_NOT_STARTED;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Start_Relay_A_Mgr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the relay A  manager


Allocated Requirements:(SSDAC_SWRS_0419),(SSDAC_SWRS_0420)

Design Requirements:	SSDAC_DR_5084

Interfaces
    Calls           :   AXLE_MON.C-Clear_US_AxleCount()
                        AXLE_MON.C-Clear_US_Local_Counts()
                        RLYA_MGR.C-Clear_Relay_A_Counts()
                        RESET.C-Clear_Reset_Info()
                        ERROR.C-Clear_Error_Display()
                        RELAYDRV.C - Energise_Preparatory_Relay_A()

    Called by       :   DAC_MAIN.C-main()
                        RLYA_MGR.C-Update_Relay_A_State()

Input Variables                     Name                    Type
    Global          :   DIP_Switch_Info.Flags.ATC_Enabled   Bit
                        ATC_WAIT_FOR_REMOTE_CLEAR           Enumerator
                        WAIT_FOR_PILOT_TRAIN                Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_A_Info.State              Enumerator
                        Relay_A_Info.ATC_Local_State    Byte

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        TRUE                      1
References          :

Derived Requirements:


Algorithm           :1.Clear the up stream axle direction, Supervisory counts, wheel type
                     2.Clear the forward & reverse up stream & local counts axle counts
                     3.Clear the reset state
                     4.Clear all the errors if set
                     5.If the system configuration is set to ATC configuration than wait to
                       receive remote clear
                     6.If the system is not configured to ATC configuration than make the system
                       wait for the pilot train


*******************************************************************************/
void Start_Relay_A_Mgr(void)
{
    if (Status.Flags.System_Status != NORMAL || Status.Flags.US_System_Status != NORMAL)
    {
        return;
    }
    Clear_US_AxleCount();
    Clear_US_Local_Counts();
    Clear_Relay_A_Counts();

    
     Clear_Reset_Info();                    /* Clear the reset flags and reset state */
     if( DIP_Switch_Info.Flags.ATC_Enabled == TRUE)
     {
        Relay_A_Info.State = ATC_WAIT_FOR_REMOTE_CLEAR;
        Relay_A_Info.ATC_Local_State = ATC_READY_TO_CLEAR;
     }
     else
     {
         Energise_Preparatory_Relay_A();
        Relay_A_Info.State = WAIT_FOR_PILOT_TRAIN;
     }
    

    Clear_Error_Display();              /* Clear Errors set in Minor & Major Errors */

}

/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Set_Relay_A_Reset_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Set relay A reset state

Allocated Requirements:

Design Requirements:	SSDAC_DR_5086

Interfaces
    Calls           :   AXLE_MON.C-Clear_US_Local_Counts()
                        AXLE_MON.C-Stop_US_Axle_Counting()
                        SYS_MON.C-Clear_Comm_US_Failures()
                        SYS_MON.C-Clear_US_Power_Status()
                        ERROR.c - Clear_US_Error_Code()

    Called by       :   RESET.C-Process_CF_Reset_Command()


Input Variables                     Name                Type
    Global          :   DAC_RESET_PROGRESS              Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_A_Info.State              Enumerator
                        Status.Flags.US_System_Status   Bit

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        TRUE                      1
References          :

Derived Requirements:


Algorithm           :1.Set the relay A info state as RESET PROGRESS.
                     2.Set the upsteam system status flag as 1
                     3.Clear the upstream local counts.
                     4.stop Upstream axle counting, set all the communication links related to US
                     as OK & set all the old & new power status related to US as OK
*******************************************************************************/
void Set_Relay_A_Reset_State(void)
{
    Relay_A_Info.State = DAC_RESET_PROGRESS;
    Status.Flags.US_System_Status = TRUE;
    Clear_US_Local_Counts();
    Stop_US_Axle_Counting();
    Clear_Comm_US_Failures();
    Clear_US_Power_Status();
    Clear_US_Error_Code();
    DeEnergise_Preparatory_Relay_A();
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_Relay_A_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the relay A state.

Allocated Requirements:(SSDAC_SWRS_0035), (SSDAC_SWRS_0200),(SSDAC_SWRS_0419),(SSDAC_SWRS_0420)
					  (SSDAC_SWRS_0449), (SSDAC_SWRS_0455),(SSDAC_SWRS_0457),(SSDAC_SWRS_0429)
					  (SSDAC_SWRS_0245), (SSDAC_SWRS_0438), (SSDAC_SWRS_0421), (SSDAC_SWRS_0412)
					  (SSDAC_SWRS_0415), (SSDAC_SWRS_0272), (SSDAC_SWRS_0273), 
					  (SSDAC_SWRS_0355), (SSDAC_SWRS_0361), (SSDAC_SWRS_0324), (SSDAC_SWRS_0364)
					  (SSDAC_SWRS_0369), (SSDAC_SWRS_0370), (SSDAC_SWRS_0371), (SSDAC_SWRS_0372)
					  (SSDAC_SWRS_0330), (SSDAC_SWRS_0339), (SSDAC_SWRS_0312), (SSDAC_SWRS_0836)

Design Requirements:	SSDAC_DR_5087

Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Preparatory_Relay()
                        LCD_DRV.C-Clear_Line_on_LCD()
                        RESET.C-Clear_Local_Reset2_Flag()
                        RELAYDRV.C-Energise_Preparatory_Relay()
                        RELAYMGR.C-Declare_DAC_Defective_US()
                        ERROR.C-Set_Error_Status_Bit()
                        RELAYDRV.C-Get_Relay_Energising_Key()
                        RELAYDRV.C-Energise_Vital_Relay_A()
                        AXLE_MON.C-Clear_US_AxleCount()
                        RELAYDRV.C-DeEnergise_Vital_Relay_A()
                        RESET.C-Get_CF_Reset_State()
                        RLYA_MGR.C-Start_Relay_A_Mgr()
                        AXLE_MON.C-Start_US_Axle_Counting()
                        RELAYDRV.C-DeEnergise_Vital_Relay_A()


    Called by       :   DAC_MAIN.C-Control_DAC_Type_SF()
                        RELAYMGR.C-Update_Relay_A_Counts()

Input Variables                     Name                    Type
    Global          :   Relay_A_Info.State                  Enumerator
                        Relay_A_Info.ATC_Remote1_State      Byte
                        TRAIN_IN_SECTION                    Enumerator
                        Relay_A_Info.ATC_Remote2_State      Byte
                        Relay_A_Info.LU1_Fwd_Count          UINT16
                        Relay_A_Info.LU1_Rev_Count          UINT16
                        Relay_A_Info.LU2_Fwd_Count          UINT16
                        Relay_A_Info.LU2_Rev_Count          UINT16
                        Relay_A_Info.US1_IN_Count           UINT16
                        Relay_A_Info.US2_IN_Count           UINT16
                        Relay_A_Info.US1_OUT_Count          UINT16
                        Relay_A_Info.US2_OUT_Count          UINT16
                        PILOT_TRAIN_IN_SECTION              Enumerator
                        WAIT_FOR_PILOT_TRAIN                Enumertor
                        Relay_A_Info.US1_Direction          BYTE
                        DIRECTION_NOT_DEFINED               Enumerator
                        Relay_A_Info.US2_Direction          BYTE
                        Relay_A_Info.LU1_Direction          BYTE
                        FORWARD_DIRECTION                   Enumerator
                        Relay_A_Info.LU2_Direction          BYTE
                        NO_TRAIN_IN_SECTION                 Enumerator
                        CF_DS_RESET_SEQUENCE_COMPLETED      Enumerator
                        CF_US_RESET_SEQUENCE_COMPLETED      Enumerator
                        DIP_Switch_Info.Flags.ATC_Enabled   Bit
                        Relay_A_Info.Next_Block_CPU1_Status Byte
                        Relay_A_Info.Next_Block_CPU2_Status Byte
                        DAC_REPEATER_RELAY_OFF              Enumerator
                        DAC_REPEATER_RELAY_ON               Enumerator


    Local           :   uiAuthorisationKey              UINT16
                        uchState                        BYTE


Output Variables                    Name                Type
    Global          :   Relay_A_Info.State              Enumerator
                        Status.Flags.Direct_Out_Count   Bit

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        FALSE                     0
                                        ATC_READY_TO_CLEAR        1
                                        SET_HIGH                  1
                                        DIRECT_OUT_ERROR_NUM       44
                                        TRACK_CLEAR               1
References          :

Derived Requirements:


Algorithm           :1. If the system is configured to ATC configuration then the system shall check
                        the remote units to be clear so that it can allow the train in section
                     2. If the system has started receiving the forward / reverse counts  or IN counts
                        /OUT counts indicate that pilot train has enter the section,make the system to
                        come out of reset mode & clear the 3rd line of LCD and clear the Local reset flags,
                        clear the flags setted for updating the remote reset
                     3. If train enters in forward direction at EF and if EF local axle count is more than
                        remote unit axle count,declare DAC defective,set Direct out count error ID.
                        If pilot train enters in forward direction if SF unit direction is not setted and EF
                        unit direction is setted forward then declare it as DAC defective, Set direct out count error ID
                     4. If the system has received the counts of the piolt train but the direction is not
                        defined or system has received only OUT counts but no IN counts then system has
                        to go to error state then declare as DAC defective & set the error bit to indicate
                        the error
                     5. Since the system requires minimum of 2 counts to come out of pilot mode, check
                        wheather it has received more than two counts if not just return with the system
                        in pilot mode waiting for the pilot train
                     6. If the system has started receiving the counts of the pilot train and the counts
                        match at both local & remote unit then deenergise the preparatory relay A, get the
                        security key to energise the Vital relay A to indicate that US track is clear &
                        there is no pilot train in the section
                     7.If the system has started receiving the counts and the counts are not matched at
                        local & remote unit thae allow the Vital relay A to be in de-energised state to
                        indicate that DS track is occupied & there is train the section
                     8.If the system has cleared the last entered train but it receives axle count immediately with
                        local and remote direction are in forward direction then declare DAC defectine,set
                        direct out count error ID.
                     9. If train enters in forward direction at EF and if EF local axle count is more than
                        remote unit axle count,declare DAC defective,set Direct out count error ID.
                        [If train enters in forward direction if SF unit count is greater than EF unit
                        axle count then declare it as DAC defective, Set direct out count error ID]
                     10. If the system has received the counts of the train but the direction is not defined
                       or system has received only OUT counts but no IN counts then system has to go to error
                        state then declare as DAC defective & set the error bit to indicate the error
                     11.If the system has started receiving the counts of the train and the counts matchs at
                        both local & remote unit than get the security key to energise the Vital relay A to
                        indicate that US track is clear & there is no train in the section
                     12.If the repeater relay is ON  then check the status of the next block,if it's not
                        clear than de-energise the repeater relay to indicate that track is not clear for the
                        previous block
                     13.If the repeater relay is OFF check the status of the next block if its clear than
                        get the security key to energise the vital relay B to indicate that track is clear

*******************************************************************************/
void Update_Relay_A_State(void)
{
    INT16 uiAuthorisationKey;

    switch (Relay_A_Info.State)
    {
        case RELAY_MANAGER_NOT_STARTED:
        case DAC_REPEATER_RELAY_ON:
        case DAC_REPEATER_RELAY_OFF:
        case RELAY_MANAGER_IDLE:
             break;
        case ATC_WAIT_FOR_REMOTE_CLEAR:
            if(Relay_A_Info.ATC_Remote1_State == ATC_READY_TO_CLEAR &&
               Relay_A_Info.ATC_Remote2_State == ATC_READY_TO_CLEAR)
            {
               Relay_A_Info.State = TRAIN_IN_SECTION;
            }
            break;
        case WAIT_FOR_PILOT_TRAIN:
            if (Relay_A_Info.LU1_Fwd_Count > 0 || Relay_A_Info.LU1_Rev_Count > 0 ||
                Relay_A_Info.LU2_Fwd_Count > 0 || Relay_A_Info.LU2_Rev_Count > 0 ||
                Relay_A_Info.US1_IN_Count  > 0 || Relay_A_Info.US2_IN_Count > 0 ||
                Relay_A_Info.US1_OUT_Count > 0 || Relay_A_Info.US2_OUT_Count > 0 )
            {
                Relay_A_Info.State = PILOT_TRAIN_IN_SECTION;
                //Clear_Line_on_LCD((BYTE) 3);
                Clear_Local_Reset_Flag();
                Clear_US_Remote_Reset_Flag();
            }
            break;
        case PILOT_TRAIN_IN_SECTION:
            if(Relay_A_Info.US1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_A_Info.US2_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_A_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_A_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
            {
                if(Relay_A_Info.US_Local_Count > Relay_A_Info.US1_Axle_Count ||
                   Relay_A_Info.US_Local_Count > Relay_A_Info.US2_Axle_Count )
                   {
                    Declare_DAC_Defective_US();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
            }
            if ((Relay_A_Info.LU1_Fwd_Count  < 2 ||  Relay_A_Info.LU2_Fwd_Count <2) &&
                (Relay_A_Info.LU1_Rev_Count  < 2 ||  Relay_A_Info.LU2_Rev_Count <2) )
               {
                if(Relay_A_Info.US1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
                   Relay_A_Info.US2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
                   Relay_A_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
                   Relay_A_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
                  {
                   /* no in-counts, only out-counts,system goes to error state*/
                    Declare_DAC_Defective_US();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
                else
                {
                 break;
                }
               }
            if (Relay_A_Info.LU1_Fwd_Count  == Relay_A_Info.US1_IN_Count  &&
                Relay_A_Info.LU2_Fwd_Count  == Relay_A_Info.US2_IN_Count  &&
                Relay_A_Info.LU1_Rev_Count  == Relay_A_Info.US1_OUT_Count &&
                Relay_A_Info.LU2_Rev_Count  == Relay_A_Info.US2_OUT_Count )
            {
                if (Relay_A_Info.LU1_Fwd_Count  == Relay_A_Info.LU2_Fwd_Count &&
                    Relay_A_Info.US1_IN_Count   == Relay_A_Info.US2_IN_Count  &&
                    Relay_A_Info.LU1_Rev_Count  == Relay_A_Info.LU2_Rev_Count &&
                    Relay_A_Info.US1_OUT_Count  == Relay_A_Info.US2_OUT_Count )
                {
                /* Train cleared the section */
                    DeEnergise_Preparatory_Relay_A();           /* from relaydrv.c */
                    uiAuthorisationKey = Get_Relay_Energising_Key();
                    Energise_Vital_Relay_A(uiAuthorisationKey);
                    Clear_US_AxleCount();
                    Relay_A_Info.State = NO_TRAIN_IN_SECTION;
                    US_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;
                    US_Section_Mode.Remote_Unit = SYSTEM_CLEAR_MODE;
                    break;
                }
             }

            if(Relay_A_Info.US1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_A_Info.US2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_A_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_A_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
               {
                /* no in-counts, only out-counts,system goes to error state*/
                Declare_DAC_Defective_US();
                Status.Flags.Direct_Out_Count = SET_HIGH;
                Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                }
              break;
        case NO_TRAIN_IN_SECTION:
            if (Relay_A_Info.LU1_Fwd_Count  != Relay_A_Info.US1_IN_Count  ||
                Relay_A_Info.LU2_Fwd_Count  != Relay_A_Info.US2_IN_Count  ||
                Relay_A_Info.LU1_Rev_Count  != Relay_A_Info.US1_OUT_Count ||
                Relay_A_Info.LU2_Rev_Count  != Relay_A_Info.US2_OUT_Count )
            {
                DeEnergise_Vital_Relay_A();     /* from relaydrv.c */
                Relay_A_Info.State = TRAIN_IN_SECTION;
                US_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
                US_Section_Mode.Remote_Unit = SYSTEM_OCCUPIED_MODE;
                break;
            }
            if (Relay_A_Info.LU1_Fwd_Count  != Relay_A_Info.LU2_Fwd_Count ||
                Relay_A_Info.US1_IN_Count   != Relay_A_Info.US2_IN_Count  ||
                Relay_A_Info.LU1_Rev_Count  != Relay_A_Info.LU2_Rev_Count ||
                Relay_A_Info.US1_OUT_Count  != Relay_A_Info.US2_OUT_Count )
            {
                DeEnergise_Vital_Relay_A();     /* from relaydrv.c */
                Relay_A_Info.State = TRAIN_IN_SECTION;
                US_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
                US_Section_Mode.Remote_Unit = SYSTEM_OCCUPIED_MODE;
            }
            break;
        case TRAIN_IN_SECTION:
            if(Get_US_Local_Counts_Clearing_Status())
                {
                  if(Relay_A_Info.US1_Direction  == (BYTE)FORWARD_DIRECTION &&
                     Relay_A_Info.US2_Direction  == (BYTE)FORWARD_DIRECTION &&
                     Relay_A_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
                     Relay_A_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
                      {
                        if(Relay_A_Info.US_Local_Count > 0 )
                           {
                            Declare_DAC_Defective_US();
                             Status.Flags.Direct_Out_Count = SET_HIGH;
                            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                            break;
                        }
                    }
             }
            if(Relay_A_Info.US1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_A_Info.US2_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_A_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_A_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
            {
                if(Relay_A_Info.US_Local_Count > Relay_A_Info.US1_Axle_Count ||
                   Relay_A_Info.US_Local_Count > Relay_A_Info.US2_Axle_Count )
                   {
                    Declare_DAC_Defective_US();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
            }
            if (Relay_A_Info.LU1_Fwd_Count  == Relay_A_Info.US1_IN_Count &&
                Relay_A_Info.LU2_Fwd_Count  == Relay_A_Info.US2_IN_Count &&
                Relay_A_Info.LU1_Rev_Count  == Relay_A_Info.US1_OUT_Count &&
                Relay_A_Info.LU2_Rev_Count  == Relay_A_Info.US2_OUT_Count )
            {
                if (Relay_A_Info.LU1_Fwd_Count  == Relay_A_Info.LU2_Fwd_Count &&
                    Relay_A_Info.US1_IN_Count   == Relay_A_Info.US2_IN_Count &&
                    Relay_A_Info.LU1_Rev_Count  == Relay_A_Info.LU2_Rev_Count &&
                    Relay_A_Info.US1_OUT_Count  == Relay_A_Info.US2_OUT_Count )
                {
                /* Train cleared the section */
                    uiAuthorisationKey = Get_Relay_Energising_Key();
                    Energise_Vital_Relay_A(uiAuthorisationKey);
                    Relay_A_Info.State = NO_TRAIN_IN_SECTION;
                    Clear_US_AxleCount();
                    US_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;
                    US_Section_Mode.Remote_Unit = SYSTEM_CLEAR_MODE;
                    break;
                }
            }
            if(Relay_A_Info.US1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_A_Info.US2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_A_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_A_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
              {
                /* no in-counts, only out-counts,system goes to error state*/
                Declare_DAC_Defective_US();
                Status.Flags.Direct_Out_Count = SET_HIGH;
                Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
              }
            break;
        case DAC_DEFECTIVE:
            break;
        case DAC_RESET_PROGRESS:
            break;
        default: 
            break;
    }
}

//15_03_10

void Update_3S_Relay_A_State(void)
{
 INT16 uiAuthorisationKey;

    switch (Relay_A_Info.State)
    {
        case RELAY_MANAGER_NOT_STARTED:
        case DAC_REPEATER_RELAY_ON:
        case DAC_REPEATER_RELAY_OFF:
        case RELAY_MANAGER_IDLE:
             break;
        case ATC_WAIT_FOR_REMOTE_CLEAR:
            if(Relay_A_Info.ATC_Remote1_State == ATC_READY_TO_CLEAR &&
               Relay_A_Info.ATC_Remote2_State == ATC_READY_TO_CLEAR)
            {
               Relay_A_Info.State = TRAIN_IN_SECTION;
            }
            break;
        case WAIT_FOR_PILOT_TRAIN:
            if (Relay_A_Info.LU1_Fwd_Count > 0 || Relay_A_Info.LU1_Rev_Count > 0 ||
                Relay_A_Info.LU2_Fwd_Count > 0 || Relay_A_Info.LU2_Rev_Count > 0 ||
                Relay_A_Info.US1_IN_Count  > 0 || Relay_A_Info.US2_IN_Count > 0 ||
                Relay_A_Info.US1_OUT_Count > 0 || Relay_A_Info.US2_OUT_Count > 0 )
            {
                Relay_A_Info.State = PILOT_TRAIN_IN_SECTION;
                //Clear_Line_on_LCD((BYTE) 3);
                Clear_Local_Reset_Flag();
                Clear_US_Remote_Reset_Flag();
            }
            break;
        case PILOT_TRAIN_IN_SECTION:
            if(Relay_A_Info.US1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_A_Info.US2_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_A_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_A_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
            {
                if(Relay_A_Info.US_Local_Count > Relay_A_Info.US1_Axle_Count ||
                   Relay_A_Info.US_Local_Count > Relay_A_Info.US2_Axle_Count )
                   {
                    Declare_DAC_Defective_US();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
            }
            if ((Relay_A_Info.LU1_Fwd_Count  < 2 ||  Relay_A_Info.LU2_Fwd_Count <2) &&
                (Relay_A_Info.LU1_Rev_Count  < 2 ||  Relay_A_Info.LU2_Rev_Count <2) )
               {
                if(Relay_A_Info.US1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
                   Relay_A_Info.US2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
                   Relay_A_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
                   Relay_A_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
                  {
                   /* no in-counts, only out-counts,system goes to error state*/
                    Declare_DAC_Defective_US();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
                else
                {
                 break;
                }
               }
            if (Relay_A_Info.LU1_Fwd_Count  == Relay_A_Info.US1_OUT_Count  &&
                Relay_A_Info.LU2_Fwd_Count  == Relay_A_Info.US2_OUT_Count  &&
                Relay_A_Info.LU1_Rev_Count  == Relay_A_Info.US1_IN_Count &&
                Relay_A_Info.LU2_Rev_Count  == Relay_A_Info.US2_IN_Count )
            {
                if (Relay_A_Info.LU1_Fwd_Count  == Relay_A_Info.LU2_Fwd_Count &&
                    Relay_A_Info.US1_IN_Count   == Relay_A_Info.US2_IN_Count  &&
                    Relay_A_Info.LU1_Rev_Count  == Relay_A_Info.LU2_Rev_Count &&
                    Relay_A_Info.US1_OUT_Count  == Relay_A_Info.US2_OUT_Count )
                {
                /* Train cleared the section */
                    DeEnergise_Preparatory_Relay_A();           /* from relaydrv.c */
                    uiAuthorisationKey = Get_Relay_Energising_Key();
                    Energise_Vital_Relay_A(uiAuthorisationKey);
                    Clear_US_AxleCount();
                    Relay_A_Info.State = NO_TRAIN_IN_SECTION;
                    US_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;
                    US_Section_Mode.Remote_Unit = SYSTEM_CLEAR_MODE;
                    break;
                }
             }

            if(Relay_A_Info.US1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_A_Info.US2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_A_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_A_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
               {
                /* no in-counts, only out-counts,system goes to error state*/
                Declare_DAC_Defective_US();
                Status.Flags.Direct_Out_Count = SET_HIGH;
                Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                }
              break;
        case NO_TRAIN_IN_SECTION:
            if (Relay_A_Info.LU1_Fwd_Count  != Relay_A_Info.US1_OUT_Count  ||
                Relay_A_Info.LU2_Fwd_Count  != Relay_A_Info.US2_OUT_Count  ||
                Relay_A_Info.LU1_Rev_Count  != Relay_A_Info.US1_IN_Count ||
                Relay_A_Info.LU2_Rev_Count  != Relay_A_Info.US2_IN_Count )
            {
                DeEnergise_Vital_Relay_A();     /* from relaydrv.c */
                Relay_A_Info.State = TRAIN_IN_SECTION;
                US_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
                US_Section_Mode.Remote_Unit = SYSTEM_OCCUPIED_MODE;
                break;
            }
            if (Relay_A_Info.LU1_Fwd_Count  != Relay_A_Info.LU2_Fwd_Count ||
                Relay_A_Info.US1_IN_Count   != Relay_A_Info.US2_IN_Count  ||
                Relay_A_Info.LU1_Rev_Count  != Relay_A_Info.LU2_Rev_Count ||
                Relay_A_Info.US1_OUT_Count  != Relay_A_Info.US2_OUT_Count )
            {
                DeEnergise_Vital_Relay_A();     /* from relaydrv.c */
                Relay_A_Info.State = TRAIN_IN_SECTION;
                US_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
                US_Section_Mode.Remote_Unit = SYSTEM_OCCUPIED_MODE;
            }
            break;
        case TRAIN_IN_SECTION:
            if(Get_US_Local_Counts_Clearing_Status())
                {
                  if(Relay_A_Info.US1_Direction  == (BYTE)REVERSE_DIRECTION &&
                     Relay_A_Info.US2_Direction  == (BYTE)REVERSE_DIRECTION &&
                     Relay_A_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
                     Relay_A_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
                      {
                        if(Relay_A_Info.US_Local_Count > 0 )
                           {
                            Declare_DAC_Defective_US();
                             Status.Flags.Direct_Out_Count = SET_HIGH;
                            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                            break;
                        }
                    }
             }
            if(Relay_A_Info.US1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_A_Info.US2_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_A_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_A_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
            {
                if(Relay_A_Info.US_Local_Count > Relay_A_Info.US1_Axle_Count ||
                   Relay_A_Info.US_Local_Count > Relay_A_Info.US2_Axle_Count )
                   {
                    Declare_DAC_Defective_US();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
            }
            if (Relay_A_Info.LU1_Fwd_Count  == Relay_A_Info.US1_OUT_Count &&
                Relay_A_Info.LU2_Fwd_Count  == Relay_A_Info.US2_OUT_Count &&
                Relay_A_Info.LU1_Rev_Count  == Relay_A_Info.US1_IN_Count &&
                Relay_A_Info.LU2_Rev_Count  == Relay_A_Info.US2_IN_Count )
            {
                if (Relay_A_Info.LU1_Fwd_Count  == Relay_A_Info.LU2_Fwd_Count &&
                    Relay_A_Info.US1_IN_Count   == Relay_A_Info.US2_IN_Count &&
                    Relay_A_Info.LU1_Rev_Count  == Relay_A_Info.LU2_Rev_Count &&
                    Relay_A_Info.US1_OUT_Count  == Relay_A_Info.US2_OUT_Count )
                {
                /* Train cleared the section */
                    uiAuthorisationKey = Get_Relay_Energising_Key();
                    Energise_Vital_Relay_A(uiAuthorisationKey);
                    Relay_A_Info.State = NO_TRAIN_IN_SECTION;
                    Clear_US_AxleCount();
                    US_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;
                    US_Section_Mode.Remote_Unit = SYSTEM_CLEAR_MODE;
                    break;
                }
            }
            if(Relay_A_Info.US1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_A_Info.US2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_A_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_A_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
              {
                /* no in-counts, only out-counts,system goes to error state*/
                Declare_DAC_Defective_US();
                Status.Flags.Direct_Out_Count = SET_HIGH;
                Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
              }
            break;
        case DAC_DEFECTIVE:
            break;
        case DAC_RESET_PROGRESS:
            break;
        default:
            break;

    }


}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :BOOL Reset_Allowed_For_US()
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Function return TRUE if the system is defective or when it has
                     Forward/Reverse direction of axles which means that you can reset the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5088

Interfaces
    Calls           :   None

    Called by       :   RESET.C-Process_CF_Reset_Command()
                        RESET.C-EF_Wait_for_Local_Reset()

Input Variables                     Name                Type
    Global          :   DAC_DEFECTIVE                   Enumerator
                        Relay_A_Info.State              Enumerator
                        Relay_A_Info.US1_Direction      Byte
                        FORWARD_DIRECTION               Enumerator
                        Relay_A_Info.US2_Direction      Byte
                        Relay_A_Info.LU1_Direction      Byte
                        Relay_A_Info.LU2_Direction      Byte
                        REVERSE_DIRECTION               Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   None

    Local           :   ReturnValue                     BOOL

Signal Variables


Macros defined      :                   Macro                   Value
                                        FALSE                     0
                                        TRUE                      1
References          :

Derived Requirements:

Algorithm           :1.Check the relay A info state and if it is in defective state return high
                     2.If both the local and upstream axle direction are setted in forward return high
                     3.If both the local and upstream axle direction are setted in reverse return high
*******************************************************************************/
BOOL Reset_Allowed_For_US(void)
{
    BOOL ReturnValue = FALSE;

    if( Relay_A_Info.State == DAC_DEFECTIVE)
    {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
    }

    if(Relay_A_Info.US1_Direction  == (BYTE)FORWARD_DIRECTION &&
       Relay_A_Info.US2_Direction  == (BYTE)FORWARD_DIRECTION &&
       Relay_A_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
       Relay_A_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
    {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
    }

    if (Relay_A_Info.US1_Direction == (BYTE)REVERSE_DIRECTION &&
       Relay_A_Info.US2_Direction  == (BYTE)REVERSE_DIRECTION &&
       Relay_A_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
       Relay_A_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION)
    {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
    }
       return(ReturnValue);
}



/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Process_Relay_A_Local_Direction(BYTE uchDirection)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit axle direction in CPU1 if CPU1 flag is HIGH
                     else update the direction in CPU2 & update the local axle counts


Allocated Requirements:

Design Requirements:SSDAC_DR_5089

Interfaces
    Calls           :   RLYA_MGR.C-Update_A_LU1_Direction()
                        RLYA_MGR.C-Update_A_LU2_Direction()
                        RLYA_MGR.C-Update_A_Local_AxleCount()

    Called by       :   RELAYMGR.C-Update_Relay_A_Counts()


Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_CPU1   Bit

    Local           :   uchDirection                    BYTE
                        uiAxleCount                     UINT16

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1
References          :

Derived Requirements:

Algorithm           :1.Update the local axle direction of CPU1 if the dip switch info flag CPU1 is high
                        otherwise update the axle direction of CPU2
*******************************************************************************/
void Process_Relay_A_Local_Direction(BYTE uchDirection,UINT16 uiAxleCount)
{

    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
    {
        Update_A_LU1_Direction((BYTE) uchDirection);
    }
    else
    {
        Update_A_LU2_Direction((BYTE) uchDirection);
    }
        Update_A_Local_AxleCount(uiAxleCount);
}

/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Process_Relay_A_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit axle counts (both forward & reverse) in CPU1 if
                     CPU1 flag is HIGH else update the direction in CPU2


Allocated Requirements:

Design Requirements:SSDAC_DR_5090

Interfaces
    Calls           :   RLYA_MGR.C-Update_A_LU1_Fwd_Count()
                        RLYA_MGR.C-Update_A_LU1_Rev_Count()
                        RLYA_MGR.C-Update_A_LU2_Fwd_Count()
                        RLYA_MGR.C-Update_A_LU2_Rev_Count()

    Called by       :   RELAYMGR.C-Update_Relay_A_Counts()


Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_CPU1   Bit

    Local           :   uiFwdAxleCount                  UINT16
                        uiRevAxleCount                  UINT16

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1
References          :

Derived Requirements:

Algorithm           :1.If the dip switch info flag CPU1 is high update the CPU1 forward and reverse axle counts
                       otherwise update the CPU2 forward and reverse axle counts
*******************************************************************************/
void Process_Relay_A_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{
    if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
    {
        Update_A_LU1_Fwd_Count(uiFwdAxleCount);
        Update_A_LU1_Rev_Count(uiRevAxleCount);
    }
    else
    {
        Update_A_LU2_Fwd_Count(uiFwdAxleCount);
        Update_A_LU2_Rev_Count(uiRevAxleCount);
    }
}

/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void  Process_Peer_Relay_A_Direction( BYTE uchDirection)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit axle direction in CPU2 if CPU2 flag is HIGH
                     else update the direction in CPU1

Allocated Requirements:

Design Requirements: 	SSDAC_DR_5091

Interfaces
    Calls           :   RLYA_MGR.C-Update_A_LU2_Direction()
                        RLYA_MGR.C-Update_A_LU1_Direction()

    Called by       :   COMM_SM.C-Process_Interprocess_Message()


Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_CPU1   Bit

    Local           :   uchDirection                    BYTE

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1
References          :

Derived Requirements:

Algorithm           :1.If it is CPU1 update the peer CPU (ie.CPU2) axle direction
                     2.If it is CPU2 update the peer CPU (ie.CPU1) axle direction
*******************************************************************************/
void  Process_Peer_Relay_A_Direction( BYTE uchDirection)
{
    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
    {
        Update_A_LU2_Direction((BYTE) uchDirection);
    }
    else
    {
        Update_A_LU1_Direction((BYTE) uchDirection);
    }
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Process_Peer_Relay_A_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit axle counts (both forward & reverse) in CPU2 if
                     CPU2 flag is HIGH else update the direction in CPU1

Allocated Requirements:

Design Requirements:	SSDAC_DR_5092


Interfaces
    Calls           :   RLYA_MGR.C-Update_A_LU1_Fwd_Count()
                        RLYA_MGR.C-Update_A_LU1_Rev_Count()
                        RLYA_MGR.C-Update_A_LU2_Fwd_Count()
                        RLYA_MGR.C-Update_A_LU2_Rev_Count()

    Called by       :   COMM_SM.C-Process_Interprocess_Message()


Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_CPU1   Bit

    Local           :   uiFwdAxleCount                  UINT16
                        uiRevAxleCount                  UINT16

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1

References          :

Derived Requirements:

Algorithm           :1.If it is CPU1 update the peer CPU (ie.CPU2) forward and reverse axle counts
                     2.If it is CPU2 update the peer CPU (ie.CPU1) forward and reverse axle counts
*******************************************************************************/

void Process_Peer_Relay_A_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{

    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
    {
        Update_A_LU2_Fwd_Count(uiFwdAxleCount);
        Update_A_LU2_Rev_Count(uiRevAxleCount);
    }
    else
    {
        Update_A_LU1_Fwd_Count(uiFwdAxleCount);
        Update_A_LU1_Rev_Count(uiRevAxleCount);
    }
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_Next_Block_State(bitadrb_t SrcAdr, BYTE Next_Block_Status)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update next block status in CPU1, if CPU1 flag bit for the remote unit is set high else
                     update it in CPU2


Allocated Requirements:

Design Requirements:	SSDAC_DR_5093

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_Axle_Count_Message()


Input Variables                     Name                    Type
    Global          :   None

    Local           :   SrcAdr                              bitadrb_t
                        Next_Block_Status                   BYTE
                        SrcAdr.Bit.b0                       Bit

Output Variables                    Name                    Type
    Global          :   Relay_A_Info.Next_Block_CPU1_Status Byte
                        Relay_A_Info.Next_Block_CPU2_Status Byte

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1

References          :

Derived Requirements:

Algorithm           :1.Update the CPU1 status of the next block if the message is received from the remote unit CPU1
                        otherwise update the status of the CPU2 of the next block.
*******************************************************************************/
void Update_Next_Block_State(bitadrb_t SrcAdr, BYTE Next_Block_Status)
{
    if (SrcAdr.Bit.b0 == SET_HIGH)
    {
        /* From Remote Unit - CPU1 */
        Relay_A_Info.Next_Block_CPU1_Status = Next_Block_Status;
    }
    else
    {
        /* From Remote Unit - CPU2 */
        Relay_A_Info.Next_Block_CPU2_Status = Next_Block_Status;
    }
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_US1_OUT_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Remote unit CPU1 Out count required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5094

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_AxleCount()


Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US1_OUT_Count      UINT16

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the upstream CPU1 OUT count
*******************************************************************************/
void Update_A_US1_OUT_Count(UINT16 uiCount)
{
    Relay_A_Info.US1_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_US2_OUT_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Remote unit CPU2 Out count required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5095

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US2_OUT_Count      UINT16

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the upstream CPU2 OUT count
*******************************************************************************/
void Update_A_US2_OUT_Count(UINT16 uiCount)
{
    Relay_A_Info.US2_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_US1_IN_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Remote Unit CPU1 In count required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5096

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_AxleCount()


Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US1_IN_Count       UINT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the upstream CPU1 IN count
*******************************************************************************/
void Update_A_US1_IN_Count(UINT16 uiCount)
{
    Relay_A_Info.US1_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_US2_IN_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Remote Unit CPU2 In count required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5097

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US2_IN_Count       UINT16

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the upstream CPU2 IN count
*******************************************************************************/
void Update_A_US2_IN_Count(UINT16 uiCount)
{
    Relay_A_Info.US2_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_LU1_Fwd_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the Forward count in Local Unit CPU1 required to monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5098

Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Process_Relay_A_Local_AxleCount()
                        RLYA_MGR.C-Process_Peer_Relay_A_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.LU1_Fwd_Count      UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the local CPU1 forward axle count
*******************************************************************************/
void Update_A_LU1_Fwd_Count(UINT16 uiCount)
{
    Relay_A_Info.LU1_Fwd_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_LU2_Fwd_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Local Unit CPU2 Forward count required to monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5099
Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Process_Relay_A_Local_AxleCount()
                        RLYA_MGR.C-Process_Peer_Relay_A_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.LU2_Fwd_Count      UINT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the local CPU2 forward axle count
*******************************************************************************/
void Update_A_LU2_Fwd_Count(UINT16 uiCount)
{
    Relay_A_Info.LU2_Fwd_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_LU1_Rev_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Local Unit CPU1 Reverse count required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5100


Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Process_Relay_A_Local_AxleCount()
                        RLYA_MGR.C-Process_Peer_Relay_A_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.LU1_Rev_Count      UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the local CPU1 reverse axle count
*******************************************************************************/
void Update_A_LU1_Rev_Count(UINT16 uiCount)
{
    Relay_A_Info.LU1_Rev_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_LU2_Rev_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Reverse count Local Unit CPU2 required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5101

Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Process_Relay_A_Local_AxleCount()
                        RLYA_MGR.C-Process_Peer_Relay_A_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.LU2_Rev_Count      UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the local CPU2 reverse axle count
*******************************************************************************/
void Update_A_LU2_Rev_Count(UINT16 uiCount)
{
    Relay_A_Info.LU2_Rev_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_Local_AxleCount(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Up stream local counts required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5102


Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Process_Relay_A_Local_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US_Local_Count     UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the upstrean local axle count
*******************************************************************************/
void Update_A_Local_AxleCount(UINT16 uiCount)
{
    Relay_A_Info.US_Local_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_US1_AxleCount(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update remote unit CPU1 axle counts required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5103

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Direction()


Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US1_Axle_Count     UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :Update the upstream CPU1 axle count
*******************************************************************************/
void Update_A_US1_AxleCount(UINT16 uiCount)
{
    Relay_A_Info.US1_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_US2_AxleCount(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update remote unit CPU2 axle counts required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5104

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Direction()


Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US2_Axle_Count     UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :Update the upstream CPU2 axle count
*******************************************************************************/
void Update_A_US2_AxleCount(UINT16 uiCount)
{
    Relay_A_Info.US2_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_US1_Direction(BYTE Direction)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update remote Unit CPU1 direction(Forward/Reverse) required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5105

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US1_Direction      BYTE

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the upstream CPU1 axle direction
*******************************************************************************/
void Update_A_US1_Direction(BYTE Direction)
{
    Relay_A_Info.US1_Direction = Direction;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_US2_Direction(BYTE Direction)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Remote Unit CPU2 Direction(Forward/Reverse) required to monitor the system


Allocated Requirements:

Design Requirements:SSDAC_DR_5106

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_A_Info.US2_Direction      BYTE

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the upstream CPU2 axle direction
*******************************************************************************/
void Update_A_US2_Direction(BYTE Direction)
{
    Relay_A_Info.US2_Direction = Direction;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_LU1_Direction(BYTE Direction)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update Local unit CPU1 Direction(Forward/Reverse)required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5107

Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Process_Relay_A_Local_Direction()
                        RLYA_MGR.C-Process_Peer_Relay_A_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_A_Info.LU1_Direction      BYTE

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the local CPU1 axle direction
*******************************************************************************/
void Update_A_LU1_Direction(BYTE Direction)
{
    Relay_A_Info.LU1_Direction = Direction;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_A_LU2_Direction(BYTE Direction)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Upadte Local Unit CPU2 direction(Forward/Reverse)required to
                     monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5372

Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Process_Relay_A_Local_Direction()
                        RLYA_MGR.C-Process_Peer_Relay_A_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_A_Info.LU2_Direction      BYTE

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Update the local CPU2 axle direction
*******************************************************************************/
void Update_A_LU2_Direction(BYTE Direction)
{
    Relay_A_Info.LU2_Direction = Direction;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_ATC_Remote1_Relay_A_State(BYTE Remote_state)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update remote unit CPU1 state for ATC configuration required to
                     monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5373

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Axle_Count_Message()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Remote_state                    BYTE

Output Variables                    Name                Type
    Global          :   Relay_A_Info.ATC_Remote1_State  BYTE

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :Update the relay A info ATC remote1 state
*******************************************************************************/
void Update_ATC_Remote1_Relay_A_State(BYTE Remote_state)
{
    Relay_A_Info.ATC_Remote1_State = Remote_state;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Update_ATC_Remote2_Relay_A_State(BYTE Remote_state)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update remote unit CPU2 state for ATC configuration required to
                     monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5374

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Axle_Count_Message()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Remote_state                    BYTE

Output Variables                    Name                Type
    Global          :   Relay_A_Info.ATC_Remote2_State  BYTE

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :Update the relay A info ATC remote2 state
*******************************************************************************/
void Update_ATC_Remote2_Relay_A_State(BYTE Remote_state)
{
    Relay_A_Info.ATC_Remote2_State = Remote_state;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Set_Relay_A_DAC_Defective(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :DeEnergise the Vital relay A and Declare DAC defective if any error
                     in the system

Allocated Requirements:(SSDAC_SWRS_0455)

Design Requirements:	SSDAC_DR_5375

Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Vital_Relay_A()

    Called by       :   RELAYMGR.C-Declare_DAC_Defective()
                        RELAYMGR.C-Declare_DAC_Defective_US()
                        RELAYMGR.C-Declare_DAC_Defective_DS()


Input Variables                     Name                Type
    Global          :   DAC_DEFECTIVE                   Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_A_Info.State              Enumerator

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Deenergise the vital relay A
                     2.Set the relay A info state as Defective
*******************************************************************************/
void Set_Relay_A_DAC_Defective(void)
{
    DeEnergise_Vital_Relay_A();
    Relay_A_Info.State = DAC_DEFECTIVE;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :BYTE Get_Relay_A_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get Relay A state to know wheather
                     1.The relay A is initialised
                     2.Waiting for pilot train
                     3.No train in section
                     4.Train in section
                     5.System is defective
                     6.System is waiting for reset
                     7.To know the status of repeat relay A

Allocated Requirements:

Design Requirements:	SSDAC_DR_5376

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Axle_Count_Message()
                        ERROR.C-Clear_Error_Display()
                        RELAYMGR.C-Update_CF_Track_Status()
                        RELAYMGR.C-Update_EF_Track_Status()
                        RESET.C-Process_CF_Reset_Command()
                        SYS_MON.C-Check_Communication_Links()


Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_A_Info.State              BYTE

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Return the relay A info state
*******************************************************************************/
BYTE Get_Relay_A_State(void)
{
 return((BYTE)Relay_A_Info.State);
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :BYTE Get_Relay_A_ATC_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get Relay A state for ATC configuration to know wheather
                     1.The relay A is initialised
                     2.Waiting for pilot train
                     3.No train in section
                     4.Train in section
                     5.System is defective
                     6.System is waiting for reset
                     7.To know the status of repeat relay A


Allocated Requirements:

Design Requirements:	SSDAC_DR_5377

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Build_US_Axle_Count_Message()

Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_A_Info.ATC_Local_State    Byte

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :Return the relay A info ATC local state
*******************************************************************************/
BYTE Get_Relay_A_ATC_State(void)
{
    return( Relay_A_Info.ATC_Local_State);
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Clear_ATC_Local_Relay_A_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear all the states of ATC configuration
                     to indicate that it is not ready to clear


Allocated Requirements:

Design Requirements:	SSDAC_DR_5378

Interfaces
    Calls           :   None

    Called by       :   RELAYMGR.C-Declare_DAC_Defective()
                        RELAYMGR.C-Declare_DAC_Defective_US()

Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_A_Info.ATC_Local_State    Byte
                        Relay_A_Info.ATC_Remote1_State  Byte
                        Relay_A_Info.ATC_Remote2_State  Byte


    Local           :   None

Signal Variables



Macros defined      :                   Macro                       Value
                                        ATC_NOT_READY_T0_CLEAR        0
References          :

Derived Requirements:

Algorithm           :1.Set the relay A info ATC local,remote1 and remote2  state as ATC NOT READY TO CLEAR
*******************************************************************************/
void Clear_ATC_Local_Relay_A_State(void)
{
  Relay_A_Info.ATC_Local_State = ATC_NOT_READY_T0_CLEAR;
  Relay_A_Info.ATC_Remote1_State =  ATC_NOT_READY_T0_CLEAR;
  Relay_A_Info.ATC_Remote2_State =  ATC_NOT_READY_T0_CLEAR;
}
/******************************************************************************
Component name      :RLYA_MGR
Module Name         :void Clear_Relay_A_Counts(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the local,remote,IN count & OUT counts

Allocated Requirements:

Design Requirements:	SSDAC_DR_5379

Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Start_Relay_A_Mgr()


Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_A_Info.LU1_Fwd_Count      UINT16
                        Relay_A_Info.LU2_Fwd_Count      UINT16
                        Relay_A_Info.LU1_Rev_Count      UINT16
                        Relay_A_Info.LU2_Rev_Count      UINT16
                        Relay_A_Info.US1_IN_Count       UINT16
                        Relay_A_Info.US2_IN_Count       UINT16
                        Relay_A_Info.US1_OUT_Count      UINT16
                        Relay_A_Info.US2_OUT_Count      UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Clear the relay A info local CPU1 and CPU2 forward counts
                     2.Clear the relay A info local CPU1 and CPU2 reverse counts
                     3.Clear the relay A info upstream CPU1 and CPU2 IN counts
                     4.Clear the relay A info upstream CPU1 and CPU2 OUT counts
*******************************************************************************/
void Clear_Relay_A_Counts(void)
{

    
    /*clear local counts */
     Relay_A_Info.LU1_Fwd_Count = 0;
     Relay_A_Info.LU2_Fwd_Count = 0;
     Relay_A_Info.LU1_Rev_Count = 0;
     Relay_A_Info.LU2_Rev_Count = 0;
     Relay_A_Info.US1_IN_Count  = 0;
     Relay_A_Info.US2_IN_Count  = 0;
     Relay_A_Info.US1_OUT_Count = 0;
     Relay_A_Info.US2_OUT_Count = 0;
 

}

