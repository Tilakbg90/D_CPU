/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   AXLE_MON
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
    Functions   :   void Initialise_AxleMon(void)
                    void Validate_PD_Signals(void)
                    void Monitor_Supervisory_Pulse(void)
                    void Monitor_Wheel_Pulse_4(void)
                    void Determine_TrackState_4(BYTE uchPD_IO_Value)
                    void Analyse_Supervisory_Sequence_4(void)
                    void Detect_PD_Failures_4(void)
                    void Increment_DS_Forward_Count(void)
                    void Decrement_DS_Forward_Count(void)
                    void Increment_DS_Reverse_Count(void)
                    void Decrement_DS_Reverse_Count(void)
                    void Increment_US_Forward_Count(void)
                    void Decrement_US_Forward_Count(void)
                    void Increment_US_Reverse_Count(void)
                    void Decrement_US_Reverse_Count(void)
                    void Register_PD1212_Transition_4(void)
                    void Register_PD2121_Transition_4(void)
                    void Chk_for_AxleCount_Completion(void)
                    void Check_for_PD_Error(void)
                    void Decrement_TrackMon_50msTmr(void)
                    void Clear_US_AxleCount(void)
                    void Clear_DS_AxleCount(void)
                    UINT16 Get_US_AxleCount(void)
                    UINT16 Get_DS_AxleCount(void)
                    BYTE Get_US_AxleDirection(void)
                    BYTE Get_DS_AxleDirection(void)
                    BYTE Get_Shunting_State(void)
                    UINT16 Get_US_Fwd_AxleCount(void)
                    UINT16 Get_US_Rev_AxleCount(void)
                    UINT16 Get_DS_Fwd_AxleCount(void)
                    UINT16 Get_DS_Rev_AxleCount(void)
                    void Clear_DS_Local_Counts(void)
                    void Clear_US_Local_Counts(void)
                    void Clear_PD1_Supervisory_Count_4(void)
                    void Clear_PD2_Supervisory_Count_4(void)
                    void Clear_PD12_Main_Pulse_Count_4(void)
                    void Clear_Wheel_Type_4(void)

*****************************************************************************/
#include <xc.h>
#include <string.h>

#include "COMMON.h"
#include "AXLE_MON.h"
#include "ERROR.h"
#include "RESTORE.h"

extern  /*near*/  dac_status_t Status;                /* from dac_main.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from DAC_MAIN.c */
 /*near*/  track_info_t Track_Info_4;
extern fdp_info FDP_Info;


extern const BYTE uchPD_Transition_Table[NO_OF_TRACK_PROCESS_STATES][NO_OF_PD_TRANSITIONS];

void Initialise_AxleMon_4(void);
void Validate_PD_Signals_4(void);
void Monitor_Supervisory_Pulse_4(void);
void Monitor_Wheel_Pulse_4(bitadrb_t);
void Determine_TrackState_4(BYTE);
void Analyse_Supervisory_Sequence_4(void);
void Register_PD1212_Transition_4(void);
void Register_PD2121_Transition_4(void);
void Clear_PD1_Supervisory_Count_4(void);
void Clear_PD2_Supervisory_Count_4(void);
void Clear_PD12_Main_Pulse_Count_4(void);
BOOL Chk_For_Track_Occupancy_4(void);
void Clear_Wheel_Type_4(void);
void Detect_PD_Failures_4(void);
void Clear_DS_AxleCount_4(void);
void Decrement_TrackMon_50msTmr_4(void);

/*********************************************************************
Component name      :AXLE_MON
Module Name         :BYTE Get_Shunting_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the train shunting state at the phasw detectors

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   RLYDE_MGR.c -   Reset_Allowed_For_DE()

Input Variables         Name                                Type
    Global          :   None

    Local           :   None

Output Variables        Name                                Type
    Global          :   Track_Info_4.Flags.Train_Shunting       BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:
************************************************************************/
//BYTE Get_Shunting_State(void)
//{
//  return((BYTE)Track_Info_4.Flags.Train_Shunting);
//}

/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Initialise_AxleMon(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initailise axle monitoring by clear the direction and axle
                     count(Forward and reverse). If both the PDs signal is
                     HIGH assign "WAITING_FOR_AXLE" to track state else assign
                     "OUT_OF_SYNC_OR_PD_ERROR" to track state, declare DAC defective
                     and set pd state fail error id

Allocated Requirements  :
Design Requirements     :


Interfaces
    Calls           :   RELAYMGR.c  -   Declare_DAC_Defective()
                        ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   DAC_MAIN.c  -   Initialise_System()

Input Variables             Name                            Type
    Global          :   PRE_SYNCHRONISATION_WAIT        Enumerator
                        DIRECTION_NOT_DEFINED           Enumerator
                        WHEEL_TYPE_NOT_DETERMINED       Enumerator
                        WAITING_FOR_AXLE                Enumerator
                        OUT_OF_SYNC_OR_PD_ERROR         Enumerator
    Local           :   None

Output Variables        Name                                                  Type
    Global          :   Track_Info_4.State                                      Enumerator
                        Track_Info_4.US_Axle_Direction                          BYTE
                        Track_Info_4.DS_Axle_Direction                          BYTE
                        Track_Info_4.US_Axle_Count                              UINT16
                        Track_Info_4.DS_Axle_Count                              UINT16
                        Track_Info_4.DS_Fwd_Axle_Count                          UINT16
                        Track_Info_4.DS_Rev_Axle_Count                          UINT16
                        Track_Info_4.US_Fwd_Axle_Count                          UINT16
                        Track_Info_4.US_Rev_Axle_Count                          UINT16
                        Track_Info_4.PD1M_Count                                 UINT16
                        Track_Info_4.PD2M_Count                                 UINT16
                        Track_Info_4.PD1S_Count                                 UINT16
                        Track_Info_4.PD2S_Count                                 UINT16
                        Track_Info_4.PD1S_Falling_Edge_Count                    UINT16
                        Track_Info_4.PD2S_Falling_Edge_Count                    UINT16
                        Track_Info_4.PD1S_Rising_Edge_Count                     UINT16
                        Track_Info_4.PD2S_Rising_Edge_Count                     UINT16
                        Track_Info_4.Wheel_Type                                 Enumarartor
                        Track_Info_4.Flags.PD_State_Counted                     Bit
                        Track_Info_4.Flags.Train_Shunting                       Bit
                        Track_Info_4.Flags.PD1S_Timer_On                        Bit
                        Track_Info_4.Flags.PD2S_Timer_On                        Bit
                        Track_Info_4.Flags.PD1S_Port_Latch                      Bit
                        Track_Info_4.Flags.PD2S_Port_Latch                      Bit
                        Track_Info_4.Flags.PD1M_Port_Latch                      Bit
                        Track_Info_4.Flags.PD2M_Port_Latch                      Bit
                        Track_Info_4.Flags.US_Counting_Enable                   Bit
                        Track_Info_4.Flags.DS_Counting_Enable                   Bit
                        Track_Info_4.PD_Last_Change_Timeout_50ms                BYTE
                        Track_Info_4.PD1S_Timeout_50ms                          UNIT16
                        Track_Info_4.PD2S_Timeout_50ms                          UNIT16
                        Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms BYTE
                        Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms BYTE
                        Track_Info_4.Trolley_Timeout_50ms                       BYTE

    Local           :   None
Signal Variables

                                PD1_PULSE_MAIN                  Input                   RB4
                                PD2_PULSE_MAIN                  Input                   RB5
                                PD1_SUPERVISORY_PULSE           Input                   RB0
                                PD2_SUPERVISORY_PULSE           Input                   RB1

Macro definitions used:     Macro                           Value
                            SET_HIGH                        1
                            SET_LOW                         0
                            BOOTUP_AD_FAIL_ERROR_NUM               29
References          :

Derived Requirements:

Algorithm           :1.Initialise all the Track_Info variables
                     2.Set the Track_Info state as "WAITING FOR AXLE" if all the
                        main,secondary,supervisory pulses are at HIGH otherwise
                        set the state as "OUT_OF_SYNC_OR_PD_ERROR" ,declare DAC defective
                        and set the error status bit"BOOTUP_AD_FAIL_ERROR_NUM"
************************************************************************/
void Initialise_AxleMon_4(void)
{
    Track_Info_4.State = PRE_SYNCHRONISATION_WAIT;
    Track_Info_4.US_Axle_Direction = DIRECTION_NOT_DEFINED;
    Track_Info_4.DS_Axle_Direction = DIRECTION_NOT_DEFINED;
    Track_Info_4.US_Axle_Count = 0;
    Track_Info_4.DS_Axle_Count = 0;

    Track_Info_4.LCWS_Axle_Direction = DIRECTION_NOT_DEFINED;
    Track_Info_4.LCWS_Count = 0;
    Track_Info_4.LCWS_Fwd_Count = 0;
    Track_Info_4.LCWS_Rev_Count = 0;
    Track_Info_4.LCWS_Total_Fwd_Count = 0;
    Track_Info_4.LCWS_Total_Rev_Count = 0;

    Track_Info_4.DS_Fwd_Axle_Count = 0;
    Track_Info_4.DS_Rev_Axle_Count = 0;
    Track_Info_4.US_Fwd_Axle_Count = 0;
    Track_Info_4.US_Rev_Axle_Count = 0;
    Track_Info_4.PD1M_Count = 0;
    Track_Info_4.PD2M_Count = 0;
    Track_Info_4.PD1S_Count = 0;
    Track_Info_4.PD2S_Count = 0;
    Track_Info_4.PD1S_Falling_Edge_Count = 0;
    Track_Info_4.PD2S_Falling_Edge_Count = 0;
    Track_Info_4.PD1S_Rising_Edge_Count  = 0;
    Track_Info_4.PD2S_Rising_Edge_Count  = 0;
    Track_Info_4.Wheel_Type = WHEEL_TYPE_NOT_DETERMINED;
    Track_Info_4.Flags.PD_State_Counted  = FALSE;
    Track_Info_4.Flags.PD1S_Timer_On = FALSE;
    Track_Info_4.Flags.PD2S_Timer_On = FALSE;
    Track_Info_4.Flags.PD1S_Port_Latch = SET_HIGH;
    Track_Info_4.Flags.PD2S_Port_Latch = SET_HIGH;
    Track_Info_4.Flags.PD1M_Port_Latch = SET_HIGH;
    Track_Info_4.Flags.PD2M_Port_Latch = SET_HIGH;
    Track_Info_4.Flags.US_Counting_Enable = SET_LOW;
    Track_Info_4.Flags.DS_Counting_Enable = SET_LOW;
    Track_Info_4.Flags.PD_Non_Overlapping = FALSE;
    Track_Info_4.Flags.PD_Without_Overlapping = FALSE;
    Track_Info_4.Flags.DS_Local_Counts_Just_Cleared = FALSE;
    Track_Info_4.Flags.US_Local_Counts_Just_Cleared = FALSE;
    Track_Info_4.Flags.FDP_US_Counting_Enable = SET_LOW;
    Track_Info_4.Flags.FDP_DS_Counting_Enable = SET_LOW;
    Track_Info_4.PD_Last_Change_Timeout_50ms = 0;
    Track_Info_4.PD1S_Timeout_50ms = 0;
    Track_Info_4.PD2S_Timeout_50ms = 0;
    Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms = 0;
    Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms = 0;
    Track_Info_4.Trolley_Timeout_50ms = 0;
    TRISBbits.TRISB9 = 1;
    TRISBbits.TRISB14 = 1;

    if(PORTBbits.RB9 == SET_HIGH && PORTBbits.RB14 == SET_HIGH)
        {
        /* Both Leading and Trailing are high, so not necessary to synchronise */
           Track_Info_4.State = WAITING_FOR_AXLE;
        }
    else
       {
          Track_Info_4.State = OUT_OF_SYNC_OR_PD_ERROR;
          Declare_DAC_Defective();
          Set_Error_Status_Bit(BOOTUP_AD_FAIL_ERROR_NUM);
       }
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Validate_PD_Signals(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Validate the PD signals.First monitor the supervisory signal.
                     If wheel is not detected means then analyse supervisory
                     sequence.Get the PD main siganls and secondary signals from the
                     two cards and determine the Track status.Monitor the wheel pulses
                     and check for the PD failures



Design Requirements     :


Interfaces
    Calls           :   AXLE_MON.c  -   Monitor_Supervisory_Pulse()
                        AXLE_MON.c  -   Analyse_Supervisory_Sequence_4()
                        AXLE_MON.c  -   Determine_TrackState_4()
                        AXLE_MON.c  -   Monitor_Wheel_Pulse_4()
                        AXLE_MON.c  -   Detect_PD_Failures_4()
                        RELAYMGR.c  -   Declare_DAC_Defective()
                        ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_DE()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()


Input Variables         Name                                Type
    Global          :   Track_Info_4.Wheel_Type               Enumerator

    Local           :   Track0_IO                           bitadrb_t
                        Temp_IO                             bitadrb_t

Output Variables        Name                                Type
    Global          :

    Local           :   None

Signal Variables

                                PD1_PULSE_MAIN                  Input                   RB4
                                PD2_PULSE_MAIN                  Input                   RB5
                                PD1_SUPERVISORY_PULSE           Input                   RB0
                                PD2_SUPERVISORY_PULSE           Input                   RB1

Macro definitions used:     Macro                           Value
                            AD_STATE_FAIL_ERROR_NUM                38

References          :

Derived Requirements:

Algorithm           :1. Check for the wheel type, if it is not determined
                        analyse the supervisory sequence for erroe detection
                     2. Check for the both main and secondary signal of both
                        the PD cards,if both are equal determine the track
                        state, monitor the wheel pulse and detect the PD failures
                     3. Before declaring error, Main and secondary pulses are
                        compared once again for equality Because there may be some
                        nano sec gap between main and secondary. By that time we
                        should not declare error.
*************************************************************************/
void Validate_PD_Signals_4(void)
{
    bitadrb_t Track0_IO;
    bitadrb_t Temp_IO;

    Monitor_Supervisory_Pulse_4();
    if(Track_Info_4.Wheel_Type == WHEEL_TYPE_NOT_DETERMINED)
      {
        Analyse_Supervisory_Sequence_4();
       }
    Track0_IO.Byte = (BYTE) 0;

        Temp_IO.Bit.b0 = PORTBbits.RB14;
        Temp_IO.Bit.b1 = PORTBbits.RB14;
        Temp_IO.Bit.b2 = PORTBbits.RB9;
        Temp_IO.Bit.b3 = PORTBbits.RB9;
        Track0_IO.Bit.b0 = (Temp_IO.Bit.b0 & Temp_IO.Bit.b1);
        Track0_IO.Bit.b1 = (Temp_IO.Bit.b2 & Temp_IO.Bit.b3);
        Determine_TrackState_4(Track0_IO.Byte);
        Monitor_Wheel_Pulse_4(Track0_IO);
        Detect_PD_Failures_4();

}

/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Monitor_Supervisory_Pulse(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Monitor the incoming supervisory signal pluse transitions
                     from the phase detector cards to detect the entry/exit of
                     a wheel axle at the phase detectors


Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Validate_PD_Signals()

Input Variables             Name                                Type
    Global          :   Track_Info_4.Flags.PD1S_Port_Latch        Bit
                        Track_Info_4.PD1S_Falling_Edge_Count      UINT16
                        Track_Info_4.PD1S_Count                   UINT16
                        Track_Info_4.PD1S_Rising_Edge_Count       UINT16
                        Track_Info_4.Flags.PD2S_Port_Latch        Bit
                        Track_Info_4.Flags.PD2S_Port_Latch        Bit
                        Track_Info_4.PD2S_Falling_Edge_Count      UINT16
                        Track_Info_4.PD2S_Rising_Edge_Count       UINT16
                        Track_Info_4.PD2S_Count                   UINT16

    Local           :   PortB_Buffer                            bitadrb_t
                        PortB_Buffer.Bit.b0                     Bit
                        PortB_Buffer.Bit.b1                     Bit

Output Variables            Name                                Type
    Global          :   Track_Info_4.Flags.PD1S_Timer_On          Bit
                        Track_Info_4.PD1S_Timeout_50ms            UNIT16
                        Track_Info_4.PD_Last_Change_Timeout_50ms  BYTE
                        Track_Info_4.PD1S_Falling_Edge_Count      UINT16
                        Track_Info_4.PD1S_Count                   UINT16
                        Track_Info_4.PD1S_Rising_Edge_Count       UINT16
                        Track_Info_4.Flags.PD1S_Port_Latch        Bit
                        Track_Info_4.Flags.PD2S_Timer_On          Bit
                        Track_Info_4.PD2S_Timeout_50ms            BYTE
                        Track_Info_4.PD2S_Falling_Edge_Count      UINT16
                        Track_Info_4.PD2S_Rising_Edge_Count       UINT16
                        Track_Info_4.PD2S_Count                   UINT16
                        Track_Info_4.Flags.PD2S_Port_Latch        Bit

    Local           :   None

Signal Variables

                                PD1_PULSE_MAIN                  Input                   RB4
                                PD2_PULSE_MAIN                  Input                   RB5
                                PD1_SUPERVISORY_PULSE           Input                   RB0
                                PD2_SUPERVISORY_PULSE           Input                   RB1
Macro definitions used:     Macro                           Value
                            SET_LOW                         0
                            SET_HIGH                        1
                            TRUE                            1
                            PD_SUPERVISORY_LOW_TIMEOUT      60
                            PD_CLEARING_TIMEOUT             120
References          :

Derived Requirements:

Algorithm           :1. Check for the Phase detector1 current supervisory signal and
                        previous supervisory signal, if it goes from HIGH to LOW, turn
                        on the PD1 superviosry timer On, assign PD1 supervisory timeout
                        to 3s and PD clearing timeout to 6s,Increment the PD1 falling
                        egde count
                     2. If Phase detector1 supervisory signal is goes from LOW to HIGH,
                        indicates that phase reversal is completed,turn on the PD1
                        superviosry timer On,assign PD1 supervisory timeout to zero
                        and PD clearing timeout to 6s,incremant the PD1 supervisory
                        count,Increment the PD1 rising egde count
                     3. Check for the Phase detector2 current supervisory signal and
                        previous supervisory signal, if it goes from HIGH to LOW, turn
                        on the PD2 superviosry timer On, assign PD2 supervisory timeout
                        to 3s and PD clearing timeout to 6s,Increment the PD2 falling
                        egde count
                     4. If Phase detector2 supervisory signal is goes from LOW to HIGH,
                        indicates that phase reversal is completed,turn on the PD2
                        superviosry timer On,assign PD2 supervisory timeout to zero
                        and PD clearing timeout to 6s,incremant the PD2 supervisory
                        count,Increment the PD2 rising egde count
************************************************************************/
void Monitor_Supervisory_Pulse_4(void)
{
    bitadrb_t PortB_Buffer;

    //PortB_Buffer.Byte = PORTB;  /* Read the Port into buffer */

    PortB_Buffer.Bit.b0 = 1;
    PortB_Buffer.Bit.b1 = 1;

    if (PortB_Buffer.Bit.b0 == SET_LOW)
    {
        if (Track_Info_4.Flags.PD1S_Port_Latch == SET_HIGH)
        {
            /*
             * Supervisory pulse transition from HIGH to LOW indicates train axle
             * has just started to enter/exit the magnetic field in PD range and
             * phase reversal has just begun.
             */
            Track_Info_4.Flags.PD1S_Timer_On = TRUE;
            Track_Info_4.PD1S_Timeout_50ms = PD_SUPERVISORY_LOW_TIMEOUT;
            Track_Info_4.PD_Last_Change_Timeout_50ms  = PD_CLEARING_TIMEOUT;
            Track_Info_4.PD1S_Falling_Edge_Count = Track_Info_4.PD1S_Falling_Edge_Count + 1;
        }
    }
    else
    {
        if (Track_Info_4.Flags.PD1S_Port_Latch == SET_LOW)
        {
            /*
             * Supervisory pulse transition from LOW to HIGH indicates that
             * phase reversal is completed and PDx_PULSE starts to change
             * (HIGH to LOW) or (LOW to HIGH).
             */
            Track_Info_4.Flags.PD1S_Timer_On = FALSE;
            Track_Info_4.PD1S_Timeout_50ms = 0;
            Track_Info_4.PD_Last_Change_Timeout_50ms  = PD_CLEARING_TIMEOUT;
            Track_Info_4.PD1S_Count = Track_Info_4.PD1S_Count + 1;
            Track_Info_4.PD1S_Rising_Edge_Count = Track_Info_4.PD1S_Rising_Edge_Count + 1;
        }
    }
    Track_Info_4.Flags.PD1S_Port_Latch = PortB_Buffer.Bit.b0;

    if (PortB_Buffer.Bit.b1 == SET_LOW)
    {
        if (Track_Info_4.Flags.PD2S_Port_Latch == SET_HIGH)
        {
            /*
             * Supervisory pulse transition from HIGH to LOW indicates train axle
             * has just started to enter/exit the magnetic field in PD range and
             * phase reversal has just begun.
             */
            Track_Info_4.Flags.PD2S_Timer_On = TRUE;
            Track_Info_4.PD2S_Timeout_50ms = PD_SUPERVISORY_LOW_TIMEOUT;
            Track_Info_4.PD_Last_Change_Timeout_50ms  = PD_CLEARING_TIMEOUT;
            Track_Info_4.PD2S_Falling_Edge_Count = Track_Info_4.PD2S_Falling_Edge_Count + 1;
        }
    }
    else
    {
        if (Track_Info_4.Flags.PD2S_Port_Latch == SET_LOW)
        {
            /*
             * Supervisory pulse transition from LOW to HIGH indicates that
             * phase reversal is completed and PDx_PULSE starts to change
             * (HIGH to LOW) or (LOW to HIGH).
             */
            Track_Info_4.Flags.PD2S_Timer_On = FALSE;
            Track_Info_4.PD2S_Timeout_50ms = 0;
            Track_Info_4.PD_Last_Change_Timeout_50ms  = PD_CLEARING_TIMEOUT;
            Track_Info_4.PD2S_Rising_Edge_Count = Track_Info_4.PD2S_Rising_Edge_Count + 1;
            Track_Info_4.PD2S_Count = Track_Info_4.PD2S_Count + 1;
        }
    }
    Track_Info_4.Flags.PD2S_Port_Latch = PortB_Buffer.Bit.b1;
}


/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Monitor_Wheel_Pulse_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Monitor the incoming main signal pulse transitions from
                     phase detector cards to detect the entry of the wheel
                     axle at the phase detectors


Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Validate_PD_Signals()

Input Variables             Name                                Type
    Global          :   Track_Info_4.Flags.PD1M_Port_Latch        Bit
                        Track_Info_4.PD1M_Count                   UINT16
                        Track_Info_4.Flags.PD2M_Port_Latch        Bit
                        Track_Info_4.PD2M_Count                   UINT16

    Local           :   Temp_IO                                  bitadrb_t

Output Variables            Name                                Type
    Global          :   Track_Info_4.PD1M_Count                   UINT16
                        Track_Info_4.Flags.PD1M_Port_Latch        Bit
                        Track_Info_4.PD2M_Count                   UINT16
                        Track_Info_4.Flags.PD2M_Port_Latch        Bit

    Local           :   None

Signal Variables

                                    Nil                             Nil                 Nil


Macro definitions used:     Macro                           Value
                            SET_LOW                         0
                            SET_HIGH                        1
References          :

Derived Requirements:

Algorithm           :1. If Phase detector1 main signal  transition from HIGH
                        to LOW indicates train axle going from Phase detector.
                        So PD1 pulse count is incremented
                     2. If Phase detector2 main signal  transition from HIGH
                        to LOW indicates train axle going from Phase detector.
                        So PD2 pulse count is incremented

************************************************************************/
void Monitor_Wheel_Pulse_4( bitadrb_t Temp_IO)
{

    if (Temp_IO.Bit.b1 == SET_HIGH &&
        Track_Info_4.Flags.PD1M_Port_Latch == SET_LOW)
    {
        /*
         * Main pulse transition from HIGH to LOW indicates train axle
         * Going away from Pd. So, Pulse count will be incrementd
        */
         Track_Info_4.PD1M_Count = Track_Info_4.PD1M_Count + 1;
    }
    Track_Info_4.Flags.PD1M_Port_Latch = Temp_IO.Bit.b1;

    if (Temp_IO.Bit.b0 == SET_HIGH &&
        Track_Info_4.Flags.PD2M_Port_Latch == SET_LOW)
    {
        /*
         * Main pulse transition from HIGH to LOW indicates train axle
         * Going away from Pd. So, Pulse count will be incrementd
        */
         Track_Info_4.PD2M_Count = Track_Info_4.PD2M_Count + 1;
    }
    Track_Info_4.Flags.PD2M_Port_Latch = Temp_IO.Bit.b0;
}

/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Determine_TrackState_4(BYTE uchPD_IO_Value)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Determine the track state on the entry of an axle
                     at the phse detectors, clear the supervisory counts,
                     register the PD counts or declare errors if any

                    WAITING_FOR_AXLE                    0
                    WAIT_FOR_AXLE_AT_TRAILING_PD        1
                    LET_AXLE_CLEAR_LEADING_PD_FIRST     2
                    LET_AXLE_CLEAR_TRAILING_PD_NEXT     3
                    DIR_CHANGE_FROM_FWD_TO_REV          4
                    WAIT_FOR_AXLE_AT_LEADING_PD         5
                    LET_AXLE_CLEAR_TRAILING_PD_FIRST    6
                    LET_AXLE_CLEAR_LEADING_PD_NEXT      7
                    DIR_CHANGE_FROM_REV_TO_FWD          8
                    PRE_SYNCHRONISATION_WAIT            9
                    OUT_OF_SYNC_OR_PD_ERROR             10

                     Forward counting logic
                     0------>1------>2----->3------>0
                     Reverse counting logic
                     0------> 5------>6------>7----->0


Design Requirements     :


Interfaces
    Calls           :   AXLE_MON.c  -   Clear_PD1_Supervisory_Count_4()
                        AXLE_MON.c  -   Clear_PD2_Supervisory_Count_4()
                        AXLE_MON.C  -   Register_PD1212_Transition_4()
                        AXLE_MON.c  -   Register_PD2121_Transition_4()
                        RELAYMGR.c  -   Declare_DAC_Defective()
                        ERROR.C     -   Set_Error_Status_Bit()

    Called by       :   AXLE_MON.c  -   Validate_PD_Signals()


Input Variables         Name                                    Type
    Global          :   WAITING_FOR_AXLE                        Enumerator
                        WAIT_FOR_AXLE_AT_TRAILING_PD            Enumerator
                        LET_AXLE_CLEAR_LEADING_PD_FIRST         Enumerator
                        LET_AXLE_CLEAR_TRAILING_PD_NEXT         Enumerator
                        DIR_CHANGE_FROM_FWD_TO_REV              Enumerator
                        WAIT_FOR_AXLE_AT_LEADING_PD             Enumerator
                        LET_AXLE_CLEAR_TRAILING_PD_FIRST        Enumerator
                        LET_AXLE_CLEAR_LEADING_PD_NEXT          Enumerator
                        DIR_CHANGE_FROM_REV_TO_FWD              Enumerator
                        OUT_OF_SYNC_OR_PD_ERROR                 Enumerator
                        Track_Info_4.State                        Enumerator


    Local           :   uchPD_IO_Value                          BYTE
                        uchNewState                             BYTE

Output Variables        Name                                    Type
    Global          :   Track_Info_4.State                        Enumerator
                        Track_Info_4.PD_Last_Change_Timeout_50ms  BYTE
                        Status.Flags.PD1_Status                 Bit
                        Status.Flags.PD2_Status                 Bit

    Local           :

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            NO_OF_PD_TRANSITIONS            4
                            PHASE_DETECTOR_FAILED           0
                            AD_STATE_FAIL_ERROR_NUM                38
                            PD_CLEARING_TIMEOUT             120
References          :

Derived Requirements:

Algorithm           :1. Check for the number of PD transitions, Get the new
                        PD state by getting the PD main siganls and track state
                     2. If train enters in forward direction and moving forward
                        to middle od PDs of both PD, so clear the supervisory
                        errors
                     3. If train enters in forward direction and axle clears the
                        trailing PD and waiting for axle then register the forward
                        count
                     4. If train enters in reverse direction and axle clears the
                        leading PD and waiting for axle then register the reverse
                        count
                     5. If train enters in reverse direction and wheel is moving
                        Backward to middle of Both PD,so clear the supervisory
                        errors
                     6. If the direction of train changes from forward to reverse,
                        wheel is moving Backward to middle of Both PD,clear the
                        supervisory errors
                     7. If train enters in reverse direction and moving forward
                        to middle od PDs of both PD, so clear the supervisory
                        errors
                     8. If train enters in reverse direction and axle clears the
                        leading PD and waiting for axle then register the reverse
                        count
                     9. If train enters in the reverse direction, wheel is moving
                        Backward to middle of Both PD, clear the supervisory error
                    10. If train enters in the reverse direction and clears the both
                        PD and waiting for axle then register the reverse count
                    11. If the direction of train changes from reverse to forward,
                        wheel is moving Backward to middle of Both PD,clear the
                        supervisory errors
                    12. If there is Out of sync or any PD error set PD1 and PD2 status
                        flag as phase detector failed and declare DAC defective and
                        set PD state fail error ID


************************************************************************/
void Determine_TrackState_4(BYTE uchPD_IO_Value)
{
    BYTE uchNewState;

    if (uchPD_IO_Value >= NO_OF_PD_TRANSITIONS)
        return;
    uchNewState = uchPD_Transition_Table[Track_Info_4.State][uchPD_IO_Value];

    switch (Track_Info_4.State)
    {
        case WAITING_FOR_AXLE:
        case PRE_SYNCHRONISATION_WAIT:
             break;
        case WAIT_FOR_AXLE_AT_TRAILING_PD:
            if (uchNewState == LET_AXLE_CLEAR_LEADING_PD_FIRST)
                {
                /* wheel is moving forward to middle of Both PD, So clear the sup errors */
                    Clear_PD1_Supervisory_Count_4();
                    Clear_PD2_Supervisory_Count_4();
                }
            if(uchNewState == WAITING_FOR_AXLE)
              {
                Track_Info_4.Flags.PD_Without_Overlapping = TRUE;
              }
            break;
        case LET_AXLE_CLEAR_LEADING_PD_FIRST:
            Track_Info_4.Flags.PD_Non_Overlapping = FALSE;
            if (uchNewState == WAITING_FOR_AXLE)
                {
                Register_PD1212_Transition_4();
                }
            break;
        case LET_AXLE_CLEAR_TRAILING_PD_NEXT:
            if (uchNewState == WAITING_FOR_AXLE)
                {
                Register_PD1212_Transition_4();
                break;
                }
            if (uchNewState == LET_AXLE_CLEAR_LEADING_PD_FIRST)
                {
                /* wheel is moving Backward to middle of Both PD,so clear the sup errors */
                Clear_PD1_Supervisory_Count_4();
                Clear_PD2_Supervisory_Count_4();
                }
            break;
        case DIR_CHANGE_FROM_FWD_TO_REV:
            if (uchNewState == LET_AXLE_CLEAR_LEADING_PD_FIRST)
                {
                /* wheel is moving Backward to middle of Both PD */
                Clear_PD1_Supervisory_Count_4();
                Clear_PD2_Supervisory_Count_4();
                }
            break;
        case WAIT_FOR_AXLE_AT_LEADING_PD:
            if (uchNewState == LET_AXLE_CLEAR_TRAILING_PD_FIRST)
                {
                /* wheel is moving forward to middle of Both PD */

                Clear_PD1_Supervisory_Count_4();
                Clear_PD2_Supervisory_Count_4();
                }
            if(uchNewState == WAITING_FOR_AXLE)
              {
                Track_Info_4.Flags.PD_Without_Overlapping = TRUE;
              }
            break;
        case LET_AXLE_CLEAR_TRAILING_PD_FIRST:
            Track_Info_4.Flags.PD_Non_Overlapping = FALSE;
            if (uchNewState == WAITING_FOR_AXLE)
                {
                Register_PD2121_Transition_4();
                }
            break;
        case LET_AXLE_CLEAR_LEADING_PD_NEXT:
            if (uchNewState == LET_AXLE_CLEAR_TRAILING_PD_FIRST)
                {
                /* wheel is moving Backward to middle of Both PD */
                Clear_PD1_Supervisory_Count_4();
                Clear_PD2_Supervisory_Count_4();
                }
            if (uchNewState == WAITING_FOR_AXLE)
                {
                Register_PD2121_Transition_4();
                }
            break;
        case DIR_CHANGE_FROM_REV_TO_FWD:
            if (uchNewState == LET_AXLE_CLEAR_TRAILING_PD_FIRST)
                {
                /* wheel is moving Backward to middle of Both PD */
                Clear_PD1_Supervisory_Count_4();
                Clear_PD2_Supervisory_Count_4();
                }
            break;
        case OUT_OF_SYNC_OR_PD_ERROR:
            Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
            Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
            Declare_DAC_Defective();
            Set_Error_Status_Bit(AD_STATE_FAIL_ERROR_NUM);
            break;
     }

       Track_Info_4.State = uchNewState;
       if(Track_Info_4.State != WAITING_FOR_AXLE )
        {
         Track_Info_4.PD_Last_Change_Timeout_50ms  = PD_CLEARING_TIMEOUT;
        }
}



/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Analyse_Supervisory_Sequence_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Analyse the supervisory signal(falling/ rising edges) and thereby
                     determine the type of the wheel detected at the phase detectors



Design Requirements     :
Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Validate_PD_Signals()

Input Variables         Name                                    Type
    Global          :   Track_Info_4.PD1S_Falling_Edge_Count      UINT16
                        Track_Info_4.PD2S_Falling_Edge_Count      UINT16
                        Track_Info_4.PD1S_Rising_Edge_Count       UINT16
                        Track_Info_4.PD2S_Rising_Edge_Count       UINT16
                        TRAIN_WHEEL_WITH_SINGLE_SUP             Enumerator
                        TRAIN_WHEEL_WITH_DOUBLE_SUP             Enumerator

    Local           :   None

Output Variables        Name                                    Type
    Global          :   Track_Info_4.Wheel_Type                   Enumerator

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1. If the both the PDs falling edge count is one and rising edge
                        count is zero,declare wheel type as train wheel with single
                        supervisory pulse else declare it as train wheel with double
                        supervisory pulse
                     2. If both the PDs rising edge count is one declare wheel type as
                        train wheel with double supervisory pulse

************************************************************************/

void Analyse_Supervisory_Sequence_4(void)
{

  if(Track_Info_4.PD1S_Falling_Edge_Count == 1 &&
     Track_Info_4.PD2S_Falling_Edge_Count == 1 )
     {
       if(Track_Info_4.PD1S_Rising_Edge_Count == 0 &&
         Track_Info_4.PD2S_Rising_Edge_Count == 0 )
         {
          Track_Info_4.Wheel_Type = TRAIN_WHEEL_WITH_SINGLE_SUP;
         }
         else
         {
          Track_Info_4.Wheel_Type = TRAIN_WHEEL_WITH_DOUBLE_SUP;
         }
        return;
     }
    if(Track_Info_4.PD1S_Rising_Edge_Count == 1 &&
       Track_Info_4.PD2S_Rising_Edge_Count == 1 )
    {
       Track_Info_4.Wheel_Type = TRAIN_WHEEL_WITH_DOUBLE_SUP;
       return;
    }
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Detect_PD_Failures_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Check for PD errors.If any error is there set for
                     particular error bit and declare it as DAC defective



Design Requirements     :

Interfaces
    Calls           :   AXLE_MON.c  -   Clear_PD1_Supervisory_Count_4()
                        AXLE_MON.c  -   Clear_PD2_Supervisory_Count_4()
                        AXLE_MON.c  -   Clear_PD12_Main_Pulse_Count_4()
                        AXLE_MON.c  -   Clear_Wheel_Type_4()
                        ERROR.c     -   Set_Error_Status_Bit()
                        RELAYMGR.c  -   Declare_DAC_Defective()

    Called by       :   AXLE_MON.c  -   Validate_PD_Signals()

Input Variables         Name                                                        Type
    Global          :   Track_Info_4.Flags.PD_State_Counted                           Bit
                        Track_Info_4.PD_Last_Change_Timeout_50ms                      BYTE
                        Track_Info_4.Flags.PD1S_Timer_On                              Bit
                        Track_Info_4.PD1S_Timeout_50ms                                UNIT16
                        Track_Info_4.Flags.PD2S_Timer_On                              Bit
                        Track_Info_4.PD2S_Timeout_50ms                                UNIT16
                        Track_Info_4.PD1M_Count                                       UINT16
                        Track_Info_4.PD2M_Count                                       UINT16
                        Track_Info_4.PD1S_Count                                       UINT16
                        Track_Info_4.PD2S_Count                                       UINT16
                        Track_Info_4.Wheel_Type                                       Enumerator
                        TRAIN_WHEEL_WITH_SINGLE_SUP                                 Enumerator
                        TRAIN_WHEEL_WITH_DOUBLE_SUP                                 Enumerator
                        Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms       BYTE
                        Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms       BYTE
                        Track_Info_4.Flags.US_Local_Counts_Just_Cleared               Bit
                        Track_Info_4.Flags.DS_Local_Counts_Just_Cleared               Bit
    Local           :   None

Output Variables        Name                                                        Type
    Global          :   Track_Info_4.Flags.PD_State_Counted                           Bit
                        Status.Flags.PD1_Status                                     Bit
                        Status.Flags.PD2_Status                                     Bit
                        Track_Info_4.Flags.US_Local_Counts_Just_Cleared               Bit
                        Track_Info_4.Flags.DS_Local_Counts_Just_Cleared               Bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            TRUE                            1
                            TIMEOUT_EVENT                   0
                            FALSE                           0
                            PHASE_DETECTOR_FAILED           0
                            AD1_SUP_LOW_ERROR_NUM                  32
                            AD2_SUP_LOW_ERROR_NUM                  33
                            AD1_PULSING_ERROR_NUM                  34
                            AD2_PULSING_ERROR_NUM                  35
                            AD_STATE_MISSING_ERROR_NUM             36
                            AD_SUP_PULSATING_ERROR_NUM             37
                            AD_NOT_SENSING_ERROR_NUM               39

References          :

Derived Requirements:

Algorithm           :1. Wheels counted,clear the PD state error,because of trolley
                     2. After Local counts of US section gets cleared off 2s is over hence clear the flag
                     3. After Local counts of DS section gets cleared off 2s is over hence clear the flag
                     4. Wheel state is not changed for
                        last 6s,clear the PD state error,because of trolley
                     5. If the PD1 supervisory signal is Low for more than 60s
                        set PD1 SUP LOW ID also set PD1 status flag as phase detector
                        is failed and declare DAC defective
                     6. If the PD2 supervisory signal is Low for more than 60s
                        set PD2 SUP LOW ID also set PD2 status flag as phase detector
                        is failed and declare DAC defective
                     7. If only PD1 is detecting the wheel and other is not detecting
                        means set PD1 PULSATING ID and set PD1 status flag as phase detector
                        is failed and declare DAC defective
                     8. If only PD1 is detecting the wheel and other PD supervisory is stuck at low
                        means set PD1 PULSATING ID and set PD1 status flag as phase detector
                        is failed and declare DAC defective
                     9. If only PD2 is detecting the wheel and other is not detecting
                        means set PD2 PULSATING ID and set PD2 status flag as phase detector
                        is failed and declare DAC defective
                     10. If only PD2 is detecting the wheel and other PD supervisory is stuck at low
                        means set PD2 PULSATING ID and set PD2 status flag as phase detector
                        is failed and declare DAC defective
                     11.If two wheels passed and there is a more than 4 PD transiton is there in
                        main pulse then declare PD state missing error and set PD1 and PD2 status
                        flag as Phase detector failed and declare DAC defecive
                     12.If non overlapping pulse comes without supervisory count then declare PD state fail
                        error and set PD1 and PD2 status flag as Phase detector failed and declare DAC defecive
                     13. If wheel type is train wheel with single sup, if supervisory signal
                        pulsates more than one time,set PD_SUP_PULSATING  error ID and set PD1
                        and PD2 status flag as Phase detector failed and declare DAC defecive
                     14.If only one wheel went(supervisory count is >1) and both the PD main
                        signals are high, set PD_NOT_SENSING error ID and set PD1 and PD2
                        status flag as Phase detector failed and declare DAC defective
                    15. If wheel type is train wheel with double sup, one wheel went off (supervisory
                        count is > 4) but it is not counted means set PD_SUP_PULSATING  error ID and
                        set PD1 and PD2 status flag as Phase detector failed and declare DAC defecive
                    16. If only one wheel went(supervisory count is > 2) and both the PD main
                        signals are high, set PD_NOT_SENSING error ID and set PD1 and PD2
                        status flag as Phase detector failed and declare DAC defective
                    17. If two trolley wheels went off and if the third non-overlapping pulse or
                        trolley wheel comes system goes to error by PD1M or PD2M count > 2
                    18.If wheel type is trolley wheel, one wheel went off (supervisory
                        count is > 1) and both the PD main signals are high, means set
                        PD_STATE_MISSING error ID and set PD1 and PD2   status flag as Phase detector
                        failed and declare DAC defective
                    19.If any of the PD supervisory signal pulses more than 4 set
                        PD_SUP_PULSATING error ID and set PD1 and PD2   status flag as Phase detector
                        failed and declare DAC defective
************************************************************************/
void Detect_PD_Failures_4(void)
{
   if(Track_Info_4.Flags.PD_State_Counted == TRUE)
    {
         /* Wheel counted .so, clear the PD state errors. Because of Trolley    */
         Clear_PD1_Supervisory_Count_4();
         Clear_PD2_Supervisory_Count_4();
         Clear_PD12_Main_Pulse_Count_4();
         Clear_Wheel_Type_4();
         Track_Info_4.Flags.PD_State_Counted =FALSE;
         Track_Info_4.Flags.PD_Non_Overlapping = FALSE;
     return;
    }
    if(Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms == TIMEOUT_EVENT)
     {
      Track_Info_4.Flags.US_Local_Counts_Just_Cleared = SET_LOW;
     }
    if(Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms == TIMEOUT_EVENT)
     {
      Track_Info_4.Flags.DS_Local_Counts_Just_Cleared = SET_LOW;
     }
    if(Track_Info_4.PD_Last_Change_Timeout_50ms == TIMEOUT_EVENT )
    {
         /*  Wheel state not changed for last 6 sec.
            so, clear the PD state errors. Because of Trolley   */
         Clear_PD1_Supervisory_Count_4();
         Clear_PD2_Supervisory_Count_4();
         Clear_PD12_Main_Pulse_Count_4();
         Clear_Wheel_Type_4();
         Track_Info_4.Flags.PD_State_Counted =FALSE;
         Track_Info_4.Flags.PD_Non_Overlapping = FALSE;
    }
  if(!Chk_For_Track_Occupancy_4())
   {
    if (Track_Info_4.Flags.PD1S_Timer_On == TRUE && Track_Info_4.PD1S_Timeout_50ms == TIMEOUT_EVENT)
      {
        Set_Error_Status_Bit (AD1_SUP_LOW_ERROR_NUM);
        Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
      }
     if (Track_Info_4.Flags.PD2S_Timer_On == TRUE && Track_Info_4.PD2S_Timeout_50ms == TIMEOUT_EVENT)
     {
        Set_Error_Status_Bit (AD2_SUP_LOW_ERROR_NUM);
        Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }

     if(Track_Info_4.PD1M_Count >=2 && Track_Info_4.PD2M_Count == 0 &&
        Track_Info_4.State == WAITING_FOR_AXLE)
     {
        Set_Error_Status_Bit (AD1_PULSING_ERROR_NUM);
        Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }
     if(Track_Info_4.PD1M_Count >2 && Track_Info_4.PD2M_Count == 0 &&
        PORTBbits.RB14 == SET_LOW)
     {
        Set_Error_Status_Bit (AD1_PULSING_ERROR_NUM);
        Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }
     if(Track_Info_4.PD1M_Count >=2 &&
       Track_Info_4.PD1S_Count > 2 && Track_Info_4.PD2S_Count == 0)
     {
        Set_Error_Status_Bit (AD1_PULSING_ERROR_NUM);
        Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }
     if(Track_Info_4.PD2M_Count >=2 && Track_Info_4.PD1M_Count == 0 &&
        Track_Info_4.State == WAITING_FOR_AXLE)
     {
        Set_Error_Status_Bit (AD2_PULSING_ERROR_NUM);
        Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }
     if(Track_Info_4.PD2M_Count >2 && Track_Info_4.PD1M_Count == 0 &&
        PORTBbits.RB9 == SET_LOW)
     {
        Set_Error_Status_Bit (AD2_PULSING_ERROR_NUM);
        Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }
     if(Track_Info_4.PD2M_Count >=2  &&
       Track_Info_4.PD2S_Count > 2 && Track_Info_4.PD1S_Count == 0)
     {
        Set_Error_Status_Bit (AD2_PULSING_ERROR_NUM);
        Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }
     if(Track_Info_4.PD1M_Count >=4 && Track_Info_4.PD2M_Count >=4)
     {
        Set_Error_Status_Bit(DOUBLE_COIL_INFLUENCE_ERROR_NUM);
        Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
        Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }
   }
    if(Track_Info_4.PD1M_Count >=4 && Track_Info_4.PD2M_Count == 0 &&
        Track_Info_4.State == WAITING_FOR_AXLE)
     {
        Set_Error_Status_Bit (AD1_PULSING_ERROR_NUM);
        Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
      }
    if(Track_Info_4.PD2M_Count >=4 && Track_Info_4.PD1M_Count == 0 &&
        Track_Info_4.State == WAITING_FOR_AXLE)
     {
        Set_Error_Status_Bit (AD2_PULSING_ERROR_NUM);
        Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
        return;
     }
    if(Track_Info_4.PD1M_Count == 1 && Track_Info_4.PD2M_Count == 1 &&
       Track_Info_4.PD1S_Count == 0 && Track_Info_4.PD2S_Count == 0 &&
       Track_Info_4.Flags.PD_Without_Overlapping == TRUE)
    {
        Set_Error_Status_Bit(AD_SUP_MISSING_ERROR_NUM);
        Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
        Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
    }
    if(Track_Info_4.PD1M_Count >= 1 && Track_Info_4.PD2M_Count >= 1 &&
       Track_Info_4.Flags.PD_Without_Overlapping == TRUE &&
       Track_Info_4.Trolley_Timeout_50ms != TIMEOUT_EVENT )
    {
        Set_Error_Status_Bit(DECEPTIVE_AXLE_ERROR_NUM);
        Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
        Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
        Declare_DAC_Defective();
    }
  switch(Track_Info_4.Wheel_Type)
    {
        case TRAIN_WHEEL_WITH_SINGLE_SUP:
            if(Track_Info_4.PD1S_Count >= 2 && Track_Info_4.PD2S_Count >= 2)
            {
              /* Two wheels went off, But it was not Counted */
              Set_Error_Status_Bit (AD_SUP_PULSATING_ERROR_NUM);
              Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
              Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
              Declare_DAC_Defective();
            }
            if(Track_Info_4.PD1S_Count > 1 && Track_Info_4.PD2S_Count > 1 &&
               Track_Info_4.PD1M_Count ==0  && Track_Info_4.PD2M_Count == 0)
            {
              /* One wheel went off, But Both Pd outputs are at high */
              Set_Error_Status_Bit (AD_NOT_SENSING_ERROR_NUM);
              Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
              Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
              Declare_DAC_Defective();
            }
            break;
        case TRAIN_WHEEL_WITH_DOUBLE_SUP:
            if(Track_Info_4.PD1S_Count >= 4 && Track_Info_4.PD2S_Count >= 4)
            {
              /* One wheel went off, But it was not counted */
              Set_Error_Status_Bit (AD_SUP_PULSATING_ERROR_NUM);
              Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
              Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
              Declare_DAC_Defective();
            }
            if(Track_Info_4.PD1S_Count > 2 &&  Track_Info_4.PD2S_Count > 2 &&
               Track_Info_4.PD1M_Count ==0 &&  Track_Info_4.PD2M_Count == 0)
            {
              /* One wheel went off, But Both Pd outputs are at high */
              Set_Error_Status_Bit (AD_NOT_SENSING_ERROR_NUM);
              Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
              Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
              Declare_DAC_Defective();
            }
           break;
        default:
          if(Track_Info_4.PD1M_Count > 2 && Track_Info_4.PD2M_Count > 2)
            {
              /* Two trolley wheels went off,hence further nonoverlapping pulse is not alowed */
              Set_Error_Status_Bit (AD_STATE_MISSING_ERROR_NUM);
              Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
              Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
              Declare_DAC_Defective();
            }
          if(Track_Info_4.PD1S_Count > 1 && Track_Info_4.PD2S_Count > 1 &&
             Track_Info_4.PD1M_Count ==0 &&  Track_Info_4.PD2M_Count == 0)
            {
              /* One wheel went off, But Both Pd outputs are at high */
              Set_Error_Status_Bit (AD_NOT_SENSING_ERROR_NUM);
              Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
              Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
              Declare_DAC_Defective();
            }
          if(Track_Info_4.PD1M_Count ==0 &&  Track_Info_4.PD2M_Count == 0)
            {
             if(Track_Info_4.PD1S_Count > 4 || Track_Info_4.PD2S_Count > 4)
              {
               Set_Error_Status_Bit (AD_SUP_PULSATING_ERROR_NUM);
               Status.Flags.PD1_Status = PHASE_DETECTOR_FAILED;
               Status.Flags.PD2_Status = PHASE_DETECTOR_FAILED;
               Declare_DAC_Defective();
              }
            }
        break;
    }
}


/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Register_PD1212_Transition_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Register the correct PD transition in the forward direction
                     at remote units and increment/decrement the axle counts


Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Determine_TrackState_4()

Input Variables         Name                                    Type
    Global          :   Track_Info_4.Flags.US_Counting_Enable     Bit
                        Track_Info_4.US_Axle_Direction            BYTE
                        Track_Info_4.Flags.DS_Counting_Enable     Bit
                        Track_Info_4.DS_Axle_Direction            BYTE
                        DIRECTION_NOT_DEFINED                   Enumerator
                        FORWARD_DIRECTION                       Enumerator

    Local           :   None

Output Variables        Name                                    Type
    Global          :   Track_Info_4.Flags.PD_State_Counted       Bit
                        Track_Info_4.US_Axle_Direction            BYTE
                        Track_Info_4.Flags.Train_Shunting         Bit
                        Track_Info_4.DS_Axle_Direction            BYTE
                        Track_Info_4.Trolley_Timeout_50ms         BYTE

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil


Macro definitions used:     Macro                           Value
                            TRUE                            1
                            SET_HIGH                        1
                            FALSE                           0
                            TROLLEY_TIMEOUT                 120
References          :

Derived Requirements:

Algorithm           :1. If Up stream counting is enabled, check for the axle
                        direction, if it is not defined set it as Forward and
                        increment the Up stream forward axle count and clear the train
                        shunting flag else decrement the up stream reverse count
                        and set the train shunting flag
                     2. If Down stream counting is enabled, check for the axle
                        direction, if it is not defined set it as Forward and
                        increment the down stream forward axle count and clear the train
                        shunting flag else decrement the down stream reverse count
                        and set the train shunting flag
************************************************************************/
void Register_PD1212_Transition_4(void)
{

    Track_Info_4.Flags.PD_State_Counted = TRUE;
    Track_Info_4.Trolley_Timeout_50ms = TROLLEY_TIMEOUT;
#if NO_SHUNTING
    Track_Info_4.LCWS_Axle_Direction = FORWARD_DIRECTION;
    Track_Info_4.LCWS_Fwd_Count = Track_Info_4.LCWS_Fwd_Count + 1;
    if (Track_Info_4.LCWS_Total_Fwd_Count >= MAX_AXLE_COUNTS)
    {
        Track_Info_4.LCWS_Total_Fwd_Count = 0;
    }
    else
    {
        Track_Info_4.LCWS_Total_Fwd_Count = Track_Info_4.LCWS_Total_Fwd_Count + 1;
    }
#else
    if(Track_Info_4.LCWS_Axle_Direction == DIRECTION_NOT_DEFINED || Track_Info_4.LCWS_Axle_Direction == FORWARD_DIRECTION)
    {
        Track_Info_4.LCWS_Axle_Direction = FORWARD_DIRECTION;
        Track_Info_4.LCWS_Fwd_Count = Track_Info_4.LCWS_Fwd_Count + 1;
        if (Track_Info_4.LCWS_Total_Fwd_Count >= MAX_AXLE_COUNTS)
        {
            Track_Info_4.LCWS_Total_Fwd_Count = 0;
        }
        else
        {
            Track_Info_4.LCWS_Total_Fwd_Count = Track_Info_4.LCWS_Total_Fwd_Count + 1;
        }
    }
    else if(Track_Info_4.LCWS_Axle_Direction == REVERSE_DIRECTION)
    {
        Track_Info_4.LCWS_Rev_Count = Track_Info_4.LCWS_Rev_Count - 1;
        if (Track_Info_4.LCWS_Total_Rev_Count >= MAX_AXLE_COUNTS)
        {
            Track_Info_4.LCWS_Total_Rev_Count = 0;
        }
        else
        {
            Track_Info_4.LCWS_Total_Rev_Count = Track_Info_4.LCWS_Total_Rev_Count - 1;
        }
    }
#endif
}

/**************************************************************************
Component name      :AXLE_MON
Module Name         :void Register_PD2121_Transition_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Register the correct PD transition in the reverse direction
                     at remote units and increment/decrement the axle counts

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Determine_TrackState_4()

Input Variables         Name                                    Type
    Global          :   Track_Info_4.Flags.US_Counting_Enable     Bit
                        Track_Info_4.US_Axle_Direction            BYTE
                        Track_Info_4.Flags.DS_Counting_Enable     Bit
                        Track_Info_4.DS_Axle_Direction            BYTE
                        DIRECTION_NOT_DEFINED                   Enumerator
                        REVERSE_DIRECTION                       Enumerator

    Local           :   None

Output Variables        Name                                    Type
    Global          :   Track_Info_4.Flags.PD_State_Counted       Bit
                        Track_Info_4.US_Axle_Direction            BYTE
                        Track_Info_4.Flags.Train_Shunting         Bit
                        Track_Info_4.DS_Axle_Direction            BYTE
                        Track_Info_4.Trolley_Timeout_50ms         BYTE

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil


Macro definitions used:     Macro                           Value
                            TRUE                            1
                            SET_HIGH                        1
                            FALSE                           0
                            TROLLEY_TIMEOUT                 120
References          :

Derived Requirements:

Algorithm           :1. If Up stream counting is enabled, check for the axle
                        direction, if it is not defined set it as Forward and
                        increment the Up stream reverse axle count and clear the train
                        shunting flag else decrement the up stream forward count
                        and set the train shunting flag
                     2. If Down stream counting is enabled, check for the axle
                        direction, if it is not defined set it as Forward and
                        increment the down stream reverse axle count and clear the train
                        shunting flag else decrement the down stream forward count
                        and set the train shunting flag

************************************************************************/
void Register_PD2121_Transition_4(void)
{

    Track_Info_4.Flags.PD_State_Counted = TRUE;
    Track_Info_4.Trolley_Timeout_50ms = TROLLEY_TIMEOUT;
#if NO_SHUNTING
    Track_Info_4.LCWS_Axle_Direction = REVERSE_DIRECTION;
    Track_Info_4.LCWS_Rev_Count = Track_Info_4.LCWS_Rev_Count + 1;
    if (Track_Info_4.LCWS_Total_Rev_Count >= MAX_AXLE_COUNTS)
    {
        Track_Info_4.LCWS_Total_Rev_Count = 0;
    }
    else
    {
        Track_Info_4.LCWS_Total_Rev_Count = Track_Info_4.LCWS_Total_Rev_Count + 1;
    }
#else
    if(Track_Info_4.LCWS_Axle_Direction == DIRECTION_NOT_DEFINED || Track_Info_4.LCWS_Axle_Direction == REVERSE_DIRECTION)
    {
        Track_Info_4.LCWS_Axle_Direction = REVERSE_DIRECTION;
        Track_Info_4.LCWS_Rev_Count = Track_Info_4.LCWS_Rev_Count + 1;
        if (Track_Info_4.LCWS_Total_Rev_Count >= MAX_AXLE_COUNTS)
        {
            Track_Info_4.LCWS_Total_Rev_Count = 0;
        }
        else
        {
            Track_Info_4.LCWS_Total_Rev_Count = Track_Info_4.LCWS_Total_Rev_Count + 1;
        }
    }
    else
    {
        Track_Info_4.LCWS_Fwd_Count = Track_Info_4.LCWS_Fwd_Count - 1;
        if (Track_Info_4.LCWS_Total_Fwd_Count >= MAX_AXLE_COUNTS)
        {
            Track_Info_4.LCWS_Total_Fwd_Count = 0;
        }
        else
        {
            Track_Info_4.LCWS_Total_Fwd_Count = Track_Info_4.LCWS_Total_Fwd_Count - 1;
        }
    }
#endif
}

/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Chk_for_AxleCount_Completion(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Check for the complitation of axle counts at the
                     remote unit.If the Up stream or Down stream axle counts
                     are zero, clear the direction.If Up stream or down
                     stream axle counts is greater than 9999, set Invalid
                     count error status bit, declare DAC defective
                     bit


Design Requirements     :

Interfaces
    Calls           :   ERROR.c     -   Set_Error_Status_Bit()
                        RELAYMGR.c  -   Declare_DAC_Defective()

    Called by       :   DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_DE()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables         Name                                    Type
    Global          :   Track_Info_4.US_Axle_Count                UINT16
                        DIRECTION_NOT_DEFINED                   Enumerator
                        Track_Info_4.DS_Axle_Count                UINT16

    Local           :   None

Output Variables        Name                                    Type
    Global          :   Track_Info_4.US_Axle_Direction            BYTE
                        Track_Info_4.DS_Axle_Direction            BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            MAX_AXLE_COUNTS                 9999
                            INOPERATIVE_COUNTS_ERROR_NUM               59
References          :

Derived Requirements:

Algorithm           :1.If Upstream axle count is equal to zero set the Upstream axle direction as "DIRECTION NOT DEFINED"
                     2.If Downstream axle count is equal to zero set the downstream axle direction as "DIRECTION NOT DEFINED"
                     3.If Upstream or downstream axle count is greater than 9999 then set the error Id "INVALID ERROR CCOUNTS"
************************************************************************/
void Chk_for_AxleCount_Completion_4(void)
{
    if (Track_Info_4.LCWS_Fwd_Count == 0 && Track_Info_4.LCWS_Rev_Count == 0)
    {
        Track_Info_4.LCWS_Axle_Direction = DIRECTION_NOT_DEFINED;
    }
    if (Track_Info_4.LCWS_Fwd_Count > MAX_AXLE_COUNTS )
    {
        Set_Error_Status_Bit(INOPERATIVE_COUNTS_ERROR_NUM);
        Declare_DAC_Defective();
    }
    if (Track_Info_4.LCWS_Rev_Count > MAX_AXLE_COUNTS )
    {
        Set_Error_Status_Bit(INOPERATIVE_COUNTS_ERROR_NUM);
        Declare_DAC_Defective();
    }
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Decrement_TrackMon_50msTmr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Decrement the track monitoring 50ms timer


Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_DE()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables         Name                                                      Type
    Global          :   Track_Info_4.PD_Last_Change_Timeout_50ms                    BYTE
                        Track_Info_4.PD1S_Timeout_50ms                              UNIT16
                        Track_Info_4.PD2S_Timeout_50ms                              UNIT16
                        Track_Info_4.Local_Counts_Clearing_Flag_Timeout_50ms        BYTE
                        Track_Info_4.Trolley_Timeout_50ms                           BYTE

    Local           :   None

Output Variables        Name                                                      Type
    Global          :   Track_Info_4.PD_Last_Change_Timeout_50ms                    BYTE
                        Track_Info_4.PD1S_Timeout_50ms                              UNIT16
                        Track_Info_4.PD2S_Timeout_50ms                              UNIT16
                        Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms     BYTE
                        Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms     BYTE
                        Track_Info_4.Trolley_Timeout_50ms                           BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

Algorithm           :1.Decreament the Track_info timeout variables whenever the 50ms timer overflows
************************************************************************/
void Decrement_TrackMon_50msTmr_4(void)
{
    if (Track_Info_4.PD_Last_Change_Timeout_50ms > 0)
    {
        Track_Info_4.PD_Last_Change_Timeout_50ms = Track_Info_4.PD_Last_Change_Timeout_50ms - 1;
    }
    if (Track_Info_4.PD1S_Timeout_50ms > 0)
    {
        Track_Info_4.PD1S_Timeout_50ms = Track_Info_4.PD1S_Timeout_50ms - 1;
    }
    if (Track_Info_4.PD2S_Timeout_50ms > 0)
    {
        Track_Info_4.PD2S_Timeout_50ms = Track_Info_4.PD2S_Timeout_50ms - 1;
    }
    if(Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms > 0)
      {
        Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms = Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms - 1;
      }
    if(Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms > 0)
      {
        Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms = Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms - 1;
      }
    if(Track_Info_4.Trolley_Timeout_50ms > 0)
      {
        Track_Info_4.Trolley_Timeout_50ms = Track_Info_4.Trolley_Timeout_50ms - 1;
      }
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Clear_US_AxleCount(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the Up stream axle counts and direction and
                     clear the PD1 and PD2 supervisory counts and clear the
                     wheel type.


Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   AXLE_MON.c  -   Clear_PD1_Supervisory_Count_4()
                        AXLE_MON.c  -   Clear_PD2_Supervisory_Count_4()
                        AXLE_MON.c  -   Clear_Wheel_Type_4()

    Called by       :   RLYA_MGR.c      -   Start_Relay_A_Mgr()
                        RLAY_MGR.c      -   Update_Relay_A_State()
                        RLAYD3_MGR.c    -   Update_Relay_D3_State()

Input Variables         Name                                Type
    Global          :   DIRECTION_NOT_DEFINED               Enumerator

    Local           :   None

Output Variables        Name                                                Type
    Global          :   Track_Info_4.US_Axle_Direction                        BYTE
                        Track_Info_4.Flags.Train_Shunting                     Bit
                        Track_Info_4.US_Axle_Count                            UINT16
                        Track_Info_4.Local_Counts_Clearing_Flag_Timeout_50ms  BYTE
                        Track_Info_4.Flags.US_Local_Counts_Just_Cleared       Bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil


Macro definitions used:     Macro                               Value
                            FALSE                               0
                            SET_HIGH                            1
                            LOCAL_COUNT_CLEARING_FLAG_TIMEOUT   40
References          :

Derived Requirements:

Algorithm           :1.Set the Upstream axle direction as "DIRECTION NOT DEFINED" and axle count value as zero
                     2.Set the US local count just cleared flag
                     3.Clear both PDs supervisory counts and wheel type determined
************************************************************************/
void Clear_US_AxleCount_4(void)
{
    Track_Info_4.US_Axle_Direction = DIRECTION_NOT_DEFINED;
    Track_Info_4.US_Axle_Count = 0;
    Track_Info_4.Flags.US_Local_Counts_Just_Cleared = SET_HIGH;
    Track_Info_4.US_Local_Counts_Clearing_Flag_Timeout_50ms = LOCAL_COUNT_CLEARING_FLAG_TIMEOUT;
    Clear_PD1_Supervisory_Count_4();
    Clear_PD2_Supervisory_Count_4();
    Clear_Wheel_Type_4();

}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Clear_DS_AxleCount(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the down stream axle counts and direction
                     and clear the PD1 and PD2 supervisory counts and clear
                     the wheel type.


Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   AXLE_MON.c  -   Clear_PD1_Supervisory_Count_4()
                        AXLE_MON.c  -   Clear_PD2_Supervisory_Count_4()
                        AXLE_MON.c  -   Clear_Wheel_Type_4()

    Called by       :   RLYB_MGR.c  -   Start_Relay_B_Mgr()
                        RLYB_MGR.c  -   Update_Relay_B_State()
                        RLYD3_MGR.c -   Update_Relay_D3_State()
                        RLYDE_MGR.c -   Update_Relay_DE_State

Input Variables         Name                            Type
    Global          :   DIRECTION_NOT_DEFINED           Enumerator

    Local           :   None

Output Variables        Name                                              Type
    Global          :   Track_Info_4.DS_Axle_Direction                      BYTE
                        Track_Info_4.Flags.Train_Shunting                   Bit
                        Track_Info_4.DS_Axle_Count                          UINT16
                        Track_Info_4.Flags.DS_Local_Counts_Just_Cleared     Bit
                        Track_Info_4.Local_Counts_Clearing_Flag_Timeout_50ms BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil


Macro definitions used:     Macro                               Value
                            FALSE                               0
                            SET_HIGH                            1
                            LOCAL_COUNT_CLEARING_FLAG_TIMEOUT   40
References          :

Derived Requirements:

Algorithm           :1.Set the Downstream axle direction as "DIRECTION NOT DEFINED" and axle count value as zero
                     2.Set the DS local count just cleared flag
                     3.Clear both PDs supervisory counts and wheel type determined
************************************************************************/
void Clear_DS_AxleCount_4(void)
{
    Track_Info_4.DS_Axle_Direction = DIRECTION_NOT_DEFINED;
    Track_Info_4.DS_Axle_Count = 0;
    Track_Info_4.Flags.DS_Local_Counts_Just_Cleared = SET_HIGH;
    Track_Info_4.DS_Local_Counts_Clearing_Flag_Timeout_50ms = LOCAL_COUNT_CLEARING_FLAG_TIMEOUT;
    Clear_PD1_Supervisory_Count_4();
    Clear_PD2_Supervisory_Count_4();
    Clear_Wheel_Type_4();
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :UINT16 Get_US_AxleCount(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the axle count at the Up stream remote unit


Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   COMM_US.c   -   Build_US_Axle_Count_Message()
                        RELAYMGR.c  -   Update_Relay_A_Counts()

Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.US_Axle_Count        UINT16

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

************************************************************************/
UINT16 Get_US_AxleCount_4(void)
{
    return Track_Info_4.US_Axle_Count;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :UINT16 Get_DS_AxleCount(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the axle count at the down stream remote unit


Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   COMM_DS.c   -   Build_DS_Axle_Count_Message()
                        RELAYMGR.c  -   Update_Relay_B_Counts()
                        RELAYMGR.c  -   Update_Relay_D3_Counts()

Input Variables         Name                        Type
    Global          :   None

    Local           :   None

Output Variables        Name                        Type
    Global          :   Track_Info_4.DS_Axle_Count    UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

************************************************************************/
UINT16 Get_DS_AxleCount_4(void)
{
    return Track_Info_4.DS_Axle_Count;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :BYTE Get_US_AxleDirection(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the axle direction at Up stream remote unit


Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   COMM_SM.c   -   Build_Interprocess_Message()
                        COMM_US.c   -   Build_US_Axle_Count_Message()
                        RELAYMGR.c  -   Update_Relay_A_Counts()

Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.US_Axle_Direction    BYTE

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

************************************************************************/
BYTE Get_US_AxleDirection_4(void)
{
    return Track_Info_4.US_Axle_Direction;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :BYTE Get_DS_AxleDirection(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the axle direction at Down stream remote unit



Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   COMM_DS.c   -   Build_DS_Axle_Count_Message()
                        COMM_SM.c   -   Build_Interprocess_Message()
                        RELAYMGR.c  -   Update_Relay_B_Counts()
                        RELAYMGR.c  -   Update_Relay_DE_Counts()
                        RELAYMGR.c  -   Update_Relay_D3_Counts()

Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.DS_Axle_Direction    BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil


Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

************************************************************************/
BYTE Get_DS_AxleDirection_4(void)
{
    return Track_Info_4.DS_Axle_Direction;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :UINT16 Get_US_Fwd_AxleCount(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the forward axle count at the Up stream remote
                     unit


Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   COMM_SM.c   -   Build_Interprocess_Message()
                        COMM_US.c   -   Build_US_Axle_Count_Message()
                        RELAYMGR.c  -   Update_Relay_A_Counts()

Input Variables         Name                        Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.US_Fwd_Axle_Count    UINT16

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

************************************************************************/
UINT16 Get_US_Fwd_AxleCount_4(void)
{
    return Track_Info_4.US_Fwd_Axle_Count;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :UINT16 Get_US_Rev_AxleCount(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            : Get the reverse axle count at the Up stream remote
                      unit


Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   COMM_SM.c   -   Build_Interprocess_Message()
                        COMM_US.c   -   Build_US_Axle_Count_Message()
                        RELAYMGR.c  -   Update_Relay_A_Counts()


Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.US_Rev_Axle_Count    UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil


Macro definitions used:     Macro                           Value
References          :

Derived Requirements:

************************************************************************/
UINT16 Get_US_Rev_AxleCount_4(void)
{
    return Track_Info_4.US_Rev_Axle_Count;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :UINT16 Get_DS_Fwd_AxleCount(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the forward axle count at the down stream remote
                     unit


Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   COMM_DS.c   -   Build_DS_Axle_Count_Message()
                        COMM_SM.c   -   Build_Interprocess_Message()
                        RELAYMGR.c  -   Update_Relay_B_Counts()
                        RELAYMGR.c  -   Update_Relay_DE_Counts()
                        RELAYMGR.c  -   Update_Relay_D3_Counts()

Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.DS_Fwd_Axle_Count    UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value

References          :

Derived Requirements:

************************************************************************/
UINT16 Get_DS_Fwd_AxleCount_4(void)
{
    return Track_Info_4.DS_Fwd_Axle_Count;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :UINT16 Get_DS_Rev_AxleCount(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the reverse axle count at the down stream remote
                     unit


Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   COMM_DS.c   -   Build_DS_Axle_Count_Message()
                        COMM_SM.c   -   Build_Interprocess_Message()
                        RELAYMGR.c  -   Update_Relay_B_Counts()
                        RELAYMGR.c  -   Update_Relay_DE_Counts()
                        RELAYMGR.c  -   Update_Relay_D3_Counts()

Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.DS_Rev_Axle_Count    UINT16

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

************************************************************************/
UINT16 Get_DS_Rev_AxleCount_4(void)
{
    return Track_Info_4.DS_Rev_Axle_Count;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Clear_DS_Local_Counts(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the Down stream remote unit forward and reverse axle
                     counts at the Local unit


Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   RLYB_MGR.c  -   Start_Relay_B_Mgr()
                        RLYB_MGR.c  -   Set_Relay_B_Reset_State()
                        RLYD3_MGR.c -   Start_Relay_D3_Mgr()
                        RLYDE_MGR.c -   Start_Relay_DE_Mgr()

Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.DS_Fwd_Axle_Count    UINT16
                        Track_Info_4.DS_Rev_Axle_Count    UINT16

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

************************************************************************/
void Clear_DS_Local_Counts_4(void)
{
    Track_Info_4.DS_Fwd_Axle_Count = 0;
    Track_Info_4.DS_Rev_Axle_Count = 0;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Clear_US_Local_Counts(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the Up stream remote unit forward and reverse axle
                     counts at the Local unit


Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   RLYA_MGR.c  -   Start_Relay_A_Mgr()
                        RLYA_MGR.c  -   Set_Relay_A_Reset_Stat()
                        RLYD3_MGR.c -   Start_Relay_D3_Mgr()

Input Variables         Name                        Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.US_Fwd_Axle_Count    UINT16
                        Track_Info_4.US_Rev_Axle_Count    UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

************************************************************************/
void Clear_US_Local_Counts_4(void)
{
    Track_Info_4.US_Fwd_Axle_Count = 0;
    Track_Info_4.US_Rev_Axle_Count = 0;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Clear_PD1_Supervisory_Count_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the Phase detector1 supervisory counts

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Determine_TrackState_4()

Input Variables             Name                            Type
    Global          :   None

    Local           :   None

Output Variables            Name                            Type
    Global          :   Track_Info_4.PD1S_Count               UINT16
                        Track_Info_4.PD1S_Falling_Edge_Count  UINT16
                        Track_Info_4.PD1S_Rising_Edge_Count   UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            FALSE                           0

References          :

Derived Requirements:

************************************************************************/
void Clear_PD1_Supervisory_Count_4(void)
{
    Track_Info_4.PD1S_Count = 0;
    Track_Info_4.Flags.PD_Without_Overlapping = FALSE;
    Track_Info_4.PD1S_Falling_Edge_Count = 0;
    Track_Info_4.PD1S_Rising_Edge_Count  = 0;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Clear_PD2_Supervisory_Count_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the Phase detector2 supervisory counts


Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Determine_TrackState_4()

Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables            Name                            Type
    Global          :   Track_Info_4.PD2S_Count               UINT16
                        Track_Info_4.PD2S_Falling_Edge_Count  UINT16
                        Track_Info_4.PD2S_Rising_Edge_Count   UINT16

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            FALSE                           0

References          :

Derived Requirements:

************************************************************************/
void Clear_PD2_Supervisory_Count_4(void)
{
    Track_Info_4.PD2S_Count = 0;
    Track_Info_4.PD2S_Falling_Edge_Count = 0;
    Track_Info_4.PD2S_Rising_Edge_Count  = 0;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Clear_PD12_Main_Pulse_Count_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the main counts at PD1 and PD2

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Detect_PD_Failures_4()

Input Variables         Name                                Type
    Global          :   None

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.PD1M_Count           UINT16
                        Track_Info_4.PD2M_Count           UINT16

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

************************************************************************/
void Clear_PD12_Main_Pulse_Count_4(void)
{
    Track_Info_4.PD1M_Count = 0;
    Track_Info_4.PD2M_Count = 0;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Clear_Wheel_Type_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the wheel type detected at the PDs at remote units

Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Detect_PD_Failures_4()
                        AXLE_MON.c  -   Clear_US_AxleCount()
                        AXLE_MON.c  -   Clear_DS_AxleCount()

Input Variables         Name                            Type
    Global          :   Track_Info_4.US_Axle_Count        UINT16
                        WHEEL_TYPE_NOT_DETERMINED       Enumerator
                        Track_Info_4.DS_Axle_Count        UINT16
                        Track_Info_4.PD1M_Count           UINT16
                        Track_Info_4.PD2M_Count           UINT16

    Local           :   None

Output Variables        Name                            Type
    Global          :   Track_Info_4.Wheel_Type           Enumerator

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Clear the wheel type determined whenever the Upstream or downstream axle count is zero
                     2.Clear the wheel type determined whenever the PD1M or PD2M axle count is zero
************************************************************************/
void Clear_Wheel_Type_4(void)
{
    if (Track_Info_4.LCWS_Fwd_Count == 0)
    {
    Track_Info_4.Wheel_Type = WHEEL_TYPE_NOT_DETERMINED;
    }
    if (Track_Info_4.LCWS_Rev_Count == 0)
    {
    Track_Info_4.Wheel_Type = WHEEL_TYPE_NOT_DETERMINED;
    }
    if(Track_Info_4.PD1M_Count == 0 && Track_Info_4.PD2M_Count == 0)
    {
     Track_Info_4.Wheel_Type = WHEEL_TYPE_NOT_DETERMINED;
    }
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Start_US_Axle_Counting(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the Up stream remote unit axle counting

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()
                        RLYA_MGR.c  -   Update_Relay_A_State()

Input Variables             Name                                Type
    Global          :   None

    Local           :   None

Output Variables            Name                                Type
    Global          :   Track_Info_4.Flags.US_Counting_Enable     Bit

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil
Macro definitions used:     Macro                       Value
                            SET_HIGH                    1

References          :

Derived Requirements:

************************************************************************/
void Start_US_Axle_Counting_4(void)
{
 Track_Info_4.Flags.US_Counting_Enable = SET_HIGH;
}

//01/09/10
void Start_FDP_US_Axle_Counting_4(void)
{
 Track_Info_4.Flags.FDP_US_Counting_Enable = SET_HIGH;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Start_DS_Axle_Counting(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the Down stream remote unit axle counting

Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_DE()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()
                        RLYB_MGR.c  -   Update_Relay_B_State

Input Variables         Name                            Type
    Global          :   None

    Local           :   None

Output Variables        Name                                    Type
    Global          :   Track_Info_4.Flags.DS_Counting_Enable     Bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                       Value
                            SET_HIGH                    1

References          :

Derived Requirements:
************************************************************************/
void Start_DS_Axle_Counting_4(void)
{
 Track_Info_4.Flags.DS_Counting_Enable = SET_HIGH;
}
//01/09/10
void Start_FDP_DS_Axle_Counting_4(void)
{
 Track_Info_4.Flags.FDP_DS_Counting_Enable = SET_HIGH;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Stop_US_Axle_Counting(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Stop the Up stream remote unit axle counting

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   RELAYMGR.c  -   Declare_DAC_Defective()
                        RELAYMGR.c  -   Declare_DAC_Defective_US()
                        RELAYMGR.c  -   Declare_DAC_Defective_DS()
                        RLYA_MGR.c  -   Set_Relay_A_Reset_State()

Input Variables         Name                                Type
    Global          :   None

    Local           :   None

Output Variables            Name                                Type
    Global          :   Track_Info_4.Flags.US_Counting_Enable     Bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_LOW                     0

References          :

Derived Requirements:
************************************************************************/
void Stop_US_Axle_Counting_4(void)
{
 Track_Info_4.Flags.US_Counting_Enable = SET_LOW;
}
//01/09/10
void Stop_FDP_US_Axle_Counting_4(void)
{
 Track_Info_4.Flags.FDP_US_Counting_Enable = SET_LOW;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :void Stop_DS_Axle_Counting(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Stop the down stream remote unit axle counting

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   RELAYMGR.c  -   Declare_DAC_Defective()
                        REALYMGR.c  -   Declare_DAC_Defective_US()
                        REALYMGR.c  -   Declare_DAC_Defective_DS()
                        RLYB_MGR.c  -   Set_Relay_B_Reset_State()

Input Variables             Name                                Type
    Global          :   None                                    None

    Local           :   None                                    None

Output Variables            Name                                Type
    Global          :   Track_Info_4.Flags.DS_Counting_Enable     Bit

    Local           :   None                                    None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_LOW                         0

References          :

Derived Requirements:

Algorithm           :Disable the downstream axle counting by set the flag DS_Counting_Enable as LOW
************************************************************************/
void Stop_DS_Axle_Counting_4(void)
{
 Track_Info_4.Flags.DS_Counting_Enable = SET_LOW;
}
//01/09/10

void Stop_FDP_DS_Axle_Counting_4(void)
{
 Track_Info_4.Flags.FDP_DS_Counting_Enable = SET_LOW;
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :BOOL Get_DS_Local_Counts_Clearing_Status(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract                :Return the status of the flag Track_Info_4.Flags.DS_Local_Counts_Just_Cleared

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls               :   Nil

    Called by       :   RLYB_MGR.c  - Update_Relay_B_State()

Input Variables             Name                                                 Type
    Global          :   Track_Info_4.Flags.DS_Local_Counts_Just_Cleared            Bit

    Local           :   ReturnValue                                              BOOL

Output Variables            Name                                Type
    Global          :   None                                    None

    Local           :   ReturnValue                             BOOL
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_HIGH                         1
                            TRUE                             1
                            FALSE                            0
References          :

Derived Requirements:

Algorithm           :1.If the Downstream local count cleared flag got setted
                        return the value as 1 otherwise return zero
************************************************************************/
 BOOL Get_DS_Local_Counts_Clearing_Status_4(void)
{
  BOOL ReturnValue = FALSE;
   if(Track_Info_4.Flags.DS_Local_Counts_Just_Cleared == TRUE)
      {
         ReturnValue = (BOOL)SET_HIGH;
      }
  return(ReturnValue);
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :BOOL Get_US_Local_Counts_Clearing_Status(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Return the status of the flag Track_Info_4.Flags.US_Local_Counts_Just_Cleared

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   RLYA_MGR.c  - Update_Relay_A_State()

Input Variables             Name                                          Type
    Global          :   Track_Info_4.Flags.US_Local_Counts_Just_Cleared      Bit

    Local           :   ReturnValue                                        BOOL

Output Variables            Name                                Type
    Global          :   None                                    None

    Local           :   ReturnValue                              BOOL
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_LOW                         0
                            SET_HIGH                        1
                            TRUE                            1

References          :

Derived Requirements:

Algorithm           :1.If the Upstream local count cleared flag got setted
                        return the value as 1 otherwise return zero
************************************************************************/
BOOL Get_US_Local_Counts_Clearing_Status_4(void)
{
  BOOL ReturnValue = FALSE;
   if(Track_Info_4.Flags.US_Local_Counts_Just_Cleared == TRUE)
      {
         ReturnValue = (BOOL) SET_HIGH;
      }
  return(ReturnValue);
}
/*********************************************************************
Component name      :AXLE_MON
Module Name         :BOOL Chk_For_Track_Occupancy_4(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Return the status of the flag Track_Info_4.Flags.US_Local_Counts_Just_Cleared

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   RLYA_MGR.c  - Update_Relay_A_State()

Input Variables             Name                                          Type
    Global          :   Track_Info_4.Flags.US_Local_Counts_Just_Cleared      Bit

    Local           :   ReturnValue                                        BOOL

Output Variables            Name                                Type
    Global          :   None                                    None

    Local           :   ReturnValue                              BOOL
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_LOW                         0
                            SET_HIGH                        1
                            TRUE                            1

References          :

Derived Requirements:

Algorithm           :1.If the Upstream local count cleared flag got setted
                        return the value as 1 otherwise return zero
************************************************************************/
BOOL Chk_For_Track_Occupancy_4(void)
{
 if (Track_Info_1.LCWS_Fwd_Count == 0 && Track_Info_2.LCWS_Fwd_Count == 0 && Track_Info_3.LCWS_Fwd_Count == 0 && Track_Info_4.LCWS_Fwd_Count == 0 &&
     Track_Info_1.LCWS_Rev_Count == 0 && Track_Info_2.LCWS_Rev_Count == 0 && Track_Info_3.LCWS_Rev_Count == 0 && Track_Info_4.LCWS_Rev_Count == 0 )
  {

     return((BOOL)FALSE);
  }
 return((BOOL)TRUE);

}

