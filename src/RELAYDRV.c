/*******************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RELAYDRV
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
    Functions   :   INT16 Get_Relay_Energising_Key(void)
                    void Energise_Preparatory_Relay_A(void)
                    void Energise_Preparatory_Relay_B(void)
                    void Energise_Vital_Relay_A(INT16 iKey)
                    void Energise_Vital_Relay_B(INT16 iKey)
                    void DeEnergise_Preparatory_Relay_A(void)]
                    void DeEnergise_Preparatory_Relay_B(void)
                    void DeEnergise_Vital_Relay_A(void)
                    void DeEnergise_Vital_Relay_B(void)
                    void Energise_Relay_A(void)
                    void DeEnergise_Relay_A(void)
                    void Energise_Relay_B(void)
                    void DeEnergise_Relay_B(void)

*******************************************************************************/
#include <xc.h>
#include <stdlib.h>


#include "COMMON.h"
#include "RELAYDRV.h"
#include "SYS_MON.h"


relay_security_info_t Security;
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;

void Energise_Relay_A(void);
void DeEnergise_Relay_A(void);
void Energise_Relay_B(void);
void DeEnergise_Relay_B(void);


BYTE Relay_B_State = 1;
BYTE Relay_B_PR_State = 1;
BYTE ISO_ctrl_A =0,ISO_ctrl_B=0; 
/******************************************************************************
Component name      :RELAYDRV
Module Name         :INT16 Get_Relay_Energising_Key(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Get Security Key

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5177

Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.C-Update_Relay_A_State()
                        RLYB_MGR.C-Update_Relay_B_State()
                        RLYD3_MGR.C-Update_Relay_D3_State()
                        RLYDE_MGR.C-Update_Relay_DE_State()

Input Variables                     Name                Type
    Global          :               None

    Local           :               None

Output Variables                    Name                Type
    Global          :           Security.Key            INT16

    Local           :           None
Signal Variables

Macros defined      :                   Macro                   Value
                                        None                    None
References          :
Derived Requirements:
*******************************************************************************/
INT16 Get_Relay_Energising_Key(void)
{
    return (Security.Key);
}

/******************************************************************************
Component name      :RELAYDRV
Module Name         :void Energise_Preparatory_Relay_A(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Energise the preparatory relay A,change the SEED ,get new Key and
                     feedback of relay is checked

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Compiler defined-srand()
                        Compiler defined-rand()
:
                        SYS_MON.C-Start_FeedBack_Check()

    Called by       :   RELAYMGR.C-Start_Relay_Mgr()
                        RLYA_MGR.C-Update_Relay_A_State()


Input Variables                     Name                    Type
    Global          :   Security.Rand_Generator_Seed        INT16

    Local           :   FB_STATUS_RELAY_A_ON_ID             Enumerator

Output Variables                    Name                    Type
    Global          :   Security.Rand_Generator_Seed        INT16
                        Security.Key                        INT16

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                TURN_ON_PREPARATORY_RELAY        1

References          :

Derived Requirements:

Algorithm           :1.Increment the rand generator seed for giving the next no of the sequence
                       which is passed to srand function as the argument.
                     2.If the rand generator seed is negative than initialize it to zero to have positive
                       no sequence
                     3.The srand() function initializes the random number generator accessed
                       by rand() with the given seed. This provides a mechanism for varying the
                       starting point of the pseudo-random sequence yielded by rand().Through this
                       we are Changing the SEED
                     4.The rand() function is a pseudo-random number generator. It returns an integer
                       in the range 0 to 32767, which changes in a pseudo-random fashion on each call.
                       By calling this function we areGenerating new KEY
                     5.After changing the seed & generating the new key initialize the preparatory relay
                       port to ON state
                     6.Check the feedback signal to ensure that preparatory relay A  has energised


*******************************************************************************/
void Energise_Preparatory_Relay_A(void)
{
    if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE)
    {
        return;
    }

    Security.Rand_Generator_Seed = Security.Rand_Generator_Seed + 1;
    if (Security.Rand_Generator_Seed <= 0)
    {
        Security.Rand_Generator_Seed = 0;
    }
    srand((UINT16)Security.Rand_Generator_Seed);    /* Change the SEED */
    Security.Key = rand();                  /* Generate new KEY */
//  PREPARATORY_A_ENABLE_PORT = TURN_ON_PREPARATORY_RELAY;
    PREPARATORY_RELAY_A_PORT = TURN_ON_PREPARATORY_RELAY;
    LATGbits.LATG7 = 1;
    ISO_ctrl_A = 1;

    Start_FeedBack_Check(FB_STATUS_RELAY_A_ON_ID);
}

/******************************************************************************
Component name      :RELAYDRV
Module Name         :void Energise_Preparatory_Relay_B(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Energise the preparatory relay B,change the SEED ,get new Key and
                     feedback of relay is checked

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Compiler defined-srand()
                        Compiler defined-rand()
:
                        SYS_MON.C-Start_FeedBack_Check()

    Called by       :   RELAYMGR.C-Start_Relay_Mgr()
                        RLYB_MGR.C- Update_Relay_B_State()

Input Variables                     Name                    Type
    Global          :   Security.Rand_Generator_Seed        INT16

    Local           :   FB_STATUS_RELAY_B_ON_ID             Enumerator

Output Variables                    Name                    Type
    Global          :   Security.Rand_Generator_Seed        INT16
                        Security.Key                        INT16

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                TURN_ON_PREPARATORY_RELAY        1

References          :

Derived Requirements:

Algorithm           :1.Increment the rand generator seed for giving the next no of the sequence
                       which is passed to srand function as the argument.
                     2.If the rand generator seed is negative than initialize it to zero to have positive
                       no sequence
                     3.The srand() function initializes the random number generator accessed
                       by rand() with the given seed. This provides a mechanism for varying the
                       starting point of the pseudo-random sequence yielded by rand().Through this
                       we are Changing the SEED
                     4.The rand() function is a pseudo-random number generator. It returns an integer
                       in the range 0 to 32767, which changes in a pseudo-random fashion on each call.
                       By calling this function we areGenerating new KEY
                     5.After changing the seed & generating the new key initialize the preparatory relay
                       port to ON state
                     6.Check the feedback signal to ensure that preparatory relay has energised


*******************************************************************************/
void Energise_Preparatory_Relay_B(void)
{
    if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE)
    {
        return;
    }


    Security.Rand_Generator_Seed = Security.Rand_Generator_Seed + 1;
    if (Security.Rand_Generator_Seed <= 0)
    {
        Security.Rand_Generator_Seed = 0;
    }
    srand((UINT16)Security.Rand_Generator_Seed);    /* Change the SEED */
    Security.Key = rand();                  /* Generate new KEY */
//  PREPARATORY_B_ENABLE_PORT = TURN_ON_PREPARATORY_RELAY;
    PREPARATORY_RELAY_B_PORT = TURN_ON_PREPARATORY_RELAY;
    LATGbits.LATG7 = 1;
    ISO_ctrl_B = 1;

    Relay_B_PR_State = 1;
    Start_FeedBack_Check(FB_STATUS_RELAY_B_ON_ID);
}

/******************************************************************************
Component name      :RELAYDRV
Module Name         :void Energise_Vital_Relay_A(INT16 iKey)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :This fuction calls Energise the vital relay A and start Feedback
                     check of Vital relay A.This function return TRUE
                     if the Security key is correct else FALSE

Allocated Requirements	:	(SSDAC_SWRS_0456), (SSDAC_SWRS_0327)	

Design Requirements		:	SSDAC_DR_5180, SSDAC_DR_5321, SSDAC_DR_5322, SSDAC_DR_5323

Interfaces
    Calls           :   RELAYDRV.C-Energise_Relay_A()
                        SYS_MON.C-Start_FeedBack_Check()

    Called by       :   RLYA_MGR.C-Update_Relay_A_State()

Input Variables                 Name                Type
    Global          :   Security.Key                INT16

    Local           :   iKey                        INT16
                        FB_VITAL_RELAY_A_ON_ID      Enumerator
                        US_TRACK_STATUS_CHECK_ID    Enumerator

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.To energise the vital relay A, a check shall be performed that the security
                     key generated are same.
                     2.If security keys are same than its safe to energise the relay else its a invalid key
                       hence just return with out energising the relay A
                     3.Check the feedback to ensure that vital relay A is ON & to know the US track status

*******************************************************************************/

void Energise_Vital_Relay_A(INT16 iKey)
{

    if (iKey != Security.Key)
    {
        return;             /* Invalid Key */
    }
    Energise_Relay_A();
    Start_FeedBack_Check(FB_VITAL_RELAY_A_ON_ID);
    Start_FeedBack_Check(US_TRACK_STATUS_CHECK_ID);
    return;
}
/******************************************************************************
Component name      :RELAYDRV
Module Name         :void Energise_Vital_Relay_B(INT16 iKey)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :This fuction calls Energise the vital relay B and start Feedback check of
                     Vital relay B.This function return TRUE if the Security key is correct else FALSE

Allocated Requirements	: 	(SSDAC_SWRS_0270), (SSDAC_SWRS_0377), (SSDAC_SWRS_0328)

Design Requirements		:	SSDAC_DR_5181, SSDAC_DR_5324, SSDAC_DR_5325, SSDAC_DR_5326
						

Interfaces
    Calls           :RELAYDRV.C-Energise_Relay_B()
                     SYS_MON.C-Start_FeedBack_Check()

    Called by       :RLYB_MGR.C-Update_Relay_B_State()
                     RLYD3_MGR.C-Update_Relay_D3_State()
                     RLYDE_MGR.C-Update_Relay_DE_State()

Input Variables                 Name                Type
    Global          :   Security.Key                INT16

    Local           :   iKey                        INT16
                        FB_VITAL_RELAY_B_ON_ID      Enumerator
                        DS_TRACK_STATUS_CHECK_ID    Enumerator

Output Variables                Name                Type
    Global          :   None

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

Algorithm           :1.To energise the vital relay B, a check shall be performed that the security
                       key generated are same.
                     2.If security keys are same than its safe to energise the relay else its a invalid key
                       hence just return with out energising the relay B
                     3.Check the feedback to ensure that vital relay B is ON & to check the status of the
                       DS track because vital relay B may be a repeat relay for the next block
*******************************************************************************/
void Energise_Vital_Relay_B(INT16 iKey)
{
    if (iKey != Security.Key)
    {
        return;             /* Invalid Key */
    }
    
        Energise_Relay_B();
    
    Start_FeedBack_Check(FB_VITAL_RELAY_B_ON_ID);
    Start_FeedBack_Check(DS_TRACK_STATUS_CHECK_ID);
    return;
}
/******************************************************************************
Component name      :RELAYDRV
Module Name         :void DeEnergise_Preparatory_Relay_A(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Deenergise the preporatory relay

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Compiler defined-srand()
                        Compiler defined-rand()
:
                        SYS_MON.C-Start_FeedBack_Check()

    Called by       :   RELAYMGR.C-Initialise_Relay_Mgr()
                        RELAYMGR.C-Declare_DAC_Defective()
                        RELAYMGR.C-Declare_DAC_Defective_US()
                        RELAYMGR.C-Declare_DAC_Defective_DS()
                        RLYA_MGR.C-Update_Relay_A_State()

Input Variables                     Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16

    Local           :   FB_STATUS_RELAY_A_OFF_ID            Enumerator

Output Variables                    Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16
                        Security.Key                    INT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                TURN_OFF_PREPARATORY_RELAY        0
References          :

Derived Requirements:

Algorithm           :1.Increment the rand generator seed for giving the next no of the sequence
                       which is passed to srand function as the argument.
                     2.If the rand generator seed is negative than initialize it to zero to have
                       positive no sequence
                     3.The srand() function initializes the random number generator accessed
                       by rand() with the given seed. This provides a mechanism for varying the
                       starting point of the pseudo-random sequence yielded by rand().Through this
                       we are Changing the SEED
                     4.The rand() function is a pseudo-random number generator. It returns an integer
                       in the range 0 to 32767, which changes in a pseudo-random fashion on each call.
                       By calling this function we areGenerating new KEY
                     5.After changing the seed & generating the new key initialize the preparatory relay
                       port to OFF state
                     6.Check the feedback signal to ensure that preparatory relay A has been de-energised

*******************************************************************************/
void DeEnergise_Preparatory_Relay_A(void)
{
    if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE)
    {
        return;
    }

    
    Security.Rand_Generator_Seed = Security.Rand_Generator_Seed + 1;
    if (Security.Rand_Generator_Seed <= 0)
    {
        Security.Rand_Generator_Seed = 0;
    }
    srand((UINT16)Security.Rand_Generator_Seed);    /* Change the SEED */
    Security.Key = rand();                  /* Generate new KEY */
    PREPARATORY_RELAY_A_PORT = TURN_OFF_PREPARATORY_RELAY;


//  PREPARATORY_A_ENABLE_PORT = TURN_OFF_PREPARATORY_RELAY;
    Start_FeedBack_Check(FB_STATUS_RELAY_A_OFF_ID);
}

/******************************************************************************
Component name      :RELAYDRV
Module Name         :void DeEnergise_Preparatory_Relay_B(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Deenergise the preporatory relay B

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Compiler defined-srand()
                        Compiler defined-rand()
:
                        SYS_MON.C-Start_FeedBack_Check()

    Called by       :   RELAYMGR.C-Initialise_Relay_Mgr()
                        RELAYMGR.C-Declare_DAC_Defective()
                        RELAYMGR.C-Declare_DAC_Defective_US()
                        RELAYMGR.C-Declare_DAC_Defective_DS()
                        RLYB_MGR.C-Update_Relay_B_State()

Input Variables                     Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16

    Local           :   FB_STATUS_RELAY_B_OFF_ID            Enumerator

Output Variables                    Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16
                        Security.Key                    INT16

    Local           :   None
Signal Variables


Macros defined      :                   Macro                   Value
                                TURN_OFF_PREPARATORY_RELAY        0
References          :

Derived Requirements:

Algorithm           :1.Increment the rand generator seed for giving the next no of the sequence
                       which is passed to srand function as the argument.
                     2.If the rand generator seed is negative than initialize it to zero to have
                       positive no sequence
                     3.The srand() function initializes the random number generator accessed
                       by rand() with the given seed. This provides a mechanism for varying the
                       starting point of the pseudo-random sequence yielded by rand().Through this
                       we are Changing the SEED
                     4.The rand() function is a pseudo-random number generator. It returns an integer
                       in the range 0 to 32767, which changes in a pseudo-random fashion on each call.
                       By calling this function we areGenerating new KEY
                     5.After changing the seed & generating the new key initialize the preparatory relay
                       port to OFF state
                     6.Check the feedback signal to ensure that preparatory relay B has been de-energised

*******************************************************************************/
void DeEnergise_Preparatory_Relay_B(void)
{
    if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE)
    {
        return;
    }

    Security.Rand_Generator_Seed = Security.Rand_Generator_Seed + 1;
    if (Security.Rand_Generator_Seed <= 0)
    {
        Security.Rand_Generator_Seed = 0;
    }
    srand((UINT16)Security.Rand_Generator_Seed);    /* Change the SEED */
    Security.Key = rand();                  /* Generate new KEY */
    PREPARATORY_RELAY_B_PORT = TURN_OFF_PREPARATORY_RELAY;

    Relay_B_PR_State = 0;
    Start_FeedBack_Check(FB_STATUS_RELAY_B_OFF_ID);
}
/******************************************************************************
Component name      :RELAYDRV
Module Name         :void DeEnergise_Vital_Relay_A(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :De-energise the Vital relay A, check the feedback signal to ensure that
                     vital relay A is in OFF state & also check the status of the US track because
                     vital relay A may be repeat relay of next block

Allocated Requirements	:	(SSDAC_SWRS_0454), (SSDAC_SWRS_0369)

Design Requirements		:	SSDAC_DR_5183, SSDAC_DR_5329, SSDAC_DR_5330, SSDAC_DR_5331
	    	
Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Relay_A()
                        SYS_MON.C-Start_FeedBack_Check()

    Called by       :   RLYA_MGR.C-Update_Relay_A_State()
                        RLYA_MGR.C-Set_Relay_A_DAC_Defective()

Input Variables                     Name                Type
    Global          :   FB_VITAL_RELAY_A_OFF_ID         Enumerator
                        US_TRACK_STATUS_CHECK_ID        Enumerator


    Local           :   None

Output Variables                    Name                Type
    Global          :   None

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:


*******************************************************************************/
void DeEnergise_Vital_Relay_A(void)
{
    DeEnergise_Relay_A();
    Start_FeedBack_Check(FB_VITAL_RELAY_A_OFF_ID);
    Start_FeedBack_Check(US_TRACK_STATUS_CHECK_ID);
 }
/******************************************************************************
Component name      :RELAYDRV
Module Name         :void DeEnergise_Vital_Relay_B(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :De-energise the vital relay B, check the feedback signal to ensure that
                     vital relay B is in OFF state & also check the status of the DS track because
                     vital relay B may be repeat relay of next block

Allocated Requirements	: 	(SSDAC_SWRS_0268)

Design Requirements		:	SSDAC_DR_5184, SSDAC_DR_5332, SSDAC_DR_5333

Interfaces
    Calls           :   RELAYDRV.C-DeEnergise_Relay_B()
                        SYS_MON.C-Start_FeedBack_Check()

    Called by       :   RLYB_MGR.C-Update_Relay_B_State()
                        RLYB_MGR.C-Set_Relay_B_DAC_Defective()
                        RLYD3_MGR.C-Update_Relay_D3_State()
                        RLYDE_MGR.C-Update_Relay_DE_State()

Input Variables                 Name                Type
    Global          :   None

    Local           :   FB_VITAL_RELAY_B_OFF_ID     Enumerator
                        DS_TRACK_STATUS_CHECK_ID    Enumerator


Output Variables                Name                Type
    Global          :   None

    Local           :   None
Signal Variables



Macros defined      :                   Macro                   Value
                                        None                    None
References          :

Derived Requirements:

*******************************************************************************/
void DeEnergise_Vital_Relay_B(void)
{
    DeEnergise_Relay_B();
    Start_FeedBack_Check(FB_VITAL_RELAY_B_OFF_ID);
    Start_FeedBack_Check(DS_TRACK_STATUS_CHECK_ID);
}
/******************************************************************************
Component name      :RELAYDRV
Module Name         :void Energise_Relay_A(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Energise the Vital relay A,Change the SEED and Generate New Key

Allocated Requirements	: 	(SSDAC_SWRS_0006), (SSDAC_SWRS_0274), (SSDAC_SWRS_0426), (SSDAC_SWRS_0327)

Design Requirements		:	SSDAC_DR_5185, SSDAC_DR_5334, SSDAC_DR_5335

Interfaces
    Calls           :   Compiler defined-srand()
                        Compiler defined-rand()
:

    Called by       :   RELAYDRV.C-Energise_Vital_Relay_A()

Input Variables                     Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16

    Local           :   None

Output Variables                    Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16
                        Security.Key                    INT16
                        CCPR2                           Enhanced Capture/Compare/PWM Register
                        CCP2CON                         Enhanced Capture/Compare/PWM Control Register

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        PWM_DUTY_CYCLE          21
                                        CPU1_TURN_ON_PWM        0b00101100
                                        CPU2_TURN_ON_PWM

References          :

Derived Requirements:

Algorithm           :1.Increment the rand generator seed for giving the next no of the sequence
                       which is passed to srand function as the argument.
                     2.If the rand generator seed is negative than initialize it to zero to have
                       positive no sequence
                     3.The srand() function initializes the random number generator accessed
                       by rand() with the given seed. This provides a mechanism for varying the
                       starting point of the pseudo-random sequence yielded by rand().Through this
                       we are Changing the SEED
                     4.The rand() function is a pseudo-random number generator. It returns an integer
                       in the range 0 to 32767, which changes in a pseudo-random fashion on each call.
                       By calling this function we areGenerating new KEY
                     5.After changing the seed & generating the new key turn ON the vital relay A


*******************************************************************************/
void Energise_Relay_B(void)
{
    Security.Rand_Generator_Seed = Security.Rand_Generator_Seed + 1;
    if (Security.Rand_Generator_Seed <= 0)
    {
        Security.Rand_Generator_Seed = 1;
    }
    srand((UINT16)Security.Rand_Generator_Seed);    /* Change the SEED */
    Security.Key = rand();                  /* Generate new KEY */
//  if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
//  {
     //CCPR2 = PWM_DUTY_CYCLE_A;                  /* Turn ON Vital Relay-A */
     //CCP2CON = TURN_ON_PWM_A;
    Relay_B_State = 1;
    ISO_ctrl_B = 1;
    LATGbits.LATG7 = 1; // To Enable Isolated Power Supply
    OC1CON1bits.OCM = TURN_ON_PWM_A;


//  }
//  else
//  {
//   CCPR2 = CPU2_PWM_DUTY_CYCLE;                   /* Turn ON Vital Relay-A */
//   CCP2CON = CPU2_TURN_ON_PWM;
//  }
//    return;
}
/******************************************************************************
Component name      :RELAYDRV
Module Name         :void DeEnergise_Relay_A(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Deenergise the relay A,Change the SEED and Get New Key,Turn OFF the PWM

Allocated Requirements	: 	(SSDAC_SWRS_0006)

Design Requirements		:	SSDAC_DR_5186, SSDAC_DR_5336, SSDAC_DR_5337

Interfaces
    Calls           :   Compiler defined-srand()
                        Compiler defined-rand()
:

    Called by       :   RELAYDRV.C-DeEnergise_Vital_Relay_A()


Input Variables                     Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16

    Local           :   None

Output Variables                    Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16
                        Security.Key                    INT16
                        CCP2CON                         Enhanced Capture/Compare/PWM Control Register

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        CPU1_TURN_OFF_PWM       0b00100000
                                        CPU2_TURN_OFF_PWM
References          :

Derived Requirements:

Algorithm           :1.Increment the rand generator seed for giving the next no of the sequence
                       which is passed to srand function as the argument.
                     2.If the rand generator seed is negative than initialize it to zero to have
                       positive no sequence
                     3.The srand() function initializes the random number generator accessed
                       by rand() with the given seed. This provides a mechanism for varying the
                       starting point of the pseudo-random sequence yielded by rand().Through this
                       we are Changing the SEED
                     4.The rand() function is a pseudo-random number generator. It returns an integer
                       in the range 0 to 32767, which changes in a pseudo-random fashion on each call.
                       By calling this function we areGenerating new KEY
                     5.After changing the seed & generating the new key turn OFF the vital relay A
*******************************************************************************/
void DeEnergise_Relay_B(void)
{
    Security.Rand_Generator_Seed = Security.Rand_Generator_Seed + 1;
    if (Security.Rand_Generator_Seed <= 0)
    {
        Security.Rand_Generator_Seed = 0;
    }
    srand((UINT16)Security.Rand_Generator_Seed);    /* Change the SEED */
    Security.Key = rand();                  /* Generate new KEY */
//  if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
//  {
    //CCP2CON = TURN_OFF_PWM_A;                   /* Turn OFF Vital Relay-A */
    Relay_B_State = 0;
    ISO_ctrl_B = 0;
    if(ISO_ctrl_A == 0 && ISO_ctrl_B == 0)
    {
        LATGbits.LATG7 = 0; // To Disable Isolated Power Supply
    }
    
    OC1CON1bits.OCM = TURN_OFF_PWM_A;
//  }
//  else
//  {
//   CCP2CON = CPU2_TURN_OFF_PWM;                   /* Turn OFF Vital Relay-A */
//  }
}
/******************************************************************************
Component name      :RELAYDRV
Module Name         :void Energise_Relay_B(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Energise the Vital relay B,Change the SEED and Generate New Key

Allocated Requirements	: 	(SSDAC_SWRS_0006), (SSDAC_SWRS_0132), (SSDAC_SWRS_0526),
							(SSDAC_SWRS_0527), (SSDAC_SWRS_0426), 
							(SSDAC_SWRS_0230), (SSDAC_SWRS_0460), (SSDAC_SWRS_0627), (SSDAC_SWRS_0726)
							(SSDAC_SWRS_0628), (SSDAC_SWRS_0727)
							(SSDAC_SWRS_0377), (SSDAC_SWRS_0328)

Design Requirements		:	    	SSDAC_DR_5187, SSDAC_DR_5338, SSDAC_DR_5339

Interfaces
    Calls           :   Compiler defined-srand()
                        Compiler defined-rand()
:

    Called by       :   RELAYDRV.C-DeEnergise_Vital_Relay_A()


Input Variables                     Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16

    Local           :   None

Output Variables                    Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16
                        Security.Key                    INT16
                        CCP2CON                         Enhanced Capture/Compare/PWM Control Register

    Local           :   None

Signal Variables


Macros defined      :                   Macro                   Value
                                        CPU1_TURN_ON_PWM        0b00100000
                                        CPU2_TURN_ON_PWM
References          :

Derived Requirements:

Algorithm           :1.Increment the rand generator seed for giving the next no of the sequence
                       which is passed to srand function as the argument.
                     2.If the rand generator seed is negative than initialize it to zero to have
                       positive no sequence
                     3.The srand() function initializes the random number generator accessed
                       by rand() with the given seed. This provides a mechanism for varying the
                       starting point of the pseudo-random sequence yielded by rand().Through this
                       we are Changing the SEED
                     4.The rand() function is a pseudo-random number generator. It returns an integer
                       in the range 0 to 32767, which changes in a pseudo-random fashion on each call.
                       By calling this function we areGenerating new KEY
                     5.After changing the seed & generating the new key turn ON the vital relay B
*******************************************************************************/
void Energise_Relay_A(void)
{
    Security.Rand_Generator_Seed = Security.Rand_Generator_Seed + 1;
    if (Security.Rand_Generator_Seed <= 0)
    {
        Security.Rand_Generator_Seed = 0;
    }
    srand((UINT16)Security.Rand_Generator_Seed);    /* Change the SEED */
    Security.Key = rand();                  /* Generate new KEY */
//  if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
//  {
     //CCPR4 = PWM_DUTY_CYCLE_B;                  /* Turn ON Vital Relay-B */
     //CCP4CON = TURN_ON_PWM_B;

    ISO_ctrl_A = 1;
    LATGbits.LATG7 = 1; // To Enable Isolated Power Supply
    OC2CON1bits.OCM = TURN_ON_PWM_B;
//  }
//  else
//  {
//   CCPR4 = CPU2_PWM_DUTY_CYCLE;                   /* Turn ON Vital Relay-B */
//   CCP4CON = CPU2_TURN_ON_PWM;
//  }
//    return;
}
/******************************************************************************
Component name      :RELAYDRV
Module Name         :void DeEnergise_Relay_B(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :De-energise the Relay B,Change the SEED,Get New Key,Turn OFF PWM

Allocated Requirements	: 	(SSDAC_SWRS_0006)

Design Requirements		:	SSDAC_DR_5188, SSDAC_DR_5340, SSDAC_DR_5341    	

Interfaces
    Calls           :   Compiler defined-srand()
                        Compiler defined-rand()
:


    Called by       :   RELAYDRV.C-DeEnergise_Vital_Relay_B()

Input Variables                     Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16

    Local           :   None

Output Variables                    Name                Type
    Global          :   Security.Rand_Generator_Seed    INT16
                        Security.Key                    INT16
                        CCP4CON                         Enhanced Capture/Compare/PWM Control Register


    Local           :None
Signal Variables



Macros defined      :                   Macro                   Value
                                        CPU1_TURN_OFF_PWM       0b00100000
                                        CPU2_TURN_OFF_PWM
References          :

Derived Requirements:

Algorithm           :1.Increment the rand generator seed for giving the next no of the sequence
                       which is passed to srand function as the argument.
                     2.If the rand generator seed is negative than initialize it to zero to have
                       positive no sequence
                     3.The srand() function initializes the random number generator accessed
                       by rand() with the given seed. This provides a mechanism for varying the
                       starting point of the pseudo-random sequence yielded by rand().Through this
                       we are Changing the SEED
                     4.The rand() function is a pseudo-random number generator. It returns an integer
                       in the range 0 to 32767, which changes in a pseudo-random fashion on each call.
                       By calling this function we areGenerating new KEY
                     5.After changing the seed & generating the new key turn OFF the vital relay B

*******************************************************************************/
void DeEnergise_Relay_A(void)
{
    Security.Rand_Generator_Seed = Security.Rand_Generator_Seed + 1;
    if (Security.Rand_Generator_Seed <= 0)
    {
        Security.Rand_Generator_Seed = 0;
    }
    srand((UINT16)Security.Rand_Generator_Seed);    /* Change the SEED */
    Security.Key = rand();                  /* Generate new KEY */
//  if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
//  {
    //CCP4CON = TURN_OFF_PWM_B;                   /* Turn OFF Vital Relay-B */
    ISO_ctrl_A = 0;
    if(ISO_ctrl_A == 0 && ISO_ctrl_B == 0)   // If both the relays are off, then switch off the iso line
    {
        LATGbits.LATG7 = 0; // To Disable Isolated Power Supply
    }
    OC2CON1bits.OCM = TURN_OFF_PWM_B;
//  }
//  else
//  {
//   CCP4CON = CPU2_TURN_OFF_PWM;                   /* Turn OFF Vital Relay-B */
//  }
}
