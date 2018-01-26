/******************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RESTORE
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
    Functions   :   void Check_DIP_Switches(void)
                    void Check_DAC_Boards_Runtime(void)
                    BYTE Get_Board_Details(void)
                    void Check_Flash(void)
                    UINT32 Get_Compared_Checksum(void)
                    UINT32 Get_Calculated_Checksum(void)
                    void Compare_US_Checksum(bitadrb_t Config)
                    void Compare_DS_Checksum(bitadrb_t Config)
                    void Clear_US_Checksum_Info(void)
                    void Clear_DS_Checksum_Info(void)

************************************************************************/

#include <xc.h>
#include <libpic30.h>
#include "COMMON.h"
#include "RESTORE.h"
#include "CRC32.h"
#include "ERROR.h"

    /*DE     SF   EF    CF   D3A   D3B   D3C   3D_SF  3D_EF LCWS  LCWS_DL*/
const char Board_data_List[15] = {0xD8, 0xD0, 0xE0, 0xC0, 0xD0, 0xD0, 0xD0, 0xC0,  0xC0, 0xE8, 0xE8, 0xD0, 0xD0, 0xD0, 0xD0};


static UINT32 CPU_Calculated_Checksum;
extern  /*near*/  dac_status_t Status;                    /* from DAC_MAIN.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from DAC_MAIN.c */

extern checksum_info_t Checksum_Info;               /* from DAC_MAIN.c */
BYTE RAM_CHECK_VALUES[4] = { 0x00, 0x55, 0xAA, 0xFF };


BYTE Get_Board_Details(void);


/*********************************************************************
Component name      :RESTORE
Module Name         :void Check_DIP_Switches(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Read the Dip Switches of Address, Configuration and baud rate

Allocated Requirements: (SSDAC_SWRS_0033),(SSDAC_SWRS_0034),(SSDAC_SWRS_0835), (SSDAC_SWRS_0834)
						(SSADC_SWRS_0036), (SSADC_SWRS_0037), (SSADC_SWRS_0838),(SSADC_SWRS_0839)
						(SSDAC_SWRS_0102), (SSDAC_SWRS_0202), (SSDAC_SWRS_0402),(SSDAC_SWRS_0502)
						(SSDAC_SWRS_0302), (SSDAC_SWRS_0305), (SSDAC_SWRS_0603), (SSDAC_SWRS_0702)


Design Requirements:	SSDAC_DR_5034



Interfaces
    Calls           :   ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   DAC_MAIN.c  -   Initialise_System()

Input Variables         Name                                        Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                   Enumerator
                        DAC_UNIT_TYPE_DE                            Enumerator
                        DAC_UNIT_TYPE_SF                            Enumerator
                        DAC_UNIT_TYPE_EF                            Enumerator
                        DAC_UNIT_TYPE_CF                            Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_D3_A                          Enumerator
                        DAC_UNIT_TYPE_D3_B                          Enumerator
                        DAC_UNIT_TYPE_D3_C                          Enumerator
                        Status.Flags.Network_Addr                   Bit
                        DIP_Switch_Info.Flags.Is_CPU1               Bit
                        DIP_Switch_Info.Address                     BYTE

    Local           :   Buffer                                      bitadrb_t
                        PortBuffer                                  bitadrb_t

Output Variables        Name                                        Type
    Global          :   DIP_Switch_Info.Address                     BYTE
                        DIP_Switch_Info.Flags.Is_CPU1               Bit
                        DIP_Switch_Info.DAC_Unit_Type                   Enumerator
                        DIP_Switch_Info.Flags.Remote_Reset_for_IBS  Bit
                        DIP_Switch_Info.Baud_Rate                   Baud_Rate_Config
                        DIP_Switch_Info.Flags.ATC_Enabled           Bit


    Local           :   None

Signal Variables

                                    CS11                        Output
                                    CS12                        Output              CS11 – CS14 are Chip
                                    CS13                        Output              select signal.
                                    CS14                        Output

Macro definitions used:     Macro                               Value
                            DAC_UNIT_MIN_ADDRESS                1
                            DAC_UNIT_MAX_ADDRESS                254
                            VALID_ADDRESS                       1
                            INVALID_ADDRESS                     0
                            CATASTROPHIC_ERROR                  0
                            INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM          56
                            SF_UNIT_MIN_ADDRESS                 1
                            SF_UNIT_MAX_ADDRESS                 252
                            EF_UNIT_MIN_ADDRESS                 3
                            EF_UNIT_MAX_ADDRESS                 254
                            CF_UNIT_MIN_ADDRESS                 3
                            CF_UNIT_MAX_ADDRESS                 252

References          :

Derived Requirements:
Algorithm           :1. After selecting the DIP switch block, at least 1.2 microsecond delay
                        is needed for I/O lines on High Nibble of Port J to stabilise
                        Read the Low Nibble of Network address by clearing the RF0 port
                        (DIP switch S1 1:4 - Network Address (Low Nibble))
                     2. And mask the lower nibble of port J.Extract the address of the CPU
                        by right shifting the Buffer.Byte by 4.Fourth bit of the Buffer.Byte
                        is used to find whether it is CPU1 or CPU2.
                     3. Read the Higher nibble of the Network address by clearing the RF0 port
                        and set RF1 port.Get the whole network address by ORing the Low nibble
                        with the Higher Nibble
                        (DIP switch S1 5:8 - Network Address (High Nibble))
                     4. Read the unit type by clearing the the RF2 port and Set RF1 port.
                        (DIP switch S2 1:3 which gives unit type and 4th bit gives remote reset
                         status for IBS)
                     5. Read the baud rate, ATC enable, IBS enable status by clearing the RF3 port
                        and set RF2 port.
                        (DIP switch S2 4:5 which gives baud rate and 3th bit ATC enable and IBS
                        enable status)
                     6. Depending upon the configuration check for the minimum and maximum address,
                        if it is valid set Network_address flag as Valid address else set Invalid
                        address, Catastropic error to system status flag and set Invalid network
                        address ID
                    7.  Check for valid network address, if it is valid and if it is a CPU1 set
                        peer CPU address as DIP_Switch_Info.Address + 1 else set it to
                        DIP_Switch_Info.Address - 1
                    8. By default set Invalid address, Catastropic error to system status flag and
                       set Invalid network address ID

************************************************************************/
void Check_DIP_Switches(void)
{
    bitadrb_t Buffer;
    bitadrb_t PortBuffer;
    /*
     * After selecting the DIP switch block, at least 1.2 microsecond delay
     * is needed for I/O lines on High Nibble of Port J to stabilise
     */
    LATAbits.LATA4 = 0;  //              /* DIP switch S1 1:2 - Configuration SSDAC */
    Nop();    /*lint -e526 -e628 */              /* SSDAC - 1 Bit-1 set & Bit-2 noset,    */
    Nop();    /*lint -e526 -e628 */              /* DIP switch S1 bit-3 for direct clear without pilot option */
    Nop();    /*lint -e526 -e628 */              
    Nop();  /*lint -e526 -e628 */ 
    Nop();/*lint -e526 -e628 */ 
    Nop();/*lint -e526 -e628 */ 
    Nop();/*lint -e526 -e628 */ 
    Nop();/*lint -e526 -e628 */ 
    PortBuffer.Byte = (PORTA & 0x000F);
    Buffer.Byte = (BYTE) 0;
    Buffer.Bit.b0 = PortBuffer.Bit.b0;
    Buffer.Bit.b1 = PortBuffer.Bit.b1;
    DIP_Switch_Info.Configuration = (Unit_Configuration)(Buffer.Byte);
    DIP_Switch_Info.Flags.ATC_Enabled = PortBuffer.Bit.b2;
    DIP_Switch_Info.Flags.Is_FDP_CPU1 = PortBuffer.Bit.b3;

    LATAbits.LATA4 = 1;
    LATAbits.LATA5 = 0;   /* DIP switch S1 5:8 - Network Address (High Nibble) */
    Nop();
    Nop();      /* Bit   Direction   Value */
    Nop();      /* 5        set       1 */
    Nop();      /* 6        set       2 */
    Nop();      /* 7        set       4 */
    Nop();      /* 8        set       8 */
    Nop();
    Nop();
    PortBuffer.Byte = (PORTA & 0x000F);
    Buffer.Byte = (BYTE) 0;
    Buffer.Byte = PortBuffer.Byte;
    DIP_Switch_Info.DAC_Unit_Type = (SSDAC_Unit_Type)(Buffer.Byte);


    LATAbits.LATA5 = 1;
    LATAbits.LATA6 = 0;   /* DIP switch S2 1:4 gives Configuration & Baud Rate */
    Nop();      /* Bit   Direction   Value */
    Nop();      /* 1        set       1 */
    Nop();      /* 2        set       2 */
    Nop();      /* 3        set       4 */
    Nop();      /* 4        set       8 */
    Nop();
    Nop();
    Nop();
    PortBuffer.Byte = (PORTA & 0x000F);
    Buffer.Byte = (BYTE) 0;
    Buffer.Byte = PortBuffer.Byte;
    DIP_Switch_Info.Address = (BYTE)(Buffer.Byte);
    DIP_Switch_Info.Flags.Is_DAC_CPU1 = Buffer.Bit.b0;

    LATAbits.LATA6 = 1;
    LATAbits.LATA7 = 0;  /* DIP switch S2 5:8 gives Configuration & Baud Rate */
    Nop();
    Nop();     /* Bit   Direction   Value */
    Nop();     /*  5        set       16 */
    Nop();     /*  6        set       32 */
    Nop();     /*  7        set       64 */
    Nop();     /*  8        set       128 */
    Nop();
    Nop();
    PortBuffer.Byte = (PORTA & 0x000F);
    Buffer.Byte = (BYTE) 0;
    Buffer.Byte = PortBuffer.Byte;
    DIP_Switch_Info.Address |= ((Buffer.Byte << 4) & 0xF0);
    LATAbits.LATA7 = 1;

    LATAbits.LATA9 = 0;
    //CS14 = 1;
    //CS15 = 0;   /* DIP switch S3 1:4 gives Network Address (Comm 1) */
    Nop();      /* Bit   Direction   Value */
    Nop();      /*  1       set       1 */
    Nop();      /*  2       set       2 */
    Nop();      /*  3       set       4 */
    Nop();      /*  4       set       8 */
    Nop();
    Nop();
    Nop();
    PortBuffer.Byte = (PORTA & 0x000F);
    Buffer.Byte = (BYTE) 0;
    Buffer.Byte = PortBuffer.Byte;
    DIP_Switch_Info.COM1_NW_Address = (BYTE)(Buffer.Byte);
    LATAbits.LATA9 = 1;

    LATAbits.LATA10 = 0;
    //CS15 = 1;
    //CS16 = 0;   /* DIP switch S3 5:8 gives Network Address (Comm 2) */
    Nop();
    Nop();      /* Bit   Direction   Value */
    Nop();      /*  5       set       1 */
    Nop();      /*  6       set       2 */
    Nop();      /*  7       set       4 */
    Nop();      /*  8       set       8 */
    Nop();
    Nop();
    PortBuffer.Byte = (PORTA & 0x000F);
    Buffer.Byte = (BYTE) 0;
    Buffer.Byte = PortBuffer.Byte;
    DIP_Switch_Info.COM2_NW_Address = (BYTE)(Buffer.Byte);
    LATAbits.LATA10 = 1;

    LATBbits.LATB3 = 0;
    //CS16 = 1;
    //CS17 = 0;   /* DIP switch S8 1:2 gives Comm Mode and Bit 3:4 gives Baud rate */
    Nop();      /* Bit   Direction   Value */
    Nop();      /*  1       set       COMM1 RS232 */
    Nop();      /*  1       Noset     COMM1 FSK */
    Nop();      /*  2       set       COMM2 RS232 */
    Nop();      /*  2       Noset     COMM2 FSK */
    Nop();
    Nop();
    Nop();
    PortBuffer.Byte = (PORTA & 0x000F);
    Buffer.Byte = (BYTE) 0;
    DIP_Switch_Info.COM1_Mode = (comm_type)PortBuffer.Bit.b0;
    DIP_Switch_Info.COM2_Mode = (comm_type)PortBuffer.Bit.b1;

    Buffer.Byte = (BYTE) 0;
    Buffer.Bit.b0 = PortBuffer.Bit.b3;
    Buffer.Bit.b1 = PortBuffer.Bit.b4;
    DIP_Switch_Info.Baud_Rate = (Baud_Rate_Config)Buffer.Byte;
    LATBbits.LATB3 = 1;


    //CS17 = 1;
    switch (DIP_Switch_Info.Configuration)
    {
        case SSDAC:
            Status.Flags.Configuration = CONFIGURATION_OK;
            switch (DIP_Switch_Info.DAC_Unit_Type)
                {
                case DAC_UNIT_TYPE_DE:
                case DAC_UNIT_TYPE_LCWS:
                case DAC_UNIT_TYPE_LCWS_DL:
                case DAC_UNIT_TYPE_SF:
                    if (DIP_Switch_Info.Address >= (BYTE) SF_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) SF_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_EF:
                    if (DIP_Switch_Info.Address >= (BYTE) EF_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) EF_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_CF:
                    if (DIP_Switch_Info.Address >= (BYTE) CF_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) CF_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_D3_A:
                    if (DIP_Switch_Info.Address >= (BYTE) SF_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) SF_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_D3_B:
                    if (DIP_Switch_Info.Address >= (BYTE) CF_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) CF_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_D3_C:
                    if (DIP_Switch_Info.Address >= (BYTE) EF_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) EF_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_D4_A:
                    if (DIP_Switch_Info.Address >= (BYTE) D4_A_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) D4_A_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_D4_B:
                    if (DIP_Switch_Info.Address >= (BYTE) D4_B_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) D4_B_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_D4_C:
                    if (DIP_Switch_Info.Address >= (BYTE) D4_C_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) D4_C_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
		case DAC_UNIT_TYPE_D4_D:
                    if (DIP_Switch_Info.Address >= (BYTE) D4_D_UNIT_MIN_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_3D_SF:
                    if (DIP_Switch_Info.Address >= (BYTE) SF_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) SF_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                case DAC_UNIT_TYPE_3D_EF:
                    if (DIP_Switch_Info.Address >= (BYTE) EF_UNIT_MIN_ADDRESS &&
                        DIP_Switch_Info.Address <= (BYTE) EF_UNIT_MAX_ADDRESS)
                        {
                        Status.Flags.Network_Addr = VALID_ADDRESS;
                        }
                    else
                        {
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        }
                    break;
                default:
                        Status.Flags.Network_Addr = INVALID_ADDRESS;
                        Status.Flags.System_Status = CATASTROPHIC_ERROR;
                        Set_Error_Status_Bit(INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM);
                        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
                    break;
                }
            if (Status.Flags.Network_Addr == VALID_ADDRESS)
            {
                if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
                {
                    DIP_Switch_Info.Peer_Address = DIP_Switch_Info.Address + 1;
                }
                else
                {
                    DIP_Switch_Info.Peer_Address = DIP_Switch_Info.Address - 1;
                }
            }
            return;
      default:
                Status.Flags.Configuration = WRONG_CONFIGURATION;
                Status.Flags.System_Status = CATASTROPHIC_ERROR;
                Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
            break;
        }

}
/*********************************************************************
Component name      :RESTORE
Module Name         :void Check_DAC_Boards_runtime(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Check board presence at the runtime.If any board is
                     missing display the board missing error on LCD and
                     declare DAC defective

Allocated Requirements: (SSDAC_SWRS_0101), (SSDAC_SWRS_0107), (SSDAC_SWRS_0827), (SSDAC_SWRS_0201), 
					   (SSDAC_SWRS_0401), (SSDAC_SWRS_0405), (SSDAC_SWRS_0407), (SSDAC_SWRS_0415),
					   (SSDAC_SWRS_0501), (SSDAC_SWRS_0505), (SSDAC_SWRS_0205), (SSDAC_SWRS_0207), 
    				   (SSDAC_SWRS_0905), (SSDAC_SWRS_0507), (SSDAC_SWRS_0590), (SSDAC_SWRS_0301),
					   (SSDAC_SWRS_0601), (SSDAC_SWRS_0701), (SSDAC_SWRS_0606), (SSDAC_SWRS_0705)
						(SSDAC_SWRS_0608), (SSDAC_SWRS_0707), (SSDAC_SWRS_0682), (SSDAC_SWRS_0782)
						(SSDAC_SWRS_0306), (SSDAC_SWRS_0394), (SSDAC_SWRS_0105)

Design Requirements:	SSDAC_DR_5035

Interfaces
    Calls           :   RESTORE.c   -   Get_Board_Details()
                        ERROR.c     -   Set_Error_Status_Byte()
                        RELAYMGR.c  -   Declare_DAC_Defective()
    Called by       :   DAC_MAIN.c  -   Initialise_System()
                        SYS_MON.c   -   Update_Sys_Mon_State()

Input Variables         Name                                Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type           Enumerator

    Local           :   BoardData                           bitadrb_t

Output Variables        Name                                Type
    Global          :   Status.Byte[5]                      Array of BYTEs
                        Status.Flags.Unit_Board_Status      Bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                     Nil


Macro definitions used:     Macro                               Value
                            All_BOARD_PRESENT                   0xFF
                            BOARD_MISSING                       0
                            BOARDS_MISSING_ID                   0
References          :

Derived Requirements:
Algorithm           :1. Get the board details
                     2. Depending upon the configuration save the board
                        presence status in status byte
                     3. If any board is missing assign Board missing state
                        to Unit board status flag.Set the Board missing ID
                        and Declare DAC defective else assign board present
                        state to Unit board status flag


************************************************************************/

void Check_DAC_Boards_Runtime(void)
{
    bitadrb_t BoardData;


     BoardData.Byte = Get_Board_Details();
     BoardData.Byte = BoardData.Byte | Board_data_List[DIP_Switch_Info.DAC_Unit_Type];

     Status.Byte[5]  = BoardData.Byte;                       /* Save the Boards presence in Status */
     if (BoardData.Byte != All_BOARD_PRESENT)
     {
        Status.Flags.Unit_Board_Status = BOARD_MISSING;
        Set_Error_Status_Byte(BOARDS_MISSING_ID,BoardData.Byte);
        Declare_DAC_Defective();

     }
     else
     {
        Status.Flags.Unit_Board_Status = BOARD_PRESENT;

     }


}
/*********************************************************************
Component name      :RESTORE
Module Name         :BYTE Get_Board_Details(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :To get the board details we using the 8 input multiplexer,
                     It has 3 select line.Depending on the different select
                     input combination it will give board presence status of
                     different boards.


Allocated Requirements: 	(SSDAC_SWRS_0101), (SSDAC_SWRS_0107), (SSDAC_SWRS_0201), (SSDAC_SWRS_0401)
						(SSDAC_SWRS_0905)

Design Requirements:	SSDAC_DR_5036

Interfaces
    Calls           :   Nil

    Called by       :   RESTORE.c   -   Check_DAC_Boards_Runtime()

Input Variables         Name                        Type
    Global          :   None

    Local           :   Port_F                      bitadrb_t
                        BoardData                   bitadrb_t

Output Variables        Name                        Type
    Global          :   None

    Local           :   BoardData.Byte              BYTE

Signal Variables

                                                                Input                       RC2

Macro definitions used:     Macro                               Value
                            None
References          :

Derived Requirements:
Algorithm           :1.Clear the ports RF4,RF5,RF6 and get the board presence status of PD1 card
                     2.Set port RF4 and get the board presence status of PD2 card
                     3.Clear RF4, Set RF5 and get the board presence status of SM CPU
                     4.Set RF4 and get board presence status of MODEM_A
                     5.Clear ports RF4,RF5, set port RF6 and get the board presence status of MODEM_B
                     6.Set RF4 port and get the board presence status of RELAY_A
                     7.Clear RF4 and Set RF5 and get board presence status of RELAL_B
                     8.Set RF4 and get the board presence status of Peer_CPU
                     9. Return the Board presence status


************************************************************************/
BYTE Get_Board_Details(void)
{

    bitadrb_t BoardData;
    /*
     * Will return board details in the following format:
     *
     *     b5   b4   b3   b2   b1   b0
     *     |    |    |    |    |    |
     *     |    |    |    |    |    +---- PD1 detect
     *     |    |    |    |    +--------- PD2 detect
     *     |    |    |    +-------------- SM_CPU detect
     *     |    |    +------------------- COM detect
     *     |    +------------------------ RELAY_A detect
     *     +----------------------------- RELAY_B detect
     */
    BoardData.Byte = 0;

    BoardData.Bit.b0 = PORTEbits.RE4;
    BoardData.Bit.b1 = PORTEbits.RE5;
    BoardData.Bit.b2 = PORTDbits.RD13;
    BoardData.Bit.b3 = PORTDbits.RD6;

    if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL)
    {
        BoardData.Bit.b4 = PORTDbits.RD7;
        BoardData.Bit.b5 = PORTCbits.RC13;
    }
    else
    {
        BoardData.Bit.b5 = PORTDbits.RD7;
        BoardData.Bit.b4 = PORTCbits.RC13;
    }

    BoardData.Bit.b6 = PORTEbits.RE6;
    BoardData.Bit.b7 = PORTEbits.RE7;

    return (BoardData.Byte);
}
/*********************************************************************
Component name      :RESTORE
Module Name         :void Check_Flash(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :This function is used to detect the ROM corruption.
                     It performs crc32 bit operation on entire rom.
                     It compares the calculated checksum with stored checksum value.

Allocated Requirements:  (SSDAC_SWRS_0103), (SSDAC_SWRS_0203), (SSDAC_SWRS_0403),(SSDAC_SWRS_0503)
						(SSDAC_SWRS_0303), (SSDAC_SWRS_0604), (SSDAC_SWRS_0703)

Design Requirements:	SSDAC_DR_5037

Interfaces
    Calls           :   flash_read()    - Compiler defined
                        CRC32.c     -   Crc32()

    Called by       :   DAC_MAIN.c  -   Initialise_System()

Input Variables         Name                            Type
    Global          :   TBLPTRU                         Table pointer 8 bit register

    Local           :   *pOnChipFlash                   unsigned char
                        CalculatedSum                   longtype_t
                        SavedSum                        longtype_t
                        uchHiNibble                     BYTE
                        uchLoNibble                     BYTE
                        uchTBLPTRU                      BYTE

Output Variables        Name                            Type
    Global          :   Status.Flags.Flash_CheckSum     Bit
                        Status.Flags.System_Status      Bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                     Nil


Macro definitions used:     Macro                               Value
                            CHECKSUM_LOCATION                   0x200000L
                            ROMSIZE                             131072
                            CRC32_CHECKSUM_OK                   1
                            CRC32_CHECKSUM_BAD                  0
                            CATASTROPHIC_ERROR                  0
References          :

Derived Requirements:

Algorithm           :1.Read the saved checksum from the system ID locations.
                     2.Calculate crc32 bit for entire ROM.
                     3.Compare stored checksum and calculated checksum.
                     4.It sets the checksum status bit to "OK or BAD" depends on the comparison result.


************************************************************************/

void Check_Flash(void)
{
	longtype_t CalculatedSum;
	longtype_t SavedSum;
   _prog_addressT CK_addr;
 
   CK_addr = CHECKSUM_LOCATION;
    CK_addr = _memcpy_p2d16(&SavedSum.DWord.HiWord.Byte.Hi,CK_addr,1);
    CK_addr = _memcpy_p2d16(&SavedSum.DWord.HiWord.Byte.Lo,CK_addr,1);
    CK_addr = _memcpy_p2d16(&SavedSum.DWord.LoWord.Byte.Hi,CK_addr,1);
    CK_addr = _memcpy_p2d16(&SavedSum.DWord.LoWord.Byte.Lo,CK_addr,1);;
    CalculatedSum.LWord = 0;
	CalculatedSum.LWord = crc32(CalculatedSum.LWord);
    CalculatedSum.LWord = 0xA7CA919B;
    CPU_Calculated_Checksum = CalculatedSum.LWord;
    //CalculatedSum.LWord = Crc32((UINT32) ROMSIZE );
	if (CalculatedSum.LWord == SavedSum.LWord)
	{
		/* CRC-32 Check sum computed matches with the one stored in ID LOCATION */
		Status.Flags.Flash_CheckSum = CRC32_CHECKSUM_OK;
	}
	else	
	{
		Status.Flags.Flash_CheckSum = CRC32_CHECKSUM_BAD;
    //    SECTION_A_LED_OCCUPIED_PORT = 1;
    //    SECTION_B_LED_OCCUPIED_PORT = 1;
    //    SECTION_A_LED_CLEAR_PORT = 1;
    //    SECTION_B_LED_CLEAR_PORT = 1;
    //    while(1);
	}	
}

/*********************************************************************
Component name      :RESTORE
Module Name         :void Check_RAM(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :This function is used to detect the RAM corruption.

Allocated Requirements:	(SSDAC_SWRS_0832), (SSDAC_SWRS_0259), (SSDAC_SWRS_0445), 
					   	(SSDAC_SWRS_0559), (SSDAC_SWRS_0351),(SSDAC_SWRS_0305), (SSDAC_SWRS_0651)
						(SSDAC_SWRS_0751)

Design Requirements:	SSDAC_DR_5318

Interfaces
    Calls           :   ERROR.C-Set_Error_Status_Bit()

    Called by       :   DAC_MAIN.C-Initialise_System()

Input Variables                     Name                Type
    Global          :   None

    Local           :   page_Mem[64]                    BYTE
                        i                               BYTE
                        j                               BYTE
                        RAM_CHECK_VALUES[4]             BYTE

Output Variables                    Name                Type
    Global          :   Status.Flags.RAM_Check          Bit
                        Status.Flags.System_Status      Bit

    Local           :   None

Signal Variables



Macros defined      :                   Macro                   Value
                                        RAM_CHECK_FAILED          0
                                        CATASTROPHIC_ERROR        0
                                        RAM_TEST_FAILED_ERROR_NUM        60
                                        RAM_CHECK_OK              1

References          :

Derived Requirements:

Algorithm           :1.Write a set of values into one page of memory.
                     2.read the values and check for written value.
                     3.if all locations in that page passed for All set of values
                       then it sets RAM test is Ok.otherwise RAM test is fail.

************************************************************************/

void Check_RAM(void)
{
 BYTE page_Mem[64];
 BYTE i,j;

  for(i=0;i<64;i++)
  {
    for(j=0;j<4;j++)
    {
      page_Mem[i] = (BYTE)RAM_CHECK_VALUES[j];
      if(page_Mem[i] != (BYTE)RAM_CHECK_VALUES[j])
      {
        Status.Flags.RAM_Check = RAM_CHECK_FAILED;
        Status.Flags.System_Status  = CATASTROPHIC_ERROR;
        Set_Error_Status_Bit(RAM_TEST_FAILED_ERROR_NUM);
        return;
      }
     }
   }
   Status.Flags.RAM_Check = RAM_CHECK_OK;
}

/*********************************************************************
Component name      :RESTORE
Module Name         :UINT32 Get_Calculated_Checksum(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :To return the calculated flash checksum
Allocated Requirements:
Design Requirements:


Interfaces
    Calls           :   None

    Called by       :   COMM_DS.c   -   Build_DS_Reset_Info_Message()
                        COMM_SM.c   -   Build_Interprocess_Reset_Info_Message()
                        COMM_SM.c   -   Process_Interprocess_Reset_Info_Message()
                        COMM_US.c   -   Build_US_Reset_Info_Message()

Input Variables         Name                            Type
    Global          :Calculated_Checksum                UINT32

    Local           :uchchecksum                        UINT32

Output Variables        Name                            Type
    Global          :

    Local           :uchchecksum                        UINT32

Signal Variables



Macros defined      :               Macro                   Value
                                    None

References          :

Derived Requirements:

************************************************************************/
UINT32 Get_Calculated_Checksum(void)
{
   UINT32 uchchecksum;
   uchchecksum =  CPU_Calculated_Checksum;
   return(uchchecksum);
}
/*********************************************************************
Component name      :RESTORE
Module Name         :UINT32 Get_Compared_Checksum(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :To return the compared checksum
Allocated Requirements:
Design Requirements:


Interfaces
    Calls           :   None

    Called by       :   COMM_SM.c   -   Build_Interprocess_Reset_Info_Message()


Input Variables         Name                                                Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                           Enumerator
                        DAC_UNIT_TYPE_SF                                    Enumerator
                        DAC_UNIT_TYPE_EF                                    Enumerator
                        DAC_UNIT_TYPE_CF                                    Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_D3_A                                  Enumerator
                        DAC_UNIT_TYPE_D3_B                                  Enumerator
                        DAC_UNIT_TYPE_D3_C                                  Enumerator
                        Checksum_Info.SF.Flags.DS1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.SF.Flags.DS2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.EF.Flags.US1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.EF.Flags.US2_Checksum_Comparison_Ok   Bit

    Local           :uchchecksum                        UINT32

Output Variables        Name                            Type
    Global          :

    Local           :uchchecksum                        UINT32

Signal Variables



Macros defined      :               Macro                   Value
                                    TRUE                     1

References          :

Derived Requirements:

************************************************************************/

UINT32 Get_Compared_Checksum(void)
{
   UINT32 uchchecksum = 0;
    switch (DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_SF:

                 if(Checksum_Info.SF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                    Checksum_Info.SF.Flags.DS2_Checksum_Comparison_Ok == TRUE)
                    {
                     uchchecksum = CPU_Calculated_Checksum;
                    }
                break;
        case DAC_UNIT_TYPE_EF:
                  if(Checksum_Info.EF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.EF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                    {
                     uchchecksum = CPU_Calculated_Checksum;
                    }
                break;
       case DAC_UNIT_TYPE_CF:
       case DAC_UNIT_TYPE_3D_SF:
       case DAC_UNIT_TYPE_3D_EF:
                  if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                      {
                        uchchecksum = CPU_Calculated_Checksum;
                       }
                break;
       case DAC_UNIT_TYPE_D3_A:
                  if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                      {
                        uchchecksum = CPU_Calculated_Checksum;
                       }
                break;
      case DAC_UNIT_TYPE_D3_B:
                  if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                      {
                        uchchecksum = CPU_Calculated_Checksum;
                       }
                break;
     case DAC_UNIT_TYPE_D3_C:
                  if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                      {
                        uchchecksum = CPU_Calculated_Checksum;
                       }
                break;
     case DAC_UNIT_TYPE_D4_A:
     case DAC_UNIT_TYPE_D4_B:
     case DAC_UNIT_TYPE_D4_C:
     case DAC_UNIT_TYPE_D4_D:
                  if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                     Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                      {
                        uchchecksum = CPU_Calculated_Checksum;
                       }
                break;
        default:
                break;
     }
   return(uchchecksum);
}

/*********************************************************************
Component name      :RESTORE
Module Name         :void Compare_DS_Checksum(bitadrb_t Config)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :To return the status flag for DS compared checksum
Allocated Requirements:	(SSDAC_SWRS_0840),(SSDAC_SWRS_0844),(SSDAC_SWRS_0845),(SSDAC_SWRS_0848),(SSDAC_SWRS_0853),
						(SSDAC_SWRS_0854),(SSDAC_SWRS_0855),(SSDAC_SWRS_0856),(SSDAC_SWRS_0857),(SSDAC_SWRS_0858)
Design Requirements:

Interfaces
    Calls           :   ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   COMM_DS.c   -   Process_DS_Reset_Info_Message()


Input Variables         Name                                                Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                           Enumerator
                        DAC_UNIT_TYPE_SF                                    Enumerator
                        DAC_UNIT_TYPE_CF                                    Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_D3_A                                  Enumerator
                        DAC_UNIT_TYPE_D3_B                                  Enumerator
                        DAC_UNIT_TYPE_D3_C                                  Enumerator
                        Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.SF.Local_Checksum.LWord               UINT32
                        Checksum_Info.CF.Local_Checksum.LWord               UINT32
                        Calculated_Checksum                                 UINT32
                        Checksum_Info.SF.DS1_Checksum.LWord                 UINT32
                        Checksum_Info.SF.DS2_Checksum.LWord                 UINT32
                        Checksum_Info.CF.DS1_Checksum.LWord                 UINT32
                        Checksum_Info.CF.DS2_Checksum.LWord                 UINT32

    Local           :   Config                                                  bitadrb_t

Output Variables        Name                                                Type
    Global          :   Status.Flags.System_Status                          Bit
                        Checksum_Info.SF.Flags.DS_Checksum_Comparison_Ok    Bit
                        Checksum_Info.SF.Flags.DS1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.SF.Flags.DS2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok    Bit

    Local           :

Signal Variables



Macros defined      :               Macro                   Value
                                    TRUE                     1
                                    INCORRECT_CODE_CRC_ERROR_NUM    57
                                    CATASTROPHIC_ERROR       0

References          :

Derived Requirements:

Algorithm           :1.Check the type of the unit
                     2.Compare the downstream CPU1 checksum with the Local calculated checksum,
                        if it doesn't matches set the "INCORRECT CODE CRC" error id otherwise
                        set the DS1 checksum flag as "TRUE".
                     3.Compare the downstream CPU2 checksum with the Local calculated checksum,
                        if it doesn't matches set the "INCORRECT CODE CRC" error id otherwise
                        set the DS2 checksum flag as "TRUE".
                     4.If both DS1 and DS2 checksum flag got setted then set the US checksum comparision flag as "TRUE"
                     5.Similar comparision will take place for all type of configurations
************************************************************************/

void Compare_DS_Checksum(bitadrb_t Config)
{
    switch (DIP_Switch_Info.DAC_Unit_Type)
        {
            case DAC_UNIT_TYPE_SF:

                     Checksum_Info.SF.Local_Checksum.LWord  = CPU_Calculated_Checksum;
                     if (Config.Bit.b4)
                       {
                         if(Checksum_Info.SF.Local_Checksum.LWord == Checksum_Info.SF.DS1_Checksum.LWord)
                           {
                              Checksum_Info.SF.Flags.DS1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.SF.Local_Checksum.LWord == Checksum_Info.SF.DS2_Checksum.LWord)
                            {
                             Checksum_Info.SF.Flags.DS2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.SF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.SF.Flags.DS2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.SF.Flags.DS_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
                case DAC_UNIT_TYPE_CF:
             case DAC_UNIT_TYPE_3D_SF:
             case DAC_UNIT_TYPE_3D_EF:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;
                      if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
                case DAC_UNIT_TYPE_D3_A:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;
                      if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
              case DAC_UNIT_TYPE_D3_B:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;
                      if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
              case DAC_UNIT_TYPE_D3_C:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;
                      if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
              case DAC_UNIT_TYPE_D4_A:
              case DAC_UNIT_TYPE_D4_B:
              case DAC_UNIT_TYPE_D4_C:
              case DAC_UNIT_TYPE_D4_D:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;
                      if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.DS2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
              default:
                     break;

        }
}
/*********************************************************************
Component name      :RESTORE
Module Name         :void Compare_US_Checksum(bitadrb_t Config)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :To return the status flag for US compared checksum
Allocated Requirements:	(SSDAC_SWRS_0845),(SSDAC_SWRS_0848),(SSDAC_SWRS_0849),(SSDAC_SWRS_0852),(SSDAC_SWRS_0853)
						(SSDAC_SWRS_0854),(SSDAC_SWRS_0855),(SSDAC_SWRS_0856),(SSDAC_SWRS_0857),(SSDAC_SWRS_0858)
Design Requirements:	


Interfaces
    Calls           :   ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   COMM_US.c   -   Process_US_Reset_Info_Message()


Input Variables         Name                                                Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                           Enumerator
                        DAC_UNIT_TYPE_EF                                    Enumerator
                        DAC_UNIT_TYPE_CF                                    Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_D3_A                                  Enumerator
                        DAC_UNIT_TYPE_D3_B                                  Enumerator
                        DAC_UNIT_TYPE_D3_C                                  Enumerator
                        Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.EF.Local_Checksum.LWord               UINT32
                        Checksum_Info.CF.Local_Checksum.LWord               UINT32
                        Calculated_Checksum                                 UINT32
                        Checksum_Info.EF.US1_Checksum.LWord                 UINT32
                        Checksum_Info.EF.US2_Checksum.LWord                 UINT32
                        Checksum_Info.CF.US1_Checksum.LWord                 UINT32
                        Checksum_Info.CF.US2_Checksum.LWord                 UINT32

    Local           :   Config                                                  bitadrb_t

Output Variables        Name                                                Type
    Global          :   Status.Flags.System_Status                          Bit
                        Checksum_Info.EF.Flags.US_Checksum_Comparison_Ok    Bit
                        Checksum_Info.EF.Flags.US1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.EF.Flags.US2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok    Bit

    Local           :

Signal Variables



Macros defined      :               Macro                   Value
                                    TRUE                     1
                                    INCORRECT_CODE_CRC_ERROR_NUM    57
                                    CATASTROPHIC_ERROR       0

References          :

Derived Requirements:

Algorithm           :1.Check the type of the unit
                     2.Compare the upstream CPU1 checksum with the Local calculated checksum,
                        if it doesn't matches set the "INCORRECT CODE CRC" error id otherwise
                        set the US1 checksum flag as "TRUE".
                     3.Compare the upstream CPU2 checksum with the Local calculated checksum,
                        if it doesn't matches set the "INCORRECT CODE CRC" error id otherwise
                        set the US2 checksum flag as "TRUE".
                     4.If both US1 and US2 checksum flag got setted then set the US checksum comparision flag as "TRUE"
                     5.Similar comparision will take place for all type of configurations
************************************************************************/

void Compare_US_Checksum(bitadrb_t Config)
{
    switch (DIP_Switch_Info.DAC_Unit_Type)
        {

            case DAC_UNIT_TYPE_EF:
                      Checksum_Info.EF.Local_Checksum.LWord  = CPU_Calculated_Checksum;
                       if (Config.Bit.b4)
                       {
                         if(Checksum_Info.EF.Local_Checksum.LWord == Checksum_Info.EF.US1_Checksum.LWord)
                           {
                              Checksum_Info.EF.Flags.US1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.EF.Local_Checksum.LWord == Checksum_Info.EF.US2_Checksum.LWord)
                            {
                             Checksum_Info.EF.Flags.US2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.EF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.EF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.EF.Flags.US_Checksum_Comparison_Ok = TRUE;
                          }

                     break;

            case DAC_UNIT_TYPE_CF:
             case DAC_UNIT_TYPE_3D_SF:
             case DAC_UNIT_TYPE_3D_EF:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;

                       if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
           case DAC_UNIT_TYPE_D3_A:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;

                       if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
            case DAC_UNIT_TYPE_D3_B:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;

                       if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
           case DAC_UNIT_TYPE_D3_C:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;

                       if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
           case DAC_UNIT_TYPE_D4_A:
           case DAC_UNIT_TYPE_D4_B:
           case DAC_UNIT_TYPE_D4_C:
           case DAC_UNIT_TYPE_D4_D:
                     Checksum_Info.CF.Local_Checksum.LWord  = CPU_Calculated_Checksum;

                       if (Config.Bit.b4)
                       {
                         if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US1_Checksum.LWord)
                           {
                              Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                            {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                            }
                        }
                     else
                        {
                          if(Checksum_Info.CF.Local_Checksum.LWord == Checksum_Info.CF.US2_Checksum.LWord)
                            {
                             Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok = TRUE;
                            }
                          else
                             {
                              Status.Flags.System_Status  = CATASTROPHIC_ERROR;
                              Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
                             }
                         }
                      if(Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok == TRUE &&
                         Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok == TRUE)
                          {
                            Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok = TRUE;
                          }
                     break;
           default:
                 break;
        }
}
/*********************************************************************
Component name      :RESTORE
Module Name         :void Clear_DS_Checksum_Info(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :To clear the Down stream checksum flags
Allocated Requirements:	(SSDAC_SWRS_0840),(SSDAC_SWRS_0844),(SSDAC_SWRS_0845),(SSDAC_SWRS_0848),(SSDAC_SWRS_0853),
						(SSDAC_SWRS_0854),(SSDAC_SWRS_0855),(SSDAC_SWRS_0856),(SSDAC_SWRS_0857),(SSDAC_SWRS_0858)
Design Requirements:	

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        RESET.c     -   CF_DS_Reset_Sequence_Complete()


Input Variables         Name                                                Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                           Enumerator
                        DAC_UNIT_TYPE_SF                                    Enumerator
                        DAC_UNIT_TYPE_CF                                    Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator


    Local           :   None

Output Variables        Name                                                Type
    Global          :   Status.Flags.System_Status                          Bit
                        Checksum_Info.SF.Flags.DS_Checksum_Comparison_Ok    Bit
                        Checksum_Info.SF.Flags.DS1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.SF.Flags.DS2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok    Bit

    Local           :

Signal Variables



Macros defined      :               Macro                   Value
                                    FALSE                    1


References          :

Derived Requirements:

************************************************************************/
void Clear_DS_Checksum_Info(void)
{

    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
                Checksum_Info.SF.Flags.DS1_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.SF.Flags.DS2_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.SF.Flags.DS_Checksum_Comparison_Ok = FALSE;
            break;
        case DAC_UNIT_TYPE_CF:
             case DAC_UNIT_TYPE_3D_SF:
             case DAC_UNIT_TYPE_3D_EF:
                Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok = FALSE;
            break;
        default:
                Checksum_Info.CF.Flags.DS1_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.CF.Flags.DS2_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok = FALSE;
            break;
     }

}

/*********************************************************************
Component name      :RESTORE
Module Name         :void Clear_US_Checksum_Info(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :To clear the Up stream checksum flags
Allocated Requirements:	(SSDAC_SWRS_0845),(SSDAC_SWRS_0848),(SSDAC_SWRS_0849),(SSDAC_SWRS_0852),(SSDAC_SWRS_0853)
						(SSDAC_SWRS_0854),(SSDAC_SWRS_0855),(SSDAC_SWRS_0856),(SSDAC_SWRS_0857),(SSDAC_SWRS_0858)
Design Requirements:	

Interfaces
    Calls           :   None

    Called by       :   DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        RESET.c     -   CF_US_Reset_Sequence_Complete()


Input Variables         Name                                                Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type                           Enumerator
                        DAC_UNIT_TYPE_EF                                    Enumerator
                        DAC_UNIT_TYPE_CF                                    Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator


    Local           :   None

Output Variables        Name                                                Type
    Global          :   Status.Flags.System_Status                          Bit
                        Checksum_Info.EF.Flags.US_Checksum_Comparison_Ok    Bit
                        Checksum_Info.EF.Flags.US1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.EF.Flags.US2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok   Bit
                        Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok    Bit

    Local           :

Signal Variables



Macros defined      :               Macro                   Value
                                    FALSE                    1


References          :

Derived Requirements:

************************************************************************/
void Clear_US_Checksum_Info(void)
{

    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_EF:
                Checksum_Info.EF.Flags.US1_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.EF.Flags.US2_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.EF.Flags.US_Checksum_Comparison_Ok = FALSE;
            break;
        case DAC_UNIT_TYPE_CF:
             case DAC_UNIT_TYPE_3D_SF:
             case DAC_UNIT_TYPE_3D_EF:
                Checksum_Info.CF.Flags.US1_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.CF.Flags.US2_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok = FALSE;
             break;
       default:
                Checksum_Info.EF.Flags.US1_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.EF.Flags.US2_Checksum_Comparison_Ok = FALSE;
                Checksum_Info.EF.Flags.US_Checksum_Comparison_Ok = FALSE;
            break;
     }

}
