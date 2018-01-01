/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    ERROR
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
#ifndef  _Error_H_
#define _Error_H_

/**********Macros used related to errors ************************************/
    /* Total error number is fixed to 64 */
#define  TOTAL_NO_ERRORS                    64

    /* Total number of major errors is 8 */
#define  TOTAL_NO_MAJOR_ERRORS              8

    /* Maximum number of character on LCD  */
#define  NO_OF_CHARACTERS_PER_LINE          21

    /* Numnber of bits in each byte */
#define  BITS_PER_BYTE                      8

    /* Display refresh time */
#define  DISPLAY_REFRESH_TIME               0

#define  NO_ERROR_VALUE                     0xFF    //Not used
/**********Macros used related to errors ************************************/

/************************* States in Error Display****************************/
    typedef enum
                {
    /* Check for the major error */
                 MAJOR_ERROR =0,

    /* Check for the minor error */
                 MINOR_ERROR,

    /* Clears the fourth line of the display */
                 DISPLAY_CLEAR,

    /* Copy’s the string message to indicate the error */
                 STRING_COPY,

    /* Display the message on LCD */
                 DISPLAY_MESSAGE,

    /* Display the error code on 7-Segment */
                 DISPLAY_ERROR_CODE

                }Error_Display_State;

/*************************States in Error Display****************************/

/********************  structure to hold all minor error ****************/
typedef union
{
struct
{
	      unsigned PRELIMINARY_RELAY_A_FAILURE     :1;
	      unsigned PRELIMINARY_RELAY_B_FAILURE      :1;
	      unsigned CRITICAL_RELAY_A_FAILURE            :1;
	      unsigned CRITICAL_RELAY_B_FAILURE            :1;
	      unsigned TRANSIENT_POWER_FAILURE_DS1      :1;
	      unsigned TRANSIENT_POWER_FAILURE_DS2      :1;
	      unsigned TRANSIENT_POWER_FAILURE_US1      :1;
	      unsigned TRANSIENT_POWER_FAILURE_US2      :1;

	      unsigned INOPERATIVE_NETWORK_ADDRESS          :1;
	      unsigned INCORRECT_CODE_CRC               :1;
	      unsigned INOPERATIVE_CONFIGURATION            :1;
	      unsigned INOPERATIVE_COUNTS                   :1;
	      unsigned RAM_TEST_FAILED                  :1;
	      unsigned DIRECT_OUT_COUNT                 :1;
	      unsigned                                  :1;
	      unsigned                                  :1;

	      unsigned AD1_SUP_LOW                      :1;
	      unsigned AD2_SUP_LOW                      :1;
	      unsigned AD1_PULSING                      :1;
	      unsigned AD2_PULSING                      :1;
	      unsigned AD_STATE_MISSING                 :1;
	      unsigned AD_SUP_PULSATING                 :1;
	      unsigned AD_STATE_FAIL                    :1;
	      unsigned AD_NOT_SENSING                   :1;

	      unsigned AD1_BOARD_MISSING                :1;
	      unsigned AD2_BOARD_MISSING                :1;
	      unsigned EVENT_LOGGER_MISSING                   :1;
	      unsigned MODEM_CARD_MISSING               :1;
	      unsigned              :1;
	      unsigned RELAY_DRIVE_A_MISSING                :1;
	      unsigned RELAY_DRIVE_B_MISSING            :1;
	      unsigned                  :1;

	      unsigned NO_CARRIER_IN_MODEM           :1;
	      unsigned ASSOCIATE_CPU_LINK_FAILURE            :1;
	      unsigned DS_ERROR                    :1;
	      unsigned US_ERROR                    :1;
	      unsigned AD_PULSE_MISMATCH :1;
	      unsigned BOOTUP_PD_FAIL                   :1;
	      unsigned  DECEPTIVE_AXLE                               :1;
	      unsigned DOUBLE_COIL_INFLUENCE                           :1;

	      unsigned LU1_TO_US1_COMM_FAILURE          :1;
	      unsigned LU1_TO_US2_COMM_FAILURE          :1;
	      unsigned LU1_TO_DS1_COMM_FAILURE          :1;
	      unsigned LU1_TO_DS2_COMM_FAILURE          :1;
	      unsigned US1_TO_LU1_COMM_FAILURE          :1;
	      unsigned US2_TO_LU1_COMM_FAILURE          :1;
	      unsigned DS1_TO_LU1_COMM_FAILURE          :1;
	      unsigned DS2_TO_LU1_COMM_FAILURE          :1;

	      unsigned LU2_TO_US1_COMM_FAILURE          :1;
	      unsigned LU2_TO_US2_COMM_FAILURE          :1;
	      unsigned LU2_TO_DS1_COMM_FAILURE          :1;
	      unsigned LU2_TO_DS2_COMM_FAILURE          :1;
	      unsigned US1_TO_LU2_COMM_FAILURE          :1;
	      unsigned US2_TO_LU2_COMM_FAILURE          :1;
	      unsigned DS1_TO_LU2_COMM_FAILURE          :1;
	      unsigned DS2_TO_LU2_COMM_FAILURE          :1;

	      unsigned  char junk;


 } Flags;
     unsigned  char Byte[8];
} minor_err;

/********************  structure to hold all major error ****************/

/********************  structure to hold all major error ****************/
typedef  union
{
 struct
    {
        /* Bit to check the presence of all the hardware cards */
         unsigned BOARDS_MISSING                : 1;
        /*Bit to check communication 1 link failure*/
         unsigned COMM1_LINK_FAIL               : 1;
        /*Bit to check communication 1 link failure*/
         unsigned COMM2_LINK_FAIL               : 1;
        /*Bit to check communication failure because of no carrier detection
        Peer CPU link failure, Failure at DS,US & failure in peer system*/
         unsigned COMM3_LINK_FAIL               : 1;
        /*Bit to check PD failure*/
         unsigned PD_FAIL                       : 1;
        /*Bit to check feedback failure*/
         unsigned FEEDBACK_FAILURE              : 1;
        /*Bit to check power supply failure*/
         unsigned POWER_SUPPLY_FAIL             : 1;
        /*Bit to check configuration failure */
         unsigned CONFIGURATION_FAILURE         : 1;

     } Flags;
   unsigned char  Byte;
} major_err;
/********************  structure to hold all major error ****************/

/********************  structure to hold both DS and US section error ****************/

typedef struct
  {
    BYTE DS_Error;
    BYTE US_Error;
    struct{
                     unsigned DS_Section    :1;
                     unsigned US_Section    :1;
                     unsigned  :6;
                    }Flags;
  }error_code;
/********************  structure to hold both DS and US section error ****************/



/******************** major error Look up table ****************/

        #define  BOARDS_MISSING_ID                  3
        #define  COMM1_LINK_FAIL_ID                 5
        #define  COMM2_LINK_FAIL_ID                 6
        #define  COMM3_LINK_FAILURE_ID              3
        #define  PD_FAILURE_ID                      4
        #define  FEEDBACK_FAILURE_ID                5
        #define  POWER_SUPPLY_FAIL_ID               6
        #define  CONFIGURATION_FAILURE_ID           7

/******************** major error Look up table ****************/


/******************** minor error Look up table ****************/

#define PRELIMINARY_RELAY_A_FAILURE_ERROR_NUM  	0
#define PRELIMINARY_RELAY_B_FAILURE_ERROR_NUM  	1
#define CRITICAL_RELAY_A_FAILURE_ERROR_NUM     	2
#define CRITICAL_RELAY_B_FAILURE_ERROR_NUM     	3
#define TRANSIENT_POWER_FAILURE_DS1_ERROR_NUM    	4
#define TRANSIENT_POWER_FAILURE_DS2_ERROR_NUM    	5
#define TRANSIENT_POWER_FAILURE_US1_ERROR_NUM    	6
#define TRANSIENT_POWER_FAILURE_US2_ERROR_NUM    	7

#define INOPERATIVE_NETWORK_ADDRESS_ERROR_NUM   	8
#define INCORRECT_CODE_CRC_ERROR_NUM            	9
#define INOPERATIVE_CONFIGURATION_ERROR_NUM     	10
#define INOPERATIVE_COUNTS_ERROR_NUM            	11
#define RAM_TEST_FAILED_ERROR_NUM               	12
#define DIRECT_OUT_ERROR_NUM               	13
#define AD_SUP_MISSING_ERROR_NUM               	14

#define AD1_SUP_LOW_ERROR_NUM                    	16
#define AD2_SUP_LOW_ERROR_NUM                    	17
#define AD1_PULSING_ERROR_NUM                    	18
#define AD2_PULSING_ERROR_NUM                    	19
#define AD_STATE_MISSING_ERROR_NUM               	20
#define AD_SUP_PULSATING_ERROR_NUM               	21
#define AD_STATE_FAIL_ERROR_NUM                  	22
#define AD_NOT_SENSING_ERROR_NUM                 	23

#define AD1_BOARD_MISSING_ERROR_NUM    	24
#define AD2_BOARD_MISSING_ERROR_NUM    	25
#define EVENT_LOGGER_MISSING_ERROR_NUM 	26
#define MODEM_CARD_A_MISSING_ERROR_NUM 	27
#define MODEM_CARD_B_MISSING_ERROR_NUM 	28
#define RELAY_DRIVE_A_MISSING_ERROR_NUM	29
#define RELAY_DRIVE_B_MISSING_ERROR_NUM	30
#define ASSOCIATE_CPU_MISSING_ERROR_NUM	31

#define NO_CARRIER_IN_MODEM_ERROR_NUM        	32
#define ASSOCIATE_CPU_LINK_FAILURE_ERROR_NUM      	33
#define DS_ERROR_NUM                  	34
#define US_ERROR_NUM                  	35
#define AD_PULSE_MISMATCH_ERROR_NUM            	36
#define BOOTUP_AD_FAIL_ERROR_NUM               	37
#define DECEPTIVE_AXLE_ERROR_NUM               	38
#define DOUBLE_COIL_INFLUENCE_ERROR_NUM        	39

 #define LU1_TO_US1_COMMUNICATION_ERROR_NUM	40
 #define LU1_TO_US2_COMMUNICATION_ERROR_NUM	41
 #define LU1_TO_DS1_COMMUNICATION_ERROR_NUM	42
 #define LU1_TO_DS2_COMMUNICATION_ERROR_NUM	43
 #define US1_TO_LU1_COMMUNICATION_ERROR_NUM	44
 #define US2_TO_LU1_COMMUNICATION_ERROR_NUM	45
 #define DS1_TO_LU1_COMMUNICATION_ERROR_NUM	46
 #define DS2_TO_LU1_COMMUNICATION_ERROR_NUM	47
	
 #define LU2_TO_US1_COMMUNICATION_ERROR_NUM	48
 #define LU2_TO_US2_COMMUNICATION_ERROR_NUM	49
 #define LU2_TO_DS1_COMMUNICATION_ERROR_NUM	50
 #define LU2_TO_DS2_COMMUNICATION_ERROR_NUM	51
 #define US1_TO_LU2_COMMUNICATION_ERROR_NUM	52
 #define US2_TO_LU2_COMMUNICATION_ERROR_NUM	53
 #define DS1_TO_LU2_COMMUNICATION_ERROR_NUM	54
 #define DS2_TO_LU2_COMMUNICATION_ERROR_NUM	55


/******************** minor error Look up table ****************/

/********structure to display error messages********************/
typedef struct
{
    /*To assign error states*/
 Error_Display_State State;
    /*Error message stored in this string for display purpose*/
 BYTE uchstring[21];
    /*To display the error code*/
 BYTE Error_Code;
    /*To point at the error message from error table*/
 BYTE position;
    /*Display refresh timeout*/
 BYTE refresh_Timeout;
}error_display;
/********structure to display error messages********************/


extern void Initialise_Error(void);                 //Defined in ERROR.c
extern void Update_Display_Error(void);             //Defined in ERROR.c
extern void Decrement_Err_display_50msTmr(void);    //Defined in ERROR.c
extern void Set_Error_Status_Byte ( BYTE  Major_Err_Id, BYTE status_byte);       //Defined in ERROR.c
extern void Set_Error_Status_Bit (BYTE Error_No);             //Defined in ERROR.c
extern void Clear_Error_Display(void);              //Defined in ERROR.c
extern void Update_Error_Code(void);                //Defined in ERROR.c
extern void Clear_DS_Error_Code(void);              //Defined in ERROR.c
extern void Clear_US_Error_Code(void);              //Defined in ERROR.c
#endif
