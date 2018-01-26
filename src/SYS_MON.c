/*****************************************************************************
    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    SYS_MON
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
    Functions   :   void Initialise_Sys_Mon(void)
                    void Update_Sys_Mon_State(void)
                    void Start_Sys_Mon_Decrement_50msTmr(void)
                    void Decrement_Sys_Mon_50msTmr(void)
                    void Start_FeedBack_Check(feedBack_ID uchID)
                    void Stop_FeedBack_Check(feedBack_ID uchID)
                    void Update_FeedBack_State(feedBack_ID uchID)
                    void Read_FeedBack_Signal_Port(feedBack_ID uchID)
                    void Check_Communication_Links(void)
                    void Check_Communication_Links_SF1(void)
                    void Check_Communication_Links_SF2(void)
                    void Check_Communication_US_CF1(void)
                    void Check_Communication_US_CF2(void)
                    void Check_Communication_DS_CF1(void)
                    void Check_Communication_DS_CF2(void)
                    void Check_Communication_Links_EF1(void)
                    void Check_Communication_Links_EF2(void)
                    void Check_Inter_Processor_Link(void)
                    void Update_DS_Track_Status(bitadrb_t Flags)
                    void Update_US_Track_Status(bitadrb_t Flags)
                    void Check_Power_Status(void)
                    void Check_DS_Power_Status(void)
                    void Check_US_Power_Status(void)
                    void Update_US_Power_Status(  bitadrb_t DAC_Config,bitadrb_t Flags1)
                    void Update_DS_Power_Status(  bitadrb_t DAC_Config,bitadrb_t Flags1)
                    void Clear_US_Power_Status(void)
                    void Clear_DS_Power_Status(void)
                    void Clear_Comm_US_Failures(void)
                    void Clear_Comm_DS_Failures(void)
*****************************************************************************/

#include <xc.h>

#include "COMMON.h"
#include "SYS_MON.h"
#include "ERROR.h"
#include "COMM_SM.h"
#include "RESET.h"


extern  /*near*/  dac_status_t Status;                    /* from dac_main.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from dac_main.c */
extern comm_a_countdown_t Comm_A_CountDown;         /* from comm_us.c */
extern comm_b_countdown_t Comm_B_CountDown;         /* from comm_ds.c */
extern msg_info_t   Com2RecvObject;    /* COM2: Message Receiving Buffer etc., */
extern msg_info_t   Com1RecvObject;    /* COM2: Message Receiving Buffer etc., */
extern ds_section_mode DS_Section_Mode;            /*structure to hold the DS section local and remote unit modes */
extern us_section_mode US_Section_Mode;            /*structure to hold the US section local and remote unit modes */

extern spi_transmit_object SPI_Transmit_Object;
power_supply_status Power_Supply_Status;

sysmon_info_t SysMon_Info;
feedback_info_t FeedBack_Info[MAXIMUM_TYPES_OF_FEEDBACKS];
feedback_error FeedBack_Error;
track_status_info Track_Status_Info;
comm_fail_check comm_check_SF1=
{
    .State = COMM_GOOD,
    .Wait_timeout = 0,
    .Wheel_match_fail = 0,
    .Track_counts = {0,0,0,0,0,0,0,0,0,0},
    .Local_counts = {0,0,0,0,0,0,0,0,0,0}
};
comm_fail_check comm_check_SF2=
{
    .State = COMM_GOOD,
    .Wait_timeout = 0,
    .Wheel_match_fail = 0,
    .Track_counts = {0,0,0,0,0,0,0,0,0,0},
    .Local_counts = {0,0,0,0,0,0,0,0,0,0}
};

comm_fail_check comm_check_EF1=
{
    .State = COMM_GOOD,
    .Wait_timeout = 0,
    .Wheel_match_fail = 0,
    .Track_counts = {0,0,0,0,0,0,0,0,0,0},
    .Local_counts = {0,0,0,0,0,0,0,0,0,0}
};
comm_fail_check comm_check_EF2={
    .State = COMM_GOOD,
    .Wait_timeout = 0,
    .Wheel_match_fail = 0,
    .Track_counts = {0,0,0,0,0,0,0,0,0,0},
    .Local_counts = {0,0,0,0,0,0,0,0,0,0}
};



void Update_FeedBack_State(BYTE uchID);
void Read_FeedBack_Signal_Port(BYTE uchID);
void Check_Communication_Links(void);
void Check_Communication_Links_SF1(void);
void Check_Communication_Links_SF2(void);
void Check_Communication_US_CF1(void);
void Check_Communication_US_CF2(void);
void Check_Communication_DS_CF1(void);
void Check_Communication_DS_CF2(void);
void Check_Communication_Links_EF1(void);
void Check_Communication_Links_EF2(void);
void Check_Inter_Processor_Link(void);

void Check_Power_Status(void);
void Check_US_Power_Status(void);
void Check_DS_Power_Status(void);
void Decrement_Sys_Mon_50msTmr(void);
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Initialise_Sys_Mon(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to initialise system monitor

Allocated Requirements: (SSDAC_SWRS_0032)

Design Requirements:	SSDAC_DR_5288

Interfaces
    Calls           :None

    Called by       :DAC.MAIN.C- Initialise_System()

Input Variables     :               Name                        Type
    Global          :   SYS_CHECK_BOARDS                        Enumerator
                        FEEDBACK_DECREMENT_IDLE                 Enumerator
                        FB_CHECK_IDLE                           Enumerator

    Local           :   uchCnt                                  BYTE

Output Variables    :               Name                        Type
    Global          :   SysMon_Info.State                       Enumerator
                        SysMon_Info.Timeout_50ms                UINT16
                        SysMon_Info.Decrement_State             Enumerator
                        FeedBack_Info[uchCnt].State             Enumerator
                        FeedBack_Error.Status_Rly_Fb_Error      BYTE
                        FeedBack_Error.Vital_Rly_A_Fb_Error     BYTE
                        FeedBack_Error.Vital_Rly_B_Fb_Error     BYTE
                        Power_Supply_Status.Byte                BYTE

    Local           :   None                                        None

Signal Variables


Macros defined      :                   Macro                   Value
                                SYS_STABILISATION_TIME           200
                                MAXIMUM_TYPES_OF_FEEDBACKS        10

References          :

Derived Requirements:

Algorithm           :1.Initialise the state as SYS check Boards
                     2.Load the timer value
                     3.Initialise the decreament state as IDLE
                     4.Initialise all the feedback state as idle and error as zero
*******************************************************************************/

void Initialise_Sys_Mon(void)
{
    BYTE uchCnt;

    SysMon_Info.State = SYS_CHECK_BOARDS;
    SysMon_Info.Timeout_50ms    = SYS_STABILISATION_TIME;
    SysMon_Info.Decrement_State = FEEDBACK_DECREMENT_IDLE;

    for (uchCnt = 0; uchCnt < MAXIMUM_TYPES_OF_FEEDBACKS; uchCnt++)
    {
        FeedBack_Info[uchCnt].State = FB_CHECK_IDLE;
    }
    FeedBack_Error.Status_Rly_A_Fb_Error = 0;
    FeedBack_Error.Status_Rly_B_Fb_Error = 0;
    FeedBack_Error.Vital_Rly_A_Fb_Error = 0;
    FeedBack_Error.Vital_Rly_B_Fb_Error = 0;
    Power_Supply_Status.Byte = (BYTE) 0x00;
    Track_Status_Info.Byte   = (BYTE) 0x00;
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Update_Sys_Mon_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to update the state of system monitor

Allocated Requirements: (SSDAC_SWRS_0813), (SSDAC_SWRS_0107), (SSDAC_SWRS_0108), (SSDAC_SWRS_0111)
						(SSDAC_SWRS_0828)

Design Requirements:	SSDAC_DR_5289

Interfaces
    Calls           :SYS_MON.C-Decrement_Sys_Mon_50msTmr()
                     RESTORE.C-Check_DAC_Boards_Runtime()
                     SYS_MON.C-Check_Communication_Links()
                     SYS_MON.C-Check_Inter_Processor_Link()
                     SYS_MON.C-Check_Power_Status()
                     SYS_MON.C-Update_FeedBack_State(uchFB_Index)

    Called by       :DAC_MAIN.C-Control_DAC_Type_SF
                     DAC_MAIN.C-Control_DAC_Type_CF
                     DAC_MAIN.C-Control_DAC_Type_EF
                     DAC_MAIN.C-Control_DAC_Type_DE
                     DAC_MAIN.C-Control_DAC_Type_D3

Input Variables     :               Name                            Type
    Global          :   SYS_CHECK_COMMUNICATION_LINKS               Enumerator
                        SYS_CHECK_POWER_STATUS                      Enumerator
                        SYS_FEEDBACK_SIGNALS                        Enumerator
                        SYS_CHECK_BOARDS                            Enumerator

    Local           :   uchFB_Index                                 static BYTE

Output Variables    :               Name                            Type
    Global          :   SysMon_Info.State                           Enumerator

    Local           :   uchFB_Index                                 static BYTE

Signal Variables


Macros defined      :                   Macro               Value
                            MAXIMUM_TYPES_OF_FEEDBACKS        10

References          :

Derived Requirements:

Algorithm           :1.It shall check the presence of all the hardware modules in the local system
                     2.It shall check all the communication links between the
                       units required to establish communication betweeen the local & remote units
                     3.It shall check Inter Processor Links within the CPU'S & SM-CPU required to
                       establish SPI
                     4.It shall check the power status of the remote unit
                     5.It shall check the feedback signals from status relay, vital relay & repeat relays
                       to know the status


*******************************************************************************/
void Update_Sys_Mon_State(void)
{
    static BYTE uchFB_Index = 0;


    Decrement_Sys_Mon_50msTmr();

    switch (SysMon_Info.State)
    {
        case SYS_CHECK_BOARDS:
            Check_DAC_Boards_Runtime();     /* from restore.c */
            SysMon_Info.State = SYS_CHECK_COMMUNICATION_LINKS;
            break;
        case SYS_CHECK_COMMUNICATION_LINKS:
            if (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE)
            {
                SysMon_Info.State = SYS_FEEDBACK_SIGNALS;
            }
            else
            {
                Check_Communication_Links();
                SysMon_Info.State = SYS_CHECK_POWER_STATUS;
            }
            Check_Inter_Processor_Link();
            
            break;
        case SYS_CHECK_POWER_STATUS:
            Check_Power_Status();
            SysMon_Info.State = SYS_FEEDBACK_SIGNALS;
            break;
        case SYS_FEEDBACK_SIGNALS:
            Update_FeedBack_State(uchFB_Index);
            uchFB_Index = uchFB_Index + 1;
            if (uchFB_Index >= MAXIMUM_TYPES_OF_FEEDBACKS)
            {
                uchFB_Index = 0;
            }
            SysMon_Info.State = SYS_CHECK_BOARDS;
            break;
    }
}

//21_12_09


/*******************************************************************************
Component name      :SYS_MON
Module Name         :void Start_Sys_Mon_Decrement_50msTmr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to start a system monitoring scheduling timer for each system monitor state


Allocated Requirements:

Design Requirements:	SSDAC_DR_5290

Interfaces
    Calls           :None

    Called by       :DAC_MAIN.C-Start_SF_Reset_Sequence()
                     DAC_MAIN.C-Start_CF_Reset_Sequence()
                     DAC_MAIN.C-Start_EF_Reset_Sequence()
                     DAC_MAIN.C-Start_DE_Reset_Sequence()
                     DAC_MAIN.C-Control_DAC_Type_SF()
                     DAC_MAIN.C-Control_DAC_Type_CF()
                     DAC_MAIN.C-Control_DAC_Type_EF()
                     DAC_MAIN.C-Control_DAC_Type_DE()
                     DAC_MAIN.C-Control_DAC_Type_D3()

Input Variables     :               Name                Type
    Global          :   DECREMENT_FEEDBACK_TIMER0       Enumerator

    Local           :       None                            None

Output Variables    :               Name                Type
    Global          :   SysMon_Info.Decrement_State     Enumerator

    Local           :       None                            None

Signal Variables



Macros defined      :                   Macro               Value
                                        None                    None
References          :

Derived Requirements:
*******************************************************************************/
void Start_Sys_Mon_Decrement_50msTmr(void)
{
     SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER0;
}

/******************************************************************************
Component name      :SYS_MON
Module Name         :void Decrement_Sys_Mon_50msTmr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :1.Feed back decrement state to idle state which is the initialisation state
                     2.Decrement feedback 50ms timer0 if it is greater than 0 after energising the status relay
                     3.Decrement feedback 50ms timer1 if it is greater than 0 after de-energising the status relay
                     4.Decrement feedback 50ms timer2 if it is greater than 0 after vital relay A is energised
                     5.Decrement feedback 50ms timer3 if it is greater than 0 after vital relay A is de-energised
                     6.Decrement feedback 50ms timer4 if it is greater than 0 after vital relay B is energised
                     7.Decrement feedback 50ms timer5 if it is greater than 0 after vital relay B is de-energised
                     8.Decrement feedback 50ms timer6 if it is greater than 0 after checking the US track status
                       were vital relay A is a repeat relay
                     9.Decrement feedback 50ms timer7 if it is greater than 0 after checking the DS track status
                       were vital relay B is a repeat relay


Allocated Requirements:

Design Requirements:	SSDAC_DR_5291

Interfaces
    Calls           :None

    Called by       :SYS_MON.C-Update_Sys_Mon_State()
Input Variables     :               Name                Type
    Global          :   DECREMENT_FEEDBACK_TIMER1       Enumerator
                        DECREMENT_FEEDBACK_TIMER2       Enumerator
                        DECREMENT_FEEDBACK_TIMER3       Enumerator
                        DECREMENT_FEEDBACK_TIMER4       Enumerator
                        DECREMENT_FEEDBACK_TIMER5       Enumerator
                        DECREMENT_FEEDBACK_TIMER6       Enumerator
                        DECREMENT_FEEDBACK_TIMER7       Enumerator
                        FEEDBACK_DECREMENT_IDLE         Enumerator
                        FeedBack_Info[0].Timeout_50ms   Byte
                        FeedBack_Info[1].Timeout_50ms   Byte
                        FeedBack_Info[2].Timeout_50ms   Byte
                        FeedBack_Info[3].Timeout_50ms   Byte
                        FeedBack_Info[4].Timeout_50ms   Byte
                        FeedBack_Info[5].Timeout_50ms   Byte
                        FeedBack_Info[6].Timeout_50ms   Byte
                        FeedBack_Info[7].Timeout_50ms   Byte

    Local           :       None                            None

Output Variables    :               Name                Type
    Global          :   FeedBack_Info[0].Timeout_50ms   Byte
                        FeedBack_Info[1].Timeout_50ms   Byte
                        FeedBack_Info[2].Timeout_50ms   Byte
                        FeedBack_Info[3].Timeout_50ms   Byte
                        FeedBack_Info[4].Timeout_50ms   Byte
                        FeedBack_Info[5].Timeout_50ms   Byte
                        FeedBack_Info[6].Timeout_50ms   Byte
                        FeedBack_Info[7].Timeout_50ms   Byte
                        SysMon_Info.Decrement_State     Enumerator

    Local           :       None                            None

Signal Variables


Macros defined      :                   Macro               Value
                                        None                None
References          :

Derived Requirements:

Algorithm           :1.Decrement the timer values if it is greater than zero and change the state
                        to decrement the next timer value after every decrement
                     2.If all the timer got decremented once then change the state to idle
*******************************************************************************/
void Decrement_Sys_Mon_50msTmr(void)
{
    switch(SysMon_Info.Decrement_State)
    {
        case FEEDBACK_DECREMENT_IDLE:
             break;
        case DECREMENT_FEEDBACK_TIMER0:
             if (FeedBack_Info[0].Timeout_50ms > 0)
             {
               FeedBack_Info[0].Timeout_50ms = FeedBack_Info[0].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER1;
             break;
        case DECREMENT_FEEDBACK_TIMER1:
             if(FeedBack_Info[1].Timeout_50ms > 0)
             {
               FeedBack_Info[1].Timeout_50ms = FeedBack_Info[1].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER2;
             break;
        case DECREMENT_FEEDBACK_TIMER2:
             if (FeedBack_Info[2].Timeout_50ms > 0)
             {
               FeedBack_Info[2].Timeout_50ms = FeedBack_Info[2].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER3;
             break;
        case DECREMENT_FEEDBACK_TIMER3:
             if(FeedBack_Info[3].Timeout_50ms > 0)
             {
               FeedBack_Info[3].Timeout_50ms = FeedBack_Info[3].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER4;
             break;
        case DECREMENT_FEEDBACK_TIMER4:
             if(FeedBack_Info[4].Timeout_50ms > 0)
             {
               FeedBack_Info[4].Timeout_50ms = FeedBack_Info[4].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER5;
             break;
        case DECREMENT_FEEDBACK_TIMER5:
             if(FeedBack_Info[5].Timeout_50ms > 0)
             {
               FeedBack_Info[5].Timeout_50ms = FeedBack_Info[5].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER6;
             break;
        case DECREMENT_FEEDBACK_TIMER6:
             if(FeedBack_Info[6].Timeout_50ms > 0)
             {
               FeedBack_Info[6].Timeout_50ms = FeedBack_Info[6].Timeout_50ms- 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER7;
             break;
        case DECREMENT_FEEDBACK_TIMER7:
             if(FeedBack_Info[7].Timeout_50ms > 0)
             {
               FeedBack_Info[7].Timeout_50ms = FeedBack_Info[7].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER8;
             break;
        case DECREMENT_FEEDBACK_TIMER8:
             if(FeedBack_Info[8].Timeout_50ms > 0)
             {
               FeedBack_Info[8].Timeout_50ms = FeedBack_Info[8].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = DECREMENT_FEEDBACK_TIMER9;
             break;
        case DECREMENT_FEEDBACK_TIMER9:
             if(FeedBack_Info[9].Timeout_50ms > 0)
             {
               FeedBack_Info[9].Timeout_50ms = FeedBack_Info[9].Timeout_50ms - 1;
             }
             SysMon_Info.Decrement_State = FEEDBACK_DECREMENT_IDLE;
             break;
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Start_FeedBack_Check(BYTE uchID)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to start the feedback check on the status of all the relays and the track status at
                     remote units


Allocated Requirements: (SSDAC_SWRS_0813), (SSDAC_SWRS_0111)

Design Requirements:	SSDAC_DR_5292

Interfaces
    Calls           :None

    Called by       :RELAYDRV.C-Energise_Preparatory_Relay()
                     RELAYDRV.C-Energise_Vital_Relay_A
                     RELAYDRV.C-Energise_Vital_Relay_B
                     RELAYDRV.C-DeEnergise_Preparatory_Relay
                     RELAYDRV.C-DeEnergise_Vital_Relay_A
                     RELAYDRV.C-DeEnergise_Vital_Relay_B
                     SYS_MON.C-Update_DS_Track_Status
                     SYS_MON.C-Update_US_Track_Status

Input Variables                     Name                    Type
    Global          :   FB_CHECK_DEBOUNCE_WAIT              Enumerator
                        FB_CHECK_IDLE                       Enumerator
                        DAC_UNIT_TYPE_SF                    Enumerator
                        DAC_UNIT_TYPE_DE                    Enumerator
                        DAC_UNIT_TYPE_EF                    Enumerator
                        DIP_Switch_Info.DAC_Unit_Type           Enumerator
                        Status.Flags.US_System_Status       Unsigned
                        Status.Flags.DS_System_Status       Unsigned

    Local           :   uchID                               feedBack_ID

Output Variables                    Name                    Type
    Global          :   FeedBack_Info[uchID].State          Enumerator
                        FeedBack_Info[uchID].Timeout_50ms   BYTE

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                            RELAY_FB_DEBOUNCE_TIMEOUT           30
                            NORMAL                              1
                            TRUE                                1
                            TRACK_STATUS_COMPARE_WAIT_TIMEOUT   200
References          :
Derived Requirements:
Algorithm           :1.Activate feedback checking and load relay feedback checking time as 1.5s
                     2.De-activate the feedback checking signal after energizing the status relay
                     3.De-activate the feedback checking signal after de-energizing the status relay
                     4.De-activate the feedback checking signal after energizing the vital relay A
                     5.De-activate the feedback checking signal after de-energizing the vital relay A
                     6.De-activate the feedback checking signal after energizing the vital relay B
                     7.De-activate the feedback checking signal after de-energizing the vital relay B
                     8.De-activate the feedback checking of US track status signal after de-energizing the
                       vital relay A but if the unit type is SF & of IBS configuration then this Vital
                       relay A will become reapeat relay for the next block status
                     9.De-activate the feedback checking of DS track status signal after de-energizing the
                       vital relay B but if the unit type is EF & of IBS configuration then this Vital
                       relay B will become reapeat relay for the next block status
*******************************************************************************/

void Start_FeedBack_Check(feedBack_ID uchID)
{

//    if(FeedBack_Info[uchID].State != FB_CHECK_IDLE) // If the requested F/B is already under check, no need to do anything
//    {
//        return;
//    }
    
    /* Activate the FeedBack Checking */
    FeedBack_Info[uchID].State = FB_CHECK_DEBOUNCE_WAIT;
    FeedBack_Info[uchID].Timeout_50ms = RELAY_FB_DEBOUNCE_TIMEOUT;

    /* De-Activate the FeedBack Checking of Same Signal, but different logical level */
    switch (uchID)
    {
        case FB_STATUS_RELAY_A_ON_ID:
            FeedBack_Info[FB_STATUS_RELAY_A_OFF_ID].State = FB_CHECK_IDLE;
            FeedBack_Info[FB_STATUS_RELAY_A_OFF_ID].Timeout_50ms = (BYTE) 0;
            break;
        case FB_STATUS_RELAY_A_OFF_ID:
            FeedBack_Info[FB_STATUS_RELAY_A_ON_ID].State = FB_CHECK_IDLE;
            FeedBack_Info[FB_STATUS_RELAY_A_ON_ID].Timeout_50ms = (BYTE) 0;
            break;
        case FB_STATUS_RELAY_B_ON_ID:
            FeedBack_Info[FB_STATUS_RELAY_B_OFF_ID].State = FB_CHECK_IDLE;
            FeedBack_Info[FB_STATUS_RELAY_B_OFF_ID].Timeout_50ms = (BYTE) 0;
            break;
        case FB_STATUS_RELAY_B_OFF_ID:
            FeedBack_Info[FB_STATUS_RELAY_B_ON_ID].State = FB_CHECK_IDLE;
            FeedBack_Info[FB_STATUS_RELAY_B_ON_ID].Timeout_50ms = (BYTE) 0;
            break;
        case FB_VITAL_RELAY_A_ON_ID:
            FeedBack_Info[FB_VITAL_RELAY_A_OFF_ID].State = FB_CHECK_IDLE;
            FeedBack_Info[FB_VITAL_RELAY_A_OFF_ID].Timeout_50ms = (BYTE) 0;
            break;
        case FB_VITAL_RELAY_A_OFF_ID:
            FeedBack_Info[FB_VITAL_RELAY_A_ON_ID].State = FB_CHECK_IDLE;
            FeedBack_Info[FB_VITAL_RELAY_A_ON_ID].Timeout_50ms = (BYTE) 0;
            break;
        case FB_VITAL_RELAY_B_ON_ID:
            FeedBack_Info[FB_VITAL_RELAY_B_OFF_ID].State = FB_CHECK_IDLE;
            FeedBack_Info[FB_VITAL_RELAY_B_OFF_ID].Timeout_50ms = (BYTE) 0;
            break;
        case FB_VITAL_RELAY_B_OFF_ID:
            FeedBack_Info[FB_VITAL_RELAY_B_ON_ID].State = FB_CHECK_IDLE;
            FeedBack_Info[FB_VITAL_RELAY_B_ON_ID].Timeout_50ms = (BYTE) 0;
            break;
        case US_TRACK_STATUS_CHECK_ID:
            if(Status.Flags.US_System_Status != NORMAL)
            {
            /* Already local system failed, So no need to compare Relay status */
              FeedBack_Info[uchID].State = FB_CHECK_IDLE;
              FeedBack_Info[uchID].Timeout_50ms = (BYTE) 0;
              break;
            }
            FeedBack_Info[uchID].Timeout_50ms = TRACK_STATUS_COMPARE_WAIT_TIMEOUT;
            break;
        case DS_TRACK_STATUS_CHECK_ID:
            if(Status.Flags.DS_System_Status != NORMAL)
            {
            /* Already local system failed, So no need to compare Relay status */
               FeedBack_Info[uchID].State = FB_CHECK_IDLE;
               FeedBack_Info[uchID].Timeout_50ms = (BYTE) 0;
               break;
            }
            FeedBack_Info[uchID].Timeout_50ms = TRACK_STATUS_COMPARE_WAIT_TIMEOUT;
            break;
    }

}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Stop_FeedBack_Check(feedBack_ID uchID)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to stop the feedback check on the status of all the relays and the track status at
                     the remote units

Allocated Requirements:(SSDAC_SWRS_0813), (SSDAC_SWRS _0111)

Design Requirements: SSDAC_DR_5293

Interfaces
    Calls           :None

    Called by       :None

Input Variables                     Name                    Type
    Global          :   FB_CHECK_IDLE                       Enumerator

    Local           :       uchID                           feedBack_ID

Output Variables                    Name                    Type
    Global          :   FeedBack_Info[uchID].State          Enumerator
                        FeedBack_Info[uchID].Timeout_50ms   BYTE
    Local           :

Signal Variables


Macros defined      :                   Macro               Value
                                        None
References          :

Derived Requirements:

Algorithm           :1.Change the feedback info state as idle
                     2.Clear the timeout value
*******************************************************************************/
void Stop_FeedBack_Check(feedBack_ID uchID)
{

    FeedBack_Info[uchID].State = FB_CHECK_IDLE;
    FeedBack_Info[uchID].Timeout_50ms = (BYTE) 0;
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Update_FeedBack_State(BYTE uchID)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to update the feedback status of all the relays and track status at remote units
                     to the feedback information signal port

Allocated Requirements: (SSDAC_SWRS_0813), (SSDAC_SWRS _0111)

Design Requirements:	SSDAC_DR_5294
	
Interfaces
    Calls           :   SYS_MON.c-Read_FeedBack_Signal_Port()

    Called by       :   SYS_MON.C-Update_Sys_Mon_State()
                        SYS_MON.C-Update_FeedBack_State()

Input Variables                     Name                    Type
    Global          :   FB_CHECK_DEBOUNCE_WAIT              Enumerator
                        FeedBack_Info[uchID].State          Enumerator
                        FeedBack_Info[uchID].Timeout_50ms   Byte
                        FB_CHECK_PORT                       Enumerator

    Local           :           uchID                       BYTE

Output Variables                    Name                    Type
    Global          :   FeedBack_Info[uchID].State          Enumerator
                        FeedBack_Info[uchID].Timeout_50ms   Byte

    Local           :   None

Signal Variables


Macros defined      :                   Macro               Value
                        TIMEOUT_EVENT                         0
                        RELAY_FB_DEBOUNCE_TIMEOUT            30
References          :
Derived Requirements:
Algorithm           :1.If the feedback checking is activated and relay feed back checking time  1.5s has overflowed then
                       check the feedback port
                     2.If the feedback state is to check the feedback port then read the feedback signal port
                       which gives the status of remote units & again initialize the state to activate the
                       feedback checking for the next check
*******************************************************************************/
void Update_FeedBack_State(BYTE uchID)
{

    if (FeedBack_Info[uchID].State == FB_CHECK_DEBOUNCE_WAIT &&
        FeedBack_Info[uchID].Timeout_50ms == TIMEOUT_EVENT)
    {
        FeedBack_Info[uchID].State = FB_CHECK_PORT;
        return;
    }
    if (FeedBack_Info[uchID].State == FB_CHECK_PORT)
    {
        Read_FeedBack_Signal_Port(uchID);
        FeedBack_Info[uchID].State = FB_CHECK_DEBOUNCE_WAIT;
        FeedBack_Info[uchID].Timeout_50ms = RELAY_FB_DEBOUNCE_TIMEOUT;
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Read_FeedBack_Signal_Port(BYTE uchID)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            : Routine to read the feedback status of all the relays and the track status at remote units
                      from the feedback information signal port


Allocated Requirements: (SSDAC_SWRS_0005), (SSDAC_SWRS_0813), (SSDAC_SWRS_0814), (SSDAC_SWRS_0111)
					   (SSDAC_SWRS_0461), (SSDAC_SWRS_0462), (SSDAC_SWRS_0463), (SSDAC_SWRS_0464)
     				   (SSDAC_SWRS_0465), (SSDAC_SWRS_0486), (SSDAC_SWRS_0410), (SSDAC_SWRS_0489)
					   (SSDAC_SWRS_0572), (SSDAC_SWRS_0573), (SSDAC_SWRS_0586), (SSDAC_SWRS_0586)
						(SSDAC_SWRS_0275), (SSDAC_SWRS_0276), (SSDAC_SWRS_0277), (SSDAC_SWRS_0278)
						(SSDAC_SWRS_0279), (SSDAC_SWRS_0300), (SSDAC_SWRS_0214), (SSDAC_SWRS_0903)
						(SSDAC_SWRS_0510), (SSDAC_SWRS_0664), (SSDAC_SWRS_0764), (SSDAC_SWRS_0665)
						(SSDAC_SWRS_0765), (SSDAC_SWRS_0678), (SSDAC_SWRS_0778), (SSDAC_SWRS_0611)
						(SSDAC_SWRS_0710), (SSDAC_SWRS_0378), (SSDAC_SWRS_0379), (SSDAC_SWRS_0381)
						(SSDAC_SWRS_0382), (SSDAC_SWRS_0383), (SSDAC_SWRS_0390), (SSDAC_SWRS_0390)
						(SSDAC_SWRS_0309), (SSDAC_SWRS_0310), (SSDAC_SWRS_0313)

Design Requirements:	SSDAC_DR_5295

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective()
                     ERROR.C-Set_Error_Status_Bit()
                     RELAYMGR.C-Declare_DAC_Defective_US()
                     RELAYMGR.C-Declare_DAC_Defective_DS()

    Called by       :SYS_MON.C-Update_FeedBack_State()

Input Variables                     Name                                Type
    Global          :   FeedBack_Error.Status_Rly_Fb_Error              BYTE
                        FeedBack_Error.Vital_Rly_A_Fb_Error             BYTE
                        FeedBack_Error.Vital_Rly_B_Fb_Error             BYTE
                        Track_Status_Info.Bit.Remote_US_Track_Status    Unsigned
                        Track_Status_Info.Bit.Remote_DS_Track_Status    Usigned
                        Track_Status_Info.Bit.Local_US_Track_Status     Unsigned
                        Track_Status_Info.Bit.Local_DS_Track_Status     Unsigned

    Local           :   uchID                                           BYTE

Output Variables                    Name                                Type
    Global          :   FeedBack_Error.Status_Rly_Fb_Error              BYTE
                        Status.Flags.Preparatory_Relay                  Unsigned
                        FeedBack_Error.Vital_Rly_A_Fb_Error             BYTE
                        Status.Flags.US_Track_Status                    Unsigned
                        Track_Status_Info.Bit.Local_US_Track_Status     Unsigned
                        Status.Flags.Vital_Relay_A                      Unsigned
                        Status.Flags.DS_Track_Status                    Unsigned
                        Track_Status_Info.Bit.Local_DS_Track_Status     Unsigned
                        Status.Flags.Vital_Relay_B                      Unsigned


    Local           :   None

Signal Variables




Macros defined      :                   Macro               Value
                        SET_HIGH                              1
                        RELAY_OK                              1
                        MAXIMUM_FB_RETRIES                    2
                        RELAY_FAILED                          0
                        SET_LOW                               0
                        TRACK_CLEAR                           1
                        TRACK_OCCUPIED                        0
References          :

Derived Requirements:


Algorithm           :1.Check the feedback status of status relay after energizing the relay, if feedback port
                       is not set high then increment the status relay feedback error else set status relay feedback
                       error as zero & declare that status relay is OK
                     2.If the status feedback error is greater than maximum no of feedback retries(2) then declare
                       as DAC defective,status relay has failed & set the error status bit to indicate the error
                     3.Check the feedback status of status relay after de-energizing the relay, if feedback port
                       is not set low then increment the status relay feedback error else set status relay feedback
                       error as zero & declare that status relay is OK
                     4.If the status feedback error is greater than maximum no of feedback retries(2) then declare
                       as DAC defective,status relay has failed & set the error status bit to indicate the error
                     5.Check the feedback status of vital relay A after energizing the relay, if feedback port
                       is not set high then increment the vital relay A feedback error else set vital relay A feedback
                       error as zero, declare US track status as clear & declare that vital relay A is OK
                     6.If the vital relay A feedback error is greater than maximum no of feedback retries(2) then declare
                       as DAC defective at US,vital relay A has failed & set the error status bit to indicate the error
                     7.Check the feedback status of vital relay A after de-energizing the relay, if feedback port
                       is not set low then increment the vital relay A feedback error else set vital relay A feedback
                       error as zero, declare US track status as clear & declare that vital relay A is OK
                     8.If the vital relay A feedback error is greater than maximum no of feedback retries(2) then declare
                       as DAC defective at US, declare US track status as track occupied, vital relay A has failed & set
                       the error status bit to indicate the error
                     9.Check the feedback status of vital relay B after energizing the relay, if feedback port
                       is not set high then increment the vital relay B feedback error else set vital relay B feedback
                       error as zero, declare DS track status as clear & declare that vital relay B is OK
                     10.If the vital relay B feedback error is greater then maximum no of feedback retries(2) then declare
                       as DAC defective at DS,vital relay B has failed & set the error status bit to indicate the error
                     11.Check the feedback status of vital relay B after de-energizing the relay, if feedback port
                       is not set low then increment the vital relay B feedback error else set vital relay B feedback
                       error as zero, declare DS track status as clear & declare that vital relay B is OK
                     12.If the vital relay B feedback error is greater than maximum no of feedback retries(2) then declare
                       as DAC defective at DS, declare DS track status as track occupied, vital relay B has failed & set
                       the error status bit to indicate the error
                     13.If the US track status is not same as remote US track status then downside DAC failed so drop the relay,
                        declare as DAC defective & set the error bit to declare error
                     14.If the DS track status is not same as remote DS track status then downside DAC failed so drop the relay,
                        declare as DAC defective & set the error bit to declare error
*******************************************************************************/
void Read_FeedBack_Signal_Port(BYTE uchID)
{

    switch (uchID)
    {
        case FB_STATUS_RELAY_A_ON_ID:
            if (FB1_PORT_STATUS_RELAY_A != SET_HIGH ||  FB2_PORT_STATUS_RELAY_A != SET_LOW)
            {

                FeedBack_Error.Status_Rly_A_Fb_Error = FeedBack_Error.Status_Rly_A_Fb_Error + 1;
            }
            else
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
            }
            if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
             }
            else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
             }
            if(FeedBack_Error.Status_Rly_A_Fb_Error >= MAXIMUM_FB_RETRIES )
            {
                Declare_DAC_Defective_US();
                Status.Flags.Preparatory_Relay1 = RELAY_FAILED;
                Set_Error_Status_Bit(PRELIMINARY_RELAY_A_FAILURE_ERROR_NUM);
            }
            break;
        case FB_STATUS_RELAY_A_OFF_ID:
            if (FB1_PORT_STATUS_RELAY_A != SET_LOW || FB2_PORT_STATUS_RELAY_A != SET_HIGH)
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = FeedBack_Error.Status_Rly_A_Fb_Error + 1;
            }
            else
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
            }
            if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
             }
            else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D   )
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
             }
            if(FeedBack_Error.Status_Rly_A_Fb_Error >= MAXIMUM_FB_RETRIES )
            {
                Declare_DAC_Defective_US();
                Status.Flags.Preparatory_Relay1 = RELAY_FAILED;
                Set_Error_Status_Bit(PRELIMINARY_RELAY_A_FAILURE_ERROR_NUM);
            }
            break;
        case FB_STATUS_RELAY_B_ON_ID:
            if (FB1_PORT_STATUS_RELAY_B != SET_HIGH || FB2_PORT_STATUS_RELAY_B != SET_LOW)
            {
                FeedBack_Error.Status_Rly_B_Fb_Error = FeedBack_Error.Status_Rly_B_Fb_Error + 1;
            }
            else
            {
                FeedBack_Error.Status_Rly_B_Fb_Error = 0;
                Status.Flags.Preparatory_Relay = RELAY_OK;
            }
            if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
             }
            else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
            {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
             }
            if(FeedBack_Error.Status_Rly_B_Fb_Error >= MAXIMUM_FB_RETRIES )
            {
                if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL)
                {
                    Declare_DAC_Defective();
                }
                else
                {
                    Declare_DAC_Defective_DS();
                }
                Status.Flags.Preparatory_Relay = RELAY_FAILED;
                Set_Error_Status_Bit(PRELIMINARY_RELAY_B_FAILURE_ERROR_NUM);
            }
            break;
        case FB_STATUS_RELAY_B_OFF_ID:
            if (FB1_PORT_STATUS_RELAY_B != SET_LOW || FB2_PORT_STATUS_RELAY_B != SET_HIGH)
            {
                FeedBack_Error.Status_Rly_B_Fb_Error = FeedBack_Error.Status_Rly_B_Fb_Error + 1;
            }
            else
            {
                FeedBack_Error.Status_Rly_B_Fb_Error = 0;
                Status.Flags.Preparatory_Relay = RELAY_OK;
            }
            if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
             {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
             }
            else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C ||
               DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D )
             {
                FeedBack_Error.Status_Rly_A_Fb_Error = 0;
                Status.Flags.Preparatory_Relay1 = RELAY_OK;
             }
            if(FeedBack_Error.Status_Rly_B_Fb_Error >= MAXIMUM_FB_RETRIES )
            {
                if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL)
                {
                    Declare_DAC_Defective();
                }
                else
                {
                    Declare_DAC_Defective_DS();
                }
                Status.Flags.Preparatory_Relay = RELAY_FAILED;
                Set_Error_Status_Bit(PRELIMINARY_RELAY_B_FAILURE_ERROR_NUM);
            }
            break;
        case FB_VITAL_RELAY_A_ON_ID:
            if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL)
            {
                FeedBack_Error.Vital_Rly_A_Fb_Error = 0;
                Status.Flags.US_Track_Status = TRACK_CLEAR;
                Track_Status_Info.Bit.Local_US_Track_Status = TRACK_CLEAR;
                Status.Flags.Vital_Relay_A = RELAY_OK;
                break;
            }
            if (FB1_VITAL_RELAY_A_PORT != SET_HIGH || FB2_VITAL_RELAY_A_PORT != SET_LOW) // || VR1_CKFB_PORT != SET_LOW)
            {
                FeedBack_Error.Vital_Rly_A_Fb_Error = FeedBack_Error.Vital_Rly_A_Fb_Error + 1;
            }
            else
            {
                FeedBack_Error.Vital_Rly_A_Fb_Error = 0;
                Status.Flags.US_Track_Status = TRACK_CLEAR;
                Track_Status_Info.Bit.Local_US_Track_Status = TRACK_CLEAR;
                Status.Flags.Vital_Relay_A = RELAY_OK;
            }
            if(FeedBack_Error.Vital_Rly_A_Fb_Error >= MAXIMUM_FB_RETRIES)
            {
                Declare_DAC_Defective_US();
                Status.Flags.Vital_Relay_A = RELAY_FAILED;
                Set_Error_Status_Bit(CRITICAL_RELAY_A_FAILURE_ERROR_NUM);
            }
            break;
        case FB_VITAL_RELAY_A_OFF_ID:
            if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL)
            {
                FeedBack_Error.Vital_Rly_A_Fb_Error = 0;
                Status.Flags.US_Track_Status = TRACK_CLEAR;
                Track_Status_Info.Bit.Local_US_Track_Status = TRACK_CLEAR;
                Status.Flags.Vital_Relay_A = RELAY_OK;
                break;
            }

            if (FB1_VITAL_RELAY_A_PORT != SET_LOW || FB2_VITAL_RELAY_A_PORT != SET_HIGH) // || VR1_CKFB_PORT != SET_HIGH)
            {
                FeedBack_Error.Vital_Rly_A_Fb_Error = FeedBack_Error.Vital_Rly_A_Fb_Error + 1;
            }
            else
            {
                FeedBack_Error.Vital_Rly_A_Fb_Error = 0;
                Status.Flags.US_Track_Status = TRACK_OCCUPIED;
                Track_Status_Info.Bit.Local_US_Track_Status = TRACK_OCCUPIED;
                Status.Flags.Vital_Relay_A = RELAY_OK;
            }
            if(FeedBack_Error.Vital_Rly_A_Fb_Error >= MAXIMUM_FB_RETRIES )
            {
                Declare_DAC_Defective_US();
                Status.Flags.US_Track_Status = TRACK_OCCUPIED;
                Track_Status_Info.Bit.Local_US_Track_Status = TRACK_OCCUPIED;
                Status.Flags.Vital_Relay_A = RELAY_FAILED;
                Set_Error_Status_Bit(CRITICAL_RELAY_A_FAILURE_ERROR_NUM);
            }
            break;
        case FB_VITAL_RELAY_B_ON_ID:
            if (FB1_VITAL_RELAY_B_PORT != SET_HIGH || FB2_VITAL_RELAY_B_PORT != SET_LOW) // || VR2_CKFB_PORT != SET_LOW)
            {
                FeedBack_Error.Vital_Rly_B_Fb_Error = FeedBack_Error.Vital_Rly_B_Fb_Error + 1;
            }
            else
            {
                FeedBack_Error.Vital_Rly_B_Fb_Error = 0;
                Status.Flags.DS_Track_Status = TRACK_CLEAR;
                Track_Status_Info.Bit.Local_DS_Track_Status = TRACK_CLEAR;
                Status.Flags.Vital_Relay_B = RELAY_OK;
            }
            if(FeedBack_Error.Vital_Rly_B_Fb_Error >= MAXIMUM_FB_RETRIES )
            {
                if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE)
                {
                    Declare_DAC_Defective();
                }
                else
                {
                    Declare_DAC_Defective_DS();
                }
                Status.Flags.Vital_Relay_B = RELAY_FAILED;
                Set_Error_Status_Bit(CRITICAL_RELAY_B_FAILURE_ERROR_NUM);
            }
            break;
        case FB_VITAL_RELAY_B_OFF_ID:
            if (FB1_VITAL_RELAY_B_PORT != SET_LOW || FB2_VITAL_RELAY_B_PORT != SET_HIGH)// || VR2_CKFB_PORT != SET_HIGH)
            {
                FeedBack_Error.Vital_Rly_B_Fb_Error = FeedBack_Error.Vital_Rly_B_Fb_Error + 1;
            }
            else
            {
                FeedBack_Error.Vital_Rly_B_Fb_Error = 0;
                Status.Flags.DS_Track_Status = TRACK_OCCUPIED;
                Track_Status_Info.Bit.Local_DS_Track_Status = TRACK_OCCUPIED;
                Status.Flags.Vital_Relay_B = RELAY_OK;
            }
            if(FeedBack_Error.Vital_Rly_B_Fb_Error >= MAXIMUM_FB_RETRIES)
            {
                if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL  || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE)
                {
                    Declare_DAC_Defective();
                }
                else
                {
                    Declare_DAC_Defective_DS();
                }
                Status.Flags.DS_Track_Status = TRACK_OCCUPIED;
                Track_Status_Info.Bit.Local_DS_Track_Status = TRACK_OCCUPIED;
                Status.Flags.Vital_Relay_B = RELAY_FAILED;
                Set_Error_Status_Bit(CRITICAL_RELAY_B_FAILURE_ERROR_NUM);
            }
            break;
        case US_TRACK_STATUS_CHECK_ID:
            if(Track_Status_Info.Bit.Local_US_Track_Status != Track_Status_Info.Bit.Remote_US_Track_Status)
            {
                if(DIP_Switch_Info.DAC_Unit_Type != DAC_UNIT_TYPE_LCWS && DIP_Switch_Info.DAC_Unit_Type != DAC_UNIT_TYPE_LCWS_DL && DIP_Switch_Info.DAC_Unit_Type != DAC_UNIT_TYPE_DE)
                {
                    Declare_DAC_Defective_US();                 /*Downside DAC failed so drop the relay */
                    Set_Error_Status_Bit(US_ERROR_NUM);
                }
            }
            break;
        case DS_TRACK_STATUS_CHECK_ID:
            if(Track_Status_Info.Bit.Local_DS_Track_Status != Track_Status_Info.Bit.Remote_DS_Track_Status)
            {
                if(DIP_Switch_Info.DAC_Unit_Type != DAC_UNIT_TYPE_LCWS && DIP_Switch_Info.DAC_Unit_Type != DAC_UNIT_TYPE_LCWS_DL && DIP_Switch_Info.DAC_Unit_Type != DAC_UNIT_TYPE_DE)
                {
                    Declare_DAC_Defective_DS();                 /*Downside DAC failed so drop the relay */
                    Set_Error_Status_Bit(DS_ERROR_NUM);
                }
            }
            break;
        default:
            break;
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_Links(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to start the checking of communication links of each DAC unit type depending on the dip
                     switch settings ,with the remote units

Allocated Requirements:

Design Requirements:	SSDAC_DR_5296

Interfaces
    Calls           :   SYS_MON.C-Check_Communication_Links_SF1()
                        SYS_MON.C-Check_Communication_Links_SF2()
                        SYS_MON.C-Check_Communication_Links_EF1()
                        SYS_MON.C-Check_Communication_Links_EF2()
                        RLYA_MGR.C-Get_Relay_A_State()
                        RLYA_MGR.C-Get_Relay_B_State()
                        SYS_MON.C-Check_Communication_DS_CF2()
                        SYS_MON.C-Check_Communication_US_CF1()
                        SYS_MON.C-Check_Communication_US_CF2()
                        SYS_MON.C-Check_Communication_DS_CF1()

    Called by       :   SYS_MON.C-Update_Sys_Mon_State()

Input Variables                     Name                    Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type           DAC_Unit_Type
                        DIP_Switch_Info.Flags.Is_DAC_CPU1       Unsigned
                        DAC_RESET_PROGRESS                  Enumerator

    Local           :   Relay_A_State                       BYTE
                        Relay_B_State                       BYTE

Output Variables                    Name                Type
    Global          :       None

    Local           :       None
Signal Variables


Macros defined      :                   Macro               Value

References          :

Derived Requirements:

Algorithm           :1.For the unit type SF, if its CPU1 check the communication links of CPU1 else check the communication
                       links of CPU2
                     2.For the unit type EF, if it is CPU 1 check the communication links of CPU1 else check the communication
                       links of CPU2
                     3.For the unit type CF get both the vital relays (A & B) status, if relay A state is
                       not in reset state then if it is CPU1 then check the communication links of US CPU1 else check the
                       communication links of US CPU2
                        If relay B state is not in reset state then if it is CPU1 then check the communication links
                        of DS CPU1 else check the communication links of DS CPU2
                     4.For the unit type D3A, If it is CPU1 check the communication links of both US & DS CPU1 as D3A unit
                       communicate both the sides else check the communication links of both US & DS CPU2 as D3A unit communicate
                       both the side
                     5.For the unit type D3B, If it is CPU1 check the communication links of both US & DS CPU1 as D3B unit
                       communicate both the sides else check the communication links of both US & DS CPU2 as D3B unit communicate
                       both the side
                     6.For the unit type D3C, If it is CPU1 check the communication links of both US & DS CPU1 as D3C unit
                       communicate both the sides else check the communication links of both US & DS CPU2 as D3C unit communicate
                       both the side
********************************************************************************/

void Check_Communication_Links(void)
{
    BYTE Relay_VP_A_State;
    BYTE Relay_VP_B_State;


    
        switch (DIP_Switch_Info.DAC_Unit_Type)
        {
            case DAC_UNIT_TYPE_SF:
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
                {
                    Check_Communication_Links_SF1();
                }
                else
                {
                    Check_Communication_Links_SF2();
                }
                break;
            case DAC_UNIT_TYPE_EF:
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
                {
                    Check_Communication_Links_EF1();
                }
                else
                {
                    Check_Communication_Links_EF2();
                }
                break;
            case DAC_UNIT_TYPE_CF:
             case DAC_UNIT_TYPE_3D_SF:
             case DAC_UNIT_TYPE_3D_EF:
                Relay_VP_A_State = Get_Relay_A_State();
                Relay_VP_B_State = Get_Relay_B_State();
                if(Relay_VP_A_State != (BYTE)DAC_RESET_PROGRESS &&
                   Relay_VP_A_State >= (BYTE)WAIT_FOR_PILOT_TRAIN)
                {
                    if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
                    {
                     Check_Communication_US_CF1();
                    }
                    else
                    {
                     Check_Communication_US_CF2();
                    }
                }
                if(Relay_VP_B_State != (BYTE)DAC_RESET_PROGRESS &&
                   Relay_VP_B_State >= (BYTE)WAIT_FOR_PILOT_TRAIN)
                {
                    if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
                    {
                      Check_Communication_DS_CF1();
                    }
                    else
                    {
                      Check_Communication_DS_CF2();
                    }
                }
                break;
            case DAC_UNIT_TYPE_D3_A:
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
                {
                    Check_Communication_US_CF1();
                    Check_Communication_DS_CF1();
                }
                else
                {
                    Check_Communication_US_CF2();
                    Check_Communication_DS_CF2();
                }
                break;
            case DAC_UNIT_TYPE_D3_B:
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
                {
                    Check_Communication_US_CF1();
                    Check_Communication_DS_CF1();
                }
                else
                {
                    Check_Communication_US_CF2();
                    Check_Communication_DS_CF2();
                }
                break;
            case DAC_UNIT_TYPE_D3_C:
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
                {
                    Check_Communication_US_CF1();
                    Check_Communication_DS_CF1();
                }
                else
                {
                    Check_Communication_US_CF2();
                    Check_Communication_DS_CF2();
                }
                break;
            case DAC_UNIT_TYPE_D4_A:
            case DAC_UNIT_TYPE_D4_B:
            case DAC_UNIT_TYPE_D4_C:
            case DAC_UNIT_TYPE_D4_D:
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
                {
                    Check_Communication_US_CF1();
                    Check_Communication_DS_CF1();
                }
                else
                {
                    Check_Communication_US_CF2();
                    Check_Communication_DS_CF2();
                }
                break;
            default:
                break;
        }
    

}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_Links_SF1(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication links between the CPU1 of SF unit with both the down stream
                     remote unit CPU's(CPU1 & CPU2) and declare any link failure errors

Allocated Requirements: (SSDAC_SWRS_0300), (SSDAC_SWRS_0208), (SSDAC_SWRS_0906)
					  
Design Requirements:	SSDAC_DR_5297

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective()
                     ERROR.C-Set_Error_Status_Byte()

    Called by       :SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                        Type
    Global          :
                        Comm_B_CountDown.DS1_to_LU1             BYTE
                        Comm_B_CountDown.DS2_to_LU1             BYTE
                        Status.Flags.LU1_to_DS1_Link            Bit
                        Status.Flags.LU1_to_DS2_Link            Bit
                        Status.Flags.LU2_to_DS1_Link            Bit
                        Status.Flags.LU2_to_DS2_Link            Bit

    Local           :   uchFailure                              BYTE
                        Status.Byte[3]                          Bit
                        Status.Byte[4]                          Bit

Output Variables                    Name                        Type
    Global          :   Status.Flags.DS1_to_LU1_Link            Bit
                        Status.Flags.DS2_to_LU1_Link            Bit

    Local           :   None

Signal Variables


Macros defined      :                   Macro               Value
                                        TRUE                 1
                                        FALSE                0
                                    TIMEOUT_EVENT            0
                                    COMMUNICATION_FAILED     0
                                    COMM1_LINK_FAIL_ID       1
                                    COMM2_LINK_FAIL_ID       2

References          :
Derived Requirements:
Algorithm           :1.If the Start fed CPU1 doesn't receive proper message more than 3 times from the downstream CPU1
                        then set the downstream CPU1 communication flag as FAILED
                     2.If the Start fed CPU1 doesn't receive proper message more than 3 times from the downstream CPU2
                        then set the downstream CPU2 communication flag as FAILED
                     3.If the commmunication has failed between Start fed CPU1 to Downstream CPU1 or
                        Start fed CPU1 to Downstream CPU2 or Start fed CPU2 to Downstream CPU1 or
                        Start fed CPU2 to Downstream CPU2 then declare DAC defective and set the error status byte
                        "COMM1_LINK_FAIL_ID" or "COMM2_LINK_FAIL_ID"
*******************************************************************************/
void Check_Communication_Links_SF1(void)
{
    BYTE uchFailure = FALSE;
    BYTE uchTrack;
    if(DS_Section_Mode.Local_Unit == SYSTEM_OCCUPIED_MODE)
    {
        if (Status.Flags.LU1_to_DS1_Link == COMMUNICATION_FAILED ||
            Status.Flags.LU1_to_DS2_Link == COMMUNICATION_FAILED ||
            Status.Flags.LU2_to_DS1_Link == COMMUNICATION_FAILED ||
            Status.Flags.LU2_to_DS2_Link == COMMUNICATION_FAILED ||
            Comm_B_CountDown.DS1_to_LU1 == TIMEOUT_EVENT ||
            Comm_B_CountDown.DS2_to_LU1 == TIMEOUT_EVENT
            )
        {
            Comm_B_CountDown.DS1_to_LU1 = MAXIMUM_COM_RETRIES;//re-assign so that comm does not fail constantly
            Comm_B_CountDown.DS2_to_LU1 = MAXIMUM_COM_RETRIES;
            //Comm has failed
            //if Comm has failed for 1st time in occupied state, start the countdown for 2 mins
            if(comm_check_SF1.State == COMM_GOOD )
            {
                comm_check_SF1.State = COMM_FAILED;
                comm_check_SF1.Wait_timeout = COMM_ERROR_WAIT_TIME;
                //Save local count values
                if(Get_DS_AxleDirection() == (BYTE)REVERSE_DIRECTION)
                {
                    //Track only if train is entering from reverse direction	(EF to SF)
                    comm_check_SF1.Local_counts[0] = (BYTE)Get_DS_Rev_AxleCount();
                    comm_check_SF1.Local_counts[1] = (BYTE)(Get_DS_Rev_AxleCount()>>8);
                }
                return; /*do not declare any error. Wait for timeout*/
            }
            else
            {
                if(comm_check_SF1.Wait_timeout != 0)
                {
                    //Check for local count values
                    if(Get_DS_AxleDirection() == (BYTE)REVERSE_DIRECTION)
                    {
                        comm_check_SF1.Wheel_match_fail = 0;
                        if(comm_check_SF1.Local_counts[0] != (BYTE)Get_DS_Rev_AxleCount())
                            comm_check_SF1.Wheel_match_fail = 1;
                        if(comm_check_SF1.Local_counts[1] != (BYTE)(Get_DS_Rev_AxleCount()>>8))
                            comm_check_SF1.Wheel_match_fail = 1;
                        if(comm_check_SF1.Wheel_match_fail)
                        {
                            // wheel has mismatched. declare direct out count and comm failures!
                            Status.Flags.DS1_to_LU1_Link = COMMUNICATION_FAILED;
                            Status.Flags.DS2_to_LU1_Link = COMMUNICATION_FAILED;
                            Declare_DAC_Defective();
                            Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
                            Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);

                            Status.Flags.Direct_Out_Count = SET_HIGH;
                            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                        }
                    }
                    return; /*do not declare any error. Wait for timeout*/
                }
                //Communication has not been restored
                // proceed in declaring the error.
                Comm_B_CountDown.DS1_to_LU1 = TIMEOUT_EVENT;
                Comm_B_CountDown.DS2_to_LU1 = TIMEOUT_EVENT;
            }
            //2mins countdown has elapsed, declare the error
        }
        else
        {
            //No Comm error
            //if communication has been restored from failure state, check for the count
            if (Com2RecvObject.State != (BYTE)COM_VALID_MESSAGE)
                return;
            if(comm_check_SF1.State == COMM_FAILED)
            {
                //if the wheel counts are unchanged, no error.
                for(uchTrack = 0;uchTrack<10;uchTrack++)
                    if(comm_check_SF1.Track_counts[uchTrack] != Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET + uchTrack])
                        comm_check_SF1.Wheel_match_fail = 1;
                if(comm_check_SF1.Wheel_match_fail)
                {
                    // wheel has mismatched. declare direct out count and comm failures!
                    Status.Flags.DS1_to_LU1_Link = COMMUNICATION_FAILED;
                    Status.Flags.DS2_to_LU1_Link = COMMUNICATION_FAILED;
                    Declare_DAC_Defective();
                    Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
                    Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
                    
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    
                    comm_check_SF1.State = COMM_GOOD;
                    return;
                }    
                //count matches, do not declare error
            }   
            comm_check_SF1.State = COMM_GOOD;
        }
    }
    else if(DS_Section_Mode.Local_Unit == SYSTEM_CLEAR_MODE)
    {
        comm_check_SF1.State = COMM_GOOD;
        comm_check_SF1.Wait_timeout = 0;
    }
    if (Comm_B_CountDown.DS1_to_LU1 == TIMEOUT_EVENT)
    {
        Status.Flags.DS1_to_LU1_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Comm_B_CountDown.DS2_to_LU1 == TIMEOUT_EVENT)
    {
        Status.Flags.DS2_to_LU1_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Status.Flags.LU1_to_DS1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_DS2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_DS1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_DS2_Link == COMMUNICATION_FAILED)
    {
        uchFailure = TRUE;
    }
    if (uchFailure)
    {
        Declare_DAC_Defective();
        Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
        Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_Links_SF2(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication links between the CPU2 of SF unit with both the down stream
                     remote CPU's(CPU1 & CPU2)and declare any link failure erors


Allocated Requirements:(SSDAC_SWRS_0300),(SSDAC_SWRS_0208), (SSDAC_SWRS_0906)
					  
Design Requirements:	SSDAC_DR_5298

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective()
                     ERROR.C-Set_Error_Status_Byte()


    Called by       :SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                    Type
    Global          :   Comm_B_CountDown.DS1_to_LU2         Byte
                        Comm_B_CountDown.DS2_to_LU2         Byte
                        Status.Flags.LU2_to_DS1_Link        Bit
                        Status.Flags.LU2_to_DS2_Link        Bit
                        Status.Flags.LU1_to_DS1_Link        Bit
                        Status.Flags.LU1_to_DS2_Link        Bit

    Local           :   uchFailure                          BYTE
                        Status.Byte[3]                      Bit
                        Status.Byte[4]                      Bit

Output Variables                    Name                    Type
    Global          :   Status.Flags.DS1_to_LU2_Link        Bit
                        Status.Flags.DS2_to_LU2_Link        Bit

    Local           :   None

Signal Variables


Macros defined      :                   Macro               Value
                                    TIMEOUT_EVENT             0
                                    TRUE                      1
                                    FALSE                     0
                                    COMMUNICATION_FAILED      0
                                    COMM1_LINK_FAIL_ID        1
                                    COMM2_LINK_FAIL_ID        2

References          :
Derived Requirements:
Algorithm           :1.If the Start fed CPU2 doesn't receive proper message more than 3 times from the downstream CPU1
                        then set the downstream CPU1 communication flag as FAILED
                     2.If the Start fed CPU2 doesn't receive proper message more than 3 times from the downstream CPU2
                        then set the downstream CPU2 communication flag as FAILED
                     3.If the commmunication has failed between Start fed CPU2 to Downstream CPU1 or
                        Start fed CPU2 to Downstream CPU2 or Start fed CPU1 to Downstream CPU1 or
                        Start fed CPU1 to Downstream CPU2 then declare DAC defective and set the error status byte
                        "COMM1_LINK_FAIL_ID" or "COMM2_LINK_FAIL_ID"
*******************************************************************************/
void Check_Communication_Links_SF2(void)
{
    BYTE uchFailure = FALSE;
    BYTE uchTrack;
    
        if(DS_Section_Mode.Local_Unit == SYSTEM_OCCUPIED_MODE)
    {
        if (Status.Flags.LU2_to_DS1_Link == COMMUNICATION_FAILED ||
            Status.Flags.LU2_to_DS2_Link == COMMUNICATION_FAILED ||
            Status.Flags.LU1_to_DS1_Link == COMMUNICATION_FAILED ||
            Status.Flags.LU1_to_DS2_Link == COMMUNICATION_FAILED ||
            Comm_B_CountDown.DS1_to_LU2 == TIMEOUT_EVENT ||
            Comm_B_CountDown.DS2_to_LU2 == TIMEOUT_EVENT
            )
        {
            Comm_B_CountDown.DS1_to_LU2 = MAXIMUM_COM_RETRIES;//re-assign so that comm does not fail constantly
            Comm_B_CountDown.DS2_to_LU2 = MAXIMUM_COM_RETRIES;
            //Comm has failed
            //if Comm has failed for 1st time in occupied state, start the countdown for 2 mins
            if(comm_check_SF2.State == COMM_GOOD )
            {
                comm_check_SF2.State = COMM_FAILED;
                comm_check_SF2.Wait_timeout = COMM_ERROR_WAIT_TIME;
                //Save local count values
                if(Get_DS_AxleDirection() == (BYTE)REVERSE_DIRECTION)
                {
                    //Track only if train is entering from reverse direction	(EF to SF)
                    comm_check_SF2.Local_counts[0] = (BYTE)Get_DS_Rev_AxleCount();
                    comm_check_SF2.Local_counts[1] = (BYTE)(Get_DS_Rev_AxleCount()>>8);
                }
                return; /*do not declare any error. Wait for timeout*/
            }
            else
            {
                if(comm_check_SF2.Wait_timeout != 0)
                {
                    //Check for local count values
                    if(Get_DS_AxleDirection() == (BYTE)REVERSE_DIRECTION)
                    {
                        comm_check_SF2.Wheel_match_fail = 0;
                        if(comm_check_SF2.Local_counts[0] != (BYTE)Get_DS_Rev_AxleCount())
                            comm_check_SF2.Wheel_match_fail = 1;
                        if(comm_check_SF2.Local_counts[1] != (BYTE)(Get_DS_Rev_AxleCount()>>8))
                            comm_check_SF2.Wheel_match_fail = 1;
                        if(comm_check_SF2.Wheel_match_fail)
                        {
                            // wheel has mismatched. declare direct out count and comm failures!
                            Status.Flags.DS1_to_LU2_Link = COMMUNICATION_FAILED;
                            Status.Flags.DS2_to_LU2_Link = COMMUNICATION_FAILED;
                            Declare_DAC_Defective();
                            Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
                            Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);

                            Status.Flags.Direct_Out_Count = SET_HIGH;
                            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                        }
                    }
                    return; /*do not declare any error. Wait for timeout*/
                }
                Comm_B_CountDown.DS1_to_LU2 = TIMEOUT_EVENT;
                Comm_B_CountDown.DS2_to_LU2 = TIMEOUT_EVENT;
                //Communication has not been restored
                // proceed in declaring the error.
                
            }
            //2mins countdown has elapsed, declare the error
        }
        else
        {
            //No Comm error
            //if communication has been restored from failure state, check for the count
            if (Com2RecvObject.State != (BYTE)COM_VALID_MESSAGE)
                return;
            if(comm_check_SF2.State == COMM_FAILED)
            {
                //if the wheel counts are unchanged, no error.
                for(uchTrack = 0;uchTrack<10;uchTrack++)
                    if(comm_check_SF2.Track_counts[uchTrack] != Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET + uchTrack])
                        comm_check_SF2.Wheel_match_fail = 1;
                if(comm_check_SF2.Wheel_match_fail)
                {
                    // wheel has mismatched. declare direct out count and comm failures!
                    Status.Flags.DS1_to_LU2_Link = COMMUNICATION_FAILED;
                    Status.Flags.DS2_to_LU2_Link = COMMUNICATION_FAILED;
                    Declare_DAC_Defective();
                    Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
                    Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
                    
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    
                    comm_check_SF2.State = COMM_GOOD;
                    return;
                }    
                //count matches, do not declare error
            }   
            comm_check_SF2.State = COMM_GOOD;
        }
    }
    else if(DS_Section_Mode.Local_Unit == SYSTEM_CLEAR_MODE)
    {
        comm_check_SF2.State = COMM_GOOD;
        comm_check_SF2.Wait_timeout = 0;
    }

    if (Comm_B_CountDown.DS1_to_LU2 == TIMEOUT_EVENT)
    {
        Status.Flags.DS1_to_LU2_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Comm_B_CountDown.DS2_to_LU2 == TIMEOUT_EVENT)
    {
        Status.Flags.DS2_to_LU2_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Status.Flags.LU2_to_DS1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_DS2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_DS1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_DS2_Link == COMMUNICATION_FAILED)
    {
        uchFailure = TRUE;
    }
    if (uchFailure)
    {
        Declare_DAC_Defective();
        Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
        Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_US_CF1(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication links between the CPU1 of CF unit with both the up stream
                     remote unit CPU's(CPU1 & CPU2) and declare any link failure erors

Allocated Requirements:(SSDAC_SWRS_0586), (SSDAC_SWRS_0509),(SSDAC_SWRS_0592)
						(SSDAC_SWRS_0678), (SSDAC_SWRS_0778), (SSDAC_SWRS_0610), (SSDAC_SWRS_0709)
						(SSDAC_SWRS_0684),(SSDAC_SWRS_0784), (SSDAC_SWRS_0390), (SSDAC_SWRS_0308)
						(SSDAC_SWRS_0396)

Design Requirements:	SSDAC_DR_5299

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective_US()
                     ERROR.C-Set_Error_Status_Byte()

    Called by       :SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                    Type
    Global          :   Comm_A_CountDown.US1_to_LU1         Byte
                        Comm_A_CountDown.US2_to_LU1         Byte
                        Status.Flags.LU1_to_US1_Link        Bit
                        Status.Flags.LU1_to_US2_Link        Bit
                        Status.Flags.LU2_to_US1_Link        Bit
                        Status.Flags.LU2_to_US2_Link        Bit

    Local           :   uchFailure                          BYTE
                        Status.Byte[3]                      Bit
                        Status.Byte[4]                      Bit

Output Variables                    Name                    Type
    Global          :   Status.Flags.US1_to_LU1_Link        Bit
                        Status.Flags.US2_to_LU1_Link        Bit

    Local           :   None
Signal Variables


Macros defined      :                   Macro               Value
                                    TIMEOUT_EVENT             0
                                    TRUE                      1
                                    FALSE                     0
                                    COMMUNICATION_FAILED      0
                                    COMM1_LINK_FAIL_ID        1
                                    COMM2_LINK_FAIL_ID        2
References          :
Derived Requirements:
Algorithm           :1.If the Centre fed CPU1 doesn't receive proper message more than 3 times from the upstream CPU1
                        then set the upstream CPU1 communication flag as FAILED
                     2.If the Centre fed CPU1 doesn't receive proper message more than 3 times from the upstream CPU2
                        then set the upstream CPU2 communication flag as FAILED
                     3.If the commmunication has failed between centre fed CPU1 to Upstream CPU1 or
                        Centre fed CPU1 to Upstream CPU2 or centre fed CPU2 to Upstream CPU1 or
                        Centre fed CPU2 to Upstream CPU2 then declare DAC defective and set the error status byte
                        "COMM1_LINK_FAIL_ID" or "COMM2_LINK_FAIL_ID"
*******************************************************************************/
void Check_Communication_US_CF1(void)
{
    BYTE uchFailure = FALSE;

    if (Comm_A_CountDown.US1_to_LU1 == TIMEOUT_EVENT)
    {
        Status.Flags.US1_to_LU1_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Comm_A_CountDown.US2_to_LU1 == TIMEOUT_EVENT)
    {
        Status.Flags.US2_to_LU1_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Status.Flags.LU1_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US2_Link == COMMUNICATION_FAILED )
    {
        uchFailure = TRUE;
    }
    if (uchFailure)
    {
        Declare_DAC_Defective_US();
        Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
        Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_US_CF2(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication links between the CPU2 of CF unit with both the up stream
                     remote unit CPU's(CPU1 & CPU2) and declare any link failure erors


Allocated Requirements:(SSDAC_SWRS_0586),(SSDAC_SWRS_0509),(SSDAC_SWRS_0592)
							(SSDAC_SWRS_0678), (SSDAC_SWRS_0778)
							(SSDAC_SWRS_0610), (SSDAC_SWRS_0709),(SSDAC_SWRS_0308)
							(SSDAC_SWRS_0684),(SSDAC_SWRS_0784), (SSDAC_SWRS_0390), (SSDAC_SWRS_0396)

Design Requirements:	SSDAC_DR_5300

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective_US()
                     ERROR.C-Set_Error_Status_Byte()

    Called by       :SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                    Type
    Global          :   Comm_A_CountDown.US1_to_LU2         Byte
                        Comm_A_CountDown.US2_to_LU2         Byte
                        Status.Flags.LU2_to_US1_Link        Bit
                        Status.Flags.LU2_to_US2_Link        Bit
                        Status.Flags.LU1_to_US1_Link        Bit
                        Status.Flags.LU1_to_US2_Link        Bit

    Local           :   uchFailure                          BYTE
                        Status.Byte[4]                      Bit
                        Status.Byte[3]                      Bit

Output Variables                    Name                    Type
    Global          :   Status.Flags.US1_to_LU2_Link        Bit
                        Status.Flags.US2_to_LU2_Link        Bit

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        FALSE                    0
                                        TIMEOUT_EVENT            0
                                        COMMUNICATION_FAILED     0
                                        TRUE                     1
                                        COMM1_LINK_FAIL_ID       1
                                        COMM1_LINK_FAIL_ID       2

References          :

Derived Requirements:
Algorithm           :1.If the Centre fed CPU2 doesn't receive proper message more than 3 times from the upstream CPU1
                        then set the upstream CPU1 communication flag as FAILED
                     2.If the Centre fed CPU2 doesn't receive proper message more than 3 times from the upstream CPU2
                        then set the upstream CPU2 communication flag as FAILED
                     3.If the commmunication has failed between centre fed CPU2 to Upstream CPU1 or
                        Centre fed CPU2 to Upstream CPU2 or centre fed CPU1 to Upstream CPU1 or
                        Centre fed CPU1 to Upstream CPU2 then declare DAC defective and set the error status byte
                        "COMM1_LINK_FAIL_ID" or "COMM2_LINK_FAIL_ID"
*******************************************************************************/
void Check_Communication_US_CF2(void)
{
    BYTE uchFailure = FALSE;

    if (Comm_A_CountDown.US1_to_LU2 == TIMEOUT_EVENT)
    {
        Status.Flags.US1_to_LU2_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Comm_A_CountDown.US2_to_LU2 == TIMEOUT_EVENT)
    {
        Status.Flags.US2_to_LU2_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Status.Flags.LU2_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US2_Link == COMMUNICATION_FAILED)
    {
        uchFailure = TRUE;
    }
    if (uchFailure)
    {
        Declare_DAC_Defective_US();
        Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
        Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_DS_CF1(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication links between the CPU1 of CF unit with both the down stream
                     remote unit CPU's(CPU1 & CPU2) and declare any link failure erors

Allocated Requirements:(SSDAC_SWRS_0586),(SSDAC_SWRS_0509),(SSDAC_SWRS_0592)
						(SSDAC_SWRS_0678), (SSDAC_SWRS_0778), 
						(SSDAC_SWRS_0610), (SSDAC_SWRS_0709),(SSDAC_SWRS_0308)
						(SSDAC_SWRS_0684),(SSDAC_SWRS_0784), (SSDAC_SWRS_0390)
						(SSDAC_SWRS_0396)

Design Requirements:	SSDAC_DR_5301

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective_DS()
                     ERROR.C-Set_Error_Status_Byte()

    Called by       :SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                Type
    Global          :   Comm_B_CountDown.DS1_to_LU1         Byte
                        Comm_B_CountDown.DS2_to_LU1         Byte
                        Status.Flags.LU1_to_DS1_Link        Bit
                        Status.Flags.LU1_to_DS2_Link        Bit
                        Status.Flags.LU2_to_DS1_Link        Bit
                        Status.Flags.LU2_to_DS2_Link        Bit

    Local           :   uchFailure                          BYTE
                        Status.Byte[4]                      Bit
                        Status.Byte[3]                      Bit

Output Variables                    Name                    Type
    Global          :   Status.Flags.US1_to_LU2_Link        Bit
                        Status.Flags.US2_to_LU2_Link        Bit

    Local           :   None
Signal Variables



Macros defined      :                   Macro               Value
                                        FALSE                    0
                                        TIMEOUT_EVENT            0
                                        COMMUNICATION_FAILED     0
                                        TRUE                     1
                                        COMM1_LINK_FAIL_ID       1
                                        COMM1_LINK_FAIL_ID       2
References          :

Derived Requirements:

Algorithm           :1.If the Centre fed CPU1 doesn't receive proper message more than 3 times from the Downstream CPU1
                        then set the Downstream CPU1 communication flag as FAILED
                     2.If the Centre fed CPU1 doesn't receive proper message more than 3 times from the Downstream CPU2
                        then set the Downstream CPU2 communication flag as FAILED
                     3.If the commmunication has failed between centre fed CPU1 to Downstream CPU1 or
                        Centre fed CPU1 to Downstream CPU2 or centre fed CPU2 to Downstream CPU1 or
                        Centre fed CPU2 to Downstream CPU2 then declare DAC defective and set the error status byte
                        "COMM1_LINK_FAIL_ID" or "COMM2_LINK_FAIL_ID"
*******************************************************************************/
void Check_Communication_DS_CF1(void)
{
    BYTE uchFailure = FALSE;

    if (Comm_B_CountDown.DS1_to_LU1 == TIMEOUT_EVENT)
    {
        Status.Flags.DS1_to_LU1_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Comm_B_CountDown.DS2_to_LU1 == TIMEOUT_EVENT)
    {
        Status.Flags.DS2_to_LU1_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Status.Flags.LU1_to_DS1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_DS2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_DS1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_DS2_Link == COMMUNICATION_FAILED )
    {
        uchFailure = TRUE;
    }
    if (uchFailure)
    {
        Declare_DAC_Defective_DS();
        Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
        Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_DS_CF2(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication links between the CPU2 of CF unit with both the down stream
                     remote unit CPU's(CPU1 & CPU2) and declare any link failure errors

Allocated Requirements:(SSDAC_SWRS_0586),(SSDAC_SWRS_0509),(SSDAC_SWRS_0592)
						(SSDAC_SWRS_0678), (SSDAC_SWRS_0778)
						(SSDAC_SWRS_0610), (SSDAC_SWRS_0709), (SSDAC_SWRS_0308)
						(SSDAC_SWRS_0684),(SSDAC_SWRS_0784), (SSDAC_SWRS_0390)
						(SSDAC_SWRS_0396)

Design Requirements:	SSDAC_DR_5302

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective_DS()
                     ERROR.C-Set_Error_Status_Byte()

    Called by       :SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                Type
    Global          :   Comm_B_CountDown.DS1_to_LU2         Byte
                        Comm_B_CountDown.DS2_to_LU1         Byte
                        Status.Flags.LU2_to_DS1_Link        Bit
                        Status.Flags.LU2_to_DS2_Link        Bit
                        Status.Flags.LU1_to_DS1_Link        Bit
                        Status.Flags.LU1_to_DS2_Link        Bit

    Local           :   uchFailure                          BYTE
                        Status.Byte[4]                      Bit
                        Status.Byte[3]                      Bit

Output Variables                    Name                Type
    Global          :   Status.Flags.DS1_to_LU1_Link        Bit
                        Status.Flags.DS2_to_LU2_Link        Bit

    Local           :   None
Signal Variables


Macros defined      :                   Macro               Value
                                        FALSE                    0
                                        TIMEOUT_EVENT            0
                                        COMMUNICATION_FAILED     0
                                        TRUE                     1
                                        COMM1_LINK_FAIL_ID       1
                                        COMM1_LINK_FAIL_ID       2
References          :

Derived Requirements:

Algorithm           :1.If the Centre fed CPU2 doesn't receive proper message more than 3 times from the Downstream CPU1
                        then set the Downstream CPU1 communication flag as FAILED
                     2.If the Centre fed CPU2 doesn't receive proper message more than 3 times from the Downstream CPU2
                        then set the Downstream CPU2 communication flag as FAILED
                     3.If the commmunication has failed between centre fed CPU2 to Downstream CPU1 or
                        Centre fed CPU2 to Downstream CPU2 or centre fed CPU1 to Downstream CPU1 or
                        Centre fed CPU1 to Downstream CPU2 then declare DAC defective and set the error status byte
                        "COMM1_LINK_FAIL_ID" or "COMM2_LINK_FAIL_ID"
*******************************************************************************/
void Check_Communication_DS_CF2(void)
{
    BYTE uchFailure = FALSE;

    if (Comm_B_CountDown.DS1_to_LU2 == TIMEOUT_EVENT)
    {
        Status.Flags.DS1_to_LU2_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Comm_B_CountDown.DS2_to_LU2 == TIMEOUT_EVENT)
    {
        Status.Flags.DS2_to_LU2_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Status.Flags.LU2_to_DS1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_DS2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_DS1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_DS2_Link == COMMUNICATION_FAILED )
    {
        uchFailure = TRUE;
    }
    if (uchFailure)
    {
        Declare_DAC_Defective_DS();
        Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
        Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_Links_EF1(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication links between the CPU1 of EF unit with both the up stream
                     remote unit CPU's(CPU1 & CPU2) and declare any link failure erors


Allocated Requirements:(SSDAC_SWRS_0486), (SSDAC_SWRS_0409), (SSDAC_SWRS_0415)

Design Requirements:	SSDAC_DR_5303

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective()
                     ERROR.C-Set_Error_Status_Byte()

    Called by       :SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                    Type
    Global          :   Comm_A_CountDown.US1_to_LU1         Byte
                        Comm_A_CountDown.US2_to_LU1         Byte
                        Status.Flags.LU1_to_US1_Link        Bit
                        Status.Flags.LU1_to_US2_Link        Bit
                        Status.Flags.LU2_to_US1_Link        Bit
                        Status.Flags.LU2_to_US2_Link        Bit

    Local           :   uchFailure                          BYTE
                        Status.Byte[4]                      Bit
                        Status.Byte[3]                      Bit

Output Variables                    Name                Type
    Global          :   Status.Flags.US1_to_LU1_Link        Bit
                        Status.Flags.US2_to_LU1_Link        Bit


    Local           :   None

Signal Variables



Macros defined      :                   Macro               Value
                                        FALSE                    0
                                        TIMEOUT_EVENT            0
                                        COMMUNICATION_FAILED     0
                                        TRUE                     1
                                        COMM1_LINK_FAIL_ID       1
                                        COMM1_LINK_FAIL_ID       2
References          :

Derived Requirements:


Algorithm           :1.If the End fed CPU1 doesn't receive proper message more than 3 times from the upstream CPU1
                        then set the upstream CPU1 communication flag as FAILED
                     2.If the End fed CPU1 doesn't receive proper message more than 3 times from the upstream CPU2
                        then set the upstream CPU2 communication flag as FAILED
                     3.If the commmunication has failed between End fed CPU1 to Upstream CPU1 or
                        End fed CPU1 to Upstream CPU2 or End fed CPU2 to Upstream CPU1 or
                        End fed CPU2 to Upstream CPU2 then declare DAC defective and set the error status byte
                        "COMM1_LINK_FAIL_ID" or "COMM2_LINK_FAIL_ID"
*******************************************************************************/
void Check_Communication_Links_EF1(void)
{
    BYTE uchFailure = FALSE;
    BYTE uchTrack;
    if(US_Section_Mode.Local_Unit == SYSTEM_OCCUPIED_MODE)
    {
        if (Status.Flags.LU1_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US2_Link == COMMUNICATION_FAILED ||
            Comm_A_CountDown.US1_to_LU1 == TIMEOUT_EVENT ||
            Comm_A_CountDown.US2_to_LU1 == TIMEOUT_EVENT
            )
        {
            Comm_A_CountDown.US1_to_LU1 = MAXIMUM_COM_RETRIES;//re-assign so that comm does not fail constantly
            Comm_A_CountDown.US2_to_LU1 = MAXIMUM_COM_RETRIES;
            //Comm has failed
            //if Comm has failed for 1st time in occupied state, start the countdown for 2 mins
            if(comm_check_EF1.State == COMM_GOOD )
            {
                comm_check_EF1.State = COMM_FAILED;
                comm_check_EF1.Wait_timeout = COMM_ERROR_WAIT_TIME;
                //Save local count values
                if(Get_US_AxleDirection() == (BYTE)FORWARD_DIRECTION)
                {
                    //Track only if train is entering from reverse direction	(EF to SF)
                    comm_check_EF1.Local_counts[0] = (BYTE)Get_US_Fwd_AxleCount();
                    comm_check_EF1.Local_counts[1] = (BYTE)(Get_US_Fwd_AxleCount()>>8);
                }
                return; /*do not declare any error. Wait for timeout*/
            }
            else
            {
                if(comm_check_EF1.Wait_timeout != 0)
                {
                    //Check for local count values
                    if(Get_US_AxleDirection() == (BYTE)FORWARD_DIRECTION)
                    {
                        comm_check_EF1.Wheel_match_fail = 0;
                        if(comm_check_EF1.Local_counts[0] != (BYTE)Get_US_Fwd_AxleCount())
                            comm_check_EF1.Wheel_match_fail = 1;
                        if(comm_check_EF1.Local_counts[1] != (BYTE)(Get_US_Fwd_AxleCount()>>8))
                            comm_check_EF1.Wheel_match_fail = 1;
                        if(comm_check_EF1.Wheel_match_fail)
                        {
                            // wheel has mismatched. declare direct out count and comm failures!
                            Status.Flags.US1_to_LU1_Link = COMMUNICATION_FAILED;
                            Status.Flags.US2_to_LU1_Link = COMMUNICATION_FAILED;
                            Declare_DAC_Defective();
                            Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
                            Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);

                            Status.Flags.Direct_Out_Count = SET_HIGH;
                            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                        }
                    }
                    return; /*do not declare any error. Wait for timeout*/
                }
                Comm_A_CountDown.US1_to_LU1 = TIMEOUT_EVENT;
                Comm_A_CountDown.US2_to_LU1 = TIMEOUT_EVENT;
                //Communication has not been restored
                // proceed in declaring the error.
                
            }
            //2mins countdown has elapsed, declare the error
        }
        else
        {
            //No Comm error
            //if communication has been restored from failure state, check for the count
            if (Com1RecvObject.State != (BYTE)COM_VALID_MESSAGE)
                return;
            if(comm_check_EF1.State == COMM_FAILED)
            {
                //if the wheel counts are unchanged, no error.
                for(uchTrack = 0;uchTrack<10;uchTrack++)
                    if(comm_check_EF1.Track_counts[uchTrack] != Com1RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET + uchTrack])
                        comm_check_EF1.Wheel_match_fail = 1;
                if(comm_check_EF1.Wheel_match_fail)
                {
                    // wheel has mismatched. declare direct out count and comm failures!
                    Status.Flags.US1_to_LU1_Link = COMMUNICATION_FAILED;
                    Status.Flags.US2_to_LU1_Link = COMMUNICATION_FAILED;
                    Declare_DAC_Defective();
                    Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
                    Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
                    
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    
                    comm_check_EF1.State = COMM_GOOD;
                    return;
                }    
                //count matches, do not declare error
            }   
            comm_check_EF1.State = COMM_GOOD;
        }
    }
    else if(US_Section_Mode.Local_Unit == SYSTEM_CLEAR_MODE)
    {
        comm_check_EF1.State = COMM_GOOD;
        comm_check_EF1.Wait_timeout = 0;
    }
    if (Comm_A_CountDown.US1_to_LU1 == TIMEOUT_EVENT)
    {
        Status.Flags.US1_to_LU1_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Comm_A_CountDown.US2_to_LU1 == TIMEOUT_EVENT)
    {
        Status.Flags.US2_to_LU1_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Status.Flags.LU1_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US2_Link == COMMUNICATION_FAILED)
    {
        uchFailure = TRUE;
    }
    if (uchFailure)
    {
        Declare_DAC_Defective();
        Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
        Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
    }
}

//21_12_2009


/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Communication_Links_EF2(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication links between the CPU2 of EF unit with both the up stream
                     remote unit CPU's(CPU1 & CPU2) and declare any link failure erors

Allocated Requirements:(SSDAC_SWRS_0486), (SSDAC_SWRS_0409)

Design Requirements:	SSDAC_DR_5304

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective()
                     ERROR.C-Set_Error_Status_Byte()

    Called by       :SYS_MON.C-Check_Communication_Links()

Input Variables                     Name                Type
    Global          :   Comm_A_CountDown.US1_to_LU2     Byte
                        Comm_A_CountDown.US2_to_LU2     Byte
                        Status.Flags.LU2_to_US1_Link    Bit
                        Status.Flags.LU2_to_US2_Link    Bit
                        Status.Flags.LU1_to_US1_Link    Bit
                        Status.Flags.LU1_to_US2_Link    Bit

    Local           :   uchFailure                          BYTE
                        Status.Byte[4]                      Bit
                        Status.Byte[3]                      Bit

Output Variables                    Name                Type
    Global          :   Status.Flags.US1_to_LU2_Link        Bit
                        Status.Flags.US2_to_LU2_Link        Bit

    Local           :   None

Signal Variables



Macros defined      :                   Macro               Value
                                        FALSE                    0
                                        TIMEOUT_EVENT            0
                                        COMMUNICATION_FAILED     0
                                        TRUE                     1
                                        COMM1_LINK_FAIL_ID       1
                                        COMM1_LINK_FAIL_ID       2
References          :

Derived Requirements:

Algorithm           :1.If the End fed CPU2 doesn't receive proper message more than 3 times from the upstream CPU1
                        then set the upstream CPU1 communication flag as FAILED
                     2.If the End fed CPU2 doesn't receive proper message more than 3 times from the upstream CPU2
                        then set the upstream CPU2 communication flag as FAILED
                     3.If the commmunication has failed between End fed CPU1 to Upstream CPU1 or
                        End fed CPU1 to Upstream CPU2 or End fed CPU2 to Upstream CPU1 or
                        End fed CPU2 to Upstream CPU2 then declare DAC defective and set the error status byte
                        "COMM1_LINK_FAIL_ID" or "COMM2_LINK_FAIL_ID"
*******************************************************************************/
void Check_Communication_Links_EF2(void)
{
    BYTE uchFailure = FALSE;
    BYTE uchTrack;
    if(US_Section_Mode.Local_Unit == SYSTEM_OCCUPIED_MODE)
    {
        if (Status.Flags.LU2_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US2_Link == COMMUNICATION_FAILED ||
            Comm_A_CountDown.US1_to_LU2 == TIMEOUT_EVENT ||
            Comm_A_CountDown.US2_to_LU2 == TIMEOUT_EVENT
            )
        {
            Comm_A_CountDown.US1_to_LU2 = MAXIMUM_COM_RETRIES;//re-assign so that comm does not fail constantly
            Comm_A_CountDown.US2_to_LU2 = MAXIMUM_COM_RETRIES;
            //Comm has failed
            //if Comm has failed for 1st time in occupied state, start the countdown for 2 mins
            if(comm_check_EF2.State == COMM_GOOD )
            {
                comm_check_EF2.State = COMM_FAILED;
                comm_check_EF2.Wait_timeout = COMM_ERROR_WAIT_TIME;
                //Save local count values
                if(Get_US_AxleDirection() == (BYTE)FORWARD_DIRECTION)
                {
                    //Track only if train is entering from reverse direction	(EF to SF)
                    comm_check_EF2.Local_counts[0] = (BYTE)Get_US_Fwd_AxleCount();
                    comm_check_EF2.Local_counts[1] = (BYTE)(Get_US_Fwd_AxleCount()>>8);
                }
                return; /*do not declare any error. Wait for timeout*/
            }
            else
            {
                if(comm_check_EF2.Wait_timeout != 0)
                {
                    //Check for local count values
                    if(Get_US_AxleDirection() == (BYTE)FORWARD_DIRECTION)
                    {
                        comm_check_EF2.Wheel_match_fail = 0;
                        if(comm_check_EF2.Local_counts[0] != (BYTE)Get_US_Fwd_AxleCount())
                            comm_check_EF2.Wheel_match_fail = 1;
                        if(comm_check_EF2.Local_counts[1] != (BYTE)(Get_US_Fwd_AxleCount()>>8))
                            comm_check_EF2.Wheel_match_fail = 1;
                        if(comm_check_EF2.Wheel_match_fail)
                        {
                            // wheel has mismatched. declare direct out count and comm failures!
                            Status.Flags.US1_to_LU2_Link = COMMUNICATION_FAILED;
                            Status.Flags.US2_to_LU2_Link = COMMUNICATION_FAILED;
                            Declare_DAC_Defective();
                            Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
                            Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);

                            Status.Flags.Direct_Out_Count = SET_HIGH;
                            Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                        }                        
                    }
                    return; /*do not declare any error. Wait for timeout*/
                }
                Comm_A_CountDown.US1_to_LU2 = TIMEOUT_EVENT;
                Comm_A_CountDown.US2_to_LU2 = TIMEOUT_EVENT;
                //Communication has not been restored
                // proceed in declaring the error.
                
            }
            //2mins countdown has elapsed, declare the error
        }
        else
        {
            //No Comm error
            //if communication has been restored from failure state, check for the count
            if (Com1RecvObject.State != (BYTE)COM_VALID_MESSAGE)
                            return;
            if(comm_check_EF2.State == COMM_FAILED)
            {
                //if the wheel counts are unchanged, no error.
                for(uchTrack = 0;uchTrack<10;uchTrack++)
                    if(comm_check_EF2.Track_counts[uchTrack] != Com1RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET + uchTrack])
                        comm_check_EF2.Wheel_match_fail = 1;
                if(comm_check_EF2.Wheel_match_fail)
                {
                    // wheel has mismatched. declare direct out count and comm failures!
                    Status.Flags.US1_to_LU2_Link = COMMUNICATION_FAILED;
                    Status.Flags.US2_to_LU2_Link = COMMUNICATION_FAILED;
                    Declare_DAC_Defective();
                    Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
                    Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
                    
                    Status.Flags.Direct_Out_Count = SET_HIGH;
                    Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
                    
                    comm_check_EF2.State = COMM_GOOD;
                    return;
                }    
                //count matches, do not declare error
            }   
            comm_check_EF2.State = COMM_GOOD;
        }
    }
    else if(US_Section_Mode.Local_Unit == SYSTEM_CLEAR_MODE)
    {
        comm_check_EF2.State = COMM_GOOD;
        comm_check_EF2.Wait_timeout = 0;
    }
    if (Comm_A_CountDown.US1_to_LU2 == TIMEOUT_EVENT)
    {
        Status.Flags.US1_to_LU2_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Comm_A_CountDown.US2_to_LU2 == TIMEOUT_EVENT)
    {
        Status.Flags.US2_to_LU2_Link = COMMUNICATION_FAILED;
        uchFailure = TRUE;
    }
    if (Status.Flags.LU2_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU2_to_US2_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US1_Link == COMMUNICATION_FAILED ||
        Status.Flags.LU1_to_US2_Link == COMMUNICATION_FAILED)
    {
        uchFailure = TRUE;
    }
    if (uchFailure)
    {
        Declare_DAC_Defective();
        Set_Error_Status_Byte(COMM1_LINK_FAIL_ID,Status.Byte[3]);
        Set_Error_Status_Byte(COMM2_LINK_FAIL_ID,Status.Byte[4]);
    }
}

//21_12_2009


/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Inter_Processor_Link(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the communication link between a CPU with its peer in the same unit and
                     declare any link failure errors

Allocated Requirements:(SSDAC_SWRS_ 0819),(SSDAC_ SWRS_0820), (SSDAC_SWRS_0108), (SSDAC_SWRS_0828)
					  (SSDAC_SWRS_0408), (SSDAC_SWRS_0415), (SSDAC_SWRS_0209), (SSDAC_SWRS_0907)
					  (SSDAC_SWRS_0508), (SSDAC_SWRS_0591), (SSDAC_SWRS_0609), (SSDAC_SWRS_0708)
						(SSDAC_SWRS_0683), (SSDAC_SWRS_0783), (SSDAC_SWRS_0307), (SSDAC_SWRS_0395)
					
Design Requirements:	SSDAC_DR_5305


Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective()
                     ERROR.C-Set_Error_Status_Bit()

    Called by       :SYS_MON.C-Update_Sys_Mon_State()

Input Variables                     Name                Type
    Global          :   SPI_Transmit_Object.Timeout     UINT16

    Local           :   None

Output Variables                    Name                Type
    Global          :   Status.Flags.Peer_CPU_Link      Bit

    Local           :   None
Signal Variables




Macros defined      :                   Macro               Value
                                    TIMEOUT_EVENT             0
                                    COMMUNICATION_FAILED      0
                                    ASSOCIATE_CPU_LINK_FAILURE_ERROR_NUM  25
References          :

Derived Requirements:

Algorithm           :If there is no transmission for 20ms then declare as SPI communication
                     failed, set the error bit to indicate the error & declare as DAC defective
*******************************************************************************/
void Check_Inter_Processor_Link(void)
{
    if(SPI_Transmit_Object.Timeout == TIMEOUT_EVENT)
    {
      Status.Flags.Peer_CPU_Link = COMMUNICATION_FAILED;
      Set_Error_Status_Bit(ASSOCIATE_CPU_LINK_FAILURE_ERROR_NUM);
      Declare_DAC_Defective();
    }
}

//01/09/10

/******************************************************************************
Component name      :SYS_MON
Module Name         :void Update_DS_Track_Status(BYTE new_Status)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to update the track status information bit from the down stream remote unit if there
                     is any change

Allocated Requirements:

Design Requirements:	SSDAC_DR_5306

Interfaces
    Calls           :SYS_MON.C-Start_FeedBack_Check()

    Called by       :COMM_DS.C-Process_DS_Reset_Info_Message()
                     COMM_DS.C-Process_DS_Axle_Count_Message()

Input Variables                     Name                                Type
    Global          :DIP_Switch_Info.DAC_Unit_Type                          DAC_Unit_Type
                        Flags.Bit.b3                                    Bit
                        Flags.Bit.b2                                    Bit
                        Track_Status_Info.Bit.Remote_DS_Track_Status    Bit
                        DAC_UNIT_TYPE_CF                                Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator

    Local           :   new_Status                                      BYTE
                        DS_TRACK_STATUS_CHECK_ID                        Enumerator
                        Flags                                           bitadrb_t

Output Variables                    Name                                Type
    Global          :   Track_Status_Info.Bit.Remote_DS_Track_Status    Bit

    Local           :   None

Signal Variables



Macros defined      :   Macro                       Value
                        None

References          :

Derived Requirements:


Algorithm           :1.If the unit type is D3 then assign the new state same as down strean track status
                       else assign new status same as up stream track status
                     2.If the remote unit down stream track status and new status is not same then assign remote
                       down stream track status as new status & check the feedback of DS track status to ensure that
                       both the units(local & remote) are in same down stream track status
*******************************************************************************/
void Update_DS_Track_Status(bitadrb_t Flags)
{
    BYTE new_Status;

   if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
   {
        /* 3D configuartion */
        new_Status = (BYTE) Flags.Bit.b3;
   }
   else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
   {
        /* 3D configuartion */
        new_Status = (BYTE) Flags.Bit.b3;
   }
   else
   {
        /* 2D,3D-2S and 3D3S configuartion */
        new_Status = (BYTE) Flags.Bit.b2;
   }
   if(Track_Status_Info.Bit.Remote_DS_Track_Status != new_Status)
   {
    Track_Status_Info.Bit.Remote_DS_Track_Status = new_Status;
    Start_FeedBack_Check(DS_TRACK_STATUS_CHECK_ID);
   }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Update_US_Track_Status(BYTE new_Status)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to update the track status information bit from the up stream remote unit if there
                     is any change

Allocated Requirements:

Design Requirements:	SSDAC_DR_5307


Interfaces
    Calls           :SYS_MON.C-Start_FeedBack_Check()

    Called by       :COMM_US.C-Process_US_Reset_Info_Message()
                     COMM_US.C-Process_US_Axle_Count_Message

Input Variables                     Name                                Type
    Global          :   DAC_UNIT_TYPE_CF                                Enumerator
 *                      DAC_UNIT_TYPE_3D_SF
 *                      DAC_UNIT_TYPE_3D_EF
                        DIP_Switch_Info.DAC_Unit_Type                       DAC_Unit_Type
                        Flags.Bit.b3                                    Bit
                        Track_Status_Info.Bit.Remote_US_Track_Status    Bit
                        US_TRACK_STATUS_CHECK_ID                        Enumerator


    Local           :   new_Status                                      Byte
                        Flags                                           bitadrb_t

Output Variables                    Name                                Type
    Global          :   Track_Status_Info.Bit.Remote_US_Track_Status    Bit

    Local           :   None

Signal Variables


Macros defined      :   Macro               Value
                        None

References          :
Derived Requirements:


Algorithm           :1.If the unit type is D3 then just return
                     2.Assign new status as down stream track status
                     3.If the remote unit Up stream track status and new status is not same then assign remote
                     Up stream track status as new status & check the feedback of US track status to ensure that
                     both the units(local & remote) are in same Up stream track status
*******************************************************************************/
void Update_US_Track_Status(bitadrb_t Flags)
{
    BYTE new_Status;

   if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
   {
        /* 3D configuartion */
        new_Status = (BYTE) Flags.Bit.b2;
   }
   else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C ||
      DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
   {
        /* 3D configuartion */
        new_Status = (BYTE) Flags.Bit.b2;
   }
   else
   {
       new_Status = (BYTE) Flags.Bit.b3;
   }
   if(Track_Status_Info.Bit.Remote_US_Track_Status != new_Status)
   {
    Track_Status_Info.Bit.Remote_US_Track_Status = new_Status;
    Start_FeedBack_Check(US_TRACK_STATUS_CHECK_ID);
   }
}

/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_Power_Status(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to start the checking of the power status information bits from the remote units if there
                     is any change

Allocated Requirements:

Design Requirements:	SSDAC_DR_5308

Interfaces
    Calls           :SYS_MON.C-Check_US_Power_Status()
                     SYS_MON.C-Check_DS_Power_Status()

    Called by       :SYS_MON.C-Update_Sys_Mon_State()

Input Variables                     Name                                Type
    Global          :   Power_Supply_Status.Bit.US1_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.US1_New_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_New_Power_Status    Bit
                        Power_Supply_Status.Bit.DS1_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.DS1_New_Power_Status    Bit
                        Power_Supply_Status.Bit.DS2_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.DS2_New_Power_Status    Bit

    Local           :   None

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables



Macros defined      :   Macro               Value
                        None
References          :

Derived Requirements:

Algorithm           :1.Check the Upstream CPU1,CPU2 and downstream CPU1,CPU2 old power status and new power status are same
                     2.If all CPU's old and new status are same then do nothing return immediately otherwise go for checking the
                        Upstream and downstream power status
*******************************************************************************/
void Check_Power_Status(void)
{
    if( Power_Supply_Status.Bit.US1_Old_Power_Status ==  Power_Supply_Status.Bit.US1_New_Power_Status &&
        Power_Supply_Status.Bit.US2_Old_Power_Status ==  Power_Supply_Status.Bit.US2_New_Power_Status &&
        Power_Supply_Status.Bit.DS1_Old_Power_Status ==  Power_Supply_Status.Bit.DS1_New_Power_Status &&
        Power_Supply_Status.Bit.DS2_Old_Power_Status ==  Power_Supply_Status.Bit.DS2_New_Power_Status)
    {
        /* no changes in remote power supply status */
        return;
    }
    Check_US_Power_Status();
    Check_DS_Power_Status();
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_DS_Power_Status(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the power status information bits from down stream remote units and indicate the errors
                     if there are any

Allocated Requirements:(SSDAC_SWRS_0586), (SSDAC_SWRS_0300), (SSDAC_SWRS_0215), (SSDAC_SWRS_0908)
					  (SSDAC_SWRS_0513), (SSDAC_SWRS_0593), (SSDAC_SWRS_0678), (SSDAC_SWRS_0778)
						(SSDAC_SWRS_0614), (SSDAC_SWRS_0685), (SSDAC_SWRS_0713), (SSDAC_SWRS_0785)
						(SSDAC_SWRS_0390), (SSDAC_SWRS_0314), (SSDAC_SWRS_0397).

Design Requirements:	SSDAC_DR_5309

Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective_DS()
                     ERROR.C-Set_Error_Status_Bit()

    Called by       :SYS_MON.C-Check_Power_Status

Input Variables                     Name                                Type
    Global          :   Power_Supply_Status.Bit.DS1_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.DS1_New_Power_Status    Bit
                        Power_Supply_Status.Bit.DS2_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.DS2_New_Power_Status    Bit

    Local           :   None

Output Variables                    Name                                Type
    Global          :   Power_Supply_Status.Bit.DS1_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.DS2_Old_Power_Status    Bit
                        Status.Flags.Power_Fail_at_DS1                  Bit
                        Status.Flags.Power_Fail_at_DS2                  Bit

    Local           :   None
Signal Variables


Macros defined      :                   Macro                           Value
                                        SET_LOW                           0
                                        SET_HIGH                          1
                                        POWER_FAILED                      0
                                        TRANSIENT_POWER_FAILURE_DS1_ID    48
                                        TRANSIENT_POWER_FAILURE_DS2_ID    49
References          :

Derived Requirements:
Algorithm           :1.If the down stream remote unit CPU1 old power status is off and down stream remote unit CPU1 new
                     power status is on then on the down stream remote unit CPU1 old power status
                     2.If the down stream remote unit CPU2 old power status is off and down stream remote unit CPU2 new
                     power status is on then on the down stream remote unit CPU2 old power status
                     3.If down stream remote unit CPU1 old power status is on and down stream remote unit CPU1 new power
                     status is off then declare as DAC defective, down stream  remote unit power status CPU1
                     has failed & set the error bit to indicate the error
                     4.If down stream remote unit CPU2 old power status is on and down stream remote unit CPU2 new power
                     status is off then declare as DAC defective, down stream remote unit power status CPU2
                     has failed & set the error bit to indicate the error
*******************************************************************************/
void Check_DS_Power_Status(void)
{
    if( Power_Supply_Status.Bit.DS1_Old_Power_Status == SET_LOW &&
        Power_Supply_Status.Bit.DS1_New_Power_Status == SET_HIGH)
    {
        Power_Supply_Status.Bit.DS1_Old_Power_Status =  SET_HIGH;
    }
    if( Power_Supply_Status.Bit.DS2_Old_Power_Status == SET_LOW &&
        Power_Supply_Status.Bit.DS2_New_Power_Status == SET_HIGH )
    {
        Power_Supply_Status.Bit.DS2_Old_Power_Status =  SET_HIGH;
    }
    if(Power_Supply_Status.Bit.DS1_Old_Power_Status == SET_HIGH &&
       Power_Supply_Status.Bit.DS1_New_Power_Status == SET_LOW )
    {
        Declare_DAC_Defective_DS();
        Status.Flags.Power_Fail_at_DS1 = POWER_FAILED;
        Set_Error_Status_Bit(TRANSIENT_POWER_FAILURE_DS1_ERROR_NUM);
    }
    if(Power_Supply_Status.Bit.DS2_Old_Power_Status == SET_HIGH &&
       Power_Supply_Status.Bit.DS2_New_Power_Status == SET_LOW)
    {
        Declare_DAC_Defective_DS();
        Status.Flags.Power_Fail_at_DS2 = POWER_FAILED;
        Set_Error_Status_Bit(TRANSIENT_POWER_FAILURE_DS2_ERROR_NUM);
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Check_US_Power_Status(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to check the power status information bits from up stream remote units and indicate the errors
                     if there are any

Allocated Requirements:(SSDAC_SWRS_0486), (SSDAC_SWRS_0413)
					  (SSDAC_SWRS_0415), (SSDAC_SWRS_0586)
					  (SSDAC_SWRS_0513), (SSDAC_SWRS_0593),(SSDAC_SWRS_0314), (SSDAC_SWRS_0397).
						(SSDAC_SWRS_0678), (SSDAC_SWRS_0778), (SSDAC_SWRS_0390)
						(SSDAC_SWRS_0614), (SSDAC_SWRS_0685), (SSDAC_SWRS_0713), (SSDAC_SWRS_0785)
						
Design Requirements:	SSDAC_DR_5310
Interfaces
    Calls           :RELAYMGR.C-Declare_DAC_Defective_US()
                     ERROR.C-Set_Error_Status_Bit()

    Called by       :SYS_MON.C-Check_Power_Status

Input Variables                     Name                                Type
    Global          :   Power_Supply_Status.Bit.US1_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.US1_New_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_New_Power_Status    Bit

    Local           :   None

Output Variables                    Name                Type
    Global          :   Power_Supply_Status.Bit.US1_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_Old_Power_Status    Bit
                        Status.Flags.Power_Fail_at_US1                  Bit
                        Status.Flags.Power_Fail_at_US2                  Bit

    Local           :   None

Signal Variables


Macros defined      :                   Macro               Value
                                        SET_LOW                           0
                                        SET_HIGH                          1
                                        POWER_FAILED                      0
                                        TRANSIENT_POWER_FAILURE_US1_ID    50
                                        TRANSIENT_POWER_FAILURE_US2_ID    51
References          :

Derived Requirements:
Algorithm           :1.If the up stream remote unit CPU1 old old power status is off and up stream remote unit CPU1 new
                     power status is on then on up stream remote unit CPU1 old power status
                     2.If the up stream remote unit CPU2 old old power status is off and up stream remote unit CPU2 new
                     power status is on then on up stream remote unit CPU2 old power status
                     3.If up stream remote unit CPU1 old power status is on and up stream remote unit CPU1 new power status
                     is off then declare as DAC defective, up stream remote unit CPU1 power failure & set the error
                     bit to indicate that error
                     4.If up stream remote unit CPU2 old power status is on and up stream remote unit CPU2 new power status
                     is off then declare as DAC defective, up stream remote unit CPU2 power failure & set the error
                     bit to indicate that error
*******************************************************************************/
void Check_US_Power_Status(void)
{
    if( Power_Supply_Status.Bit.US1_Old_Power_Status == SET_LOW &&
        Power_Supply_Status.Bit.US1_New_Power_Status == SET_HIGH)
    {
        Power_Supply_Status.Bit.US1_Old_Power_Status =  SET_HIGH;
    }
    if( Power_Supply_Status.Bit.US2_Old_Power_Status == SET_LOW &&
        Power_Supply_Status.Bit.US2_New_Power_Status == SET_HIGH )
    {
        Power_Supply_Status.Bit.US2_Old_Power_Status =  SET_HIGH;
    }
    if(Power_Supply_Status.Bit.US1_Old_Power_Status == SET_HIGH &&
       Power_Supply_Status.Bit.US1_New_Power_Status == SET_LOW )
    {
        Set_Error_Status_Bit(TRANSIENT_POWER_FAILURE_US1_ERROR_NUM);
        Status.Flags.Power_Fail_at_US1 = POWER_FAILED;
        Declare_DAC_Defective_US();
    }
    if(Power_Supply_Status.Bit.US2_Old_Power_Status == SET_HIGH &&
       Power_Supply_Status.Bit.US2_New_Power_Status == SET_LOW)
    {
        Declare_DAC_Defective_US();
        Status.Flags.Power_Fail_at_US2 = POWER_FAILED;
        Set_Error_Status_Bit(TRANSIENT_POWER_FAILURE_US2_ERROR_NUM);
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Update_US_Power_Status(  bitadrb_t DAC_Config,bitadrb_t Flags1)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to update the power status information bits/flags from up stream remote units indicating
                     the changes

Allocated Requirements:

Design Requirements:	SSDAC_DR_5311

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Reset_Info_Message()
                        COMM_US.C-Process_US_Axle_Count_Message()

Input Variables                     Name                                Type
    Global          :   DAC_Config.Bit.b4                               Bit
                        Flags1.Bit.b0                                   Bit

    Local           :   DAC_Config                                      bitadrb_t
                        Flags1                                          bitadrb_t

Output Variables                    Name                                Type
    Global          :   Power_Supply_Status.Bit.US1_New_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_New_Power_Status    Bit

    Local           :   None
Signal Variables


Macros defined      :   Macro               Value
                        None
References          :

Derived Requirements:

Algorithm           :Update the New power supply status form Up stream remote units(CPU1 or CPU2) before
                     making any changes
*******************************************************************************/
void Update_US_Power_Status(  bitadrb_t DAC_Config,bitadrb_t Flags1)
{

    if (DAC_Config.Bit.b4)
    {
      Power_Supply_Status.Bit.US1_New_Power_Status =Flags1.Bit.b0;
    }
    else
    {
      Power_Supply_Status.Bit.US2_New_Power_Status =Flags1.Bit.b0;
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Update_US_Power_Status(  bitadrb_t DAC_Config,bitadrb_t Flags1)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to update the power status information bits/flags from up stream remote units indicating
                     the changes

Allocated Requirements:

Design Requirements:	SSDAC_DR_5312

Interfaces
    Calls           :   None

    Called by       :   COMM_US.C-Process_US_Reset_Info_Message()
                        COMM_US.C-Process_US_Axle_Count_Message()

Input Variables                     Name                                Type
    Global          :   DAC_Config.Bit.b4                               Bit
                        Flags1.Bit.b0                                   Bit

    Local           :   DAC_Config                                      bitadrb_t
                        Flags1                                          bitadrb_t

Output Variables                    Name                                Type
    Global          :   Power_Supply_Status.Bit.US1_New_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_New_Power_Status    Bit

    Local           :   None
Signal Variables


Macros defined      :   Macro               Value
                        None
References          :

Derived Requirements:

Algorithm           :Update the New power supply status form Up stream remote units(CPU1 or CPU2) before
                     making any changes
*******************************************************************************/
void Update_DS_Power_Status(  bitadrb_t DAC_Config,bitadrb_t Flags1)
{
    if (DAC_Config.Bit.b4)
    {
      Power_Supply_Status.Bit.DS1_New_Power_Status =Flags1.Bit.b0;
    }
    else
    {
      Power_Supply_Status.Bit.DS2_New_Power_Status =Flags1.Bit.b0;
    }
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Clear_US_Power_Status(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to clear old power status information bits/flag from up stream remote units before
                     making any changes to the bits/flag

Allocated Requirements:

Design Requirements:	SSDAC_DR_5313

Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Set_Relay_A_Reset_State()

Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                                Type
    Global          :   Power_Supply_Status.Bit.US1_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.US1_New_Power_Status    Bit
                        Power_Supply_Status.Bit.US2_New_Power_Status    Bit
                        Status.Flags.Power_Fail_at_US1                  Bit
                        Status.Flags.Power_Fail_at_US2                  Bit

    Local           :   None

Signal Variables


Macros defined      :                   Macro               Value
                                        SET_LOW               0
                                        POWER_OK              1
References          :

Derived Requirements:

Algorithm           :1.Set all the Upstream CPU1 ,CPU2 old and new power status as LOW
                     2.Set the Upstream power status flag bits of CPU1 and CPU2 as high
*******************************************************************************/
void Clear_US_Power_Status(void)
{
    Power_Supply_Status.Bit.US1_Old_Power_Status = SET_LOW;
    Power_Supply_Status.Bit.US2_Old_Power_Status = SET_LOW;
    Power_Supply_Status.Bit.US1_New_Power_Status = SET_LOW;
    Power_Supply_Status.Bit.US2_New_Power_Status = SET_LOW;
    Status.Flags.Power_Fail_at_US1 = POWER_OK;
    Status.Flags.Power_Fail_at_US2 = POWER_OK;
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Clear_DS_Power_Status(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to clear old power status information bits/flag from down stream remote units before
                     making any changes to the bits/flag

Allocated Requirements:

Design Requirements:	SSDAC_DR_5314

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.c  -   Set_Relay_B_Reset_State()

Input Variables         Name                Type
    Global          :   None

    Local           :   None

Output Variables            Name                                        Type
    Global          :   Power_Supply_Status.Bit.DS1_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.DS2_Old_Power_Status    Bit
                        Power_Supply_Status.Bit.DS1_New_Power_Status    Bit
                        Power_Supply_Status.Bit.DS2_New_Power_Status    Bit
                        Status.Flags.Power_Fail_at_DS1                  Bit
                        Status.Flags.Power_Fail_at_DS2                  Bit

    Local           :   None
Signal Variables

Macros defined      :       Macro               Value
                            SET_LOW               0
                            POWER_OK              1
References          :

Derived Requirements:

Algorithm           :1.Set all the Downstream CPU1 ,CPU2 old and new power status as LOW
                     2.Set the Downstream power status flag bits of CPU1 and CPU2 as high
*******************************************************************************/
void Clear_DS_Power_Status(void)
{
    Power_Supply_Status.Bit.DS1_Old_Power_Status = SET_LOW;
    Power_Supply_Status.Bit.DS2_Old_Power_Status = SET_LOW;
    Power_Supply_Status.Bit.DS1_New_Power_Status = SET_LOW;
    Power_Supply_Status.Bit.DS2_New_Power_Status = SET_LOW;
    Status.Flags.Power_Fail_at_DS1 = POWER_OK;
    Status.Flags.Power_Fail_at_DS2 = POWER_OK;
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Clear_Comm_US_Failures(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to clear the old link failure messages in upstream remote units before making any
                     changes

Allocated Requirements:

Design Requirements:	SSDAC_DR_5315

Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.c  -   Set_Relay_A_Reset_State()

Input Variables         Name                Type
    Global          :   None

    Local           :   None

Output Variables            Name                        Type
    Global          :   Status.Flags.US1_to_LU1_Link    Bit
                        Status.Flags.US1_to_LU2_Link    Bit
                        Status.Flags.US2_to_LU1_Link    Bit
                        Status.Flags.US2_to_LU2_Link    Bit

    Local           :   None

Signal Variables



Macros defined      :   Macro               Value
                        COMMUNICATION_OK          1
References          :

Derived Requirements:

Algorithm           :Set all the communication link status flags for the Upstream section as OK
*******************************************************************************/
void Clear_Comm_US_Failures(void)
{
    Status.Flags.US1_to_LU1_Link = COMMUNICATION_OK;
    Status.Flags.US1_to_LU2_Link = COMMUNICATION_OK;
    Status.Flags.US2_to_LU1_Link = COMMUNICATION_OK;
    Status.Flags.US2_to_LU2_Link = COMMUNICATION_OK;
}
/******************************************************************************
Component name      :SYS_MON
Module Name         :void Clear_Comm_DS_Failures(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Routine to clear the old link failure messages in down stream remote units before making any
                     changes

Allocated Requirements:

Design Requirements:	SSDAC_DR_5316

Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.c  -   Set_Relay_B_Reset_State()

Input Variables         Name                Type
    Global          :   None

    Local           :   None

Output Variables            Name                        Type
    Global          :   Status.Flags.DS1_to_LU1_Link    Bit
                        Status.Flags.DS1_to_LU2_Link    Bit
                        Status.Flags.DS2_to_LU1_Link    Bit
                        Status.Flags.DS2_to_LU2_Link    Bit

    Local           :   None

Signal Variables



Macros defined      :   Macro               Value
                        COMMUNICATION_OK          1
References          :

Derived Requirements:

Algorithm           :Set all the communication link status flags for the Downstream section as OK
*******************************************************************************/
void Clear_Comm_DS_Failures(void)
{
    Status.Flags.DS1_to_LU1_Link = COMMUNICATION_OK;
    Status.Flags.DS1_to_LU2_Link = COMMUNICATION_OK;
    Status.Flags.DS2_to_LU1_Link = COMMUNICATION_OK;
    Status.Flags.DS2_to_LU2_Link = COMMUNICATION_OK;
}


void Decrement_comm_check_50msTmr(void)
{
    if(comm_check_SF1.Wait_timeout > 0)
    {
        comm_check_SF1.Wait_timeout = comm_check_SF1.Wait_timeout -1;
    }
    if(comm_check_SF2.Wait_timeout > 0)
    {
        comm_check_SF2.Wait_timeout = comm_check_SF2.Wait_timeout -1;
    }
    if(comm_check_EF1.Wait_timeout > 0)
    {
        comm_check_EF1.Wait_timeout = comm_check_EF1.Wait_timeout -1;
    }
    if(comm_check_EF2.Wait_timeout > 0)
    {
        comm_check_EF2.Wait_timeout = comm_check_EF2.Wait_timeout -1;
    }
    
}