/********************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RELAYMGR
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
    Functions   :   void Initialise_Relay_Mgr(void)
                    void Update_Relay_A_Counts(void)
                    void Update_Relay_B_Counts(void)
                    void Update_Relay_DE_Counts(void)
                    void Update_Relay_D3_Counts(void)
                    void Update_SF_Track_Status(void)
                    void Update_CF_Track_Status(void)
                    void Update_EF_Track_Status(void)
                    void Declare_DAC_Defective(void)
                    void Declare_DAC_Defective_US(void)
                    void Declare_DAC_Defective_DS(void)

************************************************************************************/
#include <xc.h>

#include "COMMON.h"
#include "RELAYDRV.h"
#include "RELAYMGR.h"
#include "RLYA_MGR.h"
#include "RLYB_MGR.h"
#include "RLYD3_MGR.h"
#include "RLYD4_MGR.h"
#include "RESET.h"
#include "SYS_MON.h"
#include "ERROR.h"
#include "AXLE_MON.h"

UINT16 LCWS_Section_TimeOut;

extern  /*near*/  dac_status_t Status;                    /* from dac_main.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from dac_main.c */
extern ds_section_mode DS_Section_Mode;             /*from DAC_MAIN.c*/
extern us_section_mode US_Section_Mode;             /*from DAC_MAIN.c*/





extern BYTE Relay_B_State;



extern void Clear_DS_AxleCount_1(void);
extern void Clear_DS_AxleCount_2(void);
extern void Clear_DS_AxleCount_3(void);
extern void Clear_DS_AxleCount_4(void);

void Update_LCWS_Relay_State(void);
void Update_LCWS_DL_Relay_State(void);
void Update_DE_Relay_State(void);
/***************************************************************************
Component name      :RELAYMGR
Module Name         :void Initialise_Relay_Mgr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initialise Relay Manager by calling Deenergise the preporatory
                     relay,initailise the relay A, B, D3 manager functions

Allocated Requirements	: 	(SSDAC_SWRS_0032),(SSDAC_SWRS_0429), (SSDAC_SWRS_0234)

Design Requirements		:	SSDAC_DR_5189


Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Preparatory_Relay_A()
                        RELAYDRV.C-DeEnergise_Preparatory_Relay_B()
                        RLYA_MGR.C-Initialise_Relay_A_Mgr()
                        RLYB_MGR.C-Initialise_Relay_B_Mgr()
                        RLYD3_MGR.C-Initialise_Relay_D3_Mgr()
                        RLYDE_MGR.C-Initialise_Relay_DE_Mgr()

    Called by       :   DAC_MAIN.C-Initialise_System()

Input Variables                     Name                Type
    Global          :   None

    Local           :   None

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables


Macros defined      :   Macro                   Value
                        None

References          :

Derived Requirements:

Algorithm           :1.Deenergise the preparatory relay A and B
                     2.Initialise the relay managers A,B and D3
***************************************************************************/
void Initialise_Relay_Mgr(void)
{
    DeEnergise_Preparatory_Relay_A();           /* from relaydrv.c */
    DeEnergise_Preparatory_Relay_B();           /* from relaydrv.c */
    Initialise_Relay_A_Mgr();
    Initialise_Relay_B_Mgr();
    Initialise_Relay_D3_Mgr();
    Initialise_Relay_D4_Mgr();
}
/**************************************************************************
Component name      :RELAYMGR
Module Name         :void Update_Relay_A_Counts(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the Up stream axle direction, forward axle count,
                     reverse axle count.Process the Local direction and
                     counts and update the Relay A state

Allocated Requirements  :   Update_Relay_A_Counts

Design Requirements     :SSDAC_DR_5191, SSDAC_DR_5366, SSDAC_DR_5367, SSDAC_DR_5368, SSDAC_DR_5369


Interfaces
    Calls           :   AXLE_MON.C-Get_US_AxleDirection()
                        AXLE_MON.C-Get_US_Fwd_AxleCount()
                        AXLE_MON.C-Get_US_Rev_AxleCount()
                        AXLE_MON.c-Get_US_AxleCount()
                        RLYA_MGR.C-Process_Relay_A_Local_Direction()
                        RLYA_MGR.C-Process_Relay_A_Local_AxleCount()
                        RLYA_MGR.C-Update_Relay_A_State()

    Called by       :   DAC_MAIN.C-Control_DAC_Type_CF()
                        DAC_MAIN.C-Control_DAC_Type_EF()

Input Variables                 Name                    Type
    Global          :   None

    Local           :   uiFwd_Count                     UINT16
                        uiRev_Count                     UINT16
                        uiAxle_Count                    UINT16
                        uchDir                          BYTE

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None

References          :

Derived Requirements:

Algorithm           :1.Get the upstream axle direction,forward axle counts,reverse axle counts and local axle clearing counts
                     2.Update the CPU1 and CPU2 axle direction and counts
                     3.Update the relay A state
*******************************************************************************/
void Update_Relay_A_Counts(void)
{
    UINT16 uiFwd_Count;
    UINT16 uiRev_Count;
    UINT16 uiAxle_Count;
    BYTE uchDir;

    uchDir      = Get_US_AxleDirection();                       /* from axlemon.c */
    uiFwd_Count = Get_US_Fwd_AxleCount();                       /* from axlemon.c */
    uiRev_Count = Get_US_Rev_AxleCount();                       /* from axlemon.c */
    uiAxle_Count= Get_US_AxleCount();                           /* from axlemon.c */
    Process_Relay_A_Local_Direction(uchDir,uiAxle_Count);       /* from rlyb_mgr.c */
    Process_Relay_A_Local_AxleCount(uiFwd_Count,uiRev_Count);   /* from rlya_mgr.c */
    Update_Relay_A_State();                                     /* from rlya_mgr.c */
}

//15_03_10
void Update_3S_Relay_A_Counts(void)
{
    UINT16 uiFwd_Count;
    UINT16 uiRev_Count;
    UINT16 uiAxle_Count;
    BYTE uchDir;

    uchDir      = Get_US_AxleDirection();                       /* from axlemon.c */
    uiFwd_Count = Get_US_Fwd_AxleCount();                       /* from axlemon.c */
    uiRev_Count = Get_US_Rev_AxleCount();                       /* from axlemon.c */
    uiAxle_Count= Get_US_AxleCount();                           /* from axlemon.c */
    Process_Relay_A_Local_Direction(uchDir,uiAxle_Count);       /* from rlya_mgr.c */
    Process_Relay_A_Local_AxleCount(uiFwd_Count,uiRev_Count);   /* from rlya_mgr.c */
    Update_3S_Relay_A_State();                                  /* from rlya_mgr.c */
}
//19_12_09



extern feedback_info_t FeedBack_Info[MAXIMUM_TYPES_OF_FEEDBACKS];
extern track_status_info Track_Status_Info;
extern BYTE Relay_B_PR_State;
void Update_LCWS_Relay_State(void)
{
    BYTE Enable_Relay_1, Enable_Relay_2;
    INT16 uiAuthorisationKey;

    BYTE Enable_Variables_Clear;

    if (Status.Flags.System_Status != NORMAL)
    {
        if(Relay_B_PR_State == 1)
        {
            DeEnergise_Preparatory_Relay_B();     /* from relaydrv.c */
        }
        if(Relay_B_State == 1)
        {
            DeEnergise_Vital_Relay_B();     /* from relaydrv.c */
        }

        return;
    }


    Enable_Variables_Clear = 1;

    Enable_Relay_1 = 0;
    Enable_Relay_2 = 0;
//
//    if(Rlyb_State == NO_TRAIN_IN_SECTION)
//    {
//      B_Sec_Led_drive(ON);
//    }
//    else
//    {
//      B_Sec_Led_drive(OFF);
//    }
// Reverse Direction
    if(((Track_Info_2.LCWS_Total_Rev_Count > Track_Info_4.LCWS_Total_Rev_Count) || (Track_Info_3.LCWS_Total_Rev_Count > Track_Info_4.LCWS_Total_Rev_Count) || (Track_Info_1.LCWS_Total_Rev_Count > Track_Info_4.LCWS_Total_Rev_Count ) || ((Track_Info_2.LCWS_Total_Rev_Count > Track_Info_4.LCWS_Total_Rev_Count ) && (Track_Info_2.LCWS_Total_Rev_Count > Track_Info_3.LCWS_Total_Rev_Count ))))
    {
        Enable_Relay_1 = 0;
        Enable_Relay_2 = 0;

        /* no in-counts, only out-counts,system goes to error state*/
        Declare_DAC_Defective();
        Status.Flags.Direct_Out_Count = SET_HIGH;
        Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
        return;
    }

// Forward Direction
    if(((Track_Info_2.LCWS_Total_Fwd_Count > Track_Info_1.LCWS_Total_Fwd_Count) || 
            (Track_Info_3.LCWS_Total_Fwd_Count > Track_Info_1.LCWS_Total_Fwd_Count) || 
            (Track_Info_4.LCWS_Total_Fwd_Count > Track_Info_1.LCWS_Total_Fwd_Count ) || 
            ((Track_Info_3.LCWS_Total_Fwd_Count > Track_Info_2.LCWS_Total_Fwd_Count) && 
            (Track_Info_3.LCWS_Total_Fwd_Count > Track_Info_1.LCWS_Total_Fwd_Count)) || 
            ((Track_Info_4.LCWS_Total_Fwd_Count > Track_Info_3.LCWS_Total_Fwd_Count) && 
            (Track_Info_4.LCWS_Total_Fwd_Count > Track_Info_2.LCWS_Total_Fwd_Count) && 
            (Track_Info_4.LCWS_Total_Fwd_Count > Track_Info_1.LCWS_Total_Fwd_Count))))
    {
        Enable_Relay_1 = 0;
        Enable_Relay_2 = 0;

        /* no in-counts, only out-counts,system goes to error state*/
        Declare_DAC_Defective();
        Status.Flags.Direct_Out_Count = SET_HIGH;
        Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
        return;
    }

        //if card1, card2, card3 > 0 && card1 == card3 +1 - Fwd count
    if((Track_Info_1.LCWS_Fwd_Count>0 && Track_Info_2.LCWS_Fwd_Count>0 && Track_Info_3.LCWS_Fwd_Count>0) && (Track_Info_1.LCWS_Fwd_Count == (Track_Info_3.LCWS_Fwd_Count+1)))
    {
        Enable_Relay_1 = 1;
    }

    //if card1, card2, card3 > 0 && card1 == card3+1 - Rev count
    if((Track_Info_4.LCWS_Rev_Count>0 && Track_Info_3.LCWS_Rev_Count>0 && Track_Info_2.LCWS_Rev_Count>0) && (Track_Info_4.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count+1)))
    {
        Enable_Relay_2 = 1;
    }

    //if card1, card2, card3 > 0 && card1 == card3 - Fwd count
    if((Track_Info_1.LCWS_Fwd_Count>0 && Track_Info_2.LCWS_Fwd_Count>0 && Track_Info_3.LCWS_Fwd_Count>0) && (Track_Info_1.LCWS_Fwd_Count == Track_Info_3.LCWS_Fwd_Count))
    {
        Enable_Relay_1 = 1;
    }

    //if card1, card2, card3 > 0 && card1 == card3 - Rev count
    if((Track_Info_4.LCWS_Rev_Count>0 && Track_Info_3.LCWS_Rev_Count>0 && Track_Info_2.LCWS_Rev_Count>0) && (Track_Info_4.LCWS_Rev_Count == Track_Info_2.LCWS_Rev_Count))
    {
        Enable_Relay_2 = 1;
    }




    // if Card 1 == Card 2 or Card 1 == Card 2 + 1 (if Card 2 missed one count)
    if(((Track_Info_1.LCWS_Fwd_Count == (Track_Info_2.LCWS_Fwd_Count) + 1) && (Track_Info_1.LCWS_Fwd_Count > 0)) ||
    (Track_Info_1.LCWS_Fwd_Count == (Track_Info_2.LCWS_Fwd_Count))
    )
    {
        // If Card 1 ==  Card 3
        if(Track_Info_1.LCWS_Fwd_Count == Track_Info_3.LCWS_Fwd_Count)
        {
            Enable_Relay_1 = 1;
        }
        if(Track_Info_1.LCWS_Fwd_Count == Track_Info_4.LCWS_Fwd_Count)
        {
            Enable_Relay_1 = 1;
        }

        // If Card 1 ==  Card 2 && card2  = card3+2
        if((Track_Info_1.LCWS_Fwd_Count == Track_Info_2.LCWS_Fwd_Count) &&(Track_Info_2.LCWS_Fwd_Count == (Track_Info_3.LCWS_Fwd_Count +2)) && (Track_Info_3.LCWS_Fwd_Count>0))
        {
            Enable_Relay_1 = 1;
        }

        // If Card 1 ==  Card 3 + 1 (Card 3 missed one count)
        if((Track_Info_1.LCWS_Fwd_Count == (Track_Info_3.LCWS_Fwd_Count + 1)) && Track_Info_3.LCWS_Fwd_Count > 0)
        {
            if(Track_Info_2.LCWS_Fwd_Count > 0)
            {
                Enable_Relay_1 = 1;
                Enable_Variables_Clear = 0; // Do not clear the variables. because there is a possibility of one more count in card 3. as the train myy still going on on Card 3
            }
        }
        // card1 = card2 +1; card2 = card3 +1
        if((Track_Info_1.LCWS_Fwd_Count == (Track_Info_2.LCWS_Fwd_Count + 1)) && (Track_Info_2.LCWS_Fwd_Count == (Track_Info_3.LCWS_Fwd_Count + 1)))
        {
            if(Track_Info_1.LCWS_Fwd_Count>0 && Track_Info_2.LCWS_Fwd_Count>0 && Track_Info_3.LCWS_Fwd_Count>0)
            {
                Enable_Relay_1 = 1;
                Enable_Variables_Clear = 0; // Do not clear the variables. because there is a possibility of one more count in card 3. as the train myy still going on on Card 3
            }
        }
    }

    // If none of the above cases matched, if the train touched all the cards, clear the signal.
    // or if the variables are not cleared


//    {
//        Enable_Relay_1 = 1;
//    }





    // Reverse Direction

    // if Card 4 == Card 3 or Card 4 == Card 3 + 1 (if Card 3 missed one count)
    if(((Track_Info_4.LCWS_Rev_Count == (Track_Info_3.LCWS_Rev_Count) + 1) && (Track_Info_4.LCWS_Rev_Count > 0)) ||
    (Track_Info_4.LCWS_Rev_Count == (Track_Info_3.LCWS_Rev_Count))
    )
    {
        // If Card 4 ==  Card 2
        if(Track_Info_4.LCWS_Rev_Count == Track_Info_2.LCWS_Rev_Count)
        {
            Enable_Relay_2 = 1;
        }

        if(Track_Info_4.LCWS_Rev_Count == Track_Info_1.LCWS_Rev_Count)
        {
            Enable_Relay_2 = 1;
        }

        // If Card 4 ==  Card 3 && card3  = card2+2
        if((Track_Info_4.LCWS_Rev_Count == Track_Info_3.LCWS_Rev_Count) &&(Track_Info_3.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count +2)) && (Track_Info_2.LCWS_Rev_Count>0))
        {
            Enable_Relay_2 = 1;
        }

        // If Card 4 ==  Card 2 + 1 (Card 2 missed one count)
        if((Track_Info_4.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count + 1)) && Track_Info_2.LCWS_Rev_Count > 0)
        {
            if(Track_Info_3.LCWS_Rev_Count > 0)
            {
                Enable_Relay_2 = 1;
                Enable_Variables_Clear = 0; // Do not clear the variables. because there is a possibility of one more count in card 3. as the train myy still going on on Card 3
            }
        }
        if((Track_Info_4.LCWS_Rev_Count == (Track_Info_3.LCWS_Rev_Count + 1)) && (Track_Info_3.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count + 1)))
        {
            if(Track_Info_4.LCWS_Rev_Count>0 && Track_Info_3.LCWS_Rev_Count>0 && Track_Info_2.LCWS_Rev_Count>0)
            {
                Enable_Relay_2 = 1;
                Enable_Variables_Clear = 0; // Do not clear the variables. because there is a possibility of one more count in card 3. as the train myy still going on on Card 3
            }
        }
    }

    // If none of the above cases matched, if the train touched all the cards, clear the signal.
    // or if the variables are not cleared

    if((Track_Info_1.LCWS_Rev_Count > 0 && Track_Info_2.LCWS_Rev_Count > 0) && (Track_Info_3.LCWS_Rev_Count > 0 && Track_Info_4.LCWS_Rev_Count > 0))
    {
        Enable_Relay_2 = 1;
    }

    // For Shunting
    if(     (Track_Info_1.LCWS_Fwd_Count == Track_Info_1.LCWS_Rev_Count) &&
            (Track_Info_2.LCWS_Fwd_Count == Track_Info_2.LCWS_Rev_Count) &&
            (Track_Info_3.LCWS_Fwd_Count == Track_Info_3.LCWS_Rev_Count) &&
            (Track_Info_4.LCWS_Fwd_Count == Track_Info_4.LCWS_Rev_Count)
      )
    {
        Enable_Relay_1 = 1;
        Enable_Relay_2 = 1;
    }


    // Checking for Direct Out Count
    Track_Info_1.LCWS_Count = Track_Info_1.LCWS_Fwd_Count + Track_Info_1.LCWS_Rev_Count;
    Track_Info_2.LCWS_Count = Track_Info_2.LCWS_Fwd_Count + Track_Info_2.LCWS_Rev_Count;
    Track_Info_3.LCWS_Count = Track_Info_3.LCWS_Fwd_Count + Track_Info_3.LCWS_Rev_Count;
    Track_Info_4.LCWS_Count = Track_Info_4.LCWS_Fwd_Count + Track_Info_4.LCWS_Rev_Count;


    // if the counts are changing, then reset the health relay timer.
    if (Track_Info_1.LCWS_Count != Track_Info_1.LCWS_Prev_Count)
    {
        Track_Info_1.LCWS_Prev_Count = Track_Info_1.LCWS_Count;
        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
    }
    if (Track_Info_2.LCWS_Count != Track_Info_2.LCWS_Prev_Count)
    {
        Track_Info_2.LCWS_Prev_Count = Track_Info_2.LCWS_Count;
        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
    }
    if (Track_Info_3.LCWS_Count != Track_Info_3.LCWS_Prev_Count)
    {
        Track_Info_3.LCWS_Prev_Count = Track_Info_1.LCWS_Count;
        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
    }
    if (Track_Info_4.LCWS_Count != Track_Info_4.LCWS_Prev_Count)
    {
        Track_Info_4.LCWS_Prev_Count = Track_Info_4.LCWS_Count;
        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
    }



//
//    if(((Track_Info_1.LCWS_Count == 0 && Track_Info_4.LCWS_Count == 0) && (Track_Info_2.LCWS_Count > 0 || Track_Info_3.LCWS_Count > 0 ))
//    //((Track_Info_1.LCWS_Rev_Count == 0 && Track_Info_4.LCWS_Rev_Count == 0) && (Track_Info_2.LCWS_Rev_Count > 0 || Track_Info_3.LCWS_Rev_Count > 0 ))
//    )
//    {
//        Enable_Relay_1 = 0;
//        Enable_Relay_2 = 0;
//
//        /* no in-counts, only out-counts,system goes to error state*/
//        Declare_DAC_Defective();
//        Status.Flags.Direct_Out_Count = SET_HIGH;
//        Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
//        return;
//    }
    if (Enable_Relay_1 == 1 && Enable_Relay_2 == 1)
    {
        if(Enable_Variables_Clear == 1)
        {
            Track_Info_1.LCWS_Count = 0;
            Track_Info_2.LCWS_Count = 0;
            Track_Info_3.LCWS_Count = 0;
            Track_Info_4.LCWS_Count = 0;

            Track_Info_1.LCWS_Fwd_Count = 0;
            Track_Info_1.LCWS_Rev_Count = 0;

            Track_Info_2.LCWS_Fwd_Count = 0;
            Track_Info_2.LCWS_Rev_Count = 0;

            Track_Info_3.LCWS_Fwd_Count = 0;
            Track_Info_3.LCWS_Rev_Count = 0;

            Track_Info_4.LCWS_Fwd_Count = 0;
            Track_Info_4.LCWS_Rev_Count = 0;
        }

            if(Relay_B_State != 1)
            {
                /* Train cleared the section */
                uiAuthorisationKey = Get_Relay_Energising_Key();
                Energise_Vital_Relay_B(uiAuthorisationKey);
                //Relay_B_Info.State = NO_TRAIN_IN_SECTION;
            }
        if(FeedBack_Info[FB_VITAL_RELAY_B_ON_ID].State == FB_CHECK_IDLE) // If the requested F/B is already under check, no need to do anything
        {
            Start_FeedBack_Check(FB_VITAL_RELAY_B_ON_ID);
        }

        if(FeedBack_Info[DS_TRACK_STATUS_CHECK_ID].State == FB_CHECK_IDLE) // If the requested F/B is already under check, no need to do anything
        {
            Start_FeedBack_Check(DS_TRACK_STATUS_CHECK_ID);
        }
//            Start_FeedBack_Check(DS_TRACK_STATUS_CHECK_ID);
        DS_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;

        Clear_DS_AxleCount_1();
        Clear_DS_AxleCount_2();
        Clear_DS_AxleCount_3();
        Clear_DS_AxleCount_4();

        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
        B_Sec_Led_drive(ON);
    }
    else
    {
        if(Relay_B_State == 1)
        {
            DeEnergise_Vital_Relay_B();     /* from relaydrv.c */
        }
        B_Sec_Led_drive(OFF);
        DS_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
        Status.Flags.DS_Track_Status = TRACK_OCCUPIED;
        Track_Status_Info.Bit.Local_DS_Track_Status = TRACK_OCCUPIED;
        //Relay_B_Info.State = TRAIN_IN_SECTION;
    }




    /* Health Relay */
    Enable_Relay_1 = 1;
    if(LCWS_Section_TimeOut == 0)
    {
        Enable_Relay_1 = 0;
    }

    if (Enable_Relay_1 == 1)
    {

            /* Train cleared the section */
        if(Relay_B_PR_State == 0)
        {
            Energise_Preparatory_Relay_B();
            //Relay_A_Info.State = NO_TRAIN_IN_SECTION;
        }
        A_Sec_Led_drive(ON);
        if(FeedBack_Info[FB_STATUS_RELAY_B_ON_ID].State == FB_CHECK_IDLE) // If the requested F/B is already under check, no need to do anything
        {
            Start_FeedBack_Check(FB_STATUS_RELAY_B_ON_ID);
        }
    }
    else
    {
        /* Train Occupied the section */
        if(Relay_B_PR_State == 1)
        {
            DeEnergise_Preparatory_Relay_B();
        }
        A_Sec_Led_drive(OFF);
        //Relay_A_Info.State = TRAIN_IN_SECTION;
    }
}



BYTE Ready_to_clear_13_fwd =  0, Ready_to_clear_13_rev =  0, Ready_to_clear_24_fwd =  0 , Ready_to_clear_24_rev =  0;
UINT16 Count_13_fwd = 0,Count_13_rev = 0, Count_24_fwd = 0, Count_24_rev = 0;
BYTE Lane_13;
void Update_LCWS_DL_Relay_State(void)
{
    BYTE Enable_Relay_1, Enable_Relay_2;
    INT16 uiAuthorisationKey;

    BYTE Enable_Variables_Clear;
    Lane_13 =0;
    if (Status.Flags.System_Status != NORMAL)
    {
        if(Relay_B_PR_State == 1)
            {
                DeEnergise_Preparatory_Relay_B();     /* from relaydrv.c */
            }
            if(Relay_B_State == 1)
            {
                DeEnergise_Vital_Relay_B();     /* from relaydrv.c */
            }
        return;
    }


    Enable_Variables_Clear = 1;

    Enable_Relay_1 = 0;
    Enable_Relay_2 = 0;

    if(Ready_to_clear_13_fwd == 1 && Count_13_fwd < Track_Info_1.LCWS_Fwd_Count)
    {
        Track_Info_1.LCWS_Fwd_Count = Track_Info_1.LCWS_Fwd_Count - Count_13_fwd;
        Track_Info_3.LCWS_Fwd_Count = 0;
        Ready_to_clear_13_fwd = 0;
    }
    if(Ready_to_clear_13_rev == 1 && Count_13_rev < Track_Info_3.LCWS_Rev_Count)
    {
        Track_Info_3.LCWS_Rev_Count = Track_Info_3.LCWS_Rev_Count - Count_13_rev;
        Track_Info_1.LCWS_Rev_Count = 0;
        Ready_to_clear_13_rev = 0;
    }

    if(Ready_to_clear_24_fwd == 1 && Count_24_fwd < Track_Info_2.LCWS_Fwd_Count)
    {
        Track_Info_2.LCWS_Fwd_Count = Track_Info_2.LCWS_Fwd_Count - Count_24_fwd;
        Track_Info_4.LCWS_Fwd_Count = 0;
        Ready_to_clear_24_fwd = 0;
    }
    if(Ready_to_clear_24_rev == 1 && Count_24_rev < Track_Info_4.LCWS_Rev_Count)
    {
        Track_Info_4.LCWS_Rev_Count = Track_Info_4.LCWS_Rev_Count - Count_24_rev;
        Track_Info_2.LCWS_Rev_Count = 0;
        Ready_to_clear_24_rev = 0;
    }


    //2nd adjustment
    if(Ready_to_clear_13_fwd == 1 && Track_Info_3.LCWS_Rev_Count>0)
    {
        Track_Info_1.LCWS_Fwd_Count = 0;
        Track_Info_3.LCWS_Fwd_Count = 0;
        Ready_to_clear_13_fwd = 0;
    }
    if(Ready_to_clear_13_rev == 1 && Track_Info_1.LCWS_Fwd_Count>0)
    {
        Track_Info_3.LCWS_Rev_Count = 0;
        Track_Info_1.LCWS_Rev_Count = 0;
        Ready_to_clear_13_rev = 0;
    }

    if(Ready_to_clear_24_fwd == 1 && Count_24_fwd < Track_Info_4.LCWS_Rev_Count)
    {
        Track_Info_2.LCWS_Fwd_Count = 0;
        Track_Info_4.LCWS_Fwd_Count = 0;
        Ready_to_clear_24_fwd = 0;
    }
    if(Ready_to_clear_24_rev == 1 && Count_24_rev < Track_Info_2.LCWS_Fwd_Count)
    {
        Track_Info_4.LCWS_Rev_Count = 0;
        Track_Info_2.LCWS_Rev_Count = 0;
        Ready_to_clear_24_rev = 0;
    }

//
//    if(Rlyb_State == NO_TRAIN_IN_SECTION)
//    {
//      B_Sec_Led_drive(ON);
//    }
//    else
//    {
//      B_Sec_Led_drive(OFF);
//    }
// Reverse Direction
    if(((Track_Info_2.LCWS_Rev_Count > Track_Info_4.LCWS_Rev_Count) || (Track_Info_1.LCWS_Rev_Count > Track_Info_3.LCWS_Rev_Count) ))
    {
        Enable_Relay_1 = 0;
        Enable_Relay_2 = 0;

        /* no in-counts, only out-counts,system goes to error state*/
        Declare_DAC_Defective();
        Status.Flags.Direct_Out_Count = SET_HIGH;
        Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
        return;
    }

// Forward Direction
    if((((Track_Info_3.LCWS_Fwd_Count) > Track_Info_1.LCWS_Fwd_Count) || (Track_Info_4.LCWS_Fwd_Count > Track_Info_2.LCWS_Fwd_Count)))
    {
        Enable_Relay_1 = 0;
        Enable_Relay_2 = 0;

        /* no in-counts, only out-counts,system goes to error state*/
        Declare_DAC_Defective();
        Status.Flags.Direct_Out_Count = SET_HIGH;
        Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
        return;
    }

        //if card1, card3 > 0 && card1 == card3 +1 - Fwd count
    if((Track_Info_1.LCWS_Fwd_Count>0 && Track_Info_3.LCWS_Fwd_Count>0) && (Track_Info_1.LCWS_Fwd_Count == (Track_Info_3.LCWS_Fwd_Count+1)))
    {
        Enable_Relay_1 = 1;
        Enable_Relay_2 = 1;
        Enable_Variables_Clear = 0;
        Ready_to_clear_13_fwd = 1;
        Count_13_fwd = Track_Info_1.LCWS_Fwd_Count;
    }
    if((Track_Info_3.LCWS_Rev_Count>0 && Track_Info_1.LCWS_Rev_Count>0) && (Track_Info_3.LCWS_Rev_Count == (Track_Info_1.LCWS_Rev_Count+1)))
    {
        Enable_Relay_1 = 1;
        Enable_Relay_2 = 1;
        Enable_Variables_Clear = 0;
        Ready_to_clear_13_rev = 1;
        Count_13_rev = Track_Info_3.LCWS_Rev_Count;
    }

    //if card2 card4 > 0 && card2 == card4+1 - Rev count
    if((Track_Info_4.LCWS_Rev_Count>0 && Track_Info_2.LCWS_Rev_Count>0) && (Track_Info_4.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count+1)))
    {
        Enable_Relay_1 = 1;
        Enable_Relay_2 = 1;
        Enable_Variables_Clear = 0;
        Ready_to_clear_24_rev = 1;
        Count_24_rev = Track_Info_4.LCWS_Rev_Count;
    }

    //if card1, card2, card3 > 0 && card1 == card3 - Fwd count
    if((Track_Info_1.LCWS_Fwd_Count>0 && Track_Info_3.LCWS_Fwd_Count>0) && (Track_Info_1.LCWS_Fwd_Count == Track_Info_3.LCWS_Fwd_Count))
    {
        Enable_Relay_1 = 1;
        Enable_Relay_2 = 1;
    }
    if((Track_Info_3.LCWS_Rev_Count>0 && Track_Info_1.LCWS_Rev_Count>0) && (Track_Info_3.LCWS_Rev_Count == Track_Info_1.LCWS_Rev_Count))
    {
        Enable_Relay_1 = 1;
        Enable_Relay_2 = 1;
    }
    //if card1, card2, card3 > 0 && card1 == card3 - Rev count
    if((Track_Info_4.LCWS_Rev_Count>0 && Track_Info_2.LCWS_Rev_Count>0) && (Track_Info_4.LCWS_Rev_Count == Track_Info_2.LCWS_Rev_Count))
    {
        Enable_Relay_1 = 1;
        Enable_Relay_2 = 1;
    }




//    // if Card 1 == Card 2 or Card 1 == Card 2 + 1 (if Card 2 missed one count)
//    if(((Track_Info_1.LCWS_Fwd_Count == (Track_Info_2.LCWS_Fwd_Count) + 1) && (Track_Info_1.LCWS_Fwd_Count > 0)) ||
//    (Track_Info_1.LCWS_Fwd_Count == (Track_Info_2.LCWS_Fwd_Count)))
//    {
//        // If Card 1 ==  Card 3
//        if(Track_Info_1.LCWS_Fwd_Count == Track_Info_3.LCWS_Fwd_Count)
//        {
//            Enable_Relay_1 = 1;
//        }
//        if(Track_Info_1.LCWS_Fwd_Count == Track_Info_4.LCWS_Fwd_Count)
//        {
//            Enable_Relay_1 = 1;
//        }
//
//        // If Card 1 ==  Card 2 && card2  = card3+2
//        if((Track_Info_1.LCWS_Fwd_Count == Track_Info_2.LCWS_Fwd_Count) &&(Track_Info_2.LCWS_Fwd_Count == (Track_Info_3.LCWS_Fwd_Count +2)) && (Track_Info_3.LCWS_Fwd_Count>0))
//        {
//            Enable_Relay_1 = 1;
//        }
//
//        // If Card 1 ==  Card 3 + 1 (Card 3 missed one count)
//        if((Track_Info_1.LCWS_Fwd_Count == (Track_Info_3.LCWS_Fwd_Count + 1)) && Track_Info_3.LCWS_Fwd_Count > 0)
//        {
//            if(Track_Info_2.LCWS_Fwd_Count > 0)
//            {
//                Enable_Relay_1 = 1;
//                Enable_Variables_Clear = 0; // Do not clear the variables. because there is a possibility of one more count in card 3. as the train myy still going on on Card 3
//            }
//        }
//        // card1 = card2 +1; card2 = card3 +1
//        if((Track_Info_1.LCWS_Fwd_Count == (Track_Info_2.LCWS_Fwd_Count + 1)) && (Track_Info_2.LCWS_Fwd_Count == (Track_Info_3.LCWS_Fwd_Count + 1)))
//        {
//            if(Track_Info_1.LCWS_Fwd_Count>0 && Track_Info_2.LCWS_Fwd_Count>0 && Track_Info_3.LCWS_Fwd_Count>0)
//            {
//                Enable_Relay_1 = 1;
//                Enable_Variables_Clear = 0; // Do not clear the variables. because there is a possibility of one more count in card 3. as the train myy still going on on Card 3
//            }
//        }
//    }
//
//    // If none of the above cases matched, if the train touched all the cards, clear the signal.
//    // or if the variables are not cleared
//
//
////    {
////        Enable_Relay_1 = 1;
////    }
//
//
//
//
//
//    // Reverse Direction
//
//    // if Card 4 == Card 3 or Card 4 == Card 3 + 1 (if Card 3 missed one count)
//    if(((Track_Info_4.LCWS_Rev_Count == (Track_Info_3.LCWS_Rev_Count) + 1) && (Track_Info_4.LCWS_Rev_Count > 0)) ||
//    (Track_Info_4.LCWS_Rev_Count == (Track_Info_3.LCWS_Rev_Count))
//    )
//    {
//        // If Card 4 ==  Card 2
//        if(Track_Info_4.LCWS_Rev_Count == Track_Info_2.LCWS_Rev_Count)
//        {
//            Enable_Relay_2 = 1;
//        }
//
//        if(Track_Info_4.LCWS_Rev_Count == Track_Info_1.LCWS_Rev_Count)
//        {
//            Enable_Relay_2 = 1;
//        }
//
//        // If Card 4 ==  Card 3 && card3  = card2+2
//        if((Track_Info_4.LCWS_Rev_Count == Track_Info_3.LCWS_Rev_Count) &&(Track_Info_3.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count +2)) && (Track_Info_2.LCWS_Rev_Count>0))
//        {
//            Enable_Relay_2 = 1;
//        }
//
//        // If Card 4 ==  Card 2 + 1 (Card 2 missed one count)
//        if((Track_Info_4.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count + 1)) && Track_Info_2.LCWS_Rev_Count > 0)
//        {
//            if(Track_Info_3.LCWS_Rev_Count > 0)
//            {
//                Enable_Relay_2 = 1;
//                Enable_Variables_Clear = 0; // Do not clear the variables. because there is a possibility of one more count in card 3. as the train myy still going on on Card 3
//            }
//        }
//        if((Track_Info_4.LCWS_Rev_Count == (Track_Info_3.LCWS_Rev_Count + 1)) && (Track_Info_3.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count + 1)))
//        {
//            if(Track_Info_4.LCWS_Rev_Count>0 && Track_Info_3.LCWS_Rev_Count>0 && Track_Info_2.LCWS_Rev_Count>0)
//            {
//                Enable_Relay_2 = 1;
//                Enable_Variables_Clear = 0; // Do not clear the variables. because there is a possibility of one more count in card 3. as the train myy still going on on Card 3
//            }
//        }
//    }
//
//    // If none of the above cases matched, if the train touched all the cards, clear the signal.
//    // or if the variables are not cleared
//


//    if((Track_Info_1.LCWS_Rev_Count > 0 && Track_Info_2.LCWS_Rev_Count > 0) && (Track_Info_3.LCWS_Rev_Count > 0 && Track_Info_4.LCWS_Rev_Count > 0))
//    {
//        Enable_Relay_2 = 1;
//    }

    // For Shunting
    if(     (Track_Info_1.LCWS_Fwd_Count == Track_Info_1.LCWS_Rev_Count) &&
            (Track_Info_3.LCWS_Fwd_Count == Track_Info_3.LCWS_Rev_Count)
      )
    {
        Enable_Relay_1 = 1;
        Enable_Relay_2 = 1;
    }



    // Checking for Direct Out Count
    Track_Info_1.LCWS_Count = Track_Info_1.LCWS_Fwd_Count + Track_Info_1.LCWS_Rev_Count;
    Track_Info_2.LCWS_Count = Track_Info_2.LCWS_Fwd_Count + Track_Info_2.LCWS_Rev_Count;
    Track_Info_3.LCWS_Count = Track_Info_3.LCWS_Fwd_Count + Track_Info_3.LCWS_Rev_Count;
    Track_Info_4.LCWS_Count = Track_Info_4.LCWS_Fwd_Count + Track_Info_4.LCWS_Rev_Count;


    // if the counts are changing, then reset the health relay timer.
    if (Track_Info_1.LCWS_Count != Track_Info_1.LCWS_Prev_Count)
    {
        Track_Info_1.LCWS_Prev_Count = Track_Info_1.LCWS_Count;
        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
    }
    if (Track_Info_2.LCWS_Count != Track_Info_2.LCWS_Prev_Count)
    {
        Track_Info_2.LCWS_Prev_Count = Track_Info_2.LCWS_Count;
        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
    }
    if (Track_Info_3.LCWS_Count != Track_Info_3.LCWS_Prev_Count)
    {
        Track_Info_3.LCWS_Prev_Count = Track_Info_1.LCWS_Count;
        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
    }
    if (Track_Info_4.LCWS_Count != Track_Info_4.LCWS_Prev_Count)
    {
        Track_Info_4.LCWS_Prev_Count = Track_Info_4.LCWS_Count;
        LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
    }

            if(Track_Info_1.LCWS_Fwd_Count > 0)
            {
                if((Track_Info_1.LCWS_Fwd_Count > Track_Info_3.LCWS_Fwd_Count) && (Track_Info_1.LCWS_Fwd_Count > (Track_Info_3.LCWS_Fwd_Count + 1)))
                {
                    Lane_13 = 1;
                    //oc
                }
            }
            if(Track_Info_3.LCWS_Rev_Count >0)
            {
                if((Track_Info_3.LCWS_Rev_Count > Track_Info_1.LCWS_Rev_Count) && (Track_Info_3.LCWS_Rev_Count > (Track_Info_1.LCWS_Rev_Count + 1)))
                {
                    Lane_13 = 1;
                    //oc
                }
            }

    if(Track_Info_4.LCWS_Rev_Count > Track_Info_2.LCWS_Rev_Count)
    {
        Enable_Relay_1 = 0;
        Enable_Relay_2 = 0;
        if((Track_Info_4.LCWS_Rev_Count>0 && Track_Info_2.LCWS_Rev_Count>0) && (Track_Info_4.LCWS_Rev_Count == (Track_Info_2.LCWS_Rev_Count+1)))
        {
            if(Lane_13 != 1)
            {
                Enable_Relay_1 = 1;
                Enable_Relay_2 = 1;
                Enable_Variables_Clear = 0;
                Ready_to_clear_24_rev =1;
                Count_24_rev = Track_Info_4.LCWS_Rev_Count;
            }
        }
    }

    if(Track_Info_2.LCWS_Fwd_Count > Track_Info_4.LCWS_Fwd_Count)
    {
        Enable_Relay_1 = 0;
        Enable_Relay_2 = 0;
        if((Track_Info_4.LCWS_Fwd_Count>0 && Track_Info_2.LCWS_Fwd_Count>0) && (Track_Info_2.LCWS_Fwd_Count == (Track_Info_4.LCWS_Fwd_Count+1)))
        {
            if(Lane_13 != 1)
            {
                Enable_Relay_1 = 1;
                Enable_Relay_2 = 1;
                Enable_Variables_Clear = 0;
                Ready_to_clear_24_fwd = 1;
                Count_24_fwd = Track_Info_2.LCWS_Fwd_Count;
            }
        }
    }
//
//    if(((Track_Info_1.LCWS_Count == 0 && Track_Info_4.LCWS_Count == 0) && (Track_Info_2.LCWS_Count > 0 || Track_Info_3.LCWS_Count > 0 ))
//    //((Track_Info_1.LCWS_Rev_Count == 0 && Track_Info_4.LCWS_Rev_Count == 0) && (Track_Info_2.LCWS_Rev_Count > 0 || Track_Info_3.LCWS_Rev_Count > 0 ))
//    )
//    {
//        Enable_Relay_1 = 0;
//        Enable_Relay_2 = 0;
//
//        /* no in-counts, only out-counts,system goes to error state*/
//        Declare_DAC_Defective();
//        Status.Flags.Direct_Out_Count = SET_HIGH;
//        Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
//        return;
//    }



    if (Enable_Relay_1 == 1 && Enable_Relay_2 == 1)
    {
        if(Lane_13 == 0)
        {
            if(Enable_Variables_Clear == 1)
            {
                Track_Info_1.LCWS_Count = 0;
                Track_Info_2.LCWS_Count = 0;
                Track_Info_3.LCWS_Count = 0;
                Track_Info_4.LCWS_Count = 0;

                Track_Info_1.LCWS_Fwd_Count = 0;
                Track_Info_1.LCWS_Rev_Count = 0;

                Track_Info_2.LCWS_Fwd_Count = 0;
                Track_Info_2.LCWS_Rev_Count = 0;

                Track_Info_3.LCWS_Fwd_Count = 0;
                Track_Info_3.LCWS_Rev_Count = 0;

                Track_Info_4.LCWS_Fwd_Count = 0;
                Track_Info_4.LCWS_Rev_Count = 0;
            }

            if(Relay_B_State != 1)
            {
                /* Train cleared the section */
                uiAuthorisationKey = Get_Relay_Energising_Key();
                Energise_Vital_Relay_B(uiAuthorisationKey);
                //Relay_B_Info.State = NO_TRAIN_IN_SECTION;
            }
            if(FeedBack_Info[FB_VITAL_RELAY_B_ON_ID].State == FB_CHECK_IDLE) // If the requested F/B is already under check, no need to do anything
            {
                Start_FeedBack_Check(FB_VITAL_RELAY_B_ON_ID);
            }

            if(FeedBack_Info[DS_TRACK_STATUS_CHECK_ID].State == FB_CHECK_IDLE) // If the requested F/B is already under check, no need to do anything
            {
                Start_FeedBack_Check(DS_TRACK_STATUS_CHECK_ID);
            }
    //            Start_FeedBack_Check(DS_TRACK_STATUS_CHECK_ID);
            DS_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;

            Clear_DS_AxleCount_1();
            Clear_DS_AxleCount_2();
            Clear_DS_AxleCount_3();
            Clear_DS_AxleCount_4();

            LCWS_Section_TimeOut = LCWS_SECTION_OCCUPIED_TIMEOUT;
            B_Sec_Led_drive(ON);
        }
    }
    else
    {
        if(Relay_B_State == 1)
        {
            DeEnergise_Vital_Relay_B();     /* from relaydrv.c */
        }
        B_Sec_Led_drive(OFF);
        DS_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;
        Status.Flags.DS_Track_Status = TRACK_OCCUPIED;
        Track_Status_Info.Bit.Local_DS_Track_Status = TRACK_OCCUPIED;
        //Relay_B_Info.State = TRAIN_IN_SECTION;
    }




    /* Health Relay */
    Enable_Relay_1 = 1;
    Enable_Relay_2 = 1;
    if(LCWS_Section_TimeOut == 0)
    {
        Enable_Relay_1 = 0;
    }

    if (Enable_Relay_1 == 1)
    {

            /* Train cleared the section */
            if(Relay_B_PR_State == 0)
            {
                Energise_Preparatory_Relay_B();
            }
            //Relay_A_Info.State = NO_TRAIN_IN_SECTION;
        A_Sec_Led_drive(ON);
        if(FeedBack_Info[FB_STATUS_RELAY_B_ON_ID].State == FB_CHECK_IDLE) // If the requested F/B is already under check, no need to do anything
        {
            Start_FeedBack_Check(FB_STATUS_RELAY_B_ON_ID);
        }
    }
    else
    {
        /* Train Occupied the section */
        if(Relay_B_PR_State == 1)
        {
            DeEnergise_Preparatory_Relay_B();
        }
        A_Sec_Led_drive(OFF);
        //Relay_A_Info.State = TRAIN_IN_SECTION;
    }

//    if (Enable_Relay_2 == 1)
//    {
//
//            /* Train cleared the section */
//            if(Relay_B_PR_State == 0)
//            {
//                Energise_Preparatory_Relay_B();
//            }
//            //Relay_A_Info.State = NO_TRAIN_IN_SECTION;
//        A_Sec_Led_drive(ON);
//        if(FeedBack_Info[FB_STATUS_RELAY_B_ON_ID].State == FB_CHECK_IDLE) // If the requested F/B is already under check, no need to do anything
//        {
//            Start_FeedBack_Check(FB_STATUS_RELAY_B_ON_ID);
//        }
//    }
//    else
//    {
//        /* Train Occupied the section */
//        if(Relay_B_PR_State == 1)
//        {
//            DeEnergise_Preparatory_Relay_B();
//        }
//        A_Sec_Led_drive(OFF);
//        //Relay_A_Info.State = TRAIN_IN_SECTION;
//    }
}





void Update_DE_Relay_State(void)
{
    BYTE Enable_Relay_1;
    INT16 uiAuthorisationKey;


    if (Status.Flags.System_Status != NORMAL)
    {
        if(Relay_B_State == 1)
        {
            DeEnergise_Vital_Relay_B();     /* from relaydrv.c */
        }
        return;
    }

    Enable_Relay_1 = 0;

    if(Track_Info_1.LCWS_Rev_Count > Track_Info_1.LCWS_Fwd_Count)
    {
        /* no in-counts, only out-counts,system goes to error state*/
        Declare_DAC_Defective();
        Status.Flags.Direct_Out_Count = SET_HIGH;
        Set_Error_Status_Bit(DIRECT_OUT_ERROR_NUM);
        return;
    }



    // For Shunting
    if(Track_Info_1.LCWS_Fwd_Count == Track_Info_1.LCWS_Rev_Count)
    {
        Enable_Relay_1 = 1;
    }


    if (Enable_Relay_1 == 1)
    {

        Track_Info_1.LCWS_Count = 0;

        Track_Info_1.LCWS_Fwd_Count = 0;
        Track_Info_1.LCWS_Rev_Count = 0;

        /* Train cleared the section */
        if(Relay_B_State == 0)
        {
            uiAuthorisationKey = Get_Relay_Energising_Key();
            Energise_Vital_Relay_B(uiAuthorisationKey);
            //Relay_B_Info.State = NO_TRAIN_IN_SECTION;
        }

        DS_Section_Mode.Local_Unit = SYSTEM_CLEAR_MODE;

        Clear_DS_AxleCount_1();

        B_Sec_Led_drive(ON);
    }
    else
    {
        if(Relay_B_State == 1)
        {
            DeEnergise_Vital_Relay_B();     /* from relaydrv.c */
        }
        B_Sec_Led_drive(OFF);
        DS_Section_Mode.Local_Unit = SYSTEM_OCCUPIED_MODE;

        //Relay_B_Info.State = TRAIN_IN_SECTION;
    }

}
/*******************************************************************************
Component name      :RELAYMGR
Module Name         :void Update_Relay_B_Counts(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the Down stream axle direction, forward axle count,
                     reverse axle count.Process the Local direction and
                     counts and update the Relay B state


Allocated Requirements	: 	(SSDAC_SWRS_0035)

Design Requirements		:	SSDAC_DR_5192, SSDAC_DR_5342, SSDAC_DR_5343,SSDAC_DR_5380


Interfaces
    Calls           :   AXLE_MON.C-Get_DS_AxleDirection()
                        AXLE_MON.C-Get_DS_Fwd_AxleCount()
                        AXLE_MON.C-Get_DS_Rev_AxleCount()
                        AXLE_MON.C-Get_DS_AxleCount()
                        RLYB_MGR.C-Process_Relay_B_Local_Direction()
                        RLYB_MGR.C-Process_Relay_B_Local_AxleCount()
                        RLYB_MGR.C-Update_Relay_B_State()

    Called by       :   DAC_MAIN.C-Control_DAC_Type_SF()
                        DAC_MAIN.C-Control_DAC_Type_CF()

Input Variables                 Name                Type
    Global          :   None

    Local           :   uiFwd_Count                 UINT16
                        uiRev_Count                 UINT16
                        uiAxle_Count                UINT16
                        uchDir                      BYTE

Output Variables                Name                Type
    Global          :   None

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Get the downstream axle direction,forward axle counts,reverse axle counts and local axle clearing counts
                     2.Update the CPU1 and CPU2 axle direction and counts
                     3.Update the relay B state
*********************************************************************************/
void Update_Relay_B_Counts(void)
{
    UINT16 uiFwd_Count;
    UINT16 uiRev_Count;
    UINT16 uiAxle_Count;
    BYTE uchDir;

    uchDir      = Get_DS_AxleDirection();                       /* from axlemon.c */
    uiFwd_Count = Get_DS_Fwd_AxleCount();                       /* from axlemon.c */
    uiRev_Count = Get_DS_Rev_AxleCount();                       /* from axlemon.c */
    uiAxle_Count= Get_DS_AxleCount();                           /* from axlemon.c */
    Process_Relay_B_Local_Direction(uchDir,uiAxle_Count);       /* from rlyb_mgr.c */
    Process_Relay_B_Local_AxleCount(uiFwd_Count,uiRev_Count);   /* from rlyb_mgr.c */
    Update_Relay_B_State();                                     /* from rlyb_mgr.c */
}
//15_03_10
void Update_3S_Relay_B_Counts(void)
{
    UINT16 uiFwd_Count;
    UINT16 uiRev_Count;
    UINT16 uiAxle_Count;
    BYTE uchDir;

    uchDir      = Get_DS_AxleDirection();                       /* from axlemon.c */
    uiFwd_Count = Get_DS_Fwd_AxleCount();                       /* from axlemon.c */
    uiRev_Count = Get_DS_Rev_AxleCount();                       /* from axlemon.c */
    uiAxle_Count= Get_DS_AxleCount();                           /* from axlemon.c */
    Process_Relay_B_Local_Direction(uchDir,uiAxle_Count);       /* from rlyb_mgr.c */
    Process_Relay_B_Local_AxleCount(uiFwd_Count,uiRev_Count);   /* from rlyb_mgr.c */
    Update_3S_Relay_B_State();                                  /* from rlya_mgr.c */
}
/******************************************************************************
Component name      :RELAYMGR
Module Name         :void Update_Relay_D3_Counts(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the Down stream axle direction, forward axle count,
                     reverse axle count.Process the Local direction and
                     counts and update the Relay D3 state

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5194


Interfaces
    Calls           :   AXLE_MON.C-Get_DS_AxleDirection()
                        AXLE_MON.C-Get_DS_Fwd_AxleCount()
                        AXLE_MON.C-Get_DS_Rev_AxleCount()
                        AXLE_MON.C-Get_DS_AxleCount()
                        RLYD3_MGR.C-Process_D3_Local_Direction()
                        RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Update_Relay_D3_State()

    Called by       :   DAC_MAIN.C-Control_DAC_Type_D3()


Input Variables                 Name                Type
    Global          :   None

    Local           :   uiFwd_Count                 UINT16
                        uiRev_Count                 UINT16
                        uiAxle_Count                UINT16
                        uchDir                      BYTE

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Get the downstream axle direction,forward axle counts,reverse axle counts and local axle clearing counts
                     2.Update the CPU1 and CPU2 axle direction and counts
                     3.Update the relay D3 state
********************************************************************************/
void Update_Relay_D3_Counts(void)
{
    UINT16 uiFwd_Count;
    UINT16 uiRev_Count;
    UINT16 uiAxle_Count;
    BYTE uchDir;

    uchDir      = Get_DS_AxleDirection();                       /* from axlemon.c */
    uiFwd_Count = Get_DS_Fwd_AxleCount();                       /* from axlemon.c */
    uiRev_Count = Get_DS_Rev_AxleCount();                       /* from axlemon.c */
    uiAxle_Count= Get_DS_AxleCount();                           /* from axlemon.c */
    Process_D3_Local_Direction(uchDir,uiAxle_Count);            /* from rlyd3_mgr.c */
    Process_D3_Local_AxleCount(uiFwd_Count,uiRev_Count);        /* from rlyd3_mgr.c */
    Update_Relay_D3_State();                                    /* from rlyd3_mgr.c */
}

/******************************************************************************
Component name      :RELAYMGR
Module Name         :void Update_Relay_D4_Counts(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get the Down stream axle direction, forward axle count,
                     reverse axle count.Process the Local direction and
                     counts and update the Relay D3 state

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   AXLE_MON.C-Get_DS_AxleDirection()
                        AXLE_MON.C-Get_DS_Fwd_AxleCount()
                        AXLE_MON.C-Get_DS_Rev_AxleCount()
                        AXLE_MON.C-Get_DS_AxleCount()
                        RLYD3_MGR.C-Process_D3_Local_Direction()
                        RLYD3_MGR.C-Process_D3_Local_AxleCount()
                        RLYD3_MGR.C-Update_Relay_D3_State()

    Called by       :   DAC_MAIN.C-Control_DAC_Type_D3()


Input Variables                 Name                Type
    Global          :   None

    Local           :   uiFwd_Count                 UINT16
                        uiRev_Count                 UINT16
                        uiAxle_Count                UINT16
                        uchDir                      BYTE

Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.Get the downstream axle direction,forward axle counts,reverse axle counts and local axle clearing counts
                     2.Update the CPU1 and CPU2 axle direction and counts
                     3.Update the relay D3 state
********************************************************************************/
void Update_Relay_D4_Counts(void)
{
    UINT16 uiFwd_Count;
    UINT16 uiRev_Count;
    UINT16 uiAxle_Count;
    BYTE uchDir;

    uchDir      = Get_DS_AxleDirection();                       /* from axlemon.c */
    uiFwd_Count = Get_DS_Fwd_AxleCount();                       /* from axlemon.c */
    uiRev_Count = Get_DS_Rev_AxleCount();                       /* from axlemon.c */
    uiAxle_Count= Get_DS_AxleCount();                           /* from axlemon.c */
    Process_D4_Local_Direction(uchDir,uiAxle_Count);            /* from rlyd3_mgr.c */
    Process_D4_Local_AxleCount(uiFwd_Count,uiRev_Count);        /* from rlyd3_mgr.c */
    Update_Relay_D4_State();                                    /* from rlyd3_mgr.c */
}

//15_03_10

void Update_3S_Relay_Counts(void)
{
 switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
        case DAC_UNIT_TYPE_EF:
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_D3_A:
        case DAC_UNIT_TYPE_D3_B:
        case DAC_UNIT_TYPE_D3_C:
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
            break;
     case DAC_UNIT_TYPE_3D_SF:
        Update_Relay_B_Counts();
        Update_3S_Relay_A_Counts();                 /* from relaymgr.c */
        break;
     case DAC_UNIT_TYPE_3D_EF:
        Update_Relay_A_Counts();
        Update_3S_Relay_B_Counts();                 /* from relaymgr.c */
        break;
    }
}

/*******************************************************************************
Component name      :RELAYMGR
Module Name         :void Update_SF_Track_Status(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update SF Track status by getting the relay status

Allocated Requirements  :

Design Requirements     :	SSDAC_DR_5195

Interfaces
    Calls           :   RLYB_MGR.C-Get_Relay_B_State()
                        RELAYMGR.C-Display_Clear_Status()
                        RELAYMGR.C-Display_Occupied_Status()

    Called by       :   DAC_MAIN.C-Control_DAC_Type_SF()

Input Variables                 Name                Type
    Global          :   WAIT_FOR_PILOT_TRAIN        Enumerator
                        NO_TRAIN_IN_SECTION         Enumerator

    Local           :   Rlyb_State                  BYTE

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1. Get the Relay B state, if it is in less than or equal to "WAIT_FOR_PILOT_TRAIN"
                        state display "-P" on 7 segment display
                     2. If it is in "NO_TRAIN_IN_SECTION" state display "CL"
                        on 7 segment and turn on the Track clear Led else display
                        "OC" on 7 segment and turn on the track occupied Led

********************************************************************************/
void Update_SF_Track_Status(void)
{
    BYTE Rlyb_State;

    Rlyb_State = Get_Relay_B_State();
    if(Rlyb_State <= (BYTE)WAIT_FOR_PILOT_TRAIN)
    {
      return;
    }
    if(Rlyb_State == (BYTE)NO_TRAIN_IN_SECTION)
    {
      B_Sec_Led_drive(ON);
    }
    else
    {
      B_Sec_Led_drive(OFF);
    }
}
/*******************************************************************************
Component name      :RELAYMGR
Module Name         :void Update_CF_Track_Status(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the CF track status by getting the both the relay status

Allocated Requirements	:	(SSDAC_SWRS_0568), (SSDAC_SWRS_0527), (SSDAC_SWRS_0660), (SSDAC_SWRS_0760)	
							(SSDAC_SWRS_0628), (SSDAC_SWRS_0727)

Design Requirements		:	SSDAC_DR_5196				

Interfaces
    Calls           :   RLYA_MGR.C-Get_Relay_A_State()
                        RLYB_MGR.C-Get_Relay_B_State()
                        RELAYMGR.C-Display_Clear_Status()
                        RELAYMGR.C-Display_Occupied_Status()
                        RELAYMGR.C-Display_Semi_Occupied_Status()

    Called by       :   DAC_MAIN.C-Control_DAC_Type_CF()

Input Variables                     Name                Type
    Global          :   WAIT_FOR_PILOT_TRAIN            Enumerator
                        NO_TRAIN_IN_SECTION             Enumerator

    Local           :   Rlya_State                      BYTE
                        Rlyb_State                      BYTE


Output Variables                    Name                Type
    Global          :   None

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1. Get the Relay A and Relay B states
                     2. If both the Relay A and Relay B state is in less than or equal to "WAIT_FOR_PILOT_TRAIN"
                        state display "-P" on 7 segment
                     3. If both the Relay A and Relay B state is in "NO_TRAIN_IN_SECTION" state,
                        display "CL" on 7 segment and turn On the Track clear Led
                     4. If Relay A or Relay B state is in "NO_TRAIN_IN_SECTION" state
                        display "OC" on 7 segment and flash the Track occupied Led
                        else display ""OC" on 7 segment and Turn on the Track occupied Led

*******************************************************************************/
void Update_CF_Track_Status(void)
{
    BYTE Rlya_State,Rlyb_State;

    Rlya_State = Get_Relay_A_State();
    Rlyb_State = Get_Relay_B_State();

    if(Rlyb_State == (BYTE)NO_TRAIN_IN_SECTION)
    {
      B_Sec_Led_drive(ON);
    }
    else
    {
      B_Sec_Led_drive(OFF);
    }
    if(Rlya_State == (BYTE)NO_TRAIN_IN_SECTION)
    {
      A_Sec_Led_drive(ON);
    }
    else
    {
      A_Sec_Led_drive(OFF);
    }
}

/**********************************************************************************
Component name      :RELAYMGR
Module Name         :void Update_EF_Track_Status(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the EF Track status by getting the Relay status


Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   RLYA_MGR.C-Get_Relay_A_State()
                        RELAYMGR.C-Display_Clear_Status()
                        RELAYMGR.C-Display_Occupied_Status()


    Called by       :   DAC_MAIN.C-Control_DAC_Type_EF()

Input Variables                 Name                Type
    Global          :   WAIT_FOR_PILOT_TRAIN        Enumerator
                        NO_TRAIN_IN_SECTION         Enumerator

    Local           :   Rlya_State                  BYTE

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1. Get the Relay A state, if it is in less than or equal to "WAIT_FOR_PILOT_TRAIN"
                        state display "-P" on 7 segment display
                     2. If it is in "NO_TRAIN_IN_SECTION" state display "CL"
                        on 7 segment and Turn On the Track Clear Led else display
                        "OC" on 7 segment and Turn On the Track Occupied Led
***********************************************************************************/
void Update_EF_Track_Status(void)
{
    BYTE Rlya_State;

    Rlya_State = Get_Relay_A_State();
    if(Rlya_State <= (BYTE)WAIT_FOR_PILOT_TRAIN)
    {
      return;
    }
    if(Rlya_State == (BYTE)NO_TRAIN_IN_SECTION)
    {
      A_Sec_Led_drive(ON);
    }
    else
    {
      A_Sec_Led_drive(OFF);
    }
}

/******************************************************************************
Component name      :RELAYMGR
Module Name         :void B_Sec_Led_drive(Led_State Led_status)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :This function is used Drive the TRACK CLEAR and OCCUPIED Leds.
                     When no train in section Track Clear(Green) LED will glow.
                     When Train enters the section Track Occupied(Red) LED will glow.
                     In the IBS the two sections are there.When one section clear and
                     other is occupied then OCCUPIED LED in CF unit will be BLINKing always.

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5193


Interfaces
    Calls           :   None

    Called by       :   RELAYMGR.C-Display_Clear_Status()
                        RELAYMGR.C-Display_Occupied_Status()
                        RELAYMGR.C-Display_Semi_Occupied_Status()
                        RELAYMGR.C-Declare_DAC_Defective()
                        RELAYMGR.C-Declare_DAC_Defective_US()
                        RELAYMGR.C-Declare_DAC_Defective_DS()

Input Variables                     Name                Type
    Global          :   None

    Local           :   Timer                           static                          L
                        Led_State                       BYTE
                        Led_status                      BYTE

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        SET_LOW                   0
                                        SET_HIGH                  1
References          :

Derived Requirements:

********************************************************************************/
Led_State led_a_status=OFF;
Led_State led_b_status=OFF;

void B_Sec_Led_drive(Led_State Led_status)
{
    switch(Led_status)
    {
        case ON:
            if(led_b_status != ON)
            {
             SECTION_B_LED_OCCUPIED_PORT = SET_HIGH;
             SECTION_B_LED_CLEAR_PORT = SET_LOW;
             led_b_status = ON;
            }
             break;

        case OFF:
            if(led_b_status != OFF)
            {
              SECTION_B_LED_OCCUPIED_PORT = SET_LOW;
              SECTION_B_LED_CLEAR_PORT = SET_HIGH;
              led_b_status = OFF;
            }
             break;
    }
}


void A_Sec_Led_drive(Led_State Led_status)
{
    switch(Led_status)
    {
        case ON:
            if(led_a_status != ON)
            {
             SECTION_A_LED_OCCUPIED_PORT = SET_HIGH;
             SECTION_A_LED_CLEAR_PORT = SET_LOW;
             led_a_status = ON;
            }
             break;

        case OFF:
            if(led_a_status != OFF)
            {
                SECTION_A_LED_OCCUPIED_PORT = SET_LOW;
              SECTION_A_LED_CLEAR_PORT = SET_HIGH;
              led_a_status = OFF;
            }
             break;
        default:
            break;
    }
}
/**************************************************************************
Component name      :RELAYMGR
Module Name         :void Declare_DAC_Defective(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Set System status,US system status, DS system status flag to "CATASTROPHIC_ERROR".
                     Turn on the Track occupied Led.Clear the Local Reset Flag,Clear the ATC Local
                     relay A and relay B state.Stop Down stream and Up stream axle counting.DeEnegize the
                     Preparatory relay. Depending upon the configuration call Set_Relay_B_DAC defective
                     or Set_Relay_A_DAC_Defective(),Set_Relay_D3_DAC_Defective() and Set_Relay_DE_DAC_Defective()
                     functions to declare DAC defective in error condition

Allocated Requirements	:(SSDAC_SWRS_0449),(SSDAC_SWRS_0429), (SSDAC_SWRS_0421), (SSDAC_SWRS_0521)
						 (SSDAC_SWRS_0563), (SSDAC_SWRS_0225), (SSDAC_SWRS_0263), (SSDAC_SWRS_0234)
						(SSDAC_SWRS_0622), (SSDAC_SWRS_0721), (SSDAC_SWRS_0655),(SSDAC_SWRS_0755)
						(SSDAC_SWRS_0364), (SSDAC_SWRS_0330), (SSDAC_SWRS_0123)
Design Requirements		:	SSDAC_DR_5194						

Interfaces
    Calls           :   RELAYMGR.C-Led_drive()
                        RESET.C-Clear_Local_Reset_Flag()
                        RESET.c -   Clear_DS_Remote_Reset_Flag()
                        RESET.c -   Clear_US_Remote_Reset_Flag()
                        RLYA_MGR.C-Clear_ATC_Local_Relay_A_State()
                        RLYB_MGR.C-Clear_ATC_Local_Relay_B_State()
                        AXLE_MON.C-Stop_US_Axle_Counting()
                        AXLE_MON.C-Stop_DS_Axle_Counting()
                        RELAYDRV.C-DeEnergise_Preparatory_Relay_A()
                        RELAYDRV.C-DeEnergise_Preparatory_Relay_B()
                        RLYB_MGR.C-Set_Relay_B_DAC_Defective()
                        RLYA_MGR.C-Set_Relay_A_DAC_Defective()
                        RLYD3_MGR.C-Set_Relay_D3_DAC_Defective()
                        RLYDE_MGR.C-Set_Relay_DE_DAC_Defective()


    Called by       :   AXLE_MON.C-Initialise_AxleMon()
                        AXLE_MON.C-Validate_PD_Signals()
                        AXLE_MON.C-Determine_TrackState()
                        AXLE_MON.C-Detect_PD_Failures()
                        AXLE_MON.C-Chk_for_AxleCount_Completion()
                        COMM_SM.C-Process_Interprocess_Message()
                        COMM_SM.C-Check_Peer_Status()
                        RESTORE.C-Check_DAC_Boards_Runtime()
                        RLYD3_MGR.C-Update_Relay_D3_State()
                        RLYDE_MGR.C-Update_Relay_DE_State()
                        SYS_MON.C-Read_FeedBack_Signal_Port()
                        SYS_MON.C-Check_Communication_Links_SF1()
                        SYS_MON.C-Check_Communication_Links_SF2()
                        SYS_MON.C-Check_Communication_Links_SF2()
                        SYS_MON.C-Check_Communication_Links_EF1()
                        SYS_MON.C-Check_Communication_Links_EF2()
                        SYS_MON.C-Check_Inter_Processor_Link()


Input Variables                     Name                Type
    Global          :       ON                          Enumerator
                            OFF                         Enumerator
                            DIP_Switch_Info.DAC_Unit_Type   Enumerator
                            SYSTEM_ERROR_MODE           Enumerator
                            DP3_MODE                    Enumerator

    Local           :   None

Output Variables                    Name                    Type
    Global          :   Status.Flags.System_Status          Bit
                        Status.Flags.US_System_Status       Bit
                        Status.Flags.DS_System_Status       Bit
                        DS_Section_Mode.Local_Unit          BYTE
                        DS_Section_Mode.Remote_Unit         BYTE
                        US_Section_Mode.Local_Unit          BYTE

    Local           :   None
Signal Variables

Macros defined      :                   Macro                   Value
                                        CATASTROPHIC_ERROR        0

References          :

Derived Requirements:

Algorithm           :1.Set the system status flag,upstream system flag,downstream system flag as CATASTROPIC ERROR
                     2.Turn off clear LED
                     3.Clear local reset and reset2 flags
                     4.Clear downstream and upstream remote reset flags
                     5.stop both DS and US axle counting
                     6.Find the unit type and if it is start fed then declare DAC defective,deenergise both vital relay A and B,
                       denergise the Preparatory relay B,update the downstream local unit status as error mode
                     7.If it is end fed then declare DAC defective,deenergise both vital relay A and B,
                       denergise the Preparatory relay A,update the upstream local unit status as error mode
                     8.if it is centre fed then declare DAC defective,deenergise both vital relay A and B,
                       denergise the Preparatory relay B and B,update the downstream and upstream local unit status as error mode
                     9.If it is UNIT A or B or C then declare DAC defective,deenergise both vital relay B,
                       update the downstream local unit status as error mode and remote unit as DP3 mode
****************************************************************************/

void Declare_DAC_Defective(void)
{
    Status.Flags.System_Status = CATASTROPHIC_ERROR;
    Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
    Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
    A_Sec_Led_drive(OFF);
    B_Sec_Led_drive(OFF);
    Clear_Local_Reset_Flag();
    Clear_Local_Reset2_Flag();
    Stop_US_Axle_Counting();
    Stop_DS_Axle_Counting();

    
    
     Clear_DS_Remote_Reset_Flag();
     Clear_US_Remote_Reset_Flag();
     Clear_ATC_Local_Relay_A_State();
     Clear_ATC_Local_Relay_B_State();
        switch (DIP_Switch_Info.DAC_Unit_Type)
        {
            case DAC_UNIT_TYPE_SF:
                Set_Relay_B_DAC_Defective();            /* from rlyb_mgr.c */
                /* drop Prep relay in the failure condition */
                DeEnergise_Preparatory_Relay_B();           /* from relaydrv.c */
                DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                break;
            case DAC_UNIT_TYPE_EF:
                Set_Relay_A_DAC_Defective();            /* from rlya_mgr.c */
                /* drop Prep relay in the failure condition */
                DeEnergise_Preparatory_Relay_A();           /* from relaydrv.c */
                US_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                break;
            case DAC_UNIT_TYPE_CF:
            case DAC_UNIT_TYPE_3D_SF:
            case DAC_UNIT_TYPE_3D_EF:
                Set_Relay_A_DAC_Defective();            /* from rlya_mgr.c */
                Set_Relay_B_DAC_Defective();            /* from rlyb_mgr.c */
                /* drop Prep relay in the failure condition */
                DeEnergise_Preparatory_Relay_A();           /* from relaydrv.c */
                DeEnergise_Preparatory_Relay_B();           /* from relaydrv.c */
                DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                US_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                break;
            case DAC_UNIT_TYPE_D3_A:
                Set_Relay_D3_DAC_Defective();           /* from rlyd3_mgr.c */
                DeEnergise_Preparatory_Relay_B();           /* from relaydrv.c */
                DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                DS_Section_Mode.Remote_Unit = DP3_MODE;
                break;
            case DAC_UNIT_TYPE_D3_B:
                Set_Relay_D3_DAC_Defective();           /* from rlyd3_mgr.c */
                DeEnergise_Preparatory_Relay_B();           /* from relaydrv.c */
                DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                DS_Section_Mode.Remote_Unit = DP3_MODE;
                break;
            case DAC_UNIT_TYPE_D3_C:
                Set_Relay_D3_DAC_Defective();           /* from rlyd3_mgr.c */
                DeEnergise_Preparatory_Relay_B();           /* from relaydrv.c */
                DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                DS_Section_Mode.Remote_Unit = DP3_MODE;
                break;
            case DAC_UNIT_TYPE_D4_A:
            case DAC_UNIT_TYPE_D4_B:
            case DAC_UNIT_TYPE_D4_C:
            case DAC_UNIT_TYPE_D4_D:
                Set_Relay_D4_DAC_Defective();           /* from rlyd3_mgr.c */
                DeEnergise_Preparatory_Relay_B();           /* from relaydrv.c */
                DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                DS_Section_Mode.Remote_Unit = DP3_MODE;
                break;
            case DAC_UNIT_TYPE_LCWS:
            case DAC_UNIT_TYPE_LCWS_DL:
                Set_Relay_A_DAC_Defective();            /* from rlyb_mgr.c */
                Set_Relay_B_DAC_Defective();            /* from rlyb_mgr.c */
                DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                US_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                break;
            case DAC_UNIT_TYPE_DE:
                Set_Relay_B_DAC_Defective();            /* from rlyb_mgr.c */
                DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                US_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                break;
            default:
                break;
        }
    

}
//01/09/10

/************************************************************************
Component name      :RELAYMGR
Module Name         :void Declare_DAC_Defective_US(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Turn on the track occupied Led, Clear the Local Reset Flag, ATC local relay A state.
                     Stop Up stream axle counting.DeEnerize the Preparatory relay.Depending upon the configuration
                     call Set_Relay_B_DAC defective or Set_Relay_A_DAC_Defective(),Set_Relay_D3_DAC_Defective()
                     and Set_Relay_DE_DAC_Defective() functions to declare DAC defective in error condition.
                     Set System status,US system status, DS system status flag to "CATASTROPHIC_ERROR".

Allocated Requirements	: (SSDAC_SWRS_0449),(SSDAC_SWRS_0429), (SSDAC_SWRS_0421)
							(SSDAC_SWRS_0521),(SSDAC_SWRS_0622), (SSDAC_SWRS_0721)
							(SSDAC_SWRS_0563), (SSDAC_SWRS_0655),(SSDAC_SWRS_0755)
							(SSDAC_SWRS_0364), (SSDAC_SWRS_0330)
Design Requirements		:	SSDAC_DR_5195	

Interfaces
    Calls           :   RELAYMGR.C-Led_drive()
                        RESET.C-Clear_Local_Reset_Flag()
                        RESET.c - Clear_US_Remote_Reset_Flag()
                        RLYA_MGR.C-Clear_ATC_Local_Relay_A_State()
                        AXLE_MON.C-Stop_US_Axle_Counting()
                        AXLE_MON.C-Stop_DS_Axle_Counting()
                        RELAYDRV.C-DeEnergise_Preparatory_Relay_A()
                        RLYA_MGR.C-Set_Relay_A_DAC_Defective()
                        RLYB_MGR.C-Set_Relay_B_DAC_Defective()
                        RLYB_MGR.C-Set_Relay_D3_DAC_Defective()


    Called by       :   COMM_US.C-Update_US_Sch_State()
                        COMM_US.C-Process_US_Reset_Info_Message()
                        COMM_US.C-Check_US_Unit_Status()
                        RLYA_MGR.C-Update_Relay_A_State()
                        SYS_MON.C-Read_FeedBack_Signal_Port()
                        SYS_MON.C-Check_Communication_US_CF1()
                        SYS_MON.C-Check_Communication_US_CF2()
                        SYS_MON.C-Check_US_Power_Status()




Input Variables                 Name                        Type
    Global          :   ON                                  Enumerator
                        OFF                                 Enumerator
                        DIP_Switch_Info.DAC_Unit_Type           Enumerator
                        Status.Flags.DS_System_Status       Enumerator
                        SYSTEM_ERROR_MODE                   Enumerator
                        DP3_MODE                            Enumerator


    Local           :       None

Output Variables                    Name                Type
    Global          :    Status.Flags.System_Status     Bit
                         Status.Flags.US_System_Status  Bit
                         Status.Flags.DS_System_Status  Bit
                         US_Section_Mode.Local_Unit     BYTE
                         DS_Section_Mode.Local_Unit     BYTE
                         DS_Section_Mode.Remote_Unit    BYTE

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        CATASTROPHIC_ERROR        0
                                        TRUE                      1
References          :

Derived Requirements:

Algorithm           :1.Turn off clear LED
                     2.Clear local reset and reset2 flags
                     3.Clear upstream remote reset flag
                     4.stop US axle counting and deenergise preparatory relay A
                     6.Find the unit type and if it is start fed then set the system status flag,upstream system flag,
                        downstream system flag as CATASTROPIC ERROR,declare DAC defective,deenergise both vital relay A and B,
                     7.If it is end fed then set the system status flag,upstream system flag,downstream system flag as CATASTROPIC ERROR,
                        declare DAC defective,deenergise both vital relay A and B,update the upstream local unit status as error mode
                     8.If it is centre fed then check the IBS dip switch status.
                        If it is high declare DAC defective,deenergise both vital relay A and B,set the system status flag,upstream system flag,
                        downstream system flag as CATASTROPIC ERROR otherwise deenergise vital relay A only,set system status flag as LOW if the DS status flag is
                        setted as LOW.
                       update the upstream local unit status as error mode
                     9.If it is UNIT A or B or C then declare DAC defective,deenergise both vital relay B,
                       update the downstream local unit status as error mode and remote unit as DP3 mode,stop the DS axle counting
**************************************************************************/
void Declare_DAC_Defective_US(void)
{
    A_Sec_Led_drive(OFF);
    Clear_Local_Reset_Flag();
    Clear_US_Remote_Reset_Flag();
    Clear_ATC_Local_Relay_A_State();
    Stop_US_Axle_Counting();

    /* drop Prep relay in the failure condition */
    DeEnergise_Preparatory_Relay_A();           /* from relaydrv.c */

        switch (DIP_Switch_Info.DAC_Unit_Type)
        {
            case DAC_UNIT_TYPE_SF:
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
                 Set_Relay_B_DAC_Defective();           /* from rlyb_mgr.c */
                break;
            case DAC_UNIT_TYPE_EF:
                 Set_Relay_A_DAC_Defective();           /* from rlya_mgr.c */
                // Set_Relay_B_DAC_Defective();         /* from rlyb_mgr.c */
                 Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
                 //Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 US_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                 break;
            case DAC_UNIT_TYPE_CF:
            case DAC_UNIT_TYPE_3D_SF:
            case DAC_UNIT_TYPE_3D_EF:
                 Set_Relay_A_DAC_Defective();           /* from rlya_mgr.c */
                 if(Status.Flags.DS_System_Status == CATASTROPHIC_ERROR)
                  {
                   Status.Flags.System_Status = CATASTROPHIC_ERROR;
                  }
                  Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
                  US_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                 break;
            case DAC_UNIT_TYPE_D3_A:
                 Set_Relay_D3_DAC_Defective();          /* from rlyb_mgr.c */
                 DeEnergise_Preparatory_Relay_B();
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
                 DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                 DS_Section_Mode.Remote_Unit = DP3_MODE;
                 Stop_DS_Axle_Counting();
                 B_Sec_Led_drive(OFF);
                 break;
            case DAC_UNIT_TYPE_D3_B:
                 Set_Relay_D3_DAC_Defective();          /* from rlyb_mgr.c */
                 DeEnergise_Preparatory_Relay_B();
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
                 DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                 DS_Section_Mode.Remote_Unit = DP3_MODE;
                 Stop_DS_Axle_Counting();
                 B_Sec_Led_drive(OFF);
                 break;
            case DAC_UNIT_TYPE_D3_C:
                 Set_Relay_D3_DAC_Defective();          /* from rlyb_mgr.c */
                 DeEnergise_Preparatory_Relay_B();
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
                 DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                 DS_Section_Mode.Remote_Unit = DP3_MODE;
                 Stop_DS_Axle_Counting();
                 B_Sec_Led_drive(OFF);
                 break;
            case DAC_UNIT_TYPE_D4_A:
            case DAC_UNIT_TYPE_D4_B:
            case DAC_UNIT_TYPE_D4_C:
            case DAC_UNIT_TYPE_D4_D:
                 Set_Relay_D4_DAC_Defective();          /* from rlyb_mgr.c */
                 DeEnergise_Preparatory_Relay_B();
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
                 DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
                 DS_Section_Mode.Remote_Unit = DP3_MODE;
                 Stop_DS_Axle_Counting();
                 B_Sec_Led_drive(OFF);
                 break;
            case DAC_UNIT_TYPE_LCWS:
            case DAC_UNIT_TYPE_LCWS_DL:
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Set_Relay_A_DAC_Defective();           /* from rlya_mgr.c */
                 Set_Relay_B_DAC_Defective();           /* from rlyb_mgr.c */
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
                 break;
            case DAC_UNIT_TYPE_DE:
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Set_Relay_B_DAC_Defective();           /* from rlyb_mgr.c */
                 Status.Flags.System_Status = CATASTROPHIC_ERROR;
                 Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
                 break;
            default:
                 break;
        }

}

/*****************************************************************************
Component name      :RELAYMGR
Module Name         :void Declare_DAC_Defective_DS(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Turn on the track occupied Led, Clear the Local Reset Flag, ATC local relay B state.
                     Stop Down stream axle counting.DeEnerize the Preparatory relay.Depending upon the configuration
                     call Set_Relay_B_DAC defective or Set_Relay_A_DAC_Defective(),Set_Relay_D3_DAC_Defective()
                     and Set_Relay_DE_DAC_Defective() functions to declare DAC defective in error condition.
                     Set System status,US system status, DS system status flag to "CATASTROPHIC_ERROR".


Allocated Requirements	: 	(SSDAC_SWRS_0225), (SSDAC_SWRS_0263)
							(SSDAC_SWRS_0234), (SSDAC_SWRS_0521),(SSDAC_SWRS_0622), (SSDAC_SWRS_0721)
							(SSDAC_SWRS_0563), (SSDAC_SWRS_0655),(SSDAC_SWRS_0755)
							(SSDAC_SWRS_0364), (SSDAC_SWRS_0330)


Design Requirements		:	SSDAC_DR_5196

Interfaces
    Calls           :   RELAYMGR.C-Led_drive()
                        RESET.C-Clear_Local_Reset_Flag()
                        RESET.c - Clear_DS_Remote_Reset_Flag()
                        RLYB_MGR.C-Clear_ATC_Local_Relay_B_State()
                        AXLE_MON.C-Stop_DS_Axle_Counting()
                        Axle_MON.C-Stop_US_Axle_Counting()
                        RELAYDRV.C-DeEnergise_Preparatory_Relay_B()
                        RLYA_MGR.C-Set_Relay_A_DAC_Defective()
                        RLYB_MGR.C-Set_Relay_B_DAC_Defective()
                        RLYB_MGR.C-Set_Relay_D3_DAC_Defective()


    Called by       :   COMM_DS.C-Update_DS_Sch_State()
                        COMM_DS.C-Process_DS_Reset_Info_Message()
                        COMM_DS.C-Check_DS_Unit_Status()
                        RLYB_MGR.C-Update_Relay_B_State()
                        SYS_MON.C-Read_FeedBack_Signal_Port()
                        SYS_MON.C-Check_Communication_DS_CF1()
                        SYS_MON.C-Check_Communication_DS_CF2()
                        SYS_MON.C-Check_DS_Power_Status()

Input Variables                         Name                Type
    Global          :   ON                                  Enumerator
                        OFF                                 Enumerator
                        Status.Flags.US_System_Status       Enumerator
                        SYSTEM_ERROR_MODE                   Enumerator
                        DP3_MODE                            Enumerator

    Local           :   None

Output Variables                    Name                Type
    Global          :    Status.Flags.System_Status     Bit
                         Status.Flags.US_System_Status  Bit
                         Status.Flags.DS_System_Status  Bit
                         DS_Section_Mode.Local_Unit     BYTE
                         DS_Section_Mode.Remote_Unit    BYTE

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        CATASTROPHIC_ERROR        0
                                        TRUE                      1
References          :

Derived Requirements:

Algorithm           :1.Turn off clear LED
                     2.Clear local reset and reset2 flags
                     3.Clear upstream remote reset flag
                     4.stop DS axle counting and deenergise preparatory relay B
                     6.Find the unit type and if it is start fed then set the system status flag,upstream system flag,
                        downstream system flag as CATASTROPIC ERROR,declare DAC defective,deenergise both vital relay A and B,
                        update the downstream local unit status as error mode
                     7.If it is end fed then set the system status flag,upstream system flag,downstream system flag as CATASTROPIC ERROR,
                        declare DAC defective,deenergise both vital relay A and B.
                     8.If it is centre fed then check the IBS dip switch status.
                        If it is high declare DAC defective,deenergise both vital relay A and B,set the system status flag,upstream system flag,
                        downstream system flag as CATASTROPIC ERROR otherwise deenergise vital relay B only,set system status flag as LOW if the US status flag is
                        setted as LOW.
                       update the downstream local unit status as error mode
                     9.If it is UNIT A or B or C then declare DAC defective,deenergise both vital relay B,
                       update the downstream local unit status as error mode and remote unit as DP3 mode,stop the DS axle counting
********************************************************************************/
void Declare_DAC_Defective_DS(void)
{
    B_Sec_Led_drive(OFF);
    Clear_Local_Reset2_Flag();
    Clear_DS_Remote_Reset_Flag();
    Clear_ATC_Local_Relay_B_State();
    Stop_DS_Axle_Counting();

    /* drop Prep relay in the failure condition */
    DeEnergise_Preparatory_Relay_B();           /* from relaydrv.c */


    switch (DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_SF:
             //Set_Relay_A_DAC_Defective();         /* from rlya_mgr.c */
             Set_Relay_B_DAC_Defective();           /* from rlyb_mgr.c */
             Status.Flags.System_Status = CATASTROPHIC_ERROR;
             //Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
             Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
             DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
             break;
        case DAC_UNIT_TYPE_EF:
             Status.Flags.System_Status = CATASTROPHIC_ERROR;
             Set_Relay_A_DAC_Defective();           /* from rlya_mgr.c */
             Set_Relay_B_DAC_Defective();           /* from rlyb_mgr.c */
             Status.Flags.System_Status = CATASTROPHIC_ERROR;
             Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
             Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
             break;
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
             Set_Relay_B_DAC_Defective();           /* from rlyb_mgr.c */
             if(Status.Flags.US_System_Status == CATASTROPHIC_ERROR)
              {
                Status.Flags.System_Status = CATASTROPHIC_ERROR;
              }
              Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
              DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
              break;
        case DAC_UNIT_TYPE_D3_A:
            Set_Relay_D3_DAC_Defective();           /* from rlyb_mgr.c */
            Status.Flags.System_Status = CATASTROPHIC_ERROR;
            Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
            Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
            DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
            DS_Section_Mode.Remote_Unit = DP3_MODE;
            Stop_US_Axle_Counting();
            break;
        case DAC_UNIT_TYPE_D3_B:
            Set_Relay_D3_DAC_Defective();           /* from rlyb_mgr.c */
            Status.Flags.System_Status = CATASTROPHIC_ERROR;
            Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
            Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
            DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
            DS_Section_Mode.Remote_Unit = DP3_MODE;
            Stop_US_Axle_Counting();
            break;
        case DAC_UNIT_TYPE_D3_C:
            Set_Relay_D3_DAC_Defective();           /* from rlyb_mgr.c */
            Status.Flags.System_Status = CATASTROPHIC_ERROR;
            Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
            Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
            DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
            DS_Section_Mode.Remote_Unit = DP3_MODE;
            Stop_US_Axle_Counting();
            break;
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            Set_Relay_D4_DAC_Defective();           /* from rlyb_mgr.c */
            Status.Flags.System_Status = CATASTROPHIC_ERROR;
            Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
            Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
            DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
            DS_Section_Mode.Remote_Unit = DP3_MODE;
            Stop_US_Axle_Counting();
            break;
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
             Status.Flags.System_Status = CATASTROPHIC_ERROR;
             Set_Relay_A_DAC_Defective();           /* from rlya_mgr.c */
             Set_Relay_B_DAC_Defective();           /* from rlyb_mgr.c */
             Status.Flags.System_Status = CATASTROPHIC_ERROR;
             Status.Flags.US_System_Status = CATASTROPHIC_ERROR;
             Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
             break;
        case DAC_UNIT_TYPE_DE:
             Status.Flags.System_Status = CATASTROPHIC_ERROR;
             Set_Relay_B_DAC_Defective();           /* from rlyb_mgr.c */
             Status.Flags.System_Status = CATASTROPHIC_ERROR;
             Status.Flags.DS_System_Status = CATASTROPHIC_ERROR;
             break;
        default:
            break;
    }
}



