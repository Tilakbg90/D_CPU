/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    DAC_MAIN
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
    Functions   :   void main(void)
                    void Initialise_System(void)
                    void Start_SF_Reset_Sequence(void)
                    void Start_CF_Reset_Sequence(void)
                    void Start_D3_Reset_Sequence(void)
                    void Start_EF_Reset_Sequence(void)
                    void Start_DE_Reset_Sequence(void)
                    void Control_DAC_Type_SF(void)
                    void Control_DAC_Type_CF(void)
                    void Control_DAC_Type_EF(void)
                    void Control_DAC_Type_DE(void)
                    void Control_DAC_Type_D3(void)
                    void DisplayDAC_InfoLine(void)
                    void Decrement_Bootuplock_50msTmr(void)

*****************************************************************************/
#include <xc.h>

//#include <sys.h>
/*
 * Set configuration bits/fuses
 */
#pragma config WPFP = WPFP255           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config SOSCSEL = EC           // Secondary Oscillator Power Mode Select (Secondary oscillator is in Default (high drive strength) Oscillator mode)
#pragma config WUTSEL = LEG             // Voltage Regulator Wake-up Time Select (Default regulator start-up time is used)
#pragma config ALTPMP = ALPMPDIS        // Alternate PMP Pin Mapping (EPMP pins are in default location mode)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Segmented code protection is disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Last page (at the top of program memory) and Flash Configuration Words are not write-protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Protected code segment upper boundary is at the last page of program memory; the lower boundary is the code page specified by WPFP)

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode is selected)
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable (The IOLOCK bit can be set and cleared as needed, provided the unlock sequence has been completed)
#pragma config OSCIOFNC = OFF           // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Fail-Safe Clock Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Initial Oscillator Select (Primary Oscillator with PLL module (XTPLL, HSPLL, ECPLL))
#pragma config PLL96MHZ = ON            // 96MHz PLL Startup Select (96 MHz PLL is enabled automatically on start-up)
#pragma config PLLDIV = DIV2            // 96 MHz PLL Prescaler Select (Oscillator input is divided by 2 (8 MHz input))
#pragma config IESO = OFF               // Internal External Switchover (IESO mode (Two-Speed Start-up) is disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config ALTVREF = ALTVREDIS      // Alternate VREF location Enable (VREF is on a default pin (VREF+ on RA9 and VREF- on RA10))
#pragma config WINDIS = OFF             // Windowed WDT (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC1/PGED1)
#pragma config GWRP = OFF               // General Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)



#include "COMMON.h"
#include "AXLE_MON.h"
#include "RELAYMGR.h"
#include "COMM_DS.h"
#include "COMM_US.h"
#include "COMM_SM.h"
#include "RELAYDRV.h"
#include "SYS_MON.h"
#include "RESET.h"
#include "ERROR.h"
#include "RESTORE.h"
#include "AES.h"




const unsigned char SW_Version_No = 0x01;   /* constant hold the value of software version number to display in reset box*/
const BYTE BitMask_List[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
 /*near*/  dip_switch_info_t DIP_Switch_Info;     /* Structure to hold DIP switch settings */
 /*near*/  dac_status_t Status __attribute__((persistent));                   /* Status flags of Unit. This will be passed as it is to SM-CPU */
 /*near*/  bootup_lock Bootup_Lock;                    /* Structure to hold the value for the bootup timeout variable*/
checksum_info_t Checksum_Info;              /*structure to hold the CRC values*/
ds_section_mode DS_Section_Mode;            /*structure to hold the DS section local and remote unit modes */
us_section_mode US_Section_Mode;            /*structure to hold the US section local and remote unit modes */
BYTE uchIDInfo_List[20];
BYTE Always_true = 1;



union def_SPI_Failure SPI_Failure;



void Initialise_System(void);
void Start_SF_Reset_Sequence(void);
void Start_CF_Reset_Sequence(void);
void Start_D3_Reset_Sequence(void);
void Start_D4_Reset_Sequence(void);
void Start_EF_Reset_Sequence(void);

void Start_LCWS_Reset_Sequence(void);
void Start_DE_Reset_Sequence(void);

void Control_DAC_Type_SF(void);
void Control_DAC_Type_CF(void);
void Control_DAC_Type_EF(void);
void Control_DAC_Type_D3(void);
void Control_DAC_Type_D4(void);
void Control_DAC_Type_3S(void);

void Initialize_Reset(void);
void Update_SPI_Error(void);
void Control_DAC_Type_LCWS(void);
void Control_DAC_Type_DE(void);

extern void Update_LCWS_Reset_Monitor_State(void);
extern void Initialise_LCWS_Reset_Monitor(void);
extern void Decrement_TrackMon_50msTmr_1(void);
extern void Decrement_TrackMon_50msTmr_2(void);
extern void Decrement_TrackMon_50msTmr_3(void);
extern void Decrement_TrackMon_50msTmr_4(void);
extern void Update_LCWS_Relay_State(void);
extern void Update_LCWS_DL_Relay_State(void);
extern void US_LED_Error_Service_1mS(void);
extern void DS_LED_Error_Service_1mS(void);
extern void Start_LCWS_Reset_Monitor(void);
extern void Initialise_DE_Reset_Monitor(void);
extern void Start_DE_Reset_Monitor(void);
extern void Update_DE_Reset_Monitor_State(void);
extern void Update_DE_Relay_State(void);
extern void Decrement_comm_check_50msTmr(void);

extern unsigned int COM_DS_pkt_error_cnt,COM_US_pkt_error_cnt;
extern BYTE Reset_pressed __attribute__((persistent));  
/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void main(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Main function which calls all other modules.

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   persist_check() -   Compiler defined function
                        CLRWDT()        -   Compiler defined function
                        DAC_MAIN.c      -   Initialise_System()
                        RESET.c         -   Start_SF_Reset_Sequence()
                        RLYB_MGR.c      -   Start_Relay_B_Mgr()
                        LCD_DRV.c       -   Clear_Line_on_LCD()
                        DAC_MAIN.c      -   Control_DAC_Type_SF()
                        RESET.c         -   Start_EF_Reset_Sequence()
                        RLYA_MGR.c      -   Start_Relay_A_Mgr()
                        DAC_MAIN.c      -   Control_DAC_Type_EF()
                        RESET.c         -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c      -   Control_DAC_Type_CF()
                        RLYD3_MGR.c     -   Start_Relay_D3_Mgr()
                        DAC_MAIN.c      -   Control_DAC_Type_D3()
                        RLYDE_MGR.c     -   Start_Relay_DE_Mgr()
                        DAC_MAIN.c      -   Control_DAC_Type_DE()

Input Variables         Name                                Type
    Global          :   DIP_Switch_Info.DAC_Unit_Type           DAC_Unit_Type
                        DAC_UNIT_TYPE_SF                    Enumerator
                        DAC_UNIT_TYPE_EF                    Enumerator
                        DAC_UNIT_TYPE_CF                    Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_D3_A                  Enumerator
                        DAC_UNIT_TYPE_D3_B                  Enumerator
                        DAC_UNIT_TYPE_D3_C                  Enumerator
                        DAC_UNIT_TYPE_DE                    Enumerator


    Local           :   None

Output Variables        Name                                Type
    Global          :   TMR0ON                              TIMER0 Control register bit
                        TMR1ON                              TIMER1 Control register bit
                        TMR2ON                              TIMER2 Control register bit
                        TMR3ON                              TIMER3 Control register bit
                        TMR4ON                              TIMER4 Control register bit
                        Status.Flags.Preparatory_Reset      Bit
                        Status.Flags.Local_Power_Status     Bit


    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                           Value
                            ZERO_OUT_IF_INVALID             1
                            SET_HIGH                        1
                            PREPARATORY_RESET_PENDING       0
                            TRUE                            1
                            PREPARATORY_RESET_COMPLETED     0

References          :

Derived Requirements:

*********************************************************************************/
int main(void)
{
    /*
     * Initialise persistent variables, in case Magic number and
     */
    ClrWdt(); /*lint -e526 -e628 */
    Initialise_System();
    ClrWdt(); /*lint -e526 -e628 */
    if(RCONbits.SWR == 0)   // Do this only upon normal power up.
    {
        Initialize_Reset();
    }
    else
    {
        if(Reset_pressed == TRUE)
        {
            Status.Flags.Local_Power_Status = SET_HIGH;
        }
        Reset_pressed = FALSE;
    }
        
            switch (DIP_Switch_Info.DAC_Unit_Type)
            {
            case DAC_UNIT_TYPE_SF:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING; /* Waiting for Reset */
                Start_SF_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_COMPLETED;
                Start_Relay_B_Mgr();                                        /* from rlyb_mgr.c */
                // Clear_Line_on_LCD((BYTE) 3);
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_SF();
                break;
            case DAC_UNIT_TYPE_EF:
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
                Start_EF_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_COMPLETED;
                Start_Relay_A_Mgr();                                        /* from rlya_mgr.c */
                // Clear_Line_on_LCD((BYTE) 3);
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_EF();
                break;
            case DAC_UNIT_TYPE_CF:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
                Start_CF_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_CF();
                break;
            case DAC_UNIT_TYPE_D3_A:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
                Start_D3_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_COMPLETED;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_COMPLETED;
                Start_Relay_D3_Mgr();                                       /* from rlyd3_mgr.c */
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_D3();
                break;
            case DAC_UNIT_TYPE_D3_B:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
                Start_D3_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_COMPLETED;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_COMPLETED;
                Start_Relay_D3_Mgr();                                       /* from rlyd3_mgr.c */
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_D3();
                break;
            case DAC_UNIT_TYPE_D3_C:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
                Start_D3_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_COMPLETED;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_COMPLETED;
                Start_Relay_D3_Mgr();                                       /* from rlyd3_mgr.c */
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_D3();
                break;
            case DAC_UNIT_TYPE_3D_SF:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
                Start_CF_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_3S();
                break;
            case DAC_UNIT_TYPE_3D_EF:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
                Start_CF_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_3S();
                break;
            case DAC_UNIT_TYPE_LCWS:
            case DAC_UNIT_TYPE_LCWS_DL:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING; /* Waiting for Reset */
                Start_LCWS_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_COMPLETED;
                Start_Relay_A_Mgr();
                Start_Relay_B_Mgr();                                        /* from rlyb_mgr.c */
                // Clear_Line_on_LCD((BYTE) 3);
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_LCWS();
                break;
            case DAC_UNIT_TYPE_DE:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING; /* Waiting for Reset */
                Start_DE_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_COMPLETED;
                Start_Relay_B_Mgr();                                        /* from rlyb_mgr.c */
                // Clear_Line_on_LCD((BYTE) 3);
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_DE();
                break;
            case DAC_UNIT_TYPE_D4_A:
            case DAC_UNIT_TYPE_D4_B:
            case DAC_UNIT_TYPE_D4_C:
            case DAC_UNIT_TYPE_D4_D:
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_PENDING;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_PENDING;
                Start_D4_Reset_Sequence();
                Status.Flags.Local_Power_Status = SET_HIGH;
                Status.Flags.Preparatory_Reset_DS = PREPARATORY_RESET_COMPLETED;
                Status.Flags.Preparatory_Reset_US = PREPARATORY_RESET_COMPLETED;
                Start_Relay_D4_Mgr();                                       /* from rlyd3_mgr.c */
                COM_DS_pkt_error_cnt = 0;
                COM_US_pkt_error_cnt = 0;
                Control_DAC_Type_D4();
                break;
            default:
                /* Call dead end configuration function to display wrong address */
                //Start_DE_Reset_Sequence();
                break;
            }
         
    return(0);
}
/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Initialise_System(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Initialise all I/O ports,timer configuration,port direction,
                     disable all priority interrupt levels,disable A/D converter,
                     Disable Capture/Comparator/PWM  Modules,default status  values,
                     calls all initialisation function modules.

Allocated Requirements	: 	(SSDAC_SWRS_0023), (SSDAC_SWRS_0024), (SSDAC_SWRS_0025), (SSDAC_SWRS_0026)
							(SSDAC_SWRS_0028), (SSDAC_SWRS_0029), (SSDAC_SWRS_0030), (SSDAC_SWRS_0031)
							(SSDAC_SWRS_0032), (SSDAC_SWRS_0033), (SSDAC_SWRS_0034), (SSDAC_SWRS_0835)
							(SSDAC_SWRS_0834), (SSADC_SWRS_0036), (SSADC_SWRS_0037),(SSADC_SWRS_0838)
							(SSADC_SWRS_0839), (SSDAC_SWRS_0243), (SSDAC_SWRS_0438), (SSDAC_SWRS_0203)
							(SSDAC_SWRS_0103), (SSDAC_ SWRS_ 0148), (SSDAC_SWRS_0446), (SSDAC_SWRS_0260)
							(SSDAC_SWRS_0403), (SSDAC_SWRS_0503), (SSDAC_SWRS_0540), (SSDAC_SWRS_0243)
							(SSDAC_SWRS_0303), (SSDAC_SWRS_0305), (SSDAC_SWRS_0604), (SSDAC_SWRS_0703)
							(SSDAC_SWRS_0640), (SSDAC_SWRS_0740), (SSDAC_SWRS_0339), (SSDAC_SWRS_0339)

Design Requirements		:	SSDAC_DR_5002

Interfaces
    Calls           :       ERROR.c     -   Initialise_Error()
                            LCD_DRV.c   -   Initialise_LCD_Driver()
                            RESTORE.c   -   Check_DIP_Switches()
                            RESTORE.c   -   Check_DAC_Boards_Runtime()
                            RESTORE.c   -   Check_Flash()
                            COMM_SM.c   -   Initialise_SPI()
                            COMM_US.c   -   SetupCOM1BaudRate()
                            COMM_DS.c   -   SetupCOM2BaudRate()
                            COMM_DS.c   -   Initialise_DS_CommSch()
                            COMM_US.c   -   Initialise_US_CommSch()
                            AXLE_MON.c  -   Initialise_AxleMon()
                            RELAYMGR.c  -   Initialise_Relay_Mgr()
                            SYS_MON.c   -   Initialise_Sys_Mon()
                            DAC_MAIN.c  -   DisplayDAC_InfoLine()
                            ERROR.c     -   Set_Error_Status_Bit()
                            WRITETIMER0()   -   Compiler defined function

    Called by       :   main()

Input Variables         Name                                        Type
    Global          :   Status.Flags.System_Status                  Bit
                        DIP_Switch_Info.DAC_Unit_Type                   DAC_Unit_Type
                        DAC_UNIT_TYPE_DE                            Enumerator
                        DAC_UNIT_TYPE_SF                            Enumerator
                        DAC_UNIT_TYPE_CF                            Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_D3_A                          Enumerator
                        DAC_UNIT_TYPE_D3_B                          Enumerator
                        DAC_UNIT_TYPE_D3_C                          Enumerator

    Local           :   uchString                                   Array of BYTEs

Output Variables        Name                                    Type
    Global          :   PORTA                                   16 Bit Bidirectional Port
                        PORTB                                   16 Bit Bidirectional Port
                        PORTC                                   16 Bit Bidirectional Port
                        PORTD                                   16 Bit Bidirectional Port
                        PORTE                                   16 Bit Bidirectional Port
                        PORTF                                   16 Bit Bidirectional Port
                        PORTG                                   16 Bit Bidirectional Port
                        PORTH                                   16 Bit Bidirectional Port
                        PORTJ                                   16 Bit Bidirectional Port
                        TRISA                                   16 Bit directional register
                        TRISB                                   16 Bit directional register
                        TRISC                                   16 Bit directional register
                        TRISD                                   16 Bit directional register
                        TRISE                                   16 Bit directional register
                        TRISF                                   16 Bit directional register
                        TRISG                                   16 Bit directional register
                        TRISH                                   16 Bit directional register
                        TRISJ                                   16 Bit directional register
                        ADCON0                                  16 Bit Control register 0
                        ADCON1                                  16 Bit Control register 1
                        EBDIS                                   MEMCON register bit
                        CMCON                                   Comparator Module control register
                        CCP1CON                                 Enhanced Capture/Compare/PWM Control Register
                        CCP2CON                                 Enhanced Capture/Compare/PWM Control Register
                        CCP3CON                                 Enhanced Capture/Compare/PWM Control Register
                        CCP4CON                                 Enhanced Capture/Compare/PWM Control Register
                        CCP5CON                                 Enhanced Capture/Compare/PWM Control Register
                        T0CON                                   Timer 0 Control register
                        T1CON                                   Timer 1 Control register
                        T2CON                                   Timer 2 Control register
                        T3CON                                   Timer 3 Control register
                        T4CON                                   Timer 4 Control register
                        PR4                                     16 bit period register
                        PR2                                     16 bit period register
                        Status.Byte[0 to 6]                     Array of Bytes
                        Checksum_Info.Byte[0 to 20]             Array of Bytes
                        DIP_Switch_Info.Address                 BYTE
                        DIP_Switch_Info.DAC_Unit_Type               DAC_Unit_Type
                        DIP_Switch_Info.Baud_Rate               Baud_Rate_Config
                        DIP_Switch_Info.Peer_Address            BYTE
                        Bootup_Lock.Timeout_50ms                UINT16


    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                       Value
                            PORT_A_DIRECTION            0b00110000
                            PORT_B_DIRECTION            0b00111011
                            PORT_C_DIRECTION            0b10011101
                            PORT_D_DIRECTION            0b00000001
                            PORT_E_DIRECTION            0b11011000
                            PORT_F_DIRECTION            0b10000000
                            PORT_G_DIRECTION            0b00000101
                            PORT_H_DIRECTION            0b10000010
                            PORT_J_DIRECTION            0b11111110
                            PORT_A_DEFAULT              0b00000000
                            PORT_B_DEFAULT              0b10000000
                            PORT_C_DEFAULT              0b00000000
                            PORT_D_DEFAULT              0b00000000
                            PORT_E_DEFAULT              0b00100000
                            PORT_F_DEFAULT              0b10001111
                            PORT_G_DEFAULT              0b00000000
                            PORT_H_DEFAULT              0b00000000
                            PORT_J_DEFAULT              0b00000001
                            NORMAL                      1
                            CRC32_CHECKSUM_BAD          0
                            INCORRECT_CODE_CRC_ERROR_NUM       57
                            SEVEN_SEGMENT_UNIT1         1
                            SEVEN_SEGMENT_UNIT2         2
                            BOOTUP_RESET_TIMEOUT        120
References          :

Derived Requirements:

*********************************************************************************/
void Initialise_System(void)
{

    //BYTE uchString[21];

    // Configure All ports as digital I/O
    ANSA = 0x0000;
    ANSB = 0x0000;
    ANSC = 0x0000;
    ANSD = 0x0000;
    ANSE = 0x0000;
    ANSF = 0x0000;
    ANSG = 0x0000;

    // Unused and Not Connected Ports were TRIS'ed 1
    TRISA = 0b1011100100001111;
    TRISB = 0b1111111111110011;
    TRISC = 0b1011111111111111;
    TRISD = 0b0110101111000111;
    TRISE = 0b1111111011111111;
    TRISF = 0b1101111011101001;
    TRISG = 0b1111111001111101;

    // All LATs made Output, except DIP Switch Selection Bits.
    LATA = 0x06F0;  // Make the DIP Switch seletion lines High
    LATB = 0x0008;
    LATC = 0x0000;
    LATD = 0x0000;
    LATE = 0x0000;
    LATF = 0x0000;
    LATG = 0x0000;

    // Enable Pull ups for the DIP Switch Input Pins
    CNPU3bits.CN33PUE = 1;
    CNPU3bits.CN34PUE = 1;
    CNPU3bits.CN35PUE = 1;
    CNPU3bits.CN36PUE = 1;

    // Speed Detection Timer - 1
    T1CON = 0;
    T1CONbits.TCKPS = 0b11;     // Timer Prescaler is 1:64. So, 62.5nS * 64 = 4uS is the Clock to Timer
    TMR1 = 0;
    PR1 = 0xFFFF;
    IEC0bits.T1IE = 0;
    IFS0bits.T1IF = 0;


    // Configure Timer 2 to run at 16MHz, interrupt flag to set once in 1mS

    T5CON = 0;
    TMR5 = 0;               // Reset Timer to 0
    PR5 = 250;            // 1mS / 62.5nS = 16000.

    T5CONbits.TCKPS = 0b10; // 1:64, so Timer Clock will be 4000nS

    IEC1bits.T5IE = 0;      // Disable Interrupts for Timer 2
    IFS1bits.T5IF = 0;      // Clear Interrupt Flag
    T5CONbits.TON = 1;      // Start Timer

    // Configure Timer 3 to run at 16MHz/64 and interrupt flag to set once in 50mS

    T3CON = 0;
    T3CONbits.TCKPS = 0b10; // Prescaler is 1:64. So, clock to timer counter is 16MHz / 64 = 250KHz
    TMR3 = 0;               //Reset Timer to 0
    PR3 = 12500;            // 250KHz, t = 4uS. 50ms / 4uS = 12500
    IEC0bits.T3IE = 0;      // Disable Interrupt for Timer 3
    IFS0bits.T3IF = 0;      // Clear Interrupt Flag
    T3CONbits.TON = 1;      // Start the Timer.

    //PR4 = TIMER4_SETPOINT;

    ErrorCodeForSPI = 255;
    SPI_Failure.Failure_Data = 0;

    Status.Byte[0] = (BYTE) 0b00000001;
    Status.Byte[1] = (BYTE) 0b00111110;
    Status.Byte[2] = (BYTE) 0b11111111;
    Status.Byte[3] = (BYTE) 0b11111111;
    Status.Byte[4] = (BYTE) 0b11111111;
    Status.Byte[5] = (BYTE) 0b00000000;
    Status.Byte[6] = (BYTE) 0b00100011;
    
//    if(RCONbits.SWR == 1)   // Do this only upon software reset
//    {
//        if(Reset_pressed == TRUE)
//        {
//            Status.Flags.Local_Power_Status = SET_HIGH;
//        }
//        Reset_pressed = FALSE;
//    }


    Checksum_Info.Byte[0]  = (BYTE) 0x00;
    Checksum_Info.Byte[1]  = (BYTE) 0x00;
    Checksum_Info.Byte[2]  = (BYTE) 0x00;
    Checksum_Info.Byte[3]  = (BYTE) 0x00;
    Checksum_Info.Byte[4]  = (BYTE) 0x00;
    Checksum_Info.Byte[5]  = (BYTE) 0x00;
    Checksum_Info.Byte[6]  = (BYTE) 0x00;
    Checksum_Info.Byte[7]  = (BYTE) 0x00;
    Checksum_Info.Byte[8]  = (BYTE) 0x00;
    Checksum_Info.Byte[9]  = (BYTE) 0x00;
    Checksum_Info.Byte[10] = (BYTE) 0x00;
    Checksum_Info.Byte[11] = (BYTE) 0x00;
    Checksum_Info.Byte[12] = (BYTE) 0x00;
    Checksum_Info.Byte[13] = (BYTE) 0x00;
    Checksum_Info.Byte[14] = (BYTE) 0x00;
    Checksum_Info.Byte[15] = (BYTE) 0x00;
    Checksum_Info.Byte[16] = (BYTE) 0x00;
    Checksum_Info.Byte[17] = (BYTE) 0x00;
    Checksum_Info.Byte[18] = (BYTE) 0x00;
    Checksum_Info.Byte[19] = (BYTE) 0x00;
    Checksum_Info.Byte[20] = (BYTE) 0x00;

    DIP_Switch_Info.Configuration       = (Unit_Configuration) 0x00;        /*Configuration*/
    DIP_Switch_Info.Address             = (BYTE) 0x00;      /* Unit Address */
    DIP_Switch_Info.DAC_Unit_Type       = (SSDAC_Unit_Type) 0x00;      /* Unit Type */
    DIP_Switch_Info.Baud_Rate           = (Baud_Rate_Config) 0x00;      /* 1200/2400/9600/19200 */
    DIP_Switch_Info.Peer_Address        = (BYTE) 0x00;      /* Address of Peer CPU */

    DIP_Switch_Info.COM1_Mode           = (comm_type) 0x00;
    DIP_Switch_Info.COM2_Mode           = (comm_type) 0x00;
    DIP_Switch_Info.COM1_NW_Address     = (BYTE) 0x00;
    DIP_Switch_Info.COM2_NW_Address     = (BYTE) 0x00;


    Bootup_Lock.Timeout_50ms = BOOTUP_RESET_TIMEOUT;

    //POR = 1;
    //BOR = 1;


    Initialise_Error();                             /* Initialise Error */
    //Initialise_LCD_Driver();                        /* from lcd_drv.c */
    Check_DIP_Switches();                           /* from restore.c */
    Check_DAC_Boards_Runtime();                     /* from restore.c */
    Check_Flash();                                  /* from restore.c */
    Check_RAM();                                    /* from restore.c */
    Initialise_SPI();                               /* from comm_sm.c */

    //PR2 = PWM_PERIOD;
    // Initialize OC for PWM and Pins for PWM Output
    // Set Direction registers

    TRISDbits.TRISD3 = OUTPUT_PORT;
    TRISDbits.TRISD5 = OUTPUT_PORT;


    /* Configure Remappable pins */
    //*************************************************************
    // Unlock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON & 0xbf); /*lint -e526 -e628 */ //clear the bit 6 of OSCCONL to unlock Pin Re-map

    RPOR11bits.RP22R = 18;             // Map RP22 to OC1 Output
    RPOR10bits.RP20R = 19;             // Map RP20 to OC2 Output
    //************************************************************
    // Lock Registers
    //************************************************************
    __builtin_write_OSCCONL(OSCCON | 0x40); /*lint -e526 -e628 */ //set the bit 6 of OSCCONL to lock Pin Re-map
    //******************************************************************
    // PWM for Vial Relay 1
    OC1CON1 = 0;
    OC1CON2 = 0;
    OC1CON1bits.OCTSEL = 0b111; // Fp is clock to PWM Counter
    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
    {
        OC1R = 133;                 // For Dutycycle of 25%
    }
    else
    {
        OC1R = 266;                 // For Dutycycle of 50%
    }
    OC1RS = 533;                // For 30KHz frequency
    OC1CON2bits.SYNCSEL = 0x1F; // Slef trigger

    OC1CON1bits.OCM = TURN_OFF_PWM_A;        // disable PWM for now.

    // PWM for Vial Relay 2
    OC2CON1 = 0;
    OC2CON2 = 0;
    OC2CON1bits.OCTSEL = 0b111; // Fp is clock to PWM Counter

    if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
    {
        OC2R = 133;                 // For Dutycycle of 25%
    }
    else
    {
        OC2R = 266;                 // For Dutycycle of 50%
    }
    
    OC2RS = 533;                // For 30KHz frequency
    OC2CON2bits.SYNCSEL = 0x1F; // Slef trigger

    OC2CON1bits.OCM = TURN_OFF_PWM_B;        // disable PWM for now.

    /*
     * Initialise sub-systems
     */

     PR2 = PWM_PERIOD_A;

    SetupCOM1BaudRate();                            /* from comm_us.c */
    SetupCOM2BaudRate();                            /* from comm_ds.c */
    Initialise_DS_CommSch();                        /* from comm_ds.c */
    Initialise_US_CommSch();                        /* from comm_us.c */
    Initialise_US_Crypto_State();                   /* from AES_US.c */
    Initialise_DS_Crypto_State();                   /* from AES_DS.c */

    if (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL)
    {
        Initialise_AxleMon_1();
        Initialise_AxleMon_2();
        Initialise_AxleMon_3();
        Initialise_AxleMon_4();
    }
    else if (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE)
    {
        Initialise_AxleMon_1();
    }
    else
    {
        Initialise_AxleMon();                           /* from axle_mon.c */
    }

    //Initialise_AxleMon();                           /* from axle_mon.c */
    Initialise_Relay_Mgr();                         /* from relaymgr.c */
    Initialise_Sys_Mon();                           /* Initialise System Monitor */
    //Initialise_DisplayManager();                    /* Initialise Display manager */

    if (Status.Flags.System_Status != NORMAL)
    {
        if(Status.Flags.Flash_CheckSum == CRC32_CHECKSUM_BAD)
        {
            Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
        }
    }


}
/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Start_SF_Reset_Sequence(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the Start fed reset sequence
                        1.Get the SF reset state
                        2.Start the Down stream communication scheduler
                        3.Update the reset state
                        4.In error condition update the error display and wait for 6s then reboot the system except
                          for the flash checksum failure
                        5.Update the down stream scehdular state, SPI state, LCD state

Allocated Requirements	: 	(SSDAC_SWRS_0221), (SSDAC_SWRS_0222),(SSDAC_SWRS_0244),(SSDAC_SWRS_0339),
							(SSDAC_SWRS_0842),(SSDAC_SWRS_0843)

Design Requirements		:	SSDAC_DR_5003

Interfaces
    Calls           :   
                        RESET.c     -   Get_SF_Reset_State()
                        RESET.c     -   Initialise_SF_Reset_Monitor()
                        RESET.c     -   Start_SF_Reset_Monitor()
                        COMM_DS.c   -   Start_DS_CommSch()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        COMM_DS.c   -   Decrement_DS_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        RESET.c     -   Update_SF_Reset_Monitor_State()
                        ERROR.c     -   Update_Display_Error()
                        DISPLAY.c   -   Update_itoa_SF_State()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function
                        RESET()     -   Compiler defined function
                        RESET.c     -   Get_SF_DS1_Reset_State()
                        RESET.c     -   Get_SF_DS2_Reset_State()
                        RESET.c     -   Decrement_Reset_300sTmr()
                        RESET.c     -   Decrement_Reset_300sTmr2()
                        RESTORE.c   -   Clear_DS_Checksum_Info()
                        DAC_MAIN.c  -   Decrement_Bootuplock_50msTmr()
                        RESET.c     -   Clear_Reset_State()

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                            Type
    Global          :   SF_WAIT_FOR_RESET               Enumerator
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status      Bit
                        SF_RESET_SEQUENCE_COMPLETED     Enumerator
                        RESET_APPLIED_MODE              Enumerator
                        SF_WAIT_FOR_DS_TO_RESET         Enumerator
                        Status.Flags.Flash_CheckSum     Bit
                        Bootup_Lock.Timeout_50ms        UINT16

    Local           :   uchPreparatoryState             BYTE
                        uchReset_Pending                BYTE
                        uchString                       Array of BYTEs
                        DS1_Reset_Status                BYTE
                        DS2_Reset_Status                BYTE

Output Variables        Name                            Type
    Global          :   Status.Flags.Local_Reset_Done   Bit
                        Status.Flags.Local_Reset_Done2  Bit
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        DS_Section_Mode.Local_Unit      BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                           Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            SM_HAS_RESETTED_SYSTEM          1
                            TRUE                            1
                            FALSE                           0
                            SET_LOW                         0
                            CRC32_CHECKSUM_BAD              0
                            TIMEOUT_EVENT                   0
References          :

Derived Requirements:

*********************************************************************************/
void Start_SF_Reset_Sequence(void)
{
    BYTE uchPreparatoryState;
    BYTE uchReset_Pending;
    BYTE DS1_Reset_Status;
    BYTE DS2_Reset_Status;
    BYTE Modem_State;

    /*
     * Station master has to ensure that there are no trains in the
     * controlled section by pressing the Preparatory reset button
     */



//    Reset_Info.SF.DS_State = SF_WAIT_FOR_RESET;
    uchPreparatoryState = Get_SF_Reset_State();
    if (uchPreparatoryState == (BYTE)SF_WAIT_FOR_RESET)
    {
        Initialise_SF_Reset_Monitor();
    }
    else
    {
        Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
        DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
        DS_Section_Mode.Remote_Unit = WAITING_FOR_RESET_MODE;
    }
    Start_SF_Reset_Monitor();
    uchReset_Pending = TRUE;

    Start_DS_CommSch();     /* Start Downstream communication scheduler */

    while (uchReset_Pending)
    {
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16)TIMER3_SETPOINT);
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_Bootuplock_50msTmr();
            uchPreparatoryState = Get_SF_Reset_State();
            DS1_Reset_Status = Get_SF_DS1_Reset_State();
            DS2_Reset_Status = Get_SF_DS2_Reset_State();
            if((uchPreparatoryState == (BYTE)SF_WAIT_FOR_DS_TO_RESET) ||
                  (DS1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM) ||
                  (DS2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
               {
                Decrement_Reset_300sTmr2();
               }
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_Reset_msTmr();                    /* from reset.c */
            Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            //Decrement_itoa_msTmr();                     /* from display.c */

        }
        if (Status.Flags.System_Status == NORMAL)
        {
            if (SPI_Failure.Failure_Data == 0)
            {
                ErrorCodeForSPI = 255;
            }
            else
            {
                Update_SPI_Error();
            }
            Update_SF_Reset_Monitor_State();
            uchPreparatoryState = Get_SF_Reset_State();
            if (uchPreparatoryState == (BYTE)SF_RESET_SEQUENCE_COMPLETED)
            {
              uchReset_Pending = FALSE;
              Clear_DS_Checksum_Info();
            }
        }
        else
        {
            Update_Display_Error();
            //Update_itoa_SF_State();                     /* from display.c  */
            if(Status.Flags.Flash_CheckSum != CRC32_CHECKSUM_BAD)
              {
               if(Bootup_Lock.Timeout_50ms == TIMEOUT_EVENT)
                {
                 Clear_Reset_State();                            /*Clear the reset state and intialise to waiting for reset*/
                 {asm("reset");} // RESET();                                      /* Reset the System */
                }
              }
        }

        Configure_Modem_B ();
        Modem_State = Get_Modem_B_State();
      if(Modem_State == (BYTE)CONFIGURATION_COMPLETED)
       {
        Update_DS_Sch_State();
        }                                               /* from comm_ds.c */
        Update_SPI_State();                             /* from comm_sm.c */
        //Update_LCD_State();                             /* from lcd_drv.c */
        Check_DAC_Boards_Runtime();                     /* from restore.c */
        ClrWdt();
    }
}
/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Start_CF_Reset_Sequence(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the Center fed reset sequence
                        1.Get the CF reset state
                        2.Start the Down stream communication scheduler and Up stream
                         comminication scheduler
                        3.Update the reset state
                        4.In error condition update the error display and wait for 6s then reboot the system except
                          for the flash checksum failure
                        5.Update the down stream scheduler, Up stream scheduler state,
                         SPI state, LCD state

Allocated Requirements	: (SSDAC_SWRS_0244), (SSDAC_SWRS_0541), (SSDAC_SWRS_0517),(SSDAC_SWRS_0618)
							(SSDAC_SWRS_0717), (SSDAC_SWRS_0641), (SSDAC_SWRS_0741), (SSDAC_SWRS_0339),
						  (SSDAC_SWRS_0846),(SSDAC_SWRS_0847)

Design Requirements		:	SSDAC_DR_5004

Interfaces
    Calls           :   
                        RESET.c     -   Get_CF_DS_Reset_State()
                        RESET.c     -   Get_CF_US_Reset_State()
                        RESET.c     -   Get_CF_DS1_Reset_State()
                        RESET.c     -   Get_CF_DS2_Reset_State()
                        RESET.c     -   Get_CF_US1_Reset_State()
                        RESET.c     -   Get_CF_US2_Reset_State()
                        RESET.c     -   Initialise_CF_Reset_Monitor()
                        RESET.c     -   Start_CF_Reset_Monitor()
                        COMM_US.c   -   Start_US_CommSch()
                        COMM_DS.c   -   Start_DS_CommSch()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        COMM_DS.c   -   Decrement_DS_Sch_msTmr()
                        COMM_US.c   -   Decrement_US_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        RESET.c     -   Update_CF_Reset_Monitor_State()
                        ERROR.c     -   Update_Display_Error()
                        DISPLAY.c   -   Update_itoa_SF_State()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function
                        RESET()     -   Compiler defined function
                        DAC_MAIN.c  -   Decrement_Bootuplock_50msTmr()
                        RESET.c     -   Decrement_Reset_300sTmr()
                        RESET.c     -   Decrement_Reset_300sTmr2()
                        RESET.c     -   Clear_Reset_State()

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                            Type
    Global          :   CF_WAIT_FOR_RESET               Enumerator
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status      Bit
                        CF_RESET_SEQUENCE_COMPLETED     Enumerator
                        RESET_APPLIED_MODE              Enumerator
                        SYSTEM_ERROR_MODE               Enumerator
                        CF_WAIT_FOR_US_TO_RESET         Enumerator
                        CF_WAIT_FOR_DS_TO_RESET         Enumerator
                        Bootup_Lock.Timeout_50ms        UINT16
                        Status.Flags.Flash_CheckSum     Bit

    Local           :   uchPreparatoryState             BYTE
                        uchReset_Pending                BYTE
                        uchString                       Array of BYTEs
                        DS1_Reset_Status                BYTE
                        DS2_Reset_Status                BYTE
                        US1_Reset_Status                BYTE
                        US2_Reset_Status                BYTE

Output Variables        Name                            Type
    Global          :   Status.Flags.Local_Reset_Done   Bit
                        Status.Flags.Local_Reset_Done2  Bit
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        DS_Section_Mode.Local_Unit      BYTE
                        US_Section_Mode.Local_Unit      BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                           Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            SM_HAS_RESETTED_SYSTEM          1
                            TRUE                            1
                            FALSE                           0
                            SET_LOW                         0
                            TIMEOUT_EVENT                   0
                            CRC32_CHECKSUM_BAD              0

References          :

Derived Requirements:

*********************************************************************************/
void Start_CF_Reset_Sequence(void)
{
    BYTE uchPreparatoryState,uchPreparatoryState1;
    BYTE uchReset_Pending;
    BYTE DS1_Reset_Status;
    BYTE DS2_Reset_Status;
    BYTE US1_Reset_Status;
    BYTE US2_Reset_Status;
    BYTE A_Modem_State;
    BYTE B_Modem_State;
    /*
     * Station master has to ensure that there are no trains in the
     * controlled section by pressing the Preparatory reset button
     */


    
    uchPreparatoryState = Get_CF_DS_Reset_State();
    uchPreparatoryState1 = Get_CF_US_Reset_State();
    if((uchPreparatoryState == (BYTE)CF_DS_WAIT_FOR_RESET) &&
        (uchPreparatoryState1 == (BYTE)CF_US_WAIT_FOR_RESET))
    {
        Initialise_CF_Reset_Monitor();
    }
    else
    {
        if(uchPreparatoryState > (BYTE)CF_DS_WAIT_FOR_RESET)
         {
           Status.Flags.Local_Reset_Done2  = SM_HAS_RESETTED_SYSTEM;
           DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
           US_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
         }
        if(uchPreparatoryState1 > (BYTE)CF_US_WAIT_FOR_RESET)
         {
           Status.Flags.Local_Reset_Done = SM_HAS_RESETTED_SYSTEM;
           US_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
           DS_Section_Mode.Local_Unit = SYSTEM_ERROR_MODE;
         }
    }
    Start_CF_Reset_Monitor();
    uchReset_Pending = TRUE;

    Start_US_CommSch();                             /* Start Upstream communication scheduler */
    Start_DS_CommSch();                             /* Start Downstream communication scheduler */

    while (uchReset_Pending)
    {
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16)TIMER3_SETPOINT);
            Start_Sys_Mon_Decrement_50msTmr();              /* from sys_mon.c */
            Decrement_SPI_50msTmr();                        /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_Bootuplock_50msTmr();
            uchPreparatoryState1 = Get_CF_US_Reset_State();
            uchPreparatoryState = Get_CF_DS_Reset_State();
            DS1_Reset_Status = Get_CF_DS1_Reset_State();
            DS2_Reset_Status = Get_CF_DS2_Reset_State();
            US1_Reset_Status = Get_CF_US1_Reset_State();
            US2_Reset_Status = Get_CF_US2_Reset_State();
             if((uchPreparatoryState1 == (BYTE) CF_WAIT_FOR_US_TO_RESET) ||
                (US1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM) ||
                (US2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
                {

                  Decrement_Reset_300sTmr2();
                }
             if((uchPreparatoryState == (BYTE) CF_WAIT_FOR_DS_TO_RESET)||
                (DS1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM) ||
                (DS2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
                 {

                  Decrement_Reset_300sTmr();
                }

        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_Reset_msTmr();                        /* from reset.c */
            Decrement_DS_Sch_msTmr();                       /* from comm_ds.c */
            Decrement_US_Sch_msTmr();                       /* from comm_us.c */
            //Decrement_LCD_msTmr();                          /* from lcd_drv.c */
            //Decrement_itoa_msTmr();                         /* from display.c */
        }
        if (Status.Flags.System_Status == NORMAL)
        {
            if (SPI_Failure.Failure_Data == 0)
            {
                ErrorCodeForSPI = 255;
            }
            else
            {
                Update_SPI_Error();
            }
            Update_CF_Reset_Monitor_State();
            uchPreparatoryState  = Get_CF_DS_Reset_State();
            uchPreparatoryState1 = Get_CF_US_Reset_State();
            if((uchPreparatoryState  == (BYTE)CF_DS_RESET_SEQUENCE_COMPLETED) ||
               (uchPreparatoryState1 == (BYTE)CF_US_RESET_SEQUENCE_COMPLETED))
            {
             uchReset_Pending = FALSE;
            }
        }
        else
        {
            Update_Display_Error();
            //Update_itoa_CF_State();                         /* from display.c  */
            if(Status.Flags.Flash_CheckSum != CRC32_CHECKSUM_BAD)
              {
               if(Bootup_Lock.Timeout_50ms == TIMEOUT_EVENT)
                {
                 Clear_Reset_State();                            /*Clear the reset state and intialise to waiting for reset*/
                 {asm("reset");} // RESET();                                      /* Reset the System */
                }
              }
        }
        Configure_Modem_A ();
        Configure_Modem_B ();
        A_Modem_State = Get_Modem_A_State();
        B_Modem_State = Get_Modem_B_State();
        if(B_Modem_State == (BYTE)CONFIGURATION_COMPLETED)
        {
         Update_DS_Sch_State();
        }
        if(A_Modem_State == (BYTE)CONFIGURATION_COMPLETED)
         {
          Update_US_Sch_State();                                /* from comm_us.c */
         }
        Update_SPI_State();                                     /* from comm_sm.c */
        //Update_LCD_State();                                     /* from lcd_drv.c */
        Check_DAC_Boards_Runtime();                             /* from restore.c */
        ClrWdt();
    }
}




/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Start_CF_Reset_Sequence(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the Center fed reset sequence
                        1.Get the CF reset state
                        2.Start the Down stream communication scheduler and Up stream
                         comminication scheduler
                        3.Update the reset state
                        4.In error condition update the error display and wait for 6s then reboot the system except
                          for the flash checksum failure
                        5.Update the down stream scheduler, Up stream scheduler state,
                         SPI state, LCD state


Design Requirements     :



Interfaces
    Calls           :   
                        RESET.c     -   Get_CF_DS_Reset_State()
                        RESET.c     -   Get_CF_US_Reset_State()
                        RESET.c     -   Get_CF_DS1_Reset_State()
                        RESET.c     -   Get_CF_DS2_Reset_State()
                        RESET.c     -   Get_CF_US1_Reset_State()
                        RESET.c     -   Get_CF_US2_Reset_State()
                        RESET.c     -   Initialise_CF_Reset_Monitor()
                        RESET.c     -   Start_CF_Reset_Monitor()
                        COMM_US.c   -   Start_US_CommSch()
                        COMM_DS.c   -   Start_DS_CommSch()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        COMM_DS.c   -   Decrement_DS_Sch_msTmr()
                        COMM_US.c   -   Decrement_US_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        RESET.c     -   Update_CF_Reset_Monitor_State()
                        ERROR.c     -   Update_Display_Error()
                        DISPLAY.c   -   Update_itoa_SF_State()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function
                        RESET()     -   Compiler defined function
                        DAC_MAIN.c  -   Decrement_Bootuplock_50msTmr()
                        RESET.c     -   Decrement_Reset_300sTmr()
                        RESET.c     -   Decrement_Reset_300sTmr2()
                        RESET.c     -   Clear_Reset_State()

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                            Type
    Global          :   CF_WAIT_FOR_RESET               Enumerator
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status      Bit
                        CF_RESET_SEQUENCE_COMPLETED     Enumerator
                        RESET_APPLIED_MODE              Enumerator
                        SYSTEM_ERROR_MODE               Enumerator
                        CF_WAIT_FOR_US_TO_RESET         Enumerator
                        CF_WAIT_FOR_DS_TO_RESET         Enumerator
                        Bootup_Lock.Timeout_50ms        UINT16
                        Status.Flags.Flash_CheckSum     Bit

    Local           :   uchPreparatoryState             BYTE
                        uchReset_Pending                BYTE
                        uchString                       Array of BYTEs
                        DS1_Reset_Status                BYTE
                        DS2_Reset_Status                BYTE
                        US1_Reset_Status                BYTE
                        US2_Reset_Status                BYTE

Output Variables        Name                            Type
    Global          :   Status.Flags.Local_Reset_Done   Bit
                        Status.Flags.Local_Reset_Done2  Bit
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        DS_Section_Mode.Local_Unit      BYTE
                        US_Section_Mode.Local_Unit      BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                           Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            SM_HAS_RESETTED_SYSTEM          1
                            TRUE                            1
                            FALSE                           0
                            SET_LOW                         0
                            TIMEOUT_EVENT                   0
                            CRC32_CHECKSUM_BAD              0

References          :

Derived Requirements:

*********************************************************************************/
void Start_LCWS_Reset_Sequence(void)
{
    BYTE uchPreparatoryState;
    BYTE uchReset_Pending;
    //BYTE DS1_Reset_Status;
    //BYTE DS2_Reset_Status;
    //BYTE Modem_State;

    /*
     * Station master has to ensure that there are no trains in the
     * controlled section by pressing the Preparatory reset button
     */



//    Reset_Info.SF.DS_State = SF_WAIT_FOR_RESET;
    uchPreparatoryState = Get_SF_Reset_State();
    if (uchPreparatoryState == (BYTE)SF_WAIT_FOR_RESET)
    {
        Initialise_LCWS_Reset_Monitor();
    }
    else
    {
        Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
        DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
    }
    Start_LCWS_Reset_Monitor();
    uchReset_Pending = TRUE;

    // Commented by Shankar for LCWS
    //Start_DS_CommSch();     /* Start Downstream communication scheduler */

    while (uchReset_Pending)
    {
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16)TIMER3_SETPOINT);
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_Bootuplock_50msTmr();

            // Below is commented by Shankar, as there is no need of the co-operative reset in LCWS mode

//            uchPreparatoryState = Get_SF_Reset_State();
//            DS1_Reset_Status = Get_SF_DS1_Reset_State();
//            DS2_Reset_Status = Get_SF_DS2_Reset_State();
//            if((uchPreparatoryState == (BYTE)SF_WAIT_FOR_DS_TO_RESET) ||
//                  (DS1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM) ||
//                  (DS2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
//               {
//                Decrement_Reset_300sTmr2();
//               }
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_Reset_msTmr();                    /* from reset.c */
            US_LED_Error_Service_1mS();
            DS_LED_Error_Service_1mS();
            //Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            //Decrement_itoa_msTmr();                     /* from display.c */

        }
        if (Status.Flags.System_Status == NORMAL)
        {
            if (SPI_Failure.Failure_Data == 0)
            {
                ErrorCodeForSPI = 255;
            }
            else
            {
                Update_SPI_Error();
            }
            Update_LCWS_Reset_Monitor_State();
            uchPreparatoryState = Get_SF_Reset_State();
            if (uchPreparatoryState == (BYTE)SF_RESET_SEQUENCE_COMPLETED)
            {
              uchReset_Pending = FALSE;
              Clear_DS_Checksum_Info();
            }
        }
        else
        {
            Update_Display_Error();
            //Update_itoa_SF_State();                     /* from display.c  */
            if(Status.Flags.Flash_CheckSum != CRC32_CHECKSUM_BAD)
              {
               if(Bootup_Lock.Timeout_50ms == TIMEOUT_EVENT)
                {
                 Clear_Reset_State();                            /*Clear the reset state and intialise to waiting for reset*/
                 {asm("reset");} // RESET();                                      /* Reset the System */
                }
              }
        }

        //Configure_Modem_B ();
        //Modem_State = Get_Modem_B_State();
      //if(Modem_State == CONFIGURATION_COMPLETED)
      // {
      //  Update_DS_Sch_State();
      //  }                                               /* from comm_ds.c */
        Update_SPI_State();                             /* from comm_sm.c */
        //Update_LCD_State();                             /* from lcd_drv.c */
        Check_DAC_Boards_Runtime();                     /* from restore.c */
        ClrWdt();
    }
}

/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Start_D3_Reset_Sequence(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the End fed reset sequence
                        1.Get the EF reset state
                        2.Start the Up stream communication scheduler
                        3.Update the reset state
                        4.In error condition update the error display and wait for 6s then reboot the system except
                          for the flash checksum failure
                        5.Update the Up stream scheduler state, SPI state, LCD state

Allocated Requirements	: 	(SSDAC_SWRS_0417), (SSDAC_SWRS_0418), (SSDAC_SWRS_0244),(SSDAC_SWRS_0438)
						    (SSDAC_SWRS_0339)

Design Requirements		:	SSDAC_DR_5005

Interfaces
    Calls           :   
                        RESET.c     -   Get_CF_DS_Reset_State()
                        RESET.c     -   Get_CF_US_Reset_State()
                        RESET.c     -   Initialise_CF_Reset_Monitor()
                        RESET.c     -   Start_CF_Reset_Monitor()
                        COMM_US.c   -   Start_US_CommSch()
                        COMM_DS.c   -   Start_DS_CommSch()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        COMM_DS.c   -   Decrement_DS_Sch_msTmr()
                        COMM_US.c   -   Decrement_US_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        RESET.c     -   Update_CF_Reset_Monitor_State()
                        ERROR.c     -   Update_Display_Error()
                        DISPLAY.c   -   Update_itoa_SF_State()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function
                        RESET()     -   Compiler defined function
                        DAC_MAIN.c      -   Decrement_Bootuplock_50msTmr()
                        RESET.c     -   Clear_Reset_State()

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                            Type
    Global          :   CF_WAIT_FOR_RESET               Enumerator
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status      Bit
                        CF_RESET_SEQUENCE_COMPLETED     Enumerator
                        RESET_APPLIED_MODE              Enumerator
                        DP3_MODE                        Enumerator
                        Status.Flags.Flash_CheckSum     Bit
                        Bootup_Lock.Timeout_50ms        UINT16

    Local           :   uchPreparatoryState             BYTE
                        uchReset_Pending                BYTE
                        uchString                       Array of BYTEs

Output Variables        Name                            Type
    Global          :   Status.Flags.Local_Reset_Done   Bit
                        Status.Flags.Local_Reset_Done2  Bit
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        DS_Section_Mode.Local_Unit      BYTE
                        DS_Section_Mode.Remote_Unit     BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                           Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            SM_HAS_RESETTED_SYSTEM          1
                            TRUE                            1
                            FALSE                           0
                            SET_LOW                         0
                            CRC32_CHECKSUM_BAD              0
                            TIMEOUT_EVENT                   0

References          :

Derived Requirements:

*********************************************************************************/
void Start_D3_Reset_Sequence(void)
{
    BYTE uchPreparatoryState,uchPreparatoryState1;
    BYTE uchReset_Pending;
    BYTE A_Modem_State;
    BYTE B_Modem_State;
    /*
     * Station master has to ensure that there are no trains in the
     * controlled section by pressing the Preparatory reset button
     */


    
    uchPreparatoryState  = Get_CF_DS_Reset_State();
    uchPreparatoryState1 = Get_CF_US_Reset_State();
    if((uchPreparatoryState == (BYTE)CF_DS_WAIT_FOR_RESET) &&
       (uchPreparatoryState1== (BYTE)CF_US_WAIT_FOR_RESET))
    {
       Initialise_CF_Reset_Monitor();
    }
    else
    {
       Status.Flags.Local_Reset_Done  = SM_HAS_RESETTED_SYSTEM;
       Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
       DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
       DS_Section_Mode.Remote_Unit = DP3_MODE;
    }

    Start_CF_Reset_Monitor();
    uchReset_Pending = TRUE;

    Start_US_CommSch();                             /* Start Upstream communication scheduler   */
    Start_DS_CommSch();                             /* Start Downstream communication scheduler */

    while (uchReset_Pending)
    {
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16)TIMER3_SETPOINT);
            Start_Sys_Mon_Decrement_50msTmr();              /* from sys_mon.c */
            Decrement_SPI_50msTmr();                        /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_Bootuplock_50msTmr();
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_Reset_msTmr();                        /* from reset.c */
            Decrement_DS_Sch_msTmr();                       /* from comm_ds.c */
            Decrement_US_Sch_msTmr();                       /* from comm_us.c */
            //Decrement_LCD_msTmr();                          /* from lcd_drv.c */
            //Decrement_itoa_msTmr();                         /* from display.c */
        }
        if (Status.Flags.System_Status == NORMAL)
        {
            if (SPI_Failure.Failure_Data == 0)
            {
                ErrorCodeForSPI = 255;
            }
            else
            {
                Update_SPI_Error();
            }
            Update_CF_Reset_Monitor_State();
            uchPreparatoryState  = Get_CF_DS_Reset_State();
            uchPreparatoryState1 = Get_CF_US_Reset_State();
            if((uchPreparatoryState  == (BYTE)CF_DS_RESET_SEQUENCE_COMPLETED) &&
               (uchPreparatoryState1 == (BYTE)CF_US_RESET_SEQUENCE_COMPLETED ))
            {

             uchReset_Pending = FALSE;
            }
        }
        else
        {
            Update_Display_Error();
            //Update_itoa_CF_State();                         /* from display.c  */
            if(Status.Flags.Flash_CheckSum != CRC32_CHECKSUM_BAD)
              {
               if(Bootup_Lock.Timeout_50ms == TIMEOUT_EVENT)
                {
                 Clear_Reset_State();                            /*Clear the reset state and intialise to waiting for reset*/
                 {asm("reset");} // RESET();                                      /* Reset the System */
                }
              }
        }
        Configure_Modem_A ();
        Configure_Modem_B ();
        A_Modem_State = Get_Modem_A_State();
        B_Modem_State = Get_Modem_B_State();
        if(B_Modem_State == (BYTE)CONFIGURATION_COMPLETED)
        {
         Update_DS_Sch_State();
        }
        if(A_Modem_State == (BYTE)CONFIGURATION_COMPLETED)
         {
          Update_US_Sch_State();                                /* from comm_us.c */
         }
        Update_SPI_State();                                 /* from comm_sm.c */
        //Update_LCD_State();                                 /* from lcd_drv.c */
        Check_DAC_Boards_Runtime();                             /* from restore.c */
        ClrWdt();
    }
}
/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Start_D4_Reset_Sequence(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the End fed reset sequence
                        1.Get the EF reset state
                        2.Start the Up stream communication scheduler
                        3.Update the reset state
                        4.In error condition update the error display and wait for 6s then reboot the system except
                          for the flash checksum failure
                        5.Update the Up stream scehdular state, SPI state, LCD state


Design Requirements     :



Interfaces
    Calls           :   
                        RESET.c     -   Get_CF_DS_Reset_State()
                        RESET.c     -   Get_CF_US_Reset_State()
                        RESET.c     -   Initialise_CF_Reset_Monitor()
                        RESET.c     -   Start_CF_Reset_Monitor()
                        COMM_US.c   -   Start_US_CommSch()
                        COMM_DS.c   -   Start_DS_CommSch()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        COMM_DS.c   -   Decrement_DS_Sch_msTmr()
                        COMM_US.c   -   Decrement_US_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        RESET.c     -   Update_CF_Reset_Monitor_State()
                        ERROR.c     -   Update_Display_Error()
                        DISPLAY.c   -   Update_itoa_SF_State()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function
                        RESET()     -   Compiler defined function
                        DAC_MAIN.c      -   Decrement_Bootuplock_50msTmr()
                        RESET.c     -   Clear_Reset_State()

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                            Type
    Global          :   CF_WAIT_FOR_RESET               Enumerator
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status      Bit
                        CF_RESET_SEQUENCE_COMPLETED     Enumerator
                        RESET_APPLIED_MODE              Enumerator
                        DP3_MODE                        Enumerator
                        Status.Flags.Flash_CheckSum     Bit
                        Bootup_Lock.Timeout_50ms        UINT16

    Local           :   uchPreparatoryState             BYTE
                        uchReset_Pending                BYTE
                        uchString                       Array of BYTEs

Output Variables        Name                            Type
    Global          :   Status.Flags.Local_Reset_Done   Bit
                        Status.Flags.Local_Reset_Done2  Bit
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        DS_Section_Mode.Local_Unit      BYTE
                        DS_Section_Mode.Remote_Unit     BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                           Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            SM_HAS_RESETTED_SYSTEM          1
                            TRUE                            1
                            FALSE                           0
                            SET_LOW                         0
                            CRC32_CHECKSUM_BAD              0
                            TIMEOUT_EVENT                   0

References          :

Derived Requirements:

*********************************************************************************/
void Start_D4_Reset_Sequence(void)
{
    BYTE uchPreparatoryState,uchPreparatoryState1;
    BYTE uchReset_Pending;
    BYTE A_Modem_State;
    BYTE B_Modem_State;
    /*
     * Station master has to ensure that there are no trains in the
     * controlled section by pressing the Preparatory reset button
     */

    uchPreparatoryState  = Get_CF_DS_Reset_State();
    uchPreparatoryState1 = Get_CF_US_Reset_State();
    if((uchPreparatoryState == (BYTE)CF_DS_WAIT_FOR_RESET) &&
       (uchPreparatoryState1== (BYTE)CF_US_WAIT_FOR_RESET))
    {
       Initialise_CF_Reset_Monitor();
    }
    else
    {
       Status.Flags.Local_Reset_Done  = SM_HAS_RESETTED_SYSTEM;
       Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
       DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
       DS_Section_Mode.Remote_Unit = DP3_MODE;
    }

    Start_CF_Reset_Monitor();
    uchReset_Pending = TRUE;

    Start_US_CommSch();                             /* Start Upstream communication scheduler   */
    Start_DS_CommSch();                             /* Start Downstream communication scheduler */

    while (uchReset_Pending)
    {
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16)TIMER3_SETPOINT);
            Start_Sys_Mon_Decrement_50msTmr();              /* from sys_mon.c */
            Decrement_SPI_50msTmr();                        /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_Bootuplock_50msTmr();
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_Reset_msTmr();                        /* from reset.c */
            Decrement_DS_Sch_msTmr();                       /* from comm_ds.c */
            Decrement_US_Sch_msTmr();                       /* from comm_us.c */
            //Decrement_LCD_msTmr();                          /* from lcd_drv.c */
            //Decrement_itoa_msTmr();                         /* from display.c */
        }
        if (Status.Flags.System_Status == NORMAL)
        {
            if (SPI_Failure.Failure_Data == 0)
            {
                ErrorCodeForSPI = 255;
            }
            else
            {
                Update_SPI_Error();
            }
            Update_CF_Reset_Monitor_State();
            uchPreparatoryState  = Get_CF_DS_Reset_State();
            uchPreparatoryState1 = Get_CF_US_Reset_State();
            if((uchPreparatoryState  == (BYTE)CF_DS_RESET_SEQUENCE_COMPLETED) &&
               (uchPreparatoryState1 == (BYTE)CF_US_RESET_SEQUENCE_COMPLETED ))
            {

             uchReset_Pending = FALSE;
            }
        }
        else
        {
            Update_Display_Error();
            //Update_itoa_CF_State();                         /* from display.c  */
            if(Status.Flags.Flash_CheckSum != CRC32_CHECKSUM_BAD)
              {
               if(Bootup_Lock.Timeout_50ms == TIMEOUT_EVENT)
                {
                 Clear_Reset_State();                            /*Clear the reset state and intialise to waiting for reset*/
                 {asm("reset");} // RESET();                                      /* Reset the System */
                }
              }
        }
        Configure_Modem_A ();
        Configure_Modem_B ();
        A_Modem_State = Get_Modem_A_State();
        B_Modem_State = Get_Modem_B_State();
        if(B_Modem_State == (BYTE)CONFIGURATION_COMPLETED)
        {
         Update_DS_Sch_State();
        }
        if(A_Modem_State == (BYTE)CONFIGURATION_COMPLETED)
         {
          Update_US_Sch_State();                                /* from comm_us.c */
         }
        Update_SPI_State();                                 /* from comm_sm.c */
        //Update_LCD_State();                                 /* from lcd_drv.c */
        Check_DAC_Boards_Runtime();                             /* from restore.c */
        ClrWdt();
    }
}

/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Start_EF_Reset_Sequence(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the Dead End reset sequence
                        1.Get the DE reset state
                        2.Update the reset state
                        4.In error condition update the error display and wait for 6s then reboot the system except
                          for the flash checksum failure
                        5.Update SPI state and LCD state

Allocated Requirements	: 	(SSDAC_SWRS_0119), (SSDAC_SWRS_0120), (SSDAC_ SWRS_ 0149), (SSDAC_SWRS_0244)
							(SSDAC_SWRS_0339),(SSDAC_SWRS_0850),(SSDAC_SWRS_0851)
Design Requirements		:	SSDAC_DR_5006
						

Interfaces
    Calls           :   
                        RESET.c     -   Get_EF_Reset_State()
                        RESET.c     -   Get_EF_US1_Reset_State()
                        RESET.c     -   Get_EF_US2_Reset_State()
                        RESET.c     -   Initialise_EF_Reset_Monitor()
                        RESET.c     -   Start_EF_Reset_Monitor()
                        COMM_US.c   -   Start_US_CommSch()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        RESET.c     -   Decrement_Reset_300sTmr()
                        RESET.c     -   Decrement_Reset_300sTmr2()
                        COMM_US.c   -   Decrement_US_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        RESET.c     -   Update_EF_Reset_Monitor_State()
                        ERROR.c     -   Update_Display_Error()
                        DISPLAY.c   -   Update_itoa_SF_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function
                        RESET()     -   Compiler defined function
                        DAC_MAIN.c  -   Decrement_Bootuplock_50msTmr()
                        RESTORE.c   -   Clear_US_Checksum_Info()
                        RESET.c     -   Clear_Reset_State()

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                            Type
    Global          :   EF_WAIT_FOR_RESET               Enumerator
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status      Bit
                        EF_RESET_SEQUENCE_COMPLETED     Enumerator
                        RESET_APPLIED_MODE              Enumerator
                        EF_WAIT_FOR_US_TO_RESET         Enumerator
                        Status.Flags.Flash_CheckSum     Bit
                        Bootup_Lock.Timeout_50ms        UINT16

    Local           :   uchPreparatoryState             BYTE
                        uchReset_Pending                BYTE
                        uchString                       Array of BYTEs
                        US1_Reset_Status;               BYTE
                        US2_Reset_Status;               BYTE

Output Variables        Name                            Type
    Global          :   Status.Flags.Local_Reset_Done   Bit
                        Status.Flags.Local_Reset_Done2  Bit
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        US_Section_Mode.Local_Unit      BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                           Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            SM_HAS_RESETTED_SYSTEM          1
                            TRUE                            1
                            FALSE                           0
                            SET_LOW                         0
                            CRC32_CHECKSUM_BAD              0
                            TIMEOUT_EVENT                   0

References          :

Derived Requirements:
*********************************************************************************/
void Start_EF_Reset_Sequence(void)
{
    BYTE uchPreparatoryState;
    BYTE uchReset_Pending;
    BYTE US1_Reset_Status;
    BYTE US2_Reset_Status;
    BYTE Modem_State;

    /*
     * Station master has to ensure that there are no trains in the
     * controlled section by pressing the Preparatory reset button
     */


    
//    Reset_Info.EF.US_State = EF_WAIT_FOR_RESET;

    uchPreparatoryState = Get_EF_Reset_State();
    if (uchPreparatoryState == (BYTE)EF_WAIT_FOR_RESET)
    {
        Initialise_EF_Reset_Monitor();
    }
    else
    {
        Status.Flags.Local_Reset_Done = SM_HAS_RESETTED_SYSTEM;
        US_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
        US_Section_Mode.Remote_Unit = WAITING_FOR_RESET_MODE;
    }
    Start_EF_Reset_Monitor();
    uchReset_Pending = TRUE;

    Start_US_CommSch();                                 /* Start Upstream communication scheduler */

    while (uchReset_Pending)
    {
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16)TIMER3_SETPOINT);
            Start_Sys_Mon_Decrement_50msTmr();              /* from sys_mon.c */
            Decrement_SPI_50msTmr();                        /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_Bootuplock_50msTmr();
             uchPreparatoryState = Get_EF_Reset_State();
             US1_Reset_Status = Get_EF_US1_Reset_State();
             US2_Reset_Status = Get_EF_US2_Reset_State();
            if((uchPreparatoryState == (BYTE) EF_WAIT_FOR_US_TO_RESET)||
               (US1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM)||
               (US2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
               {
                Decrement_Reset_300sTmr();
               }
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_Reset_msTmr();                        /* from reset.c */
            Decrement_US_Sch_msTmr();                       /* from comm_us.c */
            //Decrement_LCD_msTmr();                          /* from lcd_drv.c */
            //Decrement_itoa_msTmr();                         /* from display.c */
        }
        if (Status.Flags.System_Status == NORMAL)
        {
            if (SPI_Failure.Failure_Data == 0)
            {
                ErrorCodeForSPI = 255;
            }
            else
            {
                Update_SPI_Error();
            }
            Update_EF_Reset_Monitor_State();
            uchPreparatoryState = Get_EF_Reset_State();
            if(uchPreparatoryState == (BYTE)EF_RESET_SEQUENCE_COMPLETED)
            {
              uchReset_Pending = FALSE;
              Clear_US_Checksum_Info();
            }
        }
        else
        {
            Update_Display_Error();
            //Update_itoa_EF_State();                         /* from display.c  */
            if(Status.Flags.Flash_CheckSum != CRC32_CHECKSUM_BAD)
              {
               if(Bootup_Lock.Timeout_50ms == TIMEOUT_EVENT)
                {
                 Clear_Reset_State();                            /*Clear the reset state and intialise to waiting for reset*/
                 {asm("reset");} // RESET();                                      /* Reset the System */
                }
              }
        }

        Configure_Modem_A ();
        Modem_State = Get_Modem_A_State();
        if(Modem_State == (BYTE)CONFIGURATION_COMPLETED)
         {
          Update_US_Sch_State();                                /* from comm_us.c */
         }
        Update_SPI_State();                                 /* from comm_sm.c */
        //Update_LCD_State();                                 /* from lcd_drv.c */
        Check_DAC_Boards_Runtime();                         /* from restore.c */
        ClrWdt();
    }
}
/*********************************************************************************
Component name      :DAC_MAIN.c
Module Name         :void Start_DE_Reset_Sequence(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Start the Dead End reset sequence
                        1.Get the DE reset state
                        2.Update the reset state
                        4.In error condition update the error display
                        5.Update SPI state and LCD state

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   
                        RESET.c     -   Get_DE_Reset_State()
                        RESET.c     -   Initialise_DE_Reset_Monitor()
                        RESET.c     -   Start_DE_Reset_Monitor()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        RESET.c     -   Update_DE_Reset_Monitor_State()
                        ERROR.c     -   Update_Display_Error()
                        DISPLAY.c   -   Update_itoa_DE_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                            Type
    Global          :   DE_WAIT_FOR_RESET               Enumerator
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status      Bit
                        DE_RESET_SEQUENCE_COMPLETED     Enumerator

    Local           :   uchPreparatoryState             BYTE
                        uchReset_Pending                BYTE
                        uchString                       Array of BYTEs

Output Variables        Name                            Type
    Global          :   Status.Flags.Local_Reset_Done   Bit
                        Status.Flags.Local_Reset_Done2  Bit
                        TMR3IF                          Timer3 overflow interrupt flag bit
                        TMR4IF                          Timer4 overflow interrupt flag bit

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                           Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            SM_HAS_RESETTED_SYSTEM          1
                            TRUE                            1
                            FALSE                           0
                            SET_LOW                         0

References          :

Derived Requirements:
*********************************************************************************/
void Start_DE_Reset_Sequence(void)
{
    BYTE uchPreparatoryState;
    BYTE uchReset_Pending;
    //BYTE DS1_Reset_Status;
    //BYTE DS2_Reset_Status;
    //BYTE Modem_State;

    /*
     * Station master has to ensure that there are no trains in the
     * controlled section by pressing the Preparatory reset button
     */


    
//    Reset_Info.SF.DS_State = SF_WAIT_FOR_RESET;
    uchPreparatoryState = Get_SF_Reset_State();
    if (uchPreparatoryState == (BYTE)SF_WAIT_FOR_RESET)
    {
        Initialise_DE_Reset_Monitor();
    }
    else
    {
        Status.Flags.Local_Reset_Done2 = SM_HAS_RESETTED_SYSTEM;
        DS_Section_Mode.Local_Unit = RESET_APPLIED_MODE;
    }
    Start_DE_Reset_Monitor();
    uchReset_Pending = TRUE;

    // Commented by Shankar for LCWS
    //Start_DS_CommSch();     /* Start Downstream communication scheduler */

    while (uchReset_Pending)
    {
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16)TIMER3_SETPOINT);
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_Bootuplock_50msTmr();

            // Below is commented by Shankar, as there is no need of the co-operative reset in LCWS mode

//            uchPreparatoryState = Get_SF_Reset_State();
//            DS1_Reset_Status = Get_SF_DS1_Reset_State();
//            DS2_Reset_Status = Get_SF_DS2_Reset_State();
//            if((uchPreparatoryState == (BYTE)SF_WAIT_FOR_DS_TO_RESET) ||
//                  (DS1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM) ||
//                  (DS2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
//               {
//                Decrement_Reset_300sTmr2();
//               }
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_Reset_msTmr();                    /* from reset.c */
            DS_LED_Error_Service_1mS();
            //Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            //Decrement_itoa_msTmr();                     /* from display.c */

        }
        if (Status.Flags.System_Status == NORMAL)
        {

            if (SPI_Failure.Failure_Data == 0)
            {
                ErrorCodeForSPI = 255;
            }
            else
            {
                Update_SPI_Error();
            }
            Update_DE_Reset_Monitor_State();
            uchPreparatoryState = Get_SF_Reset_State();
            if (uchPreparatoryState == (BYTE)SF_RESET_SEQUENCE_COMPLETED)
            {
              uchReset_Pending = FALSE;
              Clear_DS_Checksum_Info();
            }
        }
        else
        {

            Update_Display_Error();
            //Update_itoa_SF_State();                     /* from display.c  */
            if(Status.Flags.Flash_CheckSum != CRC32_CHECKSUM_BAD)
              {
               if(Bootup_Lock.Timeout_50ms == TIMEOUT_EVENT)
                {
                 Clear_Reset_State();                            /*Clear the reset state and intialise to waiting for reset*/
                 {asm("reset");} // RESET();                                      /* Reset the System */
                }
              }
        }

        //Configure_Modem_B ();
        //Modem_State = Get_Modem_B_State();
      //if(Modem_State == CONFIGURATION_COMPLETED)
      // {
      //  Update_DS_Sch_State();
      //  }                                               /* from comm_ds.c */
        Update_SPI_State();                             /* from comm_sm.c */
        //Update_LCD_State();                             /* from lcd_drv.c */
        Check_DAC_Boards_Runtime();                     /* from restore.c */
        ClrWdt();
    }
}

/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Control_DAC_Type_SF(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :1.Start the down stream axle counting
                     2.Start the up stream axle counting
                     3.If system is normal,validate the PD signals, check for
                       completion of the axle counting,update both vital relay A & B counts &
                       update the track status
                     4.If the system is in error update the display to indicate the error
                     5.Update down stream scheduler,up stream scheduler, CF reset state
                     6.Update SPI state & LCD state
                     7.Update itoa state for CF unit type for display

Allocated Requirements	: 	(SSDAC_SWRS_ 0035), (SSDAC_SWRS_0246)
							(SSDAC_SWRS_0339)

Design Requirements		:	SSDAC_DR_5008
						
Interfaces
    Calls           :   
                        AXLE_MON.c  -   Start_DS_Axle_Counting()
                        AXLE_MON.c  -   Decrement_TrackMon_50msTmr()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        COMM_DS.c   -   Decrement_DS_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        AXLE_MON.C  -   Validate_PD_Signals()
                        AXLE_MON.c  -   Chk_for_AxleCount_Completion()
                        RELAYMGR.c  -   Update_Relay_B_Counts()
                        RLYA_MGR.c  -   Update_Relay_A_State()
                        RELAYMGR.c  -   Update_SF_Track_Status()
                        ERROR.c     -   Update_Display_Error()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        SYS_MON.c   -   Update_Sys_Mon_State()
                        RESET.c     -   Update_SF_Reset_Monitor_State()
                        DISPLAY.c   -   Update_itoa_SF_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                                    Type
    Global          :   DIP_Switch_Info.Flags.ATC_Enabled       Bit
                        TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status              Bit
                        WAIT_FOR_PILOT_TRAIN_MODE               Enumerator

    Local           :   uchString                               Array of BYTEs

Output Variables        Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit
                        DS_Section_Mode.Local_Unit              BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                       Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            TRUE                            1
                            NORMAL                          1
                            SET_LOW                         0
References          :

Derived Requirements:
*********************************************************************************/
void Control_DAC_Type_SF(void)
{

    // Clear_Line_on_LCD((BYTE) 2);
    if( DIP_Switch_Info.Flags.ATC_Enabled != TRUE)
    {
        //strcpy(uchString,"Wait For Pilot Train");
        
        DS_Section_Mode.Local_Unit = WAIT_FOR_PILOT_TRAIN_MODE;
    }
    Start_DS_Axle_Counting();
    do {
        ClrWdt();
        CS11 = !CS11;
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16) TIMER3_SETPOINT);
            Decrement_TrackMon_50msTmr();               /* from axle_mon.c */
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_comm_check_50msTmr();
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */

            IFS1bits.T5IF = SET_LOW;
            Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            Decrement_Reset_msTmr();                    /* from reset.c */
            //Decrement_itoa_msTmr();                     /* from display.c */
        }

        if (Status.Flags.System_Status == NORMAL)
        {
            Validate_PD_Signals();                      /* from axle_mon.c */
            Chk_for_AxleCount_Completion();             /* from axle_mon.c */
            Update_Relay_B_Counts();                    /* from relaymgr.c */
            Update_SF_Track_Status();                   /* from relaymgr.c  */
            if(Status.Flags.System_Status != CATASTROPHIC_ERROR)
                ErrorCodeForSPI = 0xFF; 
        }
        else
        {
            /* Catastrophic Error */
            Update_Display_Error();
        }
        Update_DS_Sch_State();                          /* from comm_ds.c */
        Update_SPI_State();                             /* from comm_sm.c */

        Update_Sys_Mon_State();                         /* from sys_mon.c */
        Update_SF_Reset_Monitor_State();                /* from reset.c */

    } while(Always_true == 1);
}
/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Control_DAC_Type_CF(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :1.Start the up stream axle counting
                     2.If system is normal,validate the PD signals, check for
                       completion of the axle counting,update relay A counts &
                       update the up stream track status
                     3.If the system is normal & configured for IBS configuration then update the Vital relay B
                       state, as it will be a repeat relay for next block
                     4.If the system is in error update the display to indicate the error
                     5.Update up stream scheduler, EF reset state
                     6.Update SPI state & LCD state
                     7.Update itoa state for EF unit type for display

Allocated Requirements	: (SSDAC_SWRS_0035), (SSDAC_SWRS_0418), (SSDAC_SWRS_0246), (SSDAC_SWRS_0438)	
						  (SSDAC_SWRS_0339)
Design Requirements		:	SSDAC_DR_5009

Interfaces
    Calls           :   
                        AXLE_MON.c  -   Start_US_Axle_Counting()
                        AXLE_MON.c  -   Start_DS_Axle_Counting()
                        AXLE_MON.c  -   Decrement_TrackMon_50msTmr()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        COMM_DS.c   -   Decrement_DS_Sch_msTmr()
                        COMM_US.c   -   Decrement_US_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        AXLE_MON.C  -   Validate_PD_Signals()
                        AXLE_MON.c  -   Chk_for_AxleCount_Completion()
                        RELAYMGR.c  -   Update_Relay_A_Counts()
                        RELAYMGR.c  -   Update_Relay_B_Counts()
                        RLYA_MGR.c  -   Update_Relay_A_State()
                        RELAYMGR.c  -   Update_CF_Track_Status()
                        ERROR.c     -   Update_Display_Error()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        SYS_MON.c   -   Update_Sys_Mon_State()
                        RESET.c     -   Update_CF_Reset_Monitor_State()
                        DISPLAY.c   -   Update_itoa_CF_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function
                        RESET.c     -   Get_CF_US_Reset_State()
                        RESET.c     -   Get_CF_US1_Reset_State()
                        RESET.c     -   Get_CF_US2_Reset_State()
                        RESET.c     -   Decrement_Reset_300sTmr2()
                        RESET.c     -   Decrement_Reset_300sTmr()
                        RESET.c     -   Get_CF_DS_Reset_State()
                        RESET.c     -   Get_CF_DS1_Reset_State()
                        RESET.c     -   Get_CF_DS2_Reset_State()

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status              Bit
                        Status.Flags.US_System_Status           Bit
                        Status.Flags.DS_System_Status           Bit
                        CF_US_RESET_SEQUENCE_COMPLETED          Enumerator
                        CF_DS_RESET_SEQUENCE_COMPLETED          Enumerator
                        CF_WAIT_FOR_US_TO_RESET                 Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                 Enumerator

    Local           :   uchString                               Array of BYTEs
                        DS1_Reset_Status                        BYTE
                        DS2_Reset_Status                        BYTE
                        US1_Reset_Status                        BYTE
                        US2_Reset_Status                        BYTE
                        uchPreparatoryState                     BYTE
                        uchPreparatoryState1                        BYTE

Output Variables        Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                       Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            TRUE                            1
                            NORMAL                          1
                            SET_LOW                         0
                            CATASTROPHIC_ERROR              0
                            SM_HAS_RESETTED_SYSTEM          1


References          :

Derived Requirements:
*********************************************************************************/

void Control_DAC_Type_CF(void)
{
    BYTE uchPreparatoryState,uchPreparatoryState1;
    BYTE DS1_Reset_Status,DS2_Reset_Status;
    BYTE US1_Reset_Status,US2_Reset_Status;

    do {
        ClrWdt();
        CS11 = !CS11;
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16) TIMER3_SETPOINT);
            Decrement_TrackMon_50msTmr();               /* from axle_mon.c */
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            uchPreparatoryState1 = Get_CF_US_Reset_State();
            if(uchPreparatoryState1 != (BYTE)CF_US_RESET_SEQUENCE_COMPLETED)
               {
                 US1_Reset_Status = Get_CF_US1_Reset_State();
                 US2_Reset_Status = Get_CF_US2_Reset_State();
                  if((uchPreparatoryState1 == (BYTE) CF_WAIT_FOR_US_TO_RESET)||
                     (US1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM)||
                      (US2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
                     {

                       Decrement_Reset_300sTmr2();
                     }
                }

              uchPreparatoryState = Get_CF_DS_Reset_State();
             if(uchPreparatoryState != (BYTE)CF_DS_RESET_SEQUENCE_COMPLETED)
                {
                  DS1_Reset_Status = Get_CF_DS1_Reset_State();
                  DS2_Reset_Status = Get_CF_DS2_Reset_State();
                 if((uchPreparatoryState == (BYTE) CF_WAIT_FOR_DS_TO_RESET)||
                     (DS1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM)||
                      (DS2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
                       {

                          Decrement_Reset_300sTmr();
                       }
                 }

        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            Decrement_US_Sch_msTmr();                   /* from comm_us.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            Decrement_Reset_msTmr();                    /* from reset.c */
            //Decrement_itoa_msTmr();                     /* from display.c */
        }

        if (Status.Flags.System_Status == NORMAL)
        {
            Validate_PD_Signals();                      /* from axle_mon.c */
            Chk_for_AxleCount_Completion();             /* from axle_mon.c */
            Update_Relay_A_Counts();                    /* from relaymgr.c */
            Update_Relay_B_Counts();                    /* from relaymgr.c */
            Update_CF_Track_Status();                   /* from relaymgr.c */
            if(Status.Flags.System_Status != CATASTROPHIC_ERROR)
                ErrorCodeForSPI = 0xFF; 
        }
        if((Status.Flags.US_System_Status == CATASTROPHIC_ERROR)||
           (Status.Flags.DS_System_Status == CATASTROPHIC_ERROR))
        {
            /* Catastrophic Error */
            Update_Display_Error();                     /* from error.c    */
        }
        Update_DS_Sch_State();                          /* from comm_ds.c  */
        Update_US_Sch_State();                          /* from comm_us.c  */
        Update_SPI_State();                             /* from comm_sm.c  */
        //Update_LCD_State();                             /* from lcd_drv.c  */
        Update_Sys_Mon_State();                         /* from sys_mon.c  */
        Update_CF_Reset_Monitor_State();                /* from reset.c    */
        //Update_itoa_CF_State();                         /* from display.c  */
    } while (Always_true == 1);
}




/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Control_DAC_Type_CF(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :1.Start the up stream axle counting
                     2.If system is normal,validate the PD signals, check for
                       completion of the axle counting,update relay A counts &
                       update the up stream track status
                     3.If the system is normal & configured for IBS configuration then update the Vital relay B
                       state, as it will be a repeat relay for next block
                     4.If the system is in error update the display to indicate the error
                     5.Update up stream scheduler, EF reset state
                     6.Update SPI state & LCD state
                     7.Update itoa state for EF unit type for display

Design Requirements     :

Interfaces
    Calls           :   
                        AXLE_MON.c  -   Start_US_Axle_Counting()
                        AXLE_MON.c  -   Start_DS_Axle_Counting()
                        AXLE_MON.c  -   Decrement_TrackMon_50msTmr()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        COMM_DS.c   -   Decrement_DS_Sch_msTmr()
                        COMM_US.c   -   Decrement_US_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        AXLE_MON.C  -   Validate_PD_Signals()
                        AXLE_MON.c  -   Chk_for_AxleCount_Completion()
                        RELAYMGR.c  -   Update_Relay_A_Counts()
                        RELAYMGR.c  -   Update_Relay_B_Counts()
                        RLYA_MGR.c  -   Update_Relay_A_State()
                        RELAYMGR.c  -   Update_CF_Track_Status()
                        ERROR.c     -   Update_Display_Error()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        SYS_MON.c   -   Update_Sys_Mon_State()
                        RESET.c     -   Update_CF_Reset_Monitor_State()
                        DISPLAY.c   -   Update_itoa_CF_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function
                        RESET.c     -   Get_CF_US_Reset_State()
                        RESET.c     -   Get_CF_US1_Reset_State()
                        RESET.c     -   Get_CF_US2_Reset_State()
                        RESET.c     -   Decrement_Reset_300sTmr2()
                        RESET.c     -   Decrement_Reset_300sTmr()
                        RESET.c     -   Get_CF_DS_Reset_State()
                        RESET.c     -   Get_CF_DS1_Reset_State()
                        RESET.c     -   Get_CF_DS2_Reset_State()

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status              Bit
                        Status.Flags.US_System_Status           Bit
                        Status.Flags.DS_System_Status           Bit
                        CF_US_RESET_SEQUENCE_COMPLETED          Enumerator
                        CF_DS_RESET_SEQUENCE_COMPLETED          Enumerator
                        CF_WAIT_FOR_US_TO_RESET                 Enumerator
                        CF_WAIT_FOR_DS_TO_RESET                 Enumerator

    Local           :   uchString                               Array of BYTEs
                        DS1_Reset_Status                        BYTE
                        DS2_Reset_Status                        BYTE
                        US1_Reset_Status                        BYTE
                        US2_Reset_Status                        BYTE
                        uchPreparatoryState                     BYTE
                        uchPreparatoryState1                        BYTE

Output Variables        Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                       Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            TRUE                            1
                            NORMAL                          1
                            SET_LOW                         0
                            CATASTROPHIC_ERROR              0
                            SM_HAS_RESETTED_SYSTEM          1


References          :

Derived Requirements:
*********************************************************************************/
extern BYTE Relay_B_State, Relay_B_PR_State;
void Control_DAC_Type_LCWS(void)
{
    do {
        ClrWdt();
        CS11 = !CS11;
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16) TIMER3_SETPOINT);
            Decrement_TrackMon_50msTmr_1();               /* from axle_mon.c */
            Decrement_TrackMon_50msTmr_2();               /* from axle_mon.c */
            Decrement_TrackMon_50msTmr_3();               /* from axle_mon.c */
            Decrement_TrackMon_50msTmr_4();               /* from axle_mon.c */
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_LCWS_Health_Relay_Timer();
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */

            IFS1bits.T5IF = SET_LOW;
            //Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            Decrement_Reset_msTmr();                    /* from reset.c */
            US_LED_Error_Service_1mS();
            DS_LED_Error_Service_1mS();
            //Decrement_itoa_msTmr();                     /* from display.c */
        }

        if (Status.Flags.System_Status == NORMAL)
        {
            Validate_PD_Signals_1();                      /* from axle_mon.c */
            Validate_PD_Signals_2();                      /* from axle_mon.c */
            Validate_PD_Signals_3();                      /* from axle_mon.c */
            Validate_PD_Signals_4();                      /* from axle_mon.c */
            Chk_for_AxleCount_Completion_1();             /* from axle_mon.c */
            Chk_for_AxleCount_Completion_2();             /* from axle_mon.c */
            Chk_for_AxleCount_Completion_3();             /* from axle_mon.c */
            Chk_for_AxleCount_Completion_4();             /* from axle_mon.c */
            if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS)
            {
                Update_LCWS_Relay_State();
            }
            else
            {
               Update_LCWS_DL_Relay_State();
            }
            if(Status.Flags.System_Status != CATASTROPHIC_ERROR)
                ErrorCodeForSPI = 0xFF; 
        }
        else
        {
            if(Relay_B_PR_State == 1)
            {
                DeEnergise_Preparatory_Relay_B();     /* from relaydrv.c */
            }
            if(Relay_B_State == 1)
            {
                DeEnergise_Vital_Relay_B();     /* from relaydrv.c */
            }
        /* Catastrophic Error */
            Update_Display_Error();
        }
        //Update_DS_Sch_State();                          /* from comm_ds.c */
        Update_SPI_State();                             /* from comm_sm.c */
        //Update_LCD_State();                             /* from lcd_drv.c */
        Update_Sys_Mon_State();                         /* from sys_mon.c */
        Update_LCWS_Reset_Monitor_State();                /* from reset.c */
        //Update_itoa_SF_State();                         /* from display.c  */
    } while (Always_true == 1);
}




void Control_DAC_Type_DE(void)
{


        INT16 uiAuthorisationKey;
        uiAuthorisationKey = Get_Relay_Energising_Key();
        Energise_Vital_Relay_B(uiAuthorisationKey);
    do {
        ClrWdt();
        CS11 = !CS11;
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16) TIMER3_SETPOINT);
            Decrement_TrackMon_50msTmr_1();               /* from axle_mon.c */
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */

            IFS1bits.T5IF = SET_LOW;
            //Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            Decrement_Reset_msTmr();                    /* from reset.c */
            DS_LED_Error_Service_1mS();
            //Decrement_itoa_msTmr();                     /* from display.c */
        }

        if (Status.Flags.System_Status == NORMAL)
        {
            Validate_PD_Signals_1();                      /* from axle_mon.c */
            Chk_for_AxleCount_Completion_1();             /* from axle_mon.c */
            Update_DE_Relay_State();
            if(Status.Flags.System_Status != CATASTROPHIC_ERROR)
                ErrorCodeForSPI = 0xFF;         
        }
        else
        {
            /* Catastrophic Error */
            Update_Display_Error();
        }
        //Update_DS_Sch_State();                          /* from comm_ds.c */
        Update_SPI_State();                             /* from comm_sm.c */
        //Update_LCD_State();                             /* from lcd_drv.c */
        Update_Sys_Mon_State();                         /* from sys_mon.c */
        Update_DE_Reset_Monitor_State();                /* from reset.c */
        //Update_itoa_SF_State();                         /* from display.c  */
    } while (Always_true == 1);
}

/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Control_DAC_Type_EF(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :1.Start the down stream axle counting
                     2.If system is normal,validate the PD signals, check for
                       completion of the axle counting,update relay B counts
                     3.If the system is in error update the display to indicate the error
                     4.Update DE reset state
                     5.Update SPI state & LCD state
                     6.Update itoa state for DE unit type for display

Allocated Requirements	: 	(SSDAC_SWRS_0120)

Design Requirements		:	SSDAC_DR_5010
						

Interfaces
    Calls           :   
                        AXLE_MON.c  -   Start_US_Axle_Counting()
                        AXLE_MON.c  -   Decrement_TrackMon_50msTmr()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        COMM_US.c   -   Decrement_US_Sch_msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        AXLE_MON.C  -   Validate_PD_Signals()
                        AXLE_MON.c  -   Chk_for_AxleCount_Completion()
                        RELAYMGR.c  -   Update_Relay_A_Counts()
                        RELAYMGR.c  -   Update_EF_Track_Status()
                        RLYB_MGR.c  -   Update_Relay_B_State()
                        ERROR.c     -   Update_display_error()
                        COMM_US.c   -   Update_US_Sch_State()
                        COMM_SM.c   -   Update_SPI_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        SYS_MON.c   -   Update_Sys_Mon_State()
                        RESET.c     -   Update_EF_Reset_Monitor_State()
                        DISPLAY.c   -   Update_itoa_EF_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                                    Type
    Global          :   DIP_Switch_Info.Flags.ATC_Enabled       Bit
                        TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status              Bit


    Local           :   uchString                               Array of BYTEs

Output Variables        Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit
                        US_Section_Mode.Local_Unit              BYTE

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                       Value
                            SEVEN_SEGMENT_UNIT1             1
                            SEVEN_SEGMENT_UNIT2             2
                            TRUE                            1
                            NORMAL                          1
                            SET_LOW                         0

References          :

Derived Requirements:
*********************************************************************************/
void Control_DAC_Type_EF(void)
{
//    BYTE uchString[21];
    // Clear_Line_on_LCD((BYTE) 2);
    if( DIP_Switch_Info.Flags.ATC_Enabled != TRUE)
    {
        US_Section_Mode.Local_Unit = WAIT_FOR_PILOT_TRAIN_MODE;
    }
    Start_US_Axle_Counting();
    do {
        ClrWdt();
        CS11 = !CS11;
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16) TIMER3_SETPOINT);
            Decrement_TrackMon_50msTmr();               /* from axle_mon.c */
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c  */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c  */
            Decrement_Err_display_50msTmr();
            Decrement_comm_check_50msTmr();
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_US_Sch_msTmr();                   /* from comm_us.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            Decrement_Reset_msTmr();                    /* from reset.c */
            //Decrement_itoa_msTmr();                     /* from display.c */
         }
        if (Status.Flags.System_Status == NORMAL)
        {
            Validate_PD_Signals();                      /* from axle_mon.c */
            Chk_for_AxleCount_Completion();             /* from axle_mon.c */
            Update_Relay_A_Counts();                    /* from relaymgr.c */
            Update_EF_Track_Status();                   /* from relaymgr.c */
            if(Status.Flags.System_Status != CATASTROPHIC_ERROR)
                ErrorCodeForSPI = 0xFF; 
        }
        else
        {
            /* Catastrophic Error */
            Update_Display_Error();                     /* from error.c */
        }
        Update_US_Sch_State();                          /* from comm_us.c */
        Update_SPI_State();                             /* from comm_sm.c */
        //Update_LCD_State();                             /* from lcd_drv.c */
        Update_Sys_Mon_State();                         /* from sys_mon.c */
        Update_EF_Reset_Monitor_State();                /* from reset.c */
        //Update_itoa_EF_State();                         /* from display.c  */
    } while (Always_true == 1);
}

//19_12_09

/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Control_DAC_Type_D3(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :1.Start the up stream axle counting
                     2.Start the down stream axle counting
                     3.If system is normal,validate the PD signals, check for
                       completion of the axle counting,update relay B counts
                     4.If the system is in error update the display to indicate the error
                     5.Update up stream scheduler,down stream scheduler, D3 reset state
                     6.Update SPI state & LCD state
                     7.Update itoa state for D3 unit type for display

Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   LCD_DRV.c   -   Clear_Line_on_LCD()
                        AXLE_MON.c  -   Start_US_Axle_Counting()
                        AXLE_MON.c  -   Start_DS_Axle_Counting()
                        AXLE_MON.c  -   Decrement_TrackMon_50msTmr()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        AXLE_MON.C  -   Validate_PD_Signals()
                        AXLE_MON.c  -   Chk_for_AxleCount_Completion()
                        RELAYMGR.c  -   Update_Relay_D3_Counts()
                        ERROR.c     -   Update_Display_Error()
                        COMM_SM.c   -   Update_SPI_State()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        SYS_MON.c   -   Update_Sys_Mon_State()
                        RESET.c     -   Update_CF_Reset_Monitor_State()
                        DISPLAY.c   -   Update_itoa_D3_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status              Bit

    Local           :   uchString                               Array of BYTEs

Output Variables        Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                       Value
                            NORMAL                          1
                            SET_LOW                         0


References          :

Derived Requirements:
*********************************************************************************/
void Control_DAC_Type_D3(void)
{
    // Clear_Line_on_LCD((BYTE) 2);
    // Clear_Line_on_LCD((BYTE) 3);
    if( DIP_Switch_Info.Flags.ATC_Enabled != TRUE)
    {
        DS_Section_Mode.Local_Unit = WAIT_FOR_PILOT_TRAIN_MODE;
        DS_Section_Mode.Remote_Unit = DP3_MODE;
    }
    Start_US_Axle_Counting();
    Start_DS_Axle_Counting();
    do {
        ClrWdt();
        CS11 = !CS11;
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16) TIMER3_SETPOINT);
            Decrement_TrackMon_50msTmr();               /* from axle_mon.c */
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_D3_Track_Clearing_Timeout_50msTmr();
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            Decrement_US_Sch_msTmr();                   /* from comm_us.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            Decrement_Reset_msTmr();                    /* from reset.c */
            //Decrement_itoa_msTmr();                     /* from display.c */
        }

        if (Status.Flags.System_Status == NORMAL)
        {
            Validate_PD_Signals();                      /* from axle_mon.c */
            Chk_for_AxleCount_Completion();             /* from axle_mon.c */
            Update_Relay_D3_Counts();                   /* from relaymgr.c */
            if(Status.Flags.System_Status != CATASTROPHIC_ERROR)
                ErrorCodeForSPI = 0xFF; 
        }
        else
        {
            /* Catastrophic Error */
            Update_Display_Error();                     /* from error.c  */
        }
        Update_DS_Sch_State();                          /* from comm_ds.c  */
        Update_US_Sch_State();                          /* from comm_us.c  */
        Update_SPI_State();                             /* from comm_sm.c  */
        //Update_LCD_State();                             /* from lcd_drv.c  */
        Update_Sys_Mon_State();                         /* from sys_mon.c  */
        Update_CF_Reset_Monitor_State();                /* from reset.c    */
        //Update_itoa_D3_State();                         /* from display.c  */
    } while (Always_true == 1);
}

/*********************************************************************************
Component name      :DAC_MAIN
Module Name         :void Control_DAC_Type_D4(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :1.Start the up stream axle counting
                     2.Start the down stream axle counting
                     3.If system is normal,validate the PD signals, check for
                       completion of the axle counting,update relay B counts
                     4.If the system is in error update the display to indicate the error
                     5.Update up stream scheduler,down stream scheduler, D3 reset state
                     6.Update SPI state & LCD state
                     7.Update itoa state for D3 unit type for display

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5012



Interfaces
    Calls           :   LCD_DRV.c   -   Clear_Line_on_LCD()
                        AXLE_MON.c  -   Start_US_Axle_Counting()
                        AXLE_MON.c  -   Start_DS_Axle_Counting()
                        AXLE_MON.c  -   Decrement_TrackMon_50msTmr()
                        SYS_MON.c   -   Start_Sys_Mon_Decrement_50msTmr()
                        COMM_SM.c   -   Decrement_SPI_50msTmr()
                        ERROR.c     -   Decrement_Err_display_50msTmr()
                        LCD_DRV.c   -   Decrement_LCD_msTmr()
                        RESET.c     -   Decrement_Reset_msTmr()
                        DISPLAY.c   -   Decrement_itoa_msTmr()
                        AXLE_MON.C  -   Validate_PD_Signals()
                        AXLE_MON.c  -   Chk_for_AxleCount_Completion()
                        RELAYMGR.c  -   Update_Relay_D3_Counts()
                        ERROR.c     -   Update_Display_Error()
                        COMM_SM.c   -   Update_SPI_State()
                        COMM_DS.c   -   Update_DS_Sch_State()
                        COMM_US.c   -   Update_US_Sch_State()
                        LCD_DRV.c   -   Update_LCD_State()
                        SYS_MON.c   -   Update_Sys_Mon_State()
                        RESET.c     -   Update_CF_Reset_Monitor_State()
                        DISPLAY.c   -   Update_itoa_D3_State()
                        WRITETIMER3 -   Compiler defined function
                        CLRWDT()    -   Compiler defined function

    Called by       :   DAC_MAIN.c  -   main()

Input Variables         Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit
                        Status.Flags.System_Status              Bit

    Local           :   uchString                               Array of BYTEs

Output Variables        Name                                    Type
    Global          :   TMR3IF                                  Timer3 overflow interrupt flag bit
                        TMR4IF                                  Timer4 overflow interrupt flag bit

    Local           :   None

Signal Variables

                                    Nil                          Nil                        Nil

Macro definitions used:     Macro                       Value
                            NORMAL                          1
                            SET_LOW                         0


References          :

Derived Requirements:
*********************************************************************************/
void Control_DAC_Type_D4(void)
{
    // Clear_Line_on_LCD((BYTE) 2);
    // Clear_Line_on_LCD((BYTE) 3);
    if( DIP_Switch_Info.Flags.ATC_Enabled != TRUE)
    {
        DS_Section_Mode.Local_Unit = WAIT_FOR_PILOT_TRAIN_MODE;
    }
    Start_US_Axle_Counting();
    Start_DS_Axle_Counting();
    do {
        ClrWdt();
        CS11 = !CS11;
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16) TIMER3_SETPOINT);
            Decrement_TrackMon_50msTmr();               /* from axle_mon.c */
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            Decrement_D4_Track_Clearing_Timeout_50msTmr();
        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            Decrement_US_Sch_msTmr();                   /* from comm_us.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            Decrement_Reset_msTmr();                    /* from reset.c */
            //Decrement_itoa_msTmr();                     /* from display.c */
        }

        if (Status.Flags.System_Status == NORMAL)
        {
            Validate_PD_Signals();                      /* from axle_mon.c */
            Chk_for_AxleCount_Completion();             /* from axle_mon.c */
            Update_Relay_D4_Counts();                   /* from relaymgr.c */
            if(Status.Flags.System_Status != CATASTROPHIC_ERROR)
                ErrorCodeForSPI = 0xFF; 
        }
        else
        {
            /* Catastrophic Error */
            Update_Display_Error();                     /* from error.c  */
        }
        Update_DS_Sch_State();                          /* from comm_ds.c  */
        Update_US_Sch_State();                          /* from comm_us.c  */
        Update_SPI_State();                             /* from comm_sm.c  */
        //Update_LCD_State();                             /* from lcd_drv.c  */
        Update_Sys_Mon_State();                         /* from sys_mon.c  */
        Update_CF_Reset_Monitor_State();                /* from reset.c    */
        //Update_itoa_D3_State();                         /* from display.c  */
    } while (Always_true == 1);
}

//15_03_10

void Control_DAC_Type_3S(void)
{
    BYTE uchPreparatoryState,uchPreparatoryState1;
    BYTE DS1_Reset_Status,DS2_Reset_Status;
    BYTE US1_Reset_Status,US2_Reset_Status;
//    BYTE uchString[21];

    // Clear_Line_on_LCD((BYTE) 2);
    if( DIP_Switch_Info.Flags.ATC_Enabled != TRUE)
    {
        //strcpy(uchString,"Wait For Pilot Train");
        //Display_on_LCD(3,0,uchString);
    }
    do {
        ClrWdt();
        CS11 = !CS11;
        if (IFS0bits.T3IF)
        {
            /* 50-mS Timer has overflowed */
            IFS0bits.T3IF = SET_LOW;
            //WRITETIMER3((UINT16) TIMER3_SETPOINT);
            Decrement_TrackMon_50msTmr();               /* from axle_mon.c */
            Start_Sys_Mon_Decrement_50msTmr();          /* from sys_mon.c */
            Decrement_SPI_50msTmr();                    /* from comm_sm.c */
            Decrement_Err_display_50msTmr();
            uchPreparatoryState1 = Get_CF_US_Reset_State();
            if(uchPreparatoryState1 != (BYTE)CF_US_RESET_SEQUENCE_COMPLETED)
               {
                 US1_Reset_Status = Get_CF_US1_Reset_State();
                 US2_Reset_Status = Get_CF_US2_Reset_State();
                  if((uchPreparatoryState1 == (BYTE) CF_WAIT_FOR_US_TO_RESET)||
                     (US1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM)||
                      (US2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
                     {

                       Decrement_Reset_300sTmr2();
                     }
                }

              uchPreparatoryState = Get_CF_DS_Reset_State();
             if(uchPreparatoryState != (BYTE)CF_DS_RESET_SEQUENCE_COMPLETED)
                {
                  DS1_Reset_Status = Get_CF_DS1_Reset_State();
                  DS2_Reset_Status = Get_CF_DS2_Reset_State();
                 if((uchPreparatoryState == (BYTE) CF_WAIT_FOR_DS_TO_RESET)||
                     (DS1_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM)||
                      (DS2_Reset_Status == (BYTE)SM_HAS_RESETTED_SYSTEM))
                       {
                          Decrement_Reset_300sTmr();
                       }
                 }

        }
        if (IFS1bits.T5IF)
        {
            /* 1-mS Timer has overflowed */
            IFS1bits.T5IF = SET_LOW;
            Decrement_DS_Sch_msTmr();                   /* from comm_ds.c */
            Decrement_US_Sch_msTmr();                   /* from comm_us.c */
            //Decrement_LCD_msTmr();                      /* from lcd_drv.c */
            Decrement_Reset_msTmr();                    /* from reset.c */
            //Decrement_itoa_msTmr();                     /* from display.c */
        }
        if (Status.Flags.System_Status == NORMAL)
        {
            Validate_PD_Signals();                      /* from axle_mon.c */
            Chk_for_AxleCount_Completion();             /* from axle_mon.c */
            Update_3S_Relay_Counts();                   /* from relaymgr.c */
            Update_CF_Track_Status();                   /* from relaymgr.c */
            if(Status.Flags.System_Status != CATASTROPHIC_ERROR)
                ErrorCodeForSPI = 0xFF; 
        }
        if((Status.Flags.US_System_Status == CATASTROPHIC_ERROR)||
           (Status.Flags.DS_System_Status == CATASTROPHIC_ERROR))
        {
            /* Catastrophic Error */
            Update_Display_Error();                     /* from error.c    */
        }
        Update_DS_Sch_State();                          /* from comm_ds.c  */
        Update_US_Sch_State();                          /* from comm_us.c  */
        Update_SPI_State();                             /* from comm_sm.c  */
        //Update_LCD_State();                             /* from lcd_drv.c  */
        Update_Sys_Mon_State();                         /* from sys_mon.c  */
        Update_CF_Reset_Monitor_State();                /* from reset.c    */

    } while (Always_true == 1);
}


/*********************************************************************
Component name      :RESET
Module Name         :void Decrement_Bootuplock_50msTmr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
                    Rev No          Date        Description
                     --              --             --




Abstract            :Decremnet the bootup timer

Allocated Requirements	: 	SSDAC_SWRS_0841,SSDAC_SWRS_0859,SSDAC_SWRS_0860,SSDAC_SWRS_0861,SSDAC_SWRS_0862
							SSDAC_SWRS_0863

Design Requirements		: 	

Interfaces
    Calls           :   NIL

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_D3_Reset_Sequence()


Input Variables         Name                                Type
    Global          :   Bootup_Lock.Timeout_50ms           UINT16


    Local           :   None

Output Variables        Name                                Type
    Global          :   Bootup_Lock.Timeout_50ms           UINT16


    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None


References          :
Derived Requirements:
************************************************************************/
void Decrement_Bootuplock_50msTmr(void)
{
  if(Bootup_Lock.Timeout_50ms > 0)
   {
     Bootup_Lock.Timeout_50ms = Bootup_Lock.Timeout_50ms - 1;
   }
}

void Initialize_Reset(void)
{
    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_SF:
            Reset_Info.SF.DS_State =  SF_WAIT_FOR_RESET;
            break;
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
            Reset_Info.CF.DS_State =  CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State =  CF_US_WAIT_FOR_RESET;
            break;
        case DAC_UNIT_TYPE_D3_A:
            Reset_Info.CF.DS_State =  CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State =  CF_US_WAIT_FOR_RESET;
            break;
        case DAC_UNIT_TYPE_D3_B:
            Reset_Info.CF.DS_State =  CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State =  CF_US_WAIT_FOR_RESET;
            break;
        case DAC_UNIT_TYPE_D3_C:
            Reset_Info.CF.DS_State =  CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State =  CF_US_WAIT_FOR_RESET;
            break;
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            Reset_Info.CF.DS_State =  CF_DS_WAIT_FOR_RESET;
            Reset_Info.CF.US_State =  CF_US_WAIT_FOR_RESET;
            break;
        case DAC_UNIT_TYPE_EF:
            Reset_Info.EF.US_State =  EF_WAIT_FOR_RESET;
            break;
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
            Reset_Info.SF.DS_State =  SF_WAIT_FOR_RESET;
            break;
        case DAC_UNIT_TYPE_DE:
            Reset_Info.SF.DS_State =  SF_WAIT_FOR_RESET;
            break;
        default:
            break;
     }


}


void Update_SPI_Error(void)
{
    if (ErrorCodeForSPI == 255)
    {
        if (SPI_Failure.fail_bits.DS_Failure == 1)
        {
            ErrorCodeForSPI = DS_ERROR_NUM;
        }
        else if (SPI_Failure.fail_bits.US_Failure == 1)
        {
            ErrorCodeForSPI = US_ERROR_NUM;
        }
    }
}
