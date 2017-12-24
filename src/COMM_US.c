/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   COMM_US
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
    Function    :   void SetupCOM1BaudRate(BYTE uchBaudRate)
                    void Initialise_US_CommSch(void)
                    void Start_US_CommSch(void)
                    void Update_US_Sch_State(void)
                    void Receive_COM1_Message(void)
                    void Set_US_Sch_Idle(void)
                    void Decrement_US_Sch_msTmr(void)
                    void Decrement_Comm_US_CountDown(void)
                    void Build_US_Message(void)
                    void Build_US_Reset_Info_Message(void)
                    void Build_US_Axle_Count_Message(void)
                    void Process_US_Message(void)
                    void Process_US_Reset_Info_Message(void)
                    void Process_US_Axle_Count_Message(void)
                    void Process_US_AxleCount(bitadrb_t SrcAdr,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
                    void Process_US_Direction(bitadrb_t SrcAdr, BYTE uchDirection)
                    void Synchronise_US_Sch(bitadrb_t SrcAdr)
                    void Reset_US_CountDown(bitadrb_t SrcAdr,bitadrb_t MsgFlags)
                    void Check_US_Unit_Status(bitadrb_t Buffer)

*****************************************************************************/
#include <xc.h>

#include "COMMON.h"
#include "COMM_US.h"
#include "CRC16.h"
#include "RESET.h"
#include "SYS_MON.h"
#include "ERROR.h"
#include "AES.h"
#include "RLYD4_MGR.h"



extern  /*near*/  dac_status_t Status;                        /* from DAC_MAIN.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;          /* from DAC_MAIN.c */
extern checksum_info_t Checksum_Info;                   /* from DAC_MAIN.c */

extern us_section_mode US_Section_Mode;                 /*from DAC_MAIN.c*/
const UINT16 uiCOM1_BalanceTicks_Table[2][2] = {
                        /*
                         * Communcation Sequence is RU1-LU1-RU2-LU2
                         * Message originated from Unit...
                         *         RU1          RU2
                         *         CPU1         CPU2
                         *         Telegram1    Telegram1
                         * LUx -> Local Unit CPUx
                         * RUx -> Remote Unit CPUx (Up Stream)
                         */
    /* Receiving Unit CPU1 */   {  50,          600},
    /* Receiving Unit CPU2 */   {  600,          50}
    };

 /*near*/  comm_sch_info_t US_Sch_Info;


comm_a_countdown_t Comm_A_CountDown =
            {MAXIMUM_COM_RETRIES, MAXIMUM_COM_RETRIES,
             MAXIMUM_COM_RETRIES, MAXIMUM_COM_RETRIES};

msg_info_t    Com1XmitObject;   /* COM1: Message Transmission Buffer etc., */
msg_info_t    Com1RecvObject;   /* COM1: Message Receiving Buffer etc., */
modem_info_t  Comm_Modem_A;
unsigned long int us_pkt_error_time = 0;
unsigned int COM_US_pkt_error_cnt = 0;



void Receive_COM1_Message(void);
void Process_US_Message(void);
void Process_US_Reset_Info_Message(void);
void Process_US_Axle_Count_Message(void);
void Process_US_AxleCount(bitadrb_t SrcAdr,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);    
void Process_3S_US_AxleCount(bitadrb_t SrcAdr,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount); /* added for 3D3S configuration */
void Process_US_Direction(bitadrb_t SrcAdr, BYTE uchDirection,UINT16 axle_Count);
void Decrement_Comm_US_CountDown(void);
void Set_US_Sch_Idle(void);
void Build_US_Message(void);
void Build_US_Reset_Info_Message(void);
void Build_US_Axle_Count_Message(void);
void Synchronise_US_Sch(bitadrb_t SrcAdr);
void Reset_US_CountDown(bitadrb_t SrcAdr,bitadrb_t MsgFlags);
void Check_US_Unit_Status(bitadrb_t Buffer);

void US_LED_Error_Service_1mS(void);
void Clear_COM1_Errors(void);
/*******************************************************************
Component name      :COMM_US
Module Name         :void SetupCOM1BaudRate(BYTE uchBaudRate)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Set the Baud Rate for USART communication,clear the bit
                     TX9 and RX9 for 8 bit transmission and reception.Disbale the
                     Transmit and Recieve interrupt,disable the continuous reception


Design Requirements     :



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

Output Variables        Name                                Type
    Global          :   BRGH                            8 Bit register
                        BRG161                          8 Bit register
                        SPBRG1                          8 Bit register
                        SPBRGH1                         8 Bit register
                        SYNC                            TXSTA2 control register bit
                        SPEN                            RCSTA2 control register bit
                        TX9                             TXSTA2 control register bit
                        RX9                             RCSTA2 control register bit
                        TX1IE                           PIE3 peripharal interrupt register
                        RC1IE                           PIE3 peripharal interrupt register
                        TXEN                            TXSTA2 control register bit
                        SREN                            RCSTA2 control register bit
                        CREN                            RCSTA2 control register bit
                        WUE1                            BAUDCON2 register bit
                        ABDEN1                          BAUDCON2 register bit

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
**********************************************************************/
void SetupCOM1BaudRate(void)
{
  if(DIP_Switch_Info.COM1_Mode == (comm_type)FSK_COMMUNICATION)
   {
        U1BRG = 3332;
    }
  else
   {


        switch (DIP_Switch_Info.Baud_Rate)
        {
            case BAUDRATE_1200:
                U1BRG = 3332;
                break;
            case BAUDRATE_2400:
                U1BRG = 1665;
                break;
            case BAUDRATE_9600:
                U1BRG = 415;
                break;
            case BAUDRATE_19200:
                U1BRG = 207;
                break;
        }
     }
    TRISFbits.TRISF2 = 0;       // RF2 is Tx, Output
    TRISFbits.TRISF3 = 1;       // RF3 is Rx, Input

    /* Configure Remappable pins */
    //*************************************************************
    // Unlock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON & 0xbf); //clear the bit 6 of OSCCONL to unlock Pin Re-map

    RPINR18bits.U1RXR = 16;           // Map U1RX input to RP16
    RPOR15bits.RP30R = 3;             // Map RP30 to U1TX Output
    //************************************************************
    // Lock Registers
    //************************************************************
    __builtin_write_OSCCONL(OSCCON | 0x40); //set the bit 6 of OSCCONL to lock Pin Re-map
    //******************************************************************

    U1MODE = 0;
    U1STA = 0;

    // Operate at 8 bit mode, No Parity, 1 Stop bit
    U1MODEbits.RTSMD = 1;   // Operate in Simplex Mode

    U1MODEbits.BRGH = 1;    // High Speed Clocks to BRG @ 4x of Fp


    IEC4bits.U1ERIE = 0;
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 0;

    IFS4bits.U1ERIF = 0;
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;

    U1MODEbits.UARTEN = 1;  // Enable UART
}
/*******************************************************************
Component name      :COMM_US
Module Name         :void Initialise_US_CommSch(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Set the Baud Rate for USART communication,clear the bit
                     TX9 and RX9 for 8 bit transmission and reception.Disbale the
                     Transmit and Recieve interrupt,disable the continuous reception


Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Initialise_System()

Input Variables         Name                            Type
    Global          :   DIP_Switch_Info.Baud_Rate       Baud_Rate_Config
                        BAUDRATE_1200                   Enumerator
                        BAUDRATE_2400                   Enumerator
                        BAUDRATE_9600                   Enumerator
                        BAUDRATE_19200                  Enumerator
                        COM_RECV_BUFFER_EMPTY           Enumerator

    Local           :   None

Output Variables        Name                                Type
    Global          :   BRGH                            8 Bit register
                        BRG161                          8 Bit register
                        SPBRG1                          8 Bit register
                        SPBRGH1                         8 Bit register
                        SYNC                            TXSTA2 control register bit
                        SPEN                            RCSTA2 control register bit
                        TX9                             TXSTA2 control register bit
                        RX9                             RCSTA2 control register bit
                        TX1IE                           PIE3 peripharal interrupt register
                        RC1IE                           PIE3 peripharal interrupt register
                        TXEN                            TXSTA2 control register bit
                        SREN                            RCSTA2 control register bit
                        CREN                            RCSTA2 control register bit
                        WUE1                            BAUDCON2 register bit
                        ABDEN1                          BAUDCON2 register bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            NO_MSG_TO_XMIT                  0
                            COMM_MESSAGE_LENGTH             14
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

Algorithm           :1.Set the communication transmit state as "NO_MSG_TO_XMIT" and load the transmission length as 14
                     2.Set the upstream schedular state as "COMM_SCHEDULER_NOT_STARTED" scan period as 2400
                     3.Clear the transmission object buffer
                     4.Select the transmission and reception byte period as per the baudrate setted and then calculate the total transmission time
**********************************************************************/
void Initialise_US_CommSch(void)
{
    BYTE uchCnt;

    Com1XmitObject.State = NO_MSG_TO_XMIT;
    Com1XmitObject.Index = 0;
    Com1XmitObject.Msg_Length  = COMM_MESSAGE_LENGTH;
    Com1RecvObject.State = (BYTE)COM_RECV_BUFFER_EMPTY;
    Com1RecvObject.Index = 0;
    Com1RecvObject.Timeout_ms = 0;
    Com1RecvObject.Consecutive_Error_Count = 0;
    Com1RecvObject.Msg_Length = COMM_MESSAGE_LENGTH;
    US_Sch_Info.State = COMM_SCHEDULER_NOT_STARTED;
    MODEM_A_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
    US_Sch_Info.ScanPeriod = COMM_SCHEDULER_SCAN_RATE;
    for (uchCnt = 0; uchCnt < COMM_MESSAGE_LENGTH; uchCnt++)
    {
      Com1XmitObject.Msg_Buffer[uchCnt] = (BYTE) 0;
    }
    if(DIP_Switch_Info.COM1_Mode == (comm_type)FSK_COMMUNICATION)
    {
     Com1XmitObject.BytePeriod = BYTE_PERIOD_9MS;
     Com1RecvObject.BytePeriod = BYTE_PERIOD_9MS;
    }
    else
    {
     switch (DIP_Switch_Info.Baud_Rate)
      {
        case BAUDRATE_1200:
            Com1XmitObject.BytePeriod = BYTE_PERIOD_9MS;
            Com1RecvObject.BytePeriod = BYTE_PERIOD_9MS;
            break;
        case BAUDRATE_2400:
            Com1XmitObject.BytePeriod = BYTE_PERIOD_5MS;
            Com1RecvObject.BytePeriod = BYTE_PERIOD_5MS;
            break;
        case BAUDRATE_9600:
            Com1XmitObject.BytePeriod = BYTE_PERIOD_1MS;
            Com1RecvObject.BytePeriod = BYTE_PERIOD_1MS;
            break;
        case BAUDRATE_19200:
            Com1XmitObject.BytePeriod = BYTE_PERIOD_1MS;
            Com1RecvObject.BytePeriod = BYTE_PERIOD_1MS;
            break;
        }
    }
    Com1XmitObject.Transmission_Time = (BYTE)((Com1XmitObject.Msg_Length + (BYTE) 2) * Com1XmitObject.BytePeriod);
    Comm_Modem_A.State = MODEM_IDLE;
    Comm_Modem_A.Timeout_ms = 0;
}

/******************************************************************************
Component name      :COMM_US
Module Name         :void Start_US_CommSch(void)
Created By          :
Date Created        :
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

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()

Input Variables                     Name                        Type
    Global          :   COMM_SCHEDULER_IDLE                 Enumerator
                        DIP_Switch_Info.Flags.Is_CPU1       Bit


    Local           :   NONE

Output Variables                    Name                        Type
    Global          :       US_Sch_Info.State               Enumerator
                            US_Sch_Info.Timeout_ms          UINT16
                            TXEN                            TXSTA1 control register bit
                            CREN                            RCSTA1 control register bit

    Local           :   NONE

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                        COMM_RE_SYNCHRONIZATION_TIME        4800
                        COMM_SCHEDULER_SCAN_RATE            2400
                        SET_HIGH                            1

References          :

Derived Requirements:

Algorithm           :1.Set the upstream schedular state as idle
                     2.Calculate the schedular timeout value
                     3.Enable the transmission and continuos reception
***************************************************************************/
void Start_US_CommSch(void)
{

    US_Sch_Info.State = COMM_SCHEDULER_IDLE;
    if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
    {
        US_Sch_Info.Timeout_ms = (UINT16)(COMM_RE_SYNCHRONIZATION_TIME + (COMM_SCHEDULER_SCAN_RATE / 4));
        Comm_Modem_A.Timeout_ms = 0;
    }
    else
    {
        US_Sch_Info.Timeout_ms = (UINT16) (COMM_RE_SYNCHRONIZATION_TIME + ((COMM_SCHEDULER_SCAN_RATE * 3)/4));
        Comm_Modem_A.Timeout_ms = 30;
    }
    //TXEN = SET_HIGH;    /* Enable Transmitter */
    //CREN = SET_HIGH;    /* Enable Continous Receive */
    U1STAbits.UTXEN = SET_HIGH;    /* Enable Transmitter */

}

/*******************************************************************************
Component name      :COMM_US
Module Name         :void Update_US_Sch_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the communication status.calls Receive_COM1_Message().
                     If received message is valid then process it.


Design Requirements     :



Interfaces
    Calls           :   COMM_US.C   -   Receive_COM1_Message()
                        COMM_US.c   -   Process_US_Message()
                        COMM_US.c   -   Decrement_Comm_US_CountDown()
                        COMM_US.c   -   Set_US_Sch_Idle()
                        COMM_US.C   -   Build_US_Message()
                        RELAYMGR.c  -   Declare_DAC_Defective_US()
                        ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables                     Name                    Type
    Global          :   Com1RecvObject.State                Enumerator
                        COM_VALID_MESSAGE                   Enumerator
                        COM_RECV_BUFFER_EMPTY               Enumerator
                        US_Sch_Info.State                   Enumerator
                        COMM_SCHEDULER_NOT_STARTED          Enumerator
                        COMM_SCHEDULER_IDLE                 Enumerator
                        US_Sch_Info.Timeout_ms              UINT16
                        GET_MODEM_CHANNEL                   Enumerator
                        SET_MODEM_RS_LOW                    Enumerator
                        DIP_Switch_Info.Flags.Is_CPU1       Bit
                        WAIT_FOR_CS_LOW                     Enumerator
                        WAIT_FOR_CARRIER_STABALISE          Enumerator
                        ERROR_MODEM_NO_CARRIER              Enumerator
                        TRANSMIT_TELEGRAM1                  Enumerator
                        Com1XmitObject.Transmission_Time    BYTE
                        Com1XmitObject.Index                BYTE
                        WAIT_FOR_DELAY_BETWEEN_TELEGRAMS    Enumerator
                        TX1IF                               Peripheral Interrupt register bit
                        Com1XmitObject.Msg_Buffer           Array of BYTEs
                        TRANSMIT_TELEGRAM2                  Enumerator
                        HOLD_MODEM_RS_LOW                   Enumerator

    Local           :   Telegram_No                         BYTE

Output Variables            Name                            Type
    Global          :   Com1RecvObject.State                Enumerator
                        US_Sch_Info.State                   Enumerator
                        US_Sch_Info.Timeout_ms              UINT16
                        US_Telegram_No                      BYTE
                        Status.Flags.Modem_B                Bit
                        TXREG1                              USART1 Transmit Register
                        Com1XmitObject.Index                BYTE
                        Status.Flags.Modem_A                Bit

    Local           :   None

Signal Variables

                            MODEM_A_CHANNEL_STATUS              Input                   RB3
                            MODEM_A_CHANNEL_CONTROL             Output                  LATB2
                            MODEM_A_CD                          Input                   RE6
                            MODEM_A_M1                          Output                  RE7
                            MODEM_A_M0                          Output                  LATE5

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
                            COMM_MESSAGE_LENGTH                 12
                            TIME_BETWEEN_TWO_TELEGRAMS          20
                            MODEM_HOLD_RS_LOW_TIMEOUT           30

References          :

Derived Requirements:

Algorithm           :1.Get the message from the other unit,if it Valid message process it
                     2.Initailly Up Stream communication schedular will be in Idle state
                     3.After Up Stream schedular scan time completed,decrement the Up
                       Stream communication retries by 1 and go to step 4
                     4.Check for communication channel status,if it is free grab
                        the channel and go to step 6
                     5.Check whether other processor has also grabbed the channel,
                       if it is grabbed release the channel
                     6.Check for CD and CS HIGH, if both are HIGH go to step 7
                     7.Check for CS low, if it is low build the transimit message
                        and go to step 9
                     8.Check for carrier generation, if it is not generated, release
                        the modem channel declare DAC defective and set modem error no
                        carrier Id
                     9.After carrier stabilisation time over(50ms) go to step 10
                        to start the transmission
                     10.If meassage length is greater than Maximum message length(12)
                        go to step 13
                     11.Check for USART transmitter buffer interrupt flag is set means
                        TXREG is empty, load the data to TXREG2 register
                     12.Check for Up Stream communication schedular timeout, if it has
                        occured means, make the Up stream schedular ready for Next transmission
                     13.Check for Up Stream communication schedular timeout, if it has
                        occured means,build the second message and go to step 14 to send the
                        second telegram
                     14.If meassage length is greater than Maximum message length(12) go
                        to step 17
                     15.Check for USART transmitter buffer interrupt flag is set means
                        TXREG is empty, load the data to TXREG2 register
                     16.Check for Up Stream communication schedular timeout, if it has
                        occured means, make the Up stream schedular ready for Next
                        transmission
                     17.Check for Up stream communication schedular timeout, if it has
                        occured means, make the Up stream schedular ready for Next
                        transmission

**********************************************************************************/
unsigned int us_failure_led_timer;
#include "RELAYMGR.h"

void US_LED_Error_Service_1mS(void)        // This is the Master Transmission
{
    us_failure_led_timer = us_failure_led_timer + 1;
    if(us_failure_led_timer >= 100)
    {
        us_failure_led_timer = 0;
        if(US_Section_Mode.Local_Unit == SYSTEM_ERROR_MODE || Status.Flags.System_Status != NORMAL)
        {
            SECTION_A_LED_CLEAR_PORT = ~SECTION_A_LED_CLEAR_PORT;
        }
    }
}

void US_Packet_error_timmer(void)        // This is the Master Transmission
{
    us_pkt_error_time = us_pkt_error_time + 1;
    if(us_pkt_error_time >= PACKET_ERROR_TIME_THRESHOLD)
    {
        us_pkt_error_time = 0;
        //COM_US_pkt_error_cnt = 0;
    }
}

void Update_US_Sch_State(void)
{
    BYTE uchEncipher_State;

    Receive_COM1_Message();

    if (Com1RecvObject.State == (BYTE)COM_VALID_MESSAGE)
    {
        Process_US_Message();       /* Valid Message received, process it */
        Com1RecvObject.State = (BYTE)COM_RECV_BUFFER_EMPTY;
    }

    switch (US_Sch_Info.State)
    {
        case TRANSMIT_DUMMY_BYTES:
        case COMM_SCHEDULER_NOT_STARTED:
            break;
        case COMM_SCHEDULER_IDLE:
            if (US_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Decrement_Comm_US_CountDown();
                US_Sch_Info.State = GET_MODEM_CHANNEL;
            }
            break;
        case GET_MODEM_CHANNEL:
            if (MODEM_A_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
            {
                MODEM_A_CHANNEL_CONTROL = GRAB_MODEM_CHANNEL;
                if (MODEM_A_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
                {
                    US_Sch_Info.State = SET_MODEM_TX_MODE;
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
                        MODEM_A_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
                        US_Sch_Info.Timeout_ms = (UINT16)(COMM_RE_SYNCHRONIZATION_TIME);
                        US_Sch_Info.State = COMM_SCHEDULER_IDLE;
                    }
                }
                break;
            }
            if (MODEM_A_CHANNEL_STATUS != MODEM_CHANNEL_FREE)
            {
                US_Sch_Info.Timeout_ms = (UINT16) (COMM_RE_SYNCHRONIZATION_TIME);
                US_Sch_Info.State = COMM_SCHEDULER_IDLE;
            }
            break;
        case SET_MODEM_TX_MODE:
            if(DIP_Switch_Info.COM1_Mode == (comm_type)FSK_COMMUNICATION)
            {
              if(MODEM_A_CD == FALSE)
               {
                MODEM_A_M1 = SET_LOW;
                MODEM_A_M0 = SET_HIGH;

                Clear_COM1_Errors();
              }
                Clear_US_Transmit_Buffer();
                Build_US_Message();
                US_Sch_Info.State = WAIT_FOR_DATA_ENCRYPT;
                US_Sch_Info.Timeout_ms = TRANSMIT_WAIT_TIME;
                Start_US_Data_Encryption();
            }
            else
            {
                Clear_US_Transmit_Buffer();
                Build_US_Message();
                US_Sch_Info.State = WAIT_FOR_DATA_ENCRYPT;
                US_Sch_Info.Timeout_ms = TRANSMIT_WAIT_TIME;
                Start_US_Data_Encryption();
            }
            break;
        case WAIT_FOR_DATA_ENCRYPT:
            Update_US_Data_Encode();
            uchEncipher_State = Get_US_Encryption_State();
            if(uchEncipher_State == (BYTE)ENCRYPTION_COMLETED)
            {
             Set_US_Encryption_Idle();
             US_Sch_Info.State = WAIT_FOR_CARRIER_STABALISE;
            }
            if (US_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Set_US_Sch_Idle();
            }
            break;
        case WAIT_FOR_CARRIER_STABALISE:
            if (US_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                /* Carrier Stabilisation time of 50 ms go now for transmission */
                US_Sch_Info.State  = TRANSMIT_TELEGRAM;
                US_Sch_Info.Timeout_ms = Com1XmitObject.Transmission_Time;
            }
            break;
        case TRANSMIT_TELEGRAM:
            if (Com1XmitObject.Index >= COMM_MESSAGE_LENGTH)
            {
                if(U1STAbits.TRMT == 1)
                {
                    US_Sch_Info.State      = HOLD_MODEM_RS_LOW;
                    US_Sch_Info.Timeout_ms = MODEM_HOLD_RS_LOW_TIMEOUT;
                }   
                break;
            }

            if(U1STAbits.UTXEN == 0)
            {
                U1STAbits.UTXEN = 1;
            }

            if (U1STAbits.UTXBF == 0)
            {
                U1TXREG = Com1XmitObject.Msg_Buffer[Com1XmitObject.Index];
                Com1XmitObject.Index = Com1XmitObject.Index + 1;
            }
            if (US_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Set_US_Sch_Idle();
            }
            break;
        case HOLD_MODEM_RS_LOW:
            if (US_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Set_US_Sch_Idle();
            }
            break;
        case ERROR_MODEM_NO_CARRIER:
            break;
    }

}
/***************************************************************************
Component name      :COMM_US
Module Name         :void Receive_COM1_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Checks for Overrrun error, framing error and Recieves the data.


Design Requirements     :

Interfaces
    Calls           :   CRC16.c -   Crc16()

    Called by       :   COMM_US.c   -   Update_US_Sch_State()

Input Variables         Name                                    Type
    Global          :   OERR                                    RCSTA1 control register bit
                        FERR                                    RCSTA1 control register bit
                        RC1IF                                   Peripheral Interrupt register bit
                        COM_RECV_BUFFER_EMPTY                   Enumerator
                        COM_RECV_SRC_ADDR                       Enumerator
                        Com1RecvObject.Index                    BYTE
                        COM_RECV_DATA_ID                        Enumerator
                        COM_RECV_DATA_BYTES                     Enumerator
                        COM_CHECK_CRC16                         Enumerator
                        Com1RecvObject.State                    Enumerator
                        COM_INVALID_MESSAGE                     Enumerator
                        Status.Flags.Modem_A_Err_Toggle_Bit     Bit
                        COM_VALID_MESSAGE                       Enumerator

    Local           :   Crc16_Return_Value                      UINT16
                        uchData                                 BYTE

Output Variables        Name                                    Type
    Global          :   TXEN                                    TXSTA1 control register bit
                        CREN                                    RCSTA1 control register bit
                        RCREG1                                  USART1 receiver buffer register
                        Com1RecvObject.Timeout_ms               UINT16
                        Com1ecvObject.Msg_Buffer                Array of BYTES
                        Com1RecvObject.Index                    BYTE
                        Com1RecvObject.State                    Enumerator
                        Status.Flags.Modem_A_Err_Toggle_Bit     Bit


    Local           :   None
Signal Variables

                                MODEM_A_CD                          Input                   RE6

Macro definitions used:     Macro                           Value
                            SET_LOW                         0
                            SET_HIGH                        1
                            COM_INVALID_MESSAGE_TIMEOUT     18
                            COMM_MESSAGE_LENGTH             12
                            TIMEOUT_EVENT                   0

References          :

Derived Requirements:

Algorithm           :1.Check for overrun error, if it is there clear the error
                     2.Check for Framing error, if it is there clear the error
                     3.Until reciever buffer become empty load the recieved data to
                        reciever message buffer
                     4.Check for communiction CRC, if it invalid set the recieve
                        message state to INVALID_MESSAGE
                     5.Check for recieve message state EMPTY,if message is not in
                        expected format/incomplete, probably it is out of SYNC
                     6.Check for invalid message and check for Carrier Detection,
                        if it is not generated release the buffer

**************************************************************************/
void Clear_COM1_Errors(void)
{
    if(U1STAbits.FERR == 1)
    {
        U1STAbits.FERR = 0;
    }
    if(U2STAbits.OERR == 1)
    {
        U1STAbits.OERR = 0;
    }
    if(U1STAbits.PERR == 1)
    {
        U1STAbits.PERR = 0;
//        uchData = U1RXREG;
//        while(U1STAbits.URXDA)
//        {
//            uchData = U1RXREG;
//        }
//        U1MODEbits.UARTEN = 0;
//        Nop();
//        Nop();
//        U1MODEbits.UARTEN = 1;
    }
}


void Receive_COM1_Message(void)
{
    UINT16 Crc16_Return_Value = 0;
    BYTE   uchData =0;
    BYTE uchDecipher_State;

    if (MODEM_A_CD == 1)
    {
        Clear_COM1_Errors();
        //U2MODEbits.UARTEN = 1;
    }


    if(U1STAbits.URXDA == 1)
    {
        uchData = (BYTE)U1RXREG;
        Com1RecvObject.Timeout_ms = COM_INVALID_MESSAGE_TIMEOUT;
        switch (Com1RecvObject.State)
        {
            case COM_RECV_BUFFER_EMPTY:
                Com1RecvObject.Msg_Buffer[0] = uchData;
                Com1RecvObject.Index = 1;
                Com1RecvObject.State = (BYTE)COM_RECV_SRC_ADDR;
                break;
            case COM_RECV_SRC_ADDR:
                Com1RecvObject.Msg_Buffer[Com1RecvObject.Index] = uchData;
                Com1RecvObject.Index = Com1RecvObject.Index + 1;
                Com1RecvObject.State = (BYTE)COM_RECV_DATA_ID;
                break;
            case COM_RECV_DATA_ID:
                Com1RecvObject.Msg_Buffer[Com1RecvObject.Index] = uchData;
                Com1RecvObject.Index = Com1RecvObject.Index + 1;
                Com1RecvObject.State = (BYTE)COM_RECV_DATA_BYTES;
                break;
            case COM_RECV_DATA_BYTES:
                Com1RecvObject.Msg_Buffer[Com1RecvObject.Index] = uchData;
                Com1RecvObject.Index = Com1RecvObject.Index + 1;
                if (Com1RecvObject.Index >= COMM_MESSAGE_LENGTH )
                {
                    Com1RecvObject.State = (BYTE)COM_DECRYPT_DATA_BYTES;
                    Start_US_Data_Decryption();
                }
                break;
            default:
                break;
            }
    }

    if (Com1RecvObject.State == (BYTE)COM_DECRYPT_DATA_BYTES)
    {
      Update_US_Data_Decode();
      uchDecipher_State = Get_US_Decryption_State();
        if(uchDecipher_State == (BYTE)DECRYPTION_COMPLETED)
        {
         Set_US_Decryption_Idle();
         Com1RecvObject.State = (BYTE)COM_CHECK_CRC16;
        }

    }
    if (Com1RecvObject.State == (BYTE)COM_CHECK_CRC16)
    {

        Crc16_Return_Value = Crc16(COM1_RX,COMM_MESSAGE_LENGTH);
        if (Crc16_Return_Value != 0)
        {
            Com1RecvObject.State = (BYTE)COM_INVALID_MESSAGE;
            Status.Flags.Modem_A_Err_Toggle_Bit = !(Status.Flags.Modem_A_Err_Toggle_Bit);
            COM_US_pkt_error_cnt++;
            return;     /* Invalid CRC-16 Checksum */
        }
        Com1RecvObject.State = (BYTE)COM_VALID_MESSAGE;
        //COM_US_pkt_error_cnt = 0;
        return;
    }
    if (Com1RecvObject.State != (BYTE)COM_RECV_BUFFER_EMPTY)
    {
        /* Message is not in expected format/incomplete, probably it is out of SYNC */
        if (Com1RecvObject.Timeout_ms == TIMEOUT_EVENT)
        {
        //    COM_US_pkt_error_cnt++;
            Com1RecvObject.State = (BYTE)COM_RECV_BUFFER_EMPTY;
        }
    }
    if (Com1RecvObject.State == (BYTE)COM_INVALID_MESSAGE)
    {
        /*
         * Message can be invalid for following reasons:
         *  - Reception out of Sync.
         *  - probably it is not addressed to DAC
         *  - Corrupted message
         */
        //COM_US_pkt_error_cnt++;
        /* No Carrier Detected, so clear the Buffer */
        Com1RecvObject.State = (BYTE)COM_RECV_BUFFER_EMPTY;
    }
}
/***************************************************************************
Component name      :COMM_US
Module Name         :void Set_US_Sch_Idle(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Make the Up stream schedular ready for Next transmission

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   COMM_US.c   -   Update_US_Sch_State()

Input Variables         Name                            Type
    Global          :   COMM_SCHEDULER_IDLE             Enumerator
                        US_Sch_Info.ScanPeriod          UINT16
                        US_Sch_Info.ElapsedTime         UINT16

    Local           :   uiTmp                           UINT16

Output Variables        Name                            Type
    Global          :   US_Sch_Info.State               Enumerator
                        US_Sch_Info.Timeout_ms          UINT16

    Local           :   None
Signal Variables

                            MODEM_A_M0                          Output                  LATE5
                            MODEM_A_CHANNEL_CONTROL             Output                  LATB2

Macro definitions used:     Macro                           Value
                            SET_HIGH                            1
                            RELEASE_MODEM_CHANNEL               0

References          :

Derived Requirements:

Algorithm           :1.Set the upstream schedular state as idle
                     2.Calculate the  schedular timeout
                     3.Release the modem channel
*****************************************************************************/
void Set_US_Sch_Idle(void)
{
    UINT16 uiTmp =0;

        US_Sch_Info.State = COMM_SCHEDULER_IDLE;
        if(US_Sch_Info.ElapsedTime > US_Sch_Info.ScanPeriod)
        {
            uiTmp = (US_Sch_Info.ElapsedTime % COMM_SCHEDULER_SCAN_RATE);
        }
        else
        {
            uiTmp = US_Sch_Info.ElapsedTime;
        }
        US_Sch_Info.Timeout_ms = US_Sch_Info.ScanPeriod - uiTmp;
        if(DIP_Switch_Info.COM1_Mode == (comm_type)FSK_COMMUNICATION)
        {
         MODEM_A_M0 = SET_LOW;
         MODEM_A_M1 = SET_HIGH;
        }
        MODEM_A_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
}
/*************************************************************************
Component name      :COMM_US
Module Name         :void Decrement_US_Sch_msTmr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Decrement the US schedular timer

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Start_CF_Reset_Sequence()
                        DAC_MAIN.c  -   Start_EF_Reset_Sequence()
                        DAC_MAIN.c  -   Control_DAC_Type_CF()
                        DAC_MAIN.c  -   Control_DAC_Type_EF()
                        DAC_MAIN.c  -   Control_DAC_Type_D3()

Input Variables             Name                        Type
    Global          :   US_Sch_Info.Timeout_ms          UINT16
                        US_Sch_Info.State               Enumerator
                        COMM_SCHEDULER_IDLE             Enumerator
                        Com1RecvObject.Timeout_ms       UINT16

    Local           :   None

Output Variables            Name                        Type
    Global          :   US_Sch_Info.Timeout_ms          UINT16
                        US_Sch_Info.ElapsedTime         UINT16
                        Com1RecvObject.Timeout_ms       UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            None

References          :

Derived Requirements:

Algorithm           :1.Whenever the 1ms timer overflows decreament the upstream schedular timeout value by 1
                        if it is greater than zero
                     2.Increament the elapsed time if the schedular is not in the idle state otherwise set the elapsed time as zero
                     3.Decreament the communication receving timeout value if it is greater than zero
**************************************************************************/
void Decrement_US_Sch_msTmr(void)
{
    US_LED_Error_Service_1mS();
    US_Packet_error_timmer();
    
    if (US_Sch_Info.Timeout_ms > 0)
    {
        US_Sch_Info.Timeout_ms = US_Sch_Info.Timeout_ms - 1;
    }
    if (US_Sch_Info.State == COMM_SCHEDULER_IDLE)
    {
        US_Sch_Info.ElapsedTime = 0;
    }
    else
    {
        US_Sch_Info.ElapsedTime = US_Sch_Info.ElapsedTime + 1;
    }
    if (Com1RecvObject.Timeout_ms > 0)
    {
        Com1RecvObject.Timeout_ms = Com1RecvObject.Timeout_ms - 1;
    }
    if(Comm_Modem_A.Timeout_ms > 0)
    {
     Comm_Modem_A.Timeout_ms = Comm_Modem_A.Timeout_ms - 1;
    }
}
/*****************************************************************************
Component name      :COMM_US
Module Name         :void Decrement_Comm_US_CountDown(void)
Created By          :
Date Created        :
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

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   COMM_US.c   -   Update_US_Sch_State()

Input Variables         Name                                Type
    Global          :   DIP_Switch_Info.Flags.Is_CPU1       Bit
                        Comm_A_CountDown.US1_to_LU1         BYTE
                        Comm_A_CountDown.US2_to_LU1         BYTE
                        Comm_A_CountDown.US1_to_LU2         BYTE
                        Comm_A_CountDown.US2_to_LU2         BYTE

    Local           :   None

Output Variables        Name                                Type
    Global          :   Comm_A_CountDown.US1_to_LU1         BYTE
                        Comm_A_CountDown.US2_to_LU1         BYTE
                        Comm_A_CountDown.US1_to_LU2         BYTE
                        Comm_A_CountDown.US2_to_LU2         BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            TRUE                            1
References          :

Derived Requirements:

Algorithm           :1.Decreament the communication A countdown value of LU1 if the value is greater than zero and the CPU is 1
                     2.Decreament the communication A countdown value of LU2 if the value is greater than zero and the CPU is 2
********************************************************************************/
void Decrement_Comm_US_CountDown(void)
{
    if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == TRUE)
    {
        if (Comm_A_CountDown.US1_to_LU1 > 0)
        {
            Comm_A_CountDown.US1_to_LU1 = Comm_A_CountDown.US1_to_LU1 - 1;
        }
        if (Comm_A_CountDown.US2_to_LU1 > 0)
        {
            Comm_A_CountDown.US2_to_LU1 = Comm_A_CountDown.US2_to_LU1 - 1;
        }
    }
    else
    {
        if (Comm_A_CountDown.US1_to_LU2 > 0)
        {
            Comm_A_CountDown.US1_to_LU2 = Comm_A_CountDown.US1_to_LU2 - 1;
        }
        if (Comm_A_CountDown.US2_to_LU2 > 0)
        {
            Comm_A_CountDown.US2_to_LU2 = Comm_A_CountDown.US2_to_LU2 - 1;
        }
    }
}
/**********************************************************************************
Component name      :COMM_US
Module Name         :void Build_US_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :In reset mode calls Build_US_Reset_Info_Message(),normal mode calls
                     Build_US_Axle_Count_Message(), checks for overrun and framing error.


Design Requirements     :



Interfaces
    Calls           :   COMM_US.c   -   Build_US_Reset_Info_Message()
                        COMM_US.c   -   Build_US_Axle_Count_Message()

    Called by       :   COMM_US.c   -   Update_US_Sch_State()

Input Variables         Name                                Type
    Global          :   Status.Flags.Preparatory_Reset      Bit
                        RCREG1                              USART1 receiver buffer register
                        OERR                                RCSTA1 control register bit
                        FERR                                RCSTA1 control register bit

    Local           :   uchData                         BYTE

Output Variables        Name                            Type
    Global          :   TXEN                            TXSTA1 control register bit
                        CREN                            RCSTA1 control register bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                        PREPARATORY_RESET_PENDING           0
                        SET_LOW                             0
                        SET_HIGH                            1
References          :

Derived Requirements:

Algorithm           :1.Build the US reset message if the system is in reset mode
                     2.Build the axle count message if the system comes to pilot mode.
                     3.Disable and enable the transmission and reception once the overrun or framing error occurs.
**********************************************************************************/
void Build_US_Message(void)
{
    BYTE uchData =0;


    if (Status.Flags.Preparatory_Reset_US == PREPARATORY_RESET_PENDING)
    {
        Build_US_Reset_Info_Message();
    }
    else
    {
        Build_US_Axle_Count_Message();
    }

    if(U1STAbits.PERR == 1)
    {
        U1STAbits.PERR = 0;
    }

    if (U1STAbits.OERR)
    {
        /* Overrun Error! Clear the error */
        U1STAbits.OERR = 0;
        //U1STAbits.UTXEN = SET_LOW;
        //U1STAbits.UTXEN = SET_HIGH;
    }
    if (U1STAbits.FERR)
    {
        /* Framing Error! Clear the error */
        uchData = (BYTE)U1RXREG;
        U1STAbits.FERR = 0;
        //U1STAbits.UTXEN   = SET_LOW;
        //U1STAbits.UTXEN = SET_HIGH;
    }
    if(IFS4bits.U1ERIF == 1)
    {
        IFS4bits.U1ERIF = 0;
    }
}
/*****************************************************************************
Component name      :COMM_US
Module Name         :void Build_US_Reset_Info_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Builds the reset transmit message buffer to transmit to
                     other unit CPUs


Design Requirements     :



Interfaces
    Calls           :   CRC16.c     - Crc16()
                        RESTORE.c   - Get_Calculated_Checksum()

    Called by       :   COMM_US.c   -   Build_US_Message()

Input Variables         Name                                             Type
    Global          :   DAC_UNIT_TYPE_D3_A                               Enumerator
                        DIP_Switch_Info.DAC_Unit_Type                        DAC_Unit_Type
                        DIP_Switch_Info.Address                          BYTE
                        DIP_Switch_Info.Flags.Is_CPU1                    Bit
                        US_Telegram_No                                   BYTE
                        Status.Flags.Local_Power_Status                  Bit
                        Status.Flags.Local_Reset_Done                    Bit
                        Status.Flags.US1_to_LU1_Link                     Bit
                        Status.Flags.US2_to_LU1_Link                     Bit
                        Status.Flags.US1_to_LU2_Link                     Bit
                        Status.Flags.US2_to_LU2_Link                     Bit
                        Status.Flags.System_Status                       Bit
                        Status.Flags.US_System_Status                    Bit
                        Status.Flags.US_Track_Status                     Bit
                        Status.Flags.DS_Track_Status                     Bit
                        Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok Bit

    Local           :   Flags                                       bitadrb_t
                        CheckSum                                    wordtype_t
                        Calculated_Checksum                         longtype_t
                        Flags.Byte                                  BYTE
                        CheckSum.Byte.Lo                            BYTE
                        CheckSum.Byte.Hi                            BYTE
                        Calculated_Checksum.DWord.HiWord.Byte.Hi    BYTE
                        Calculated_Checksum.DWord.HiWord.Byte.Lo    BYTE
                        Calculated_Checksum.DWord.LoWord.Byte.Hi    BYTE
                        Calculated_Checksum.DWord.LoWord.Byte.Lo    BYTE

Output Variables        Name                                Type
    Global          :   Com1XmitObject.Msg_Buffer           Array of BYTEs
                        Com1XmitObject.Msg_Length           BYTE
                        Com1XmitObject.Index                BYTE

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            READ_RESET_INFO                 0xAA
                            SET_HIGH                        1
                            TRUE                            1
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
                        into the Com1 Transmit buffer inorder to send
******************************************************************************/
void Build_US_Reset_Info_Message(void)
{
    bitadrb_t Flags;
    wordtype_t CheckSum;
    longtype_t Calculated_Checksum;

    Flags.Byte = (BYTE) 0;
    /* Byte 1 Destination Address */
    if( DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_SF)
    {
        /* In 3 D Configuration  D3-A unit will communicate with D3-C by US Communication. In this Case
           US comm addrss is higher than Local Address */
        Com1XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address + 4);
    }
    else if( DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A)
    {
        /* In 3 D Configuration  D3-A unit will communicate with D3-C by US Communication. In this Case
           US comm addrss is higher than Local Address */
        Com1XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address + 6);
    }
    else
    {
        Com1XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address - 2);
    }
    /* Byte 2 Source Address */
    Com1XmitObject.Msg_Buffer[COM_SRC_ADDR_FIELD_OFFSET] = DIP_Switch_Info.Address;
    /* Byte 3 Command */
    Com1XmitObject.Msg_Buffer[COM_DATA_ID_FIELD_OFFSET]  = READ_RESET_INFO;
    /* Byte 4 Configuration details */
    Flags.Byte   = (BYTE)DIP_Switch_Info.DAC_Unit_Type;               /* DAC Type */
    Flags.Bit.b5 = SET_LOW;                             /* Message over COM1 */
    Flags.Bit.b4 = DIP_Switch_Info.Flags.Is_DAC_CPU1;       /* CPU1 or CPU2 */

    Com1XmitObject.Msg_Buffer[COM_DAC_CONFIG_OFFSET] = Flags.Byte;
    Flags.Byte = 0;
    Flags.Bit.b0 = Status.Flags.Local_Power_Status;
//    if(Checksum_Info.CF.Flags.US_Checksum_Comparison_Ok == TRUE)
//       {
         Flags.Bit.b1 = Status.Flags.Local_Reset_Done;
//       }
    Flags.Bit.b2 = Status.Flags.US1_to_LU1_Link;
    Flags.Bit.b3 = Status.Flags.US2_to_LU1_Link;
    Flags.Bit.b4 = Status.Flags.US1_to_LU2_Link;
    Flags.Bit.b5 = Status.Flags.US2_to_LU2_Link;
    Flags.Bit.b6 = Status.Flags.Local_Reset_Done;
    Com1XmitObject.Msg_Buffer[COM_FLAGS1_FIELD_OFFSET] = Flags.Byte;
    Flags.Byte = 0;
    Flags.Bit.b0 = Status.Flags.System_Status;
    Flags.Bit.b1 = Status.Flags.US_System_Status;
    Flags.Bit.b2 = Status.Flags.US_Track_Status;
    Flags.Bit.b3 = Status.Flags.DS_Track_Status;
    if(DIP_Switch_Info.Flags.ATC_Enabled == TRUE)
    {
      Flags.Bit.b4 = SET_HIGH;
    }
    Com1XmitObject.Msg_Buffer[COM_FLAGS2_FIELD_OFFSET] = Flags.Byte;
    Calculated_Checksum.LWord = Get_Calculated_Checksum();
    Com1XmitObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET] =  Calculated_Checksum.DWord.HiWord.Byte.Hi ;
    Com1XmitObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET] =  Calculated_Checksum.DWord.HiWord.Byte.Lo ;
    Com1XmitObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET] =  Calculated_Checksum.DWord.LoWord.Byte.Hi ;
    Com1XmitObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET] =  Calculated_Checksum.DWord.LoWord.Byte.Lo ;
    Com1XmitObject.Msg_Buffer[14] = 0;
    Com1XmitObject.Msg_Buffer[15] = 0;
    Com1XmitObject.Msg_Buffer[16] = 0;
    Com1XmitObject.Msg_Buffer[17] = 0;
    CheckSum.Word = Crc16(COM1_TX, 18); 
    Com1XmitObject.Msg_Buffer[18] = CheckSum.Byte.Lo;
    Com1XmitObject.Msg_Buffer[19] = CheckSum.Byte.Hi;
    Com1XmitObject.Msg_Length = (BYTE) COMM_MESSAGE_LENGTH;
    Com1XmitObject.Index = 0;
}
/******************************************************************************
Component name      :COMM_US
Module Name         :void Build_US_Axle_Count_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Build the axle message buffer for transmission to other unit CPUs


Design Requirements     :


Interfaces
    Calls           :   AXLE_MON.c  -   Get_US_AxleDirection()
                        RLYA_MGR.c  -   Get_Relay_A_ATC_State()
                        AXLE_MON.c  -   Get_US_Fwd_AxleCount()
                        AXLE_MON.c  -   Get_US_Rev_AxleCount()
                        CRC16.c     -   Crc16()

    Called by       :   COMM_US.c   -   Build_US_Message()

Input Variables         Name                                Type
    Global          :   DAC_UNIT_TYPE_D3_A                  Enumerator
                        DIP_Switch_Info.DAC_Unit_Type           DAC_Unit_Type
                        DIP_Switch_Info.Address             BYTE
                        DIP_Switch_Info.Flags.Is_CPU1       Bit
                        DS_Telegram_No                      BYTE
                        Status.Flags.Local_Power_Status     Bit
                        Status.Flags.Local_Reset_Done       Bit
                        Status.Flags.US1_to_LU1_Link        Bit
                        Status.Flags.US2_to_LU1_Link        Bit
                        Status.Flags.US1_to_LU2_Link        Bit
                        Status.Flags.US2_to_LU2_Link        Bit
                        Status.Flags.System_Status          Bit
                        Status.Flags.US_System_Status       Bit
                        Status.Flags.US_Track_Status        Bit
                        Status.Flags.DS_Track_Status        Bit
                        DIP_Switch_Info.Flags.ATC_Enabled   Bit
                        Status.Flags.Preparatory_Reset_US   Bit

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
    Global          :   Com1XmitObject.Msg_Buffer           Array of BYTEs
                        Com1XmitObject.Msg_Length           BYTE
                        Com1XmitObject.Index                BYTE

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
                            COM_AXLE_COUNT_LO_OFFSET        10
                            COM_AXLE_COUNT_HI_OFFSET        11
References          :

Derived Requirements:

Algorithm           :1.Load the destination CPU address, sending CPU address,message id,unit type,CPU id,
                        Telegram number,reset information,system status flags,axle count messages,16bit CRC
                        into the Com1 Transmit buffer inorder to send
****************************************************************************/
void Build_US_Axle_Count_Message(void)
{
    bitadrb_t Flags,Buffer;
    wordtype_t Fwd_AxleCount;
    wordtype_t Rev_AxleCount;
    wordtype_t AxleCount;
    wordtype_t CheckSum;

    Flags.Byte = (BYTE) 0;
    /* Byte 1 Destination Address */
    if( DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_3D_SF)
    {
        /* In 3 D Configuration  D3-A unit will communicate with D3-C by US Communication. In this Case
           US comm addrss is higher than Local Address */
         Com1XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address +4);
    }
    else if( DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A)
    {
        /* In 3 D Configuration  D3-A unit will communicate with D3-C by US Communication. In this Case
           US comm addrss is higher than Local Address */
         Com1XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address +6);
    }
    else
    {
         Com1XmitObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET] = (BYTE) (DIP_Switch_Info.Address -2);
    }
    /* Byte 2 Source Address */
    Com1XmitObject.Msg_Buffer[COM_SRC_ADDR_FIELD_OFFSET] = DIP_Switch_Info.Address;
    /* Byte 3 Command */
    Com1XmitObject.Msg_Buffer[COM_DATA_ID_FIELD_OFFSET] = READ_AXLE_COUNT;
    /* Byte 4 Configuration details */
    Flags.Byte = 0;
    Flags.Byte = (BYTE)DIP_Switch_Info.DAC_Unit_Type;             /* DAC Type */
    Flags.Bit.b5 = SET_LOW;                             /* Message over COM1 */
    Flags.Bit.b4 = DIP_Switch_Info.Flags.Is_DAC_CPU1;       /* CPU1 or CPU2 */

    Com1XmitObject.Msg_Buffer[COM_DAC_CONFIG_OFFSET] = Flags.Byte;
    Flags.Byte  = 0;
    Flags.Bit.b0 = Status.Flags.Local_Power_Status;
    Flags.Bit.b1 = Status.Flags.Local_Reset_Done;
    Flags.Bit.b2 = Status.Flags.US1_to_LU1_Link;
    Flags.Bit.b3 = Status.Flags.US2_to_LU1_Link;
    Flags.Bit.b4 = Status.Flags.US1_to_LU2_Link;
    Flags.Bit.b5 = Status.Flags.US2_to_LU2_Link;
    Buffer.Byte  = Get_US_AxleDirection();
    Flags.Bit.b6 = Buffer.Bit.b0;
    Flags.Bit.b7 = Buffer.Bit.b1;
    Com1XmitObject.Msg_Buffer[COM_FLAGS1_FIELD_OFFSET] = Flags.Byte;
    Flags.Byte = 0;
    Flags.Bit.b0 = Status.Flags.System_Status;
    Flags.Bit.b1 = Status.Flags.US_System_Status;
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
      else if((DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A)||
         (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B)||
         (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C)||
         (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D))
        {
         Flags.Bit.b4 = (BYTE) Get_Relay_D4_ATC_State();
        }
      else
        {
         Flags.Bit.b4 = (BYTE) Get_Relay_A_ATC_State();
        }
    }
    Flags.Bit.b5 = Status.Flags.Preparatory_Reset_US;
    Com1XmitObject.Msg_Buffer[COM_FLAGS2_FIELD_OFFSET] = Flags.Byte;
    Fwd_AxleCount.Word = Get_US_Fwd_AxleCount();        /* from axlemon.c */
    Rev_AxleCount.Word = Get_US_Rev_AxleCount();        /* from axlemon.c */
    Com1XmitObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
    Com1XmitObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
    Com1XmitObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
    Com1XmitObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;
    AxleCount.Word = Get_US_AxleCount();                /* from axlemon.c */
    Com1XmitObject.Msg_Buffer[COM_AXLE_COUNT_LO_OFFSET] = AxleCount.Byte.Lo;
    Com1XmitObject.Msg_Buffer[COM_AXLE_COUNT_HI_OFFSET] = AxleCount.Byte.Hi;
    Com1XmitObject.Msg_Buffer[12] = 0;
    Com1XmitObject.Msg_Buffer[13] = 0;
    Com1XmitObject.Msg_Buffer[14] = 0;
    Com1XmitObject.Msg_Buffer[15] = 0;
    Com1XmitObject.Msg_Buffer[16] = 0;
    Com1XmitObject.Msg_Buffer[17] = 0;
    Com1XmitObject.Msg_Buffer[18] = 0;
    Com1XmitObject.Msg_Buffer[19] = 0;
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
            //US -dst address is D, so send B's data
            //Always send CPU2 data
            Fwd_AxleCount.Word = Relay_D4_Info.B2_IN_Count;
            Rev_AxleCount.Word = Relay_D4_Info.B2_OUT_Count;        
            Com1XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
            Com1XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
            Com1XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
            Com1XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;            
            break;
        case DAC_UNIT_TYPE_D4_B: 
            //US -dst address is A, so send C's data
            //Always send CPU2 data
            Fwd_AxleCount.Word = Relay_D4_Info.C2_IN_Count;
            Rev_AxleCount.Word = Relay_D4_Info.C2_OUT_Count;        
            Com1XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
            Com1XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
            Com1XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
            Com1XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;            
            break;
        case DAC_UNIT_TYPE_D4_C:
            //US -dst address is B, so send D's data
            //Always send CPU1 data
            Fwd_AxleCount.Word = Relay_D4_Info.D2_IN_Count;
            Rev_AxleCount.Word = Relay_D4_Info.D2_OUT_Count;        
            Com1XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
            Com1XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
            Com1XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
            Com1XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;   
            break;
        case DAC_UNIT_TYPE_D4_D:
            //US -dst address is C, so send A's data
            //Always send CPU1 data
            Fwd_AxleCount.Word = Relay_D4_Info.A2_IN_Count;
            Rev_AxleCount.Word = Relay_D4_Info.A2_OUT_Count;        
            Com1XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET] = Fwd_AxleCount.Byte.Lo;
            Com1XmitObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET] = Fwd_AxleCount.Byte.Hi;
            Com1XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET] = Rev_AxleCount.Byte.Lo;
            Com1XmitObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET] = Rev_AxleCount.Byte.Hi;   
            break;
        default:
            break;
    }
    CheckSum.Word = Crc16(COM1_TX, 20);
    Com1XmitObject.Msg_Buffer[20] = CheckSum.Byte.Lo;
    Com1XmitObject.Msg_Buffer[21] = CheckSum.Byte.Hi;
    Com1XmitObject.Msg_Length = (BYTE) COMM_MESSAGE_LENGTH;
    Com1XmitObject.Index = 0;
}
/****************************************************************************
Component name      :COMM_US
Module Name         :void Process_US_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :If command is for Axle count calls Process_US_Axle_Count_message()
                     else calls Process_US_Reset_Info_Message()


Design Requirements     :


Interfaces
    Calls           :   COMM_US.C   -   Update_US_Sch_State()

    Called by       :   COMM_US.c   -   Process_US_Reset_Info_Message()
                        COMM_US.c   -   Process_US_Axle_Count_Message()

Input Variables         Name                        Type
    Global          :   Com1RecvObject.Msg_Buffer   BYTE

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

Algorithm           :1.Call the Process US Axle count message if the message id is 0x55
                     2.Call the Process US Reset info message if the message id is 0xAA
********************************************************************************/
void Process_US_Message(void)
{
    BYTE uchCommand;

    uchCommand = Com1RecvObject.Msg_Buffer[COM_DATA_ID_FIELD_OFFSET];

    if (uchCommand == READ_AXLE_COUNT)
    {
        Process_US_Axle_Count_Message();
    }
    if (uchCommand == READ_RESET_INFO)
    {
        Process_US_Reset_Info_Message();
    }
}
/******************************************************************************
Component name      :COMM_US
Module Name         :void Process_US_Reset_Info_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Process the Up Stream reset massage


Design Requirements     :

Interfaces
    Calls           :   RELAYMGR.c  -   Declare_DAC_Defective_US()
                        ERROR.c     -   Set_Error_Status_Bit()
                        SYS_MON.c   -   Update_US_Power_Status()
                        SYS_MON.c   -   Update_US_Track_Status()
                        COMM_US.c   -   Synchronise_US_Sch()
                        COMM_US.c   -   Reset_US_CountDown()
                        RESET.c     -   Post_US1_has_been_Reset()
                        RESET.c     -   Post_US2_has_been_Reset()
                        RESTORE.c   -   Compare_US_Checksum()
                        RESET.c     -   Update_US_Section_Remote_Reset()

    Called by       :   COMM_US.c   -   Process_US_Message()

Input Variables         Name                            Type
    Global          :   DIP_Switch_Info.Address         BYTE
                        DIP_Switch_Info.Peer_Address    BYTE
                        WAITING_FOR_RESET_MODE          Enumerator
                        SYSTEM_ERROR_MODE               Enumerator
                        DAC_UNIT_TYPE_CF                Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator

    Local           :   SrcMsgAdr                       bitadrb_t
                        DestMsgAdr                      bitadrb_t
                        DAC_Config                      bitadrb_t
                        Flags1                          bitadrb_t
                        Flags2                          bitadrb_t
                        DAC_Config.Bit.b3               Bit
                        DAC_Config.Bit.b4               Bit
                        Flags1.Bit.b1                   Bit

Output Variables        Name                                                    Type
    Global          :   Checksum_Info.CF.US1_Checksum.DWord.HiWord.Byte.Hi      BYTE
                        Checksum_Info.CF.US1_Checksum.DWord.HiWord.Byte.Lo      BYTE
                        Checksum_Info.CF.US1_Checksum.DWord.LoWord.Byte.Hi      BYTE
                        Checksum_Info.CF.US1_Checksum.DWord.LoWord.Byte.Lo      BYTE
                        Checksum_Info.CF.US2_Checksum.DWord.HiWord.Byte.Hi      BYTE
                        Checksum_Info.CF.US2_Checksum.DWord.HiWord.Byte.Lo      BYTE
                        Checksum_Info.CF.US2_Checksum.DWord.LoWord.Byte.Hi      BYTE
                        Checksum_Info.CF.US2_Checksum.DWord.LoWord.Byte.Lo      BYTE
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
                            SET_HIGH                        1
                            SM_HAS_RESETTED_SYSTEM          1


References          :

Derived Requirements:

Algorithm           :1.Get the configuration details, source address,destination
                        address, FLAG1 and FLAG2
                     2.Check the destination address with the CPU address and Peer
                        CPU address, if both are not equal message does belong to
                        our network declare DAC defective and set Invalid configuration
                        error ID
                     3.Check for source address, if it is zero, message does belong to
                        our network declare DAC defective and set Invalid configuration
                        error ID
                     4.Check for both source and destination address if both are equal
                        declare DAC defective and set Invalid configuration error ID
                     5.Check whether message is coming from COM1 port of remote unit,
                        if not declare DAC defective and set Invalid configuration
                        error ID
                     6.Update the US power status, Track status,Sychronise the US shedular
                     7.According to the status of the remote unit update the US section remote
                        unit mode inorder to display in the reset box
                     8.update the reset in CPU1 or CPU2
                     9.Update the remote unit US1 and US2 checksum and compare it with the calculated checksum


******************************************************************************/
void Process_US_Reset_Info_Message(void)
{
    bitadrb_t SrcMsgAdr,DestMsgAdr;
    bitadrb_t DAC_Config, Flags1, Flags2;

    DAC_Config.Byte = Com1RecvObject.Msg_Buffer[COM_DAC_CONFIG_OFFSET];
    SrcMsgAdr.Byte  = Com1RecvObject.Msg_Buffer[COM_SRC_ADDR_FIELD_OFFSET];
    DestMsgAdr.Byte = Com1RecvObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET];
    Flags1.Byte     = Com1RecvObject.Msg_Buffer[COM_FLAGS1_FIELD_OFFSET];
    Flags2.Byte     = Com1RecvObject.Msg_Buffer[COM_FLAGS2_FIELD_OFFSET];

    if ((DIP_Switch_Info.Address != DestMsgAdr.Byte)&&
        (DIP_Switch_Info.Peer_Address != DestMsgAdr.Byte))
    {
        Declare_DAC_Defective_US();
        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
        return;         /* Message does not belong to our network */
    }
    if (SrcMsgAdr.Byte == 0)
    {
        SPI_Failure.fail_bits.US_Failure = 1;
        Declare_DAC_Defective_US();
        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
        return;         /* Message does not belong to our network */
    }
    if (DestMsgAdr.Byte == SrcMsgAdr.Byte)
    {
        SPI_Failure.fail_bits.US_Failure = 1;
        Declare_DAC_Defective_US();
        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
        return;         /* Source address and Destination addresses are same */
    }
    if (DAC_Config.Bit.b5 != SET_HIGH)
    {
        SPI_Failure.fail_bits.US_Failure = 1;
        Declare_DAC_Defective_US();
        Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
        return;         /*  Message should come from COM1 port of remote unit */
    }
    if(DIP_Switch_Info.Flags.ATC_Enabled == TRUE)
    {
        if(Flags2.Bit.b4 == FALSE)
        {
            SPI_Failure.fail_bits.US_Failure = 1;
            Declare_DAC_Defective_US();
            Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
            return;         /* In Remote Unit Auto Clear Bit is not set */
        }
    }
    Update_US_Power_Status(DAC_Config,Flags1);
    Update_US_Track_Status(Flags2);
    Synchronise_US_Sch(SrcMsgAdr);
    Reset_US_CountDown(SrcMsgAdr,Flags1);
     if(Flags2.Bit.b1 != NORMAL )
       {
            SPI_Failure.fail_bits.US_Failure = 1;
            US_Section_Mode.Remote_Unit = SYSTEM_ERROR_MODE;
       }
     else
     {
         SPI_Failure.fail_bits.US_Failure = 0;
     }
     if(Flags1.Bit.b6 == SM_HAS_RESETTED_SYSTEM)
       {
            Update_US_Section_Remote_Reset();
       }
     else
       {
            US_Section_Mode.Remote_Unit = WAITING_FOR_RESET_MODE;
       }
    if (DAC_Config.Bit.b4)
    {
        /* Sender is CPU1 */
        Post_US1_has_been_Reset((BYTE) Flags1.Bit.b1);
        Checksum_Info.CF.US1_Checksum.DWord.HiWord.Byte.Hi = Com1RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET];
        Checksum_Info.CF.US1_Checksum.DWord.HiWord.Byte.Lo = Com1RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET];
        Checksum_Info.CF.US1_Checksum.DWord.LoWord.Byte.Hi = Com1RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET];
        Checksum_Info.CF.US1_Checksum.DWord.LoWord.Byte.Lo = Com1RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET];
     }
    else
    {
        /* Sender is CPU2 */
        Post_US2_has_been_Reset((BYTE) Flags1.Bit.b1);
        Checksum_Info.CF.US2_Checksum.DWord.HiWord.Byte.Hi = Com1RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET];
        Checksum_Info.CF.US2_Checksum.DWord.HiWord.Byte.Lo = Com1RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET];
        Checksum_Info.CF.US2_Checksum.DWord.LoWord.Byte.Hi = Com1RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET];
        Checksum_Info.CF.US2_Checksum.DWord.LoWord.Byte.Lo = Com1RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET];
     }
//    Compare_US_Checksum(DAC_Config);
}
/*************************************************************************
Component name      :COMM_US
Module Name         :void Process_US_Axle_Count_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Process the Down Stream axle count message


Design Requirements     :


Interfaces
    Calls           :   COMM_US.c   -   Synchronise_US_Sch()
                        COMM_US.c   -   Reset_US_CountDown()
                        RLYA_MGR.c  -   Get_Relay_A_State()
                        RESET.c     -   Post_US1_has_been_Reset()
                        RESET.c     -   Post_US2_has_been_Reset()
                        RLYA_MGR.c  -   Update_ATC_Remote1_Relay_A_State()
                        RLYB_MGR.c  -   Update_ATC_Remote2_Relay_A_State()
                        SYS_MON.c   -   Update_US_Power_Status()
                        SYS_MON.c   -   Update_DS_Track_Status()
                        COMM_US.c   -   Check_US_Unit_Status()
                        COMM_US.c   -   Process_US_AxleCount()
                        COMM_US.c   -   Process_US_Direction()
                        RLYB_MGR.c  -   Update_Previous_Block_State()
                        RLYD3_MGR.c -   Process_D3_Remote_AxleCount()
                        RLYD3_MGR.c -   Process_D3_Remote_Direction()
                        RESET.c     -   Update_US_Section_Remote_Reset()
                        RESET.c     -   Update_US_Section_Remote_Preparatory()

    Called by       :   COMM_US.c   -   Process_US_Message()

Input Variables         Name                            Type
    Global          :   Com1RecvObject.Msg_Buffer       Array of BYTES
                        DIP_Switch_Info.Address         BYTE
                        DIP_Switch_Info.Peer_Address    BYTE
                        DAC_RESET_PROGRESS              Enumerator
                        Status.Flags.Preparatory_Reset  Enumerator
                        ATC_WAIT_FOR_REMOTE_CLEAR       Enumerator
                        Status.Flags.US_System_Status   Bit
                        DIP_Switch_Info.DAC_Unit_Type       DAC_Unit_Type
                        DAC_UNIT_TYPE_FF                Enumerator
                        DAC_UNIT_TYPE_CF                Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_D3_A              Enumerator
                        DAC_UNIT_TYPE_D3_B              Enumerator
                        DAC_UNIT_TYPE_D3_C              Enumerator

    Local           :   SrcMsgAdr                       bitadrb_t
                        DestMsgAdr                      bitadrb_t
                        Buffer                          bitadrb_t
                        DAC_Config                      bitadrb_t
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
                            PREPARATORY_RESET_COMPLETED     1
                            SM_HAS_RESETTED_SYSTEM          1

References          :

Derived Requirements:

Algorithm           :1.Get the configuration details, source address,destination
                        address, FLAG1 and FLAG2
                     2.Check the destination address with the CPU address and Peer
                        CPU address, if both are not equal then message does not belong to
                        our network,declare DAC defective and set Invalid configuration
                        error ID
                     3.Check for source address, if it is zero, message does not belong
                        to our network declare DAC defective and set Invalid
                        configuration error ID
                     4.Check for both source and destination address if both are equal
                        declare DAC defective and set Invalid configuration error ID
                     5.Check whether message is coming from COM1 port of remote unit,
                        if not declare DAC defective and set Invalid configuration
                        error ID
                     6.Sychronise the US shedular, update the reset in CPU1 or CPU2
                     7.Update the reset applied status and pilot condition and system status of the source CPU
                     8.Update the US power status,if local system failed, So no need
                        to Process the axle counts of remote unit
                     9.Update the forward count,reverse count and axle count of the source CPU
                     10.According to configuration Process the axle count and direction
                        and update the US track status, update the next block status


*******************************************************************************/
void Process_US_Axle_Count_Message(void)
{
    wordtype_t Fwd_CountValue;
    wordtype_t Rev_CountValue;
    wordtype_t R_Fwd_CountValue;
    wordtype_t R_Rev_CountValue;
    wordtype_t Axle_CountValue;
    bitadrb_t  SrcMsgAdr,DestMsgAdr,Buffer;
    bitadrb_t  DAC_Config,Flags1,Flags2;
    BYTE uchDirection;
    BYTE uchState =0;


    DAC_Config.Byte = Com1RecvObject.Msg_Buffer[COM_DAC_CONFIG_OFFSET];
    SrcMsgAdr.Byte  = Com1RecvObject.Msg_Buffer[COM_SRC_ADDR_FIELD_OFFSET];
    DestMsgAdr.Byte = Com1RecvObject.Msg_Buffer[COM_DEST_ADDR_FIELD_OFFSET];
    Flags1.Byte     = Com1RecvObject.Msg_Buffer[COM_FLAGS1_FIELD_OFFSET];
    Flags2.Byte     = Com1RecvObject.Msg_Buffer[COM_FLAGS2_FIELD_OFFSET];

    if ((DIP_Switch_Info.Address != DestMsgAdr.Byte)&&
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
    if (DAC_Config.Bit.b5 != SET_HIGH)
    {
        return;         /*  Message should come from COM1 port of remote unit */
    }

    Synchronise_US_Sch(SrcMsgAdr);
    Reset_US_CountDown(SrcMsgAdr,Flags1);
    if(Flags1.Bit.b1 == SM_HAS_RESETTED_SYSTEM)
       {
         if(DIP_Switch_Info.DAC_Unit_Type <= DAC_UNIT_TYPE_CF || DIP_Switch_Info.DAC_Unit_Type >= DAC_UNIT_TYPE_3D_SF)
           {
            Update_US_Section_Remote_Reset();
           }
       }
    if(US_Section_Mode.Flags.Remote_Preparatory_State == FALSE)
      {
       if(Flags2.Bit.b5 == PREPARATORY_RESET_COMPLETED)
        {
         if(DIP_Switch_Info.DAC_Unit_Type <= DAC_UNIT_TYPE_CF || DIP_Switch_Info.DAC_Unit_Type >= DAC_UNIT_TYPE_3D_SF)
           {
            Update_US_Section_Remote_Preparatory();
           }
        }
      }
    if(Flags2.Bit.b1 != NORMAL )
       {
        if(DIP_Switch_Info.DAC_Unit_Type <= DAC_UNIT_TYPE_CF || DIP_Switch_Info.DAC_Unit_Type >= DAC_UNIT_TYPE_3D_SF)
          {
            US_Section_Mode.Remote_Unit = SYSTEM_ERROR_MODE;
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
         (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C)||
         (DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D))
        {
         uchState = Get_Relay_D4_State();
        }
    else
        {
        uchState = Get_Relay_A_State();
        }
    if (Status.Flags.Preparatory_Reset_US == PREPARATORY_RESET_PENDING ||
        uchState == (BYTE)DAC_RESET_PROGRESS)
    {
            if (DAC_Config.Bit.b4)
            {
            /* Sender is CPU1 */
                Post_US1_has_been_Reset((BYTE) Flags1.Bit.b1);
            }
            else
            {
            /* Sender is CPU2 */
                Post_US2_has_been_Reset((BYTE) Flags1.Bit.b1);
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
                Update_ATC_Remote3_Relay_D3_State((BYTE) Flags2.Bit.b4);
                }
            else
                {
                /* Sender is CPU2 */
                Update_ATC_Remote4_Relay_D3_State((BYTE) Flags2.Bit.b4);
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
                Update_ATC_Remote3_Relay_D4_State((BYTE) Flags2.Bit.b4);
                }
            else
                {
                /* Sender is CPU2 */
                Update_ATC_Remote4_Relay_D4_State((BYTE) Flags2.Bit.b4);
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
                Update_ATC_Remote1_Relay_A_State((BYTE) Flags2.Bit.b4);
            }
            else
            {
                /* Sender is CPU2 */
                Update_ATC_Remote2_Relay_A_State((BYTE) Flags2.Bit.b4);
            }
        }
     }
    Update_US_Power_Status(DAC_Config,Flags1);
    Update_US_Track_Status(Flags2);

    if(Status.Flags.US_System_Status != NORMAL)
    {
    /* Already local system failed, So no need to Process the axle counts of remote unit */
       return;
    }
    Check_US_Unit_Status(Flags2);

     /*
     * Process the Axle Count
     */
    Fwd_CountValue.Byte.Lo  = Com1RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_LO_OFFSET];
    Fwd_CountValue.Byte.Hi  = Com1RecvObject.Msg_Buffer[COM_FWD_AXLE_COUNT_HI_OFFSET];
    Rev_CountValue.Byte.Lo  = Com1RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_LO_OFFSET];
    Rev_CountValue.Byte.Hi  = Com1RecvObject.Msg_Buffer[COM_REV_AXLE_COUNT_HI_OFFSET];
    R_Fwd_CountValue.Byte.Lo  = Com1RecvObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_LO_OFFSET];
    R_Fwd_CountValue.Byte.Hi  = Com1RecvObject.Msg_Buffer[COM_R_FWD_AXLE_COUNT_HI_OFFSET];
    R_Rev_CountValue.Byte.Lo  = Com1RecvObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_LO_OFFSET];
    R_Rev_CountValue.Byte.Hi  = Com1RecvObject.Msg_Buffer[COM_R_REV_AXLE_COUNT_HI_OFFSET];

    Axle_CountValue.Byte.Lo = Com1RecvObject.Msg_Buffer[COM_AXLE_COUNT_LO_OFFSET];
    Axle_CountValue.Byte.Hi = Com1RecvObject.Msg_Buffer[COM_AXLE_COUNT_HI_OFFSET];



    Update_SM_US_Remote_Axle_Counts(Axle_CountValue.Byte.Lo,Axle_CountValue.Byte.Hi);

    uchDirection = (Flags1.Byte >> 6);

    switch(DIP_Switch_Info.DAC_Unit_Type)
    {
            case DAC_UNIT_TYPE_EF:
                if(SrcMsgAdr.Byte < DestMsgAdr.Byte)
                {
                    Process_US_AxleCount(SrcMsgAdr,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_US_Direction(SrcMsgAdr,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_CF:
                if(SrcMsgAdr.Byte < DestMsgAdr.Byte)
                {
                    Process_US_AxleCount(SrcMsgAdr,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_US_Direction(SrcMsgAdr,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_D3_A:
                if(SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Buffer.Byte = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Process_D3_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D3_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_D3_B:
                if(SrcMsgAdr.Byte < DestMsgAdr.Byte)
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
                if(SrcMsgAdr.Byte < DestMsgAdr.Byte)
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
                if(SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Buffer.Byte = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Buffer.Bit.b3 = DAC_Config.Bit.b3;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                    //A will recv C remote data and will always recieve CPU1 data
                    SrcMsgAdr.Byte = 1;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,DAC_UNIT_TYPE_D4_C,R_Rev_CountValue.Word,R_Fwd_CountValue.Word);                
                    //Axle Count
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_C,(BYTE)FORWARD_DIRECTION,R_Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_C,(BYTE)REVERSE_DIRECTION,R_Fwd_CountValue.Word);                
                }
                break;
            case DAC_UNIT_TYPE_D4_B:
                if(SrcMsgAdr.Byte < DestMsgAdr.Byte)
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Buffer.Bit.b3 = DAC_Config.Bit.b3;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                    //B will recv D remote data and will always recieve CPU1 data
                    SrcMsgAdr.Byte = 1;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,DAC_UNIT_TYPE_D4_D,R_Rev_CountValue.Word,R_Fwd_CountValue.Word);                
                    //Axle Count
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_D,(BYTE)FORWARD_DIRECTION,R_Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_D,(BYTE)REVERSE_DIRECTION,R_Fwd_CountValue.Word);                
                }
                break;
            case DAC_UNIT_TYPE_D4_C:
                if(SrcMsgAdr.Byte < DestMsgAdr.Byte)
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Buffer.Bit.b3 = DAC_Config.Bit.b3;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                    //C will recv A remote data and will always recieve CPU1 data
                    SrcMsgAdr.Byte = 1;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,DAC_UNIT_TYPE_D4_A,R_Fwd_CountValue.Word,R_Rev_CountValue.Word);                
                    //Axle Count
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_A,(BYTE)FORWARD_DIRECTION,R_Fwd_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_A,(BYTE)REVERSE_DIRECTION,R_Rev_CountValue.Word);                
                }
                break;
            case DAC_UNIT_TYPE_D4_D:
                if(SrcMsgAdr.Byte < DestMsgAdr.Byte)
                {
                    Buffer.Byte   = (BYTE) 0;
                    Buffer.Bit.b0 = DAC_Config.Bit.b0;
                    Buffer.Bit.b1 = DAC_Config.Bit.b1;
                    Buffer.Bit.b2 = DAC_Config.Bit.b2;
                    Buffer.Bit.b3 = DAC_Config.Bit.b3;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,(SSDAC_Unit_Type)Buffer.Byte,uchDirection,Axle_CountValue.Word);
                    //D will recv B remote data and will always recieve CPU1 data
                    SrcMsgAdr.Byte = 1;
                    Process_D4_Remote_AxleCount(SrcMsgAdr,DAC_UNIT_TYPE_D4_B,R_Rev_CountValue.Word,R_Fwd_CountValue.Word);                
                    //Axle Count
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_B,(BYTE)FORWARD_DIRECTION,R_Rev_CountValue.Word);
                    Process_D4_Remote_Direction(SrcMsgAdr,DAC_UNIT_TYPE_D4_B,(BYTE)REVERSE_DIRECTION,R_Fwd_CountValue.Word);                
                }
                break;
            case DAC_UNIT_TYPE_3D_SF:
                if(SrcMsgAdr.Byte > DestMsgAdr.Byte)
                {
                    Process_3S_US_AxleCount(SrcMsgAdr,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_US_Direction(SrcMsgAdr,uchDirection,Axle_CountValue.Word);
                }
                break;
            case DAC_UNIT_TYPE_3D_EF:
                if(SrcMsgAdr.Byte < DestMsgAdr.Byte)
                {
                    Process_3S_US_AxleCount(SrcMsgAdr,Fwd_CountValue.Word,Rev_CountValue.Word);
                    Process_US_Direction(SrcMsgAdr,uchDirection,Axle_CountValue.Word);
                }
                break;
            default:
                break;
    }

}
/****************************************************************************
Component name      :COMM_US
Module Name         :void Process_US_AxleCount(bitadrb_t SrcAdr, BYTE uchDirection, UINT16 uiAxleCount)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the forward and reverse axle counts in Up stream
                     unit CPU1 or in CPU2

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RLYA_MGR.c  -   Update_A_US1_IN_Count()
                        RLYA_MGR.c  -   Update_A_US1_OUT_Count()
                        RLYA_MGR.c  -   Update_B_US2_IN_Count()
                        RLYA_MGR.c  -   Update_B_US2_OUT_Count()

    Called by       :   COMM_US.c   -   Process_US_Axle_Count_Message()

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

Algorithm           :1.Update the upstream CPU1,CPU2 IN and OUT count
**************************************************************************/
void Process_US_AxleCount(bitadrb_t SrcAdr,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)

{
    if (SrcAdr.Bit.b0 == SET_HIGH)
    {
      Update_A_US1_IN_Count(uiFwdAxleCount);
      Update_A_US1_OUT_Count(uiRevAxleCount);
    }
    else
    {
      Update_A_US2_IN_Count (uiFwdAxleCount);
      Update_A_US2_OUT_Count(uiRevAxleCount);
    }

}


//15_03_10

void Process_3S_US_AxleCount(bitadrb_t SrcAdr,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
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
              Update_A_US1_IN_Count(uiRevAxleCount);
              Update_A_US1_OUT_Count(uiFwdAxleCount);
             }
            else
              {
               Update_A_US2_IN_Count (uiRevAxleCount);
               Update_A_US2_OUT_Count(uiFwdAxleCount);
              }
             break;
         case DAC_UNIT_TYPE_3D_EF:
             if (SrcAdr.Bit.b0 == SET_HIGH)
             {
              Update_A_US1_IN_Count(uiFwdAxleCount);
              Update_A_US1_OUT_Count(uiRevAxleCount);
            }
            else
            {
              Update_A_US2_IN_Count (uiFwdAxleCount);
              Update_A_US2_OUT_Count(uiRevAxleCount);
            }
             break;
        }

}
/****************************************************************************
Component name      :COMM_US
Module Name         :void Process_US_Direction(bitadrb_t SrcAdr, BYTE uchDirection)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update direction and axle counts in Up stream unit
                     CPU1 or CPU2

Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   RLYA_MGR.c  -   Update_A_US1_Direction()
                        RLYA_MGR.c  -   Update_A_US2_Direction()

    Called by       :   COMM_US.c   -   Process_US_Axle_Count_Message()

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

Algorithm           :1.Update the Upstream CPU1,CPU2 axle direction and axle count value
*******************************************************************************/
void Process_US_Direction(bitadrb_t SrcAdr, BYTE uchDirection,UINT16 axle_Count)
{
    if (SrcAdr.Bit.b0 == SET_HIGH)
    {
        Update_A_US1_Direction(uchDirection);
        Update_A_US1_AxleCount(axle_Count);
    }
    else
    {
        Update_A_US2_Direction(uchDirection);
        Update_A_US2_AxleCount(axle_Count);
    }
}
/*******************************************************************************
Component name      :COMM_US
Module Name         :void Synchronise_US_Sch(bitadrb_t SrcAdr)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Sychronise the communication scheduler to prevent the
                     communication line from always being busy.

Allocated Requirements  :

Design Requirements     :



Interfaces
    Calls           :   Nil

    Called by       :   COMM_US.c   -   Process_US_Reset_Info_Message()
                        COMM_US.c   -   Process_US_Axle_Count_Message()

Input Variables         Name                                Type
    Global          :   COMM_SCHEDULER_IDLE                 Enumerator
                        US_Sch_Info.State                   Enumerator
                        DIP_Switch_Info.Flags.Is_CPU1       Bit
                        uiCOM1_BalanceTicks_Table           UINT16

    Local           :   SrcAdr                              bitadrb_t
                        Config                              bitadrb_t
                        uchRow                              BYTE
                        uchCol                              BYTE
                        SrcAdr.Bit.b0                       Bit
                        Config.Bit.b5                       Bit

Output Variables        Name                                Type
    Global          :   US_Sch_Info.Timeout_ms              UINT16

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            TRUE                            1
                            SET_HIGH                        1
                            SET_LOW                         1


References          :

Derived Requirements:

Algorithm           :1.Check the Upstream schedular state and if it is idle then return immediately
                     2.Select the row value as per the CPU flag
                     3.Select the column value as per the received telegram number
                     4.Load the Schedular timeout value by reading the Balanceticks table
***********************************************************************/
void Synchronise_US_Sch(bitadrb_t SrcAdr)
{
    BYTE uchRow;
    BYTE uchCol;
    /*
     * Synchronise in order to prevent communication line from always being busy.
     *
     * Message sequence is RU1-LU1-RU2-LU2........
     */
    if (US_Sch_Info.State != COMM_SCHEDULER_IDLE)
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

    /* Remote Unit */
    if (SrcAdr.Bit.b0 == SET_HIGH)
    {

     uchCol = 0;        /* Remote Unit - CPU1  Telegram  received*/
    }
    else
    {
     uchCol = 1;        /* Remote Unit - CPU2  Telegram  received*/
    }
    US_Sch_Info.Timeout_ms = uiCOM1_BalanceTicks_Table[uchRow][uchCol];
}
/******************************************************************************
Component name      :COMM_US
Module Name         :void Reset_US_CountDown(bitadrb_t SrcAdr,bitadrb_t MsgFlags)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Resets communication link status for local and remote units

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   COMM_US.c   -   Process_US_Reset_Info_Message()
                        COMM_US.c   -   Process_US_Axle_Count_Message()

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
    Global          :   Comm_A_CountDown.US1_to_LU1         BYTE
                        Status.Flags.US1_to_LU1_Link        Bit
                        Status.Flags.LU1_to_US1_Link        Bit
                        Status.Flags.LU2_to_US1_Link        Bit
                        Comm_A_CountDown.US2_to_LU1         BYTE
                        Status.Flags.US2_to_LU1_Link        Bit
                        Status.Flags.LU1_to_US2_Link        Bit
                        Status.Flags.LU2_to_US2_Link        Bit
                        Comm_A_CountDown.US1_to_LU2         BYTE
                        Status.Flags.US1_to_LU2_Link        Bit
                        Status.Flags.LU1_to_US1_Link        Bit
                        Status.Flags.LU2_to_US1_Link        Bit
                        Comm_B_CountDown.US2_to_LU2         BYTE
                        Status.Flags.US2_to_LU2_Link        Bit
                        Status.Flags.LU1_to_US2_Link        Bit
                        Status.Flags.LU2_to_US2_Link        Bit

    Local           :   None

Signal Variables

                                    Nil                         Nil                 Nil

Macro definitions used:     Macro                           Value
                            SET_HIGH                        1
                            MAXIMUM_COM_RETRIES             3
                            COMMUNICATION_OK                1
References          :

Derived Requirements:

Algorithm           :1.Check the message is from CPU1 or CPU2
                     2.Load the communication retries value and set the comm. link as OK
******************************************************************************/
void Reset_US_CountDown(bitadrb_t SrcAdr,bitadrb_t MsgFlags)
{
    if (DIP_Switch_Info.Flags.Is_DAC_CPU1)
    {
        /* Local Unit - CPU1 */
        if (SrcAdr.Bit.b0 == SET_HIGH)
        {
            /* From Remote Unit - CPU1 */
            Comm_A_CountDown.US1_to_LU1  = MAXIMUM_COM_RETRIES;
            Status.Flags.US1_to_LU1_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_US1_Link =  MsgFlags.Bit.b2;
            Status.Flags.LU2_to_US1_Link =  MsgFlags.Bit.b3;
        }
        else
        {
            /* From Remote Unit - CPU2 */
            Comm_A_CountDown.US2_to_LU1  = MAXIMUM_COM_RETRIES;
            Status.Flags.US2_to_LU1_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_US2_Link =  MsgFlags.Bit.b4;
            Status.Flags.LU2_to_US2_Link =  MsgFlags.Bit.b5;
        }
    }
    else
    {
        /* Local Unit - CPU2 */
        if (SrcAdr.Bit.b0 == SET_HIGH)
        {
            /* From Remote Unit - CPU1 */
            Comm_A_CountDown.US1_to_LU2  = MAXIMUM_COM_RETRIES;
            Status.Flags.US1_to_LU2_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_US1_Link =  MsgFlags.Bit.b2;
            Status.Flags.LU2_to_US1_Link =  MsgFlags.Bit.b3;
        }
        else
        {
            /* From Remote Unit - CPU2 */
            Comm_A_CountDown.US2_to_LU2  = MAXIMUM_COM_RETRIES;
            Status.Flags.US2_to_LU2_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_US2_Link =  MsgFlags.Bit.b4;
            Status.Flags.LU2_to_US2_Link =  MsgFlags.Bit.b5;
        }
    }
}
/******************************************************************************
Component name      :COMM_US
Module Name         :void Check_US_Unit_Status(bitadrb_t Buffer)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Checks for the Up Stream system status,if any error declare it as
                     DAC defective .Downside DAC failed so drop the relay
                     and set US_ERROR_NUM error bit


Design Requirements     :


Interfaces
    Calls           :   RELAYMGR.c  -   Declare_DAC_Defective_US()
                        ERROR.c     -   Set_Error_Status_Bit()

    Called by       :   COMM_US..c  -   Process_US_Axle_Count_Message()

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
                            US_ERROR_NUM                27


References          :

Derived Requirements:

Algorithm           :1.Check the Upstream system status flag
                     2.If the flag value is LOW then Declare DAC Deective US and set the error status bit
                        "FAILURE AT US" Id
******************************************************************************/
void Check_US_Unit_Status(bitadrb_t Buffer)
{
    if(Buffer.Bit.b0 == 0 || Buffer.Bit.b1 == 0 )
    {
        /*Downside DAC failed so drop the relay */
        Declare_DAC_Defective_US();
        Set_Error_Status_Bit(US_ERROR_NUM);
    }
}


void Configure_Modem_A (void)
{
 if(DIP_Switch_Info.COM1_Mode == (comm_type)FSK_COMMUNICATION)
   {
     switch(Comm_Modem_A.State)
        {
         case MODEM_IDLE:
            if(Comm_Modem_A.Timeout_ms == 0)
             {
              Comm_Modem_A.State = GET_MODEM_LINE;
             }
            break;
         case GET_MODEM_LINE:
            if (MODEM_A_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
             {
              MODEM_A_CHANNEL_CONTROL = GRAB_MODEM_CHANNEL;
                if(MODEM_A_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
                 {
                   MODEM_A_M1 = SET_HIGH;
                   MODEM_A_M0 = SET_HIGH;
                   Comm_Modem_A.State = SET_MODEM_ZP_MODE;
                   Comm_Modem_A.Timeout_ms = 20;

                 }
               }
              else
                {
                 if (DIP_Switch_Info.Flags.Is_DAC_CPU1 == FALSE)
                 {
                  MODEM_A_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
                  Comm_Modem_A.State = MODEM_IDLE;
                  Comm_Modem_A.Timeout_ms = 30;
                 }
                }
             break;
         case SET_MODEM_ZP_MODE:
            if(Comm_Modem_A.Timeout_ms == 0)
            {
             MODEM_A_M1 = SET_HIGH;
             MODEM_A_M0 = SET_LOW;
             Comm_Modem_A.State = DELAY_FOR_CONFIGURE;
             MODEM_A_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
             Comm_Modem_A.Timeout_ms = 50;
            }
             break;
         case DELAY_FOR_CONFIGURE:
            if(Comm_Modem_A.Timeout_ms == 0)
            {
             Comm_Modem_A.State = CONFIGURATION_COMPLETED;
            }
             break;
         case CONFIGURATION_COMPLETED:
             break;
         }
   }
   else
    {
     if(Comm_Modem_A.State != CONFIGURATION_COMPLETED)
     {
      MODEM_A_M1 = SET_HIGH;
      MODEM_A_M0 = SET_HIGH;
      Comm_Modem_A.State = CONFIGURATION_COMPLETED;
      Comm_Modem_A.Timeout_ms = 30;
     }
    }
}



BYTE Get_Modem_A_State(void)
{
    BYTE State;
    State = (BYTE) Comm_Modem_A.State;
  return(State);
}



void Clear_US_Transmit_Buffer(void)
{
 BYTE count;
 for(count = 0; count < COMM_MESSAGE_LENGTH; count++)
 {
    Com1XmitObject.Msg_Buffer[count] = 0x00;
 }
}

