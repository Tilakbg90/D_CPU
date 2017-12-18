/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   COMM1_FDP
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


extern  /*near*/  dac_status_t Status;                        /* from DAC_MAIN.c */
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;          /* from DAC_MAIN.c */
extern checksum_info_t Checksum_Info;                   /* from DAC_MAIN.c */


const UINT16 FDPCOM1_BalanceTicks_Table[2] = {100,450};
static BYTE FDP_Sequence_No = 0;
static BYTE MDP_Previous_Seq_No = 255;
static BYTE MDP_Computed_Seq_No = 0;
BYTE MDP_Message_Sequence_Missed = 0;
BYTE MDP_Address = 0;

comm_sch_info_t FDP_Com1_Sch_Info;

comm_a_countdown_t FDP_Comm1_CountDown =
            {MAXIMUM_COM_RETRIES, MAXIMUM_COM_RETRIES,
             MAXIMUM_COM_RETRIES, MAXIMUM_COM_RETRIES};

msg_info_t    FDP_Com1XmitObject;   /* FDP-COM1: Message Transmission Buffer etc., */
msg_info_t    FDP_Com1RecvObject;   /* FDP-COM1: Message Receiving Buffer etc., */
fdp_info FDP_Info;
modem_info_t  FDP_Comm_Modem_A;

void Receive_FDP_COM1_Message(void);
void Process_FDP_COM1_Message(void);
extern void Process_US_AxleCount(bitadrb_t,UINT16,UINT16);
extern void Process_US_Direction(bitadrb_t,BYTE,UINT16);
void Decrement_FDP_Comm1_CountDown(void);
void Set_FDP_Comm1_Sch_Idle(void);
void Build_FDP_COM1_Message(void);
void Synchronise_FDP_Sch(void);
void Reset_FDP_CountDown(bitadrb_t);
extern void Check_US_Unit_Status(bitadrb_t);
void Process_FDP_Command(const BYTE);

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
void Setup_FDP_COM1BaudRate(void)
{
  if(DIP_Switch_Info.COM1_Mode == FSK_COMMUNICATION)
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
Module Name         :void Initialise_Comm1_Sch(void)
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
void Initialise_Comm1_Sch(void)
{
    BYTE uchCnt =0;

    FDP_Com1XmitObject.State = NO_MSG_TO_XMIT;
    FDP_Com1XmitObject.Index = 0;
    FDP_Com1XmitObject.Msg_Length  = COMM1_MESSAGE_LENGTH;
    FDP_Com1RecvObject.State = COM_RECV_BUFFER_EMPTY;
    FDP_Com1RecvObject.Index = 0;
    FDP_Com1RecvObject.Timeout_ms = 0;
    FDP_Com1RecvObject.Consecutive_Error_Count = 0;
    FDP_Com1RecvObject.Msg_Length = COMM1_MESSAGE_LENGTH;
    FDP_Com1_Sch_Info.State = COMM_SCHEDULER_NOT_STARTED;
    MODEM_A_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
    FDP_Com1_Sch_Info.ScanPeriod = FDP_SCHEDULER_SCAN_RATE;
    for (uchCnt = 0; uchCnt < COMM1_MESSAGE_LENGTH; uchCnt++)
    {
      FDP_Com1XmitObject.Msg_Buffer[uchCnt] = (BYTE) 0;
    }
    if(DIP_Switch_Info.COM1_Mode == FSK_COMMUNICATION)
    {
     FDP_Com1XmitObject.BytePeriod = BYTE_PERIOD_9MS;
     FDP_Com1RecvObject.BytePeriod = BYTE_PERIOD_9MS;
    }
    if(DIP_Switch_Info.COM1_Mode == RS232_COMMUNICATION)
    {
     switch (DIP_Switch_Info.Baud_Rate)
      {
        case BAUDRATE_1200:
            FDP_Com1XmitObject.BytePeriod = BYTE_PERIOD_9MS;
            FDP_Com1RecvObject.BytePeriod = BYTE_PERIOD_9MS;
            break;
        case BAUDRATE_2400:
            FDP_Com1XmitObject.BytePeriod = BYTE_PERIOD_5MS;
            FDP_Com1RecvObject.BytePeriod = BYTE_PERIOD_5MS;
            break;
        case BAUDRATE_9600:
            FDP_Com1XmitObject.BytePeriod = BYTE_PERIOD_1MS;
            FDP_Com1RecvObject.BytePeriod = BYTE_PERIOD_1MS;
            break;
        case BAUDRATE_19200:
            FDP_Com1XmitObject.BytePeriod = BYTE_PERIOD_1MS;
            FDP_Com1RecvObject.BytePeriod = BYTE_PERIOD_1MS;
            break;
        }
    }
    FDP_Com1XmitObject.Transmission_Time = (FDP_Com1XmitObject.Msg_Length) * FDP_Com1XmitObject.BytePeriod;
    FDP_Comm_Modem_A.State = MODEM_IDLE;
    if(DIP_Switch_Info.Flags.Is_FDP_CPU1 == TRUE)
    {
     FDP_Comm_Modem_A.Timeout_ms = 0;
    }
    else
    {
     FDP_Comm_Modem_A.Timeout_ms = 30;
    }
    FDP_Info.Response_Command = 0;
    FDP_Info.Reply_Command = 0;
    FDP_Info.Token_Id = 0;
    FDP_Info.Previous_Token = 0;
    FDP_Info.State = WAITING_FOR_RESET;
    FDP_Info.Fwd_Axle_Count.Word = 0;
    FDP_Info.Rev_Axle_Count.Word= 0;
    FDP_Info.Xmit_Fwd_Count= 0;
    FDP_Info.Xmit_Rev_Count= 0;
    FDP_Info.Previous_Fwd_Count= 0;
    FDP_Info.Previous_Rev_Count= 0;
    FDP_Info.Current_Fwd_Count= 0;
    FDP_Info.Current_Rev_Count= 0;
}

/******************************************************************************
Component name      :COMM_US
Module Name         :void Start_Comm1_Sch(void)
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
    Global          :       FDP_Com1_Sch_Info.State                 Enumerator
                            FDP_Com1_Sch_Info.Timeout_ms            UINT16
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
void Start_Comm1_Sch(void)
{

    FDP_Com1_Sch_Info.State = COMM_SCHEDULER_NOT_STARTED;
//  FDP_Com1_Sch_Info.State = COMM_SCHEDULER_IDLE;
    FDP_Com1_Sch_Info.Timeout_ms = TIMEOUT_EVENT;
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
                        FDP_Com1_Sch_Info.State                 Enumerator
                        COMM_SCHEDULER_NOT_STARTED          Enumerator
                        COMM_SCHEDULER_IDLE                 Enumerator
                        FDP_Com1_Sch_Info.Timeout_ms                UINT16
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
                        FDP_Com1_Sch_Info.State                 Enumerator
                        FDP_Com1_Sch_Info.Timeout_ms                UINT16
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
void Update_FDP_COM1Sch_State(void)
{
    static BYTE uchCount = 0;
//  if(MODEM_A_CD == TRUE)
//  {
    Receive_FDP_COM1_Message();
//  }
    if (FDP_Com1RecvObject.State == COM_VALID_MESSAGE)
    {
        Process_FDP_COM1_Message();     /* Valid Message received, process it */
        FDP_Com1RecvObject.State = COM_RECV_BUFFER_EMPTY;
        FDP_Com1_Sch_Info.State = COMM_SCHEDULER_IDLE;
//      FDP_Com1_Sch_Info.Timeout_ms = 100;
    }

    switch (FDP_Com1_Sch_Info.State)
    {
        case WAIT_FOR_DATA_ENCRYPT:
        case COMM_SCHEDULER_NOT_STARTED:
            break;
        case COMM_SCHEDULER_IDLE:
            if (FDP_Com1_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
//              Decrement_FDP_Comm1_CountDown();
                FDP_Com1_Sch_Info.State = GET_MODEM_CHANNEL;
            }
            break;
        case GET_MODEM_CHANNEL:
            if (MODEM_A_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
            {
                MODEM_A_CHANNEL_CONTROL = GRAB_MODEM_CHANNEL;
                if (MODEM_A_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
                {
                    FDP_Com1_Sch_Info.State = SET_MODEM_TX_MODE;
//                  FDP_Com1_Sch_Info.Timeout_ms = WAIT_FOR_RS_LOW_TIME;
                }
                else
                {
                    /* Simultaneously other processor has also grabbed the channel, So release it */
                    if (DIP_Switch_Info.Flags.Is_FDP_CPU1 == FALSE)
                    {
                        /*
                         * Simultaneously other processor has also grabbed the channel.
                         * CPU2 will relenqish the channel in favour of CPU1
                          */
                        MODEM_A_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
                        FDP_Com1_Sch_Info.Timeout_ms = 450;
                        FDP_Com1_Sch_Info.State = COMM_SCHEDULER_IDLE;
                    //  FDP_Com1_Sch_Info.State = COMM_SCHEDULER_NOT_STARTED
                    }
                }
                break;
            }
            if (MODEM_A_CHANNEL_STATUS != MODEM_CHANNEL_FREE)
            {
            //  FDP_Com1_Sch_Info.Timeout_ms = (UINT16) (FDP_RE_SYNCHRONIZATION_TIME);
                FDP_Com1_Sch_Info.State = COMM_SCHEDULER_NOT_STARTED;
            }
            break;
        case SET_MODEM_TX_MODE:
            if(MODEM_A_CD == FALSE)
            {
              if(DIP_Switch_Info.COM1_Mode == FSK_COMMUNICATION)
               {
                MODEM_A_M1 = SET_LOW;
                MODEM_A_M0 = SET_HIGH;
                }
                if(FDP_Info.Token_Id < 255)
                {
                 FDP_Info.Token_Id += 1;
                }
                if(FDP_Info.Token_Id == 255)
                {
                 FDP_Info.Token_Id += 2;
                }
                Build_FDP_COM1_Message();
                FDP_Com1_Sch_Info.State = WAIT_FOR_CARRIER_STABALISE;
                FDP_Com1_Sch_Info.Timeout_ms = TRANSMIT_WAIT_TIME;
//              FDP_Com1_Sch_Info.State  = TRANSMIT_TELEGRAM;
//              FDP_Com1_Sch_Info.Timeout_ms = FDP_Com1XmitObject.Transmission_Time;
            }
            if (FDP_Com1_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
             Set_FDP_Comm1_Sch_Idle();
            }
            break;
        case WAIT_FOR_CARRIER_STABALISE:
            if (FDP_Com1_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                /* Carrier Stabilisation time of 50 ms go now for transmission */
//              FDP_Com1_Sch_Info.State  = TRANSMIT_DUMMY_BYTES;
                FDP_Com1_Sch_Info.State  = TRANSMIT_TELEGRAM;
                FDP_Com1_Sch_Info.Timeout_ms = FDP_Com1XmitObject.Transmission_Time;
            }
            break;
        case TRANSMIT_DUMMY_BYTES:
            if (U1STAbits.UTXBF == 0)
            {
                U1TXREG = 0xAA;
                uchCount = uchCount + 1;
            }
            if (FDP_Com1_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                FDP_Com1_Sch_Info.State  = TRANSMIT_TELEGRAM;
                FDP_Com1_Sch_Info.Timeout_ms = FDP_Com1XmitObject.Transmission_Time;
                uchCount = 0;
            }
            break;
        case TRANSMIT_TELEGRAM:
//          if (FDP_Com1XmitObject.Index >= COMM_MESSAGE_LENGTH)
            if (FDP_Com1XmitObject.Index >= 16)
            {
                FDP_Com1_Sch_Info.State      = HOLD_MODEM_RS_LOW;
                FDP_Com1_Sch_Info.Timeout_ms = MODEM_HOLD_RS_LOW_TIMEOUT;
                FDP_Sequence_No += 1;
                Update_FDP_Previous_Axle_Counts();
                break;
            }
            if (U1STAbits.UTXBF == 0)
            {
                U1TXREG = FDP_Com1XmitObject.Msg_Buffer[FDP_Com1XmitObject.Index];
                FDP_Com1XmitObject.Index = FDP_Com1XmitObject.Index + 1;
            }
            if (FDP_Com1_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Set_FDP_Comm1_Sch_Idle();
            }
            break;
        case HOLD_MODEM_RS_LOW:
            if (FDP_Com1_Sch_Info.Timeout_ms == TIMEOUT_EVENT)
            {
                Set_FDP_Comm1_Sch_Idle();
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
void Receive_FDP_COM1_Message(void)
{
    UINT16 Crc16_Return_Value = 0;
    BYTE   uchData =0;

    if (U1STAbits.PERR == 1)
    {
        U1STAbits.PERR = 0;
    }
    if(U1STAbits.FERR == 1)
    {
        U1STAbits.FERR = 0;
    }
    if(U1STAbits.OERR == 1)
    {
        uchData = U1RXREG;
        U1STAbits.OERR = 0;
    }

    if(IFS4bits.U1ERIF == 1)
    {
        IFS4bits.U1ERIF = 0;
    }


    while (U1STAbits.URXDA)
    {
        uchData = U1RXREG;
        FDP_Com1RecvObject.Timeout_ms = COM_INVALID_MESSAGE_TIMEOUT;
//      FDP_Com1RecvObject.Timeout_ms = 200;
        switch (FDP_Com1RecvObject.State)
        {
            case COM_RECV_BUFFER_EMPTY:
//              if(uchData == DIP_Switch_Info.Address)
//              {
                FDP_Com1RecvObject.Msg_Buffer[0] = uchData;
                FDP_Com1RecvObject.Index = 1;
                FDP_Com1RecvObject.State = COM_RECV_SRC_ADDR;
//              }
                break;
            case COM_RECV_SRC_ADDR:
//              if(uchData == DIP_Switch_Info.COM1_NW_Address)
//              {
                FDP_Com1RecvObject.Msg_Buffer[FDP_Com1RecvObject.Index] = uchData;
                FDP_Com1RecvObject.Index = FDP_Com1RecvObject.Index + 1;
                FDP_Com1RecvObject.State = COM_RECV_DATA_ID;
        //      }
                break;
            case COM_RECV_DATA_ID:
                FDP_Com1RecvObject.Msg_Buffer[FDP_Com1RecvObject.Index] = uchData;
                FDP_Com1RecvObject.Index = FDP_Com1RecvObject.Index + 1;
                FDP_Com1RecvObject.State = COM_RECV_DATA_BYTES;
                if(MODEM_A_CD != TRUE)
                {
                FDP_Com1RecvObject.State = COM_RECV_BUFFER_EMPTY;
                }
                break;
            case COM_RECV_DATA_BYTES:
                FDP_Com1RecvObject.Msg_Buffer[FDP_Com1RecvObject.Index] = uchData;
                FDP_Com1RecvObject.Index = FDP_Com1RecvObject.Index + 1;
//              if (FDP_Com1RecvObject.Index >= COMM1_MESSAGE_LENGTH )
                if (FDP_Com1RecvObject.Index >= 16 )
                {
                    FDP_Com1RecvObject.State = COM_CHECK_CRC16;
                }
                break;
        }
    }
    if (FDP_Com1RecvObject.State == COM_CHECK_CRC16)
    {

        Crc16_Return_Value = Crc16(FDP_Com1RecvObject.Msg_Buffer,COMM1_MESSAGE_LENGTH);
        if (Crc16_Return_Value != 0)
        {
            FDP_Com1RecvObject.State = COM_INVALID_MESSAGE;
            Status.Flags.Modem_A_Err_Toggle_Bit = !(Status.Flags.Modem_A_Err_Toggle_Bit);
            return;     /* Invalid CRC-16 Checksum */
        }
        FDP_Com1RecvObject.State = COM_VALID_MESSAGE;
        return;
    }
    if (FDP_Com1RecvObject.State != COM_RECV_BUFFER_EMPTY)
    {
        /* Message is not in expected format/incomplete, probably it is out of SYNC */
        if (FDP_Com1RecvObject.Timeout_ms == TIMEOUT_EVENT)
        {
            FDP_Com1RecvObject.State = COM_RECV_BUFFER_EMPTY;
        }
    }
    if (FDP_Com1RecvObject.State == COM_INVALID_MESSAGE)
    {
        /*
         * Message can be invalid for following reasons:
         *  - Reception out of Sync.
         *  - probably it is not addressed to DAC
         *  - Corrupted message
         */
            FDP_Com1RecvObject.State = COM_RECV_BUFFER_EMPTY;

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
                        FDP_Com1_Sch_Info.ScanPeriod            UINT16
                        FDP_Com1_Sch_Info.ElapsedTime           UINT16

    Local           :   uiTmp                           UINT16

Output Variables        Name                            Type
    Global          :   FDP_Com1_Sch_Info.State             Enumerator
                        FDP_Com1_Sch_Info.Timeout_ms            UINT16

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
void Set_FDP_Comm1_Sch_Idle(void)
{


//      FDP_Com1_Sch_Info.State = COMM_SCHEDULER_IDLE;
        FDP_Com1_Sch_Info.State =   COMM_SCHEDULER_NOT_STARTED;
//      if(FDP_Com1_Sch_Info.ElapsedTime > FDP_Com1_Sch_Info.ScanPeriod)
//      {
//          uiTmp = (FDP_Com1_Sch_Info.ElapsedTime % FDP_SCHEDULER_SCAN_RATE);
//      }
//      else
//      {
//          uiTmp = FDP_Com1_Sch_Info.ElapsedTime;
//      }
//      FDP_Com1_Sch_Info.Timeout_ms = FDP_Com1_Sch_Info.ScanPeriod - uiTmp;
        if(DIP_Switch_Info.COM1_Mode == FSK_COMMUNICATION)
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
    Global          :   FDP_Com1_Sch_Info.Timeout_ms            UINT16
                        FDP_Com1_Sch_Info.State             Enumerator
                        COMM_SCHEDULER_IDLE             Enumerator
                        Com1RecvObject.Timeout_ms       UINT16

    Local           :   None

Output Variables            Name                        Type
    Global          :   FDP_Com1_Sch_Info.Timeout_ms            UINT16
                        FDP_Com1_Sch_Info.ElapsedTime           UINT16
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
void Decrement_FDP_Comm1_Sch_msTmr(void)
{
    if (FDP_Com1_Sch_Info.Timeout_ms > 0)
    {
        FDP_Com1_Sch_Info.Timeout_ms = FDP_Com1_Sch_Info.Timeout_ms - 1;
    }
    if (FDP_Com1_Sch_Info.State == COMM_SCHEDULER_NOT_STARTED)
    {
        FDP_Com1_Sch_Info.ElapsedTime = 0;
    }
    else
    {
        FDP_Com1_Sch_Info.ElapsedTime = FDP_Com1_Sch_Info.ElapsedTime + 1;
    }
    if (FDP_Com1RecvObject.Timeout_ms > 0)
    {
        FDP_Com1RecvObject.Timeout_ms = FDP_Com1RecvObject.Timeout_ms - 1;
    }
    if(FDP_Comm_Modem_A.Timeout_ms > 0)
    {
     FDP_Comm_Modem_A.Timeout_ms = FDP_Comm_Modem_A.Timeout_ms - 1;
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
void Decrement_FDP_Comm1_CountDown(void)
{
    if (DIP_Switch_Info.Flags.Is_FDP_CPU1 == TRUE)
    {
        if (FDP_Comm1_CountDown.US1_to_LU1 > 0)
        {
            FDP_Comm1_CountDown.US1_to_LU1 = FDP_Comm1_CountDown.US1_to_LU1 - 1;
        }
    }
    else
    {
        if (FDP_Comm1_CountDown.US1_to_LU2 > 0)
        {
            FDP_Comm1_CountDown.US1_to_LU2 = FDP_Comm1_CountDown.US1_to_LU2 - 1;
        }
    }
}

/*****************************************************************************
Component name      :COMM1_FDP
Module Name         :void Build_FDP_COM1_Message(void)
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
void Build_FDP_COM1_Message(void)
{
    BYTE uchData =0;
    bitadrb_t Flags;
    wordtype_t CheckSum;



    Flags.Byte = (BYTE) 0;


    /* Byte 0 FDP Source Address */

    FDP_Com1XmitObject.Msg_Buffer[0] = DIP_Switch_Info.Address;

    /* Byte 1 MDP Destination Address */
    FDP_Com1XmitObject.Msg_Buffer[1] = MDP_Address;

    /* Byte 2 communication sequence number */
    FDP_Com1XmitObject.Msg_Buffer[2] = (BYTE) FDP_Sequence_No; /* FDP Sequence Number*/

    /* Byte 3  FDP Type and Network Address */
    uchData = DIP_Switch_Info.COM1_NW_Address;
    uchData = uchData << 4;
    if (DIP_Switch_Info.Flags.Is_FDP_CPU1 == TRUE)
    {
    uchData = uchData | FDP_UNIT_CPU1_ID;
    }
    else
    {
    uchData = uchData | FDP_UNIT_CPU2_ID;
    }
    FDP_Com1XmitObject.Msg_Buffer[3] = (BYTE) uchData; /* FDP Network Id in hi nibble and CPU Id in low nibble*/

    /* Byte 4 FDP Response command */
    FDP_Com1XmitObject.Msg_Buffer[4] = FDP_Info.Response_Command;

    /* Byte 5 FDP Reply command */
    FDP_Com1XmitObject.Msg_Buffer[5] = FDP_Info.Reply_Command;

    /* Byte 5 Status Flags */
    Flags.Bit.b0 = DIP_Switch_Info.Flags.Is_FDP_CPU1;       /* CPU1 or CPU2 */
    Flags.Bit.b1 = Status.Flags.System_Status;
    Flags.Bit.b2 = Status.Flags.US_System_Status;
    Flags.Bit.b3 = Status.Flags.Local_Reset_Done;
    Flags.Bit.b4 = Status.Flags.Preparatory_Reset_US;
    Flags.Bit.b5 = Status.Flags.Configuration;
    Flags.Bit.b6 = Status.Flags.Local_Power_Status;
    FDP_Com1XmitObject.Msg_Buffer[6] = Flags.Byte;

//  Flags.Byte = (BYTE) 0;
//    /* Byte 5 Comm. link Status Flags */
//  Flags.Bit.b0 = Status.Flags.US1_to_LU1_Link;
//  Flags.Bit.b1 = Status.Flags.US1_to_LU2_Link;
//  FDP_Com1XmitObject.Msg_Buffer[FDP_FLAGS2_FIELD_OFFSET] = Flags.Byte;

    FDP_Info.Fwd_Axle_Count.Word = (UINT16) FDP_Info.Xmit_Fwd_Count;
    FDP_Info.Rev_Axle_Count.Word = (UINT16) FDP_Info.Xmit_Rev_Count;
    /* Byte 7 Forward Axle count low byte */
    FDP_Com1XmitObject.Msg_Buffer[FDP_FWD_AXLE_COUNT_LO_OFFSET] =  FDP_Info.Fwd_Axle_Count.Byte.Lo;
    /* Byte 8 Forward Axle count hi byte*/
    FDP_Com1XmitObject.Msg_Buffer[FDP_FWD_AXLE_COUNT_HI_OFFSET] =  FDP_Info.Fwd_Axle_Count.Byte.Hi;
    /* Byte 9 Reverse Axle count low byte */
    FDP_Com1XmitObject.Msg_Buffer[FDP_REV_AXLE_COUNT_LO_OFFSET] =  FDP_Info.Rev_Axle_Count.Byte.Lo;
    /* Byte 10 Reverse Axle count hi byte*/
    FDP_Com1XmitObject.Msg_Buffer[FDP_REV_AXLE_COUNT_HI_OFFSET] =  FDP_Info.Rev_Axle_Count.Byte.Hi;

    /* FDP Error Code*/
    uchData = (BYTE) 0;
    uchData = Get_US_Error_Code();
    FDP_Com1XmitObject.Msg_Buffer[11] = uchData;

    FDP_Com1XmitObject.Msg_Buffer[12] = FDP_Info.Token_Id;

    //FDP_Info.Previous_Token = FDP_Info.Token_Id;

    CheckSum.Word = Crc16(FDP_Com1XmitObject.Msg_Buffer, 14);
    FDP_Com1XmitObject.Msg_Buffer[14] = CheckSum.Byte.Lo;
    FDP_Com1XmitObject.Msg_Buffer[15] = CheckSum.Byte.Hi;
    FDP_Com1XmitObject.Msg_Length = (BYTE) 16;
    FDP_Com1XmitObject.Index = 0;

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
        uchData = U1RXREG;
        U1STAbits.FERR = 0;
        //U1STAbits.UTXEN   = SET_LOW;
        //U1STAbits.UTXEN = SET_HIGH;
    }
    if(IFS4bits.U1ERIF == 1)
    {
        IFS4bits.U1ERIF = 0;
    }
}

/******************************************************************************
Component name      :COMM_US
Module Name         :void Process_FDP_COM1_Message(void)
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
void Process_FDP_COM1_Message(void)
{


    bitadrb_t SrcMsgAdr,DestMsgAdr;
    bitadrb_t Flags1;
    BYTE Command,Sequence_No,Network_Id,Board_Type;

    DestMsgAdr.Byte  = FDP_Com1RecvObject.Msg_Buffer[FDP_ID_FIELD_OFFSET];
    Network_Id       = FDP_Com1RecvObject.Msg_Buffer[FDP_NW_ID_FIELD_OFFSET];
    Sequence_No      = FDP_Com1RecvObject.Msg_Buffer[FDP_SEQUENCE_NUMNBER_OFFSET];
    SrcMsgAdr.Byte   = FDP_Com1RecvObject.Msg_Buffer[FDP_CAN_ID_OFFSET];
    Flags1.Byte      = FDP_Com1RecvObject.Msg_Buffer[FDP_FLAGS1_FIELD_OFFSET];

    if (DIP_Switch_Info.Flags.Is_FDP_CPU1 == TRUE)
    {
     Command         = FDP_Com1RecvObject.Msg_Buffer[FDP_CPU1_COMMAND_ID_OFFSET];
     FDP_Info.Previous_Token = FDP_Com1RecvObject.Msg_Buffer[FDP_CPU1_TOKEN_ID_OFFSET];
    }
    else
    {
    Command          = FDP_Com1RecvObject.Msg_Buffer[FDP_CPU2_COMMAND_ID_OFFSET];
    FDP_Info.Previous_Token = FDP_Com1RecvObject.Msg_Buffer[FDP_CPU2_TOKEN_ID_OFFSET];
    }

    Board_Type       = FDP_Com1RecvObject.Msg_Buffer[FDP_COMM_BOARD_ID_OFFSET];

//  if (DIP_Switch_Info.COM1_NW_Address != SrcMsgAdr.Byte)
//  {
//      Declare_DAC_Defective_US();
//      Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
//      return;         /* Message does not belong to our network */
//  }
//
//  if (DIP_Switch_Info.Address !=  DestMsgAdr.Byte)
//  {
//      Declare_DAC_Defective_US();
//      Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
//      return;         /* Message does not belong to our FDP */
//  }
//
//  if (SrcMsgAdr.Byte == 0)
//  {
//      Declare_DAC_Defective_US();
//      Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
//      return;         /* Network ID should not be zero */
//  }
//  if (DestMsgAdr.Byte == 0)
//
//  {
//      Declare_DAC_Defective_US();
//      Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
//      return;         /* FDP ID should not be zero */
//  }
//
//  Fwd_CountValue.Byte.Lo = FDP_Com1RecvObject.Msg_Buffer[FDP_FWD_AXLE_COUNT_LO_OFFSET];
//  Fwd_CountValue.Byte.Hi = FDP_Com1RecvObject.Msg_Buffer[FDP_FWD_AXLE_COUNT_HI_OFFSET];
//  Rev_CountValue.Byte.Lo = FDP_Com1RecvObject.Msg_Buffer[FDP_REV_AXLE_COUNT_LO_OFFSET];
//  Rev_CountValue.Byte.Hi = FDP_Com1RecvObject.Msg_Buffer[FDP_REV_AXLE_COUNT_HI_OFFSET];

    Process_FDP_Command(Command);
    MDP_Address = SrcMsgAdr.Byte;
    Synchronise_FDP_Sch();
//  Reset_FDP_CountDown(Flags2);

    Post_FDP_US_Reset(Flags1);
    Update_MDP_Sequence_Number(Sequence_No);
    Update_FDP_Axle_Counts(FDP_Info.Previous_Token);
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
//void Process_US_AxleCount(bitadrb_t SrcAdr,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
//
//{
//  if (SrcAdr.Bit.b0 == SET_HIGH)
//  {
//    Update_A_US1_IN_Count(uiFwdAxleCount);
//    Update_A_US1_OUT_Count(uiRevAxleCount);
//  }
//  else
//  {
//    Update_A_US2_IN_Count (uiFwdAxleCount);
//    Update_A_US2_OUT_Count(uiRevAxleCount);
//  }
//
//}
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
//void Process_US_Direction(bitadrb_t SrcAdr, BYTE uchDirection,UINT16 axle_Count)
//{
//  if (SrcAdr.Bit.b0 == SET_HIGH)
//  {
//      Update_A_US1_Direction(uchDirection);
//      Update_A_US1_AxleCount(axle_Count);
//  }
//  else
//  {
//      Update_A_US2_Direction(uchDirection);
//      Update_A_US2_AxleCount(axle_Count);
//  }
//}
/*******************************************************************************
Component name      :COMM_US
Module Name         :void Synchronise_FDP_Sch(void)
Created By          :
ate Created     :16/12/2009
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
                        FDP_Com1_Sch_Info.State                 Enumerator
                        DIP_Switch_Info.Flags.Is_CPU1       Bit
                        uiCOM1_BalanceTicks_Table           UINT16

    Local           :   SrcAdr                              bitadrb_t
                        Config                              bitadrb_t
                        uchRow                              BYTE
                        uchCol                              BYTE
                        SrcAdr.Bit.b0                       Bit
                        Config.Bit.b5                       Bit

Output Variables        Name                                Type
    Global          :   FDP_Com1_Sch_Info.Timeout_ms                UINT16

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
void Synchronise_FDP_Sch(void)
{

    /*
     * Synchronise in order to prevent communication line from always being busy.
     *
     * Message sequence is MDP-LU1-LU2-MDP........
     */
    if (FDP_Com1_Sch_Info.State != COMM_SCHEDULER_NOT_STARTED)
    {
        return;
    }
    if (DIP_Switch_Info.Flags.Is_FDP_CPU1 == TRUE)
    {
     FDP_Com1_Sch_Info.Timeout_ms = FDPCOM1_BalanceTicks_Table[0];
    }
    else
    {
     FDP_Com1_Sch_Info.Timeout_ms = FDPCOM1_BalanceTicks_Table[1];
    }

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
void Reset_FDP_CountDown(bitadrb_t MsgFlags)
{
    if (DIP_Switch_Info.Flags.Is_FDP_CPU1)
    {
        /* Local Unit - CPU1 */
            FDP_Comm1_CountDown.US1_to_LU1  = MAXIMUM_COM_RETRIES;
            Status.Flags.US1_to_LU1_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_US1_Link =  MsgFlags.Bit.b2;
            Status.Flags.LU2_to_US1_Link =  MsgFlags.Bit.b3;
    }
    else
    {
        /* Local Unit - CPU2 */
            FDP_Comm1_CountDown.US1_to_LU2  = MAXIMUM_COM_RETRIES;
            Status.Flags.US1_to_LU2_Link = COMMUNICATION_OK;
            Status.Flags.LU1_to_US1_Link =  MsgFlags.Bit.b2;
            Status.Flags.LU2_to_US1_Link =  MsgFlags.Bit.b3;

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
//void Check_US_Unit_Status(bitadrb_t Buffer)
//{
//  if(Buffer.Bit.b0 == 0 || Buffer.Bit.b1 == 0 )
//  {
//  /*Downside DAC failed so drop the relay */
//      Declare_DAC_Defective_US();
//  Set_Error_Status_Bit(US_ERROR_NUM);
//  }
//}

void Configure_FDP_Modem_A (void)
{
     switch(FDP_Comm_Modem_A.State)
        {
         case GET_MODEM_LINE:
             break;
         case MODEM_IDLE:
            if(FDP_Comm_Modem_A.Timeout_ms == 0)
             {
            if (MODEM_A_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
             {
              MODEM_A_CHANNEL_CONTROL = GRAB_MODEM_CHANNEL;
                if(MODEM_A_CHANNEL_STATUS == MODEM_CHANNEL_FREE)
                 {
                   MODEM_A_M1 = SET_HIGH;
                   MODEM_A_M0 = SET_HIGH;
                   FDP_Comm_Modem_A.State = SET_MODEM_ZP_MODE;
                   FDP_Comm_Modem_A.Timeout_ms = 20;

                 }
               }
             }
             break;
         case SET_MODEM_ZP_MODE:
             if(FDP_Comm_Modem_A.Timeout_ms == 0)
            {
             MODEM_A_M1 = SET_HIGH;
             MODEM_A_M0 = SET_LOW;
             FDP_Comm_Modem_A.State = DELAY_FOR_CONFIGURE;
             MODEM_A_CHANNEL_CONTROL = RELEASE_MODEM_CHANNEL;
             FDP_Comm_Modem_A.Timeout_ms = 30;

            }
             break;
        case DELAY_FOR_CONFIGURE:
            if(FDP_Comm_Modem_A.Timeout_ms == TIMEOUT_EVENT)
            {
             FDP_Comm_Modem_A.State = CONFIGURATION_COMPLETED;
            }
            break;
         case CONFIGURATION_COMPLETED:
             break;
         }

}

BYTE Get_FDP_Modem_A_State(void)
{
    BYTE State;
    State = (BYTE) FDP_Comm_Modem_A.State;
  return(State);
}

void Update_MDP_Sequence_Number(const BYTE Received_Seq_No)
{

    MDP_Computed_Seq_No = MDP_Previous_Seq_No + 1;

    if(Received_Seq_No == MDP_Computed_Seq_No)
    {
    MDP_Previous_Seq_No = Received_Seq_No;
    }
    else if(Received_Seq_No > MDP_Computed_Seq_No)
    {
    MDP_Message_Sequence_Missed += (Received_Seq_No - MDP_Computed_Seq_No);
    MDP_Previous_Seq_No = Received_Seq_No;
    }
    else if(Received_Seq_No < MDP_Computed_Seq_No)
    {
    MDP_Message_Sequence_Missed += (255 - MDP_Previous_Seq_No);
    MDP_Previous_Seq_No = Received_Seq_No;
    }

}

void Process_FDP_Command(const BYTE MDP_Cmd)
{
    BYTE Cmd_From_MDP = 0;

    Cmd_From_MDP = MDP_Cmd;

    switch(FDP_Info.State)
    {
        case UNIT_NORMAL:
            break;
    case WAITING_FOR_RESET:
        if(Cmd_From_MDP != FDP_RESET_COMMAND)
        {
        Cmd_From_MDP = (Cmd_From_MDP | FDP_NONRESPONSE_REPLY);
        FDP_Info.Response_Command = Cmd_From_MDP;
        FDP_Info.Reply_Command = WAITING_FOR_RESET;
        }
        break;
    case UNIT_RESETTED:
        if(Cmd_From_MDP == FDP_RESET_COMMAND)
        {
        Cmd_From_MDP = (Cmd_From_MDP | FDP_NONRESPONSE_REPLY);
        FDP_Info.Response_Command = Cmd_From_MDP;
        FDP_Info.Reply_Command = UNIT_RESETTED;
        }
        else
        {
        FDP_Info.Response_Command = Cmd_From_MDP;
        FDP_Info.Reply_Command = UNIT_NORMAL;
        }
        break;
    case FDP_DEFECTIVE:
        if(Cmd_From_MDP != FDP_RESET_COMMAND)
        {
        Cmd_From_MDP = (Cmd_From_MDP | FDP_NONRESPONSE_REPLY);
        FDP_Info.Response_Command = Cmd_From_MDP;
        FDP_Info.Reply_Command = FDP_DEFECTIVE;
        }
        break;

    }


}

void Update_FDP_Previous_Axle_Counts(void)
{

    FDP_Info.Previous_Fwd_Count =  (UINT16)FDP_Info.Xmit_Fwd_Count;
    FDP_Info.Previous_Rev_Count =  (UINT16)FDP_Info.Xmit_Rev_Count;

}

