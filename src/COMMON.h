/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    COMMON
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
*****************************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

/********* SSDAC CSCI Scheduler data elements **********************/

    /* To check the timeouts */
#define TIMEOUT_EVENT       (0)

    /* Macro for setting 250 ms timer for scheduling purpose */
#define DELAY_250MS         (250)

    /* Macro for setting 30ms timer for scheduling purpose */
#define DELAY_30MS          (30)

    /* Macro for setting 5ms timer for scheduling purpose */
#define DELAY_5MS           (5)

    /* Macro for setting 2ms timer for scheduling purpose */
#define DELAY_2MS           (2)

/********* SSDAC CSCI Scheduler data elements **********************/

#define FALSE               (0)
#define TRUE                (1)
#define SET_LOW             (0)
#define SET_HIGH            (1)

/************************Defines the datatype*************************/
typedef unsigned char BOOL;
typedef signed char SCHAR;
typedef unsigned char BYTE;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed long INT32;
typedef unsigned long UINT32;
/************************Defines the datatype*************************/

/************** Enumerator for direction *********************************/
typedef enum
        {
    /* Enumerator to check the forward direction of the axles */
         FORWARD_DIRECTION =1,

    /* Enumerator to check the reverse direction of the axles */
         REVERSE_DIRECTION,

    /* Enumerator to check whether the direction of the axles is not defined */
         DIRECTION_NOT_DEFINED
        }Wheel_Direction;
/************** Enumerator for direction *********************************/


/***************Other Macros used **************************************/
#define ZERO_OUT_IF_INVALID     (1)

#ifndef ROMSIZE
#define ROMSIZE                 131072
#endif

#define INPUT_PORT                  (1)
#define OUTPUT_PORT                 (0)

	/* Macro to define Message length for SPI communication */			
#define SPI_MESSAGE_LENGTH			(80)

    /* Macro to define Message length for USART communication */
#define COMM_MESSAGE_LENGTH         (22)

/* Macro to define Message length for Comm1 port USART communication*/
#define COMM1_MESSAGE_LENGTH            (22)

/* Macro to define Message length for Comm2 port USART communication*/
#define COMM2_MESSAGE_LENGTH            (22)

    /* Macro define the number of Phase detector cards */
#define NO_OF_PHASE_DETECTORS       (2)

/***************Other Macros used **************************************/



/****************** Unit Types and Address ranges **********************/
typedef enum
            {
            DAC_UNIT_TYPE_DE = 0,
    /* Enumerator to define SF Unit type of DAC system */
              DAC_UNIT_TYPE_SF , // 1

    /* Enumerator to define EF Unit type of DAC system */
              DAC_UNIT_TYPE_EF, // 2

    /* Enumerator to define CF Unit type of DAC system*/
              DAC_UNIT_TYPE_CF, // 3

    /* Enumerator to define D3A Unit type of DAC system */
              DAC_UNIT_TYPE_D3_A, // 4

    /* Enumerator to define D3B Unit type of DAC system */
              DAC_UNIT_TYPE_D3_B, // 5

    /* Enumerator to define D3C Unit type of DAC system */
              DAC_UNIT_TYPE_D3_C, // 6

    /* Enumerator to define 3D3S-SF Unit type of DAC system */
              DAC_UNIT_TYPE_3D_SF, // 7

    /* Enumerator to define 3D3S-EF Unit type of DAC system */
              DAC_UNIT_TYPE_3D_EF, // 8
                    
    /* Enumerator to define LCWS Unit type of DAC system */
              DAC_UNIT_TYPE_LCWS,   // 9

    /* Enumerator to define LCWS Unit type of DAC system Dual Line*/
              DAC_UNIT_TYPE_LCWS_DL,   // 10

    /* Enumerator to define D4A Unit type of DAC system */
              DAC_UNIT_TYPE_D4_A, // 11

    /* Enumerator to define D4B Unit type of DAC system */
              DAC_UNIT_TYPE_D4_B, // 12

    /* Enumerator to define D4C Unit type of DAC system */
              DAC_UNIT_TYPE_D4_C, // 13

    /* Enumerator to define D4D Unit type of DAC system */
              DAC_UNIT_TYPE_D4_D // 14

             }SSDAC_Unit_Type;

typedef enum
            {
              SSDAC = 1
            }Unit_Configuration;

typedef enum
            {
    /*Enumerator to define FSK communication type on UART module*/
              FSK_MODE = 0,
    /*Enumerator to define RS232 Serial port communication type on  UART module*/
              RS232_MODE
            }comm_type;

#define G39_DAC (1)

    /* Minimum address for SF unit is defined to 1 */
#define SF_UNIT_MIN_ADDRESS             (1)

    /* SF unit should communicate with the system which is having address
    higher than the its address, there fore assign the maximum SF unit
    address to 252*/
#define SF_UNIT_MAX_ADDRESS             (252)

    /* CF is the center unit between SF and EF there fore minimum CF unit
    address is defined to 3 */
#define CF_UNIT_MIN_ADDRESS             (3)

    /* CF unit can communicate with the system which is having address
    higher than the its address, there fore assign the maximum CF unit
    address to 252*/
#define CF_UNIT_MAX_ADDRESS             (252)

    /* EF unit should communicate with the system which is having address
    lower than the its address,there fore assign the maximum EF unit
    address to 3*/
#define EF_UNIT_MIN_ADDRESS             (3)

    /* Maximum address for EF unit system is defined to 254, with the dipswitch
    we can set maximum address of 255, because we are using two CPU therefore
    we are setting maximum address to 254 */
#define EF_UNIT_MAX_ADDRESS             (254)

#define D4_A_UNIT_MIN_ADDRESS           (1)
#define D4_A_UNIT_MAX_ADDRESS           (249)

#define D4_B_UNIT_MIN_ADDRESS           (3)
#define D4_B_UNIT_MAX_ADDRESS           (251)

#define D4_C_UNIT_MIN_ADDRESS           (5)
#define D4_C_UNIT_MAX_ADDRESS           (253)

#define D4_D_UNIT_MIN_ADDRESS           (7)
#define D4_D_UNIT_MAX_ADDRESS           (255)
            
            
            
/************************ Unit Types and Address ranges ************************/

/*************** COM1/COM2 Communication related definitions *****************/
#define RELEASE_MODEM_CHANNEL           (0)
#define GRAB_MODEM_CHANNEL              (1)
#define MODEM_CHANNEL_FREE              (0)
#define COM_DEST_ADDR_FIELD_OFFSET      (0)
#define COM_SRC_ADDR_FIELD_OFFSET       (1)
#define COM_DATA_ID_FIELD_OFFSET        (2)
#define COM_DAC_CONFIG_OFFSET           (3)
#define COM_FLAGS1_FIELD_OFFSET         (4)
#define COM_FLAGS2_FIELD_OFFSET         (5)
#define COM_FWD_AXLE_COUNT_LO_OFFSET    (6)
#define COM_FWD_AXLE_COUNT_HI_OFFSET    (7)
#define COM_REV_AXLE_COUNT_LO_OFFSET    (8)
#define COM_REV_AXLE_COUNT_HI_OFFSET    (9)
#define COM_AXLE_COUNT_LO_OFFSET        (10)
#define COM_AXLE_COUNT_HI_OFFSET        (11)
#define COM_R_FWD_AXLE_COUNT_LO_OFFSET    (12)
#define COM_R_FWD_AXLE_COUNT_HI_OFFSET    (13)
#define COM_R_REV_AXLE_COUNT_LO_OFFSET    (14)
#define COM_R_REV_AXLE_COUNT_HI_OFFSET    (15)

/*************** COM1/COM2 Communication related definitions *****************/




#define RESET_ID_OFFSET               (4)
#define COUNITNG_ID_OFFSET            (5)
#define CLEAR_COUNT_ID_OFFSET         (6)


/* Maximum communication retries assigned to 3 means 3 times we should check
    for communication established between the units */

#define MAXIMUM_COM_RETRIES             (3)
#define MAXIMUM_OCCUPIED_COM_RETRIES       (3)
    /*  This is the command used for sending/recieving the reset information */
#define READ_RESET_INFO                 0xAA

    /*  This is the command used for sending/recieving the axle counts */
#define READ_AXLE_COUNT                 0x55
#define ERROR_COUNT_THRESHOLD           0x00

#define VALID_ADDRESS                   (1)
#define INVALID_ADDRESS                 (0)
#define COMMUNICATION_OK                (1)
#define COMMUNICATION_FAILED            (0)
#define PHASE_DETECTOR_FAILED           (0)
#define RELAY_OK                        (1)
#define RELAY_FAILED                    (0)
#define BOARD_PRESENT                   (1)
#define BOARD_MISSING                   (0)
#define POWER_FAILED                    (0)
#define POWER_OK                        (1)
#define CATASTROPHIC_ERROR              (0)
#define NORMAL                          (1)
#define TRACK_OCCUPIED                  (0)
#define TRACK_CLEAR                     (1)
#define SM_RESET_PENDING                (0)
#define SM_HAS_RESETTED_SYSTEM          (1)
#define PREPARATORY_RESET_PENDING       (0)
#define PREPARATORY_RESET_COMPLETED     (1)
#define CRC32_CHECKSUM_BAD              (0)
#define CRC32_CHECKSUM_OK               (1)
#define RAM_CHECK_OK                    (1)
#define RAM_CHECK_FAILED                (0)
#define ATC_READY_TO_CLEAR              (1)
#define ATC_NOT_READY_T0_CLEAR          (0)
#define WRONG_CONFIGURATION             (0)
#define CONFIGURATION_OK                (0)


        /* Macro to check the Unit1 7 segment display */
#define SEVEN_SEGMENT_UNIT1             (1)

        /* Macro to check the Unit2 7 segment display */
#define SEVEN_SEGMENT_UNIT2             (2)

/************ Serial Communication related definitions (for 40MHz) ***********/

typedef enum
            {
    /* Enumerator to define Baud rate of 1200 */
               BAUDRATE_1200 =0,

    /* Enumerator to define Baud rate of 2400 */
               BAUDRATE_2400,

    /* Enumerator to define Baud rate of 9600 */
               BAUDRATE_9600,

    /* Enumerator to define Baud rate of 19200 */
               BAUDRATE_19200
            }Baud_Rate_Config;

/************ Serial Communication related definitions (for 40MHz) ***********/


/***************Macros related to USART communictaion *********************/

    /* The system generates FSK output for communicating with the other system*/
#define FSK_COMMUNICATION          (0)

    /* The system generates RS232 output for communicating with the other system*/
#define RS232_COMMUNICATION        (1)

    /* The system generates RS232 output for COMMA module and FSK ooutput on COMMB module*/
#define MIXED_MODE_COMM_1   (2)

    /* The system generates RS232 output for COMMB module and FSK ooutput on COMMA module*/
#define MIXED_MODE_COMM_2   (3)

    /* In asychronous mode for high speed transmission/reception set it 1*/
#define BRGH_FOR_1200                   (1)

    /* In asychronous mode for high speed transmission/reception set it 1*/
#define BRGH_FOR_2400                   (1)

    /* In asychronous mode for high speed transmission/reception set it 1*/
#define BRGH_FOR_9600                   (1)

    /* In asychronous mode for high speed transmission/reception set it 1*/
#define BRGH_FOR_19200                  (1)

    /* Enable the 16 bit baud rate register */
#define BRGH16_FOR_1200                 (1)

    /* Enable the 16 bit baud rate register */
#define BRGH16_FOR_2400                 (1)

    /* Enable the 16 bit baud rate register */
#define BRGH16_FOR_9600                 (1)

    /* Enable the 16 bit baud rate register */
#define BRGH16_FOR_19200                (1)

    /* In asychronous mode with 1200 baud rate,16 bit baud rate register,
    High speed,for 40Mhz crystal load the SPBRG register with 8332
    according to datasheet of PIC18F8722 */
#define SPBRG_FOR_1200                  (8332)

    /* In asychronous mode with 2400 baud rate, 16 bit baud rate register,
    High speed,for 40Mhz crystal load the SPBRG register with 4165
    according to datasheet of PIC18F8722 */
#define SPBRG_FOR_2400                  (4165)

    /* In asychronous mode with 9600 baud rate, 16 bit baud rate register,
     High speed,for 40Mhz crystal load the SPBRG register with 1040
     according to datasheet of PIC18F8722*/
#define SPBRG_FOR_9600                  (1040)

    /* In asychronous mode with 19200 baud rate, 16 bit baud rate register,
    High speed, for 40Mhz crystal load the SPBRG register with 520 according
    to datasheet of PIC18F8722 */
#define SPBRG_FOR_19200                 (520)

    /* Macro for setting the byte period of 9ms for 1200bps
    (1/1200 * 10 bits = 8.33ms ~9ms) */
#define BYTE_PERIOD_9MS                 (9)

    /* Macro for setting the byte period of 5ms for 2400bps
    (1/2400 * 10 = 4.16 ~ 5ms) */
#define BYTE_PERIOD_5MS                 (5)

    /* Macro for setting the byte period of 1ms for 9600bps,
    19200bps (1/9600 *10 = 1.04 ~1ms) */
#define BYTE_PERIOD_1MS                 (1)


/* Timeout for resetting the system at the bootup time*/
#define BOOTUP_RESET_TIMEOUT     (120)

/***************Macros related to USART communictaion *********************/

/* Process states - MESSAGE TRANSMITTER  */
#define NO_MSG_TO_XMIT                  (0)

#define LCWS_SECTION_OCCUPIED_TIMEOUT   12000 // 10 Mins (for a timer of 50mS)
            
/***************************LED states **************************************/
typedef enum {
    /* Enumerator to check the status of LED is ON */
             ON =0,

    /* Enumerator to check the status of LED is OFF */
             OFF

             }Led_State;

/***************************LED states ***************************************/

/**************COMMUNICATION SCHEDULER Process states *********************/

typedef enum {
    /* Enumerator to indicate that communication Scheduling is not yet started */
            COMM_SCHEDULER_NOT_STARTED = 0,

    /* Enumerator to indicate that communication scheduler is IDLE */
            COMM_SCHEDULER_IDLE,

    /* Enumerator to check for modem channel free */
            GET_MODEM_CHANNEL,

    /* Enumerator to set the RS LOW for transmission */
            SET_MODEM_TX_MODE,

    /* Enumerator to wait for Data Encryption */
            WAIT_FOR_DATA_ENCRYPT,

    /* Enumerator to wait for carrier to stabilize */
            WAIT_FOR_CARRIER_STABALISE,

    /* Enumerator to transmit dummy bytes */
            TRANSMIT_DUMMY_BYTES,

    /* Enumerator to transmit the first telegram */
            TRANSMIT_TELEGRAM,

    /* Enumerator to Hold Modem RS low */
            HOLD_MODEM_RS_LOW,

    /* Enumerator to check for error modem without the carrier*/
            ERROR_MODEM_NO_CARRIER
} comm_sch_state_t;



typedef enum {

            MODEM_IDLE = 0,
            GET_MODEM_LINE,
            SET_MODEM_ZP_MODE,
            DELAY_FOR_CONFIGURE,
            CONFIGURATION_COMPLETED
}modem_state;
/**************COMMUNICATION SCHEDULER Process states *********************/

/************MESSAGE RECEIVER COM Process states***************************/

typedef enum
            {
    /* Enumerator to indicate the recieve buffer empty */
            COM_RECV_BUFFER_EMPTY =0,

    /* Enumerator to get the Source address */
            COM_RECV_SRC_ADDR,

    /* Enumerator to get the data ID */
            COM_RECV_DATA_ID,

    /* Enumerator to get the data bytes */
            COM_RECV_DATA_BYTES,

    /* Enumerator to decrypt the data bytes */
            COM_DECRYPT_DATA_BYTES,

    /* Enumerator to check for CRC */
            COM_CHECK_CRC16,

    /* Enumerator to check the valid message */
            COM_VALID_MESSAGE,

    /* Enumerator to check the Invalid message */
            COM_INVALID_MESSAGE

            }com_receive_state;

/************MESSAGE RECEIVER COM Process states**************************/

/****************Relay Process states ***************************/
typedef enum {
    /* Enumerator to indicate that relay manager is not started */
            RELAY_MANAGER_NOT_STARTED = 0,

    /* Enumerator to wait for remote clear in ATC */
            ATC_WAIT_FOR_REMOTE_CLEAR,

    /* Enumerator to wait for pilot train */
            WAIT_FOR_PILOT_TRAIN,

    /* Enumerator to pilot train in section */
            PILOT_TRAIN_IN_SECTION,

    /* Enumerator to no train in section */
            NO_TRAIN_IN_SECTION,

    /* Enumerator to train in section */
            TRAIN_IN_SECTION,

    /* Enumerator to check for DAC defective */
            DAC_DEFECTIVE,

    /* Enumerator to check for DAC reset process */
            DAC_RESET_PROGRESS,

    /* Enumerator to check the repeater relay ON */
            DAC_REPEATER_RELAY_ON,

    /* Enumerator to check the repeater relay OFF */
            DAC_REPEATER_RELAY_OFF,

   /* Enumerator to indicate that relay manager is not started */
            RELAY_MANAGER_IDLE

} relay_state_type_t;

/****************Relay Process states ***************************/

/****************Data Encryption states ***************************/
typedef enum{
            ENCRYPTION_IDLE = 0,
            START_ENCRYPTION,
            ENCRYPT_KEY_ADDITION,
            ENCRYPT_FAILURE,
            ENCRYPTION_COMLETED
}enciper_states;
/****************Data Encryption states ***************************/

/****************Data Decryption states ***************************/
typedef enum{
            DECRYPTION_IDLE = 0,
            START_DECRYPTION,
            DECRYPT_KEY_ADDITION,
            DECRYPT_FAILURE,
            DECRYPTION_COMPLETED
}deciper_states;


/****************Data Encryption states ***************************/

#define LARGEST_CONVERTABLE_INTEGER     10000       //Not used

/************** COM Process time-outs in milli-seconds************/

    /* for 1200 bps, increase in case of 600 bps */
//#define COMM_SCHEDULER_SCAN_RATE      (2400)

#define COMM_SCHEDULER_SCAN_RATE        (1200)

/* scheduler scan rate for 1200bps */


//  /* 186 for MSM6947, 206 for MSM6927 */
//#define MODEM_TRC_ON_TIMEOUT          (186)

    /* for 1200 bps, increase to 36 in case of 600 bps */
#define MODEM_HOLD_RS_LOW_TIMEOUT       (30)

/* Time interval between two telegrams */
#define TIME_BETWEEN_TWO_TELEGRAMS      (20)

     /* for 1200 bps */
#define COM_INVALID_MESSAGE_TIMEOUT     (18)

//  /* 2 transmission slots time  (2 * 550) */
#define WAIT_FOR_RS_LOW_TIME            (300)

    /* 2 Communication Cycle time (2 * 1600) */
#define COMM_RE_SYNCHRONIZATION_TIME    (3400)



//#define COMM_RE_SYNCHRONIZATION_TIME  (4800)

    /*  Transmit wait time is 50ms */
#define TRANSMIT_WAIT_TIME              (50)
#define PACKET_ERROR_TIME_THRESHOLD     (50000)
/****************** COM Process time-outs in milli-seconds**********/


typedef union {
    UINT16 Word;
    struct  {
        BYTE    Lo;     /* Low Byte (little-Endian) */
        BYTE    Hi;     /* High Byte (little-Endian)*/
    } Byte ;
} wordtype_t;

typedef union {
    UINT32 LWord;
    struct  {
        wordtype_t  LoWord;     /* Low Word (little-Endian) */
        wordtype_t  HiWord;     /* High Word (little-Endian) */
    } DWord ;
} longtype_t;

typedef union {
    struct  {
        unsigned b0 : 1;        /* Bit 0 */
        unsigned b1 : 1;        /* Bit 1 */
        unsigned b2 : 1;        /* Bit 2 */
        unsigned b3 : 1;        /* Bit 3 */
        unsigned b4 : 1;        /* Bit 4 */
        unsigned b5 : 1;        /* Bit 5 */
        unsigned b6 : 1;        /* Bit 6 */
        unsigned b7 : 1;        /* Bit 7 */
    } Bit ;
    BYTE Byte;
} bitadrb_t;

typedef union {
    struct  {
        unsigned b0  : 1;       /* Bit 0  */
        unsigned b1  : 1;       /* Bit 1  */
        unsigned b2  : 1;       /* Bit 2  */
        unsigned b3  : 1;       /* Bit 3  */
        unsigned b4  : 1;       /* Bit 4  */
        unsigned b5  : 1;       /* Bit 5  */
        unsigned b6  : 1;       /* Bit 6  */
        unsigned b7  : 1;       /* Bit 7  */
        unsigned b8  : 1;       /* Bit 8  */
        unsigned b9  : 1;       /* Bit 9  */
        unsigned b10 : 1;       /* Bit 10 */
        unsigned b11 : 1;       /* Bit 11 */
        unsigned b12 : 1;       /* Bit 12 */
        unsigned b13 : 1;       /* Bit 13 */
        unsigned b14 : 1;       /* Bit 14 */
        unsigned b15 : 1;       /* Bit 15 */
    } Bit ;
    UINT16 Word;
} bitadrw_t;

/******************Structure to hold system status ***********/
typedef union {
    struct {
        unsigned System_Status     :1;      /* Byte 1, Bit 0 */
        unsigned Network_Addr      :1;      /* Byte 1, Bit 1 */
        unsigned Flash_CheckSum    :1;      /* Byte 1, Bit 2 */
        unsigned Unit_Board_Status :1;      /* Byte 1, Bit 3 */
        unsigned Local_Reset_Done  :1;      /* Byte 1, Bit 4 */
        unsigned US_Track_Status   :1;      /* Byte 1, Bit 5 */
        unsigned DS_Track_Status   :1;      /* Byte 1, Bit 6 */
        unsigned Local_Reset_Done2 :1;      /* Byte 1, Bit 7 */
        unsigned Direct_Out_Count  :1;      /* Byte 2, Bit 0 */
        unsigned PD1_Status        :1;      /* Byte 2, Bit 1 */
        unsigned PD2_Status        :1;      /* Byte 2, Bit 2 */
        unsigned Vital_Relay_A     :1;      /* Byte 2, Bit 3 */
        unsigned Vital_Relay_B     :1;      /* Byte 2, Bit 4 */
        unsigned Preparatory_Relay :1;      /* Byte 2, Bit 5 */
        unsigned Preparatory_Reset_US :1;   /* Byte 2, Bit 6 */
        unsigned Local_Power_Status :1;     /* Byte 2, Bit 7 */
        unsigned                    :1;     /* Byte 3, Bit 0 */
        unsigned Peer_CPU_Link :1;          /* Byte 3, Bit 1 */
        unsigned Power_Fail_at_US1 :1;      /* Byte 3, Bit 2 */
        unsigned Power_Fail_at_US2 :1;      /* Byte 3, Bit 3 */
        unsigned Power_Fail_at_DS1 :1;      /* Byte 3, Bit 4 */
        unsigned Power_Fail_at_DS2 :1;      /* Byte 3, Bit 5 */
        unsigned DS_System_Status :1;       /* Byte 3, Bit 6 */
        unsigned US_System_Status :1;       /* Byte 3, Bit 7 */
        unsigned LU1_to_US1_Link  :1;       /* Byte 4, Bit 0 */
        unsigned LU1_to_US2_Link  :1;       /* Byte 4, Bit 1 */
        unsigned LU1_to_DS1_Link  :1;       /* Byte 4, Bit 2 */
        unsigned LU1_to_DS2_Link  :1;       /* Byte 4, Bit 3 */
        unsigned US1_to_LU1_Link  :1;       /* Byte 4, Bit 4 */
        unsigned US2_to_LU1_Link  :1;       /* Byte 4, Bit 5 */
        unsigned DS1_to_LU1_Link  :1;       /* Byte 4, Bit 6 */
        unsigned DS2_to_LU1_Link  :1;       /* Byte 4, Bit 7 */
        unsigned LU2_to_US1_Link  :1;       /* Byte 5, Bit 0 */
        unsigned LU2_to_US2_Link  :1;       /* Byte 5, Bit 1 */
        unsigned LU2_to_DS1_Link  :1;       /* Byte 5, Bit 2 */
        unsigned LU2_to_DS2_Link  :1;       /* Byte 5, Bit 3 */
        unsigned US1_to_LU2_Link  :1;       /* Byte 5, Bit 4 */
        unsigned US2_to_LU2_Link  :1;       /* Byte 5, Bit 5 */
        unsigned DS1_to_LU2_Link  :1;       /* Byte 5, Bit 6 */
        unsigned DS2_to_LU2_Link  :1;       /* Byte 5, Bit 7 */
        unsigned PD1_Board_Present     :1;      /* Byte 6, Bit 0 */
        unsigned PD2_Board_Present     :1;      /* Byte 6, Bit 1 */
        unsigned SM_Cpu_Present        :1;      /* Byte 6, Bit 2 */
        unsigned Modem_Present         :1;      /* Byte 6, Bit 3 */
        unsigned Relay_Drive_A_Present :1;      /* Byte 6, Bit 4 */
        unsigned Relay_Drive_B_Present :1;      /* Byte 6, Bit 5 */
        unsigned                       :1;      /* Byte 6, Bit 6 */
        unsigned                       :1;      /* Byte 6, Bit 7 */
        unsigned Modem_A_Configured    :1;      /* Byte 7, Bit 0 */
        unsigned Modem_B_Configured    :1;  /* Byte 7, Bit 1 */
        unsigned Modem_A_Err_Toggle_Bit:1;  /* Byte 7, Bit 2 */
        unsigned Modem_B_Err_Toggle_Bit:1;  /* Byte 7, Bit 3 */
        unsigned RAM_Check             :1;  /* Byte 7, Bit 4 */
        unsigned Preparatory_Relay1    :1;  /* Byte 7, Bit 5 */
        unsigned Preparatory_Reset_DS  :1;  /* Byte 7, Bit 6 */
        unsigned Configuration         :1;                  /* Byte 7, Bit 7 */
        } Flags;
    BYTE Byte[7];
} dac_status_t;

/******************Structure to hold system status ***********/

/*****Structure to hold the dip switch information regarding unit type,
                        address, baud rate, peer address ***********/

typedef struct {

        /* Unit configuration can be either SSDAC */
            Unit_Configuration   Configuration;

        /* Unit Address */
            BYTE             Address;

        /* SF/CF/EF/D3/DE */
            SSDAC_Unit_Type  DAC_Unit_Type;

        /* FSK or RS232 */
            comm_type COM1_Mode;
            comm_type COM2_Mode;

        /* 1200/2400/9600/19200 */
            Baud_Rate_Config Baud_Rate;

        /* Address of Peer CPU */
            BYTE             Peer_Address;

        /* Network Address Comm 1*/
            BYTE COM1_NW_Address;

        /* Network Address Comm 2*/
            BYTE COM2_NW_Address;

        

            struct {

            /*Bit assigned for checking the CPU1 or CPU2 of unit type DAC.If this bit is High
            means that is CPU1 else CPU2 */
                unsigned Is_DAC_CPU1     : 1;

            /* Bit assigned for checking the ATC enable or disable.If this bit
            disabled in SF/ EF configuration it will wait for pilot train.So
            energise the preparatory relay.If this bit is enabled means it will
            go directly to clear mode after reset is applied */
                unsigned ATC_Enabled : 1;

            /* Bit assigned for checking the IBS enable or disable.In the IBH
            configuration with repeat relay in SF/EF configuration energise the
            repeater relay */
                unsigned IBS_Enabled : 1;

            /* In IBS configuration RESET at CF can be done by applying resets
             to Both remote units */
                unsigned Remote_Reset_for_IBS : 1;

                /* In MSDAC configuration FDP CPU No. */
                unsigned Is_FDP_CPU1 : 1;

                unsigned            :3;
                } Flags;
} dip_switch_info_t;

/*****Structure to hold the dip switch information regarding unit type,
                        address, baud rate, peer address ***********/

/************ Structure to hold message for transsmission/reception **********/
typedef struct {

    /* Variable to store the different communication transmission
     and reception state */
            BYTE    State;

    /* Variable to store the transmission and reception timeout */
            UINT16  Timeout_ms;

    /* Variable to store the byte period */
            BYTE    BytePeriod;

    /* The length of the transmission and reception massage buffer */
            BYTE    Msg_Length;

    /* Variable to store the Transmission Time */
            BYTE    Transmission_Time;

    /* Variable to store the Consecutive error counts in communication*/
            BYTE    Consecutive_Error_Count;

    /* Variable to access the particular byte from massage buffer */
            BYTE    Index;

    /* Massage buffer to store the massage */
            BYTE    Msg_Buffer[COMM_MESSAGE_LENGTH];

} msg_info_t;



/************ Structure to hold message for transsmission/reception **********/

/************ Structure to hold the communictaion scheduler states *********/

typedef struct {

    /* State to store the different communication scheduler states */
            comm_sch_state_t    State;

    /*  Variable to store the communictaion scheduler timeout */
            UINT16  Timeout_ms;

    /*  Variable to store the communiction scheduler scan period */
            UINT16  ScanPeriod;

    /* Variable to store the communiction scheduler Elasp time */
            UINT16  ElapsedTime;


} comm_sch_info_t;

typedef struct{

            modem_state   State;

            UINT16  Timeout_ms;

        }modem_info_t;
/************ Structure to hold the communictaion scheduler states *********/


/**********Structure to hold the US1 to LU1, LU2 and US2 to LU1, LU2
     communication link status***********/

typedef struct {

    /* Variable to check the remote unit CPU1 to Local unit CPU1 communication
    link */
            BYTE US1_to_LU1;

    /* Variable to check the remote unit CPU2 to Local unit CPU1 communication
    link*/
            BYTE US2_to_LU1;

    /* Variable to check the remote unit CPU1 to Local unit CPU2 communication
    link*/
            BYTE US1_to_LU2;

    /* Variable to check the remote unit CPU1 to Local unit CPU2 communication
    link */
            BYTE US2_to_LU2;

} comm_a_countdown_t;

/**********Structure to hold the US1 to LU1, LU2 and US2 to LU1, LU2
     communication link status***********/

/******************Structure to hold the DS1 to LU1, LU2 and
     DS2 to LU1, LU2 communication link status***********/

typedef struct {

    /* Variable to check the remote unit CPU1 to Local unit CPU1 communication
     link */
            BYTE DS1_to_LU1;

    /* Variable to check the remote unit CPU2 to Local unit CPU1 communication
     link */
            BYTE DS2_to_LU1;

    /* Variable to check the remote unit CPU1 to Local unit CPU2 communication
     link */
            BYTE DS1_to_LU2;

    /* Variable to check the remote unit CPU1 to Local unit CPU2 communication
     link  */
            BYTE DS2_to_LU2;

} comm_b_countdown_t;

/******************Structure to hold the DS1 to LU1, LU2 and
     DS2 to LU1, LU2 communication link status***********/

/********** Structure to hold the Relay A direction, in and out counts********/
typedef struct {

    /* Variable to get the relay states */
            relay_state_type_t State;

    /* Variable to check the Next block CPU1 state in repeater relay
    configuration */
            BYTE   Next_Block_CPU1_Status;

    /* Variable to check the Next block CPU2 state in repeater relay
    configuration*/
            BYTE   Next_Block_CPU2_Status;

    /* Variable to Check the Local state */
            BYTE   ATC_Local_State;

    /* Variable to Check the remote unit state */
            BYTE   ATC_Remote1_State;

    /* Variable to check the remote unit state */
            BYTE   ATC_Remote2_State;

    /* Variable to store the remote unit CPU1 direction */
            BYTE   US1_Direction;

    /* Variable to store the remote unit CPU2 direction */
            BYTE   US2_Direction;

    /* Variable to store the Local unit CPU1 direction */
            BYTE   LU1_Direction;

    /* Variable to store the Local unit CPU2 direction */
            BYTE   LU2_Direction;

    /*  Variable to store the US1 axle counts */
            UINT16 US1_Axle_Count;

    /*  Variable to store the US2 axle counts */
            UINT16 US2_Axle_Count;

    /*  Variable to store the US Local counts */
            UINT16 US_Local_Count;

    /* Variable to store the remote unit CPU1 In count */
            UINT16 US1_IN_Count;

    /* Variable to store the remote unit CPU2 In count */
            UINT16 US2_IN_Count;

    /* Variable to store the remote unit CPU1 Out count */
            UINT16 US1_OUT_Count;

    /* Variable to store the remote unit CPU2 Out count */
            UINT16 US2_OUT_Count;

    /* Variable to store the Local unit CPU1 Forward count */
            UINT16 LU1_Fwd_Count;

    /* Variable to store the Local unit CPU2 Forward count */
            UINT16 LU2_Fwd_Count;

    /* Variable to store the Local unit CPU1 reverse count */
            UINT16 LU1_Rev_Count;

    /* Variable to store the Local unit CPU2 reverse count */
            UINT16 LU2_Rev_Count;

} relay_a_info_t;

/********** Structure to hold the Relay A direction, in and out counts********/

/***********Structure to hold the Relay B direction, in and out counts*******/

typedef struct {

    /* Variable to get the relay states */
            relay_state_type_t State;

    /* Variable to check the Previous block CPU1 state in repeater relay
    configuration */
            BYTE   Previous_Block_CPU1_Status;

    /* Variable to check the Previous block CPU2 state in repeater relay
    configuration */
            BYTE   Previous_Block_CPU2_Status;

    /* Variable to Check the Local state */
            BYTE   ATC_Local_State;

    /* Variable to Check the remote unit state */
            BYTE   ATC_Remote1_State;

    /* Variable to check the remote unit state */
            BYTE   ATC_Remote2_State;

    /* Variable to store the remote unit CPU1 direction */
            BYTE   DS1_Direction;

    /* Variable to store the remote unit CPU2 direction */
            BYTE   DS2_Direction;

    /* Variable to store the Local unit CPU1 direction */
            BYTE   LU1_Direction;

    /* Variable to store the Local unit CPU2 direction */
            BYTE   LU2_Direction;

    /* Variable to store the DS1 axle counts */
            UINT16 DS1_Axle_Count;

    /* Variable to store the DS2 axle counts */
            UINT16 DS2_Axle_Count;

    /* Variable to store the DS local counts */
            UINT16 DS_Local_Count;

    /* Variable to store the remote unit CPU1 Out count */
            UINT16 DS1_OUT_Count;

    /* Variablr to store the remote unit CPU2 Out count */
            UINT16 DS2_OUT_Count;

    /*  Variable to store the remote unit CPUI1 In count */
            UINT16 DS1_IN_Count;

    /* Varaible to store the remote unit CPU2 In count */
            UINT16 DS2_IN_Count;

    /* Variable to store the Local unit CPU1 forward count */
            UINT16 LU1_Fwd_Count;

    /* Variable to store the Local unit CPU2 forward count */
            UINT16 LU2_Fwd_Count;

    /* Variable to store the Local unit CPU1 reverse count */
            UINT16 LU1_Rev_Count;

    /* Variable to store the Local unit CPU2 reverse count */
            UINT16 LU2_Rev_Count;

} relay_b_info_t;

/***********Structure to hold the Relay B direction, in and out counts*******/

/********* Structure to hold the Relay D3 direction, in and out counts*******/
typedef struct {
    /* Variable to get the relay states */
            relay_state_type_t State;

        /* Variable to Check the Local state */
            BYTE   ATC_Local_State;

    /* Variable to Check the remote unit state */
            BYTE   ATC_Remote1_State;

    /* Variable to check the remote unit state */
            BYTE   ATC_Remote2_State;

    /* Variable to Check the remote unit state */
            BYTE   ATC_Remote3_State;

    /* Variable to check the remote unit state */
            BYTE   ATC_Remote4_State;

    /* Variable to store the 3D unit A CPU1 direction */
            BYTE   A1_Direction;

    /* Variable to store the 3D unit A CPU2 direction */
            BYTE   A2_Direction;

    /* Variable to store the 3D unit B CPU1 direction */
            BYTE   B1_Direction;

    /* Variable to store the 3D unit B CPU2 direction */
            BYTE   B2_Direction;

    /* Variable to store the 3D unit C CPU1 direction */
            BYTE   C1_Direction;

    /* Variable to store the 3D unit C CPU2 direction */
            BYTE   C2_Direction;

    /* Variable to store the local direction */
            BYTE   Local_Direction;

    /* Variable to store the local count */
            UINT16 Local_Count;

    /* Variable to store the 3D unit A CPU1 count */
            UINT16 A1_IN_Axle_Count;

    /* Variable to store the 3D unit A CPU2 count*/
            UINT16 A2_IN_Axle_Count;

    /* Variable to store the 3D unit A CPU1 count */
            UINT16 A1_OUT_Axle_Count;

    /* Variable to store the 3D unit A CPU2 count*/
            UINT16 A2_OUT_Axle_Count;

    /* Variable to store the 3D unit B CPU1 count*/
            UINT16 B1_IN_Axle_Count;

    /* Variable to store the 3D unit B CPU2 count*/
            UINT16 B2_IN_Axle_Count;

    /* Variable to store the 3D unit A CPU1 count */
            UINT16 B1_OUT_Axle_Count;

    /* Variable to store the 3D unit A CPU2 count*/
            UINT16 B2_OUT_Axle_Count;


    /* Variable to store the 3D unit C CPU1 count*/
            UINT16 C1_IN_Axle_Count;

    /* Variable to store the 3D unit C CPU2 count*/
            UINT16 C2_IN_Axle_Count;

    /* Variable to store the 3D unit A CPU1 count */
            UINT16 C1_OUT_Axle_Count;

    /* Variable to store the 3D unit A CPU2 count*/
            UINT16 C2_OUT_Axle_Count;

    /* Variable to store the 3D unit A CPU1 Out count s*/
            UINT16 A1_OUT_Count;

    /* Variable to store the 3D unit A CPU2 Out count */
            UINT16 A2_OUT_Count;

    /* Variable to store the 3D unit A CPU1 In count s*/
            UINT16 A1_IN_Count;

    /* Variable to store the 3D unit A CPU2 In count */
            UINT16 A2_IN_Count;

    /* Variable to store the 3D unit B CPU1 Out count */
            UINT16 B1_OUT_Count;

    /* Variable to store the 3D unit B CPU2 Out count */
            UINT16 B2_OUT_Count;

    /* Variable to store the 3D unit B CPU1 In count */
            UINT16 B1_IN_Count;

    /* Variable to store the 3D unit B CPU2 In count */
            UINT16 B2_IN_Count;

    /* Variable to store the 3D unit C CPU1 Out count */
            UINT16 C1_OUT_Count;

    /* Variable to store the 3D unit C CPU2 Out count */
            UINT16 C2_OUT_Count;

    /* Variable to store the 3D unit C CPU1 In count */
            UINT16 C1_IN_Count;

    /* Variable to store the 3D unit C CPU2 In count */
            UINT16 C2_IN_Count;

     /*variable to store 50ms timer for track clearing timeout for 3D*/
            BYTE   Track_Clearing_Timeout_50ms;
} relay_d3_info_t;
/********* Structure to hold the Relay D3 direction, in and out counts*******/

/********* Structure to hold the Relay D4 direction, in and out counts*******/
typedef struct {
    /* Variable to get the relay states */
            relay_state_type_t State;

        /* Variable to Check the Local state */
            BYTE   ATC_Local_State;

    /* Variable to Check the remote unit state */
            BYTE   ATC_Remote1_State;

    /* Variable to check the remote unit state */
            BYTE   ATC_Remote2_State;

    /* Variable to Check the remote unit state */
            BYTE   ATC_Remote3_State;

    /* Variable to check the remote unit state */
            BYTE   ATC_Remote4_State;

    /* Variable to store the 4D unit A CPU1 direction */
            BYTE   A1_Direction;

    /* Variable to store the 4D unit A CPU2 direction */
            BYTE   A2_Direction;

    /* Variable to store the 4D unit B CPU1 direction */
            BYTE   B1_Direction;

    /* Variable to store the 4D unit B CPU2 direction */
            BYTE   B2_Direction;

    /* Variable to store the 4D unit C CPU1 direction */
            BYTE   C1_Direction;

    /* Variable to store the 4D unit C CPU2 direction */
            BYTE   C2_Direction;

    /* Variable to store the 4D unit D CPU1 direction */
            BYTE   D1_Direction;

    /* Variable to store the 4D unit D CPU2 direction */
            BYTE   D2_Direction;

    /* Variable to store the local direction */
            BYTE   Local_Direction;

    /* Variable to store the local count */
            UINT16 Local_Count;

    /* Variable to store the 4D unit A CPU1 count */
            UINT16 A1_IN_Axle_Count;

    /* Variable to store the 4D unit A CPU2 count*/
            UINT16 A2_IN_Axle_Count;

    /* Variable to store the 4D unit A CPU1 count */
            UINT16 A1_OUT_Axle_Count;

    /* Variable to store the 4D unit A CPU2 count*/
            UINT16 A2_OUT_Axle_Count;

    /* Variable to store the 4D unit B CPU1 count*/
            UINT16 B1_IN_Axle_Count;

    /* Variable to store the 4D unit B CPU2 count*/
            UINT16 B2_IN_Axle_Count;

    /* Variable to store the 4D unit A CPU1 count */
            UINT16 B1_OUT_Axle_Count;

    /* Variable to store the 4D unit A CPU2 count*/
            UINT16 B2_OUT_Axle_Count;


    /* Variable to store the 4D unit C CPU1 count*/
            UINT16 C1_IN_Axle_Count;

    /* Variable to store the 4D unit C CPU2 count*/
            UINT16 C2_IN_Axle_Count;

    /* Variable to store the 4D unit A CPU1 count */
            UINT16 C1_OUT_Axle_Count;

    /* Variable to store the 4D unit A CPU2 count*/
            UINT16 C2_OUT_Axle_Count;

    /* Variable to store the 4D unit C CPU1 count*/
            UINT16 D1_IN_Axle_Count;

    /* Variable to store the 4D unit C CPU2 count*/
            UINT16 D2_IN_Axle_Count;

    /* Variable to store the 4D unit A CPU1 count */
            UINT16 D1_OUT_Axle_Count;

    /* Variable to store the 4D unit A CPU2 count*/
            UINT16 D2_OUT_Axle_Count;

    /* Variable to store the 4D unit A CPU1 Out count s*/
            UINT16 A1_OUT_Count;

    /* Variable to store the 4D unit A CPU2 Out count */
            UINT16 A2_OUT_Count;

    /* Variable to store the 4D unit A CPU1 In count s*/
            UINT16 A1_IN_Count;

    /* Variable to store the 4D unit A CPU2 In count */
            UINT16 A2_IN_Count;

    /* Variable to store the 4D unit B CPU1 Out count */
            UINT16 B1_OUT_Count;

    /* Variable to store the 4D unit B CPU2 Out count */
            UINT16 B2_OUT_Count;

    /* Variable to store the 4D unit B CPU1 In count */
            UINT16 B1_IN_Count;

    /* Variable to store the 4D unit B CPU2 In count */
            UINT16 B2_IN_Count;

    /* Variable to store the 4D unit C CPU1 Out count */
            UINT16 C1_OUT_Count;

    /* Variable to store the 4D unit C CPU2 Out count */
            UINT16 C2_OUT_Count;

    /* Variable to store the 4D unit C CPU1 In count */
            UINT16 C1_IN_Count;

    /* Variable to store the 4D unit C CPU2 In count */
            UINT16 C2_IN_Count;

    /* Variable to store the 4D unit C CPU1 Out count */
            UINT16 D1_OUT_Count;

    /* Variable to store the 4D unit C CPU2 Out count */
            UINT16 D2_OUT_Count;

    /* Variable to store the 4D unit C CPU1 In count */
            UINT16 D1_IN_Count;

    /* Variable to store the 4D unit C CPU2 In count */
            UINT16 D2_IN_Count;

     /*variable to store 50ms timer for track clearing timeout for 4D*/
            BYTE   Track_Clearing_Timeout_50ms;
} relay_d4_info_t;
/********* Structure to hold the Relay D4 direction, in and out counts*******/

/********* Structure to hold the checksum information**************************/
typedef union{
      struct{
              longtype_t DS1_Checksum;
              longtype_t DS2_Checksum;
              UINT32 LWord;
              UINT32 LWord1;
              longtype_t Local_Checksum;
              struct{
                     unsigned DS1_Checksum_Comparison_Ok    :1;
                     unsigned DS2_Checksum_Comparison_Ok    :1;
                     unsigned DS_Checksum_Comparison_Ok     :1;
                     unsigned  :5;
                    }Flags;
             }SF;
      struct{
              longtype_t DS1_Checksum;
              longtype_t DS2_Checksum;
              longtype_t US1_Checksum;
              longtype_t US2_Checksum;
              longtype_t Local_Checksum;
               struct{
                     unsigned DS1_Checksum_Comparison_Ok    :1;
                     unsigned DS2_Checksum_Comparison_Ok    :1;
                     unsigned DS_Checksum_Comparison_Ok     :1;
                     unsigned US1_Checksum_Comparison_Ok    :1;
                     unsigned US2_Checksum_Comparison_Ok    :1;
                     unsigned US_Checksum_Comparison_Ok :1;
                     unsigned  :2;
                    }Flags;
             }CF;

      struct{
              UINT32 LWord;
              UINT32 LWord1;
              longtype_t US1_Checksum;
              longtype_t US2_Checksum;
              longtype_t Local_Checksum;
                struct{
                        unsigned  :3;
                        unsigned US1_Checksum_Comparison_Ok :1;
                        unsigned US2_Checksum_Comparison_Ok :1;
                        unsigned US_Checksum_Comparison_Ok  :1;
                        unsigned  :2;
                       }Flags;
             }EF;
      BYTE Byte[21];
}checksum_info_t;
/********* Structure to hold the checksum information**************************/

/********* Structure to hold the variable for bootup timeout**************************/

typedef struct{
           UINT16 Timeout_50ms;
  }bootup_lock;
/********* Structure to hold the variable for bootup timeout**************************/

/********* Structure to hold the variable for data encryption**************************/
typedef struct{
            enciper_states  State;
            BYTE Timeout_ms;

}enciper_info;
/********* Structure to hold the variable for data encryption**************************/

/********* Structure to hold the variable for data decryption**************************/
typedef struct{
            deciper_states  State;
            BYTE Timeout_ms;

}deciper_info;
/********* Structure to hold the variable for data decryption**************************/

typedef struct{

    //  BYTE Buffer[16];
        BYTE Encrypt_key[16];
        BYTE Decrypt_key[16];

}ciper_message_info;

typedef struct{

  wordtype_t DS_Remote_Axle_Count;
  wordtype_t US_Remote_Axle_Count;
}sm_message;

struct def_LU_Speed_Info
{
    BYTE CountingEnableLatch;
    BYTE CountingEnabled;
    UINT16 TimerValue;
    UINT16 SpeedValue;
};

extern struct def_LU_Speed_Info LU_Speed_Info;

extern BYTE ErrorCodeForSPI;

union def_SPI_Failure
{
    unsigned int Failure_Data;
    struct bits
    {
        unsigned int US_Failure:1;
        unsigned int DS_Failure:1;
        unsigned int LOCAL_Failure:1;
        unsigned int unused:13;
    } fail_bits;
};

extern union def_SPI_Failure SPI_Failure;

extern void Decrement_Bootuplock_50msTmr(void);

/* Declaration for functions defined in relaymgr.c */
extern void Declare_DAC_Defective(void);
extern void Declare_DAC_Defective_US(void);
extern void Declare_DAC_Defective_DS(void);
extern void Process_Peer_Relay_A_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);
extern void Process_Peer_Relay_B_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);
extern void  Process_Peer_Relay_A_Direction( BYTE uchDirection);
extern void  Process_Peer_Relay_B_Direction( BYTE uchDirection);
extern UINT16 Get_US_Remote_AxleCount(void);
extern UINT16 Get_DS_Remote_AxleCount(void);
extern UINT16 LCWS_Section_TimeOut;

/* Declaration for functions defined in rlya_mgr.c */
extern BOOL Reset_Allowed_For_US(void);
extern void Set_Relay_A_DAC_Defective(void);
extern BYTE Get_Relay_A_State(void);
extern BYTE Get_Relay_A_ATC_State(void);
extern void Set_Relay_A_Reset_State(void);
extern void Update_A_US1_IN_Count (UINT16 uiCount);
extern void Update_A_US2_IN_Count (UINT16 uiCount);
extern void Update_A_US1_OUT_Count(UINT16 uiCount);
extern void Update_A_US2_OUT_Count(UINT16 uiCount);
extern void Update_A_US1_AxleCount(UINT16 uiCount);
extern void Update_A_US2_AxleCount(UINT16 uiCount);
extern void Update_A_US1_Direction(BYTE Direction);
extern void Update_A_US2_Direction(BYTE Direction);
extern void Update_Next_Block_State(bitadrb_t SrcAdr, BYTE Next_Block_Status);


/* Declaration for functions defined in rlyb_mgr.c */
extern BOOL Reset_Allowed_For_DS(void);
extern void Set_Relay_B_DAC_Defective(void);
extern BYTE Get_Relay_B_State(void);
extern BYTE Get_Relay_B_ATC_State(void);
extern void Set_Relay_B_Reset_State(void);
extern void Update_B_DS1_OUT_Count(UINT16 uiCount);
extern void Update_B_DS2_OUT_Count(UINT16 uiCount);
extern void Update_B_DS1_IN_Count (UINT16 uiCount);
extern void Update_B_DS2_IN_Count (UINT16 uiCount);
extern void Update_B_DS1_AxleCount(UINT16 uiCount);
extern void Update_B_DS2_AxleCount(UINT16 uiCount);
extern void Update_B_DS1_Direction(BYTE Direction);
extern void Update_B_DS2_Direction(BYTE Direction);
//extern void   Update_Previous_Block_State(bitadrb_t,BYTE);


/* Declaration for functions defined in rlyd3_mgr.c */
extern void Process_D3_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);
extern void Process_D3_Remote_AxleCount(bitadrb_t SrcAdr,SSDAC_Unit_Type Src_Unit_Type,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);
extern void Process_D3_Remote_Direction(bitadrb_t SrcAdr ,SSDAC_Unit_Type Src_Unit_Type, BYTE uchDirection,UINT16 uiAxleCount);
extern void  Process_D3_Peer_Direction(BYTE uchDirection,UINT16 uiAxleCount);
extern BOOL Reset_Allowed_For_D3(void);
extern void Set_Relay_D3_DAC_Defective(void);
extern void Decrement_D3_Track_Clearing_Timeout_50msTmr(void);
extern BYTE Get_Relay_D3_State(void);
extern void Process_D4_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);
extern void Process_D4_Remote_AxleCount(bitadrb_t SrcAdr,SSDAC_Unit_Type Src_Unit_Type,UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);
extern void Process_D4_Remote_Direction(bitadrb_t SrcAdr ,SSDAC_Unit_Type Src_Unit_Type, BYTE uchDirection,UINT16 uiAxleCount);
extern void  Process_D4_Peer_Direction(BYTE uchDirection,UINT16 uiAxleCount);
extern BOOL Reset_Allowed_For_D4(void);
extern void Set_Relay_D4_DAC_Defective(void);
extern void Decrement_D4_Track_Clearing_Timeout_50msTmr(void);
extern BYTE Get_Relay_D4_State(void);


/* Declaration for functions defined in axlemon.c */
extern void Clear_US_AxleCount(void);
extern void Clear_DS_AxleCount(void);
extern void Clear_DS_Local_Counts(void);
extern void Clear_US_Local_Counts(void);
extern UINT16 Get_US_Fwd_AxleCount(void);
extern UINT16 Get_US_Rev_AxleCount(void);
extern UINT16 Get_DS_Fwd_AxleCount(void);
extern UINT16 Get_DS_Rev_AxleCount(void);
extern UINT16 Get_US_AxleCount(void);
extern BYTE Get_US_AxleDirection(void);
extern UINT16 Get_DS_AxleCount(void);
extern BYTE Get_DS_AxleDirection(void);
extern void Start_US_Axle_Counting(void);

extern void Start_DS_Axle_Counting(void);

extern void Stop_US_Axle_Counting(void);

extern void Stop_DS_Axle_Counting(void);

extern BOOL Get_DS_Local_Counts_Clearing_Status(void);
extern BOOL Get_US_Local_Counts_Clearing_Status(void);
extern BOOL Get_DS_Local_Counts_Clearing_Status_1(void);
extern BOOL Get_US_Local_Counts_Clearing_Status_1(void);
extern void Initialise_AxleMon_1(void);
extern void Validate_PD_Signals_1(void);
extern void Chk_for_AxleCount_Completion_1(void);
extern void Initialise_AxleMon_2(void);
extern void Validate_PD_Signals_2(void);
extern void Chk_for_AxleCount_Completion_2(void);
extern void Initialise_AxleMon_3(void);
extern void Validate_PD_Signals_3(void);
extern void Chk_for_AxleCount_Completion_3(void);
extern void Initialise_AxleMon_4(void);
extern void Validate_PD_Signals_4(void);
extern void Chk_for_AxleCount_Completion_4(void);

/* Declaration for functions defined in comm_ds.c */
extern void Initialise_DS_CommSch(void);
extern void Start_DS_CommSch(void);
extern void Update_DS_Sch_State(void);
extern void Decrement_DS_Sch_msTmr(void);
extern void Configure_Modem_B(void);
extern BYTE Get_Modem_B_State(void);




/* Declaration for functions defined in comm_us.c */
extern void Initialise_US_CommSch(void);
extern void Start_US_CommSch(void);
extern void Update_US_Sch_State(void);

extern void Decrement_US_Sch_msTmr(void);

extern void Configure_Modem_A(void);

extern BYTE Get_Modem_A_State(void);


extern void Clear_US_Transmit_Buffer(void);




/* Declaration for functions defined in restore.c */
extern void Check_DIP_Switches(void);
extern void Check_DAC_Boards_Runtime(void);
extern void Check_Flash(void);
extern void Check_RAM(void);
extern void Compare_DS_Checksum(bitadrb_t Config);
extern void Compare_US_Checksum(bitadrb_t Config);
extern UINT32 Get_Calculated_Checksum(void);
extern UINT32 Get_Compared_Checksum(void);
extern void Clear_DS_Checksum_Info(void);
extern void Clear_US_Checksum_Info(void);

/* Declaration for functions defined in lcd_drv.c */

extern void Start_Relay_A_Mgr(void);
extern void Clear_ATC_Local_Relay_A_State(void);
extern void Clear_ATC_Local_Relay_B_State(void);
extern void Update_ATC_Remote1_Relay_A_State (BYTE Remote_state);
extern void Update_ATC_Remote2_Relay_A_State (BYTE Remote_state);
extern void Update_ATC_Remote1_Relay_B_State (BYTE Remote_state);
extern void Update_ATC_Remote2_Relay_B_State (BYTE Remote_state);
extern void Update_ATC_Remote1_Relay_D3_State(BYTE Remote_state);
extern void Update_ATC_Remote2_Relay_D3_State(BYTE Remote_state);
extern void Update_ATC_Remote3_Relay_D3_State(BYTE Remote_state);
extern void Update_ATC_Remote4_Relay_D3_State(BYTE Remote_state);
extern void Update_ATC_Remote1_Relay_D4_State(BYTE Remote_state);
extern void Update_ATC_Remote2_Relay_D4_State(BYTE Remote_state);
extern void Update_ATC_Remote3_Relay_D4_State(BYTE Remote_state);
extern void Update_ATC_Remote4_Relay_D4_State(BYTE Remote_state);
extern BYTE Get_Relay_D3_ATC_State(void);
extern BYTE Get_Relay_D4_ATC_State(void);
extern void Start_Relay_B_Mgr(void);
extern void Start_Relay_D3_Mgr(void);
extern void Start_Relay_D4_Mgr(void);

/* Declaration for functions defined in reset.c */
extern BYTE Update_System_Mode(void);
extern BYTE Update_DS_Section_Mode(BYTE ds_remote_mode,BYTE ds_local_mode);
extern BYTE Update_US_Section_Mode(BYTE us_remote_mode,BYTE us_local_mode);

/* Declaration for functions defined in error.c */
extern BYTE Get_DS_Error_Code(void);
extern BYTE Get_US_Error_Code(void);

extern void Start_US_Data_Encryption(void);
extern void Start_DS_Data_Encryption(void);
extern void Start_US_Data_Decryption(void);
extern void Start_DS_Data_Decryption(void);
BYTE Get_US_Encryption_State(void);
BYTE Get_DS_Encryption_State(void);
BYTE Get_US_Decryption_State(void);
BYTE Get_DS_Decryption_State(void);
extern void Set_US_Decryption_Idle(void);
extern void Set_DS_Decryption_Idle(void);
extern void Set_US_Encryption_Idle(void);
extern void Set_DS_Encryption_Idle(void);
extern void Decrement_LCWS_Health_Relay_Timer(void);
/* Declaration for functions defined in comm_sm.c */
extern void Update_SM_DS_Remote_Axle_Counts(BYTE DS_Count_Lo_byte,BYTE DS_Count_Hi_byte);
extern void Update_SM_US_Remote_Axle_Counts(BYTE US_Count_Lo_byte,BYTE US_Count_Hi_byte);





#endif

