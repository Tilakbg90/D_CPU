/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Component name      :   COMM_SM.c
    Module Version      :
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
    Function    :   void Initialise_SPI(void)
                    void Update_SPI_State(void)
                    void Decrement_SPI_50msTmr(void)
                    void Clear_SPI_Receive_Buffer(void)
                    void Clear_SPI_Transmit_Buffer(void)
                    void Build_Interprocess_Message(void)
                    void Build_Interprocess_Reset_Info_Message(void)
                    void Build_Interprocess_Post_Reset_Message(void)
                    void Process_Interprocess_Message(void)
                    void Process_Interprocess_Reset_Info_Message(void)
                    void Process_Interprocess_Post_Reset_Message(void)
                    void Check_Peer_Status(BYTE uchValue)

******************************************************************************/
#include <xc.h>

#include "COMMON.h"                                 /*Header file*/
#include "CRC16.h"                                  /*Header file*/
#include "COMM_SM.h"                                /*Header file*/
#include "RESET.h"                                  /*Header file*/
#include "ERROR.h"                                  /*Header file*/
#include "RLYD4_MGR.h"
#include "AXLE_MON.h"

extern const unsigned char SW_Version_No;           /* Hold the software version number, from DAC_MAIN.c*/
extern  /*near*/  dip_switch_info_t DIP_Switch_Info;      /* from DAC_MAIN.c */
extern  /*near*/  dac_status_t Status;                    /* from DAC_MAIN.c */
void Build_Interprocess_Message(void);

void Build_Interprocess_Reset_Info_Message(void);
void Build_Interprocess_Post_Reset_Message(void);
void Clear_SPI_Receive_Buffer(void);
void Clear_SPI_Transmit_Buffer(void);
void Process_Interprocess_Message(void);
void Process_Interprocess_Reset_Info_Message(void);
void Process_Interprocess_Post_Reset_Message(void);
void re_init_spi(void);
void de_init_spi(void);
void process_spi_tx_fifo(void);
void process_spi_rx_fifo(void);


spi_receive_object  SPI_Receive_Object;        /*Structure to hold the SPI receiving buffer*/
spi_transmit_object SPI_Transmit_Object;       /*Structure to hold the SPI transmission buffer*/
sm_message SM_Message;


BYTE ErrorCodeForSPI;

extern relay_a_info_t Relay_A_Info;
extern relay_b_info_t Relay_B_Info;
/*****************************************************************************

Component name      :COMM_SM
Module Name         :void Initialise_SPI(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :SPI configuration and SPI scheduler Initilaistion will be done.


Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   DAC_MAIN.c  -   Initialise_System()

Input Variables         Name                            Type
    Global          :   SPI_WAIT_FOR_SS_LOW             Enumerator

    Local           :   None

Output Variables        Name                            Type
    Global          :   TRISC5                          Port C bit 5 directional register
                        TRISC3                          Port C bit 3 directional register
                        TRISF7                          Port F bit 7 directional register
                        SSP1STAT                        MSSP1 Status register
                        SSP1CON1                        MSSP1 Control register
                        SPI_Transmit_Object.State       Enumerator
                        SPI_Transmit_Object.Index       BYTE
                        SPI_Receive_Object.Index        BYTE
                        SPI_Transmit_Object.Timeout     UINT16

    Local           :   None

Signal Variables



Macro definitions used:     Macro                           Value
                            OUTPUT_PORT                     0
                            INPUT_PORT                      1
                            SSPSTAT_DEFAULT                 0b01000000
                            SSPCON1_DEFAULT                 0b00110100
                            SPI_SCAN_RATE                   4
                            MAXIMUM_SPI_RETRIES             5
References          :

Derived Requirements:

Algorithm           :1.Configure the ports for SPI communication
                     2.Load the required values for SPI to transmit and receive messages
******************************************************************************/

#define SPI_SDO_TRIS    TRISDbits.TRISD9
#define SPI_SDI_TRIS    TRISDbits.TRISD10
#define SPI_CLK_TRIS    TRISDbits.TRISD8
#define SPI_SS_TRIS     TRISDbits.TRISD11

void Initialise_SPI(void)
{
    // Set Direction registers
    unsigned int Temp;
    
    SPI_CLK_TRIS = INPUT_PORT;   // SCK Input
    SPI_SDI_TRIS = INPUT_PORT;   // SDI Input
    SPI_SDO_TRIS = INPUT_PORT;  // SDO as Input to release it for the other CPU. It will be made output, when the SMCPU asserts the SS Line
    SPI_SS_TRIS = INPUT_PORT;  // SS Input

    /* Configure Remappable pins */
    //*************************************************************
    // Unlock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON & 0xbf); //clear the bit 6 of OSCCONL to unlock Pin Re-map
                            
    RPINR20bits.SCK1R = 2;          // Map SCK input to RP2 (Slave Mode)
    RPINR20bits.SDI1R = 3;          // Map SDI input to RP3
    //RPINR21bits.SS1R = 12;          // Map SS input to RP12
    RPOR2bits.RP4R = 7;             // Map RP4 to SDO Output
    //************************************************************
    // Lock Registers
    //************************************************************
    __builtin_write_OSCCONL(OSCCON | 0x40); //set the bit 6 of OSCCONL to lock Pin Re-map
    //******************************************************************

    Temp = SPI1BUF;
    
    SPI1STAT = 0;
    SPI1CON1 = 0;
    SPI1CON2 = 0;               //

    SPI1CON1bits.DISSCK = 0;    // Internal SPI Clock is enabled
                                // Idle state for the clock is a high level; active state is a low level
                                // Serial output data changes on transition from Idle clock state to active clock state
    SPI1CON1bits.DISSDO = 1;    // Internal SDO is Disabled for now. It will be enabled when the SS is asserted.
    SPI1CON1bits.MODE16 = 0;    // Operate in 8 bit mode
    SPI1CON1bits.SSEN = 0;      // Disable SS pin, as it is used as port


    SPI1CON1bits.CKP = 1;       // Idle state for the clock is a high


    SPI1CON1bits.MSTEN = 0;     // Operate in Slave Mode

    SPI1STATbits.SPIROV = 0;

    SPI1CON2bits.SPIBEN = 1;    // Enable 8 Byte Enhanced SPI Buffers for both Tx and Rx

    IEC0bits.SPI1IE = 0;        // Disable SPI Interrupts
    IEC0bits.SPF1IE = 0;

    IFS0bits.SPI1IF = 0;        // Clear Interrupt Flags
    IFS0bits.SPF1IF = 0;

    SPI_Transmit_Object.State = SPI_WAIT_FOR_SS_LOW;
    SPI_Transmit_Object.Index = (BYTE) 0;
    SPI_Receive_Object.Index  = (BYTE) 0;
    SPI_Transmit_Object.Timeout = (SPI_SCAN_RATE * MAXIMUM_SPI_RETRIES);

    de_init_spi();

}
/**************************************************************************
Component name      :COMM_SM
Module Name         :void Update_SPI_State(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Update the Axle counts,direction.


Design Requirements     :


Interfaces
    Calls           :   COMM_SM.c   -   Build_Interprocess_Message()
                        CRC16.c     -   Crc16()
                        COMM_SM.c   -   Process_Interprocess_Message()
                        COMM_SM.c   -   Clear_SPI_Receive_Buffer()
                        COMM_SM.c   -   Clear_SPI_Transmit_Buffer()
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
    Global          :   SPI_Transmit_Object.State       Enumerator
                        SPI_WAIT_FOR_SS_LOW             Enumerator
                        SPI_TRANSMIT_DATA_BYTES         Enumerator
                        SSPOV                           Bit
                        SSP1BUF                         8 bit register
                        SPI_TRANSMIT_DATA_BYTES         Enumerator
                        SPI_CHECK_CRC                   Enumerator
                        BF                              Bit
                        SPI_Receive_Object.Index        BYTE
                        SPI_Transmit_Object.Index       BYTE
                        SPI_Transmit_Object.Msg_Buffer  Array of BYTES
                        SSPEN                           Bit
                        SPI_WAIT_FOR_SS_LOW             Enumerator
                        PROCESS_RECEIVED_MESSAGE        Enumerator

    Local           :   Temp                            BYTE
                        Crc16_Return_Value              UINT16

Output Variables        Name                            Type
    Global          :   SPI_Transmit_Object.State       Enumerator
                        SPI_Transmit_Object.Index       BYTE
                        SPI_Receive_Object.Index        BYTE
                        SSP1BUF                         8 bit register
                        SPI_Receive_Object.Msg_Buffer   Array of BYTES

    Local           :   None

Signal Variables

                                SPI_SLAVE_SELECT                Input                   RF7

Macro definitions used:     Macro                           Value
                            SPI_MESSAGE_LENGTH              33

References          :

Derived Requirements:

Algorithm           :1.Check for slave select port,if the master has selected the slave then change the state to
                        "SPI_TRANSMIT_DATA_BYTES" and clear the transmit buffer before loading the message into the transmit buffer.
                     2.Again check the slave select port,if the master deselected the slave then slave will go for the state
                        "SPI_CHECK_CRC" otherwise goto step 3
                     3.Check the SPI buffer flag,if it got setted load the received message from the master into the
                        SPI receive object buffer and then load the message from the transmit object buffer into the
                        SPI buffer inorder to send it to master.This will continue until the total bytes has been exchanged
                     4.Calculate the checksum for the received message, if the checksum is valid then change the SPI state to
                        "PROCESS_RECEIVED_MESSAGE" otherwise wait for another message has to receive from the master
                     5.After process the received message clear the SPI receive object buffer before reading another reception
                        from the master
**********************************************************************/

void re_init_spi(void)
{
    unsigned int Temp;
    /* Prepare the Module */
    SPI1STATbits.SPIEN = 0;     // Enable Module
    SPI_SDO_TRIS = OUTPUT_PORT;    // Make SDO as Output to drive the SPI Signals
    SPI1CON1bits.DISSDO = 0;            // Enable the SDO Pin.
    Temp = SPI1BUF;
    SPI1STATbits.SPIROV = 0;
    SPI1CON2bits.SPIBEN = 1;    // Enable 8 Byte Enhanced SPI Buffers for both Tx and Rx
    SPI1STATbits.SPIEN = 1;     // Enable Module    
}

void de_init_spi(void)
{
    // Disable the Module and Free up the SDO Line for the other CPU to Send Data Out.
    SPI1CON1bits.DISSDO = 1;            // Disable the SDO Pin.
    SPI1STATbits.SPIEN = 0;     // Disable Module
    SPI_SDO_TRIS = INPUT_PORT;    // Make SDO as Inout for not to drive the SPI Signals
}


void process_spi_tx_fifo(void)
{
    if (SPI_Transmit_Object.State == SPI_TRANSMIT_DATA_BYTES)
    {
        if (SPI_Transmit_Object.Index < SPI_MESSAGE_LENGTH)
        {
            /* Load the next byte into the buffer */
            if(SPI1STATbits.SPITBF == 0) // If the Transmit FIFO buffer is free for few bytes
            {
                SPI1BUF = SPI_Transmit_Object.Msg_Buffer[SPI_Transmit_Object.Index];
                SPI_Transmit_Object.Index = SPI_Transmit_Object.Index + 1;
            }
        }
    }
}

void process_spi_rx_fifo(void)
{
    /* Flush out all the bytes in the FIFO to the RX Processing Buffer */
    if (SPI1STATbits.SRXMPT == 0)
    {
        /* Data has been received from Master (SM-CPU) */
        SPI_Receive_Object.Msg_Buffer[SPI_Receive_Object.Index] = (BYTE)SPI1BUF;
        if (SPI_Receive_Object.Index < (SPI_MESSAGE_LENGTH - 1))
        {
            SPI_Receive_Object.Index = SPI_Receive_Object.Index + 1;
        }
    }
}


void Update_SPI_State(void)
{
    UINT16 Crc16_Return_Value = 0;

    switch (SPI_Transmit_Object.State)
    {
        case SPI_WAIT_FOR_SS_LOW:
            if (SPI_SLAVE_SELECT == 0)
            {
                re_init_spi();

                SPI_Transmit_Object.State = SPI_TRANSMIT_DATA_BYTES;
                SPI_Transmit_Object.Index = (BYTE) 0;
                SPI_Receive_Object.Index  = (BYTE) 0;
                Clear_SPI_Transmit_Buffer();
                Build_Interprocess_Message();

            }
            break;
        case SPI_TRANSMIT_DATA_BYTES:
            if (SPI_SLAVE_SELECT == 1)
            {
                /* Master (SM-CPU)  has de-selected Slave (this board) */
                SPI_Transmit_Object.State = SPI_CHECK_CRC;
                SPI_Transmit_Object.Index = (BYTE) 0;
                //break;
            }

            process_spi_rx_fifo();
            process_spi_tx_fifo();

            break;
        case SPI_CHECK_CRC:
            Crc16_Return_Value = Crc16(SPI_RX,80);

            de_init_spi();
            
            if(Crc16_Return_Value != 0)
            {
                /* Invalid CRC-16 Checksum */
                SPI_Transmit_Object.State = SPI_WAIT_FOR_SS_LOW;
            }
            else
            {
                SPI_Transmit_Object.State = PROCESS_RECEIVED_MESSAGE;
            }
            break;
        case PROCESS_RECEIVED_MESSAGE:
            Process_Interprocess_Message();
            Clear_SPI_Receive_Buffer();
            SPI_Transmit_Object.State = SPI_WAIT_FOR_SS_LOW;
            break;
        default:
            break;
    }
}


/******************************************************************************
Component name      :COMM_SM
Module Name         :void Decrement_SPI_50msTmr(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :For every 50ms,SPI info timeout variable is decremented.

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
    Global          :   SPI_Transmit_Object.Timeout     UINT16

    Local           :   None

Output Variables        Name                            Type
    Global          :   SPI_Transmit_Object.Timeout     UINT16

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

Algorithm           :1.Decreament the SPI transmit timer value whenever the 50ms timer has overflowed
*****************************************************************************/
void Decrement_SPI_50msTmr(void)
{
    if(SPI_Transmit_Object.Timeout > 0)
    {
      SPI_Transmit_Object.Timeout = SPI_Transmit_Object.Timeout - 1;
    }

}
/*****************************************************************************
Component name      :COMM_SM
Module Name         :void Clear_SPI_Receive_Buffer(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the SPI reciever massage buffer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   COMM_SM.c   -   Update_SPI_State()

Input Variables         Name                            Type
    Global          :   None

    Local           :   uchCnt                          BYTE

Output Variables        Name                            Type
    Global          :   SPI_Receive_Object.Msg_Buffer   Array of BYTES

    Local           :   None

Signal Variables



Macro definitions used:     Macro                           Value
                            SPI_MESSAGE_LENGTH              33
References          :

Derived Requirements:

Algorithm           :1.Clear all the bytes of the SPI receive object buffer
******************************************************************************/
void Clear_SPI_Receive_Buffer(void)
{
 BYTE uchCnt;

    for(uchCnt = 0; uchCnt < SPI_MESSAGE_LENGTH; uchCnt++)
    {
     SPI_Receive_Object.Msg_Buffer[uchCnt] = (BYTE) 0;
    }
}

/*****************************************************************************
Component name      :COMM_SM
Module Name         :void Clear_SPI_Transmit_Buffer(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Clear the SPI transmitter massage buffer

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   Nil

    Called by       :   COMM_SM.c   -   Update_SPI_State()

Input Variables         Name                            Type
    Global          :   None

    Local           :   uchCnt                          BYTE

Output Variables        Name                            Type
    Global          :   SPI_Receive_Object.Msg_Buffer   Array of BYTES

    Local           :   None

Signal Variables



Macro definitions used:     Macro                           Value
                            SPI_MESSAGE_LENGTH              33
References          :

Derived Requirements:

******************************************************************************/
void Clear_SPI_Transmit_Buffer(void)
{
 BYTE uchCnt;

    for(uchCnt = 0; uchCnt < SPI_MESSAGE_LENGTH; uchCnt++)
    {
     SPI_Transmit_Object.Msg_Buffer[uchCnt] = (BYTE) 0;
    }
}

/*****************************************************************************
Component name      :COMM_SM
Module Name         :void Build_Interprocess_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Build the message buffer for interprocess communication


Design Requirements     :


Interfaces
    Calls           :   COMM_SM.c   -   Build_Interprocess_Post_Reset_Message()
                        COMM_SM.c   -   Build_Interprocess_Reset_Info_Message()

    Called by       :   COMM_SM.c   -   Update_SPI_State()

Input Variables         Name                                    Type
    Global          :   Status.Flags.Preparatory_Reset_DS       Bit
                        Status.Flags.Preparatory_Reset_US       Bit

    Local           :   None                                    None


Output Variables        Name                                    Type
    Global          :   SPI_Transmit_Object.Msg_Buffer[0 to 31] Array of BYTES

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            PREPARATORY_RESET_COMPLETED       1
References          :

Derived Requirements:

Algorithm           :1.Check the status flag to ensure the preparatory reset has applied or not
                     2.If reset applied call the Build Interprocess post reset message otherwise
                        call Build Interprocess reset info message.
**************************************************************************/
void Build_Interprocess_Message(void)
 {
     if(Status.Flags.Preparatory_Reset_DS == PREPARATORY_RESET_COMPLETED ||
         Status.Flags.Preparatory_Reset_US == PREPARATORY_RESET_COMPLETED)
      {
        Build_Interprocess_Post_Reset_Message();
       }
      else
        {
         Build_Interprocess_Reset_Info_Message();
        }
}


/*****************************************************************************
Component name      :COMM_SM
Module Name         :void Build_Interprocess_Post_Reset_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Build the message buffer for interprocess communication after the system reset has completed


Design Requirements     :


Interfaces
    Calls           :   AXLE_MON.c  -   Get_US_Fwd_AxleCount()
                        AXLE_MON.c  -   Get_US_Rev_AxleCount()
                        AXLE_MON.c  -   Get_DS_Fwd_AxleCount()
                        AXLE_MON.c  -   Get_DS_Rev_AxleCount()
                        AXLE_MON.c  -   Get_US_AxleDirection()
                        AXLE_MON.c  -   Get_DS_AxleCount()
                        AXLE_MON.c  -   Get_DS_Remote_AxleCount()
                        AXLE_MON.c  -   Get_US_AxleCount()
                        AXLE_MON.c  -   Get_US_Remote_AxleCount()
                        RESET.c     -   Update_System_Mode()
                        ERROR.c     -   Get_DS_Error_Code()
                        ERROR.c     -   Get_US_Error_Code()

    Called by       :   COMM_SM.c   -   Update_SPI_State()

Input Variables         Name                                    Type
    Global          :   DIP_Switch_Info.Address                 BYTE

    Local           :   DS_Fwd_AxleCount                        wordtype_t
                        DS_Rev_AxleCount                        wordtype_t
                        US_Fwd_AxleCount                        wordtype_t
                        US_Rev_AxleCount                        wordtype_t
                        TempBuf                                 wordtype_t
                        DS_Axle_Count                           wordtype_t
                        DS_Remote_Axle_Count                    wordtype_t
                        US_Axle_Count                           wordtype_t
                        US_Remote_Axle_Count                    wordtype_t
                        uchDirection                            BYTE
                        Status.Byte[0]                          BYTE
                        Status.Byte[1]                          BYTE
                        Status.Byte[2]                          BYTE
                        Status.Byte[3]                          BYTE
                        Status.Byte[4]                          BYTE
                        Status.Byte[5]                          BYTE
                        Status.Byte[6]                          BYTE
                        System_Mode                             BYTE
                        DS_Error_Code                           BYTE
                        US_Error_Code                           BYTE


Output Variables        Name                                    Type
    Global          :   SPI_Transmit_Object.Msg_Buffer[0 to 31] Array of BYTES

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            READ_AXLE_COUNT                 0x55
References          :

Derived Requirements:

Algorithm           :1.Load the CPU address,system status flag bytes,Upstream and downstream axle count messages,axle direction,
                        system mode,upstream and downstream error codes,message ID and 16bit CRC value into the SPI Transmit object buffer
                        inorder to send it to the master
**************************************************************************/
BYTE FWD = 0,REV = 0;
extern relay_d3_info_t Relay_D3_Info;
extern unsigned int COM_US_pkt_error_cnt, COM_DS_pkt_error_cnt;
void Build_Interprocess_Post_Reset_Message(void)
{
    wordtype_t DS_Fwd_AxleCount,DS_Rev_AxleCount;
    wordtype_t US_Fwd_AxleCount,US_Rev_AxleCount;
    wordtype_t DS_Axle_Count;
    wordtype_t US_Axle_Count;
    wordtype_t TempBuf;

    wordtype_t SPI_Relay_A_InfoLU1_Fwd_Count, SPI_Relay_A_InfoUS1_IN_Count, SPI_Relay_A_InfoLU2_Fwd_Count, SPI_Relay_A_InfoUS2_IN_Count;
    wordtype_t SPI_Relay_A_InfoLU1_Rev_Count, SPI_Relay_A_InfoUS1_OUT_Count, SPI_Relay_A_InfoLU2_Rev_Count, SPI_Relay_A_InfoUS2_OUT_Count;
    wordtype_t SPI_Relay_B_InfoLU1_Fwd_Count, SPI_Relay_B_InfoDS1_OUT_Count, SPI_Relay_B_InfoLU2_Fwd_Count, SPI_Relay_B_InfoDS2_OUT_Count;
    wordtype_t SPI_Relay_B_InfoLU1_Rev_Count, SPI_Relay_B_InfoDS1_IN_Count, SPI_Relay_B_InfoLU2_Rev_Count, SPI_Relay_B_InfoDS2_IN_Count;
    wordtype_t SPI_LU_Speed_Info;


    BYTE uchDirection;
    BYTE System_Mode;
    BYTE DS_Error_Code;
    BYTE US_Error_Code;

    longtype_t Calculated_Checksum;
    
    /* Load the Message Buffer with Data */
    US_Fwd_AxleCount.Word = Get_US_Fwd_AxleCount();     /* from axle_mon.c */
    US_Rev_AxleCount.Word = Get_US_Rev_AxleCount();     /* from axle_mon.c */
    DS_Fwd_AxleCount.Word = Get_DS_Fwd_AxleCount();     /* from axle_mon.c */
    DS_Rev_AxleCount.Word = Get_DS_Rev_AxleCount();     /* from axle_mon.c */
    DS_Axle_Count.Word    = Get_DS_AxleCount();         /* from axle_mon.c */
    US_Axle_Count.Word    = Get_US_AxleCount();         /* from axle_mon.c */

    SPI_Relay_A_InfoLU1_Fwd_Count.Word = 0;
    SPI_Relay_A_InfoUS1_IN_Count.Word = 0;
    SPI_Relay_A_InfoLU2_Fwd_Count.Word = 0;
    SPI_Relay_A_InfoUS2_IN_Count.Word = 0;
    SPI_Relay_A_InfoLU1_Rev_Count.Word = 0;
    SPI_Relay_A_InfoUS1_OUT_Count.Word = 0;
    SPI_Relay_A_InfoLU2_Rev_Count.Word = 0;
    SPI_Relay_A_InfoUS2_OUT_Count.Word = 0;

    SPI_Relay_B_InfoLU1_Fwd_Count.Word = 0;
    SPI_Relay_B_InfoDS1_OUT_Count.Word = 0;
    SPI_Relay_B_InfoLU2_Fwd_Count.Word = 0;
    SPI_Relay_B_InfoDS2_OUT_Count.Word = 0;
    SPI_Relay_B_InfoLU1_Rev_Count.Word = 0;
    SPI_Relay_B_InfoDS1_IN_Count.Word = 0;
    SPI_Relay_B_InfoLU2_Rev_Count.Word = 0;
    SPI_Relay_B_InfoDS2_IN_Count.Word = 0;

    if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_DE || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_LCWS_DL)
    {
        SPI_Relay_A_InfoLU1_Fwd_Count.Word = Track_Info_2.LCWS_Fwd_Count; // BFC //Relay_A_Info.LU1_Fwd_Count;	// Local Fwd Count CPU1
        SPI_Relay_A_InfoUS1_IN_Count.Word = Track_Info_3.LCWS_Fwd_Count;  // CFC //Relay_A_Info.US1_IN_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU1
        SPI_Relay_A_InfoLU2_Fwd_Count.Word = 0;           //Relay_A_Info.LU2_Fwd_Count;	// Local Fwd Count CPU2
        SPI_Relay_A_InfoUS2_IN_Count.Word = 0;            //Relay_A_Info.US2_IN_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU2
        SPI_Relay_A_InfoLU1_Rev_Count.Word = Track_Info_2.LCWS_Rev_Count; // BRC //Relay_A_Info.LU1_Rev_Count;	// Local Rev Count CPU1
        SPI_Relay_A_InfoUS1_OUT_Count.Word = Track_Info_3.LCWS_Rev_Count; // CRC //Relay_A_Info.US1_OUT_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU1
        SPI_Relay_A_InfoLU2_Rev_Count.Word = 0;           //Relay_A_Info.LU2_Rev_Count;	// Local Rev Count CPU2
        SPI_Relay_A_InfoUS2_OUT_Count.Word = 0;           //Relay_A_Info.US2_OUT_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU2

        SPI_Relay_B_InfoLU1_Fwd_Count.Word = Track_Info_1.LCWS_Fwd_Count; // AFC //Relay_B_Info.LU1_Fwd_Count;	// Local Fwd Count CPU1
        SPI_Relay_B_InfoDS1_OUT_Count.Word = Track_Info_4.LCWS_Fwd_Count; // DFC //Relay_B_Info.DS1_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU1
        SPI_Relay_B_InfoLU2_Fwd_Count.Word = 0;           //Relay_B_Info.LU2_Fwd_Count;	// Local Fwd Count CPU2
        SPI_Relay_B_InfoDS2_OUT_Count.Word = 0;           //Relay_B_Info.DS2_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU2
        SPI_Relay_B_InfoLU1_Rev_Count.Word = Track_Info_1.LCWS_Rev_Count; // ARC //Relay_B_Info.LU1_Rev_Count;	// Local Rev Count CPU1
        SPI_Relay_B_InfoDS1_IN_Count.Word = Track_Info_4.LCWS_Rev_Count;  // DRC  //Relay_B_Info.DS1_IN_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU1
        SPI_Relay_B_InfoLU2_Rev_Count.Word = 0;           //Relay_B_Info.LU2_Rev_Count;	// Local Rev Count CPU2
        SPI_Relay_B_InfoDS2_IN_Count.Word = 0;            //Relay_B_Info.DS2_IN_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU2
        Status.Flags.US_System_Status = 1;
        Status.Flags.US_Track_Status = 0;
        
        if(Track_Info_1.LCWS_Axle_Direction == (BYTE)FORWARD_DIRECTION || Track_Info_2.LCWS_Axle_Direction == (BYTE)FORWARD_DIRECTION || Track_Info_3.LCWS_Axle_Direction == (BYTE)FORWARD_DIRECTION || Track_Info_4.LCWS_Axle_Direction == (BYTE)FORWARD_DIRECTION)
        {
            
            US_Fwd_AxleCount.Word = Track_Info_1.LCWS_Fwd_Count;
            US_Rev_AxleCount.Word = Track_Info_2.LCWS_Fwd_Count;
            DS_Fwd_AxleCount.Word = Track_Info_3.LCWS_Fwd_Count;
            DS_Rev_AxleCount.Word = Track_Info_4.LCWS_Fwd_Count;
            FWD = 1;
            REV = 0;
        }
        else if(Track_Info_4.LCWS_Axle_Direction == (BYTE)REVERSE_DIRECTION || Track_Info_3.LCWS_Axle_Direction == (BYTE)REVERSE_DIRECTION || Track_Info_2.LCWS_Axle_Direction == (BYTE)REVERSE_DIRECTION || Track_Info_1.LCWS_Axle_Direction == (BYTE)REVERSE_DIRECTION)
        {
            
            US_Fwd_AxleCount.Word = Track_Info_1.LCWS_Rev_Count;
            US_Rev_AxleCount.Word = Track_Info_2.LCWS_Rev_Count;
            DS_Fwd_AxleCount.Word = Track_Info_3.LCWS_Rev_Count;
            DS_Rev_AxleCount.Word = Track_Info_4.LCWS_Rev_Count;
            REV = 1;
            FWD = 0;
        }
        else
        {
            if(FWD == 1)
            {
                US_Fwd_AxleCount.Word = Track_Info_1.LCWS_Fwd_Count;
                US_Rev_AxleCount.Word = Track_Info_2.LCWS_Fwd_Count;
                DS_Fwd_AxleCount.Word = Track_Info_3.LCWS_Fwd_Count;
                DS_Rev_AxleCount.Word = Track_Info_4.LCWS_Fwd_Count;
                //FWD = 0;
            }
            else if(REV == 1)
            {
                US_Fwd_AxleCount.Word = Track_Info_1.LCWS_Rev_Count;
                US_Rev_AxleCount.Word = Track_Info_2.LCWS_Rev_Count;
                DS_Fwd_AxleCount.Word = Track_Info_3.LCWS_Rev_Count;
                DS_Rev_AxleCount.Word = Track_Info_4.LCWS_Rev_Count;
                //REV = 0;
            }
            else
            {
                US_Fwd_AxleCount.Word = 1;
                US_Rev_AxleCount.Word = 2;
                DS_Fwd_AxleCount.Word = 3;
                DS_Rev_AxleCount.Word = 4;
            }
            
        }
    }
    else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_A || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_B || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D3_C)
    {
            SPI_Relay_A_InfoLU1_Fwd_Count.Word = Relay_D3_Info.A1_IN_Count;	// Local Fwd Count CPU1
            SPI_Relay_A_InfoUS1_IN_Count.Word =  Relay_D3_Info.B1_IN_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU1
            SPI_Relay_A_InfoLU2_Fwd_Count.Word = Relay_D3_Info.C1_IN_Count;	// Local Fwd Count CPU2
            SPI_Relay_A_InfoUS2_IN_Count.Word =  Relay_D3_Info.A1_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU2
            SPI_Relay_A_InfoLU1_Rev_Count.Word = Relay_D3_Info.B1_OUT_Count;	// Local Rev Count CPU1
            SPI_Relay_A_InfoUS1_OUT_Count.Word = Relay_D3_Info.C1_OUT_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU1
            SPI_Relay_A_InfoLU2_Rev_Count.Word = Relay_D3_Info.A2_IN_Count;	// Local Rev Count CPU2
            SPI_Relay_A_InfoUS2_OUT_Count.Word = Relay_D3_Info.B2_IN_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU2
            SPI_Relay_B_InfoLU1_Fwd_Count.Word = Relay_D3_Info.C2_IN_Count;	// Local Fwd Count CPU1
            SPI_Relay_B_InfoDS1_OUT_Count.Word = Relay_D3_Info.A2_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU1
            SPI_Relay_B_InfoLU2_Fwd_Count.Word = Relay_D3_Info.B2_OUT_Count;	// Local Fwd Count CPU2
            SPI_Relay_B_InfoDS2_OUT_Count.Word = Relay_D3_Info.C2_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU2
            SPI_Relay_B_InfoLU1_Rev_Count.Word = 0;	// Local Rev Count CPU1
            SPI_Relay_B_InfoDS1_IN_Count.Word = 0;	// Remote Rev Count/Remote Fwd Count (3S) CPU1
            SPI_Relay_B_InfoLU2_Rev_Count.Word = 0;	// Local Rev Count CPU2
            SPI_Relay_B_InfoDS2_IN_Count.Word = 0;	// Remote Rev Count/Remote Fwd Count (3S) CPU2
    }
    else if(DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_A || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_B || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_C  || DIP_Switch_Info.DAC_Unit_Type == DAC_UNIT_TYPE_D4_D)
    {
            SPI_Relay_A_InfoLU1_Fwd_Count.Word = Relay_D4_Info.A1_IN_Count;	
            SPI_Relay_A_InfoUS1_IN_Count.Word  = Relay_D4_Info.B1_IN_Count;	
            SPI_Relay_A_InfoLU2_Fwd_Count.Word = Relay_D4_Info.C1_IN_Count;	
            SPI_Relay_A_InfoUS2_IN_Count.Word  = Relay_D4_Info.D1_IN_Count;
            SPI_Relay_A_InfoLU1_Rev_Count.Word = Relay_D4_Info.A1_OUT_Count;
            SPI_Relay_A_InfoUS1_OUT_Count.Word = Relay_D4_Info.B1_OUT_Count;
            SPI_Relay_A_InfoLU2_Rev_Count.Word = Relay_D4_Info.C1_OUT_Count;
            SPI_Relay_A_InfoUS2_OUT_Count.Word = Relay_D4_Info.D1_OUT_Count;
            SPI_Relay_B_InfoLU1_Fwd_Count.Word = Relay_D4_Info.A2_IN_Count;		
            SPI_Relay_B_InfoDS1_OUT_Count.Word = Relay_D4_Info.B2_IN_Count;		
            SPI_Relay_B_InfoLU2_Fwd_Count.Word = Relay_D4_Info.C2_IN_Count;		
            SPI_Relay_B_InfoDS2_OUT_Count.Word = Relay_D4_Info.D2_IN_Count;
            SPI_Relay_B_InfoLU1_Rev_Count.Word = Relay_D4_Info.A2_OUT_Count;
            SPI_Relay_B_InfoDS1_IN_Count.Word  = Relay_D4_Info.B2_OUT_Count;
            SPI_Relay_B_InfoLU2_Rev_Count.Word = Relay_D4_Info.C2_OUT_Count;
            SPI_Relay_B_InfoDS2_IN_Count.Word  = Relay_D4_Info.D2_OUT_Count;
    }
    else
    {
        SPI_Relay_A_InfoLU1_Fwd_Count.Word = Relay_A_Info.LU1_Fwd_Count;	// Local Fwd Count CPU1
        SPI_Relay_A_InfoUS1_IN_Count.Word = Relay_A_Info.US1_IN_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU1
        SPI_Relay_A_InfoLU2_Fwd_Count.Word = Relay_A_Info.LU2_Fwd_Count;	// Local Fwd Count CPU2
        SPI_Relay_A_InfoUS2_IN_Count.Word = Relay_A_Info.US2_IN_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU2
        SPI_Relay_A_InfoLU1_Rev_Count.Word = Relay_A_Info.LU1_Rev_Count;	// Local Rev Count CPU1
        SPI_Relay_A_InfoUS1_OUT_Count.Word = Relay_A_Info.US1_OUT_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU1
        SPI_Relay_A_InfoLU2_Rev_Count.Word = Relay_A_Info.LU2_Rev_Count;	// Local Rev Count CPU2
        SPI_Relay_A_InfoUS2_OUT_Count.Word = Relay_A_Info.US2_OUT_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU2
        SPI_Relay_B_InfoLU1_Fwd_Count.Word = Relay_B_Info.LU1_Fwd_Count;	// Local Fwd Count CPU1
        SPI_Relay_B_InfoDS1_OUT_Count.Word = Relay_B_Info.DS1_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU1
        SPI_Relay_B_InfoLU2_Fwd_Count.Word = Relay_B_Info.LU2_Fwd_Count;	// Local Fwd Count CPU2
        SPI_Relay_B_InfoDS2_OUT_Count.Word = Relay_B_Info.DS2_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU2
        SPI_Relay_B_InfoLU1_Rev_Count.Word = Relay_B_Info.LU1_Rev_Count;	// Local Rev Count CPU1
        SPI_Relay_B_InfoDS1_IN_Count.Word = Relay_B_Info.DS1_IN_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU1
        SPI_Relay_B_InfoLU2_Rev_Count.Word = Relay_B_Info.LU2_Rev_Count;	// Local Rev Count CPU2
        SPI_Relay_B_InfoDS2_IN_Count.Word = Relay_B_Info.DS2_IN_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU2
    }
    SPI_Transmit_Object.Msg_Buffer[0] = DIP_Switch_Info.Address;
    SPI_Transmit_Object.Msg_Buffer[1] = Status.Byte[0];
    SPI_Transmit_Object.Msg_Buffer[2] = Status.Byte[1];
    SPI_Transmit_Object.Msg_Buffer[3] = Status.Byte[2];
    SPI_Transmit_Object.Msg_Buffer[4] = Status.Byte[3];
    SPI_Transmit_Object.Msg_Buffer[5] = Status.Byte[4];
    SPI_Transmit_Object.Msg_Buffer[6] = Status.Byte[5];
    SPI_Transmit_Object.Msg_Buffer[7] = Status.Byte[6];
    SPI_Transmit_Object.Msg_Buffer[8] = US_Fwd_AxleCount.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[9] = US_Fwd_AxleCount.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[10]= US_Rev_AxleCount.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[11]= US_Rev_AxleCount.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[12]= DS_Fwd_AxleCount.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[13]= DS_Fwd_AxleCount.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[14]= DS_Rev_AxleCount.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[15]= DS_Rev_AxleCount.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[16]= DS_Axle_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[17]= DS_Axle_Count.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[18]= SM_Message.DS_Remote_Axle_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[19]= SM_Message.DS_Remote_Axle_Count.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[20]= US_Axle_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[21]= US_Axle_Count.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[22]= SM_Message.US_Remote_Axle_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[23]= SM_Message.US_Remote_Axle_Count.Byte.Hi;
    uchDirection = Get_US_AxleDirection();
    SPI_Transmit_Object.Msg_Buffer[24]= uchDirection;
    uchDirection = Get_DS_AxleDirection();
    SPI_Transmit_Object.Msg_Buffer[25]= uchDirection;
    System_Mode = Update_System_Mode();
    SPI_Transmit_Object.Msg_Buffer[26] = System_Mode;
    DS_Error_Code = Get_DS_Error_Code();
    SPI_Transmit_Object.Msg_Buffer[27] = DS_Error_Code;
    US_Error_Code = Get_US_Error_Code();
    SPI_Transmit_Object.Msg_Buffer[28] = US_Error_Code;

    SPI_LU_Speed_Info.Word = LU_Speed_Info.SpeedValue;

    SPI_Transmit_Object.Msg_Buffer[29] = SPI_LU_Speed_Info.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[30] = SPI_LU_Speed_Info.Byte.Hi;
    Calculated_Checksum.LWord = Get_Calculated_Checksum();
    SPI_Transmit_Object.Msg_Buffer[31] = Calculated_Checksum.DWord.HiWord.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[32] = Calculated_Checksum.DWord.HiWord.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[33] = Calculated_Checksum.DWord.LoWord.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[34] = Calculated_Checksum.DWord.LoWord.Byte.Lo;    
    SPI_Transmit_Object.Msg_Buffer[35] = (BYTE)DIP_Switch_Info.DAC_Unit_Type;
    SPI_Transmit_Object.Msg_Buffer[36] = SW_Version_No;

    SPI_Transmit_Object.Msg_Buffer[37] = SPI_Relay_A_InfoLU1_Fwd_Count.Byte.Lo; // BFC
    SPI_Transmit_Object.Msg_Buffer[38] = SPI_Relay_A_InfoLU1_Fwd_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[39] = SPI_Relay_A_InfoUS1_IN_Count.Byte.Lo;  // CFC
    SPI_Transmit_Object.Msg_Buffer[40] = SPI_Relay_A_InfoUS1_IN_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[41] = SPI_Relay_A_InfoLU2_Fwd_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[42] = SPI_Relay_A_InfoLU2_Fwd_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[43] = SPI_Relay_A_InfoUS2_IN_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[44] = SPI_Relay_A_InfoUS2_IN_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[45] = SPI_Relay_A_InfoLU1_Rev_Count.Byte.Lo; // BRC
    SPI_Transmit_Object.Msg_Buffer[46] = SPI_Relay_A_InfoLU1_Rev_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[47] = SPI_Relay_A_InfoUS1_OUT_Count.Byte.Lo; // CRC
    SPI_Transmit_Object.Msg_Buffer[48] = SPI_Relay_A_InfoUS1_OUT_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[49] = SPI_Relay_A_InfoLU2_Rev_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[50] = SPI_Relay_A_InfoLU2_Rev_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[51] = SPI_Relay_A_InfoUS2_OUT_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[52] = SPI_Relay_A_InfoUS2_OUT_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[53] = SPI_Relay_B_InfoLU1_Fwd_Count.Byte.Lo; // AFC
    SPI_Transmit_Object.Msg_Buffer[54] = SPI_Relay_B_InfoLU1_Fwd_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[55] = SPI_Relay_B_InfoDS1_OUT_Count.Byte.Lo; // DFC
    SPI_Transmit_Object.Msg_Buffer[56] = SPI_Relay_B_InfoDS1_OUT_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[57] = SPI_Relay_B_InfoLU2_Fwd_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[58] = SPI_Relay_B_InfoLU2_Fwd_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[59] = SPI_Relay_B_InfoDS2_OUT_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[60] = SPI_Relay_B_InfoDS2_OUT_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[61] = SPI_Relay_B_InfoLU1_Rev_Count.Byte.Lo; // ARC
    SPI_Transmit_Object.Msg_Buffer[62] = SPI_Relay_B_InfoLU1_Rev_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[63] = SPI_Relay_B_InfoDS1_IN_Count.Byte.Lo; // DRC
    SPI_Transmit_Object.Msg_Buffer[64] = SPI_Relay_B_InfoDS1_IN_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[65] = SPI_Relay_B_InfoLU2_Rev_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[66] = SPI_Relay_B_InfoLU2_Rev_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[67] = SPI_Relay_B_InfoDS2_IN_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[68] = SPI_Relay_B_InfoDS2_IN_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[69] = ErrorCodeForSPI;
    if(COM_DS_pkt_error_cnt>ERROR_COUNT_THRESHOLD)
    {
        TempBuf.Word = COM_DS_pkt_error_cnt;
    }
    else
    {
        TempBuf.Word = 0;
    }    
    SPI_Transmit_Object.Msg_Buffer[70] = TempBuf.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[71] = TempBuf.Byte.Hi;    
    if(((unsigned int)COM_US_pkt_error_cnt/3)>ERROR_COUNT_THRESHOLD)
    {
        TempBuf.Word = ((unsigned int)COM_US_pkt_error_cnt/3);
    }
    else
    {
        TempBuf.Word = 0;
    }
    SPI_Transmit_Object.Msg_Buffer[72] = TempBuf.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[73] = TempBuf.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[74] = 0;
    SPI_Transmit_Object.Msg_Buffer[75] = 0;
    SPI_Transmit_Object.Msg_Buffer[76] = 0;

    SPI_Transmit_Object.Msg_Buffer[77] = READ_AXLE_COUNT;

    TempBuf.Word = Crc16(SPI_TX, 78);

    SPI_Transmit_Object.Msg_Buffer[78]= TempBuf.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[79]= TempBuf.Byte.Hi;



}


/*****************************************************************************
Component name      :COMM_SM
Module Name         :void Build_Interprocess_Reset_Info_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Build the message buffer for interprocess communication before the system reset has taken place


Design Requirements     :


Interfaces
    Calls           :   RESET.c     -   Update_System_Mode()
                        ERROR.c     -   Get_DS_Error_Code()
                        ERROR.c     -   Get_US_Error_Code()
                        RESTORE.c   -   Get_Compared_Checksum()
                        RESTORE.c   -   Get_Calculated_Checksum()

    Called by       :   COMM_SM.c   -   Update_SPI_State()

Input Variables         Name                                    Type
    Global          :   DIP_Switch_Info.Address                 BYTE
                        Status.Byte[0]                          BYTE
                        Status.Byte[1]                          BYTE
                        Status.Byte[2]                          BYTE
                        Status.Byte[3]                          BYTE
                        Status.Byte[4]                          BYTE
                        Status.Byte[5]                          BYTE
                        Status.Byte[6]                          BYTE

    Local           :
                        TempBuf                                 wordtype_t
                        uchDirection                            BYTE
                        System_Mode                             BYTE
                        DS_Error_Code                           BYTE
                        US_Error_Code                           BYTE
                        Compared_Checksum                       longtype_t
                        Calculated_Checksum                     longtype_t


Output Variables        Name                                    Type
    Global          :   SPI_Transmit_Object.Msg_Buffer[0 to 31] Array of BYTES

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            READ_RESET_INFO                 0xAA
References          :

Derived Requirements:

Algorithm           :1.Load the CPU address,system status flag bytes,Compared Checksum value,Calculated Checksum value,axle direction,
                        system mode,upstream and downstream error codes,message ID and 16bit CRC value into the SPI Transmit object buffer
                        inorder to send it to the master
**************************************************************************/

void Build_Interprocess_Reset_Info_Message(void)
{

    wordtype_t TempBuf;
    BYTE uchDirection;
    longtype_t Compared_Checksum;
    longtype_t Calculated_Checksum;
    BYTE System_Mode;
    BYTE DS_Error_Code;
    BYTE US_Error_Code;

    wordtype_t SPI_Relay_A_InfoLU1_Fwd_Count, SPI_Relay_A_InfoUS1_IN_Count, SPI_Relay_A_InfoLU2_Fwd_Count, SPI_Relay_A_InfoUS2_IN_Count;
    wordtype_t SPI_Relay_A_InfoLU1_Rev_Count, SPI_Relay_A_InfoUS1_OUT_Count, SPI_Relay_A_InfoLU2_Rev_Count, SPI_Relay_A_InfoUS2_OUT_Count;
    wordtype_t SPI_Relay_B_InfoLU1_Fwd_Count, SPI_Relay_B_InfoDS1_OUT_Count, SPI_Relay_B_InfoLU2_Fwd_Count, SPI_Relay_B_InfoDS2_OUT_Count;
    wordtype_t SPI_Relay_B_InfoLU1_Rev_Count, SPI_Relay_B_InfoDS1_IN_Count, SPI_Relay_B_InfoLU2_Rev_Count, SPI_Relay_B_InfoDS2_IN_Count;

    SPI_Relay_A_InfoLU1_Fwd_Count.Word = Relay_A_Info.LU1_Fwd_Count;	// Local Fwd Count CPU1
    SPI_Relay_A_InfoUS1_IN_Count.Word = Relay_A_Info.US1_IN_Count;		// Remote Fwd Count/Remote Rev Count (3S) CPU1
    SPI_Relay_A_InfoLU2_Fwd_Count.Word = Relay_A_Info.LU2_Fwd_Count;	// Local Fwd Count CPU2
    SPI_Relay_A_InfoUS2_IN_Count.Word = Relay_A_Info.US2_IN_Count;		// Remote Fwd Count/Remote Rev Count (3S) CPU2
    SPI_Relay_A_InfoLU1_Rev_Count.Word = Relay_A_Info.LU1_Rev_Count;	// Local Rev Count CPU1
    SPI_Relay_A_InfoUS1_OUT_Count.Word = Relay_A_Info.US1_OUT_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU1
    SPI_Relay_A_InfoLU2_Rev_Count.Word = Relay_A_Info.LU2_Rev_Count;	// Local Rev Count CPU2
    SPI_Relay_A_InfoUS2_OUT_Count.Word = Relay_A_Info.US2_OUT_Count;	// Remote Rev Count/Remote Fwd Count (3S) CPU2
    SPI_Relay_B_InfoLU1_Fwd_Count.Word = Relay_B_Info.LU1_Fwd_Count;	// Local Fwd Count CPU1
    SPI_Relay_B_InfoDS1_OUT_Count.Word = Relay_B_Info.DS1_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU1
    SPI_Relay_B_InfoLU2_Fwd_Count.Word = Relay_B_Info.LU2_Fwd_Count;	// Local Fwd Count CPU2
    SPI_Relay_B_InfoDS2_OUT_Count.Word = Relay_B_Info.DS2_OUT_Count;	// Remote Fwd Count/Remote Rev Count (3S) CPU2
    SPI_Relay_B_InfoLU1_Rev_Count.Word = Relay_B_Info.LU1_Rev_Count;	// Local Rev Count CPU1
    SPI_Relay_B_InfoDS1_IN_Count.Word = Relay_B_Info.DS1_IN_Count;		// Remote Rev Count/Remote Fwd Count (3S) CPU1
    SPI_Relay_B_InfoLU2_Rev_Count.Word = Relay_B_Info.LU2_Rev_Count;	// Local Rev Count CPU2
    SPI_Relay_B_InfoDS2_IN_Count.Word = Relay_B_Info.DS2_IN_Count;		// Remote Rev Count/Remote Fwd Count (3S) CPU2


    /* Load the Message Buffer with Data */
    SPI_Transmit_Object.Msg_Buffer[0] = DIP_Switch_Info.Address;
    SPI_Transmit_Object.Msg_Buffer[1] = Status.Byte[0];
    SPI_Transmit_Object.Msg_Buffer[2] = Status.Byte[1];
    SPI_Transmit_Object.Msg_Buffer[3] = Status.Byte[2];
    SPI_Transmit_Object.Msg_Buffer[4] = Status.Byte[3];
    SPI_Transmit_Object.Msg_Buffer[5] = Status.Byte[4];
    SPI_Transmit_Object.Msg_Buffer[6] = Status.Byte[5];
    SPI_Transmit_Object.Msg_Buffer[7] = Status.Byte[6];
    Compared_Checksum.LWord = Get_Compared_Checksum();
    SPI_Transmit_Object.Msg_Buffer[8] = Compared_Checksum.DWord.HiWord.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[9] = Compared_Checksum.DWord.HiWord.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[10]= Compared_Checksum.DWord.LoWord.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[11]= Compared_Checksum.DWord.LoWord.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[12]= (BYTE)DIP_Switch_Info.DAC_Unit_Type;
    SPI_Transmit_Object.Msg_Buffer[13]= SW_Version_No;
    Calculated_Checksum.LWord = Get_Calculated_Checksum();
    SPI_Transmit_Object.Msg_Buffer[14] = Calculated_Checksum.DWord.HiWord.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[15] = Calculated_Checksum.DWord.HiWord.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[16] =  Calculated_Checksum.DWord.LoWord.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[17] =  Calculated_Checksum.DWord.LoWord.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[20] =  (BYTE)DIP_Switch_Info.Configuration;
    uchDirection = Get_US_AxleDirection();
    SPI_Transmit_Object.Msg_Buffer[24]= uchDirection;
    uchDirection = Get_DS_AxleDirection();
    SPI_Transmit_Object.Msg_Buffer[25]= uchDirection;
    System_Mode = Update_System_Mode();
    SPI_Transmit_Object.Msg_Buffer[26] = System_Mode;
    DS_Error_Code = Get_DS_Error_Code();
    SPI_Transmit_Object.Msg_Buffer[27] = DS_Error_Code;
    US_Error_Code = Get_US_Error_Code();
    SPI_Transmit_Object.Msg_Buffer[28] = US_Error_Code;

    SPI_Transmit_Object.Msg_Buffer[29] = 0;
    SPI_Transmit_Object.Msg_Buffer[30] = 0;
    SPI_Transmit_Object.Msg_Buffer[31] = 0;
    SPI_Transmit_Object.Msg_Buffer[32] = 0;
    SPI_Transmit_Object.Msg_Buffer[33] = 0;
    SPI_Transmit_Object.Msg_Buffer[34] = 0;
    SPI_Transmit_Object.Msg_Buffer[35] = 0;
    SPI_Transmit_Object.Msg_Buffer[36] = 0;

    SPI_Transmit_Object.Msg_Buffer[37] = SPI_Relay_A_InfoLU1_Fwd_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[38] = SPI_Relay_A_InfoLU1_Fwd_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[39] = SPI_Relay_A_InfoUS1_IN_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[40] = SPI_Relay_A_InfoUS1_IN_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[41] = SPI_Relay_A_InfoLU2_Fwd_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[42] = SPI_Relay_A_InfoLU2_Fwd_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[43] = SPI_Relay_A_InfoUS2_IN_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[44] = SPI_Relay_A_InfoUS2_IN_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[45] = SPI_Relay_A_InfoLU1_Rev_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[46] = SPI_Relay_A_InfoLU1_Rev_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[47] = SPI_Relay_A_InfoUS1_OUT_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[48] = SPI_Relay_A_InfoUS1_OUT_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[49] = SPI_Relay_A_InfoLU2_Rev_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[50] = SPI_Relay_A_InfoLU2_Rev_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[51] = SPI_Relay_A_InfoUS2_OUT_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[52] = SPI_Relay_A_InfoUS2_OUT_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[53] = SPI_Relay_B_InfoLU1_Fwd_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[54] = SPI_Relay_B_InfoLU1_Fwd_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[55] = SPI_Relay_B_InfoDS1_OUT_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[56] = SPI_Relay_B_InfoDS1_OUT_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[57] = SPI_Relay_B_InfoLU2_Fwd_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[58] = SPI_Relay_B_InfoLU2_Fwd_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[59] = SPI_Relay_B_InfoDS2_OUT_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[60] = SPI_Relay_B_InfoDS2_OUT_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[61] = SPI_Relay_B_InfoLU1_Rev_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[62] = SPI_Relay_B_InfoLU1_Rev_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[63] = SPI_Relay_B_InfoDS1_IN_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[64] = SPI_Relay_B_InfoDS1_IN_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[65] = SPI_Relay_B_InfoLU2_Rev_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[66] = SPI_Relay_B_InfoLU2_Rev_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[67] = SPI_Relay_B_InfoDS2_IN_Count.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[68] = SPI_Relay_B_InfoDS2_IN_Count.Byte.Hi;

    SPI_Transmit_Object.Msg_Buffer[69] = ErrorCodeForSPI;
    if(COM_DS_pkt_error_cnt>ERROR_COUNT_THRESHOLD)
    {
        TempBuf.Word = COM_DS_pkt_error_cnt;
    }
    else
    {
        TempBuf.Word = 0;
    }
    SPI_Transmit_Object.Msg_Buffer[70] = TempBuf.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[71] = TempBuf.Byte.Hi;
    if(((unsigned int)COM_US_pkt_error_cnt/3)>ERROR_COUNT_THRESHOLD)
    {
        TempBuf.Word = ((unsigned int)COM_US_pkt_error_cnt/3);
    }
    else
    {
        TempBuf.Word = 0;
    }
    SPI_Transmit_Object.Msg_Buffer[72] = TempBuf.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[73] = TempBuf.Byte.Hi;
    SPI_Transmit_Object.Msg_Buffer[74] = 0;
    SPI_Transmit_Object.Msg_Buffer[75] = 0;
    SPI_Transmit_Object.Msg_Buffer[76] = 0;

    SPI_Transmit_Object.Msg_Buffer[77] = READ_RESET_INFO;
    TempBuf.Word = Crc16(SPI_TX, 78);
    SPI_Transmit_Object.Msg_Buffer[78]= TempBuf.Byte.Lo;
    SPI_Transmit_Object.Msg_Buffer[79]= TempBuf.Byte.Hi;


}


/***************************************************************************
Component name      :COMM_SM
Module Name         :void Process_Interprocess_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Process interprocess message

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RELAYMR.c   -   Declare_DAC_Defective()
                        ERROR.c     -   Set_Error_Status_Bit()
                        RESET.c     -   Post_Peer_CPU_Reset()
                        RESET.c     -   Post_Peer_CPU_Reset1()
                        RLYB_MGR.c  -   Process_Peer_Relay_B_AxleCount()
                        RLYB_MGR.c  -   Process_Peer_Relay_B_Direction()
                        RLYA_MGR.c  -   Process_Peer_Relay_A_AxleCount()
                        RLYA_MGR.c  -   Process_Peer_Relay_A_Direction()
                        RLYD3_MGR.c -   Process_D3_Peer_AxleCount()
                        RLYD3_MGR.c -   Process_D3_Peer_Direction()
                        RLYDE_MGR.c -   Process_Relay_DE_Peer_AxleCount()
                        RLYDE_MGR.c -   Process_Relay_DE_Peer_Direction()
                        COMM_SM.c   -   Process_Interprocess_Reset_Info_Message()
                        COMM_SM.c   -   Process_Interprocess_Post_Reset_Message()


    Called by       :   COMM_SM.c   -   Update_SPI_State()

Input Variables         Name                                    Type
    Global          : SPI_Receive_Object.Msg_Buffer[29]     BYTE

    Local           :   uchCommand                              BYTE

Output Variables        Name                                    Type
    Global          :   None

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            READ_RESET_INFO                 0xAA
                            READ_AXLE_COUNT                 0x55

References          :

Derived Requirements:

Algorithm           :1.Check the message ID,then process the message as per the received value
                     2.Call Process Interprocess reset info message if the value is 0xAA
                     3.Call Process Interprocess Post reset message if the value is 0x55

*************************************************************************/
void Process_Interprocess_Message(void)
{
   BYTE uchCommand;

   uchCommand = SPI_Receive_Object.Msg_Buffer[77];
   if(uchCommand == READ_RESET_INFO)
      {
         Process_Interprocess_Reset_Info_Message();
       }
   if (uchCommand == READ_AXLE_COUNT)
      {
       Process_Interprocess_Post_Reset_Message();
      }
}

//01/09/10

/***************************************************************************
Component name      :COMM_SM
Module Name         :
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Process interprocess reset message

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RELAYMR.c   -   Declare_DAC_Defective()
                        ERROR.c     -   Set_Error_Status_Bit()
                        RESET.c     -   Post_Peer_CPU_Reset()
                        RESET.c     -   Post_Peer_CPU_Reset1()
                        RLYB_MGR.c  -   Process_Peer_Relay_B_AxleCount()
                        RLYB_MGR.c  -   Process_Peer_Relay_B_Direction()
                        RLYA_MGR.c  -   Process_Peer_Relay_A_AxleCount()
                        RLYA_MGR.c  -   Process_Peer_Relay_A_Direction()
                        RLYD3_MGR.c -   Process_D3_Peer_AxleCount()
                        RLYD3_MGR.c -   Process_D3_Peer_Direction()
                        RLYDE_MGR.c -   Process_Relay_DE_Peer_AxleCount()
                        RLYDE_MGR.c -   Process_Relay_DE_Peer_Direction()
                        COMM_SM.c   -   Check_Peer_Status()
                        RESTORE.c   -   Get_Calculated_Checksum()

    Called by       :   COMM_SM.c   -   Update_SPI_State()

Input Variables         Name                                    Type
    Global          :   DIP_Switch_Info.Peer_Address            BYTE
                        SPI_Receive_Object.Msg_Buffer           Array of BYTEs
                        DIP_Switch_Info.DAC_Unit_Type               DAC_Unit_Type
                        DAC_UNIT_TYPE_SF                        Enumerator
                        DAC_UNIT_TYPE_EF                        Enumerator
                        DAC_UNIT_TYPE_CF                        Enumerator
                        DAC_UNIT_TYPE_D3_A                      Enumerator
                        DAC_UNIT_TYPE_D3_B                      Enumerator
                        DAC_UNIT_TYPE_D3_C                      Enumerator
                        DAC_UNIT_TYPE_DE                        Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        Status.Flags.System_Status              Bit

    Local           :   DS_Fwd_AxleCount                        wordtype_t
                        DS_Rev_AxleCount                        wordtype_t
                        US_Fwd_AxleCount                        wordtype_t
                        US_Rev_AxleCount                        wordtype_t
                        uchDirection                            BYTE
                        Buffer                                  bitadrb_t
                        SrcAdr                                  bitadrb_t
                        Calculated_Checksum                     longtype_t
                        Peer_CPU_Checksum                       longtype_t

Output Variables        Name                                    Type
    Global          :   None

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            INOPERATIVE_CONFIGURATION_ERROR_NUM        58
                            SPI_SCAN_RATE                   4
                            MAXIMUM_SPI_RETRIES             5
                            NORMAL                          1
                            CATASTROPHIC_ERROR              0
                            INCORRECT_CODE_CRC_ERROR_NUM           57
References          :

Derived Requirements:

Algorithm           :1.Check the source address with the peer CPU address,
                        if not equal declare DAC defective Set
                        INVALID_CONFIGURATION ID error bit
                     2.Assign the SPI transmit timeout to 20ms
                     3.Assign the Up stream forward and reverse axle count,
                        down stream forward and reverse axle count equal to zero
                     4.Update and compare the peer CPU checksum with it's own checksum and declare error INCORRECT CODE CRC if
                        it doesn't matches
                     5.Update the reset in peer CPU
                     6.Depending upon the configuration update the axle
                        counts and direction in peer CPU

*************************************************************************/
void Process_Interprocess_Reset_Info_Message(void)
{
  wordtype_t DS_Fwd_AxleCount,DS_Rev_AxleCount;
  wordtype_t US_Fwd_AxleCount,US_Rev_AxleCount,Axle_Count;
  BYTE uchDirection;
  bitadrb_t Buffer,SrcAdr;
  longtype_t Calculated_Checksum;
  longtype_t Peer_CPU_Checksum;
  BYTE Peer_CPU_Unit_Type;

  SrcAdr.Byte  = SPI_Receive_Object.Msg_Buffer[0];
  if (SrcAdr.Byte != DIP_Switch_Info.Peer_Address)
  {
    Declare_DAC_Defective();
    Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
    return;
  }
//  Peer_CPU_Configuration = SPI_Receive_Object.Msg_Buffer[20];
//  if(DIP_Switch_Info.Configuration != Peer_CPU_Configuration)
//   {
//  Declare_DAC_Defective();
//    Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
//  return;
//   }
  Peer_CPU_Unit_Type = SPI_Receive_Object.Msg_Buffer[12];

   if((BYTE)DIP_Switch_Info.DAC_Unit_Type != Peer_CPU_Unit_Type)
    {
     Declare_DAC_Defective();
    Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
    return;
    }

  SPI_Transmit_Object.Timeout = (SPI_SCAN_RATE * MAXIMUM_SPI_RETRIES);
  US_Fwd_AxleCount.Byte.Lo    = 0x00;
  US_Fwd_AxleCount.Byte.Hi    = 0x00;
  US_Rev_AxleCount.Byte.Lo    = 0x00;
  US_Rev_AxleCount.Byte.Hi    = 0x00;
  DS_Fwd_AxleCount.Byte.Lo    = 0x00;
  DS_Fwd_AxleCount.Byte.Hi    = 0x00;
  DS_Rev_AxleCount.Byte.Lo    = 0x00;
  DS_Rev_AxleCount.Byte.Hi    = 0x00;
  Axle_Count.Byte.Lo          = 0x00;
  Axle_Count.Byte.Hi          = 0x00;

  Peer_CPU_Checksum.DWord.HiWord.Byte.Hi = SPI_Receive_Object.Msg_Buffer[14];
  Peer_CPU_Checksum.DWord.HiWord.Byte.Lo = SPI_Receive_Object.Msg_Buffer[15];
  Peer_CPU_Checksum.DWord.LoWord.Byte.Hi = SPI_Receive_Object.Msg_Buffer[16];
  Peer_CPU_Checksum.DWord.LoWord.Byte.Lo = SPI_Receive_Object.Msg_Buffer[17];

  Calculated_Checksum.LWord = Get_Calculated_Checksum();
  if(Calculated_Checksum.LWord != Peer_CPU_Checksum.LWord)
  {
        Status.Flags.System_Status  = CATASTROPHIC_ERROR;
        Set_Error_Status_Bit(INCORRECT_CODE_CRC_ERROR_NUM);
  }

  Buffer.Byte = SPI_Receive_Object.Msg_Buffer[1];

  Post_Peer_CPU_Reset((BYTE) Buffer.Bit.b4);
  Post_Peer_CPU_Reset1((BYTE) Buffer.Bit.b7);


      switch (DIP_Switch_Info.DAC_Unit_Type)
        {
            case DAC_UNIT_TYPE_SF:
                Process_Peer_Relay_B_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
                uchDirection      = SPI_Receive_Object.Msg_Buffer[25];
                Process_Peer_Relay_B_Direction(uchDirection);
                break;
            case DAC_UNIT_TYPE_EF:
                Process_Peer_Relay_A_AxleCount(US_Fwd_AxleCount.Word,US_Rev_AxleCount.Word);
                uchDirection      = SPI_Receive_Object.Msg_Buffer[24];
                Process_Peer_Relay_A_Direction(uchDirection);
                break;
            case DAC_UNIT_TYPE_CF:
            case DAC_UNIT_TYPE_3D_SF:
            case DAC_UNIT_TYPE_3D_EF:  
                Process_Peer_Relay_A_AxleCount(US_Fwd_AxleCount.Word,US_Rev_AxleCount.Word);
                uchDirection  = SPI_Receive_Object.Msg_Buffer[24];
                Process_Peer_Relay_A_Direction(uchDirection);
                Process_Peer_Relay_B_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
                uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
                Process_Peer_Relay_B_Direction(uchDirection);
                break;
            case DAC_UNIT_TYPE_D3_A:
                Process_D3_Peer_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
                uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
                Process_D3_Peer_Direction(uchDirection,Axle_Count.Word);
                break;
            case DAC_UNIT_TYPE_D3_B:
                Process_D3_Peer_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
                uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
                Process_D3_Peer_Direction(uchDirection,Axle_Count.Word);
                break;
            case DAC_UNIT_TYPE_D3_C:
                Process_D3_Peer_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
                uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
                Process_D3_Peer_Direction(uchDirection,Axle_Count.Word);
                break;
            case DAC_UNIT_TYPE_D4_A:
            case DAC_UNIT_TYPE_D4_B:
            case DAC_UNIT_TYPE_D4_C:
            case DAC_UNIT_TYPE_D4_D:
                Process_D4_Peer_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
                uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
                Process_D4_Peer_Direction(uchDirection,Axle_Count.Word);
                break;
            case DAC_UNIT_TYPE_LCWS:
                break;
            case DAC_UNIT_TYPE_LCWS_DL:
                break;
            case DAC_UNIT_TYPE_DE:
                break;
            default:
                break;
        }

}


/***************************************************************************
Component name      :COMM_SM
Module Name         :void Process_Interprocess_Post_Reset_Message(void)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Process interprocess post reset message

Allocated Requirements  :

Design Requirements     :

Interfaces
    Calls           :   RELAYMR.c   -   Declare_DAC_Defective()
                        ERROR.c     -   Set_Error_Status_Bit()
                        RESET.c     -   Post_Peer_CPU_Reset()
                        RESET.c     -   Post_Peer_CPU_Reset1()
                        RLYB_MGR.c  -   Process_Peer_Relay_B_AxleCount()
                        RLYB_MGR.c  -   Process_Peer_Relay_B_Direction()
                        RLYA_MGR.c  -   Process_Peer_Relay_A_AxleCount()
                        RLYA_MGR.c  -   Process_Peer_Relay_A_Direction()
                        RLYD3_MGR.c -   Process_D3_Peer_AxleCount()
                        RLYD3_MGR.c -   Process_D3_Peer_Direction()
                        RLYDE_MGR.c -   Process_Relay_DE_Peer_AxleCount()
                        RLYDE_MGR.c -   Process_Relay_DE_Peer_Direction()
                        COMM_SM.c   -   Check_Peer_Status()

    Called by       :   COMM_SM.c   -   Update_SPI_State()

Input Variables         Name                                    Type
    Global          :   DIP_Switch_Info.Peer_Address            BYTE
                        SPI_Receive_Object.Msg_Buffer           Array of BYTEs
                        DIP_Switch_Info.DAC_Unit_Type               DAC_Unit_Type
                        DAC_UNIT_TYPE_SF                        Enumerator
                        DAC_UNIT_TYPE_EF                        Enumerator
                        DAC_UNIT_TYPE_CF                        Enumerator
                        DAC_UNIT_TYPE_3D_SF                Enumerator
                        DAC_UNIT_TYPE_3D_EF                Enumerator
                        DAC_UNIT_TYPE_D3_A                      Enumerator
                        DAC_UNIT_TYPE_D3_B                      Enumerator
                        DAC_UNIT_TYPE_D3_C                      Enumerator
                        DAC_UNIT_TYPE_DE                        Enumerator
                        Status.Flags.System_Status              Bit

    Local           :   DS_Fwd_AxleCount                        wordtype_t
                        DS_Rev_AxleCount                        wordtype_t
                        US_Fwd_AxleCount                        wordtype_t
                        US_Rev_AxleCount                        wordtype_t
                        Axle_Count                              wordtype_t
                        uchDirection                            BYTE
                        Buffer                                  bitadrb_t
                        SrcAdr                                  bitadrb_t

Output Variables        Name                                    Type
    Global          :   None

    Local           :   None

Signal Variables


Macro definitions used:     Macro                           Value
                            INOPERATIVE_CONFIGURATION_ERROR_NUM        58
                            SPI_SCAN_RATE                   4
                            MAXIMUM_SPI_RETRIES             5
                            NORMAL                          1

References          :

Derived Requirements:

Algorithm           :1.Check the source address with the peer CPU address,
                        if not equal declare DAC defective Set
                        INVALID_CONFIGURATION ID error bit
                     2.Assign the SPI transmit timeout to 20ms
                     3.Update the Up stream forward and reverse axle count,
                        down stream forward and reverse axle count , local axle count
                     4.Update the reset in peer CPU
                     6.Depending upon the configuration update the axle
                        counts and direction in peer CPU

*************************************************************************/
void Process_Interprocess_Post_Reset_Message(void)
{
  wordtype_t DS_Fwd_AxleCount,DS_Rev_AxleCount;
  wordtype_t US_Fwd_AxleCount,US_Rev_AxleCount,Axle_Count;
  BYTE uchDirection;
  bitadrb_t Buffer,SrcAdr;

    wordtype_t SPI_Relay_A_InfoLU1_Fwd_Count, SPI_Relay_A_InfoUS1_IN_Count, SPI_Relay_A_InfoLU2_Fwd_Count, SPI_Relay_A_InfoUS2_IN_Count;
    wordtype_t SPI_Relay_A_InfoLU1_Rev_Count, SPI_Relay_A_InfoUS1_OUT_Count, SPI_Relay_A_InfoLU2_Rev_Count, SPI_Relay_A_InfoUS2_OUT_Count;
    wordtype_t SPI_Relay_B_InfoLU1_Fwd_Count, SPI_Relay_B_InfoDS1_OUT_Count, SPI_Relay_B_InfoLU2_Fwd_Count, SPI_Relay_B_InfoDS2_OUT_Count;
    wordtype_t SPI_Relay_B_InfoLU1_Rev_Count, SPI_Relay_B_InfoDS1_IN_Count, SPI_Relay_B_InfoLU2_Rev_Count, SPI_Relay_B_InfoDS2_IN_Count;

  SrcAdr.Byte  = SPI_Receive_Object.Msg_Buffer[0];
  if (SrcAdr.Byte != DIP_Switch_Info.Peer_Address)
  {
    Declare_DAC_Defective();
    Set_Error_Status_Bit(INOPERATIVE_CONFIGURATION_ERROR_NUM);
    return;
  }

  SPI_Transmit_Object.Timeout = (SPI_SCAN_RATE * MAXIMUM_SPI_RETRIES);

  US_Fwd_AxleCount.Byte.Lo    = SPI_Receive_Object.Msg_Buffer[8];
  US_Fwd_AxleCount.Byte.Hi    = SPI_Receive_Object.Msg_Buffer[9];
  US_Rev_AxleCount.Byte.Lo    = SPI_Receive_Object.Msg_Buffer[10];
  US_Rev_AxleCount.Byte.Hi    = SPI_Receive_Object.Msg_Buffer[11];
  DS_Fwd_AxleCount.Byte.Lo    = SPI_Receive_Object.Msg_Buffer[12];
  DS_Fwd_AxleCount.Byte.Hi    = SPI_Receive_Object.Msg_Buffer[13];
  DS_Rev_AxleCount.Byte.Lo    = SPI_Receive_Object.Msg_Buffer[14];
  DS_Rev_AxleCount.Byte.Hi    = SPI_Receive_Object.Msg_Buffer[15];
  Axle_Count.Byte.Lo          = SPI_Receive_Object.Msg_Buffer[16];
  Axle_Count.Byte.Hi          = SPI_Receive_Object.Msg_Buffer[17];

  Buffer.Byte = SPI_Receive_Object.Msg_Buffer[1];

  Post_Peer_CPU_Reset((BYTE) Buffer.Bit.b4);
  Post_Peer_CPU_Reset1((BYTE) Buffer.Bit.b7);


  switch (DIP_Switch_Info.DAC_Unit_Type)
    {
        case DAC_UNIT_TYPE_SF:
            Process_Peer_Relay_B_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
            uchDirection      = SPI_Receive_Object.Msg_Buffer[25];
            Process_Peer_Relay_B_Direction(uchDirection);
            break;
        case DAC_UNIT_TYPE_EF:
            Process_Peer_Relay_A_AxleCount(US_Fwd_AxleCount.Word,US_Rev_AxleCount.Word);
            uchDirection      = SPI_Receive_Object.Msg_Buffer[24];
            Process_Peer_Relay_A_Direction(uchDirection);
            break;
        case DAC_UNIT_TYPE_CF:
        case DAC_UNIT_TYPE_3D_SF:
        case DAC_UNIT_TYPE_3D_EF:
            Process_Peer_Relay_A_AxleCount(US_Fwd_AxleCount.Word,US_Rev_AxleCount.Word);
            uchDirection  = SPI_Receive_Object.Msg_Buffer[24];
            Process_Peer_Relay_A_Direction(uchDirection);
            Process_Peer_Relay_B_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
            uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
            Process_Peer_Relay_B_Direction(uchDirection);
            break;
        case DAC_UNIT_TYPE_D3_A:
            Process_D3_Peer_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
            uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
            Process_D3_Peer_Direction(uchDirection,Axle_Count.Word);
            break;
        case DAC_UNIT_TYPE_D3_B:
            Process_D3_Peer_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
            uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
            Process_D3_Peer_Direction(uchDirection,Axle_Count.Word);
            break;
        case DAC_UNIT_TYPE_D3_C:
            Process_D3_Peer_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
            uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
            Process_D3_Peer_Direction(uchDirection,Axle_Count.Word);
            break;
        case DAC_UNIT_TYPE_D4_A:
        case DAC_UNIT_TYPE_D4_B:
        case DAC_UNIT_TYPE_D4_C:
        case DAC_UNIT_TYPE_D4_D:
            Process_D4_Peer_AxleCount(DS_Fwd_AxleCount.Word,DS_Rev_AxleCount.Word);
            uchDirection  = SPI_Receive_Object.Msg_Buffer[25];
            Process_D4_Peer_Direction(uchDirection,Axle_Count.Word);
            break;
      case DAC_UNIT_TYPE_DE:
      case DAC_UNIT_TYPE_LCWS:
      case DAC_UNIT_TYPE_LCWS_DL:
            SPI_Relay_A_InfoLU1_Fwd_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[37];
            SPI_Relay_A_InfoLU1_Fwd_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[38];

            SPI_Relay_A_InfoUS1_IN_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[39];
            SPI_Relay_A_InfoUS1_IN_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[40];

            SPI_Relay_A_InfoLU2_Fwd_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[41];
            SPI_Relay_A_InfoLU2_Fwd_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[42];

            SPI_Relay_A_InfoUS2_IN_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[43];
            SPI_Relay_A_InfoUS2_IN_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[44];

            SPI_Relay_A_InfoLU1_Rev_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[45];
            SPI_Relay_A_InfoLU1_Rev_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[46];

            SPI_Relay_A_InfoUS1_OUT_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[47];
            SPI_Relay_A_InfoUS1_OUT_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[48];

            SPI_Relay_A_InfoLU2_Rev_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[49];
            SPI_Relay_A_InfoLU2_Rev_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[50];

            SPI_Relay_A_InfoUS2_OUT_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[51];
            SPI_Relay_A_InfoUS2_OUT_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[52];

            SPI_Relay_B_InfoLU1_Fwd_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[53];
            SPI_Relay_B_InfoLU1_Fwd_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[54];

            SPI_Relay_B_InfoDS1_OUT_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[55];
            SPI_Relay_B_InfoDS1_OUT_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[56];

            SPI_Relay_B_InfoLU2_Fwd_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[57];
            SPI_Relay_B_InfoLU2_Fwd_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[58];

            SPI_Relay_B_InfoDS2_OUT_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[59];
            SPI_Relay_B_InfoDS2_OUT_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[60];

            SPI_Relay_B_InfoLU1_Rev_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[61];
            SPI_Relay_B_InfoLU1_Rev_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[62];

            SPI_Relay_B_InfoDS1_IN_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[63];
            SPI_Relay_B_InfoDS1_IN_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[64];

            SPI_Relay_B_InfoLU2_Rev_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[65];
            SPI_Relay_B_InfoLU2_Rev_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[66];

            SPI_Relay_B_InfoDS2_IN_Count.Byte.Lo = SPI_Receive_Object.Msg_Buffer[67];
            SPI_Relay_B_InfoDS2_IN_Count.Byte.Hi = SPI_Receive_Object.Msg_Buffer[68];

            Track_Info_2.LCWS_Total_Fwd_Peer_Count = SPI_Relay_A_InfoLU1_Fwd_Count.Word;
            Track_Info_3.LCWS_Total_Fwd_Peer_Count =  SPI_Relay_A_InfoUS1_IN_Count.Word;
            Track_Info_2.LCWS_Fwd_Peer_Count = SPI_Relay_A_InfoLU2_Fwd_Count.Word;
            Track_Info_3.LCWS_Fwd_Peer_Count =  SPI_Relay_A_InfoUS2_IN_Count.Word;
            Track_Info_2.LCWS_Total_Rev_Peer_Count = SPI_Relay_A_InfoLU1_Rev_Count.Word;
            Track_Info_3.LCWS_Total_Rev_Peer_Count = SPI_Relay_A_InfoUS1_OUT_Count.Word;
            Track_Info_2.LCWS_Rev_Peer_Count = SPI_Relay_A_InfoLU2_Rev_Count.Word;
            Track_Info_3.LCWS_Rev_Peer_Count = SPI_Relay_A_InfoUS2_OUT_Count.Word;

            Track_Info_1.LCWS_Total_Fwd_Peer_Count = SPI_Relay_B_InfoLU1_Fwd_Count.Word;
            Track_Info_4.LCWS_Total_Fwd_Peer_Count = SPI_Relay_B_InfoDS1_OUT_Count.Word;
            Track_Info_1.LCWS_Fwd_Peer_Count = SPI_Relay_B_InfoLU2_Fwd_Count.Word;
            Track_Info_4.LCWS_Fwd_Peer_Count = SPI_Relay_B_InfoDS2_OUT_Count.Word;
            Track_Info_1.LCWS_Total_Rev_Peer_Count = SPI_Relay_B_InfoLU1_Rev_Count.Word;
            Track_Info_4.LCWS_Total_Rev_Peer_Count =  SPI_Relay_B_InfoDS1_IN_Count.Word;
            Track_Info_1.LCWS_Rev_Peer_Count = SPI_Relay_B_InfoLU2_Rev_Count.Word;
            Track_Info_4.LCWS_Rev_Peer_Count =  SPI_Relay_B_InfoDS2_IN_Count.Word;
          break;
        default:
            break;
    }


}



void Update_SM_DS_Remote_Axle_Counts(BYTE DS_Count_Lo_byte,BYTE DS_Count_Hi_byte)
{
 SM_Message.DS_Remote_Axle_Count.Byte.Lo = DS_Count_Lo_byte;
 SM_Message.DS_Remote_Axle_Count.Byte.Hi = DS_Count_Hi_byte;

}

void Update_SM_US_Remote_Axle_Counts(BYTE US_Count_Lo_byte,BYTE US_Count_Hi_byte)
{
 SM_Message.US_Remote_Axle_Count.Byte.Lo = US_Count_Lo_byte;
 SM_Message.US_Remote_Axle_Count.Byte.Hi = US_Count_Hi_byte;

}

//29/07/10
UINT16 Get_DS_Remote_AxleCount(void)
{
 UINT16 uicount;
 uicount = (UINT16) SM_Message.DS_Remote_Axle_Count.Word;
 return(uicount);
}

//29/07/10
UINT16 Get_US_Remote_AxleCount(void)
{
   UINT16 uicount;
   uicount = (UINT16) SM_Message.US_Remote_Axle_Count.Word;
  return(uicount);
}

