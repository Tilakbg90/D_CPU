/******************************************************************************
    Project         :
    Version         :
    Revision        :
    Component name  :   RLYD3_MGR
    Target MCU      :
    Compiler        :
    Author          :
    Date            :
    Company Name    :
    Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
    Functions   :   void Initialise_Relay_D3_Mgr(void)
                    void Start_Relay_D3_Mgr(void)
                    void Update_Relay_D3_State(void)
                    BOOL Reset_Allowed_For_D3()
                    void Update_D3_A1_OUT_Count(UINT16 uiCount)
                    void Update_D3_A2_OUT_Count(UINT16 uiCount)
                    void Update_D3_A1_IN_Count(UINT16 uiCount)
                    void Update_D3_A2_IN_Count(UINT16 uiCount)
                    void Update_D3_B1_OUT_Count(UINT16 uiCount)
                    void Update_D3_B2_OUT_Count(UINT16 uiCount)
                    void Update_D3_B1_IN_Count(UINT16 uiCount)
                    void Update_D3_B2_IN_Count(UINT16 uiCount)
                    void Update_D3_C1_OUT_Count(UINT16 uiCount)
                    void Update_D3_C2_OUT_Count(UINT16 uiCount)
                    void Update_D3_C1_IN_Count(UINT16 uiCount)
                    void Update_D3_C2_IN_Count(UINT16 uiCount)
                    void Update_D3_A1_Direction(BYTE Direction)
                    void Update_D3_A2_Direction(BYTE Direction)
                    void Update_D3_B1_Direction(BYTE Direction)
                    void Update_D3_B2_Direction(BYTE Direction)
                    void Update_D3_C1_Direction(BYTE Direction)
                    void Update_D3_C2_Direction(BYTE Direction)
                    void Set_Relay_D3_DAC_Defective(void)
                    void Process_D3_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void Process_D3_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void Process_D3_Remote_AxleCount(bitadrb_t SrcAdr,DAC_Unit_Type Src_Unit_Type,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void Process_D3_Local_Direction(BYTE uchDirection)
                    void  Process_D3_Peer_Direction(BYTE uchDirection,UINT16 uiAxleCount)
                    void Process_D3_Remote_Direction(bitadrb_t SrcAdr ,DAC_Unit_Type Src_Unit_Type, BYTE uchDirection)
                    void Decrement_D3_Track_Clearing_Timeout_50msTmr(void)

*******************************************************************************/


#include "COMMON.h"
#include "RELAYDRV.h"
#include "RLYD3_MGR.h"
#include "RELAYMGR.h"
#include "ERROR.h"
#include "RESET.h"

extern  /*near*/  dac_status_t Status;                    /* from dac_main.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from dac_main.c */
relay_d3_info_t Relay_D3_Info;
extern ds_section_mode DS_Section_Mode;             /*from DAC_MAIN.c*/


void Update_D3_Local_Count     (UINT16 uiCount);
void Update_D3_A1_IN_Count     (UINT16 uiCount);
void Update_D3_A2_IN_Count     (UINT16 uiCount);
void Update_D3_B1_IN_Count     (UINT16 uiCount);
void Update_D3_B2_IN_Count     (UINT16 uiCount);
void Update_D3_C1_IN_Count     (UINT16 uiCount);
void Update_D3_C2_IN_Count     (UINT16 uiCount);
void Update_D3_A1_OUT_Count    (UINT16 uiCount);
void Update_D3_A2_OUT_Count    (UINT16 uiCount);
void Update_D3_B1_OUT_Count    (UINT16 uiCount);
void Update_D3_B2_OUT_Count    (UINT16 uiCount);
void Update_D3_C1_OUT_Count    (UINT16 uiCount);
void Update_D3_C2_OUT_Count    (UINT16 uiCount);
void Update_D3_A1_IN_AxleCount (UINT16 uiCount);
void Update_D3_A2_IN_AxleCount (UINT16 uiCount);
void Update_D3_A1_OUT_AxleCount(UINT16 uiCount);
void Update_D3_A2_OUT_AxleCount(UINT16 uiCount);
void Update_D3_B1_IN_AxleCount (UINT16 uiCount);
void Update_D3_B2_IN_AxleCount (UINT16 uiCount);
void Update_D3_B1_OUT_AxleCount(UINT16 uiCount);
void Update_D3_B2_OUT_AxleCount(UINT16 uiCount);
void Update_D3_C1_IN_AxleCount (UINT16 uiCount);
void Update_D3_C2_IN_AxleCount (UINT16 uiCount);
void Update_D3_C1_OUT_AxleCount(UINT16 uiCount);
void Update_D3_C2_OUT_AxleCount(UINT16 uiCount);
void Update_D3_A1_Direction   (BYTE Direction);
void Update_D3_A2_Direction   (BYTE Direction);
void Update_D3_B1_Direction   (BYTE Direction);
void Update_D3_B2_Direction   (BYTE Direction);
void Update_D3_C1_Direction   (BYTE Direction);
void Update_D3_C2_Direction   (BYTE Direction);
void Update_D3_Local_Direction(BYTE Direction);
void Check_For_D3_Direct_Out_Count(void);

/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Initialise_Relay_D3_Mgr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initailise Relay D3 Manager

Allocated Requiremnts:

Design Requirements:


Interfaces
    Calls           :   None

    Called by       :   RELAYMGR.C-Initialise_Relay_Mgr()


Input Variables                     Name                Type
    Global          :   RELAY_MANAGER_NOT_STARTED       Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.State             Enumerator

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:


*******************************************************************************/

void Initialise_Relay_D3_Mgr(void)
{
    Relay_D3_Info.State = RELAY_MANAGER_NOT_STARTED;
    Relay_D3_Info.Track_Clearing_Timeout_50ms = 0;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Start_Relay_D3_Mgr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start Relay D3 Manager

Allocated Requiremnts:

Design Requirements:

Interfaces
    Calls           :   RELAYMGR.C-Display_Clear_Status
                        AXLE_MON.C-Clear_DS_Local_Counts
                        AXLE_MON.C-Clear_US_Local_Counts
                        RESET.C-Clear_Reset_Info

    Called by       :   DAC_MAIN.C-main()


Input Variables                     Name                Type
    Global          :   TRAIN_IN_SECTION                Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.State             Enumerator

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:


Algorithm           :1.Display clear "CL" on seven segment display & drive the green LED to
                       indicated that track is clear for the train to enter the section
                     2.Clear the down stream forward & reverse axle counts
                     3.Clear the up stream forward & reverse axle counts
                     4.Clear all the reset flags for normal operation

*******************************************************************************/

void Start_Relay_D3_Mgr(void)
{
    if (Status.Flags.System_Status != NORMAL)
    {
        return;
    }
    if(Status.Flags.US_System_Status != NORMAL)
    {
     return;
    }
    if(Status.Flags.DS_System_Status != NORMAL)
    {
     return;
    }
    Clear_US_AxleCount();
    Clear_DS_AxleCount();
    if( DIP_Switch_Info.Flags.ATC_Enabled == TRUE)
    {
        Relay_D3_Info.State = ATC_WAIT_FOR_REMOTE_CLEAR;
        Relay_D3_Info.ATC_Local_State = ATC_READY_TO_CLEAR;
    }
    else
    {
        Energise_Preparatory_Relay_B();
        Relay_D3_Info.State = WAIT_FOR_PILOT_TRAIN;

    }
    Clear_DS_Local_Counts();
    Clear_US_Local_Counts();
    Clear_Reset_Info();                 /* Clear the reset flags and reset state */
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_Relay_D3_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the relay state.DeEnergise Vital relay when train enters the section and enegrize when leaves the section
                     If any direct outcount, declare DAC defective


Allocated Requiremnts:

Design Requirements:

Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Vital_Relay_B()
                        RELAYMGR.C-Display_Occupied_Status()
                        ERROR.C-Set_Error_Status_Bit()
                        RELAYMGR.C-Declare_DAC_Defective()
                        RELAYDRV.C-Get_Relay_Energising_Key()
                        RELAYDRV.C-Energise_Vital_Relay_B()
                        AXLE_MON.C-Clear_US_AxleCount()
                        AXLE_MON.C-Clear_DS_AxleCount()
                        RELAYMGR.C-Display_Clear_Status()

    Called by       :   RELAYMGR.C-Update_Relay_D3_Counts()


Input Variables                     Name                            Type
    Global          :   Relay_D3_Info.A1_IN_Count                   UINT16
                        Relay_D3_Info.A2_IN_Count                   UINT16
                        Relay_D3_Info.B1_OUT_Count                  UINT16
                        Relay_D3_Info.B2_OUT_Count                  UINT16
                        Relay_D3_Info.C1_OUT_Count                  UINT16
                        Relay_D3_Info.C2_OUT_Count                  UINT16
                        Relay_D3_Info.A1_OUT_Count                  UINT16
                        Relay_D3_Info.A2_OUT_Count                  UINT16
                        Relay_D3_Info.B1_IN_Count                   UINT16
                        Relay_D3_Info.B2_IN_Count                   UINT16
                        Relay_D3_Info.C1_IN_Count                   UINT16
                        Relay_D3_Info.C2_IN_Count                   UINT16
                        TRAIN_IN_SECTION                            Enumerator
                        Relay_D3_Info.A1_Direction                  BYTE
                        DIRECTION_NOT_DEFINED                       Enumerator
                        Relay_D3_Info.A2_Direction                  BYTE
                        Relay_D3_Info.B1_Direction                  BYTE
                        Relay_D3_Info.B2_Direction                  BYTE
                        FORWARD_DIRECTION                           Enumerator
                        Relay_D3_Info.C1_Direction                  BYTE
                        Relay_D3_Info.C2_Direction                  BYTE
                        REVERSE_DIRECTION                           Enumerator
                        NO_TRAIN_IN_SECTION                         Enumerator
                        RELAY_MANAGER_IDLE                          Enumerator
                        SYSTEM_OCCUPIED_MODE                        Enumerator
                        DP3_MODE                                    Enumerator
                        Relay_D3_Info.Track_Clearing_Timeout_50ms   BYTE

    Local           :   uiAuthorisationKey                          UINT16
                        Entry_Fwd_Count                             UINT16
                        Exit_Fwd_Count                              UINT16
                        Entry_Rev_Count                             UINT16
                        Exit_Rev_Count                              UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.State             Enumerator
                        Status.Flags.Direct_Out_Count   Bit
                        DS_Section_Mode.Local_Unit      BYTE
                        DS_Section_Mode.Remote_Unit     BYTE

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        DIRECT_OUT_ERROR_NUM       43
                                        SET_HIGH                   1
                                        TIMEOUT_EVENT              0

References          :

Derived Requirements:


Algorithm           :1.


                     2.If the entry & exit counts of train in forward or reverse direction is not same
                       then system should occupy the track, hence de-energise the vital relay B,
                       display as track occupied
                     3.If the train is in section check for the direct out counts
                     4.If the entry & exit counts of train in forward or reverse direction is same or equal
                       than get the security key required to energise the vital relay B,
                       display clear "CL" on 7-segment display & drive the green LED to indicate that
                       train is not in section (left the section) and change the state to idle
                    5.After 3s timeout clear up tream & down stream axle counts and change the state to
                      No Train In Section

*******************************************************************************/
void Update_Relay_D3_State(void)
{
    INT16 uiAuthorisationKey;
    UINT16 Entry1_Count,Exit1_Count;
    UINT16 Entry2_Count,Exit2_Count;
    UINT16 Entry1_Axle_Count,Entry2_Axle_Count;
    UINT16 Exit1_Axle_Count,Exit2_Axle_Count;

    Entry1_Count    = ((Relay_D3_Info.A1_IN_Count + Relay_D3_Info.B1_IN_Count + Relay_D3_Info.C1_IN_Count)% MAX_OVERFLOW_COUNTS_3);

    Entry2_Count    =  ((Relay_D3_Info.A2_IN_Count+ Relay_D3_Info.B2_IN_Count  + Relay_D3_Info.C2_IN_Count)% MAX_OVERFLOW_COUNTS_3);

    Exit1_Count     = ((Relay_D3_Info.A1_OUT_Count + Relay_D3_Info.B1_OUT_Count + Relay_D3_Info.C1_OUT_Count)% MAX_OVERFLOW_COUNTS_3);

    Exit2_Count     = ((Relay_D3_Info.A2_OUT_Count + Relay_D3_Info.B2_OUT_Count + Relay_D3_Info.C2_OUT_Count )% MAX_OVERFLOW_COUNTS_3);

    Entry1_Axle_Count = (Relay_D3_Info.A1_IN_Axle_Count + Relay_D3_Info.B1_IN_Axle_Count + Relay_D3_Info.C1_IN_Axle_Count);
    Entry2_Axle_Count = (Relay_D3_Info.A2_IN_Axle_Count + Relay_D3_Info.B2_IN_Axle_Count + Relay_D3_Info.C2_IN_Axle_Count);

    Exit1_Axle_Count = (Relay_D3_Info.A1_OUT_Axle_Count + Relay_D3_Info.B1_OUT_Axle_Count + Relay_D3_Info.C1_OUT_Axle_Count);
    Exit2_Axle_Count = (Relay_D3_Info.A2_OUT_Axle_Count + Relay_D3_Info.B2_OUT_Axle_Count + Relay_D3_Info.C2_OUT_Axle_Count);

    switch (Relay_D3_Info.State)
    {
        case RELAY_MANAGER_NOT_STARTED:
             break;
        case ATC_WAIT_FOR_REMOTE_CLEAR:
            if(Relay_D3_Info.ATC_Remote1_State == ATC_READY_TO_CLEAR &&
               Relay_D3_Info.ATC_Remote2_State == ATC_READY_TO_CLEAR)
            {
               if(Relay_D3_Info.ATC_Remote3_State == ATC_READY_TO_CLEAR &&
                  Relay_D3_Info.ATC_Remote4_State == ATC_READY_TO_CLEAR)
               {
                Relay_D3_Info.State = TRAIN_IN_SECTION;
               }
            }
            break;
        case WAIT_FOR_PILOT_TRAIN:
            Check_For_D3_Direct_Out_Count();
            if(Entry1_Count > 0 || Entry2_Count > 0)
            {
             Relay_D3_Info.State = PILOT_TRAIN_IN_SECTION;
             Clear_Local_Reset2_Flag();
             Clear_Local_Reset_Flag();
            }
            break;
        case PILOT_TRAIN_IN_SECTION:
            Check_For_D3_Direct_Out_Count();

            if(((Entry1_Axle_Count >= MAX_OVERFLOW_COUNTS_3) || (Entry2_Axle_Count >= MAX_OVERFLOW_COUNTS_3)) &&
                (Exit1_Axle_Count == 0 || Exit2_Axle_Count == 0) )
             {
                Set_Error_Status_Bit(INOPERATIVE_COUNTS_ERROR_NUM);
                Declare_DAC_Defective();
                break;
             }

            if(Entry1_Count < 2 || Entry2_Count < 2)
            {
                break;
            }
            if (Status.Flags.Direct_Out_Count == SET_LOW && Entry1_Count ==  Exit1_Count &&
                 Entry2_Count ==  Exit2_Count && Entry1_Count ==  Entry2_Count && Exit1_Count == Exit2_Count)
             {
                /* Train cleared the section */
                   DeEnergise_Preparatory_Relay_B();            /* from relaydrv.c */
                   uiAuthorisationKey = Get_Relay_Energising_Key();
                   Energise_Vital_Relay_B(uiAuthorisationKey);
                   Relay_D3_Info.State = RELAY_MANAGER_IDLE;
                   DS_Section_Mode.Local_Unit  = SYSTEM_CLEAR_MODE;
                   DS_Section_Mode.Remote_Unit = DP3_MODE;
                   Relay_D3_Info.Track_Clearing_Timeout_50ms = LOCAL_COUNT_CLEARING_TIMEOUT_3;
                    B_Sec_Led_drive(ON);
             }

            break;
        case NO_TRAIN_IN_SECTION:
          if(Entry1_Count  !=  Exit1_Count || Entry2_Count  !=  Exit2_Count)
            {
                /* Train Occupied the section */
                DeEnergise_Vital_Relay_B();
                Relay_D3_Info.State = TRAIN_IN_SECTION;
                B_Sec_Led_drive(OFF);
                DS_Section_Mode.Local_Unit  = SYSTEM_OCCUPIED_MODE;
                DS_Section_Mode.Remote_Unit = DP3_MODE;
            }
            break;
        case TRAIN_IN_SECTION:
             Check_For_D3_Direct_Out_Count();

             if(((Entry1_Axle_Count >= MAX_OVERFLOW_COUNTS_3) || (Entry2_Axle_Count >= MAX_OVERFLOW_COUNTS_3)) &&
                (Exit1_Axle_Count == 0 || Exit2_Axle_Count == 0) )
             {
                Set_Error_Status_Bit(INOPERATIVE_COUNTS_ERROR_NUM);
                Declare_DAC_Defective();
                break;
             }
             if (Status.Flags.Direct_Out_Count == SET_LOW && Entry1_Count ==  Exit1_Count &&
                 Entry2_Count ==  Exit2_Count && Entry1_Count ==  Entry2_Count && Exit1_Count == Exit2_Count)
             {
                /* Train cleared the section */
                   uiAuthorisationKey = Get_Relay_Energising_Key();
                   Energise_Vital_Relay_B(uiAuthorisationKey);
                   Relay_D3_Info.State = RELAY_MANAGER_IDLE;
                   DS_Section_Mode.Local_Unit  = SYSTEM_CLEAR_MODE;
                   DS_Section_Mode.Remote_Unit = DP3_MODE;
                   Relay_D3_Info.Track_Clearing_Timeout_50ms = LOCAL_COUNT_CLEARING_TIMEOUT_3;
                   B_Sec_Led_drive(ON);

             }
             break;
         case RELAY_MANAGER_IDLE:
               if(Relay_D3_Info.Track_Clearing_Timeout_50ms == TIMEOUT_EVENT)
                 {
                  Clear_US_AxleCount();
                  Clear_DS_AxleCount();
                  Relay_D3_Info.State = NO_TRAIN_IN_SECTION;
                 }
               break;
          default:
                break;
    }
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Check_For_D3_Direct_Out_Count(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :   Routine to check the direct out counts

Allocated Requiremnts:

Design Requirements:


Interfaces
    Calls           :   ERROR.C-Set_Error_Status_Bit()
                        RLYMGR.C-Declare_DAC_Defective()


    Called by       :   RLYD3_MGR.C-Update_Relay_D3_State()


Input Variables                     Name                Type
    Global          :   DIRECTION_NOT_DEFINED           Enumerator
                        Relay_D3_Info.A1_Direction      BYTE
                        Relay_D3_Info.A2_Direction      BYTE
                        Relay_D3_Info.B1_Direction      BYTE
                        Relay_D3_Info.B2_Direction      BYTE
                        Relay_D3_Info.C1_Direction      BYTE
                        Relay_D3_Info.C2_Direction      BYTE
                        FORWARD_DIRECTION               Enumerator
                        REVERSE_DIRECTION               Enumerator
                        Relay_D3_Info.Local_Direction   BYTE
                        Relay_D3_Info.Local_Count       UINT16
                        Relay_D3_Info.B1_Axle_Count     UINT16
                        Relay_D3_Info.C1_Axle_Count     UINT16
                        Relay_D3_Info.B2_Axle_Count     UINT16
                        Relay_D3_Info.C2_Axle_Count     UINT16
                        Relay_D3_Info.A1_Axle_Count     UINT16
                        Relay_D3_Info.A2_Axle_Count     UINT16


    Local           :   None

Output Variables                    Name                Type
    Global          :   Status.Flags.Direct_Out_Count   Bit

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        DIRECT_OUT_ERROR_NUM       43
                                        SET_HIGH                   1

References          :

Derived Requirements:

Algorithim          :   1.For D3 configuration train can enter in three directions namely A,B,C
                        2.Software shall check the same for both the CPU's hence check the direction for
                          both A1, A2, B1,B2,C1,C2
                        3.If the system at A has not received any axles direction but system at B or C
                          has started receiving the forward axle direction then there is a error of only in counts
                          & no out counts hence set the error bit to indicate the errror & declare as DAC deffective
                        4.If the system at B & C has not received any axle direction but the system at A has started
                          receiving the reverse axle direction then there is a error of only in counts
                          & no out counts hence set the error bit to indicate the errror & declare as DAC deffective
                        5.For the unit type D3A if the axles direction is reverse direction and axles counts are greater
                          than the counts received at remote units then there is a error of direct out count hence set the
                          error bit to indicate the error & declare as DAC deffective
                        6.For the unit type D3B if the axles direction is forward direction and axles counts are greater
                          than the counts received at remote unit(A) then there is a error of direct out count hence set the
                          error bit to indicate the error & declare as DAC deffective
                        7.For the unit type D3C if the axles direction is forward direction and axles counts are greater
                          than the counts received at remote unit(A) then there is a error of direct out count hence set the
                          error bit to indicate the error & declare as DAC deffective
*******************************************************************************/
void Check_For_D3_Direct_Out_Count(void)
{

    UINT16 Entry_Axle_Count,Exit_Axle_Count;


    Entry_Axle_Count = (Relay_D3_Info.A1_IN_Axle_Count  + Relay_D3_Info.B1_IN_Axle_Count + Relay_D3_Info.C1_IN_Axle_Count +
                        Relay_D3_Info.A2_IN_Axle_Count  + Relay_D3_Info.B2_IN_Axle_Count + Relay_D3_Info.C2_IN_Axle_Count);

    Exit_Axle_Count = (Relay_D3_Info.A1_OUT_Axle_Count  + Relay_D3_Info.B1_OUT_Axle_Count + Relay_D3_Info.C1_OUT_Axle_Count +
                       Relay_D3_Info.A2_OUT_Axle_Count  + Relay_D3_Info.B2_OUT_Axle_Count + Relay_D3_Info.C2_OUT_Axle_Count);

    if(Exit_Axle_Count > Entry_Axle_Count)
    {
        /* no in-counts, only out-counts,system goes to error state*/
            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
            Status.Flags.Direct_Out_Count = SET_HIGH;
            Declare_DAC_Defective();
    }

    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_D3_A :
            if(Relay_D3_Info.Local_Direction == (BYTE)REVERSE_DIRECTION )
              {
                if(Relay_D3_Info.Local_Count > (Relay_D3_Info.B1_IN_Axle_Count + Relay_D3_Info.C1_IN_Axle_Count))
                  {
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Declare_DAC_Defective();
                    break;
                  }
                if(Relay_D3_Info.Local_Count > (Relay_D3_Info.B2_IN_Axle_Count + Relay_D3_Info.C2_IN_Axle_Count))
                  {
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    Declare_DAC_Defective();
                    break;
                  }
              }
            break;
        case DAC_UNIT_TYPE_D3_B:
            if(Relay_D3_Info.Local_Direction == (BYTE)FORWARD_DIRECTION )
              {
                if(Relay_D3_Info.Local_Count > (Relay_D3_Info.A1_IN_Axle_Count + Relay_D3_Info.C1_IN_Axle_Count))
                  {
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    Declare_DAC_Defective();
                    break;
                  }
                if(Relay_D3_Info.Local_Count > (Relay_D3_Info.A2_IN_Axle_Count + Relay_D3_Info.C2_IN_Axle_Count))
                  {
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    Declare_DAC_Defective();
                    break;
                  }
              }
            break;
        case DAC_UNIT_TYPE_D3_C :
            if(Relay_D3_Info.Local_Direction == (BYTE)FORWARD_DIRECTION )
              {
                if(Relay_D3_Info.Local_Count > (Relay_D3_Info.A1_IN_Axle_Count + Relay_D3_Info.B1_IN_Axle_Count))
                   {
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    Declare_DAC_Defective();
                    break;
                  }
                if(Relay_D3_Info.Local_Count > (Relay_D3_Info.A2_IN_Axle_Count + Relay_D3_Info.B2_IN_Axle_Count))
                   {
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    Declare_DAC_Defective();
                    break;
                  }
              }
            break;
        default:
            break;
    }

}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :BOOL Reset_Allowed_For_D3()
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Function return TRUE if the system is defective or when it has
                     Forward/Reverse direction of axles which means that you can reset the system
Interfaces
    Calls           :   None

    Called by       :   RESET.C-Process_CF_Reset_Command()


Input Variables                     Name                Type
    Global          :   DAC_DEFECTIVE                   Enumerator
                        Relay_D3_Info.State             Enumerator
                        FORWARD_DIRECTION               Enumerator
                        REVERSE_DIRECTION               Enumerator
                        Relay_D3_Info.A1_Direction      BYTE
                        Relay_D3_Info.A2_Direction      BYTE
                        Relay_D3_Info.B1_Direction      BYTE
                        Relay_D3_Info.B2_Direction      BYTE
                        Relay_D3_Info.C1_Direction      BYTE
                        Relay_D3_Info.C2_Direction      BYTE

    Local           :   None

Output Variables                    Name                Type
    Global          :   None

    Local           :   ReturnValue                     BOOL
Signal Variables


Macros defined      :                   Macro                   Value
                                        FALSE                    0
                                        TRUE                     1

References          :

Derived Requirements:

*******************************************************************************/
BOOL Reset_Allowed_For_D3(void)
{
    BOOL ReturnValue = FALSE;


    if( Relay_D3_Info.State == DAC_DEFECTIVE)
    {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
    }
    if((Relay_D3_Info.A1_Direction == (BYTE)FORWARD_DIRECTION ||
       Relay_D3_Info.A2_Direction  == (BYTE)FORWARD_DIRECTION) &&
      (Relay_D3_Info.B1_Direction  == (BYTE)FORWARD_DIRECTION  ||
       Relay_D3_Info.B2_Direction  == (BYTE)FORWARD_DIRECTION  ||
       Relay_D3_Info.C1_Direction  == (BYTE)FORWARD_DIRECTION  ||
       Relay_D3_Info.C2_Direction  == (BYTE)FORWARD_DIRECTION  ))
      {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
      }
    if((Relay_D3_Info.A1_Direction  == (BYTE)REVERSE_DIRECTION ||
       Relay_D3_Info.A2_Direction   == (BYTE)REVERSE_DIRECTION) &&
       (Relay_D3_Info.B1_Direction  == (BYTE)REVERSE_DIRECTION  ||
       Relay_D3_Info.B2_Direction   == (BYTE)REVERSE_DIRECTION  ||
       Relay_D3_Info.C1_Direction   == (BYTE)REVERSE_DIRECTION  ||
       Relay_D3_Info.C2_Direction   == (BYTE)REVERSE_DIRECTION  ))
      {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
      }
    if((Relay_D3_Info.B1_Direction  == (BYTE)REVERSE_DIRECTION ||
       Relay_D3_Info.B2_Direction   == (BYTE)REVERSE_DIRECTION) &&
       (Relay_D3_Info.A1_Direction  == (BYTE)REVERSE_DIRECTION  ||
       Relay_D3_Info.A2_Direction   == (BYTE)REVERSE_DIRECTION  ||
       Relay_D3_Info.C1_Direction   == (BYTE)FORWARD_DIRECTION  ||
       Relay_D3_Info.C2_Direction   == (BYTE)FORWARD_DIRECTION ))
      {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
      }
    if((Relay_D3_Info.C1_Direction  == (BYTE)REVERSE_DIRECTION ||
       Relay_D3_Info.C2_Direction   == (BYTE)REVERSE_DIRECTION) &&
       (Relay_D3_Info.A1_Direction  == (BYTE)REVERSE_DIRECTION  ||
       Relay_D3_Info.A2_Direction   == (BYTE)REVERSE_DIRECTION  ||
       Relay_D3_Info.B1_Direction   == (BYTE)FORWARD_DIRECTION  ||
       Relay_D3_Info.B2_Direction   == (BYTE)FORWARD_DIRECTION ))
      {
        ReturnValue = (BOOL) TRUE;
        return(ReturnValue);
      }
        return(ReturnValue);
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A1_OUT_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3A units OUT counts in CPU1 of D3A
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A1_OUT_Count      UINT16

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_D3_A1_OUT_Count(UINT16 uiCount)
{
    Relay_D3_Info.A1_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A2_OUT_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3A units OUT counts in CPU2 of D3A


Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()


Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A2_OUT_Count      UINT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_A2_OUT_Count(UINT16 uiCount)
{
    Relay_D3_Info.A2_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A1_IN_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:



Abstract            :Update D3A units IN counts in CPU1 of D3A
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A1_IN_Count       UINT16

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:

*******************************************************************************/
void Update_D3_A1_IN_Count(UINT16 uiCount)
{
    Relay_D3_Info.A1_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A2_IN_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3A units IN counts in CPU2 of D3A
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A2_IN_Count       UINT16

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_A2_IN_Count(UINT16 uiCount)
{
    Relay_D3_Info.A2_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B1_OUT_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3B units OUT counts in CPU1 of D3B
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B1_OUT_Count      UINT16

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_B1_OUT_Count(UINT16 uiCount)
{
    Relay_D3_Info.B1_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B2_OUT_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3B units OUT counts in CPU2 of D3B

Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B2_OUT_Count      UINT16

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_B2_OUT_Count(UINT16 uiCount)
{
    Relay_D3_Info.B2_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B1_IN_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3B units IN counts in CPU1 of D3B
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B1_IN_Count       UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_B1_IN_Count(UINT16 uiCount)
{
    Relay_D3_Info.B1_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B2_IN_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3B units IN counts in CPU2 of D3B
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B2_IN_Count       UINT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_D3_B2_IN_Count(UINT16 uiCount)
{
    Relay_D3_Info.B2_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C1_OUT_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3C units OUT counts in CPU1 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C1_OUT_Count      UINT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_D3_C1_OUT_Count(UINT16 uiCount)
{
    Relay_D3_Info.C1_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C2_OUT_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3C units OUT counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C2_OUT_Count      UINT16

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C2_OUT_Count(UINT16 uiCount)
{
    Relay_D3_Info.C2_OUT_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C1_IN_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU1 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C1_IN_Count       UINT16

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C1_IN_Count(UINT16 uiCount)
{
    Relay_D3_Info.C1_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C2_IN_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Process_D3_Peer_AxleCount()
                        RLYD3_MGR.C-Process_D3_Remote_AxleCount()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C2_IN_Count       UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C2_IN_Count(UINT16 uiCount)
{
    Relay_D3_Info.C2_IN_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_Local_Count(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.Local_Count       UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/

void Update_D3_Local_Count(UINT16 uiCount)
{
    Relay_D3_Info.Local_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A1_IN_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A1_IN_Axle_Count      UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_A1_IN_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.A1_IN_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A1_OUT_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A1_OUT_Axle_Count     UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_A1_OUT_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.A1_OUT_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A2_IN_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A2_IN_Axle_Count      UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_A2_IN_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.A2_IN_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A2_OUT_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A2_OUT_Axle_Count     UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_A2_OUT_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.A2_OUT_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B1_IN_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B1_IN_Axle_Count      UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_B1_IN_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.B1_IN_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B1_OUT_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B1_OUT_Axle_Count     UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_B1_OUT_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.B1_OUT_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B2_IN_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B2_IN_Axle_Count      UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*****************************************************************************/
void Update_D3_B2_IN_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.B2_IN_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B2_OUT_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B2_OUT_Axle_Count     UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*****************************************************************************/
void Update_D3_B2_OUT_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.B2_OUT_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C1_IN_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C1_IN_Axle_Count      UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C1_IN_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.C1_IN_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C1_OUT_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C1_OUT_Axle_Count     UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C1_OUT_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.C1_OUT_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C2_IN_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C2_IN_Axle_Count      UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C2_IN_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.C2_IN_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C2_OUT_AxleCount(UINT16 uiCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units IN counts in CPU2 of D3C
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Remote_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   uiCount                         UINT16

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C2_OUT_Axle_Count     UINT16

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C2_OUT_AxleCount(UINT16 uiCount)
{
    Relay_D3_Info.C2_OUT_Axle_Count = uiCount;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A1_Direction(BYTE Direction)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3A units axle direction in CPU1 of D3A
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_Direction()
                        RLYD3_MGR.C-Process_D3_Remote_Direction()
                        RLYD3_MGR.C-Process_D3_Peer_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A1_Direction      BYTE

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_A1_Direction(BYTE Direction)
{
    Relay_D3_Info.A1_Direction = Direction;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_A2_Direction(BYTE Direction)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3A units axle direction in CPU2 of D3A
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_Direction()
                        RLYD3_MGR.C-Process_D3_Remote_Direction()
                        RLYD3_MGR.C-Process_D3_Peer_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.A2_Direction      BYTE

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/

void Update_D3_A2_Direction(BYTE Direction)
{
    Relay_D3_Info.A2_Direction = Direction;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B1_Direction(BYTE Direction)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3B units axle direction in CPU1 of D3B

Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_Direction()
                        RLYD3_MGR.C-Process_D3_Remote_Direction()
                        RLYD3_MGR.C-Process_D3_Peer_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B1_Direction      BYTE

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_B1_Direction(BYTE Direction)
{
    Relay_D3_Info.B1_Direction = Direction;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_B2_Direction(BYTE Direction)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3B units axle direction in CPU2 of D3B
Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_Direction()
                        RLYD3_MGR.C-Process_D3_Remote_Direction()
                        RLYD3_MGR.C-Process_D3_Peer_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.B2_Direction      BYTE

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_B2_Direction(BYTE Direction)
{
    Relay_D3_Info.B2_Direction = Direction;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C1_Direction(BYTE Direction)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update D3C units axle direction in CPU1 of D3C

Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_Direction()
                        RLYD3_MGR.C-Process_D3_Remote_Direction()
                        RLYD3_MGR.C-Process_D3_Peer_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C1_Direction      BYTE

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C1_Direction(BYTE Direction)
{
    Relay_D3_Info.C1_Direction = Direction;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_C2_Direction(BYTE Direction)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update D3C units axle direction in CPU2 of D3C


Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_Direction()
                        RLYD3_MGR.C-Process_D3_Remote_Direction()
                        RLYD3_MGR.C-Process_D3_Peer_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.C2_Direction      BYTE

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Update_D3_C2_Direction(BYTE Direction)
{
    Relay_D3_Info.C2_Direction = Direction;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Update_D3_Local_Direction(BYTE Direction)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :De-energise the Vital Relay B for D3 units(A,B,C)
                     to declare it as defective

Interfaces
    Calls           :   None

    Called by       :   RLYD3_MGR.C-Process_D3_Local_Direction()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Direction                       BYTE

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.Local_Direction   BYTE

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Update_D3_Local_Direction(BYTE Direction)
{
    Relay_D3_Info.Local_Direction = Direction;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Set_Relay_D3_DAC_Defective(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :De-energise the Vital Relay B for D3 units(A,B,C)
                     to declare it as defective

Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Vital_Relay_B()

    Called by       :   RELAYMGR.C-Declare_DAC_Defective()
                        RELAYMGR.C-Declare_DAC_Defective_US()
                        RELAYMGR.C-Declare_DAC_Defective_DS()

Input Variables                     Name                Type
    Global          :   DAC_DEFECTIVE                   Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :   Relay_D3_Info.State             Enumerator

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void Set_Relay_D3_DAC_Defective(void)
{
    DeEnergise_Vital_Relay_B();
    Relay_D3_Info.State = DAC_DEFECTIVE;
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Process_D3_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Process the Local axle counts for D3 units

Interfaces
    Calls           :   RLYD3_MGR.C-Update_D3_A1_IN_Count()
                        RLYD3_MGR.C-Update_D3_A1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_A2_IN_Count()
                        RLYD3_MGR.C-Update_D3_A2_OUT_Count()
                        RLYD3_MGR.C-Update_D3_B1_IN_Count()
                        RLYD3_MGR.C-Update_D3_B1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_B2_IN_Count()
                        RLYD3_MGR.C-Update_D3_B2_OUT_Count()
                        RLYD3_MGR.C-Update_D3_C1_IN_Count()
                        RLYD3_MGR.C-Update_D3_C2_IN_Count()
                        RLYD3_MGR.C-Update_D3_C1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_C2_OUT_Count()

    Called by       :   RELAYMGR.C-Update_Relay_D3_Counts()


Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_DAC_CPU1   Bit

    Local           :       uiFwdAxleCount              UINT16
                            uiRevAxleCount              UINT16

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1
References          :

Derived Requirements:

Algorithm           :1.If it is D3A unit type & local unit, check whether the CPU1 flag of D3A is set.
                       If set update IN/OUT counts in CPU1 else update IN/OUT counts in CPU2 of D3A unit
                     2.If it is D3B unit type & local unit, check whether the CPU1 flag of D3B is set.
                       If set update IN/OUT counts in CPU1 else update IN/OUT counts in CPU2 of D3B unit
                     3.If it is D3C unit type & local unit, check whether the CPU1 flag of D3C is set.
                       If set update IN/OUT counts in CPU1 else update IN/OUT counts in CPU2 of D3C unit


*******************************************************************************/
void Process_D3_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{
    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
        case DAC_UNIT_TYPE_EF:
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            break;
        case DAC_UNIT_TYPE_D3_A :
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
                {
                    Update_D3_A1_IN_Count(uiFwdAxleCount);
                    Update_D3_A1_OUT_Count(uiRevAxleCount);
                }
                else
                {
                    Update_D3_A2_IN_Count (uiFwdAxleCount);
                    Update_D3_A2_OUT_Count(uiRevAxleCount);
                }
            break;
        case DAC_UNIT_TYPE_D3_B :
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
                {
                    Update_D3_B1_IN_Count (uiRevAxleCount);
                    Update_D3_B1_OUT_Count(uiFwdAxleCount);
                }
                else
                {
                    Update_D3_B2_IN_Count (uiRevAxleCount);
                    Update_D3_B2_OUT_Count(uiFwdAxleCount);
                }
            break;
        case DAC_UNIT_TYPE_D3_C :
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
                {
                    Update_D3_C1_IN_Count (uiRevAxleCount);
                    Update_D3_C1_OUT_Count(uiFwdAxleCount);
                }
                else
                {
                    Update_D3_C2_IN_Count (uiRevAxleCount);
                    Update_D3_C2_OUT_Count(uiFwdAxleCount);
                }
            break;

        }
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Process_D3_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update the Axle counts in peer cpu
Interfaces
    Calls           :   RLYD3_MGR.C-Update_D3_A1_IN_Count()
                        RLYD3_MGR.C-Update_D3_A1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_A2_IN_Count()
                        RLYD3_MGR.C-Update_D3_A2_OUT_Count()
                        RLYD3_MGR.C-Update_D3_B1_IN_Count()
                        RLYD3_MGR.C-Update_D3_B1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_B2_IN_Count()
                        RLYD3_MGR.C-Update_D3_B2_OUT_Count()
                        RLYD3_MGR.C-Update_D3_C1_IN_Count()
                        RLYD3_MGR.C-Update_D3_C2_IN_Count()
                        RLYD3_MGR.C-Update_D3_C1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_C2_OUT_Count()

    Called by       :   COMM_SM.C-Process_Interprocess_Message()
                        RELAYMGR.C-Update_Relay_D3_Counts()

Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_DAC_CPU1   Bit

    Local           :       uiFwdAxleCount              UINT16
                            uiRevAxleCount              UINT16

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1

References          :

Derived Requirements:


Algorithm           :1.If it is D3A unit type & local unit, check whether the CPU1 flag of D3A is set.
                       If set update IN/OUT counts in CPU2 else update IN/OUT counts in CPU1 of D3A unit
                     2.If it is D3B unit type & local unit, check whether the CPU1 flag of D3B is set.
                       If set update IN/OUT counts in CPU2 else update IN/OUT counts in CPU1 of D3B unit
                     3.If it is D3C unit type & local unit, check whether the CPU1 flag of D3C is set.
                       If set update IN/OUT counts in CPU2 else update IN/OUT counts in CPU1 of D3C unit


*******************************************************************************/
void Process_D3_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{
    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
        case DAC_UNIT_TYPE_EF:
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            break;
        case DAC_UNIT_TYPE_D3_A :
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
                {
                    Update_D3_A2_IN_Count(uiFwdAxleCount);
                    Update_D3_A2_OUT_Count(uiRevAxleCount);
                }
                else
                {
                    Update_D3_A1_IN_Count (uiFwdAxleCount);
                    Update_D3_A1_OUT_Count(uiRevAxleCount);
                }
            break;
        case DAC_UNIT_TYPE_D3_B :
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
                {
                    Update_D3_B2_IN_Count (uiRevAxleCount);
                    Update_D3_B2_OUT_Count(uiFwdAxleCount);
                }
                else
                {
                    Update_D3_B1_IN_Count (uiRevAxleCount);
                    Update_D3_B1_OUT_Count(uiFwdAxleCount);
                }
            break;
        case DAC_UNIT_TYPE_D3_C :
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
                {
                    Update_D3_C2_IN_Count (uiRevAxleCount);
                    Update_D3_C2_OUT_Count(uiFwdAxleCount);
                }
                else
                {
                    Update_D3_C1_IN_Count (uiRevAxleCount);
                    Update_D3_C1_OUT_Count(uiFwdAxleCount);
                }
            break;
        }
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Process_D3_Remote_AxleCount(bitadrb_t SrcAdr,DAC_Unit_Type Src_Unit_Type,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update the remote unit Axle count
Interfaces
    Calls           :   RLYD3_MGR.C-Update_D3_A1_IN_Count()
                        RLYD3_MGR.C-Update_D3_A1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_A2_IN_Count()
                        RLYD3_MGR.C-Update_D3_A2_OUT_Count()
                        RLYD3_MGR.C-Update_D3_B1_IN_Count()
                        RLYD3_MGR.C-Update_D3_B1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_B2_IN_Count()
                        RLYD3_MGR.C-Update_D3_B2_OUT_Count()
                        RLYD3_MGR.C-Update_D3_C1_IN_Count()
                        RLYD3_MGR.C-Update_D3_C2_IN_Count()
                        RLYD3_MGR.C-Update_D3_C1_OUT_Count()
                        RLYD3_MGR.C-Update_D3_C2_OUT_Count()

    Called by       :   COMM_DS.C-Process_DS_Axle_Count_Message()
                        COMM_US.C-Process_US_Axle_Count_Message()

Input Variables                     Name                Type
    Global          :   None

    Local           :   SrcAdr                          bitadrb_t
                        Src_Unit_Type                   DAC_Unit_Type
                        uiFwdAxleCount                  UINT16
                        uiRevAxleCount                  UINT16
                        SrcAdr.Bit.b0                   Bit



Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1
References          :

Derived Requirements:


Algorithm           :1.If it is D3A unit type & remote unit, check whether the CPU1 flag of D3A is set.
                       If set update IN/OUT counts in CPU1 else update IN/OUT counts in CPU2 of D3A unit
                     2.If it is D3B unit type & remote unit, check whether the CPU1 flag of D3B is set.
                       If set update IN/OUT counts in CPU1 else update IN/OUT counts in CPU2 of D3B unit
                     3.If it is D3C unit type & remote unit, check whether the CPU1 flag of D3C is set.
                       If set update IN/OUT counts in CPU1 else update IN/OUT counts in CPU2 of D3C unit


*******************************************************************************/
void Process_D3_Remote_AxleCount(bitadrb_t SrcAdr,SSDAC_Unit_Type Src_Unit_Type,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{
    switch (Src_Unit_Type)
      {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
        case DAC_UNIT_TYPE_EF:
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            break;
        case DAC_UNIT_TYPE_D3_A:
                if (SrcAdr.Bit.b0 == SET_HIGH)
                {
                    Update_D3_A1_IN_Count(uiFwdAxleCount);
                    Update_D3_A1_OUT_Count(uiRevAxleCount);
                }
                else
                {
                    Update_D3_A2_IN_Count(uiFwdAxleCount);
                    Update_D3_A2_OUT_Count(uiRevAxleCount);
                }
            break;
        case DAC_UNIT_TYPE_D3_B:
                if (SrcAdr.Bit.b0 == SET_HIGH)
                {
                    Update_D3_B1_IN_Count (uiRevAxleCount);
                    Update_D3_B1_OUT_Count(uiFwdAxleCount);
                }
                else
                {
                    Update_D3_B2_IN_Count (uiRevAxleCount);
                    Update_D3_B2_OUT_Count(uiFwdAxleCount);
                }
            break;
        case DAC_UNIT_TYPE_D3_C:
                if (SrcAdr.Bit.b0 == SET_HIGH)
                {
                    Update_D3_C1_IN_Count (uiRevAxleCount);
                    Update_D3_C1_OUT_Count(uiFwdAxleCount);
                }
                else
                {
                    Update_D3_C2_IN_Count (uiRevAxleCount);
                    Update_D3_C2_OUT_Count(uiFwdAxleCount);
                }
            break;
     }

}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Process_D3_Local_Direction(BYTE uchDirection,UINT16 uiAxleCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update local unit direction
Interfaces
    Calls           :   RLYD3_MGR.C-Update_D3_A1_Direction()
                        RLYD3_MGR.C-Update_D3_A2_Direction()
                        RLYD3_MGR.C-Update_D3_B1_Direction()
                        RLYD3_MGR.C-Update_D3_B2_Direction()
                        RLYD3_MGR.C-Update_D3_C1_Direction()
                        RLYD3_MGR.C-Update_D3_C2_Direction()
                        RLYD3_MGR.C-Update_D3_Local_Count()
                        RLYD3_MGR.C-Update_D3_Local_Direction()


    Called by       :   RELAYMGR.C-Update_Relay_D3_Counts()

Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_DAC_CPU1   Bit

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


Algorithm           :1.If it is D3A unit type & local unit, check whether the CPU1 flag of D3A is set.
                       If set update the direction of axles in CPU1 else update the direction of axles
                       in CPU2 of D3A unit
                     2.If it is D3B unit type & local unit, check whether the CPU1 flag of D3B is set.
                       If set update the direction of axles in CPU1 else update the direction of axles
                       in CPU2 of D3B unit
                     3.If it is D3C unit type & local unit, check whether the CPU1 flag of D3C is set.
                       If set update the direction of axles in CPU1 else update the direction of axles
                       in CPU2 of D3C unit
*******************************************************************************/
void Process_D3_Local_Direction(BYTE uchDirection,UINT16 uiAxleCount)
{

    Update_D3_Local_Count(uiAxleCount);
    Update_D3_Local_Direction(uchDirection);
    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
        case DAC_UNIT_TYPE_EF:
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            break;
        case DAC_UNIT_TYPE_D3_A :
            if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
            {
                Update_D3_A1_Direction(uchDirection);
                if(Relay_D3_Info.A1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_A1_IN_AxleCount(uiAxleCount);
                        Update_D3_A1_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.A1_Direction  == (BYTE)FORWARD_DIRECTION)
                {
                    Update_D3_A1_IN_AxleCount(uiAxleCount);
                }
                if(Relay_D3_Info.A1_Direction  == (BYTE)REVERSE_DIRECTION)
                {
                    Update_D3_A1_OUT_AxleCount(uiAxleCount);
                 }
            }
            else
            {
                Update_D3_A2_Direction(uchDirection);
                if(Relay_D3_Info.A2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_A2_IN_AxleCount(uiAxleCount);
                        Update_D3_A2_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.A2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_A2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.A2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_A2_OUT_AxleCount(uiAxleCount);
                    }
            }
            break;
        case DAC_UNIT_TYPE_D3_B :
            if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
            {
                Update_D3_B1_Direction(uchDirection);
                if(Relay_D3_Info.B1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_B1_IN_AxleCount(uiAxleCount);
                        Update_D3_B1_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.B1_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_B1_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B1_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_B1_OUT_AxleCount(uiAxleCount);
                    }
            }
            else
            {
                Update_D3_B2_Direction(uchDirection);
                    if(Relay_D3_Info.B2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_B2_IN_AxleCount(uiAxleCount);
                        Update_D3_B2_OUT_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_B2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_B2_OUT_AxleCount(uiAxleCount);
                    }
            }
            break;
        case DAC_UNIT_TYPE_D3_C :
            if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
            {
                Update_D3_C1_Direction(uchDirection);
                if(Relay_D3_Info.C1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_C1_IN_AxleCount(uiAxleCount);
                        Update_D3_C1_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.C1_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_C1_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.C1_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_C1_OUT_AxleCount(uiAxleCount);
                    }
            }
            else
            {
                Update_D3_C2_Direction(uchDirection);
                    if(Relay_D3_Info.C2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_C2_IN_AxleCount(uiAxleCount);
                        Update_D3_C2_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.C2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_C2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.C2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_C2_OUT_AxleCount(uiAxleCount);
                    }
            }
            break;
    }
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void  Process_D3_Peer_Direction(BYTE uchDirection,UINT16 uiAxleCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:


Abstract            :Update the direction in peer cpu
Interfaces
    Calls           :   RLYD3_MGR.C-Update_D3_A1_Direction()
                        RLYD3_MGR.C-Update_D3_A2_Direction()
                        RLYD3_MGR.C-Update_D3_B1_Direction()
                        RLYD3_MGR.C-Update_D3_B2_Direction()
                        RLYD3_MGR.C-Update_D3_C1_Direction()
                        RLYD3_MGR.C-Update_D3_C2_Direction()

    Called by       :   COMM_SM.C-Process_Interprocess_Message()

Input Variables                     Name                Type
    Global          :   DIP_Switch_Info.Flags.Is_DAC_CPU1   Bit

    Local           :   uchDirection                    BYTE

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1

References          :

Derived Requirements:


Algorithm           :1.If it is D3A unit type & local unit, check whether the CPU1 flag of D3A is set.
                       If set update the direction of axles in CPU2 else update the direction of axles
                       in CPU1 of D3A unit
                     2.If it is D3B unit type & local unit, check whether the CPU1 flag of D3B is set.
                       If set update the direction of axles in CPU2 else update the direction of axles in
                       CPU1 of D3B unit
                     3.If it is D3C unit type & local unit, check whether the CPU1 flag of D3C is set.
                       If set update the direction of axles in CPU2 else update the direction of axles
                       in CPU1 of D3C unit


*******************************************************************************/
void  Process_D3_Peer_Direction(BYTE uchDirection,UINT16 uiAxleCount)
{
    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
        case DAC_UNIT_TYPE_EF:
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            break;
        case DAC_UNIT_TYPE_D3_A :
            if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
            {
                Update_D3_A2_Direction(uchDirection);
                if(Relay_D3_Info.A2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_A2_IN_AxleCount(uiAxleCount);
                        Update_D3_A2_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.A2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_A2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.A2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_A2_OUT_AxleCount(uiAxleCount);
                    }
            }
            else
            {
                Update_D3_A1_Direction(uchDirection);
                if(Relay_D3_Info.A1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_A1_IN_AxleCount(uiAxleCount);
                        Update_D3_A1_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.A1_Direction  == (BYTE)FORWARD_DIRECTION)
                {
                    Update_D3_A1_IN_AxleCount(uiAxleCount);
                }
                 if(Relay_D3_Info.A1_Direction  == (BYTE)REVERSE_DIRECTION)
                 {
                    Update_D3_A1_OUT_AxleCount(uiAxleCount);
                 }
            }
            break;
        case DAC_UNIT_TYPE_D3_B :
            if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
            {
                Update_D3_B2_Direction(uchDirection);
                    if(Relay_D3_Info.B2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_B2_IN_AxleCount(uiAxleCount);
                        Update_D3_B2_OUT_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_B2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_B2_OUT_AxleCount(uiAxleCount);
                    }
            }
            else
            {
                Update_D3_B1_Direction(uchDirection);
                    if(Relay_D3_Info.B1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_B1_IN_AxleCount(uiAxleCount);
                        Update_D3_B1_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.B1_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_B1_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B1_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_B1_OUT_AxleCount(uiAxleCount);
                    }
            }
            break;
        case DAC_UNIT_TYPE_D3_C :
            if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
            {
                Update_D3_C2_Direction(uchDirection);
                    if(Relay_D3_Info.C2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_C2_IN_AxleCount(uiAxleCount);
                        Update_D3_C2_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.C2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_C2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.C2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_C2_OUT_AxleCount(uiAxleCount);
                    }
            }
            else
            {
                Update_D3_C1_Direction(uchDirection);
                if(Relay_D3_Info.C1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_C1_IN_AxleCount(uiAxleCount);
                        Update_D3_C1_OUT_AxleCount(uiAxleCount);
                    }
                if(Relay_D3_Info.C1_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_C1_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.C1_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_C1_OUT_AxleCount(uiAxleCount);
                    }
            }
            break;
    }
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Process_D3_Remote_Direction(bitadrb_t SrcAdr ,DAC_Unit_Type Src_Unit_Type, BYTE uchDirection)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :Update the Direction
Interfaces
    Calls           :   RLYD3_MGR.C-Update_D3_A1_Direction()
                        RLYD3_MGR.C-Update_D3_A2_Direction()
                        RLYD3_MGR.C-Update_D3_B1_Direction()
                        RLYD3_MGR.C-Update_D3_B2_Direction()
                        RLYD3_MGR.C-Update_D3_C1_Direction()
                        RLYD3_MGR.C-Update_D3_C2_Direction()
                        RLYD3_MGR.C-Update_D3_A1_AxleCount()
                        RLYD3_MGR.C-Update_D3_A2_AxleCount()
                        RLYD3_MGR.C-Update_D3_B1_AxleCount()
                        RLYD3_MGR.C-Update_D3_B2_AxleCount()
                        RLYD3_MGR.C-Update_D3_C1_AxleCount()
                        RLYD3_MGR.C-Update_D3_C2_AxleCount()

    Called by       :   COMM_DS.C-Process_DS_Axle_Count_Message()
                        COMM_US.C-Process_US_Axle_Count_Message()

Input Variables                     Name                Type
    Global          :   None

    Local           :   SrcAdr                          bitadrb_t
                        Src_Unit_Type                   DAC_Unit_Type
                        uchDirection                    BYTE
                        SrcAdr.Bit.b0                   Bit
                        uiAxleCount                     UINT16

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        SET_HIGH                  1

References          :

Derived Requirements:

Algorithm           :1.If it is D3A unit type & remote unit, check whether the CPU1 flag of D3A is set.
                       If set update the direction of axles in CPU1 else update the direction of axles
                       in CPU2 of D3A unit
                     2.If it is D3B unit type & remote unit, check whether the CPU1 flag of D3B is set.
                       If set update the direction of axles in CPU1 else update the direction of axles
                       in CPU2 of D3B unit
                     3.If it is D3C unit type & local unit, check whether the CPU1 flag of D3C is set.
                       If set update the direction of axles  in CPU1 else update the direction of axles
                       in CPU2 of D3C unit


*******************************************************************************/
void Process_D3_Remote_Direction(bitadrb_t SrcAdr ,SSDAC_Unit_Type Src_Unit_Type, BYTE uchDirection,UINT16 uiAxleCount)
{
    switch (Src_Unit_Type)
    {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
        case DAC_UNIT_TYPE_EF:
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            break;
        case DAC_UNIT_TYPE_D3_A:
                if (SrcAdr.Bit.b0 == SET_HIGH)
                {
                    Update_D3_A1_Direction(uchDirection);
                    if(Relay_D3_Info.A1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_A1_IN_AxleCount(uiAxleCount);
                        Update_D3_A1_OUT_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.A1_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_A1_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.A1_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_A1_OUT_AxleCount(uiAxleCount);
                    }
                }
                else
                {
                    Update_D3_A2_Direction(uchDirection);
                    if(Relay_D3_Info.A2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_A2_IN_AxleCount(uiAxleCount);
                        Update_D3_A2_OUT_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.A2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_A2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.A2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_A2_OUT_AxleCount(uiAxleCount);
                    }
                }
            break;
        case DAC_UNIT_TYPE_D3_B:
                if (SrcAdr.Bit.b0 == SET_HIGH)
                {
                    Update_D3_B1_Direction(uchDirection);
                    if(Relay_D3_Info.B1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_B1_IN_AxleCount(uiAxleCount);
                        Update_D3_B1_OUT_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B1_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_B1_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B1_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_B1_OUT_AxleCount(uiAxleCount);
                    }
                }
                else
                {
                    Update_D3_B2_Direction(uchDirection);
                    if(Relay_D3_Info.B2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_B2_IN_AxleCount(uiAxleCount);
                        Update_D3_B2_OUT_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_B2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.B2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_B2_OUT_AxleCount(uiAxleCount);
                    }
                }
            break;
        case DAC_UNIT_TYPE_D3_C:
                if (SrcAdr.Bit.b0 == SET_HIGH)
                {
                    Update_D3_C1_Direction(uchDirection);
                    if(Relay_D3_Info.C1_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_C1_IN_AxleCount(uiAxleCount);
                        Update_D3_C1_OUT_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.C1_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_C1_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.C1_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_C1_OUT_AxleCount(uiAxleCount);
                    }
                }
                else
                {
                    Update_D3_C2_Direction(uchDirection);
                    if(Relay_D3_Info.C2_Direction  == (BYTE)DIRECTION_NOT_DEFINED)
                    {
                        Update_D3_C2_IN_AxleCount(uiAxleCount);
                        Update_D3_C2_OUT_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.C2_Direction  == (BYTE)REVERSE_DIRECTION)
                    {
                        Update_D3_C2_IN_AxleCount(uiAxleCount);
                    }
                    if(Relay_D3_Info.C2_Direction  == (BYTE)FORWARD_DIRECTION)
                    {
                        Update_D3_C2_OUT_AxleCount(uiAxleCount);
                    }
                }
            break;
      }
}
/******************************************************************************
Component name      :RLYD3_MGR
Module Name         :void Decrement_D3_Track_Clearing_Timeout_50msTmr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Allocated Requiremnts:

Design Requirements:

Abstract            :
Interfaces
    Calls           :

    Called by       :
Input Variables                     Name                            Type
    Global          :   Relay_D3_Info.Track_Clearing_Timeout_50ms   BYTE

    Local           :

Output Variables                    Name                            Type
    Global          :   Relay_D3_Info.Track_Clearing_Timeout_50ms   BYTE

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value


References          :

Derived Requirements:

******************************************************************************/
void Decrement_D3_Track_Clearing_Timeout_50msTmr(void)
{
 if(Relay_D3_Info.Track_Clearing_Timeout_50ms > 0)
  {
   Relay_D3_Info.Track_Clearing_Timeout_50ms = Relay_D3_Info.Track_Clearing_Timeout_50ms - 1;
  }
}


BYTE Get_Relay_D3_ATC_State(void)
{
    return(Relay_D3_Info.ATC_Local_State);
}

BYTE Get_Relay_D3_State(void)
{
 return((BYTE)Relay_D3_Info.State);
}

void Update_ATC_Remote1_Relay_D3_State(BYTE Remote_state)
{
    Relay_D3_Info.ATC_Remote1_State = Remote_state;
}
void Update_ATC_Remote2_Relay_D3_State(BYTE Remote_state)
{
    Relay_D3_Info.ATC_Remote2_State = Remote_state;
}
void Update_ATC_Remote3_Relay_D3_State(BYTE Remote_state)
{
    Relay_D3_Info.ATC_Remote3_State = Remote_state;
}
void Update_ATC_Remote4_Relay_D3_State(BYTE Remote_state)
{
    Relay_D3_Info.ATC_Remote4_State = Remote_state;
}
