/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    AXLE_MON
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
****************************************************************************/
#ifndef _AXLE_MON_H_
#define _AXLE_MON_H_


/******** Input signal to controller from phase detector cards ************/

    /* Input port to get the PD1 main signal from phase detector card1 */
#define PD1_PULSE_MAIN          PORTCbits.RC1

    /* Input port to get the PD2 main signal from phase detector card2 */
#define PD2_PULSE_MAIN          PORTCbits.RC4

    /* Input port to get the PD1 secondary signal from phase detector card1*/
#define PD1_PULSE_SECONDARY     PORTCbits.RC2

    /*  Input port to get the PD2 secondary signal from phase detector card2 */
#define PD2_PULSE_SECONDARY     PORTBbits.RB4

    /* Input port to get the PD1 supervisory signal from phase detector card1*/
#define PD1_SUPERVISORY_PULSE   PORTCbits.RC3

    /* Input port to get the PD2 supervisory signal from phase detector card2*/
#define PD2_SUPERVISORY_PULSE   PORTBbits.RB5

/******** Input signal to controller from phase detector cards ************/

/******* Macros for number of process states and number of transitions ***/

/*Number of track process states is assigned to 11 */
#define NO_OF_TRACK_PROCESS_STATES      (11)

/*Number of PD transitios is assigned to 4, initailly both PD state will be high
  For forward direction, PD1  PD2       For Reverse direction, PD1  PD2
                          0   1                                 1   0
                          0   0                                 0   0
                          1   0                                 0   1
                          1   1                                 1   1 */
#define NO_OF_PD_TRANSITIONS            (4)

/******* Macros for number of process states and number of transitions ***/

/************* Timeouts in multiples of 50ms********************************/

/*PD supervisory low time set to 3s, if it is low for more than 3s system
  will go error mode */

//#define PD_SUPERVISORY_LOW_TIMEOUT        (60)

/*PD supervisory low time set to 60s, if it is low for more than 60s system
  will go error mode */
#define PD_SUPERVISORY_LOW_TIMEOUT      (1200)

/*The PD clearing time for trolley, dip lorry is assigned to 6s */

#define PD_CLEARING_TIMEOUT             (120)

/* Time delay to clear the local count just cleared flag*/
#define LOCAL_COUNT_CLEARING_FLAG_TIMEOUT   (40)

/* Time delay to allow a non overlap pulse after an overlap pulse occured*/
#define TROLLEY_TIMEOUT (120)

/**************Timeouts in multiples of 50ms********************************/

/******** Macro for Maximum and Minimum counts ***************/

/* should be <= INT_MAX  and > MIN_AXLE_COUNTS */

#define MAX_AXLE_COUNTS                 (9999)

/* should be >= INT_MIN and < MAX_AXLE_COUNTS */
#define MIN_AXLE_COUNTS                 (0)

/*Macro to control shutting operation in TWS*/
#define NO_SHUNTING                     (1)

/******** Macro for Maximum and Minimum counts ***************/

/****************  AXLE MONITOR Process states  **************/

typedef enum
            {
    /*In this state both the PD signal will be HIGH */
             WAITING_FOR_AXLE =0,

    /*In this state train already touch the leading PD it is waiting
     for axle at trailing PD*/
             WAIT_FOR_AXLE_AT_TRAILING_PD,

    /*In this state train clears at leading PD */
             LET_AXLE_CLEAR_LEADING_PD_FIRST,

    /*In this state train clears the both the leading and trailing PD
      and it will enter in to wait for axle state */
             LET_AXLE_CLEAR_TRAILING_PD_NEXT,

    /*In this state train enter in forward direction and went back */
             DIR_CHANGE_FROM_FWD_TO_REV,

    /*In this state train already touch the trailing PD it is waiting
      for axle at leading PD */
             WAIT_FOR_AXLE_AT_LEADING_PD,

    /* In this state train clears at training PD */
             LET_AXLE_CLEAR_TRAILING_PD_FIRST,

    /*In this state train clears both the trailing and leading PD and
      it will enter in to wait for axle state */
             LET_AXLE_CLEAR_LEADING_PD_NEXT,

    /*In this state train enter in reverse direction and went back */
             DIR_CHANGE_FROM_REV_TO_FWD,

    /* In this state PD will wait for some time for sychronisation */
             PRE_SYNCHRONISATION_WAIT,

    /*In this state Out of sync or PD error will occur*/
             OUT_OF_SYNC_OR_PD_ERROR
             }Axle_Counter_State;

/***********************  AXLE MONITOR Process states  ***********************/

/**************************** Diffrent wheel types **************************/

typedef enum   {
    /*Initailly wheel type is assigned to its not determined */
                 WHEEL_TYPE_NOT_DETERMINED = 0,

    /*Enumerator to identify the train wheel with single sup signal */
                 TRAIN_WHEEL_WITH_SINGLE_SUP,

    /*Enumerator to identify the train wheel with double sup signal */
                 TRAIN_WHEEL_WITH_DOUBLE_SUP,

    /*Enumerator to identify the trolley wheel with double sup signal */
                 TROLLEY_WHEEL_WITH_DOUBLE_SUP
                }different_wheel_types;

/**************************** Diffrent wheel types **************************/

/***********Structure to hold axle minotor state counts, direction ************/

typedef struct {
                /*This is states of the PD */
                Axle_Counter_State  State;
                
                UINT16 LCWS_Count;
                UINT16 LCWS_Prev_Count;
                BYTE   LCWS_Axle_Direction;
                UINT16 LCWS_Fwd_Count;
                UINT16 LCWS_Rev_Count;
                UINT16 LCWS_Total_Fwd_Count;
                UINT16 LCWS_Total_Rev_Count;

                UINT16 LCWS_Peer_Count;
                BYTE   LCWS_Peer_Axle_Direction;
                UINT16 LCWS_Fwd_Peer_Count;
                UINT16 LCWS_Rev_Peer_Count;
                UINT16 LCWS_Total_Fwd_Peer_Count;
                UINT16 LCWS_Total_Rev_Peer_Count;

                /* Up Stream axle direction */
                BYTE    US_Axle_Direction;

                /* Up Stream axle count, after each train passes
                this variable is set to zero */
                UINT16  US_Axle_Count;

                /*Down Stream axle direction */
                BYTE    DS_Axle_Direction;

                /* Down Stream axle count, after each train passes
                 this variable is set to zero */
                UINT16  DS_Axle_Count;

                /*Up Stream forward axle count*/
                UINT16  US_Fwd_Axle_Count;

                /*Up Stream reverse axle count*/
                UINT16  US_Rev_Axle_Count;

                /*Down Stream forward axle count */
                UINT16  DS_Fwd_Axle_Count;

                /* Down Stream reverse axle count */
                UINT16  DS_Rev_Axle_Count;

                /* Variable to store the PD1 main signal for PD1 error
                  checking purpose */
                BYTE  PD1M_Count;

                /*Variable to store the PD2 main signal for PD2 error
                 checking purpose */
                BYTE  PD2M_Count;

                /*Variable to store the PD1 supervisory signal for PD1 error
                 checking purpose */
                BYTE    PD1S_Count;

                /*Variable to store the PD1 supervisory signal for PD2 error
                 checking purpose */
                BYTE    PD2S_Count;

                /* Variable to store the PD1 supervisory falling edge count to
                 diffrentiate between main wheel and trolley wheel */
                BYTE  PD1S_Falling_Edge_Count;

                /*Variable to store the PD2 supervisory falling edge count to
                  diffrentiate between main wheel and trolley wheel */
                BYTE  PD2S_Falling_Edge_Count;

                /* Variable to store the PD1 supervisory rising edge count to
                 diffrentiate between main wheel and trolley wheel */
                BYTE  PD1S_Rising_Edge_Count;

                /* Variable to store the PD2 supervisory rising edge count to
                  diffrentiate between main wheel and trolley wheel */
                BYTE  PD2S_Rising_Edge_Count;

                /* Variable to store the different wheel type */
                different_wheel_types Wheel_Type;
                /* Variable to store the PD state change timeout to 6s */
                BYTE    PD_Last_Change_Timeout_50ms;

                /* Variable to store the 50ms timer for PD1 supervisory signal */

                UINT16  PD1S_Timeout_50ms;
                /* Variable to store the 50ms timer for PD2 supervisory signal */

                UINT16  PD2S_Timeout_50ms;
                /* variable to store 50ms timer for clearing local count just cleared flag*/
                BYTE    DS_Local_Counts_Clearing_Flag_Timeout_50ms;
                 /* variable to store 50ms timer for clearing local count just cleared flag*/
                BYTE    US_Local_Counts_Clearing_Flag_Timeout_50ms;
                 /*variable to store 50ms timer for trolley*/
                BYTE    Trolley_Timeout_50ms;

            struct {

                /* Bit to check for PD counting is completed or not, if complated
                 clear this bit */
                    unsigned PD_State_Counted      :1;

                /* Bit to turn on the decrement of the PD1 supervisory timer */
                    unsigned PD1S_Timer_On         :1;

                /* Bit to turn on the decrement of the PD2 supervisory timer */
                    unsigned PD2S_Timer_On         :1;

                /* Bit to store the previous value of the PD1 supervisory signal
                 for error checking purpose */
                    unsigned PD1S_Port_Latch       :1;

                /* Bit to store the previous value of the PD2 supervisory signal
                 for error checking purpose */
                    unsigned PD2S_Port_Latch       :1;

                /* Bit to store the previous value of the PD1 Main signal for
                 error checking purpose */
                    unsigned PD1M_Port_Latch       :1;

                /* Bit to store the previous value of the PD2 Main signal for
                error checking purpose */
                    unsigned PD2M_Port_Latch       :1;

                /* Bit to enable only the Up Stream counting */
                    unsigned US_Counting_Enable    :1;

                /* Bit to enable only the Down Stream counting */
                    unsigned DS_Counting_Enable    :1;
                /* Bit to detect non overlapping  PD pulses */
                    unsigned PD_Non_Overlapping    :1;
                    unsigned PD_Without_Overlapping :1;

                    unsigned DS_Local_Counts_Just_Cleared :1;

                    unsigned US_Local_Counts_Just_Cleared :1;

                    unsigned FDP_US_Counting_Enable    :1;

                    unsigned FDP_DS_Counting_Enable    :1;

                    unsigned                       :1;
                } Flags;
} track_info_t;
/***********Structure to hold axle minotor state counts, direction ************/

extern void Initialise_AxleMon(void);           //Defined in AXLE_MON.c
extern void Validate_PD_Signals(void);          //Defined in AXLE_MON.c

extern void Chk_for_AxleCount_Completion(void); //Defined in AXLE_MON.c
extern void Decrement_TrackMon_50msTmr(void);   //Defined in AXLE_MON.c


extern track_info_t Track_Info_1;
extern track_info_t Track_Info_2;
extern track_info_t Track_Info_3;
extern track_info_t Track_Info_4;

extern const BYTE uchPD_Transition_Table[NO_OF_TRACK_PROCESS_STATES][NO_OF_PD_TRANSITIONS];

#endif

