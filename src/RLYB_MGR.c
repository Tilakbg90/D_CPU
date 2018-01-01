/******************************************************************************
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
    Functions   :   void Initialise_Relay_B_Mgr(void)
                    void Start_Relay_B_Mgr(void)
                    void Set_Relay_B_Reset_State(void)
                    void Update_Relay_B_State(void)
                    BOOL Reset_Allowed_For_DS()
                    void Process_Relay_B_Local_Direction(BYTE uchDirection)
                    void Process_Relay_B_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void Process_Peer_Relay_B_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void Process_Peer_Relay_B_Direction( BYTE uchDirection)
                    void Update_Previous_Block_State(bitadrb_t SrcAdr, BYTE Prev_Block_Status)
                    void Update_B_DS1_OUT_Count(UINT16 uiCount)
                    void Update_B_DS2_OUT_Count(UINT16 uiCount)
                    void Update_B_LU1_Fwd_Count(UINT16 uiCount)
                    void Update_B_LU2_Fwd_Count(UINT16 uiCount)
                    void Update_B_LU1_Rev_Count(UINT16 uiCount)
                    void Update_B_LU2_Rev_Count(UINT16 uiCount)
                    void Update_B_Local_AxleCount(UINT16 uiCount)
                    void Update_B_DS1_AxleCount(UINT16 uiCount)
                    void Update_B_DS2_AxleCount(UINT16 uiCount)
                    void Update_B_DS1_IN_Count(UINT16 uiCount)
                    void Update_B_DS2_IN_Count(UINT16 uiCount)
                    void Update_B_DS1_Direction(BYTE Direction)
                    void Update_B_DS2_Direction(BYTE Direction)
                    void Update_B_LU1_Direction(BYTE Direction)
                    void Update_B_LU2_Direction(BYTE Direction)
                    void Update_ATC_Remote1_Relay_B_State(BYTE Remote_state)
                    void Update_ATC_Remote2_Relay_B_State(BYTE Remote_state)
                    void Set_Relay_B_DAC_Defective(void)
                    BYTE Get_Relay_B_State(void)
                    BYTE Get_Relay_B_ATC_State(void)
                    void Clear_ATC_Local_Relay_B_State(void)
                    void Clear_Relay_B_Counts(void)

******************************************************************************/
#include "COMMON.h"
#include "RELAYDRV.h"
#include "RLYB_MGR.h"
#include "RESET.h"
#include "SYS_MON.h"
#include "ERROR.h"

extern  /*near*/  dac_status_t Status;                    /* from DAC_MAIN.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from DAC_MAIN.c */
relay_b_info_t Relay_B_Info;
extern ds_section_mode DS_Section_Mode;             /*from DAC_MAIN.c*/


extern relay_a_info_t Relay_A_Info;

void Clear_Relay_B_Counts(void);


void Update_B_LU1_Fwd_Count(UINT16 uiCount);
void Update_B_LU2_Fwd_Count(UINT16 uiCount);
void Update_B_LU1_Rev_Count(UINT16 uiCount);
void Update_B_LU2_Rev_Count(UINT16 uiCount);
void Update_B_LU1_Direction(BYTE Direction);
void Update_B_LU2_Direction(BYTE Direction);
void Update_B_Local_AxleCount(UINT16 uiCount);
BOOL Reset_Allowed_For_LCWS(void);
BOOL Reset_Allowed_For_DE(void);

/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Initialise_Relay_B_Mgr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initialise relay B manager

Allocated Requirements: (SSDAC_SWRS_0032)

Design Requirements:	SSDAC_DR_5108

Interfaces
    Calls           :   None

    Called by       :   RELAYMGR.C-Initialise_Relay_Mgr()

Input Variables                     Name                Type
    Global          :   RELAY_MANAGER_NOT_STARTED       Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_B_Info.State              Enumerator

    Local           :   None

Signal Variables

References          :

Derived Requirements:


*******************************************************************************/
void Initialise_Relay_B_Mgr(void)
{
    Relay_B_Info.State = RELAY_MANAGER_NOT_STARTED;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Start_Relay_B_Mgr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the relay B manager

Allocated Requirements:(SSDAC_SWRS_0223)

Design Requirements:	SSDAC_DR_5109
Interfaces
    Calls           :   AXLE_MON.C-Clear_DS_AxleCount()
                        AXLE_MON.C-Clear_DS_Local_Counts()
                        RLYB_MGR.C-Clear_Relay_B_Counts()
                        RESET.C-Clear_Reset_Info()
                        ERROR.C-Clear_Error_Display()

    Called by       :   DAC_MAIN.C-main()
                        RLYB_MGR.C-Update_Relay_B_State()

Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.ATC_Enabled   Bit
                        ATC_WAIT_FOR_REMOTE_CLEAR           Enumerator
                        WAIT_FOR_PILOT_TRAIN                Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_B_Info.State              Enumerator
                        Relay_B_Info.ATC_Local_State    Byte

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        TRUE                      1
References          :

Derived Requirements:





Algorithm           :1.Clear the down stream axle direction, Supervisory counts, wheel type
                     2.Clear the forward & reverse down stream & local counts axle counts
                     3.Clear the reset state
                     4.Clear all the errors if set
                     5.If the system configuration is set to ATC configuration than wait for the
                       remote end to clear
                     6.If the system is not configured to ATC configuration than make the system
                       wait for the pilot train
*******************************************************************************/
void Start_Relay_B_Mgr(void)
{
    if (Status.Flags.System_Status != NORMAL || Status.Flags.DS_System_Status != NORMAL )
    {
        return;
    }
    Clear_DS_AxleCount();
    Clear_DS_Local_Counts();             /*clear local counts */
    Clear_Relay_B_Counts();
    Clear_Reset_Info();                 /* Clear the reset flags and reset state */
    Clear_Error_Display();              /* Clear Errors set in Minor & Major Errors */

    if( DIP_Switch_Info.Flags.ATC_Enabled == TRUE)
    {
        Relay_B_Info.State = ATC_WAIT_FOR_REMOTE_CLEAR;
        Relay_B_Info.ATC_Local_State = ATC_READY_TO_CLEAR;
    }
    else
    {
        Energise_Preparatory_Relay_B();
        Relay_B_Info.State = WAIT_FOR_PILOT_TRAIN;
    }
}

/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Set_Relay_B_Reset_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Set relay B reset state

Allocated Requirements:

Design Requirements:SSDAC_DR_5111

Interfaces
    Calls           :   AXLE_MON.C-Clear_DS_Local_Counts()
                        AXLE_MON.C-Stop_DS_Axle_Counting()
                        SYS_MON.C-Clear_Comm_DS_Failures()
                        SYS_MON.C-Clear_DS_Power_Status()
                        ERROR.c -  Clear_DS_Error_Code()

    Called by       :   RESET.C-Process_CF_Reset_Command()

Input Variables             Name                        Type
    Global          :   DAC_RESET_PROGRESS              Enumerator

    Local           :   None

Output Variables            Name                        Type
    Global          :   Relay_B_Info.State              Enumerator
                        Status.Flags.DS_System_Status   Bit


    Local           :   None
Signal Variables


Macros defined      :   Macro                   Value
                        TRUE                      1

References          :

Derived Requirements:


Algorithm           :If the DAC system is in reset progress state, then clear all the down
                     stream local & remote counts, set all the communication links related to down stream
                     as OK & set all the old & new power status related to down stream as OK
*******************************************************************************/
void Set_Relay_B_Reset_State(void)
{
    Relay_B_Info.State = DAC_RESET_PROGRESS;
    Status.Flags.DS_System_Status = TRUE;
    Clear_DS_Local_Counts();             /*clear local counts */
    Stop_DS_Axle_Counting();
    Clear_Comm_DS_Failures();
    Clear_DS_Power_Status();
    Clear_DS_Error_Code();
    DeEnergise_Preparatory_Relay_B();
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_Relay_B_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the relay B state.

Allocated Requirements: (SSDAC_SWRS_0035), (SSDAC_SWRS_0200),(SSDAC_SWRS_0458), (SSDAC_SWRS_0459),
						(SSDAC_SWRS_0521),(SSDAC_SWRS_0563), (SSDAC_SWRS_0223), (SSDAC_SWRS_0225),
						(SSDAC_SWRS_0263), (SSDAC_SWRS_0269), (SSDAC_SWRS_0271), (SSDAC_SWRS_0234),
						(SSDAC_SWRS_0212), (SSDAC_SWRS_0319), (SSDAC_SWRS_0320), (SSDAC_SWRS_0359)
						(SSDAC_SWRS_0622), (SSDAC_SWRS_0721), (SSDAC_SWRS_0655),(SSDAC_SWRS_0755)
						(SSDAC_SWRS_0363), (SSDAC_SWRS_0324), (SSDAC_SWRS_0364), (SSDAC_SWRS_0373)
						(SSDAC_SWRS_0374), (SSDAC_SWRS_0375), (SSDAC_SWRS_0376), (SSDAC_SWRS_0377)
						(SSDAC_SWRS_0328), (SSDAC_SWRS_0330), (SSDAC_SWRS_0312), (SSDAC_SWRS_0836)
				
					  					   
Design Requirements:	SSDAC_DR_5112


Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Preparatory_Relay()
                        RESET.C-Clear_Local_Reset_Flag()
                        LCD_DRV.C-Clear_Line_on_LCD()
                        RELAYDRV.C-Energise_Preparatory_Relay()
                        ERROR.C-Set_Error_Status_Bit()
                        RELAYMGR.C-Declare_DAC_Defective_DS()
                        RELAYDRV.C-Get_Relay_Energising_Key()
                        RELAYDRV.C-Energise_Vital_Relay_B()
                        AXLE_MON.C-Clear_DS_AxleCount()
                        RELAYDRV.C-DeEnergise_Vital_Relay_B()
                        RESET.C-Get_CF_Reset_State()
                        RLYB_MGR.C-Start_Relay_B_Mgr()
                        AXLE_MON.C-Start_DS_Axle_Counting()
                        RELAYDRV.C-DeEnergise_Vital_Relay_B()

    Called by       :   DAC_MAIN.C-Control_DAC_Type_EF()
                        RELAYMGR.C-Update_Relay_B_Counts()

Input Variables                         Name                Type
    Global          :   Relay_B_Info.ATC_Remote1_State      Byte
                        Relay_B_Info.ATC_Remote2_State      Byte
                        TRAIN_IN_SECTION                    Enumerator
                        Relay_B_Info.LU1_Fwd_Count          UINT16
                        Relay_B_Info.LU2_Fwd_Count          UINT16
                        Relay_B_Info.DS1_IN_Count           UINT16
                        Relay_B_Info.DS1_OUT_Count          UINT16
                        Relay_B_Info.LU1_Rev_Count          UINT16
                        Relay_B_Info.LU2_Rev_Count          UINT16
                        Relay_B_Info.DS2_IN_Count           UINT16
                        Relay_B_Info.DS2_OUT_Count          UINT16
                        PILOT_TRAIN_IN_SECTION              Enumerator
                        WAIT_FOR_PILOT_TRAIN                Enumertor
                        Relay_B_Info.LU1_Direction          BYTE
                        Relay_B_Info.LU2_Direction          BYTE
                        Relay_B_Info.DS1_Direction          BYTE
                        Relay_B_Info.DS2_Direction          BYTE
                        DIRECTION_NOT_DEFINED               Enumerator
                        REVERSE_DIRECTION                   Enumerator
                        NO_TRAIN_IN_SECTION                 Enumerator
                        CF_DS_RESET_SEQUENCE_COMPLETED      Enumerator
                        CF_US_RESET_SEQUENCE_COMPLETED      Enumerator
                        DIP_Switch_Info.Flags.ATC_Enabled   Bit
                        Relay_B_Info.Previous_Block_CPU1_Status Byte
                        Relay_B_Info.Previous_Block_CPU2_Status Byte
                        DAC_REPEATER_RELAY_OFF              Enumerator
                        DAC_REPEATER_RELAY_ON               Enumerator
                        Relay_B_Info.State                  Enumerator

    Local           :   uiAuthorisationKey              UINT16
                        uchState                        BYTE

Output Variables                    Name                Type
    Global          :   Relay_B_Info.State              Enumerator
                        Status.Flags.Direct_Out_Count   Bit

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        FALSE                     0
                                        ATC_READY_TO_CLEAR        1
                                        SET_HIGH                  1
                                        DIRECT_OUT_ERROR_NUM       43
                                        TRACK_CLEAR               1
References          :

Derived Requirements:


Algorithm           :1. If the system is configured to ATC configuration then the system shall check
                        the remote units to be clear so that it can allow the train in section
                     2. If the system has started receiving the forward / reverse counts  or IN counts
                        /OUT counts indicate that pilot train has enter the section,make the system to
                        come out of reset mode & clear the 3rd line of LCD and clear the Local reset flags
                     3. If train enters in reverse direction at EF and if EF local axle count is more than
                        remote unit axle count,declare DAC defective,set Direct out count error ID.
                        [If pilot train enters in reverse direction if SF unit count is greater than EF
                        unit axle count then declare it as DAC defective, Set direct out count error ID]
                     4. If the system has received the counts of the piolt train but the direction is not
                        defined or system has received only OUT counts but no IN counts then system has
                        to go to error state then declare as DAC defective & set the error bit to indicate
                        the error
                     5. Since the system requires minimum of 2 counts to come out of pilot mode, check
                        wheather it has received more than two counts if not just return with the system
                        in pilot mode waiting for the pilot train
                     6. If the system has started receiving the counts of the pilot train and the counts
                        match at both local & remote unit then deenergise the preparatory relay B, get the
                        security key to energise the Vital relay B to indicate that DS track is clear &
                        there is no pilot train in the section
                     7.If the system has started receiving the counts and the counts are not matched at
                        local & remote unit thae allow the Vital relay B to be in de-energised state to
                        indicate that DS track is occupied & there is train the section
                     8. If train enters in reverse direction at EF and if EF local axle count is more than
                        remote unit axle count,declare DAC defective,set Direct out count error ID.[If train
                        enters in reverse direction if SF unit count is greater than EF unit axle count then
                        declare it as DAC defective, Set direct out count error ID]
                     9. If the system has received the counts of the train but the direction is not defined
                       or system has received only OUT counts but no IN counts then system has to go to error
                        state then declare as DAC defective & set the error bit to indicate the error
                     10.If the system has started receiving the counts of the train and the counts matchs at
                        both local & remote unit than get the security key to energise the Vital relay B to
                        indicate that DS track is clear & there is no train in the section
                     11.If the repeater relay is ON a than check the status of the previous block if its not
                        clear than de-energise the repeater relay to indicate that track is not clear for the
                        previous block
                     12.If the repeater relay is OFF check the status of the previous block if its clear than
                        get the security key to energise the vital relay B to indicate that track is clear
*******************************************************************************/
void Update_Relay_B_State(void)
{
    INT16 uiAuthorisationKey;

    switch (Relay_B_Info.State)
    {
        case RELAY_MANAGER_NOT_STARTED:
        case DAC_REPEATER_RELAY_ON:
        case DAC_REPEATER_RELAY_OFF:
        case RELAY_MANAGER_IDLE:
             break;
        case ATC_WAIT_FOR_REMOTE_CLEAR:
            if(Relay_B_Info.ATC_Remote1_State == ATC_READY_TO_CLEAR &&
               Relay_B_Info.ATC_Remote2_State == ATC_READY_TO_CLEAR)
            {
               Relay_B_Info.State = TRAIN_IN_SECTION;
            }
            break;
        case WAIT_FOR_PILOT_TRAIN:
            if (Relay_B_Info.LU1_Fwd_Count > 0 || Relay_B_Info.LU1_Rev_Count > 0 ||
                Relay_B_Info.LU2_Fwd_Count > 0 || Relay_B_Info.LU2_Rev_Count > 0 ||
                Relay_B_Info.DS1_IN_Count  > 0 || Relay_B_Info.DS2_IN_Count > 0 ||
                Relay_B_Info.DS1_OUT_Count > 0 || Relay_B_Info.DS2_OUT_Count > 0 )
            {
                Relay_B_Info.State = PILOT_TRAIN_IN_SECTION;
                Clear_Local_Reset2_Flag();
                Clear_DS_Remote_Reset_Flag();
                //Clear_Line_on_LCD((BYTE) 3);
            }
            break;
        case PILOT_TRAIN_IN_SECTION:
            if(Relay_B_Info.DS1_Direction  == (BYTE)REVERSE_DIRECTION&&
               Relay_B_Info.DS2_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_B_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_B_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
            {
                if(Relay_B_Info.DS_Local_Count > Relay_B_Info.DS1_Axle_Count ||
                   Relay_B_Info.DS_Local_Count > Relay_B_Info.DS2_Axle_Count )
                   {
                    Declare_DAC_Defective_DS();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
            }
            if ((Relay_B_Info.LU1_Fwd_Count  < 2 ||  Relay_B_Info.LU2_Fwd_Count <2) &&
                (Relay_B_Info.LU1_Rev_Count  < 2 ||  Relay_B_Info.LU2_Rev_Count <2) )
             {
              if(Relay_B_Info.DS1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
                 Relay_B_Info.DS2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
                 Relay_B_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
                 Relay_B_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
                {
                 /* no in-counts, only out-counts,system goes to error state*/
                 Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                 Status.Flags.Direct_Out_Count = SET_HIGH;
                 Declare_DAC_Defective_DS();
                 break;
                }
              else
               {
                break;
               }
             }
            if (Relay_B_Info.LU1_Fwd_Count  == Relay_B_Info.DS1_OUT_Count  &&
                 Relay_B_Info.LU2_Fwd_Count == Relay_B_Info.DS2_OUT_Count &&
                 Relay_B_Info.LU1_Rev_Count == Relay_B_Info.DS1_IN_Count  &&
                 Relay_B_Info.LU2_Rev_Count == Relay_B_Info.DS2_IN_Count  )
            {
                if(Relay_B_Info.LU1_Fwd_Count == Relay_B_Info.LU2_Fwd_Count &&
                   Relay_B_Info.DS1_IN_Count  == Relay_B_Info.DS2_IN_Count  &&
                   Relay_B_Info.LU1_Rev_Count == Relay_B_Info.LU2_Rev_Count &&
                   Relay_B_Info.DS1_OUT_Count == Relay_B_Info.DS2_OUT_Count )
                {
                /* Train cleared the section */
                   DeEnergise_Preparatory_Relay_B();            /* from relaydrv.c */
                   uiAuthorisationKey = Get_Relay_Energising_Key();
                   Energise_Vital_Relay_B(uiAuthorisationKey);
                   Clear_DS_AxleCount();
                   Relay_B_Info.State = NO_TRAIN_IN_SECTION;
                   DS_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;
                   DS_Section_Mode.Remote_Unit = SYSTEM_CLEAR_MODE;
                   break;
                }
            }
            if(Relay_B_Info.DS1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_B_Info.DS2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_B_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_B_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
              {
                /* no in-counts, only out-counts,system goes to error state*/
                Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                Status.Flags.Direct_Out_Count = SET_HIGH;
                Declare_DAC_Defective_DS();
               }
             break;
        case NO_TRAIN_IN_SECTION:
            if (Relay_B_Info.LU1_Fwd_Count != Relay_B_Info.DS1_OUT_Count  ||
                Relay_B_Info.LU2_Fwd_Count != Relay_B_Info.DS2_OUT_Count  ||
                Relay_B_Info.LU1_Rev_Count != Relay_B_Info.DS1_IN_Count   ||
                Relay_B_Info.LU2_Rev_Count != Relay_B_Info.DS2_IN_Count  )
            {
                /* Train Occupied the section */
                DeEnergise_Vital_Relay_B();
                Relay_B_Info.State = TRAIN_IN_SECTION;
                DS_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
                DS_Section_Mode.Remote_Unit = SYSTEM_OCCUPIED_MODE;
                break;
            }
           if(Relay_B_Info.LU1_Fwd_Count   != Relay_B_Info.LU2_Fwd_Count ||
                Relay_B_Info.DS1_IN_Count  != Relay_B_Info.DS2_IN_Count  ||
                Relay_B_Info.LU1_Rev_Count != Relay_B_Info.LU2_Rev_Count ||
                Relay_B_Info.DS1_OUT_Count != Relay_B_Info.DS2_OUT_Count )
            {
                /* Train Occupied the section */
                DeEnergise_Vital_Relay_B();
                Relay_B_Info.State = TRAIN_IN_SECTION;
                DS_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
                DS_Section_Mode.Remote_Unit = SYSTEM_OCCUPIED_MODE;
            }
            break;
        case TRAIN_IN_SECTION:
             if(Get_DS_Local_Counts_Clearing_Status())
                {
                 if(Relay_B_Info.DS1_Direction  == (BYTE)REVERSE_DIRECTION&&
                    Relay_B_Info.DS2_Direction  == (BYTE)REVERSE_DIRECTION &&
                    Relay_B_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
                    Relay_B_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
                     {
                        if(Relay_B_Info.DS_Local_Count > 0 )
                         {
                            Declare_DAC_Defective_DS();
                            Status.Flags.Direct_Out_Count = SET_HIGH;
                            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                            break;
                         }
                     }
                  }
            if(Relay_B_Info.DS1_Direction  == (BYTE)REVERSE_DIRECTION&&
               Relay_B_Info.DS2_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_B_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_B_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
            {
                if(Relay_B_Info.DS_Local_Count > Relay_B_Info.DS1_Axle_Count ||
                   Relay_B_Info.DS_Local_Count > Relay_B_Info.DS2_Axle_Count )
                   {
                    Declare_DAC_Defective_DS();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
            }

            if (Relay_B_Info.LU1_Fwd_Count == Relay_B_Info.DS1_OUT_Count  &&
                Relay_B_Info.LU2_Fwd_Count == Relay_B_Info.DS2_OUT_Count  &&
                Relay_B_Info.LU1_Rev_Count == Relay_B_Info.DS1_IN_Count   &&
                Relay_B_Info.LU2_Rev_Count == Relay_B_Info.DS2_IN_Count  )
            {
                if(Relay_B_Info.LU1_Fwd_Count == Relay_B_Info.LU2_Fwd_Count &&
                   Relay_B_Info.DS1_IN_Count  == Relay_B_Info.DS2_IN_Count  &&
                   Relay_B_Info.LU1_Rev_Count == Relay_B_Info.LU2_Rev_Count &&
                   Relay_B_Info.DS1_OUT_Count == Relay_B_Info.DS2_OUT_Count )
                {
                /* Train cleared the section */
                   uiAuthorisationKey = Get_Relay_Energising_Key();
                   Energise_Vital_Relay_B(uiAuthorisationKey);
                   Relay_B_Info.State = NO_TRAIN_IN_SECTION;
                   DS_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;
                   DS_Section_Mode.Remote_Unit = SYSTEM_CLEAR_MODE;
                   Clear_DS_AxleCount();
                   break;
                 }
            }
            if(Relay_B_Info.DS1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_B_Info.DS2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_B_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
               Relay_B_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
              {
                /* no in-counts, only out-counts,system goes to error state*/
                Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                Status.Flags.Direct_Out_Count = SET_HIGH;
                Declare_DAC_Defective_DS();
                }
                break;
        case  DAC_DEFECTIVE:
                break;
        case DAC_RESET_PROGRESS:
            break;
        default:
            break;
    }
}

//15_03_10
void Update_3S_Relay_B_State(void)
{
    INT16 uiAuthorisationKey;

    switch (Relay_B_Info.State)
    {
        case RELAY_MANAGER_NOT_STARTED:
        case DAC_REPEATER_RELAY_ON:
        case DAC_REPEATER_RELAY_OFF:
        case RELAY_MANAGER_IDLE:
             break;
        case ATC_WAIT_FOR_REMOTE_CLEAR:
            if(Relay_B_Info.ATC_Remote1_State == ATC_READY_TO_CLEAR &&
               Relay_B_Info.ATC_Remote2_State == ATC_READY_TO_CLEAR)
            {
               Relay_B_Info.State = TRAIN_IN_SECTION;
            }
            break;
        case WAIT_FOR_PILOT_TRAIN:
            if (Relay_B_Info.LU1_Fwd_Count > 0 || Relay_B_Info.LU1_Rev_Count > 0 ||
                Relay_B_Info.LU2_Fwd_Count > 0 || Relay_B_Info.LU2_Rev_Count > 0 ||
                Relay_B_Info.DS1_IN_Count  > 0 || Relay_B_Info.DS2_IN_Count > 0 ||
                Relay_B_Info.DS1_OUT_Count > 0 || Relay_B_Info.DS2_OUT_Count > 0 )
            {
                Relay_B_Info.State = PILOT_TRAIN_IN_SECTION;
                Clear_Local_Reset2_Flag();
                Clear_DS_Remote_Reset_Flag();
                //Clear_Line_on_LCD((BYTE) 3);
            }
            break;
        case PILOT_TRAIN_IN_SECTION:
            if(Relay_B_Info.DS1_Direction  == (BYTE)FORWARD_DIRECTION&&
               Relay_B_Info.DS2_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_B_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_B_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
            {
                if(Relay_B_Info.DS_Local_Count > Relay_B_Info.DS1_Axle_Count ||
                   Relay_B_Info.DS_Local_Count > Relay_B_Info.DS2_Axle_Count )
                   {
                    Declare_DAC_Defective_DS();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
            }
            if ((Relay_B_Info.LU1_Fwd_Count  < 2 ||  Relay_B_Info.LU2_Fwd_Count <2) &&
                (Relay_B_Info.LU1_Rev_Count  < 2 ||  Relay_B_Info.LU2_Rev_Count <2) )
             {
              if(Relay_B_Info.DS1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
                 Relay_B_Info.DS2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
                 Relay_B_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
                 Relay_B_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
                {
                 /* no in-counts, only out-counts,system goes to error state*/
                 Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                 Status.Flags.Direct_Out_Count = SET_HIGH;
                 Declare_DAC_Defective_DS();
                 break;
                }
              else
               {
                break;
               }
             }
            if (Relay_B_Info.LU1_Fwd_Count  == Relay_B_Info.DS1_IN_Count  &&
                 Relay_B_Info.LU2_Fwd_Count == Relay_B_Info.DS2_IN_Count &&
                 Relay_B_Info.LU1_Rev_Count == Relay_B_Info.DS1_OUT_Count  &&
                 Relay_B_Info.LU2_Rev_Count == Relay_B_Info.DS2_OUT_Count  )
            {
                if(Relay_B_Info.LU1_Fwd_Count == Relay_B_Info.LU2_Fwd_Count &&
                   Relay_B_Info.DS1_IN_Count  == Relay_B_Info.DS2_IN_Count  &&
                   Relay_B_Info.LU1_Rev_Count == Relay_B_Info.LU2_Rev_Count &&
                   Relay_B_Info.DS1_OUT_Count == Relay_B_Info.DS2_OUT_Count )
                {
                /* Train cleared the section */
                   DeEnergise_Preparatory_Relay_B();            /* from relaydrv.c */
                   uiAuthorisationKey = Get_Relay_Energising_Key();
                   Energise_Vital_Relay_B(uiAuthorisationKey);
                   Clear_DS_AxleCount();
                   Relay_B_Info.State = NO_TRAIN_IN_SECTION;
                   DS_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;
                   DS_Section_Mode.Remote_Unit = SYSTEM_CLEAR_MODE;
                   break;
                }
            }
            if(Relay_B_Info.DS1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_B_Info.DS2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_B_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_B_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
              {
                /* no in-counts, only out-counts,system goes to error state*/
                Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                Status.Flags.Direct_Out_Count = SET_HIGH;
                Declare_DAC_Defective_DS();
               }
             break;
        case NO_TRAIN_IN_SECTION:
            if (Relay_B_Info.LU1_Fwd_Count != Relay_B_Info.DS1_IN_Count  ||
                Relay_B_Info.LU2_Fwd_Count != Relay_B_Info.DS2_IN_Count  ||
                Relay_B_Info.LU1_Rev_Count != Relay_B_Info.DS1_OUT_Count   ||
                Relay_B_Info.LU2_Rev_Count != Relay_B_Info.DS2_OUT_Count  )
            {
                /* Train Occupied the section */
                DeEnergise_Vital_Relay_B();
                Relay_B_Info.State = TRAIN_IN_SECTION;
                DS_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
                DS_Section_Mode.Remote_Unit = SYSTEM_OCCUPIED_MODE;
                break;
            }
           if(Relay_B_Info.LU1_Fwd_Count   != Relay_B_Info.LU2_Fwd_Count ||
                Relay_B_Info.DS1_IN_Count  != Relay_B_Info.DS2_IN_Count  ||
                Relay_B_Info.LU1_Rev_Count != Relay_B_Info.LU2_Rev_Count ||
                Relay_B_Info.DS1_OUT_Count != Relay_B_Info.DS2_OUT_Count )
            {
                /* Train Occupied the section */
                DeEnergise_Vital_Relay_B();
                Relay_B_Info.State = TRAIN_IN_SECTION;
                DS_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
                DS_Section_Mode.Remote_Unit = SYSTEM_OCCUPIED_MODE;
            }
            break;
        case TRAIN_IN_SECTION:
             if(Get_DS_Local_Counts_Clearing_Status())
                {
                 if(Relay_B_Info.DS1_Direction  == (BYTE)FORWARD_DIRECTION &&
                    Relay_B_Info.DS2_Direction  == (BYTE)FORWARD_DIRECTION &&
                    Relay_B_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
                    Relay_B_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
                     {
                        if(Relay_B_Info.DS_Local_Count > 0 )
                         {
                            Declare_DAC_Defective_DS();
                            Status.Flags.Direct_Out_Count = SET_HIGH;
                            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                            break;
                         }
                     }
                  }
            if(Relay_B_Info.DS1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_B_Info.DS2_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_B_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_B_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
            {
                if(Relay_B_Info.DS_Local_Count > Relay_B_Info.DS1_Axle_Count ||
                   Relay_B_Info.DS_Local_Count > Relay_B_Info.DS2_Axle_Count )
                   {
                    Declare_DAC_Defective_DS();
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    break;
                   }
            }

            if (Relay_B_Info.LU1_Fwd_Count == Relay_B_Info.DS1_IN_Count  &&
                Relay_B_Info.LU2_Fwd_Count == Relay_B_Info.DS2_IN_Count  &&
                Relay_B_Info.LU1_Rev_Count == Relay_B_Info.DS1_OUT_Count   &&
                Relay_B_Info.LU2_Rev_Count == Relay_B_Info.DS2_OUT_Count  )
            {
                if(Relay_B_Info.LU1_Fwd_Count == Relay_B_Info.LU2_Fwd_Count &&
                   Relay_B_Info.DS1_IN_Count  == Relay_B_Info.DS2_IN_Count  &&
                   Relay_B_Info.LU1_Rev_Count == Relay_B_Info.LU2_Rev_Count &&
                   Relay_B_Info.DS1_OUT_Count == Relay_B_Info.DS2_OUT_Count )
                {
                /* Train cleared the section */
                   uiAuthorisationKey = Get_Relay_Energising_Key();
                   Energise_Vital_Relay_B(uiAuthorisationKey);
                   Relay_B_Info.State = NO_TRAIN_IN_SECTION;
                   DS_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;
                   DS_Section_Mode.Remote_Unit = SYSTEM_CLEAR_MODE;
                   Clear_DS_AxleCount();
                   break;
                 }
            }
            if(Relay_B_Info.DS1_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_B_Info.DS2_Direction  == (BYTE)DIRECTION_NOT_DEFINED &&
               Relay_B_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
               Relay_B_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
              {
                /* no in-counts, only out-counts,system goes to error state*/
                Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                Status.Flags.Direct_Out_Count = SET_HIGH;
                Declare_DAC_Defective_DS();
                }
                break;
        case  DAC_DEFECTIVE:
                break;
        case DAC_RESET_PROGRESS:
            break;
        default:
            break;
    }

}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :BOOL Reset_Allowed_For_DS()
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

Design Requirements:	SSDAC_DR_5113


Interfaces
    Calls           :   None

    Called by       :   RESET.C-Process_CF_Reset_Command()
                        RESET.C-SF_Wait_for_Local_Reset()

Input Variables                     Name                Type
    Global          :   DAC_DEFECTIVE                   Enumerator
                        Relay_B_Info.State              Enumerator
                        Relay_B_Info.DS1_Direction      Byte
                        FORWARD_DIRECTION               Enumerator
                        Relay_B_Info.DS2_Direction      Byte
                        Relay_B_Info.LU1_Direction      Byte
                        Relay_B_Info.LU2_Direction      Byte
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

Algorithm           :1.Return TRUE if the system state is in Defective
                     2.Return TRUE if both the Local and down section system relay B direction are Forward or reverse
*******************************************************************************/
BOOL Reset_Allowed_For_DS(void)
{
    BOOL ReturnValue = FALSE;

    if( Relay_B_Info.State == DAC_DEFECTIVE)
    {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
    }
    if(Relay_B_Info.DS1_Direction  == (BYTE)REVERSE_DIRECTION &&
       Relay_B_Info.DS2_Direction  == (BYTE)REVERSE_DIRECTION &&
       Relay_B_Info.LU1_Direction  == (BYTE)REVERSE_DIRECTION &&
       Relay_B_Info.LU2_Direction  == (BYTE)REVERSE_DIRECTION )
    {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
    }
    if(Relay_B_Info.DS1_Direction  == (BYTE)FORWARD_DIRECTION &&
       Relay_B_Info.DS2_Direction  == (BYTE)FORWARD_DIRECTION &&
       Relay_B_Info.LU1_Direction  == (BYTE)FORWARD_DIRECTION &&
       Relay_B_Info.LU2_Direction  == (BYTE)FORWARD_DIRECTION )
    {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
    }
        return(ReturnValue);
}


BOOL Reset_Allowed_For_LCWS(void)
{
    BOOL ReturnValue = FALSE;

    if( Relay_B_Info.State == DAC_DEFECTIVE || Relay_A_Info.State == DAC_DEFECTIVE || Status.Flags.System_Status != NORMAL)
    {
        ReturnValue = (BOOL) TRUE;
    }
    return(ReturnValue);
}


BOOL Reset_Allowed_For_DE(void)
{
    BOOL ReturnValue = FALSE;

    if( Relay_B_Info.State == DAC_DEFECTIVE || Relay_A_Info.State == DAC_DEFECTIVE || Status.Flags.System_Status != NORMAL)
    {
        ReturnValue = (BOOL) TRUE;
    }
    return(ReturnValue);
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void
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
                     else update the direction in CPU2 and update the local axle counts


Allocated Requirements:

Design Requirements:	SSDAC_DR_5114

Interfaces
    Calls           :   RLYB_MGR.C-Update_B_LU1_Direction()
                        RLYB_MGR.C-Update_B_LU2_Direction()
                        RLYB_MGR.C-Update_B_Local_AxleCount()

    Called by       :   RELAYMGR.C-Update_Relay_B_Counts()

Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_CPU1   Bit

    Local           :   uchDirection                    BYTE
                        uiAxleCount                     UNIT16

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1

References          :

Derived Requirements:

*******************************************************************************/
void Process_Relay_B_Local_Direction(BYTE uchDirection,UINT16 uiAxleCount)
{
    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
    {
        Update_B_LU1_Direction((BYTE) uchDirection);
    }
    else
    {
        Update_B_LU2_Direction((BYTE) uchDirection);
    }
        Update_B_Local_AxleCount(uiAxleCount);

}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Process_Relay_B_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
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

Design Requirements:	SSDAC_DR_5115


Interfaces
    Calls           :   RLYB_MGR.C-Update_B_LU1_Fwd_Count()
                        RLYB_MGR.C-Update_B_LU1_Rev_Count()
                        RLYB_MGR.C-Update_B_LU2_Fwd_Count()
                        RLYB_MGR.C-Update_B_LU2_Rev_Count()

    Called by       :   RELAYMGR.C-Update_Relay_B_Counts()

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
*******************************************************************************/
void Process_Relay_B_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{
    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
    {
        Update_B_LU1_Fwd_Count(uiFwdAxleCount);
        Update_B_LU1_Rev_Count(uiRevAxleCount);
    }
    else
    {
        Update_B_LU2_Fwd_Count(uiFwdAxleCount);
        Update_B_LU2_Rev_Count(uiRevAxleCount);
    }
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Process_Peer_Relay_B_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
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

Design Requirements:	SSDAC_DR_5116

Interfaces
    Calls           :   RLYB_MGR.C-Update_B_LU1_Fwd_Count()
                        RLYB_MGR.C-Update_B_LU1_Rev_Count()
                        RLYB_MGR.C-Update_B_LU2_Fwd_Count()
                        RLYB_MGR.C-Update_B_LU2_Rev_Count()

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
*******************************************************************************/
void Process_Peer_Relay_B_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{

    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
    {
        Update_B_LU2_Fwd_Count(uiFwdAxleCount);
        Update_B_LU2_Rev_Count(uiRevAxleCount);
    }
    else
    {
        Update_B_LU1_Fwd_Count(uiFwdAxleCount);
        Update_B_LU1_Rev_Count(uiRevAxleCount);
    }
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void  Process_Peer_Relay_B_Direction( BYTE uchDirection)
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

Design Requirements:	SSDAC_DR_5117


Interfaces
    Calls           :   RLYB_MGR.C-Update_B_LU2_Direction()
                        RLYB_MGR.C-Update_B_LU1_Direction()

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

*******************************************************************************/
void  Process_Peer_Relay_B_Direction( BYTE uchDirection)
{
    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
    {
        Update_B_LU2_Direction((BYTE) uchDirection);
    }
    else
    {
        Update_B_LU1_Direction((BYTE) uchDirection);
    }
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_Previous_Block_State(bitadrb_t SrcAdr, BYTE Prev_Block_Status)
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

Design Requirements:	SSDAC_DR_5118

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Axle_Count_Message()

Input Variables                     Name                Type
    Global          :   None

    Local           :   SrcAdr                              bitadrb_t
                        Prev_Block_Status                   BYTE
                        SrcAdr.Bit.b0                       Bit

Output Variables                    Name                Type
    Global          :   Relay_B_Info.Previous_Block_CPU1_Status Byte
                        Relay_B_Info.Previous_Block_CPU2_Status Byte

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1
References          :

Derived Requirements:

*******************************************************************************/
//void Update_Previous_Block_State(bitadrb_t SrcAdr, BYTE Prev_Block_Status)
//{
//  if (SrcAdr.Bit.b0 == SET_HIGH)
//  {
//      /* From Remote Unit - CPU1 */
//      Relay_B_Info.Previous_Block_CPU1_Status = Prev_Block_Status;
//  }
//  else
//  {
//      /* From Remote Unit - CPU2 */
//      Relay_B_Info.Previous_Block_CPU2_Status = Prev_Block_Status;
//  }
//}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_DS1_OUT_Count(UINT16 uiCount)
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

Design Requirements:	SSDAC_DR_5119

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS1_OUT_Count      UINT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_B_DS1_OUT_Count(UINT16 uiCount)
{
    Relay_B_Info.DS1_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_DS2_OUT_Count(UINT16 uiCount)
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

Design Requirements:	SSDAC_DR_5120

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS2_OUT_Count      UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_B_DS2_OUT_Count(UINT16 uiCount)
{
    Relay_B_Info.DS2_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_LU1_Fwd_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit forward count  in CPU1 required to monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5120

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.C-Process_Relay_B_Local_AxleCount()
                        RLYB_MGR.C-Process_Peer_Relay_B_AxleCount()


Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.LU1_Fwd_Count      UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_B_LU1_Fwd_Count(UINT16 uiCount)
{
    Relay_B_Info.LU1_Fwd_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_LU2_Fwd_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit foward count in CPU2 required to monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5122

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.C-Process_Relay_B_Local_AxleCount()
                        RLYB_MGR.C-Process_Peer_Relay_B_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.LU2_Fwd_Count      UINT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_B_LU2_Fwd_Count(UINT16 uiCount)
{
    Relay_B_Info.LU2_Fwd_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_LU1_Rev_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit reverse count in CPU1 required to monitor the system


Allocated Requirements:

Design Requirements: SSDAC_DR_5123

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.C-Process_Relay_B_Local_AxleCount()
                        RLYB_MGR.C-Process_Peer_Relay_B_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.LU1_Rev_Count      UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_B_LU1_Rev_Count(UINT16 uiCount)
{
    Relay_B_Info.LU1_Rev_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_LU2_Rev_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit reverse count in CPU2 required to monitor the system

Allocated Requirements:

Design Requirements: 	SSDAC_DR_5124


Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.C-Process_Relay_B_Local_AxleCount()
                        RLYB_MGR.C-Process_Peer_Relay_B_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.LU2_Rev_Count      UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:


*******************************************************************************/
void Update_B_LU2_Rev_Count(UINT16 uiCount)
{
    Relay_B_Info.LU2_Rev_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_Local_AxleCount(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the down stream local count

Allocated Requirements:

Design Requirements:	SSDAC_DR_5125

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.C-Process_Relay_B_Local_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS_Local_Count     UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:


*******************************************************************************/
void Update_B_Local_AxleCount(UINT16 uiCount)
{
    Relay_B_Info.DS_Local_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_DS1_IN_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the remote unit IN count in CPU1 required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5126

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS1_IN_Count       UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:

*******************************************************************************/
void Update_B_DS1_IN_Count(UINT16 uiCount)
{
    Relay_B_Info.DS1_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_DS2_IN_Count(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the remote unit IN counts in CPU2 required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5127

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS2_IN_Count       UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_B_DS2_IN_Count(UINT16 uiCount)
{
    Relay_B_Info.DS2_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_DS1_AxleCount(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the down stream counts in CPU1 required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5128

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS1_Axle_Count     UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_B_DS1_AxleCount(UINT16 uiCount)
{
    Relay_B_Info.DS1_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_DS2_AxleCount(UINT16 uiCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the down stream counts in CPU2 required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5129

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_Direction(()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS2_Axle_Count     UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_B_DS2_AxleCount(UINT16 uiCount)
{
    Relay_B_Info.DS2_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_DS1_Direction(BYTE Direction)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the remote unit axle direction in CPU1 required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5130

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS1_Direction      BYTE

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_B_DS1_Direction(BYTE Direction)
{
    Relay_B_Info.DS1_Direction = Direction;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_DS2_Direction(BYTE Direction)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the remote unit axle direction in CPU2 required to monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_513

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_B_Info.DS2_Direction      BYTE

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:

*******************************************************************************/
void Update_B_DS2_Direction(BYTE Direction)
{
    Relay_B_Info.DS2_Direction = Direction;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_LU1_Direction(BYTE Direction)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit axle direction in CPU1 required to monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5132

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.C-Process_Relay_B_Local_Direction()
                        RLYB_MGR.C-Process_Peer_Relay_B_Direction()


Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_B_Info.LU1_Direction      BYTE

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_B_LU1_Direction(BYTE Direction)
{
    Relay_B_Info.LU1_Direction = Direction;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_B_LU2_Direction(BYTE Direction)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the local unit axle direction in CPU2 required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5380

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.C-Process_Relay_B_Local_Direction()
                        RLYB_MGR.C-Process_Peer_Relay_B_Direction()


Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_B_Info.LU2_Direction      BYTE

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_B_LU2_Direction(BYTE Direction)
{
    Relay_B_Info.LU2_Direction = Direction;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_ATC_Remote1_Relay_B_State(BYTE Remote_state)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :If the system is configured to ATC configuration, then update the remote unit
                     state in CPU1 required to monitor the system


Allocated Requirements:

Design Requirements:	SSDAC_DR_5381

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_Axle_Count_Message()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Remote_state                    BYTE

Output Variables                    Name                Type
    Global          :   Relay_B_Info.ATC_Remote1_State  BYTE

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_ATC_Remote1_Relay_B_State(BYTE Remote_state)
{
    Relay_B_Info.ATC_Remote1_State = Remote_state;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Update_ATC_Remote2_Relay_B_State(BYTE Remote_state)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :If the system is configured to ATC configuration, then update the remote unit
                     state in CPU2 required to monitor the system

Allocated Requirements:

Design Requirements:	SSDAC_DR_5382

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_Axle_Count_Message()


Input Variables                     Name                Type
    Global          :   None

    Local           :   Remote_state                    BYTE

Output Variables                    Name                Type
    Global          :   Relay_B_Info.ATC_Remote2_State  BYTE

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:

*******************************************************************************/
void Update_ATC_Remote2_Relay_B_State(BYTE Remote_state)
{
    Relay_B_Info.ATC_Remote2_State = Remote_state;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Set_Relay_B_DAC_Defective(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :DeEnergise the Vital relay B and Declare DAC defective if any error
                     in the system

Allocated Requirements: (SSDAC_SWRS_0269)

Design Requirements:	SSDAC_DR_5383
	    	
Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Vital_Relay_B()

    Called by       :   RELAYMGR.C-Declare_DAC_Defective()
                        RELAYMGR.C-Declare_DAC_Defective_US()
                        RELAYMGR.C-Declare_DAC_Defective_DS()

Input Variables                     Name                Type
    Global          :   DAC_DEFECTIVE                   Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_B_Info.State              Enumerator

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:



*******************************************************************************/
void Set_Relay_B_DAC_Defective(void)
{
    DeEnergise_Vital_Relay_B();
    Relay_B_Info.State = DAC_DEFECTIVE;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :BYTE Get_Relay_B_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get Relay B state to know wheather
                     1.The relay A is initialised
                     2.Waiting for pilot train
                     3.No train in section
                     4.Train in section
                     5.System is defective
                     6.System is waiting for reset
                     7.To know the status of repeat relay B

Allocated Requirements:

Design Requirements:	SSDAC_DR_5384

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Process_DS_Axle_Count_Message()
                        ERROR.C-Clear_Error_Display()
                        RELAYMGR.C-Update_CF_Track_Status()
                        RELAYMGR.C-Update_SF_Track_Status()
                        RESET.C-Process_CF_Reset_Command()
                        SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_B_Info.State              BYTE

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:



*******************************************************************************/
BYTE Get_Relay_B_State(void)
{
 return((BYTE)Relay_B_Info.State);
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :BYTE Get_Relay_B_ATC_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get Relay B state for ATC configuration to know wheather
                     1.The relay A is initialised
                     2.Waiting for pilot train
                     3.No train in section
                     4.Train in section
                     5.System is defective
                     6.System is waiting for reset
                     7.To know the status of repeat relay B


Allocated Requirements:

Design Requirements:	SSDAC_DR_5385

Interfaces
    Calls           :   None

    Called by       :   COMM_DS.C-Build_DS_Axle_Count_Message()

Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_B_Info.ATC_Local_State    Byte

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:


*******************************************************************************/
BYTE Get_Relay_B_ATC_State(void)
{
    return(Relay_B_Info.ATC_Local_State);
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Clear_ATC_Local_Relay_B_State(void)
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

Design Requirements:	SSDAC_DR_5386

Interfaces
    Calls           :   None

    Called by       :   RELAYMGR.C-Declare_DAC_Defective()
                        RELAYMGR.C-Declare_DAC_Defective_DS()

Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_B_Info.ATC_Local_State    Byte
                        Relay_B_Info.ATC_Remote1_State  Byte
                        Relay_B_Info.ATC_Remote2_State  Byte

    Local           :   None

Signal Variables



Macros defined      :                   Macro                       Value
                                        ATC_NOT_READY_T0_CLEAR        0
References          :

Derived Requirements:
*******************************************************************************/
void Clear_ATC_Local_Relay_B_State(void)
{
  Relay_B_Info.ATC_Local_State = ATC_NOT_READY_T0_CLEAR;
  Relay_B_Info.ATC_Remote1_State =  ATC_NOT_READY_T0_CLEAR;
  Relay_B_Info.ATC_Remote2_State =  ATC_NOT_READY_T0_CLEAR;
}
/******************************************************************************
Component name      :RLYB_MGR
Module Name         :void Clear_Relay_B_Counts(void)
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

Design Requirements:	SSDAC_DR_5387

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.C-Start_Relay_B_Mgr()

Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_B_Info.LU1_Fwd_Count      UINT16
                        Relay_B_Info.LU2_Fwd_Count      UINT16
                        Relay_B_Info.LU1_Rev_Count      UINT16
                        Relay_B_Info.LU2_Rev_Count      UINT16
                        Relay_B_Info.DS1_IN_Count       UINT16
                        Relay_B_Info.DS2_IN_Count       UINT16
                        Relay_B_Info.DS1_OUT_Count      UINT16
                        Relay_B_Info.DS2_OUT_Count      UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Clear_Relay_B_Counts(void)
{
  Relay_B_Info.LU1_Fwd_Count = 0;
  Relay_B_Info.LU2_Fwd_Count = 0;
  Relay_B_Info.LU1_Rev_Count = 0;
  Relay_B_Info.LU2_Rev_Count = 0;
  Relay_B_Info.DS1_IN_Count  = 0;
  Relay_B_Info.DS2_IN_Count  = 0;
  Relay_B_Info.DS1_OUT_Count = 0;
  Relay_B_Info.DS2_OUT_Count = 0;
}



