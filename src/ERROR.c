/*****************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   ERROR
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
    Functions   :   void Initialise_Error()
                    void Update_Display_Error()
                    void Set_Error_Status_Byte ( BYTE  Major_Err_Id, BYTE status_byte)
                    void Set_Error_Status_Bit (BYTE Error_No)
                    void Clear_Error_Display(void)
                    void Decrement_Err_display_50msTmr()
                    void Update_Error_Code()
                    BYTE Get_DS_Error_Code()
                    BYTE Get_US_Error_Code()
                    void Clear_DS_Error_Code()
                    void Clear_US_Error_Code()

****************************************************************/
#include "COMMON.h"
#include "ERROR.h"



major_err  Major_Err;                               /*structure to hold the major error bits*/
minor_err  Minor_Err;                               /*structure to hold the minor error bits*/
error_display Error_Display;                        /*structure to hold the display state,error code,position,timeout*/
error_code Error_Code;                              /*structure to hold the down and up section error codes*/
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from DAC_MAIN.c */
extern  /*near*/  dac_status_t Status;                    /* from dac_main.c */



extern const BYTE BitMask_List[8];


/*const unsigned char Error_Table[TOTAL_NO_ERRORS][NO_OF_CHARACTERS_PER_LINE] = {
        "PD1 Card Missing",     "PD2 Card Missing",     "SM CPU Missing",
        "Modem Card Missing",   "RelayA Drive Missing", "RelayB Drive Missing",
        "*****************",    "*****************",    "LU1 To US1 Link Fail",
        "LU1 To US2 Link Fail", "LU1 To DS1 Link Fail", "LU1 To DS2 Link Fail",
        "US1 To LU1 Link Fail", "US2 To LU1 Link Fail", "DS1 To LU1 Link Fail",
        "DS2 To LU1 Link Fail", "LU2 To US1 Link Fail", "LU2 To US2 Link Fail",
        "LU2 To DS1 Link Fail", "LU2 To DS2 Link Fail", "US1 To LU2 Link Fail",
        "US2 To LU2 Link Fail", "DS1 To LU2 Link Fail", "DS2 To LU2 Link Fail",
        "Modem Err No Carier",  "Peer CPU Link Fail",   "Failure at DS",
        "Failure at US",        "PD Pulse Mismatch  ",  "Bootup PD Fail",
        "Deceptive Axle Det",   "PD1 & PD2 Pulsating",  "PD1 Sup Low",
        "PD2 Sup Low"       ,   "PD1 Pulsating",        "PD2 Pulsating",
        "PD State Missing",     "PD Sup Pulsating",     "PD State Fail",
        "PD Not Detecting",     "Prep Relay A Fail",    "Prep Relay B Fail",
        "Vital Relay A Fail",   "Vital Relay B Fail",   "Direct-OUTCount ",
        "PD Sup Missing",       "*****************" ,   "*****************" ,
        "Power fail at DS1",    "Power fail at DS2",    "Power fail at US1",
        "Power fail at US2",    "*****************" ,   "*****************" ,
        "*****************",    "*****************",    "Invalid Net Address",
        "Incorrect Code CRC",   "Wrong Configuration",  "Invalid_Count",
        " RAM Test Failed ",    "*****************",    "*****************",
        "*****************"
    };*/
/************************************************************
Component name      :ERROR
Module Name         :void Initialize_Error()
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initialize the error refresh timeout, display state, major
                     and minor bits and bytes to zero


Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Initialise_System()

Input Variables         Name                            Type
    Global          :   None

    Local           :   i                               BYTE

Output Variables        Name                            Type
    Global          :   Error_Display.refresh_Timeout   BYTE
                        Error_Display.State             Enumerator
                        Minor_Err.Byte                  BYTE
                        Major_Err.Byte                  BYTE

    Local           :   None

Signal Variables

Macro definitions used:     Macro                               Value
                            TOTAL_NO_MAJOR_ERRORS               8

References          :

Derived Requirements:

*************************************************************/
void Initialise_Error(void)
{
     BYTE i;
     Error_Display.refresh_Timeout    = 0;
     Error_Display.State              = (Error_Display_State)0;
     for(i=0;i<TOTAL_NO_MAJOR_ERRORS;i++)
     {
       Minor_Err.Byte[i]=(BYTE) 0x00;
     }
      Major_Err.Byte =(BYTE) 0x00;
     Error_Code.Flags.DS_Section = FALSE;
     Error_Code.Flags.US_Section = FALSE;
     Error_Code.DS_Error = (BYTE)0x00;
     Error_Code.US_Error = (BYTE)0x00;
}
/***********************************************************************
Component name      :ERROR
Module Name         :void Update_Display_Error()
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Updates the error status and displays the same on the LCD


Design Requirements     :

Interfaces
    Calls           :   ERROR.c     -   Update_Error_Code()

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_DE_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_DF()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables         Name                            Type
    Global          :   Error_Display.State             Enumerator
                        MAJOR_ERROR                     Enumerator
                        Major_Err.Byte                  BYTE
                        MINOR_ERROR                     Enumerator
                        STRING_COPY                     Enumerator
                        DISPLAY_ERROR_CODE              Enumerator

    Local           :   uchCol                          BYTE
                        Major_Bit_No                    BYTE
                        Minor_Bit_No                    BYTE
                        Minor_Byte_No                   BYTE


Output Variables        Name                            Type
    Global          :   Error_Display.State             Enumerator
                        Error_Display.refresh_Timeout   BYTE
                        Minor_Err.Byte                  BYTE
                        Error_Display.position          BYTE
                        Error_Display.uchstring         Array of BYTEs
                        Error_Display.Error_Code        BYTE

    Local           :   None

Signal Variables


Macro definitions used:     Macro                               Value
                            BITS_PER_BYTE                       8
                            DISPLAY_REFRESH_TIME                20
                            TIMEOUT_EVENT                       0
                            NO_OF_CHARACTERS_PER_LINE           21

References          :

Derived Requirements:

Algorithm           :1. Set all Major bit number, Minor bit number, Minor Byte
                        number to zero.
                     2. If there is any major error, assign major bit number to
                         minor byte number.Clear the Minor bit number, assign display
                        refresh time to 20ms and go to step 3
                     3. If minor bit number is greater than 8 and go to step 4
                     4. If dispaly refresh timeout(20ms) occured,if there is a Minor
                        error assign next state "STRING_COPY" to error display state.
                        Get the error display position from the Minor byte number and
                        minor bit number. Increment the Minor bit number
                     5. In "STRING_COPY" state read the error message from the error
                        table and put that in string and assign next state
                        "DISPLAY_ERROR_CODE" to error display state
                     6. In "DISPLAY_ERROR_CODE" state,assign error display position + 1
                        to error code,refresh timeout to 20ms,assign error display state
                        to MINOR_ERROR
                     7. By default clear the minor bit, major bit, minor byte and assign
                        error display state to "MAJOR_ERROR"

***************************************************************************/

void Update_Display_Error(void)
{
  static BYTE uchCol = 0;
  static BYTE Major_Bit_No  = 0;
  static BYTE Minor_Bit_No  = 0;
  static BYTE Minor_Byte_No = 0;

     switch(Error_Display.State)
     {
        case MAJOR_ERROR:
            if(Major_Err.Byte & BitMask_List[Major_Bit_No])
            {
              Minor_Byte_No = (Major_Bit_No);
              Minor_Bit_No  = 0;
              Error_Display.State =  MINOR_ERROR;
              Error_Display.refresh_Timeout = DISPLAY_REFRESH_TIME;
            }
            Major_Bit_No = Major_Bit_No + 1;
            if(Major_Bit_No >= BITS_PER_BYTE)
            {
              Major_Bit_No = 0;
            }
            break;
        case MINOR_ERROR:
            if(Minor_Bit_No >= BITS_PER_BYTE)
            {
              Error_Display.State=  MAJOR_ERROR;
              break;
            }
            if(Error_Display.refresh_Timeout == TIMEOUT_EVENT)
            {
                if(Minor_Err.Byte[Minor_Byte_No] & BitMask_List[Minor_Bit_No])
                {
                    Error_Display.State=  STRING_COPY;
                    uchCol = 0;
                    Error_Display.position = (BYTE)((Minor_Byte_No * BITS_PER_BYTE) + Minor_Bit_No);
                }
                Minor_Bit_No = Minor_Bit_No + 1;
            }
            break;
        case STRING_COPY:
           //Error_Display.uchstring[uchCol] = Error_Table[Error_Display.position][uchCol];

           ErrorCodeForSPI = Error_Display.position; // Set Error on any defect for SMCPU
           uchCol = uchCol + 1;
           if(uchCol >= NO_OF_CHARACTERS_PER_LINE)
           {
            uchCol = 0;
            Error_Display.State= DISPLAY_ERROR_CODE;
           }
           break;
        case DISPLAY_ERROR_CODE:
          Error_Display.Error_Code = Error_Display.position + 1;
          Error_Display.refresh_Timeout = DISPLAY_REFRESH_TIME;
          Error_Display.State =  MINOR_ERROR;
          Update_Error_Code();
          break;
        default:
          Major_Bit_No  = 0;
          Minor_Bit_No  = 0;
          Minor_Byte_No = 0;
          Error_Display.State =  MAJOR_ERROR;
          break;
      }
}
/****************************************************************
Component name      :ERROR
Module Name         :void Set_Error_Status_Byte( BYTE  Major_Err_Id,  BYTE  status_byte)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Set the error status of Major and Minor bytes

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   RESTORE.c   -   Check_DAC_Boards_Runtime()
                        SYS_MON.c   -   Check_Communication_Links_SF1()
                        SYS_MON.c   -   Check_Communication_Links_SF2()
                        SYS_MON.c   -   Check_Communication_US_CF1()
                        SYS_MON.c   -   Check_Communication_US_CF2()
                        SYS_MON.c   -   Check_Communication_DS_CF1()
                        SYS_MON.c   -   Check_Communication_DS_CF2()
                        SYS_MON.c   -   Check_Communication_Links_EF1()
                        SYS_MON.c   -   Check_Communication_Links_EF2()

Input Variables         Name                            Type
    Global          :   Major_Err_Id                    BYTE
                        status_byte                     BYTE

    Local           :   None

Output Variables        Name                        Type
    Global          :   Major_Err.Byte              BYTE
                        Minor_Err.Byte              BYTE

    Local           :   None

Signal Variables

Macro definitions used:     Macro                               Value
                            None


References          :

Derived Requirements:

Algorithm           :To set error status byte check the major byte
                     number, bit position and minor byte number.
                     1. Set major error byte with its error ID address as major
                        error byte
                     2. Set compliment of status byte as minor error byte

*****************************************************************/
void Set_Error_Status_Byte ( BYTE  Major_Err_Id, BYTE status_byte)
{
    Major_Err.Byte = (Major_Err.Byte | BitMask_List[Major_Err_Id]);
    Minor_Err.Byte[Major_Err_Id] = ~(status_byte);
}
/**************************************************************
Component name      :ERROR
Module Name         :void Set_Error_Status_Bit (BYTE status_bit)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Set the error status bit

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   AXLE_MON.c  -   Determine_TrackState()
                        AXLE_MON.c  -   Detect_PD_Failures()
                        AXLE_MON.c  -   Chk_for_AxleCount_Completion()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_DS.c   -   Process_DS_Reset_Info_Message()
                        COMM_SM.c   -   Process_Interprocess_Message()
                        COMM_SM.c   -   Check_Peer_Status()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_US.c   -   Process_US_Reset_Info_Message()
                        COMM_US.c   -   Check_US_Unit_Status()
                        DAC_MAIN.c  -   Initialise_System()
                        RESTORE.c   -   Check_DIP_Switches()
                        RLYA_MGR.c  -   Update_Relay_A_State()
                        RLYB_MGR.c  -   Update_Relay_B_State()
                        RLYD3_MGR.c -   Update_Relay_D3_State()
                        RLYDE_MGR.c -   Update_Relay_DE_State()
                        SYS_MON.c   -   Read_FeedBack_Signal_Port()
                        SYS_MON.c   -   Check_Inter_Processor_Link()
                        SYS_MON.c   -   Check_DS_Power_Status()
                        SYS_MON.c   -   Check_US_Power_Status()

Input Variables         Name                        Type
    Global          :   None

    Local           :   Error_No                    BYTE
                        Minor_Byte_No               BYTE
                        Minor_Bit_position          BYTE


Output Variables        Name                        Type
    Global          :   Major_Err.Byte              BYTE
                        Minor_Err.Byte              BYTE

    Local           :   None

Signal Variables


Macro definitions used:     Macro                               Value
                            BITS_PER_BYTE                       8

References          :

Derived Requirements:

Algorithm           :1. Assign error number / BITS_PER_BYTE (8) to Minor byte number
                     2. Get the Minor bit position from the error number
                     3. Get the new Major error byte by ORing the old value of major
                        byte with Minor_Byte_No
                     4. Get new Minor Byte by ORing the old value of minor byte number
                        with new Minor bit position
************************************************************/
void Set_Error_Status_Bit (BYTE Error_No)
{
    BYTE  Minor_Byte_No;
    BYTE  Minor_Bit_position;

    Minor_Byte_No = Error_No / BITS_PER_BYTE;
    Minor_Bit_position = Error_No % BITS_PER_BYTE;
    Major_Err.Byte = (Major_Err.Byte | BitMask_List[Minor_Byte_No]);
    Minor_Err.Byte[Minor_Byte_No] = (Minor_Err.Byte[Minor_Byte_No] | BitMask_List[Minor_Bit_position]);
}

 /****************************************************************
Component name      :ERROR
Module Name         :void Clear_Error_Display(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the error display by clearing the major and minor error bytes



Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   RLYA_MGR.c  -   Get_Relay_A_State()
                        RLYB_MGR.c  -   Get_Relay_B_State()

    Called by       :   RLYA_MGR.c  -   Start_Relay_A_Mgr()
                        RLYB_MGR.c  -   Start_Relay_B_Mgr()

Input Variables         Name                            Type
    Global          :   DAC_DEFECTIVE                   Enumerator

    Local           :   uch_A_State                     BYTE
                        uch_B_State                     BYTE
                        i                               BYTE

Output Variables        Name                            Type
    Global          :   Minor_Err.Byte                  BYTE
                        Major_Err.Byte                  BYTE

    Local           :   None

Signal Variables

Macro definitions used:     Macro                               Value
                            TOTAL_NO_MAJOR_ERRORS               8

References          :

Derived Requirements:




Algorithm           :1.Get the Relay A and Relay B status
                     2.Check for system healthy state
                     3.If the system is healthy clear the major and minor errors
*************************************************************/
void Clear_Error_Display(void)
{
    BYTE uch_A_State,uch_B_State,i =0;

    ErrorCodeForSPI = 0xFF; // No Error, at startup, Added by Shankar.

    uch_A_State = Get_Relay_A_State();
    uch_B_State = Get_Relay_B_State();
    if( uch_A_State != (BYTE)DAC_DEFECTIVE && uch_B_State != (BYTE)DAC_DEFECTIVE)
    {
       for(i=0;i<TOTAL_NO_MAJOR_ERRORS;i++)
       {
        Minor_Err.Byte[i]=(BYTE) 0x00;
       }
       Major_Err.Byte =(BYTE) 0x00;
    }
}
/**************************************************************
Component name      :ERROR
Module Name         :void Decrement_Err_display_50msTmr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Decrements the error display timer 20ms



Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_DE_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_DE()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()


Input Variables         Name                            Type
    Global          :   Error_Display.refresh_Timeout   BYTE

    Local           :   None

Output Variables        Name                            Type
    Global          :   Error_Display.refresh_Timeout   BYTE

    Local           :   None

Signal Variables

Macro definitions used:     Macro                               Value
                            None

References          :

Derived Requirements:

*************************************************************/
void Decrement_Err_display_50msTmr()
{
     if(Error_Display.refresh_Timeout > 0)
      {
        Error_Display.refresh_Timeout = Error_Display.refresh_Timeout - 1;
      }
}


void Decrement_LCWS_Health_Relay_Timer(void)
{
    if(LCWS_Section_TimeOut > 0)
    {
        LCWS_Section_TimeOut = LCWS_Section_TimeOut - 1;
    }
}
/**************************************************************
Component name      :ERROR
Module Name         :void Update_Error_Code()
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the error code inorder to send to SMCPU

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   ERROR.c - Update_Display_Error()


Input Variables         Name                            Type
    Global          :   DAC_UNIT_TYPE_SF                Enumerator
                        DAC_UNIT_TYPE_EF                Enumerator
                        DAC_UNIT_TYPE_CF                Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        Error_Code.Flags.DS_Section     Bit
                        Error_Code.Flags.US_Section     Bit
                        DIP_Switch_Info.Unit_Type       BYTE
                        Error_Display.Error_Code        BYTE

    Local           :   None

Output Variables        Name                            Type
    Global          :   Error_Code.Flags.DS_Section     Bit
                        Error_Code.Flags.US_Section     Bit
                        Error_Code.DS_Error             BYTE
                        Error_Code.US_Error             BYTE

    Local           :   None

Signal Variables

Macro definitions used:     Macro                               Value
                            FALSE                                 0
                            TRUE                                  1
                            CATASTROPHIC_ERROR                    0

References          :

Derived Requirements:

Algorithm           :1.Update the down and up stream error code if the flag of the respective section is LOW
                     2.After the error code got updated set the flag as HIGH and hence prevent further updation of the error code
                     3. Updation will take place as per the configuration type
*************************************************************/
void Update_Error_Code(void)
{

      switch (DIP_Switch_Info.DAC_Unit_Type)
        {
            case DAC_UNIT_TYPE_SF:
                  if(Error_Code.Flags.DS_Section == FALSE)
                     {
                       Error_Code.DS_Error = (BYTE)Error_Display.Error_Code;
                       Error_Code.Flags.DS_Section = TRUE;
                     }
                 break;
            case DAC_UNIT_TYPE_EF:
                  if(Error_Code.Flags.US_Section == FALSE)
                     {
                       Error_Code.US_Error = (BYTE)Error_Display.Error_Code;
                       Error_Code.Flags.US_Section = TRUE;
                     }
                 break;
            case DAC_UNIT_TYPE_CF:
            case DAC_UNIT_TYPE_3D_SF:
            case DAC_UNIT_TYPE_3D_EF:
               if (Status.Flags.DS_System_Status == CATASTROPHIC_ERROR)
                  {
                    if(Error_Code.Flags.DS_Section == FALSE)
                     {
                       Error_Code.DS_Error = (BYTE)Error_Display.Error_Code;
                       Error_Code.Flags.DS_Section = TRUE;
                     }
                  }
               if (Status.Flags.US_System_Status == CATASTROPHIC_ERROR)
                   {
                    if(Error_Code.Flags.US_Section == FALSE)
                     {
                       Error_Code.US_Error = (BYTE)Error_Display.Error_Code;
                       Error_Code.Flags.US_Section = TRUE;
                     }
                   }
                 break;
             default:
                  if(Error_Code.Flags.DS_Section == FALSE)
                     {
                       Error_Code.DS_Error = (BYTE)Error_Display.Error_Code;
                       Error_Code.Flags.DS_Section = TRUE;
                     }
                 break;
        }


}

/****************************************************************
Component name      :ERROR
Module Name         :BYTE Get_DS_Error_Code(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Return the value of a DS section error code

Allocated Requirements  :
Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   COMM_SM.c   -   Build_Interprocess_Reset_Info_Message()
                        COMM_SM.c   -   Build_Interprocess_Post_Reset_Message()

Input Variables         Name                            Type
    Global          :  Error_Code.Flags.DS_Section       Bit
                       Error_Code.DS_Error               BYTE

    Local           :  uchcode                          BYTE

Output Variables        Name                            Type
    Global          :   None

    Local           :   uchcode                         BYTE

Signal Variables

Macro definitions used:     Macro                               Value
                            TRUE                                  1

References          :

Derived Requirements:

Algorithm           :1.If the error flag for downstream section got setted return the error code
*************************************************************/
BYTE Get_DS_Error_Code(void)
{
  BYTE uchcode = 0;
  if (Error_Code.Flags.DS_Section == TRUE)
     {
      uchcode = (BYTE)Error_Code.DS_Error;
     }
 return(uchcode);
}

/****************************************************************
Component name      :ERROR
Module Name         :BYTE Get_US_Error_Code(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Return the value of a US section error code

Allocated Requirements  :
Design Requirements     :

Interfaces
    Calls           :   None

    Called by       :   COMM_SM.c   -   Build_Interprocess_Reset_Info_Message()
                        COMM_SM.c   -   Build_Interprocess_Post_Reset_Message()

Input Variables         Name                            Type
    Global          :  Error_Code.Flags.US_Section       Bit
                       Error_Code.US_Error               BYTE

    Local           :  uchcode                          BYTE

Output Variables        Name                            Type
    Global          :   None

    Local           :   uchcode                         BYTE

Signal Variables

Macro definitions used:     Macro                               Value
                            TRUE                                  1

References          :

Derived Requirements:
Algorithm           :1.If the error flag for upstream section got setted return the error code
*************************************************************/
BYTE Get_US_Error_Code(void)
{
  BYTE uchcode = 0;
  if (Error_Code.Flags.US_Section == TRUE)
     {
      uchcode = (BYTE)Error_Code.US_Error;
     }
 return(uchcode);
}
/****************************************************************
Component name      :ERROR
Module Name         :void Clear_DS_Error_Code(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the Down section error code logged for the purpose to send through SPI



Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   None

    Called by       :   RLYB_MGR.c  -   Set_Relay_B_Reset_State()

Input Variables         Name                            Type
    Global          :   None                            None

    Local           :  None                            None

Output Variables        Name                            Type
    Global          :   Error_Code.Flags.DS_Section      Bit
                        Error_Code.DS_Error             BYTE

    Local           :   None

Signal Variables

Macro definitions used:     Macro                               Value
                            FALSE                                 0

References          :

Derived Requirements:
Algorithm           :1.Clear the Down stream section error flag
                     2.Load the DS error value as zero
*************************************************************/
void Clear_DS_Error_Code(void)
{
 Error_Code.Flags.DS_Section = FALSE;
 Error_Code.DS_Error = 0x00;
}

/****************************************************************
Component name      :ERROR
Module Name         :void Clear_US_Error_Code(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the Up section error code logged for the purpose to send through SPI



Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   None

    Called by       :   RLYA_MGR.c  -   Set_Relay_A_Reset_State()

Input Variables         Name                            Type
    Global          :   None                            None

    Local           :  None                            None

Output Variables        Name                            Type
    Global          :   Error_Code.Flags.US_Section      Bit
                        Error_Code.US_Error             BYTE

    Local           :   None

Signal Variables

Macro definitions used:     Macro                               Value
                            FALSE                                 0

References          :

Derived Requirements:
Algorithm           :1.Clear the Up stream section error flag
                     2.Load the US error value as zero
*************************************************************/
void Clear_US_Error_Code(void)
{
 Error_Code.Flags.US_Section = FALSE;
 Error_Code.US_Error = 0x00;
}
