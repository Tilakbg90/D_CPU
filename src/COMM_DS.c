/**************************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    COMM_DS
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
    Functions   :   void SetupCOM2BaudRate(BYTE uchBaudRate)
                    void Initialise_DS_CommSch(void)
                    void Start_DS_CommSch(void)
                    void Update_DS_Sch_State(void)
                    void Receive_COM2_Message(void)
                    void Set_DS_Sch_Idle(void)
                    void Decrement_DS_Sch_msTmr(void)
                    void Decrement_Comm_DS_CountDown(void)
                    void Build_DS_Message(void)
                    void Build_DS_Reset_Info_Message(void)
                    void Build_DS_Axle_Count_Message(void)
                    void Process_DS_Message(void)
                    void Process_DS_Reset_Info_Message(void)
                    void Process_DS_Axle_Count_Message(void)
                    void Process_DS_AxleCount(bitadrb_t SrcAdr,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void Process_DS_Direction(bitadrb_t SrcAdr, BYTE uchDirection)
                    void Synchronise_DS_Sch(bitadrb_t SrcAdr)
                    void Reset_DS_CountDown(bitadrb_t SrcAdr,bitadrb_t MsgFlags)
                    void Check_DS_Unit_Status(bitadrb_t Buffer)

********************************************************************************/
#include <xc.h>

#include "COMMON.h"
#include "COMM_DS.h"
#include "CRC16.h"
#include "RESET.h"
#include "SYS_MON.h"
#include "ERROR.h"
#include "AES.h"
#include "RLYD4_MGR.h"




extern  /*near*/  dac_status_t Status;                    /* from DAC_MAIN.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from DAC_MAIN.c */
extern checksum_info_t Checksum_Info;               /* from DAC_MAIN.c */
extern ds_section_mode DS_Section_Mode;             /*from DAC_MAIN.c*/
extern track_status_info Track_Status_Info;
const UINT16 uiCOM2_BalanceTicks_Table[2][2] = {
                        /*
                         * Communcation Sequence is LU1-RU1-LU2-RU2
                         * Message originated from Unit...
                         *         RU1          RU2
                         *         CPU1         CPU2
                         *         Telegram     Telegram
                         * RUx -> Remote Unit CPUx (Down Stream)
                         */

    /* Receiving Unit CPU1 */   {600,            50},
    /* Receiving Unit CPU2 */   { 50,            600}
    };

 /*near*/  comm_sch_info_t DS_Sch_Info;
           extern comm_fail_check comm_check_SF1;
           extern comm_fail_check comm_check_SF2;

comm_b_countdown_t Comm_B_CountDown =
            {MAXIMUM_COM_RETRIES, MAXIMUM_COM_RETRIES,
             MAXIMUM_COM_RETRIES, MAXIMUM_COM_RETRIES};

msg_info_t   Com2XmitObject;    /* COM2: Message Transmission Buffer etc., */
msg_info_t   Com2RecvObject;    /* COM2: Message Receiving Buffer etc., */
modem_info_t  Comm_Modem_B;
unsigned long int ds_pkt_error_time = 0;
unsigned int COM_DS_pkt_error_cnt = 0;


void Receive_COM2_Message(void);
void Process_DS_Message(void);
void Process_DS_Reset_Info_Message(void);
void Process_DS_Axle_Count_Message(void);
void Process_DS_AxleCount(bitadrb_t SrcAdr, UINT16 uiFwdAxleCount, UINT16 uiRevAxleCount);
void Process_3S_DS_AxleCount(bitadrb_t SrcAdr, UINT16 uiFwdAxleCount, UINT16 uiRevAxleCount); /* added for 3D3S configuration */
void Process_DS_Direction(bitadrb_t SrcAdr, BYTE uchDirection, UINT16 axle_Count);
void Decrement_Comm_DS_CountDown(void);
void Set_DS_Sch_Idle(void);
void Build_DS_Message(void);
void Build_DS_Reset_Info_Message(void);
void Build_DS_Axle_Count_Message(void);
void Synchronise_DS_Sch(bitadrb_t SrcAdr);
void Reset_DS_CountDown(bitadrb_t SrcAdr, bitadrb_t MsgFlags);
void Check_DS_Unit_Status(bitadrb_t Buffer);
void DS_LED_Error_Service_1mS(void);
void Clear_DS_Transmit_Buffer(void);
void Clear_COM2_Errors(void);
/*********************************************************************

Component name      :COMM_DS
Module Name         :void SetupCOM2BaudRate(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Set up Baud Rate for USART communication.Set for Asynchronous mode.
                     Enable the Serial port pins.clear the bitTX92 and RX92 for 8 bit
                     transmission and reception.Disable theTransmit and Receive interrupt,
                     disable the continuous reception

Allocated Requirements	: 	(SSDAC_SWRS_0025)	

Design Requirements		:	SSDAC_DR_5045

Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Initialise_System()

Input Variables         Name                            Type
    Global          :   DIP_Switch_Info.Baud_Rate       Baud_Rate_Config
                        BAUDRATE_1200                   Enumerator
                        BAUDRATE_2400                   Enumerator
                        BAUDRATE_9600                   Enumerator
                        BAUDRATE_19200                  Enumerator

    Local           :   None

Output Variables        Name                            Type
    Global          :   BRGH2                           8 Bit register
                        BRG162                          8 Bit register
                        SPBRG2                          8 Bit register
                        SPBRGH2                         8 Bit register
                        SYNC2                           TXSTA2 control register bit
                        SPEN2                           RCSTA2 control register bit
                        TX92                            TXSTA2 control register bit
                        RX92                            RCSTA2 control register bit
                        TX2IE                           PIE3 peripharal interrupt register
                        RC2IE                           PIE3 peripharal interrupt register
                        TXEN2                           TXSTA2 control register bit
                        SREN2                           RCSTA2 control register bit
                        CREN2                           RCSTA2 control register bit
                        WUE2                            BAUDCON2 register bit
                        ABDEN2                          BAUDCON2 register bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            BRGH_FOR_1200                   1
                            BRGH_FOR_2400                   1
                            BRGH_FOR_9600                   1
                            BRGH_FOR_19200                  1
                            BRGH16_FOR_1200                 1
                            BRGH16_FOR_2400                 1
                            BRGH16_FOR_9600                 1
                            BRGH16_FOR_19200                1
                            SPBRG_FOR_1200                  8332
                            SPBRG_FOR_2400                  4165
                            SPBRG_FOR_9600                  1040
                            SPBRG_FOR_19200                 520
                            SET_LOW                         0
                            SET_HIGH                        1

References          :

Derived Requirements:

Algorith            :1.Set the communication baud rate as per the selected dip switch value
                     2.Set the mode of communication as asynchronous
                     3.Enable the serial port
                     4.Set for 8bit transmission and reception
                     5.Disable the interrupts,continuos reception,auto baud rate detection
************************************************************************/
void SetupCOM2BaudRate(void)
{
    if(DIP_Switch_Info.COM2_Mode == (comm_type)FSK_COMMUNICATION )
    {
        U2BRG = 3332;
    }
    else
    {
        switch (DIP_Switch_Info.Baud_Rate)
        {
            case BAUDRATE_1200:
                U2BRG = 3332;
                break;
            case BAUDRATE_2400:
                U2BRG = 1665;
                break;
            case BAUDRATE_9600:
                U2BRG = 415;
                break;
            case BAUDRATE_19200:
                U2BRG = 207;
                break;
        }
    }


    TRISFbits.TRISF13 = 0;       // RF13 is Tx, Output
    TRISDbits.TRISD14 = 1;       // RD14 is Rx, Input

    /* Configure Remappable pins */
    //*************************************************************
    // Unlock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON & 0xbf); //clear the bit 6 of OSCCONL to unlock Pin Re-map


    RPINR19bits.U2RXR = 43;           // Map U2RX input to RPI43
    RPOR15bits.RP31R = 5;             // Map RP31 to U2TX Output
    //************************************************************
    // Lock Registers
    //************************************************************
    __builtin_write_OSCCONL(OSCCON | 0x40); //set the bit 6 of OSCCONL to lock Pin Re-map
    //******************************************************************

    U2MODE = 0;
    U2STA = 0;

    // Operate at 8 bit mode, No Parity, 1 Stop bit
    U2MODEbits.RTSMD = 1;   // Operate in Simplex Mode

    U2MODEbits.BRGH = 1;    // High Speed Clocks to BRG @ 4x of Fp



    IEC4bits.U2ERIE = 0;
    IEC1bits.U2RXIE = 0;
    IEC1bits.U2TXIE = 0;

    IFS4bits.U2ERIF = 0;
    IFS1bits.U2RXIF = 0;
    IFS1bits.U2TXIF = 0;

    U2MODEbits.UARTEN = 1;  // Enable UART


}

/*********************************************************************

Component name      :COMM_DS
Module Name         :void Initialise_DS_CommSch(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Down stream communication scheduler initailisation will be
                     done here (Byte period, Scan period,Transmission time)


Allocated Requirements	: 	(SSDAC_SWRS_0032)	

Design Requirements		:	SSDAC_DR_5046

Interfaces
    Calls           :   NIL

    Called by       :   DAC_MAIN.c  -   Initialise_System()

Input Variables                     Name                Type
    Global          :   COM_RECV_BUFFER_EMPTY           Enumerator
                        COMM_SCHEDULER_NOT_STARTED      Enumerator
                        COMM_SCHEDULER_NOT_STARTED      Enumerator
                        DIP_Switch_Info.Baud_Rate       Baud_Rate_Config
                        BAUDRATE_1200                   Enumerator
                        BAUDRATE_2400                   Enumerator
                        BAUDRATE_9600                   Enumerator
                        BAUDRATE_19200                  Enumerator

    Local           :   uchCnt                          BYTE

Output Variables                    Name                        Type
    Global          :   Com2XmitObject.State                    Enumerator
                        Com2XmitObject.Index                    BYTE
                        Com2RecvObject.State                    Enumerator
                        Com2RecvObject.Index                    BYTE
                        Com2XmitObject.Msg_Length               BYTE
                        Com2RecvObject.Timeout_ms               UINT16
                        Com2RecvObject.Consecutive_Error_Count  BYTE
                        Com2RecvObject.Msg_Length               BYTE
                        DS_Sch_Info.State                       Enumerator
                        Com2XmitObject.Msg_Buffer               Array of BYTEs
                        DS_Sch_Info.ScanPeriod                  UINT16
                        Com2XmitObject.BytePeriod               BYTE
                        Com2RecvObject.BytePeriod               BYTE
                        Com2XmitObject.Transmission_Time        BYTE

    Local           :   None

Signal Variables

                                MODEM_B_CHANNEL_CONTROL         Output                  LATH6

Macro definitions used:     Macro                           Value
                            NO_MSG_TO_XMIT                  0
                            COMM_MESSAGE_LENGTH             14
                            RELEASE_MODEM_CHANNEL           0
                            COMM_SCHEDULER_SCAN_RATE        2400
                            BYTE_PERIOD_9MS                 9
                            BYTE_PERIOD_5MS                 5
                            BYTE_PERIOD_1MS                 1

References          :

Derived Requirements:

Algorithm           :1.Set the communication transmit state as "NO_MSG_TO_XMIT" and load the transmission length as 14
                     2.Set the downstream scheduler state as "COMM_SCHEDULER_NOT_STARTED" scan period as 2400
                     3.Clear the transmission object buffer
                     4.Select the transmission and reception byte period as per the baudrate setted and then calculate the total transmission time
************************************************************************/
void Initialise_DS_CommSch(void)
{
    BYTE uchCnt;

    Com2XmitObject.State = NO_MSG_TO_XMIT;
    Com2XmitObject.Index = 0;
    Com2XmitObject.Msg_Length  = COMM_MESSAGE_LENGTH;
    Com2RecvObject.State = (BYTE)COM_RECV_BUFFER_EMPTY;
    Com2RecvObject.Index = 0;
    Com2RecvObject.Timeout_ms = 0;
    Com2RecvObject.Consecutive_Error_Count = 0;
    Com2RecvObject.Msg_Length = COMM_MESSAGE_LENGTH;
    DS_Sch_Info.State = COMM_SCHEDULER_NOT_STARTED;
    MODEM_B_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
    DS_Sch_Info.ScanPeriod = COMM_SCHEDULER_SCAN_RATE;
    for (uchCnt = 0; uchCnt < COMM_MESSAGE_LENGTH; uchCnt++)
    {
      Com2XmitObject.Msg_Buffer[uchCnt] = (BYTE) 0;
    }
    if(DIP_Switch_Info.COM2_Mode == (comm_type)FSK_COMMUNICATION)
    {
     Com2XmitObject.BytePeriod = BYTE_PERIOD_9MS;
     Com2RecvObject.BytePeriod = BYTE_PERIOD_9MS;
    }
    else
    {
     switch (DIP_Switch_Info.Baud_Rate)
      {
        case BAUDRATE_1200:
            Com2XmitObject.BytePeriod = BYTE_PERIOD_9MS;
            Com2RecvObject.BytePeriod = BYTE_PERIOD_9MS;
            break;
        case BAUDRATE_2400:
            Com2XmitObject.BytePeriod = BYTE_PERIOD_5MS;
            Com2RecvObject.BytePeriod = BYTE_PERIOD_5MS;
            break;
        case BAUDRATE_9600:
            Com2XmitObject.BytePeriod = BYTE_PERIOD_1MS;
            Com2RecvObject.BytePeriod = BYTE_PERIOD_1MS;
            break;
        case BAUDRATE_19200:
            Com2XmitObject.BytePeriod = BYTE_PERIOD_1MS;
            Com2RecvObject.BytePeriod = BYTE_PERIOD_1MS;
            break;
      }
    }
    Com2XmitObject.Transmission_Time = (BYTE)((Com2XmitObject.Msg_Length + (BYTE) 2) * Com2XmitObject.BytePeriod);
    Comm_Modem_B.State = MODEM_IDLE;
    Comm_Modem_B.Timeout_ms = 0;
}

/*********************************************************************
Component name      :COMM_DS
Module Name         :Start_DS_CommSch(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Assign the communication scheduler time to CPU1 and CPU2.
                     Enable the Transmitter and Continous Receive

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5047


Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()

Input Variables                     Name                        Type
    Global          :   COMM_SCHEDULER_IDLE                 Enumerator
                        DIP_Switch_Info.Flags.Is_CPU1       BIT


    Local           :   NONE

Output Variables                    Name                        Type
    Global          :       DS_Sch_Info.State               Enumerator
                            DS_Sch_Info.Timeout_ms          UINT16
                            TXEN2                           TXSTA2 control register bit
                            CREN2                           RCSTA2 control register bit

    Local           :   NONE

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                        COMM_RE_SYNCHRONIZATION_TIME        4800
                        COMM_SCHEDULER_SCAN_RATE            2400
                        SET_HIGH                            1

References          :

Derived Requirements:

************************************************************************/
void Start_DS_CommSch(void)
{
    DS_Sch_Info.State = COMM_SCHEDULER_IDLE;

    if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
    {
        DS_Sch_Info.Timeout_ms = (UINT16) (COMM_RE_SYNCHRONIZATION_TIME);
        Comm_Modem_B.Timeout_ms = 0;
    }
    else
    {
        DS_Sch_Info.Timeout_ms = (UINT16)(COMM_RE_SYNCHRONIZATION_TIME+(COMM_SCHEDULER_SCAN_RATE/2));
        Comm_Modem_B.Timeout_ms = 30;
    }
    //TXEN2 = SET_HIGH;   /* Enable Transmitter */
    //CREN2 = SET_HIGH;   /* Enable Continous Receive */
    U2STAbits.UTXEN = SET_HIGH;   /* Enable Transmitter */

}

/*********************************************************************
Component name      :COMM_DS
Module Name         :void Update_DS_Sch_State(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the communication status.calls Receive_COM2_Message().
                     If received message is valid then process it.

Allocated Requirements	: 	(SSDAC_SWRS_0261), (SSDAC_SWRS_0262)
							(SSDAC_SWRS_0582), (SSDAC_SWRS_0586), (SSDAC_SWRS_0902)
							(SSDAC_SWRS_0904),(SSDAC_SWRS_0909), (SSDAC_SWRS_0588), (SSDAC_SWRS_0589)
							(SSDAC_SWRS_0594), (SSDAC_SWRS_0518), (SSDAC_SWRS_0653), (SSDAC_SWRS_0753)
							(SSDAC_SWRS_0562), (SSDAC_SWRS_0654), (SSDAC_SWRS_0754), (SSDAC_SWRS_0674), 
							(SSDAC_SWRS_0774), (SSDAC_SWRS_0678), (SSDAC_SWRS_0778), 
							(SSDAC_SWRS_0680), (SSDAC_SWRS_0780)
							(SSDAC_SWRS_0681), (SSDAC_SWRS_0686), (SSDAC_SWRS_0781), (SSDAC_SWRS_0786)
							(SSDAC_SWRS_0386), (SSDAC_SWRS_0390), (SSDAC_SWRS_0392), (SSDAC_SWRS_0393)
							(SSDAC_SWRS_0398), (SSDAC_SWRS_0008)
								
Design Requirements		:	SSDAC_DR_5048

Interfaces
    Calls           :   COMM_DS.C   -   Receive_COM2_Message()
                        COMM_DS.c   -   Process_DS_Message()
                        COMM_DS.c   -   Decrement_Comm_DS_CountDown()
                        COMM_DS.c   -   Set_DS_Sch_Idle()
                        COMM_DS.C   -   Build_DS_Message()
                        RELAYMGR.c  -   Declare_DAC_Defective_DS()
                        ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables                     Name                    Type
    Global          :   Com2RecvObject.State                Enumerator
                        COM_VALID_MESSAGE                   Enumerator
                        COM_RECV_BUFFER_EMPTY               Enumerator
                        DS_Sch_Info.State                   Enumerator
                        COMM_SCHEDULER_NOT_STARTED          Enumerator
                        COMM_SCHEDULER_IDLE                 Enumerator
                        DS_Sch_Info.Timeout_ms              UINT16
                        GET_MODEM_CHANNEL                   Enumerator
                        SET_MODEM_RS_LOW                    Enumerator
                        DIP_Switch_Info.Flags.Is_CPU1       Bit
                        WAIT_FOR_CS_LOW                     Enumerator
                        WAIT_FOR_CARRIER_STABALISE          Enumerator
                        ERROR_MODEM_NO_CARRIER              Enumerator
                        TRANSMIT_TELEGRAM1                  Enumerator
                        Com2XmitObject.Transmission_Time    BYTE
                        Com2XmitObject.Index                BYTE
                        WAIT_FOR_DELAY_BETWEEN_TELEGRAMS    Enumerator
                        TX2IF                               Peripheral Interrupt register bit
                        Com2XmitObject.Msg_Buffer           Array of BYTEs
                        TRANSMIT_TELEGRAM2                  Enumerator
                        HOLD_MODEM_RS_LOW                   Enumerator

    Local           :   Telegram_No                         BYTE

Output Variables            Name                            Type
    Global          :   Com2RecvObject.State                Enumerator
                        DS_Sch_Info.State                   Enumerator
                        DS_Sch_Info.Timeout_ms              UINT16
                        DS_Telegram_No                      BYTE
                        Status.Flags.Modem_B                Bit
                        TXREG2                              USART2 Transmit Register
                        Com2XmitObject.Index                BYTE
                        Status.Flags.Modem_B                Bit

    Local           :   None

Signal Variables

                            MODEM_B_CHANNEL_STATUS              Input                   RH7
                            MODEM_B_CHANNEL_CONTROL             Output                  LATH6
                            MODEM_B_CD                          Input                   RJ1
                            MODEM_B_M1                          Output                  LATJ2
                            MODEM_B_M0                          Output                  LATJ0

Macro definitions used:     Macro                               Value
                            TIMEOUT_EVENT                       0
                            MODEM_CHANNEL_FREE                  0
                            GRAB_MODEM_CHANNEL                  1
                            WAIT_FOR_RS_LOW_TIME                1100
                            FALSE                               0
                            RELEASE_MODEM_CHANNEL               0
                            COMM_RE_SYNCHRONIZATION_TIME        4800
                            SET_HIGH                            1
                            SET_LOW                             0
                            MODEM_TRC_ON_TIMEOUT                186
                            TRANSMIT_WAIT_TIME                  50
                            MODEM_ERROR_NO_CARRIER_ID           24
                            COMM_MESSAGE_LENGTH                 14
                            TIME_BETWEEN_TWO_TELEGRAMS          20
                            MODEM_HOLD_RS_LOW_TIMEOUT           30

References          :

Derived Requirements:

Algorithm           :1.Get the message from the other unit,if it Valid message
                        process it
                     2.Initailly Down Stream communication scheduler will be in
                        Idle state
                     3.After Down Stream scheduler scan time completed,decrement
                        the Down Stream communication retries by 1 and go to step 4
                     4.Check for communication channel status,if it is free grab
                        the channel and go to step 6
                     5.Check whether other processor has also grabbed the channel,
                        if it is grabbed release the channel
                     6.Check for CD and CS HIGH, if both are HIGH go to step 7
                     7.Check for CS low, if it is low build the transimit message
                        and go to step 9
                     8.Check for carrier generation, if it is not generated, release
                        the modem channel declare DAC defective and set modem error
                        no carrier Id
                     9.After carrier stabilisation time over(50ms) go to step 10 to
                        start the transmission
                     10.If meassage length is greater than Maximum message length(12)
                        go to step 13
                     11.Check for USART transmitter buffer interrupt flag is set
                        means TXREG is empty, load the data to TXREG2 register
                     12.Check for Down Stream communication scheduler timeout, if it
                        has occured means, make the Down stream scheduler ready for
                        Next transmission
                     13.Check for Down Stream communication scheduler timeout, if it
                        has occured means,build the second message and go to step 14 to
                        send the second telegram
                     14.If meassage length is greater than Maximum message length(12)
                        go to step 17
                     15.Check for USART transmitter buffer interrupt flag is set means
                        TXREG is empty, load the data to TXREG2 register
                     16.Check for Down Stream communication scheduler timeout, if it
                        has occured means, make the Down stream scheduler ready for
                        Next transmission
                     17.Check for Down Stream communication scheduler timeout, if it
                        has occured means, make the Down stream scheduler ready
                        for Next transmission
************************************************************************/

unsigned int ds_failure_led_timer;
#include "RELAYMGR.h"
void DS_LED_Error_Service_1mS(void)        // This is the Master Transmission
{
    ds_failure_led_timer = ds_failure_led_timer + 1;
    if(ds_failure_led_timer >= 100)
    {
        ds_failure_led_timer = 0;
        if(DS_Section_Mode.Local_Unit == SYSTEM_ERROR_MODE || Status.Flags.System_Status != NORMAL)
        {
            SECTION_B_LED_CLEAR_PORT = ~SECTION_B_LED_CLEAR_PORT;
        }
    }
}


void DS_Packet_error_timmer(void)        // This is the Master Transmission
{
    ds_pkt_error_time = ds_pkt_error_time + 1;
    if(ds_pkt_error_time >= PACKET_ERROR_TIME_THRESHOLD)
    {
        ds_pkt_error_time = 0;
        //COM_DS_pkt_error_cnt = 0;
    }
}

void Update_DS_Sch_State(void)
{
   BYTE uchEncipher_State;

    Receive_COM2_Message();

    if (Com2RecvObject.State == (BYTE)COM_VALID_MESSAGE)
    {
        Process_DS_Message();       /* Valid Message received, process it */
        Com2RecvObject.State = (BYTE)COM_RECV_BUFFER_EMPTY;
    }
    switch (DS_Sch_Info.State)
    {
        case TRANSMIT_DUMMY_BYTES:
        case COMM_SCHEDULER_NOT_STARTED:
            break;
        case COMM_SCHEDULER_IDLE:
            if (DS_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Decrement_Comm_DS_CountDown();
                DS_Sch_Info.State = GET_MODEM_CHANNEL;
            }
            break;
        case GET_MODEM_CHANNEL:
            if (MODEM_B_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
            {
                MODEM_B_CHANNEL_CONTROL = GRAB_MODEM_CHANNEL;
                if (MODEM_B_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
                {
                    DS_Sch_Info.State = SET_MODEM_TX_MODE;
                }
                else
                {
                    /* Simultaneously other processor has also grabbed the channel, So release it */
                    if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == FALSE)
                    {
                        /*
                         * Simultaneously other processor has also grabbed the channel.
                         * CPU2 will relenqish the channel in favour of CPU1
                         */
                        MODEM_B_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
                        DS_Sch_Info.Timeout_ms = (UINT16) (COMM_RE_SYNCHRONIZATION_TIME);
                        DS_Sch_Info.State = COMM_SCHEDULER_IDLE;
                    }
                }
                break;
            }
            if (MODEM_B_CHANNEL_STATUS != MODEM_CHANNEL_FREE)
            {
                DS_Sch_Info.Timeout_ms = (UINT16) (COMM_RE_SYNCHRONIZATION_TIME);
                DS_Sch_Info.State = COMM_SCHEDULER_IDLE;
            }
            break;
        case SET_MODEM_TX_MODE:
            if(DIP_Switch_Info.COM2_Mode == (comm_type)FSK_COMMUNICATION)
            {
              if(MODEM_B_CD == FALSE)
              {
                MODEM_B_M0 = SET_HIGH;
                MODEM_B_M1 = SET_LOW;
                Clear_COM2_Errors();
              }
                Clear_DS_Transmit_Buffer();
                Build_DS_Message();
                DS_Sch_Info.State = WAIT_FOR_DATA_ENCRYPT;
                DS_Sch_Info.Timeout_ms = TRANSMIT_WAIT_TIME;
                Start_DS_Data_Encryption();
            }
            else
            {
                Clear_DS_Transmit_Buffer();
                Build_DS_Message();
                DS_Sch_Info.State = WAIT_FOR_DATA_ENCRYPT;
                DS_Sch_Info.Timeout_ms = TRANSMIT_WAIT_TIME;
                Start_DS_Data_Encryption();
            }
            break;
        case WAIT_FOR_DATA_ENCRYPT:
            Update_DS_Data_Encode();
            uchEncipher_State = Get_DS_Encryption_State();
            if(uchEncipher_State == (BYTE)ENCRYPTION_COMLETED)
            {
             Set_DS_Encryption_Idle();
             DS_Sch_Info.State = WAIT_FOR_CARRIER_STABALISE;
            }
            if (DS_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Set_DS_Sch_Idle();
            }
            break;
        case WAIT_FOR_CARRIER_STABALISE:
            if (DS_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                /* Carrier Stabilisation time of 50 ms go now for transmission */
                DS_Sch_Info.State = TRANSMIT_TELEGRAM;
                DS_Sch_Info.Timeout_ms = Com2XmitObject.Transmission_Time;
            }
            break;
        case TRANSMIT_TELEGRAM:
            if (Com2XmitObject.Index >= COMM_MESSAGE_LENGTH)
            {
                if(U2STAbits.TRMT == 1)
                {
                    DS_Sch_Info.State      = HOLD_MODEM_RS_LOW;
                    DS_Sch_Info.Timeout_ms = MODEM_HOLD_RS_LOW_TIMEOUT;
                }

                break;
            }

            if (U2STAbits.UTXBF == 0)
            {
                U2TXREG = Com2XmitObject.Msg_Buffer[Com2XmitObject.Index];
                Com2XmitObject.Index = Com2XmitObject.Index + 1;
            }
            if (DS_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Set_DS_Sch_Idle();
            }
            break;
        case HOLD_MODEM_RS_LOW:
            if (DS_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Set_DS_Sch_Idle();
            }
            break;
        case ERROR_MODEM_NO_CARRIER:
            break;
    }

}
/*********************************************************************
Component name      :COMM_DS
Module Name         :Set_DS_Sch_Idle(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Make the Down stream scheduler ready for Next transmission.
                     Calculate the remaining time after the transmission of bytes and make the scheduler
                      state idle upto the calculated time has to overflow and becomes zero.

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5049


Interfaces
    Calls           :   Nil

    Called by       :   COMM_DS.c   -   Update_DS_Sch_State()

Input Variables         Name                            Type
    Global          :   COMM_SCHEDULER_IDLE             Enumerator
                        DS_Sch_Info.ScanPeriod          UINT16
                        DS_Sch_Info.ElapsedTime         UINT16

    Local           :   uiTmp                           UINT16

Output Variables        Name                            Type
    Global          :   DS_Sch_Info.State               Enumerator
                        DS_Sch_Info.Timeout_ms          UINT16

    Local           :   None
Signal Variables

                            MODEM_B_M0                          Output                  LATJ0
                            MODEM_B_CHANNEL_CONTROL             Output                  LATH6

Macro definitions used:     Macro                           Value
                            SET_HIGH                            1
                            RELEASE_MODEM_CHANNEL               0

References          :

Derived Requirements:

Algorithm           :1.Set the downstream scheduler state as idle
                     2.Calculate the  scheduler timeout
                     3.Release the modem channel
************************************************************************/
void Set_DS_Sch_Idle(void)
{
    UINT16 uiTmp =0;

        DS_Sch_Info.State = COMM_SCHEDULER_IDLE;
        if (DS_Sch_Info.ElapsedTime > DS_Sch_Info.ScanPeriod )
        {
            uiTmp = (DS_Sch_Info.ElapsedTime % COMM_SCHEDULER_SCAN_RATE);
        }
        else
        {
            uiTmp = DS_Sch_Info.ElapsedTime;
        }
        DS_Sch_Info.Timeout_ms = DS_Sch_Info.ScanPeriod - uiTmp;
         if(DIP_Switch_Info.COM2_Mode == (comm_type)FSK_COMMUNICATION)
         {
            MODEM_B_M0 = SET_LOW;
            MODEM_B_M1 = SET_HIGH;
         }
        MODEM_B_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Receive_COM2_Message(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Checks for Overrrun error, framing error and Recieves
                     the data.


Design Requirements     :

Interfaces
    Calls           :   CRC16.c -   Crc16()

    Called by       :   COMM_DS.c   -   Update_DS_Sch_State()

Input Variables         Name                                    Type
    Global          :   OERR2                                   RCSTA2 control register bit
                        FERR2                                   RCSTA2 control register bit
                        RC2IF                                   Peripheral Interrupt register bit
                        COM_RECV_BUFFER_EMPTY                   Enumerator
                        COM_RECV_SRC_ADDR                       Enumerator
                        Com2RecvObject.Index                    BYTE
                        COM_RECV_DATA_ID                        Enumerator
                        COM_RECV_DATA_BYTES                     Enumerator
                        COM_CHECK_CRC16                         Enumerator
                        Com2RecvObject.State                    Enumerator
                        COM_INVALID_MESSAGE                     Enumerator
                        Status.Flags.Modem_B_Err_Toggle_Bit     Bit
                        COM_VALID_MESSAGE                       Enumerator

    Local           :   Crc16_Return_Value                      UINT16
                        uchData                                 BYTE

Output Variables        Name                                    Type
    Global          :   TXEN2                                   TXSTA2 control register bit
                        CREN2                                   RCSTA2 control register bit
                        RCREG2                                  USART2 receiver buffer register
                        Com2RecvObject.Timeout_ms               UINT16
                        Com2RecvObject.Msg_Buffer               Array of BYTES
                        Com2RecvObject.Index                    BYTE
                        Com2RecvObject.State                    Enumerator
                        Status.Flags.Modem_B_Err_Toggle_Bit     Bit


    Local           :   None
Signal Variables

                                MODEM_B_CD                          Input                   RJ1

Macro definitions used:     Macro                           Value
                            SET_LOW                         0
                            SET_HIGH                        1
                            COM_INVALID_MESSAGE_TIMEOUT     18
                            COMM_MESSAGE_LENGTH             14
                            TIMEOUT_EVENT                   0
References          :

Derived Requirements:

Algorithm           :1.Check for overrun error, if it is there clear the error
                     2.Check for Framing error, if it is there clear the error
                     3.Until reciever buffer become empty load the recieved data
                        to reciever message buffer
                     4.Check for communiction CRC, if it invalid set the recieve
                        message state to INVALID_MESSAGE
                     5.Check for recieve message state EMPTY,if message is not
                        in expected format/incomplete, probably it is out of SYNC
                     6.Check for invalid message and check for Carrier Detection,
                        if it is not generated release the buffer


************************************************************************/
void Clear_COM2_Errors(void)
{
    if(U2STAbits.FERR == 1)
    {
        U2STAbits.FERR = 0;
//        uchData = U2RXREG;
//        while(U2STAbits.URXDA)
//        {
//            uchData = U2RXREG;
//        }
//        U2MODEbits.UARTEN = 0;
//        Nop();
//        Nop();
//        U2MODEbits.UARTEN = 1;
    }
    if(U2STAbits.OERR == 1)
    {
        U2STAbits.OERR = 0;
//        uchData = U2RXREG;
//        while(U2STAbits.URXDA)
//        {
//            uchData = U2RXREG;
//        }
//        U2MODEbits.UARTEN = 0;
//        Nop();
//        Nop();
//        U2MODEbits.UARTEN = 1;
    }
    if(U2STAbits.PERR == 1)
    {
        U2STAbits.PERR = 0;
//        uchData = U2RXREG;
//        while(U2STAbits.URXDA)
//        {
//            uchData = U2RXREG;
//        }
//        U2MODEbits.UARTEN = 0;
//        Nop();
//        Nop();
//        U2MODEbits.UARTEN = 1;
    }
}
/*********************************************************************
Component name 		:COMM_DS   	                                                        
Module Name			:void Receive_COM2_Message(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
								   
					
Abstract			:Checks for Overrrun error, framing error and Recieves 
					 the data.

Allocated Requirements	:	(SSDAC_SWRS_0551), (SSDAC_SWRS_0256), (SSDAC_SWRS_0286), (SSDAC_SWRS_0286)
							(SSDAC_SWRS_0287), (SSDAC_SWRS_0288), (SSDAC_SWRS_0289), (SSDAC_SWRS_0290)	
							(SSDAC_SWRS_0291), (SSDAC_SWRS_0292), (SSDAC_SWRS_0293), (SSDAC_SWRS_0294)
							(SSDAC_SWRS_0295), (SSDAC_SWRS_0296), (SSDAC_SWRS_0298)
							(SSDAC_SWRS_0648), (SSDAC_SWRS_0748), (SSDAC_SWRS_0348), (SSDAC_SWRS_0007)

Design Requirements		:	SSDAC_DR_5050
	    	
Interfaces
	Calls			:	CRC16.c	-	Crc16()

	Called by		:	COMM_DS.c	-	Update_DS_Sch_State()

Input Variables			Name									Type
	Global			:	OERR2									RCSTA2 control register bit									
						FERR2									RCSTA2 control register bit
						RC2IF									Peripheral Interrupt register bit
						COM_RECV_BUFFER_EMPTY					Enumerator										
						COM_RECV_SRC_ADDR						Enumerator
						Com2RecvObject.Index					BYTE
						COM_RECV_DATA_ID						Enumerator
						COM_RECV_DATA_BYTES						Enumerator
						COM_CHECK_CRC16							Enumerator
						Com2RecvObject.State					Enumerator
						COM_INVALID_MESSAGE						Enumerator
						Status.Flags.Modem_B_Err_Toggle_Bit		Bit
						COM_VALID_MESSAGE						Enumerator
					
	Local			:	Crc16_Return_Value						UINT16
						uchData									BYTE			

Output Variables		Name									Type
	Global			:	TXEN2 									TXSTA2 control register bit	
						CREN2									RCSTA2 control register bit		
						RCREG2									USART2 receiver buffer register
						Com2RecvObject.Timeout_ms				UINT16
						Com2RecvObject.Msg_Buffer				Array of BYTES
						Com2RecvObject.Index 					BYTE
						Com2RecvObject.State 					Enumerator
						Status.Flags.Modem_B_Err_Toggle_Bit		Bit
						
			
	Local			:	None
Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
								MODEM_B_CD							Input					
						|---------------------------------|--------------------|------------------------|

Macro definitions used:		Macro							Value
							SET_LOW							0
							SET_HIGH						1
							COM_INVALID_MESSAGE_TIMEOUT		18
							COMM_MESSAGE_LENGTH				14
							TIMEOUT_EVENT					0
References			:

Derived Requirements:

Algorithm			:1.Check for overrun error, if it is there clear the error
					 2.Check for Framing error, if it is there clear the error
					 3.Until reciever buffer become empty load the recieved data 
						to reciever message buffer
					 4.Check for communiction CRC, if it invalid set the recieve 
						message state to INVALID_MESSAGE
					 5.Check for recieve message state EMPTY,if message is not 
						in expected format/incomplete, probably it is out of SYNC
					 6.Check for invalid message and check for Carrier Detection, 
						if it is not generated release the buffer


************************************************************************/

void Receive_COM2_Message(void)
{
    UINT16 Crc16_Return_Value = 0;
    BYTE uchData = 0;
    BYTE uchDecipher_State;


    if (MODEM_B_CD == 1)
    {
        Clear_COM2_Errors();
        //U2MODEbits.UARTEN = 1;
    }

    if(U2STAbits.URXDA == 1)
    {
        uchData = (BYTE)U2RXREG;
        Com2RecvObject.Timeout_ms = COM_INVALID_MESSAGE_TIMEOUT;
        switch (Com2RecvObject.State)
        {
            case COM_RECV_BUFFER_EMPTY:
                Com2RecvObject.Msg_Buffer[0] = uchData;
                Com2RecvObject.Index = 1;
                Com2RecvObject.State = (BYTE)COM_RECV_SRC_ADDR;
                break;
            case COM_RECV_SRC_ADDR:
                Com2RecvObject.Msg_Buffer[Com2RecvObject.Index] = uchData;
                Com2RecvObject.Index = Com2RecvObject.Index + 1;
                Com2RecvObject.State = (BYTE)COM_RECV_DATA_ID;
                break;
            case COM_RECV_DATA_ID:
                Com2RecvObject.Msg_Buffer[Com2RecvObject.Index] = uchData;
                Com2RecvObject.Index = Com2RecvObject.Index + 1;
                Com2RecvObject.State = (BYTE)COM_RECV_DATA_BYTES;
                break;
            case COM_RECV_DATA_BYTES:
                Com2RecvObject.Msg_Buffer[Com2RecvObject.Index] = uchData;
                Com2RecvObject.Index = Com2RecvObject.Index + 1;
                if (Com2RecvObject.Index >= COMM_MESSAGE_LENGTH)
                {
                    Com2RecvObject.State = (BYTE)COM_DECRYPT_DATA_BYTES;
                    Start_DS_Data_Decryption();
                }
                break;
            default:
                break;
        }
    }

    if (Com2RecvObject.State == (BYTE)COM_DECRYPT_DATA_BYTES)
    {
        Update_DS_Data_Decode();
        uchDecipher_State = Get_DS_Decryption_State();

        if(uchDecipher_State == (BYTE)DECRYPTION_COMPLETED)
        {
         Set_DS_Decryption_Idle();
         Com2RecvObject.State = (BYTE)COM_CHECK_CRC16;
        }
    }
    if (Com2RecvObject.State == (BYTE)COM_CHECK_CRC16)
    {
        Crc16_Return_Value = Crc16(COM2_RX,COMM_MESSAGE_LENGTH);
        if (Crc16_Return_Value != 0)
        {
            /* Invalid CRC-16 Checksum */
            Com2RecvObject.State = (BYTE)COM_INVALID_MESSAGE;
            Status.Flags.Modem_B_Err_Toggle_Bit = !(Status.Flags.Modem_B_Err_Toggle_Bit);
            COM_DS_pkt_error_cnt++;
            return;
        }
        Com2RecvObject.State = (BYTE)COM_VALID_MESSAGE;
        //COM_DS_pkt_error_cnt = 0;
        return;
    }
    if (Com2RecvObject.State != (BYTE)COM_RECV_BUFFER_EMPTY)
    {
        /* Message is not in expected format/incomplete, probably it is out of SYNC */
        if( Com2RecvObject.Timeout_ms == TIMEOUT_EVENT)
        {
            //COM_DS_pkt_error_cnt++;
            Com2RecvObject.State = (BYTE)COM_RECV_BUFFER_EMPTY;
        }
    }
    if (Com2RecvObject.State == (BYTE)COM_INVALID_MESSAGE)
    {
        /*
         * Message can be invalid for following reasons:
         *  - Reception out of Sync.
         *  - probably it is not addressed to DAC
         *  - Corrupted message
         */
        //COM_DS_pkt_error_cnt++;
        Com2RecvObject.State = (BYTE)COM_RECV_BUFFER_EMPTY;
    }
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Decrement_DS_Sch_msTmr(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Decrement the DS scheduler timer

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5051



Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Start_SF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_SF()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables             Name                        Type
    Global          :   DS_Sch_Info.Timeout_ms          UINT16
                        DS_Sch_Info.State               Enumerator
                        COMM_SCHEDULER_IDLE             Enumerator
                        Com2RecvObject.Timeout_ms       UINT16

    Local           :   None

Output Variables            Name                        Type
    Global          :   DS_Sch_Info.Timeout_ms          UINT16
                        DS_Sch_Info.ElapsedTime         UINT16
                        Com2RecvObject.Timeout_ms       UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

Algorithm           :1.Decreament the scheduler timer value whenever the 1ms timer overflows.
                     2.If the scheduler is in idle state make the elapsed time as zero otherwise
                        increament the elapsed time value by 1.

************************************************************************/
void Decrement_DS_Sch_msTmr(void)
{
    DS_LED_Error_Service_1mS();
    DS_Packet_error_timmer();

    if (DS_Sch_Info.Timeout_ms > 0)
    {
        DS_Sch_Info.Timeout_ms = DS_Sch_Info.Timeout_ms - 1;
    }
    if (DS_Sch_Info.State == COMM_SCHEDULER_IDLE)
    {
        DS_Sch_Info.ElapsedTime = 0;
    }
    else
    {
        DS_Sch_Info.ElapsedTime = DS_Sch_Info.ElapsedTime + 1;
    }
    if (Com2RecvObject.Timeout_ms > 0)
    {
        Com2RecvObject.Timeout_ms = Com2RecvObject.Timeout_ms - 1;
    }
    if(Comm_Modem_B.Timeout_ms > 0)
    {
     Comm_Modem_B.Timeout_ms = Comm_Modem_B.Timeout_ms - 1;
    }
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Decrement_Comm_DS_CountDown(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :For every communication cycle decrement the communication
                     retries by 1

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5052



Interfaces
    Calls           :   Nil

    Called by       :   COMM_DS.c   -   Update_DS_Sch_State()

Input Variables         Name                                Type
    Global          :   DIP_Switch_Info.Flags.Is_CPU1       Bit
                        Comm_B_CountDown.DS1_to_LU1         BYTE
                        Comm_B_CountDown.DS2_to_LU1         BYTE
                        Comm_B_CountDown.DS1_to_LU2         BYTE
                        Comm_B_CountDown.DS2_to_LU2         BYTE

    Local           :   None

Output Variables        Name                                Type
    Global          :   Comm_B_CountDown.DS1_to_LU1         BYTE
                        Comm_B_CountDown.DS2_to_LU1         BYTE
                        Comm_B_CountDown.DS1_to_LU2         BYTE
                        Comm_B_CountDown.DS2_to_LU2         BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            TRUE                            1

References          :

Derived Requirements:

Algorithm           :1.Decreament the communication B countdown value of LU1 if the value is greater than zero and the CPU is 1
                     2.Decreament the communication B countdown value of LU2 if the value is greater than zero and the CPU is 2
************************************************************************/
void Decrement_Comm_DS_CountDown(void)
{

    if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
    {
        if (Comm_B_CountDown.DS1_to_LU1 > 0)
        {
            Comm_B_CountDown.DS1_to_LU1 = Comm_B_CountDown.DS1_to_LU1 - 1;
        }
        if (Comm_B_CountDown.DS2_to_LU1 > 0)
        {
            Comm_B_CountDown.DS2_to_LU1 = Comm_B_CountDown.DS2_to_LU1 - 1;
        }
    }
    else
    {
        if (Comm_B_CountDown.DS1_to_LU2 > 0)
        {
            Comm_B_CountDown.DS1_to_LU2 = Comm_B_CountDown.DS1_to_LU2 - 1;
        }
        if (Comm_B_CountDown.DS2_to_LU2 > 0)
        {
            Comm_B_CountDown.DS2_to_LU2 = Comm_B_CountDown.DS2_to_LU2 - 1;
        }
    }

}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Build_DS_Message(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :In reset mode calls Build_DS_Reset_Info_Message(),
                    normal mode calls Build_DS_Axle_Count_Message(), checks
                    for overrun and framing error.

Allocated Requirements	: 	(SSDAC_SWRS_0261), (SSDAC_SWRS_0262),(SSDAC_SWRS_0549),(SSDAC_SWRS_0550)
					  	  	(SSDAC_SWRS_0554),(SSDAC_SWRS_0555),(SSDAC_SWRS_0254), (SSDAC_SWRS_0255)
							(SSDAC_SWRS_0358), (SSDAC_SWRS_0648), (SSDAC_SWRS_0748)
							(SSDAC_SWRS_0649), (SSDAC_SWRS_0749),(SSDAC_SWRS_0346), (SSDAC_SWRS_0347).

Design Requirements		:	SSDAC_DR_5053

Interfaces
    Calls           :   COMM_DS.c   -   Build_DS_Reset_Info_Message()
                        COMM_DS.c   -   Build_DS_Axle_Count_Message()

    Called by       :   COMM_DS.c   -   Update_DS_Sch_State()

Input Variables         Name                            Type
    Global          :   Status.Flags.Preparatory_Reset  Bit
                        RCREG2                          USART2 receiver buffer register
                        OERR2                                   RCSTA2 control register bit
                        FERR2                                   RCSTA2 control register bit

    Local           :   uchData                         BYTE

Output Variables        Name                            Type
    Global          :   TXEN2                                   TXSTA2 control register bit
                        CREN2                                   RCSTA2 control register bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                        PREPARATORY_RESET_PENDING           0
                        SET_LOW                             0
                        SET_HIGH                            1
References          :

Derived Requirements:

Algorithm           :1.Build the DS reset message if the system is in reset mode
                     2.Build the axle count message if the system is in pilot mode.
                     3.Disable and enable the transmission and reception once the overrun or framing error occurs.
************************************************************************/
void Build_DS_Message(void)
{
    BYTE uchData =0;


    if (Status.Flags.Preparatory_Reset_DS == PREPARATORY_RESET_PENDING)
    {
        Build_DS_Reset_Info_Message();
    }
    else
    {
        Build_DS_Axle_Count_Message();
    }
    if (U2STAbits.OERR)
    {
        U2STAbits.OERR = 0;
        /* Overrun Error! Clear the error */
        //TXEN2 = SET_LOW; CREN2 = SET_LOW; TXEN2 = SET_HIGH; CREN2 = SET_HIGH;
    }
    if (U2STAbits.FERR)
    {
        /* Framing Error! Clear the error */
        uchData = (BYTE)U2RXREG;
        //TXEN2 = SET_LOW;    TXEN2 = SET_HIGH;
        U2STAbits.FERR = 0;
    }
    if(U2STAbits.PERR)
    {
        U2STAbits.PERR = 0;
    }
    if(IFS4bits.U2ERIF == 1)
    {
        IFS4bits.U2ERIF = 0;
    }
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Build_DS_Reset_Info_Message(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Builds the reset transmit message buffer to transmit
                     to other unit CPUs

Allocated Requirements	:	(SSDAC_SWRS_0261), (SSDAC_SWRS_0262), (SSDAC_SWRS_0254) 	
						  	(SSDAC_SWRS_0549), (SSDAC_SWRS_0554), (SSDAC_SWRS_0358)
						 	(SSDAC_SWRS_0518), (SSDAC_SWRS_0653), (SSDAC_SWRS_0753)	
							(SSDAC_SWRS_0562), (SSDAC_SWRS_0654), (SSDAC_SWRS_0754)
							(SSDAC_SWRS_0649), (SSDAC_SWRS_0749), (SSDAC_SWRS_0346)

Design Requirements		: SSDAC_DR_5054

Interfaces
    Calls           :   CRC16.c  -  Crc16()
                        RESTOREc -  Get_Calculated_Checksum()

    Called by       :   COMM_DS.c   -   Build_DS_Message()

Input Variables         Name                                                    Type
    Global          :   DAC_UNIT_TYPE_D3_C                                      Enumerator
                        DIP_Switch_Info.DAC_Unit_Type                           DAC_Unit_Type
                        DIP_Switch_Info.Address                                 BYTE
                        DIP_Switch_Info.Flags.Is_CPU1                           Bit
                        DS_Telegram_No                                          BYTE
                        Status.Flags.Local_Power_Status                         Bit
                        Status.Flags.Local_Reset_Done                           Bit
                        Status.Flags.DS1_to_LU1_Link                            Bit
                        Status.Flags.DS2_to_LU1_Link                            Bit
                        Status.Flags.DS1_to_LU2_Link                            Bit
                        Status.Flags.DS2_to_LU2_Link                            Bit
                        Status.Flags.System_Status                              Bit
                        Status.Flags.DS_System_Status                           Bit
                        Status.Flags.US_Track_Status                            Bit
                        Status.Flags.DS_Track_Status                            Bit
                        Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok        Bit

    Local           :   Flags                                                   bitadrb_t
                        CheckSum                                                wordtype_t
                        Flags.Byte                                              BYTE
                        CheckSum.Byte.Lo                                        BYTE
                        CheckSum.Byte.Hi                                        BYTE
                        Calculated_Checksum                                     longtype_t
                        Calculated_Checksum.DWord.HiWord.Byte.Hi                BYTE
                        Calculated_Checksum.DWord.HiWord.Byte.Lo                BYTE
                        Calculated_Checksum.DWord.LoWord.Byte.Hi                BYTE
                        Calculated_Checksum.DWord.LoWord.Byte.Lo                BYTE

Output Variables        Name                                Type
    Global          :   Com2XmitObject.Msg_Buffer           Array of BYTEs
                        Com2XmitObject.Msg_Length           BYTE
                        Com2XmitObject.Index                BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            READ_RESET_INFO                 0xAA
                            SET_HIGH                        1
                            COM_DEST_ADDR_FIELD_OFFSET      0
                            COM_SRC_ADDR_FIELD_OFFSET       1
                            COM_DATA_ID_FIELD_OFFSET        2
                            COM_DAC_CONFIG_OFFSET           3
                            COM_FLAGS1_FIELD_OFFSET         4
                            COM_FLAGS2_FIELD_OFFSET         5
                            COM_FWD_AXLE_COUNT_LO_OFFSET    6
                            COM_FWD_AXLE_COUNT_HI_OFFSET    7
                            COM_REV_AXLE_COUNT_LO_OFFSET    8
                            COM_REV_AXLE_COUNT_HI_OFFSET    9
References          :

Derived Requirements:

Algorithm           :1.Load the destination CPU address, sending CPU address,message id,unit type,CPU id,
                        Telegram number,reset information,system status flags,calculated checksum,16bit CRC
                        into the Com2 Transmit buffer inorder to send
************************************************************************/
void Build_DS_Reset_Info_Message(void)
{
    bitadrb_t Flags;
    wordtype_t CheckSum;
    longtype_t Calculated_Checksum;

    Flags.Byte = (BYTE) 0;
    /* Byte 1 Destination Address */
    if( DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
    {
    /* In 4 D Configuration  D4-D unit will communicate with D4-A by DS Communication. In this Case
       DS comm addrss is lower than Local Address */
        Com2XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address - 6);
    }
    else if( DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_EF)
    {
    /* In 3 D Configuration  D3-C unit will communicate with D3-A by DS Communication. In this Case
       DS comm addrss is lower than Local Address */
        Com2XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address - 4);
    }
    else
    {
        Com2XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address + 2);
    }
#if 0
    
#endif
    /* Byte 2 Source Address */
    Com2XmitObject.Msg_Buffer[COM_SRC_ADDR_FIELD_OFFSET] = DIP_Switch_Info.Address;
    /* Byte 3 Command */
    Com2XmitObject.Msg_Buffer[COM_DATA_ID_FIELD_OFFSET] = READ_RESET_INFO;
    /* Byte 4 Configuration details */
    Flags.Byte = 0;
    Flags.Byte = (BYTE)DIP_Switch_Info.DAC_Unit_Type;                             /* DAC Type */
    Flags.Bit.b5 = SET_HIGH;                                            /* Message over COM2 */
    Flags.Bit.b4 = DIP_Switch_Info.Flags.Is_DAC_CPU1;                       /* CPU1 or CPU2 */

    Com2XmitObject.Msg_Buffer[COM_DAC_CONFIG_OFFSET] = Flags.Byte;
    Flags.Byte = 0;
    Flags.Bit.b0 = Status.Flags.Local_Power_Status;
//    if(Checksum_Info.CF.Flags.DS_Checksum_Comparison_Ok == TRUE)
//       {
          Flags.Bit.b1 = Status.Flags.Local_Reset_Done2;
//     }
    Flags.Bit.b2 = Status.Flags.DS1_to_LU1_Link;
    Flags.Bit.b3 = Status.Flags.DS2_to_LU1_Link;
    Flags.Bit.b4 = Status.Flags.DS1_to_LU2_Link;
    Flags.Bit.b5 = Status.Flags.DS2_to_LU2_Link;
    Flags.Bit.b6 = Status.Flags.Local_Reset_Done2;
    Com2XmitObject.Msg_Buffer[COM_FLAGS1_FIELD_OFFSET] = Flags.Byte;
    Flags.Byte = 0;
    Flags.Bit.b0 = Status.Flags.System_Status;
    Flags.Bit.b1 = Status.Flags.DS_System_Status;
    Flags.Bit.b2 = Status.Flags.US_Track_Status;
    Flags.Bit.b3 = Status.Flags.DS_Track_Status;
    if(DIP_Switch_Info.Flags.ATC_Enabled == TRUE)
    {
      Flags.Bit.b4 = SET_HIGH;
    }
    Com2XmitObject.Msg_Buffer[COM_FLAGS2_FIELD_OFFSET] = Flags.Byte;
    Calculated_Checksum.LWord = Get_Calculated_Checksum();
    Com2XmitObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET] = Calculated_Checksum.DWord.HiWord.Byte.Hi ;
    Com2XmitObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET] = Calculated_Checksum.DWord.HiWord.Byte.Lo ;
    Com2XmitObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET] = Calculated_Checksum.DWord.LoWord.Byte.Hi ;
    Com2XmitObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET] = Calculated_Checksum.DWord.LoWord.Byte.Lo ;
    Com2XmitObject.Msg_Buffer[14] = 0;
    Com2XmitObject.Msg_Buffer[15] = 0;
    Com2XmitObject.Msg_Buffer[16] = 0;
    Com2XmitObject.Msg_Buffer[17] = 0;
    CheckSum.Word = Crc16(COM2_TX, 18);
    Com2XmitObject.Msg_Buffer[18] = CheckSum.Byte.Lo;
    Com2XmitObject.Msg_Buffer[19] = CheckSum.Byte.Hi;
    Com2XmitObject.Msg_Length = (BYTE) COMM_MESSAGE_LENGTH;
    Com2XmitObject.Index = 0;
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Build_DS_Axle_Count_Message(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Build the axle message buffer for transmission to
                     other unit CPUs

Allocated Requirements	:	(SSDAC_SWRS_0550), (SSDAC_SWRS_0554), (SSDAC_SWRS_0261), (SSDAC_SWRS_0262),
							(SSDAC_SWRS_0255), (SSDAC_SWRS_0648), (SSDAC_SWRS_0748)	
							(SSDAC_SWRS_0649), (SSDAC_SWRS_0749), (SSDAC_SWRS_0347)

Design Requirements		: SSDAC_DR_5055

Interfaces
    Calls           :   AXLE_MON.c  -   Get_DS_AxleDirection()
                        RLYB_MGR.c  -   Get_Relay_B_ATC_State()
                        AXLE_MON.c  -   Get_DS_Fwd_AxleCount()
                        AXLE_MON.c  -   Get_DS_Rev_AxleCount()
                        CRC16.c     -   Crc16()

    Called by       :   COMM_DS.c   -   Build_DS_Message()

Input Variables         Name                                Type
    Global          :   DAC_UNIT_TYPE_D3_C                  Enumerator
                        DIP_Switch_Info.DAC_Unit_Type           DAC_Unit_Type
                        DIP_Switch_Info.Address             BYTE
                        DIP_Switch_Info.Flags.Is_CPU1       Bit
                        DS_Telegram_No                      BYTE
                        Status.Flags.Local_Power_Status     Bit
                        Status.Flags.Local_Reset_Done       Bit
                        Status.Flags.DS1_to_LU1_Link        Bit
                        Status.Flags.DS2_to_LU1_Link        Bit
                        Status.Flags.DS1_to_LU2_Link        Bit
                        Status.Flags.DS2_to_LU2_Link        Bit
                        Status.Flags.System_Status          Bit
                        Status.Flags.DS_System_Status       Bit
                        Status.Flags.US_Track_Status        Bit
                        Status.Flags.DS_Track_Status        Bit
                        DIP_Switch_Info.Flags.ATC_Enabled   Bit
                        Status.Flags.Preparatory_Reset_DS   Bit

    Local           :   Flags                               bitadrb_t
                        Buffer                              bitadrb_t
                        Fwd_AxleCount                       wordtype_t
                        Rev_AxleCount                       wordtype_t
                        AxleCount                           wordtype_t
                        Fwd_AxleCount.Byte.Lo               BYTE
                        Fwd_AxleCount.Byte.Hi               BYTE
                        Rev_AxleCount.Byte.Lo               BYTE
                        Rev_AxleCount.Byte.Hi               BYTE
                        CheckSum                            wordtype_t
                        Flags.Byte                          BYTE
                        CheckSum.Byte.Lo                    BYTE
                        CheckSum.Byte.Hi                    BYTE


Output Variables        Name                                Type
    Global          :   Com2XmitObject.Msg_Buffer           Array of BYTEs
                        Com2XmitObject.Msg_Length           BYTE
                        Com2XmitObject.Index                BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            READ_AXLE_COUNT                 0x55
                            TRUE                            1
                            SET_HIGH                        1
                            COM_DEST_ADDR_FIELD_OFFSET      0
                            COM_SRC_ADDR_FIELD_OFFSET       1
                            COM_DATA_ID_FIELD_OFFSET        2
                            COM_DAC_CONFIG_OFFSET           3
                            COM_FLAGS1_FIELD_OFFSET         4
                            COM_FLAGS2_FIELD_OFFSET         5
                            COM_FWD_AXLE_COUNT_LO_OFFSET    6
                            COM_FWD_AXLE_COUNT_HI_OFFSET    7
                            COM_REV_AXLE_COUNT_LO_OFFSET    8
                            COM_REV_AXLE_COUNT_HI_OFFSET    9


References          :

Derived Requirements:

Algorithm           :1.Load the destination CPU address, sending CPU address,message id,unit type,CPU id,
                        Telegram number,reset information,system status flags,axle count messages,16bit CRC
                        into the Com2 Transmit buffer inorder to send
************************************************************************/
void Build_DS_Axle_Count_Message(void)
{
    bitadrb_t  Flags,Buffer;
    wordtype_t Fwd_AxleCount;
    wordtype_t Rev_AxleCount;
    wordtype_t AxleCount;
    wordtype_t CheckSum;

    Flags.Byte = (BYTE) 0;
    /* Byte 1 Destination Address */
    if( DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_EF)
    {
    /* In 3 D Configuration  D3-C unit will communicate with D3-A by DS Communication. In this Case
       DS comm addrss is lower than Local Address */
        Com2XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address - 4);
    }
	
    else if( DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
    {
    /* In 4 D Configuration  D4-D unit will communicate with D4-A by DS Communication. In this Case
       DS comm addrss is lower than Local Address */
        Com2XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address - 6);
    }
	
    else
    {
        Com2XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address + 2);
    }
    /* Byte 2 Source Address */
    Com2XmitObject.Msg_Buffer[COM_SRC_ADDR_FIELD_OFFSET] = DIP_Switch_Info.Address;
    /* Byte 3 Command */
    Com2XmitObject.Msg_Buffer[COM_DATA_ID_FIELD_OFFSET] = READ_AXLE_COUNT;
    /* Byte 4 Configuration details */
    Flags.Byte = 0;
    Flags.Byte = (BYTE)DIP_Switch_Info.DAC_Unit_Type;             /* DAC Type */
    Flags.Bit.b5 = SET_HIGH;                            /* Message over COM2 */
    Flags.Bit.b4 = DIP_Switch_Info.Flags.Is_DAC_CPU1;       /* CPU1 or CPU2 */

    Com2XmitObject.Msg_Buffer[COM_DAC_CONFIG_OFFSET] = Flags.Byte;
    Flags.Byte = 0;
    Flags.Bit.b0 = Status.Flags.Local_Power_Status;
    Flags.Bit.b1 = Status.Flags.Local_Reset_Done2;
    Flags.Bit.b2 = Status.Flags.DS1_to_LU1_Link;
    Flags.Bit.b3 = Status.Flags.DS2_to_LU1_Link;
    Flags.Bit.b4 = Status.Flags.DS1_to_LU2_Link;
    Flags.Bit.b5 = Status.Flags.DS2_to_LU2_Link;
    Buffer.Byte  = Get_DS_AxleDirection();
    Flags.Bit.b6 = Buffer.Bit.b0;
    Flags.Bit.b7 = Buffer.Bit.b1;
    Com2XmitObject.Msg_Buffer[COM_FLAGS1_FIELD_OFFSET] = Flags.Byte;
    Flags.Byte = 0;
    Flags.Bit.b0 = Status.Flags.System_Status;
    Flags.Bit.b1 = Status.Flags.DS_System_Status;
    Flags.Bit.b2 = Status.Flags.US_Track_Status;
    Flags.Bit.b3 = Status.Flags.DS_Track_Status;
    if(DIP_Switch_Info.Flags.ATC_Enabled == TRUE)
    {
        if((DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A)||
           (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B)||
            (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C))
        {
         Flags.Bit.b4 = (BYTE) Get_Relay_D3_ATC_State();
        }
		#if 1
        else if((DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A)||
           (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B)||
            (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C) ||
            (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D))
        {
         Flags.Bit.b4 = (BYTE) Get_Relay_D4_ATC_State();
        }
		#endif
        else
        {
          Flags.Bit.b4 = (BYTE) Get_Relay_B_ATC_State();
        }
    }
    Flags.Bit.b5 = Status.Flags.Preparatory_Reset_DS;
    Com2XmitObject.Msg_Buffer[COM_FLAGS2_FIELD_OFFSET] = Flags.Byte;
    Fwd_AxleCount.Word = Get_DS_Fwd_AxleCount();                /* from axlemon.c */
    Rev_AxleCount.Word = Get_DS_Rev_AxleCount();                /* from axlemon.c */
    Com2XmitObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
    Com2XmitObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
    Com2XmitObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
    Com2XmitObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;
    AxleCount.Word = Get_DS_AxleCount();                        /* from axlemon.c */
    Com2XmitObject.Msg_Buffer[COM_AXLE_COUNT_LO_OFFSET] = AxleCount.Byte.Lo;
    Com2XmitObject.Msg_Buffer[COM_AXLE_COUNT_HI_OFFSET] = AxleCount.Byte.Hi;
    Com2XmitObject.Msg_Buffer[12] = 0;
    Com2XmitObject.Msg_Buffer[13] = 0;
    Com2XmitObject.Msg_Buffer[14] = 0;
    Com2XmitObject.Msg_Buffer[15] = 0;
    Com2XmitObject.Msg_Buffer[16] = 0;
    Com2XmitObject.Msg_Buffer[17] = 0;
    Com2XmitObject.Msg_Buffer[18] = 0;
    Com2XmitObject.Msg_Buffer[19] = 0;
    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_DE:
        case DAC_UNIT_TYPE_SF:
        case DAC_UNIT_TYPE_EF:
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_D3_A:
        case DAC_UNIT_TYPE_D3_B:
        case DAC_UNIT_TYPE_D3_C:
        case DAC_UNIT_TYPE_3D_SF: 
        case DAC_UNIT_TYPE_3D_EF:
        case DAC_UNIT_TYPE_LCWS:
        case DAC_UNIT_TYPE_LCWS_DL:
            break;
        case DAC_UNIT_TYPE_D4_A: 
            //DS -dst address is B, so send D's data
            //Always send CPU1 data
            Fwd_AxleCount.Word = Relay_D4_Info.D1_IN_Count;
            Rev_AxleCount.Word = Relay_D4_Info.D1_OUT_Count;        
            Com2XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
            Com2XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
            Com2XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
            Com2XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;            
            break;
        case DAC_UNIT_TYPE_D4_B: 
            //DS -dst address is C, so send A's data
            //Always send CPU1 data
            Fwd_AxleCount.Word = Relay_D4_Info.A1_IN_Count;
            Rev_AxleCount.Word = Relay_D4_Info.A1_OUT_Count;        
            Com2XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
            Com2XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
            Com2XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
            Com2XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;            
            break;
        case DAC_UNIT_TYPE_D4_C:
            //DS -dst address is D, so send B's data
            //Always send CPU1 data
            Fwd_AxleCount.Word = Relay_D4_Info.B1_IN_Count;
            Rev_AxleCount.Word = Relay_D4_Info.B1_OUT_Count;        
            Com2XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
            Com2XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
            Com2XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
            Com2XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;   
            break;
        case DAC_UNIT_TYPE_D4_D:
            //DS -dst address is A, so send C's data
            //Always send CPU1 data
            Fwd_AxleCount.Word = Relay_D4_Info.C1_IN_Count;
            Rev_AxleCount.Word = Relay_D4_Info.C1_OUT_Count;        
            Com2XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
            Com2XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
            Com2XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
            Com2XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;   
            break;
        default:
            break;
    }
    CheckSum.Word = Crc16(COM2_TX, 20);
    Com2XmitObject.Msg_Buffer[20] = CheckSum.Byte.Lo;
    Com2XmitObject.Msg_Buffer[21] = CheckSum.Byte.Hi;
    Com2XmitObject.Msg_Length = (BYTE) COMM_MESSAGE_LENGTH;
    Com2XmitObject.Index = 0;
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Process_DS_Message(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :If command is for Axle count calls Process_DS_Axle_Count_message()
                     else calls Process_DS_Reset_Info_Message()

Allocated Requirements	: 	(SSDAC_SWRS_0552)	, (SSDAC_SWRS_0257), (SSDAC_SWRS_0357)
							(SSDAC_SWRS_0648), (SSDAC_SWRS_0748), (SSDAC_SWRS_0349)

Design Requirements		:	SSDAC_DR_5056

Interfaces
    Calls           :   COMM_DS.C   -   Update_DS_Sch_State()

    Called by       :   COMM_DS.c   -   Process_DS_Reset_Info_Message()
                        COMM_DS.c   -   Process_DS_Axle_Count_Message()

Input Variables         Name                        Type
    Global          :   Com2RecvObject.Msg_Buffer   BYTE

    Local           :   uchCommand                  BYTE

Output Variables        Name                        Type
    Global          :   None

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            COM_DATA_ID_FIELD_OFFSET        2
                            READ_RESET_INFO                 0xAA
                            READ_AXLE_COUNT                 0x55

References          :

Derived Requirements:

Algorithm           :1.Check the message ID to find out the type of message whether reset or axle count message.
                     2.Process the received message as per the message ID.
************************************************************************/
void Process_DS_Message(void)
{
    BYTE uchCommand;

    uchCommand = Com2RecvObject.Msg_Buffer[COM_DATA_ID_FIELD_OFFSET];

    if (uchCommand == READ_RESET_INFO)
    {
        Process_DS_Reset_Info_Message();
    }
    if (uchCommand == READ_AXLE_COUNT)
    {
        Process_DS_Axle_Count_Message();
    }
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Process_DS_Reset_Info_Message(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Process the DS reset massage

Allocated Requirements	:	(SSDAC_SWRS_0552) , (SSDAC_SWRS_0257), (SSDAC_SWRS_0587), (SSDAC_SWRS_0357)
							(SSDAC_SWRS_0648), (SSDAC_SWRS_0748), (SSDAC_SWRS_0679), (SSDAC_SWRS_0779)
						 	(SSDAC_SWRS_0349), (SSDAC_SWRS_0391),(SSDAC_SWRS_0840),(SSDAC_SWRS_0844),(SSDAC_SWRS_0845)
							(SSDAC_SWRS_0848),(SSDAC_SWRS_0853),(SSDAC_SWRS_0854),(SSDAC_SWRS_0855),(SSDAC_SWRS_0856)
							(SSDAC_SWRS_0857),(SSDAC_SWRS_0858)

Design Requirements		:	SSDAC_DR_5057, SSDAC_DR_5351, SSDAC_DR_5352, SSDAC_DR_5353
						
Interfaces
    Calls           :   RELAYMGR.c  -   Declare_DAC_Defective_DS()
                        ERROR.c     -   Set_Error_Status_Bit()
                        SYS_MON.c   -   Update_DS_Power_Status()
                        SYS_MON.c   -   Update_DS_Track_Status()
                        COMM_DS.c   -   Synchronise_DS_Sch()
                        COMM_DS.c   -   Reset_DS_CountDown()
                        RESET.c     -   Post_DS1_has_been_Reset()
                        RESET.c     -   Post_DS2_has_been_Reset()
                        RESTORE.c   -   Compare_DS_Checksum()
                        RESET.c     -   Update_DS_Section_Remote_Reset()

    Called by       :   COMM_DS.c   -   Process_DS_Message()

Input Variables         Name                                                Type
    Global          :   DIP_Switch_Info.Address                             BYTE
                        DIP_Switch_Info.Peer_Address                        BYTE
                        DAC_UNIT_TYPE_CF                                    Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        SYSTEM_ERROR_MODE                                   Enumerator
                        WAITING_FOR_RESET_MODE                              Enumerator


    Local           :   SrcMsgAdr                       bitadrb_t
                        DestMsgAdr                      bitadrb_t
                        DAC_Config                      bitadrb_t
                        Flags1                          bitadrb_t
                        Flags2                          bitadrb_t
                        DAC_Config.Bit.b3               Bit
                        DAC_Config.Bit.b4               Bit
                        Flags1.Bit.b1                   Bit

Output Variables        Name                                                    Type
    Global          :   Checksum_Info.CF.DS1_Checksum.DWord.HiWord.Byte.Hi      BYTE
                        Checksum_Info.CF.DS1_Checksum.DWord.HiWord.Byte.Lo      BYTE
                        Checksum_Info.CF.DS1_Checksum.DWord.LoWord.Byte.Hi      BYTE
                        Checksum_Info.CF.DS1_Checksum.DWord.LoWord.Byte.Lo      BYTE
                        Checksum_Info.CF.DS1_Checksum.DWord.HiWord.Byte.Hi      BYTE
                        Checksum_Info.CF.DS1_Checksum.DWord.HiWord.Byte.Lo      BYTE
                        Checksum_Info.CF.DS1_Checksum.DWord.LoWord.Byte.Hi      BYTE
                        Checksum_Info.CF.DS1_Checksum.DWord.LoWord.Byte.Lo      BYTE
                        DS_Section_Mode.Remote_Unit                             BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            INOPERATIVE_CONFIGURATION_ERROR_NUM        58
                            COM_DAC_CONFIG_OFFSET           3
                            COM_SRC_ADDR_FIELD_OFFSET       1
                            COM_DEST_ADDR_FIELD_OFFSET      0
                            COM_FLAGS1_FIELD_OFFSET         4
                            COM_FLAGS2_FIELD_OFFSET         5
                            SET_LOW                         0
                            SM_HAS_RESETTED_SYSTEM          1
References          :

Derived Requirements:

Algorithm           :1.Get the configuration details, source address,destination
                        address, FLAG1 and FLAG2
                     2.Check the destination address with the CPU address and Peer
                        CPU address, if both are not equal message does belong to
                        our network declare DAC defective and set Invalid
                        configuration error ID
                     3.Check for source address, if it is zero, message does belong
                        to our network declare DAC defective and set Invalid
                        configuration error ID
                     4.Check for both source and destination address if both are
                        equal declare DAC defective and set Invalid
                       configuration error ID
                     5.Check whether message is coming from COM1 port of remote unit,
                        if not declare DAC defective and set Invalid
                       configuration error ID
                     6.Update the DS power status, Track status,Sychronise the
                        DS shedular, update the reset in CPU1 or CPU2
                     7.If the remote unit status is not equal to normal and not in 3D1Sconfiguration  declare the DS remote unit
                       status as System in Error mode
                     8.If the remote unit got resetted and not in 3D1Sconfiguration declare the DS remote unit
                       status as System in reset applied mode else waiting for reset mode
                    9.If the message is from CPU1 update the DS1 checksum otherwise update the DS2 checksum
                    10.Compare the updated checksum with calculated own checksum

************************************************************************/
void Process_DS_Reset_Info_Message(void)
{
    bitadrb_t SrcMsgAdr,DestMsgAdr;
    bitadrb_t DAC_Config, Flags1, Flags2;

//    SPI_Failure.fail_bits.DS_Failure = 0;
    
    DAC_Config.Byte = Com2RecvObject.Msg_Buffer[COM_DAC_CONFIG_OFFSET];
    SrcMsgAdr.Byte  = Com2RecvObject.Msg_Buffer[COM_SRC_ADDR_FIELD_OFFSET];
    DestMsgAdr.Byte = Com2RecvObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET];
    Flags1.Byte     = Com2RecvObject.Msg_Buffer[COM_FLAGS1_FIELD_OFFSET];
    Flags2.Byte     = Com2RecvObject.Msg_Buffer[COM_FLAGS2_FIELD_OFFSET];

    if ((DIP_Switch_Info.Address != DestMsgAdr.Byte)&&
        (DIP_Switch_Info.Peer_Address != DestMsgAdr.Byte))
    {
        SPI_Failure.fail_bits.DS_Failure = 1;
        Declare_DAC_Defective_DS();
        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
        return;         /* Message does not belong to our network */
    }
    if (SrcMsgAdr.Byte == 0)
    {
        SPI_Failure.fail_bits.DS_Failure = 1;
        Declare_DAC_Defective_DS();
        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
        return;         /* Message does not belong to our network */
    }
    if (DestMsgAdr.Byte == SrcMsgAdr.Byte)
    {
        SPI_Failure.fail_bits.DS_Failure = 1;
        Declare_DAC_Defective_DS();
        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
        return;         /* Source address and Destination addresses are same */
    }
    if (DAC_Config.Bit.b5 != SET_LOW)
    {
        SPI_Failure.fail_bits.DS_Failure = 1;
        Declare_DAC_Defective_DS();
        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
        return;         /* Message should come from COM1 port of remote unit */
    }
    if(DIP_Switch_Info.Flags.ATC_Enabled == TRUE)
    {
        if(Flags2.Bit.b4 == FALSE)
        {
            SPI_Failure.fail_bits.DS_Failure = 1;
            Declare_DAC_Defective_DS();
            Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
            return;         /* In Remote Unit Auto Clear Bit is not set */
        }
    }
    Update_DS_Power_Status(DAC_Config,Flags1);
    Update_DS_Track_Status(Flags2);
    Synchronise_DS_Sch(SrcMsgAdr);
    Reset_DS_CountDown(SrcMsgAdr,Flags1);

    if(Flags2.Bit.b1 != NORMAL )
       {
        SPI_Failure.fail_bits.DS_Failure = 1;
        DS_Section_Mode.Remote_Unit = SYSTEM_ERROR_MODE;
       }
    else
    {
        SPI_Failure.fail_bits.DS_Failure = 0;
    }
    if(Flags1.Bit.b6 == SM_HAS_RESETTED_SYSTEM )
       {
            Update_DS_Section_Remote_Reset();
       }
    else
       {
           DS_Section_Mode.Remote_Unit = WAITING_FOR_RESET_MODE;
       }
    if (DAC_Config.Bit.b4)
    {
        /* Sender is CPU1 */
        Post_DS1_has_been_Reset((BYTE) Flags1.Bit.b1);
        Checksum_Info.CF.DS1_Checksum.DWord.HiWord.Byte.Hi = Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET];
        Checksum_Info.CF.DS1_Checksum.DWord.HiWord.Byte.Lo = Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET];
        Checksum_Info.CF.DS1_Checksum.DWord.LoWord.Byte.Hi = Com2RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET];
        Checksum_Info.CF.DS1_Checksum.DWord.LoWord.Byte.Lo = Com2RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET];
     }
    else
    {
        /* Sender is CPU2 */
        Post_DS2_has_been_Reset((BYTE) Flags1.Bit.b1);
        Checksum_Info.CF.DS2_Checksum.DWord.HiWord.Byte.Hi = Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET];
        Checksum_Info.CF.DS2_Checksum.DWord.HiWord.Byte.Lo = Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET];
        Checksum_Info.CF.DS2_Checksum.DWord.LoWord.Byte.Hi = Com2RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET];
        Checksum_Info.CF.DS2_Checksum.DWord.LoWord.Byte.Lo = Com2RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET];
     }
 // Compare_DS_Checksum(DAC_Config);
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Process_DS_Axle_Count_Message(void)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Process the DS axle count message


Allocated Requirements	:	(SSDAC_SWRS_0552) , (SSDAC_SWRS_0257), (SSDAC_SWRS_0587), (SSDAC_SWRS_0357)
							(SSDAC_SWRS_0648), (SSDAC_SWRS_0748), (SSDAC_SWRS_0679), (SSDAC_SWRS_0779)
						 	(SSDAC_SWRS_0349), (SSDAC_SWRS_0391)

Design Requirements		:	SSDAC_DR_5057, SSDAC_DR_5351, SSDAC_DR_5352, SSDAC_DR_5353

Interfaces
    Calls           :   COMM_DS.c   -   Synchronise_DS_Sch()
                        COMM_DS.c   -   Reset_DS_CountDown()
                        RLYB_MGR.c  -   Get_Relay_B_State()
                        RESET.c     -   Post_DS1_has_been_Reset()
                        RESET.c     -   Post_DS2_has_been_Reset()
                        RLYB_MGR.c  -   Update_ATC_Remote1_Relay_B_State()
                        RLYB_MGR.c  -   Update_ATC_Remote2_Relay_B_State()
                        SYS_MON.c   -   Update_DS_Power_Status()
                        SYS_MON.c   -   Update_DS_Track_Status()
                        COMM_DS.c   -   Check_DS_Unit_Status()
                        COMM_DS.c   -   Process_DS_AxleCount()
                        COMM_DS.c   -   Process_DS_Direction()
                        RLYA_MGR.c  -   Update_Next_Block_State()
                        RLYD3_MGR.c -   Process_D3_Remote_AxleCount()
                        RLYD3_MGR.c -   Process_D3_Remote_Direction()
                        RESET.c     -   Update_DS_Section_Remote_Reset()
                        RESET.c     -   Update_DS_Section_Remote_Preparatory()

    Called by       :   COMM_DS.c   -   Process_DS_Message()

Input Variables         Name                            Type
    Global          :   Com2RecvObject.Msg_Buffer       Array of BYTES
                        DIP_Switch_Info.Address         BYTE
                        DIP_Switch_Info.Peer_Address    BYTE
                        DAC_RESET_PROGRESS              Enumerator
                        Status.Flags.Preparatory_Reset  Enumerator
                        ATC_WAIT_FOR_REMOTE_CLEAR       Enumerator
                        Status.Flags.DS_System_Status   Bit
                        DIP_Switch_Info.DAC_Unit_Type       DAC_Unit_Type
                        DAC_UNIT_TYPE_SF                Enumerator
                        DAC_UNIT_TYPE_CF                Enumerator
                        DAC_UNIT_TYPE_D3_A              Enumerator
                        DAC_UNIT_TYPE_D3_B              Enumerator
                        DAC_UNIT_TYPE_D3_C              Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        SYSTEM_ERROR_MODE               Enumerator

    Local           :   SrcMsgAdr                       bitadrb_t
                        DestMsgAdr                      bitadrb_t
                        DAC_Config                      bitadrb_t
                        Buffer                          bitadrb_t
                        Flags1                          bitadrb_t
                        Flags2                          bitadrb_t
                        Fwd_CountValue                  wordtype_t
                        Rev_CountValue                  wordtype_t
                        Axle_CountValue                 wordtype_t
                        uchDirection                    BYTE
                        uchState                        BYTE
                        SrcMsgAdr.Byte                  BYTE
                        DestMsgAdr.Byte                 BYTE
                        DAC_Config.Bit.b3               Bit
                        DAC_Config.Bit.b4               Bit
                        Flags1.Bit.b1                   Bit
                        Flags2.Bit.b3                   Bit
                        Flags2.Bit.b4                   Bit
                        Fwd_CountValue.Byte.Lo          BYTE
                        Fwd_CountValue.Byte.Hi          BYTE
                        Rev_CountValue.Byte.Lo          BYTE
                        Rev_CountValue.Byte.Hi          BYTE
                        DAC_Config.Bit.b0               Bit
                        DAC_Config.Bit.b1               Bit
                        DAC_Config.Bit.b2               Bit
                        Buffer.Bit.b0                   Bit
                        Buffer.Bit.b1                   Bit
                        Buffer.Bit.b2                   Bit

Output Variables        Name                        Type
    Global          :   None

    Local           :   None
Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_LOW                         0
                            PREPARATORY_RESET_PENDING       0
                            NORMAL                          1
                            COM_DAC_CONFIG_OFFSET           3
                            COM_SRC_ADDR_FIELD_OFFSET       1
                            COM_DEST_ADDR_FIELD_OFFSET      0
                            COM_FLAGS1_FIELD_OFFSET         4
                            COM_FLAGS2_FIELD_OFFSET         5
                            COM_FWD_AXLE_COUNT_LO_OFFSET    6
                            COM_FWD_AXLE_COUNT_HI_OFFSET    7
                            COM_REV_AXLE_COUNT_LO_OFFSET    8
                            COM_REV_AXLE_COUNT_HI_OFFSET    9
                            COM_AXLE_COUNT_LO_OFFSET        10
                            COM_AXLE_COUNT_HI_OFFSET        11
                            SM_HAS_RESETTED_SYSTEM          1
                            PREPARATORY_RESET_COMPLETED     1
References          :

Derived Requirements:

Algorithm           :1.Get the configuration details, source address,destination
                        address, FLAG1 and FLAG2
                     2.Check the destination address with the CPU address and Peer
                        CPU address, if both are not equal message does belong to our
                        network declare DAC defective and set Invalid configuration
                        error ID
                     3.Check for source address, if it is zero, message does belong
                        to our network declare DAC defective and set Invalid
                        configuration error ID
                     4.Check for both source and destination address if both are
                        equal declare DAC defective and set Invalid configuration
                        error ID
                     5.Check whether message is coming from COM1 port of remote unit,
                        if not declare DAC defective and set Invalid configuration
                        error ID
                     6.Sychronise the DS shedular, update the reset in CPU1 or CPU2
                     7.If the remote unit status is not equal to normal and not in 3D1Sconfiguration
                       declare the DS remote unit status as System in Error mode
                     8.According to the status of remote unit update the status of DS section remote mode
                        inorder to display in the reste box.
                     9.Update the DS power status,if local system failed, So no need
                        to Process the axle counts of remote unit
                     10.According to configauration Process the axle count and direction
                        and update the DS track status, update the next block status

************************************************************************/
void Process_DS_Axle_Count_Message(void)
{
    wordtype_t Fwd_CountValue;
    wordtype_t Rev_CountValue;
    wordtype_t R_Fwd_CountValue;
    wordtype_t R_Rev_CountValue;
    wordtype_t Axle_CountValue;
    bitadrb_t SrcMsgAdr,DestMsgAdr;
    bitadrb_t DAC_Config,Flags1,Flags2,Buffer;
    BYTE uchDirection;
    BYTE uchState = 0;
    BYTE Local_Count;
    BYTE uchTrack;


    DAC_Config.Byte = Com2RecvObject.Msg_Buffer[COM_DAC_CONFIG_OFFSET];
    SrcMsgAdr.Byte  = Com2RecvObject.Msg_Buffer[COM_SRC_ADDR_FIELD_OFFSET];
    DestMsgAdr.Byte = Com2RecvObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET];
    Flags1.Byte     = Com2RecvObject.Msg_Buffer[COM_FLAGS1_FIELD_OFFSET];
    Flags2.Byte     = Com2RecvObject.Msg_Buffer[COM_FLAGS2_FIELD_OFFSET];

    if ((DIP_Switch_Info.Address != DestMsgAdr.Byte) &&
        (DIP_Switch_Info.Peer_Address != DestMsgAdr.Byte))
    {
        return;         /* Message does not belong to our network */
    }
    if (SrcMsgAdr.Byte == 0)
    {
        return;         /* Message does not belong to our network */
    }
    if (DestMsgAdr.Byte == SrcMsgAdr.Byte)
    {
        return;         /* Source address and Destination addresses are same */
    }
    if (DAC_Config.Bit.b5 != SET_LOW)
    {
        return;         /* Message should come from COM1 port of remote unit */
    }

    Synchronise_DS_Sch(SrcMsgAdr);
    Reset_DS_CountDown(SrcMsgAdr,Flags1);
   if(Flags1.Bit.b1 == SM_HAS_RESETTED_SYSTEM )
    {
     if(DIP_Switch_Info.DAC_Unit_Type <= DAC_UNIT_TYPE_CF || DIP_Switch_Info.DAC_Unit_Type >= DAC_UNIT_TYPE_3D_SF)
      {
       Update_DS_Section_Remote_Reset();
      }
    }
    if(DS_Section_Mode.Flags.Remote_Preparatory_State == FALSE)
     {
       if(Flags2.Bit.b5 == PREPARATORY_RESET_COMPLETED)
        {
         if(DIP_Switch_Info.DAC_Unit_Type <= DAC_UNIT_TYPE_CF || DIP_Switch_Info.DAC_Unit_Type >= DAC_UNIT_TYPE_3D_SF)
          {
           Update_DS_Section_Remote_Preparatory();
          }
        }
     }
   if(Flags2.Bit.b1 != NORMAL )
    {
       if(DIP_Switch_Info.DAC_Unit_Type <= DAC_UNIT_TYPE_CF || DIP_Switch_Info.DAC_Unit_Type >= DAC_UNIT_TYPE_3D_SF)
       {
        DS_Section_Mode.Remote_Unit = SYSTEM_ERROR_MODE;
       }
     }
    if((DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A)||
     (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B)||
     (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C))
    {
        uchState = Get_Relay_D3_State();
    }
     else if((DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A)||
           (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B)||
            (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C) ||
            (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D))
        {
         uchState = (BYTE) Get_Relay_D4_State();
        }
    else
    {
        uchState = Get_Relay_B_State();
    }
    if (Status.Flags.Preparatory_Reset_DS == PREPARATORY_RESET_PENDING ||
        uchState == (BYTE)DAC_RESET_PROGRESS)
    {
        if (DAC_Config.Bit.b4)
        {
            /* Sender is CPU1 */
            Post_DS1_has_been_Reset((BYTE) Flags1.Bit.b1);
        }
        else
        {
            /* Sender is CPU2 */
            Post_DS2_has_been_Reset((BYTE) Flags1.Bit.b1);
        }
        return;
    }

    if((DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A)||
     (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B)||
     (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C))
    {
        if(uchState == (BYTE)ATC_WAIT_FOR_REMOTE_CLEAR)
        {
            if (DAC_Config.Bit.b4)
            {
                /* Sender is CPU1 */
                Update_ATC_Remote1_Relay_D3_State((BYTE) Flags2.Bit.b4);
            }
            else
            {
                /* Sender is CPU2 */
                Update_ATC_Remote2_Relay_D3_State((BYTE) Flags2.Bit.b4);
            }
        }
    }
    else if((DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A)||
     (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B)||
     (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C)||
      (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D))
    {
        if(uchState == (BYTE)ATC_WAIT_FOR_REMOTE_CLEAR)
        {
            if (DAC_Config.Bit.b4)
            {
                /* Sender is CPU1 */
                Update_ATC_Remote1_Relay_D4_State((BYTE) Flags2.Bit.b4);
            }
            else
            {
                /* Sender is CPU2 */
                Update_ATC_Remote2_Relay_D4_State((BYTE) Flags2.Bit.b4);
            }
        }
    }
    else
    {
     if(uchState == (BYTE)ATC_WAIT_FOR_REMOTE_CLEAR)
        {
            if (DAC_Config.Bit.b4)
            {
                /* Sender is CPU1 */
                Update_ATC_Remote1_Relay_B_State((BYTE) Flags2.Bit.b4);
            }
            else
            {
                /* Sender is CPU2 */
                Update_ATC_Remote2_Relay_B_State((BYTE) Flags2.Bit.b4);
            }
        }
    }
    Update_DS_Power_Status(DAC_Config,Flags1);
    Update_DS_Track_Status(Flags2);

    if(Status.Flags.DS_System_Status != NORMAL)
    {
    /* Already local system failed, So no need to Process the axle counts of remote unit */
       return;
    }
    Check_DS_Unit_Status(Flags2);

    /*
     * Process the Axle Count
     */
    Fwd_CountValue.Byte.Lo  = Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET];
    Fwd_CountValue.Byte.Hi  = Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET];
    Rev_CountValue.Byte.Lo  = Com2RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET];
    Rev_CountValue.Byte.Hi  = Com2RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET];
    R_Fwd_CountValue.Byte.Lo  = Com2RecvObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET];
    R_Fwd_CountValue.Byte.Hi  = Com2RecvObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET];
    R_Rev_CountValue.Byte.Lo  = Com2RecvObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET];
    R_Rev_CountValue.Byte.Hi  = Com2RecvObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET];
    Axle_CountValue.Byte.Lo = Com2RecvObject.Msg_Buffer[COM_AXLE_COUNT_LO_OFFSET];
    Axle_CountValue.Byte.Hi = Com2RecvObject.Msg_Buffer[COM_AXLE_COUNT_HI_OFFSET];

    if(DIP_Switch_Info.Flags.Is_DAC_CPU1)
    {
        switch(DIP_Switch_Info.DAC_Unit_Type)
        {
            case DAC_UNIT_TYPE_SF:
                if(comm_check_SF1.State == COMM_GOOD && DS_Section_Mode.Local_Unit == SYSTEM_OCCUPIED_MODE)
                {
                    for(uchTrack = 0;uchTrack<10;uchTrack++)
                        comm_check_SF1.Track_counts[uchTrack] = Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET + uchTrack];
                }
                break;
            case DAC_UNIT_TYPE_EF:
                break;
            default:
                break;
        }
        
    }
    else
    {
        switch(DIP_Switch_Info.DAC_Unit_Type)
        {
            case DAC_UNIT_TYPE_SF:
                if(comm_check_SF2.State == COMM_GOOD && DS_Section_Mode.Local_Unit == SYSTEM_OCCUPIED_MODE)
                {
                    for(uchTrack = 0;uchTrack<10;uchTrack++)
                        comm_check_SF2.Track_counts[uchTrack] = Com2RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET + uchTrack];
                }
                break;
            case DAC_UNIT_TYPE_EF:
                break;
            default:
                break;
        }

    }

    Local_Count = (BYTE) Axle_CountValue.Word;

    Update_SM_DS_Remote_Axle_Counts(Axle_CountValue.Byte.Lo,Axle_CountValue.Byte.Hi);

    uchDirection = (Flags1.Byte >> 6);

    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
            case DAC_UNIT_TYPE_SF:
                if (SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Process_DS_AxleCount(SrcMsgAdr,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_DS_Direction(SrcMsgAdr,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_CF:
                if (SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Process_DS_AxleCount(SrcMsgAdr,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_DS_Direction(SrcMsgAdr,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_D3_A:
                if (SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Process_D3_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D3_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_D3_B:
                if (SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Process_D3_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D3_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_D3_C:
                if (SrcMsgAdr.Byte < DestMsgAdr.Byte )
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Process_D3_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D3_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_D4_A:
                if (SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Buffer.Bit.b3 = DAC_Config.Bit.b3;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                    //A will recv C remote data and will always recieve CPU2 data
                    SrcMsgAdr.Byte = 2;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,DAC_UNIT_TYPE_D4_C,R_Rev_CountValue.Word,R_Fwd_CountValue.Word);
                    //Axle count
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_C,(BYTE)FORWARD_DIRECTION,R_Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_C,(BYTE)REVERSE_DIRECTION,R_Fwd_CountValue.Word);                
                }
                break;
            case DAC_UNIT_TYPE_D4_B:
                if (SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Buffer.Bit.b3 = DAC_Config.Bit.b3;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                    //B will recv D remote data and will always recieve CPU1 data
                    SrcMsgAdr.Byte = 2;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,DAC_UNIT_TYPE_D4_D,R_Rev_CountValue.Word,R_Fwd_CountValue.Word);                                
                    //Axle count
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_D,(BYTE)FORWARD_DIRECTION,R_Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_D,(BYTE)REVERSE_DIRECTION,R_Fwd_CountValue.Word);                
                }
                break;
            case DAC_UNIT_TYPE_D4_C:
                if (SrcMsgAdr.Byte > DestMsgAdr.Byte )
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Buffer.Bit.b3 = DAC_Config.Bit.b3;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                    //C will recv A remote data and will always recieve CPU1 data
                    SrcMsgAdr.Byte = 2;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,DAC_UNIT_TYPE_D4_A,R_Fwd_CountValue.Word,R_Rev_CountValue.Word);                
                    //Axle count
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_A,(BYTE)FORWARD_DIRECTION,R_Fwd_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_A,(BYTE)REVERSE_DIRECTION,R_Rev_CountValue.Word);                
                }
                break;
            case DAC_UNIT_TYPE_D4_D:
                if (SrcMsgAdr.Byte < DestMsgAdr.Byte )
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Buffer.Bit.b3 = DAC_Config.Bit.b3;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                    //D will recv B remote data and will always recieve CPU1 data
                    SrcMsgAdr.Byte = 2;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,DAC_UNIT_TYPE_D4_B,R_Rev_CountValue.Word,R_Fwd_CountValue.Word);                
                    //Axle count
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_B,(BYTE)FORWARD_DIRECTION,R_Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_B,(BYTE)REVERSE_DIRECTION,R_Fwd_CountValue.Word);                
                }
                break;
        case DAC_UNIT_TYPE_3D_SF:
                if (SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Process_3S_DS_AxleCount(SrcMsgAdr,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_DS_Direction(SrcMsgAdr,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_3D_EF:
                if (SrcMsgAdr.Byte < DestMsgAdr.Byte)
                {
                 Process_3S_DS_AxleCount(SrcMsgAdr,Fwd_CountValue.Word,Rev_CountValue.Word);
                 Process_DS_Direction(SrcMsgAdr,uchDirection,Axle_CountValue.Word);
                }
                break;
            default:
                break;
    }

}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Process_DS_AxleCount(bitadrb_t SrcAdr, BYTE uchDirection, UINT16 uiAxleCount)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the forward and reverse axle counts in Down stream
                     unit CPU1 or in CPU2

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5059


Interfaces
    Calls           :   RLYB_MGR.c  -   Update_B_DS1_IN_Count()
                        RLYB_MGR.c  -   Update_B_DS1_OUT_Count()
                        RLYB_MGR.c  -   Update_B_DS2_IN_Count()
                        RLYB_MGR.c  -   Update_B_DS2_OUT_Count()

    Called by       :   COMM_DS.c   -   Process_DS_Axle_Count_Message()

Input Variables         Name                        Type
    Global          :   None

    Local           :   SrcAdr                      bitadrb_t
                        uiFwdAxleCount              UINT16
                        uiRevAxleCount              UINT16
                        SrcAdr.Bit.b0               Bit


Output Variables        Name                        Type
    Global          :   None

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_HIGH                        1

References          :

Derived Requirements:

************************************************************************/
void Process_DS_AxleCount(bitadrb_t SrcAdr, UINT16 uiFwdAxleCount, UINT16 uiRevAxleCount)
{
    if (SrcAdr.Bit.b0 == SET_HIGH)
    {
        Update_B_DS1_IN_Count(uiRevAxleCount);
        Update_B_DS1_OUT_Count(uiFwdAxleCount);
    }
    else
    {
        Update_B_DS2_IN_Count(uiRevAxleCount);
        Update_B_DS2_OUT_Count(uiFwdAxleCount);
    }
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Process_DS_Direction(bitadrb_t SrcAdr, BYTE uchDirection)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update direction and axle counts in down stream unit
                     CPU1 or CPU2

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5360

Interfaces
    Calls           :   RLYB_MGR.c  -   Update_B_DS1_Direction()
                        RLYB_MGR.c  -   Update_B_DS2_Direction()

    Called by       :   COMM_DS.c   -   Process_DS_Axle_Count_Message()

Input Variables         Name                        Type
    Global          :   None

    Local           :   SrcAdr                      bitadrb_t
                        uchDirection                BYTE
                        SrcAdr.Bit.b0               Bit
                        axle_Count                  UINT16

Output Variables        Name                        Type
    Global          :   None

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_HIGH                        1
References          :

Derived Requirements:

Algorithm           :1.Update the Downstream CPU1,CPU2 axle direction and axle count value
************************************************************************/
void Process_DS_Direction(bitadrb_t SrcAdr, BYTE uchDirection, UINT16 axle_Count)
{

    if (SrcAdr.Bit.b0 == SET_HIGH)
    {
        Update_B_DS1_Direction(uchDirection);
        Update_B_DS1_AxleCount(axle_Count);
    }
    else
    {
        Update_B_DS2_Direction(uchDirection);
        Update_B_DS2_AxleCount(axle_Count);
    }

}

//15_03_10
void Process_3S_DS_AxleCount(bitadrb_t SrcAdr, UINT16 uiFwdAxleCount, UINT16 uiRevAxleCount)
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
            if (SrcAdr.Bit.b0 == SET_HIGH)
             {
               Update_B_DS1_IN_Count(uiRevAxleCount);
               Update_B_DS1_OUT_Count(uiFwdAxleCount);
             }
             else
             {
               Update_B_DS2_IN_Count(uiRevAxleCount);
               Update_B_DS2_OUT_Count(uiFwdAxleCount);
             }
            break;
         case DAC_UNIT_TYPE_3D_EF:
            if (SrcAdr.Bit.b0 == SET_HIGH)
             {
               Update_B_DS1_IN_Count(uiFwdAxleCount);
               Update_B_DS1_OUT_Count(uiRevAxleCount);
             }
             else
             {
               Update_B_DS2_IN_Count(uiFwdAxleCount);
               Update_B_DS2_OUT_Count(uiRevAxleCount);
             }
            break;
        }
}
/*************************************************************************
Component name      :COMM_DS
Module Name         :void Synchronise_DS_Sch(bitadrb_t SrcAdr)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Sychronise the communication scheduler to prevent the communication
                      line from always being busy.

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5361



Interfaces
    Calls           :   Nil

    Called by       :   COMM_DS.c   -   Process_DS_Reset_Info_Message()
                        COMM_DS.c   -   Process_DS_Axle_Count_Message()

Input Variables         Name                                Type
    Global          :   COMM_SCHEDULER_IDLE                 Enumerator
                        DS_Sch_Info.State                   Enumerator
                        DIP_Switch_Info.Flags.Is_CPU1       Bit
                        uiCOM2_BalanceTicks_Table           UINT16

    Local           :   SrcAdr                              bitadrb_t
                        Config                              bitadrb_t
                        uchRow                              BYTE
                        uchCol                              BYTE
                        SrcAdr.Bit.b0                       Bit
                        Config.Bit.b5                       Bit

Output Variables        Name                                Type
    Global          :   DS_Sch_Info.Timeout_ms              UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            TRUE                            1
                            SET_HIGH                        1
                            SET_LOW                         1
References          :

Derived Requirements:

************************************************************************/
void Synchronise_DS_Sch(bitadrb_t SrcAdr)
{
    BYTE uchRow =0;
    BYTE uchCol =0;
    /*
     * Synchronise in order to prevent communication line from always being busy.
     *
     * Message sequence is LU1-RU1-LU2-RU2........
     */
    if (DS_Sch_Info.State != COMM_SCHEDULER_IDLE)
    {
        return;
    }
    if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
    {
        uchRow = 0;
    }
    else
    {
        uchRow = 1;
    }
    if (SrcAdr.Bit.b0 == SET_HIGH)
    {
        uchCol = 0;     /* Remote Unit - CPU1  Telegram  received*/
    }
    else
    {
        uchCol = 1;     /* Remote Unit - CPU2  Telegram  received*/
    }
    DS_Sch_Info.Timeout_ms = uiCOM2_BalanceTicks_Table[uchRow][uchCol];
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :void Reset_DS_CountDown(bitadrb_t SrcAdr,bitadrb_t MsgFlags)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Resets communication link status for local and remote units.

Allocated Requirements  :

Design Requirements     :SSDAC_DR_5362


Interfaces
    Calls           :   Nil

    Called by       :   COMM_DS.c   -   Process_DS_Reset_Info_Message()
                        COMM_DS.c   -   Process_DS_Axle_Count_Message()

Input Variables         Name                                Type
    Global          :   DIP_Switch_Info.Flags.Is_CPU1       Bit

    Local           :   SrcAdr                              bitadrb_t
                        MsgFlags                            bitadrb_t
                        SrcAdr.Bit.b0                       Bit
                        MsgFlags.Bit.b2                     Bit
                        MsgFlags.Bit.b3                     Bit
                        MsgFlags.Bit.b4                     Bit
                        MsgFlags.Bit.b5                     Bit

Output Variables            Name                            Type
    Global          :   Comm_B_CountDown.DS1_to_LU1         BYTE
                        Status.Flags.DS1_to_LU1_Link        Bit
                        Status.Flags.LU1_to_DS1_Link        Bit
                        Status.Flags.LU2_to_DS1_Link        Bit
                        Comm_B_CountDown.DS2_to_LU1         BYTE
                        Status.Flags.DS2_to_LU1_Link        Bit
                        Status.Flags.LU1_to_DS2_Link        Bit
                        Status.Flags.LU2_to_DS2_Link        Bit
                        Comm_B_CountDown.DS1_to_LU2         BYTE
                        Status.Flags.DS1_to_LU2_Link        Bit
                        Status.Flags.LU1_to_DS1_Link        Bit
                        Status.Flags.LU2_to_DS1_Link        Bit
                        Comm_B_CountDown.DS2_to_LU2         BYTE
                        Status.Flags.DS2_to_LU2_Link        Bit
                        Status.Flags.LU1_to_DS2_Link        Bit
                        Status.Flags.LU2_to_DS2_Link        Bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_HIGH                        1
                            MAXIMUM_COM_RETRIES             3
                            COMMUNICATION_OK                1
References          :

Derived Requirements:

************************************************************************/
void Reset_DS_CountDown(bitadrb_t SrcAdr, bitadrb_t MsgFlags)
{

    if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
    {
        /* Local Unit - CPU1 */
        if (SrcAdr.Bit.b0 == SET_HIGH)
        {
            /* From Remote Unit - CPU1 */
            if(Track_Status_Info.Bit.Local_DS_Track_Status == TRACK_CLEAR)
            {
                Comm_B_CountDown.DS1_to_LU1  = MAXIMUM_COM_RETRIES;
            }
            else
            {
                Comm_B_CountDown.DS1_to_LU1  = MAXIMUM_OCCUPIED_COM_RETRIES;
            }
            Status.Flags.DS1_to_LU1_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_DS1_Link = MsgFlags.Bit.b2;
            Status.Flags.LU2_to_DS1_Link = MsgFlags.Bit.b3;
        }
        else
        {
            /* From Remote Unit - CPU2 */
            if(Track_Status_Info.Bit.Local_DS_Track_Status == TRACK_CLEAR)
            {
                Comm_B_CountDown.DS2_to_LU1  = MAXIMUM_COM_RETRIES;
            }
            else
            {
                Comm_B_CountDown.DS2_to_LU1  = MAXIMUM_OCCUPIED_COM_RETRIES;
            }
            Status.Flags.DS2_to_LU1_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_DS2_Link = MsgFlags.Bit.b4;
            Status.Flags.LU2_to_DS2_Link = MsgFlags.Bit.b5;
        }
    }
    else
    {
        /* Local Unit - CPU2 */
        if (SrcAdr.Bit.b0 == SET_HIGH)
        {
            /* From Remote Unit - CPU1 */
            if(Track_Status_Info.Bit.Local_DS_Track_Status == TRACK_CLEAR)
            {
                Comm_B_CountDown.DS1_to_LU2  = MAXIMUM_COM_RETRIES;
            }
            else
            {
                Comm_B_CountDown.DS1_to_LU2  = MAXIMUM_OCCUPIED_COM_RETRIES;
            }
            Status.Flags.DS1_to_LU2_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_DS1_Link =  MsgFlags.Bit.b2;
            Status.Flags.LU2_to_DS1_Link =  MsgFlags.Bit.b3;
        }
        else
        {
            /* From Remote Unit - CPU2 */
            if(Track_Status_Info.Bit.Local_DS_Track_Status == TRACK_CLEAR)
            {
                Comm_B_CountDown.DS2_to_LU2  = MAXIMUM_COM_RETRIES;
            }
            else
            {
                Comm_B_CountDown.DS2_to_LU2  = MAXIMUM_OCCUPIED_COM_RETRIES;
            }
            Status.Flags.DS2_to_LU2_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_DS2_Link =  MsgFlags.Bit.b4;
            Status.Flags.LU2_to_DS2_Link =  MsgFlags.Bit.b5;
        }
    }
}
/*********************************************************************
Component name      :COMM_DS
Module Name         :Check_DS_Unit_Status(bitadrb_t Buffer)
Created By          :S Venkata Krishna
Date Created        :15/12/2017
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Checks for the DS system status,if any error declare it as
                     DAC defective .Downside DAC failed so drop the relay
                     and set DS_ERROR_NUM error bit

Allocated Requirements	:	(SSDAC_SWRS_0586), (SSDAC_SWRS_0300) 	
							(SSDAC_SWRS_0678), (SSDAC_SWRS_0778), (SSDAC_SWRS_0390)

Design Requirements		: 	SSDAC_DR_5363

Interfaces
    Calls           :   RELAYMGR.c  -   Declare_DAC_Defective_DS()
                        ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   COMM-DS..c  -   Process_DS_Axle_Count_Message()

Input Variables         Name                            Type
    Global          :   None

    Local           :   Buffer                          bitadrb_t
                        Buffer.Bit.b0                   Bit
                        Buffer.Bit.b1                   Bit

Output Variables        Name                            Type
    Global          :   None

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            DS_ERROR_NUM                26

References          :

Derived Requirements:

************************************************************************/
void Check_DS_Unit_Status(bitadrb_t Buffer)
{
    if(Buffer.Bit.b0 == 0 || Buffer.Bit.b1 == 0)
    {
        Declare_DAC_Defective_DS();                 /*Downside DAC failed so drop the relay */
        Set_Error_Status_Bit(DS_ERROR_NUM);
    }
}

void Configure_Modem_B (void)
{
 if(DIP_Switch_Info.COM2_Mode == (comm_type)FSK_COMMUNICATION)
 {
     switch(Comm_Modem_B.State)
        {
         case MODEM_IDLE:
            if(Comm_Modem_B.Timeout_ms == 0)
            {
             Comm_Modem_B.State = GET_MODEM_LINE;
            }
            break;
         case GET_MODEM_LINE:
             if(MODEM_B_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
             {
              MODEM_B_CHANNEL_CONTROL = GRAB_MODEM_CHANNEL;
                if(MODEM_B_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
                 {
                   MODEM_B_M1 = SET_HIGH;
                   MODEM_B_M0 = SET_HIGH;
                   Comm_Modem_B.State = SET_MODEM_ZP_MODE;
                   Comm_Modem_B.Timeout_ms = 20;
                 }
               }
              else
                {
                  MODEM_B_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
                  Comm_Modem_B.State = MODEM_IDLE;
                  Comm_Modem_B.Timeout_ms = 30;
                }
             break;
         case SET_MODEM_ZP_MODE:
            if(Comm_Modem_B.Timeout_ms == 0)
            {
             MODEM_B_M1 = SET_HIGH;
             MODEM_B_M0 = SET_LOW;
             Comm_Modem_B.State = DELAY_FOR_CONFIGURE;
             MODEM_B_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
             Comm_Modem_B.Timeout_ms = 50;
            }
             break;
         case DELAY_FOR_CONFIGURE:
            if(Comm_Modem_B.Timeout_ms == 0)
            {
             Comm_Modem_B.State = CONFIGURATION_COMPLETED;
            }
             break;
         case CONFIGURATION_COMPLETED:
             break;
         }
    }
    else
    {
     if(Comm_Modem_B.State != CONFIGURATION_COMPLETED)
     {
      MODEM_B_M1 = SET_HIGH;
      MODEM_B_M0 = SET_HIGH;
      Comm_Modem_B.State = CONFIGURATION_COMPLETED;
      Comm_Modem_B.Timeout_ms = 30;
     }
    }
}

BYTE Get_Modem_B_State(void)
{
    BYTE State;

    State = (BYTE) Comm_Modem_B.State;
  return(State);
}

void Clear_DS_Transmit_Buffer(void)
{
 BYTE count;

 for(count = 0; count < COMM_MESSAGE_LENGTH; count++)
    {
    Com2XmitObject.Msg_Buffer[count] = 0x00;
    }
}

