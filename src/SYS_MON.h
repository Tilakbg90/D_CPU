/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   SYS_MON
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
*****************************************************************************/
#ifndef _SYS_MON_H_
#define _SYS_MON_H_


/********************* Input signal for feedback checking*******************/


/* Macro to check Preparatory relay B feedback */
#define FB1_PORT_STATUS_RELAY_B     PORTBbits.RB10 // PR1_UP signal in schematics
#define FB2_PORT_STATUS_RELAY_B     PORTBbits.RB11 // PR1_DN signal in schematics

    /* Macro to check Preparatory relay B feedback */
#define FB1_PORT_STATUS_RELAY_A     PORTBbits.RB12  // PR2_UP signal in Schematics
#define FB2_PORT_STATUS_RELAY_A     PORTBbits.RB13  // PR2_DN signal in Schematics


    /* Macro to check Vital relay A feeedback */
#define FB1_VITAL_RELAY_B_PORT  PORTEbits.RE0   // VR1_UP
#define FB2_VITAL_RELAY_B_PORT  PORTEbits.RE1   // VR1_DN

    /* Macro to check Vital relay B feeedback */
#define FB1_VITAL_RELAY_A_PORT  PORTEbits.RE2   // VR2_UP
#define FB2_VITAL_RELAY_A_PORT  PORTEbits.RE3   // VR2_DN

    /* Macro to check Vital relay circuit feedback */
// These signals are not implemented, hence removed in code.
#define VR1_CKFB_PORT           PORTEbits.RE1//        RE1     // To check VR1 circuit feedback
#define VR2_CKFB_PORT           PORTEbits.RE3 //        RC2     // To check VR2 circuit feedback

/********************* Input signal for feedback checking*******************/

/*****************Feedback retries and maximum types of feedback***********/

    /* Maximum feedback retries assign to 2 */
#define MAXIMUM_FB_RETRIES              (2)

    /* Maximum types of feedback to 10 */
#define MAXIMUM_TYPES_OF_FEEDBACKS      (10)

/*****************Feedback retries and maximum types of feedback***********/

/*************Timeouts in multiples of 50ms *********************************/

    /* Relay feedback checking time is assigned to 1.0s */
#define RELAY_FB_DEBOUNCE_TIMEOUT               (20)

    /* System stabilisation time is assigned to 10s */
#define SYS_STABILISATION_TIME                  (200)

    /* To check the track status of one block to another block time delay of
    10s assigned*/
#define TRACK_STATUS_COMPARE_WAIT_TIMEOUT       (200)

/*************Timeouts in multiples of 50ms *********************************/

/**************************System Monitor - Process states ****************/

typedef enum {
    /*State to check the presence of all the hardware cards for all the units*/
            SYS_CHECK_BOARDS = 0,

    /* State to check the communication links for all the unit types  */
            SYS_CHECK_COMMUNICATION_LINKS,

    /* State to check the power status for all the unit type */
            SYS_CHECK_POWER_STATUS,

    /* State assigned to check the feedback signals */
            SYS_FEEDBACK_SIGNALS

} sys_mon_state_t;

/**************************System Monitor - Process states ****************/

/**************************Feedback timer - Process states ****************/

typedef enum
            {
    /* Enumarator to assign feedback decrement state to IDLE */
            FEEDBACK_DECREMENT_IDLE =0,

    /* Enumarator to decrement the Timer0 50ms timer */
            DECREMENT_FEEDBACK_TIMER0,

    /* Enumarator to decrement the Timer1 50ms timer */
            DECREMENT_FEEDBACK_TIMER1,

    /* Enumarator to decrement the Timer2 50ms timer */
            DECREMENT_FEEDBACK_TIMER2,

    /* Enumarator to decrement the Timer3 50ms timer */
            DECREMENT_FEEDBACK_TIMER3,

    /* Enumarator to decrement the Timer4 50ms timer */
            DECREMENT_FEEDBACK_TIMER4,

    /* Enumarator to decrement the Timer5 50ms timer */
            DECREMENT_FEEDBACK_TIMER5,

    /* Enumarator to decrement the Timer6 50ms timer */
            DECREMENT_FEEDBACK_TIMER6,

    /* Enumarator to decrement the Timer7 50ms timer */
            DECREMENT_FEEDBACK_TIMER7,
    /* Enumarator to decrement the Timer8 50ms timer */
            DECREMENT_FEEDBACK_TIMER8,
    /* Enumarator to decrement the Timer9 50ms timer */
            DECREMENT_FEEDBACK_TIMER9

            }Timer_Decrement_state;

/**************************Feedback timer - Process states ****************/

/************************Process states - FeedBack Check*******************/

typedef enum
            {
    /* Enumarator to set the feedback checking to IDLE */
             FB_CHECK_IDLE  =0,

    /* Enumarator to activate the feedback checking */
             FB_CHECK_DEBOUNCE_WAIT,

    /* Enumarator to read the feedback signals */
             FB_CHECK_PORT

            }feedBack_Check_State;
/************************Process states - FeedBack Check*******************/

/*************************FeedBack - IDs **********************************/

typedef enum
            {
    /*Enumarator to check the preparatory relay status in energize condition*/
             FB_STATUS_RELAY_A_ON_ID = 0,

    /*Enumarator to check the preparatory relay status in Deenergize condition*/
             FB_STATUS_RELAY_A_OFF_ID,
    /*Enumarator to check the preparatory relay status in energize condition*/
             FB_STATUS_RELAY_B_ON_ID,

    /*Enumarator to check the preparatory relay status in Deenergize condition*/
             FB_STATUS_RELAY_B_OFF_ID,

    /* Enumarator to check the Vital relay A status in energize condition */
             FB_VITAL_RELAY_A_ON_ID,

    /* Enumarator to check the Vital relay A status in Deenergize condition */
             FB_VITAL_RELAY_A_OFF_ID,

    /* Enumarator to check the Vital relay B status in energize condition */
             FB_VITAL_RELAY_B_ON_ID,

    /* Enumarator to check the Vital relay B status in Deenergize condition */
             FB_VITAL_RELAY_B_OFF_ID,

    /* Enumarator to check the US track status  */
             US_TRACK_STATUS_CHECK_ID,

    /* Enumarator to check the DS track status */
             DS_TRACK_STATUS_CHECK_ID

            }feedBack_ID;

/*************************FeedBack - IDs **********************************/

/***********Structure to hold system monitoring status **********************/

typedef struct {

    /* To assign various state for system monitoring */
            sys_mon_state_t       State;

    /* State to assign the decrement timer states */
            Timer_Decrement_state Decrement_State;

    /* Variable to store the timeout */
            UINT16  Timeout_50ms;
} sysmon_info_t;

/***********Structure to hold system monitoring status **********************/

/*************Structure to hold Feedback status *****************************/
typedef struct {

    /* To check the feed back state */
            feedBack_Check_State    State;

    /* Feedback timeout */
            BYTE                    Timeout_50ms;
} feedback_info_t;

/*************Structure to hold Feedback status *****************************/

/************Structure to hold Feedback error status ***********************/

typedef struct
{
    /* State to check the status of status relay s*/
            BYTE    Status_Rly_A_Fb_Error;
    /* State to check the status of status relay s*/
            BYTE    Status_Rly_B_Fb_Error;
    /* State to check the status of Vital relay A */
            BYTE    Vital_Rly_A_Fb_Error;
    /* State to check the status of Vital relay B */
            BYTE    Vital_Rly_B_Fb_Error;
}feedback_error;

/************Structure to hold Feedback error status ***********************/

/*************Structure to hold power supply status ************************/

typedef union
        {
           struct
           {
    /* Bit to check DS1 old power status */
              unsigned  DS1_Old_Power_Status   :1;

    /* Bit to check the DS1 new power status */
              unsigned  DS1_New_Power_Status   :1;

    /* Bit to check the DS2 old power status */
              unsigned  DS2_Old_Power_Status   :1;

    /* Bit to check the DS2 new power status */
              unsigned  DS2_New_Power_Status   :1;

    /* Bit to check the US1 new power status */
              unsigned  US1_Old_Power_Status   :1;

    /* Bit to check US1 new power status */
              unsigned  US1_New_Power_Status   :1;

    /* Bit to check US2 old power status */
              unsigned  US2_Old_Power_Status   :1;

    /* Bit to check US2 new power status */
              unsigned  US2_New_Power_Status   :1;

            } Bit;
              BYTE  Byte;
        } power_supply_status;

/*************Structure to hold power supply status ************************/

/*************Structure to hold Track status *******************************/

typedef union
            {
               struct
               {
    /* Bit to check Local unit DS Track status */
                unsigned Local_DS_Track_Status :1;

    /* Bit to check Local unit US Track status */
                unsigned Local_US_Track_Status :1;

    /* Bit to check Remote unit DS Track status */
                unsigned Remote_DS_Track_Status :1;

    /* Bit to check Remote unit US Track status */
                unsigned Remote_US_Track_Status :1;
                unsigned                       :4;
               }Bit;
               BYTE Byte;

            } track_status_info;

/*************Structure to hold Track status *********************************/

extern void Initialise_Sys_Mon(void);                    //Defined in SYS_MON.c
extern void Update_Sys_Mon_State(void);                  //Defined in SYS_MON.c

extern void Start_Sys_Mon_Decrement_50msTmr(void);       //Defined in SYS_MON.c
extern void Start_FeedBack_Check(feedBack_ID uchID);           //Defined in SYS_MON.c
extern void Stop_FeedBack_Check(feedBack_ID uchID);            //Defined in SYS_MON.c
extern void Update_US_Power_Status(bitadrb_t DAC_Config,bitadrb_t Flags1); //Defined in SYS_MON.c
extern void Update_DS_Power_Status(bitadrb_t DAC_Config,bitadrb_t Flags1); //Defined in SYS_MON.c
extern void Update_US_Track_Status(bitadrb_t Flags);           //Defined in SYS_MON.c
extern void Update_DS_Track_Status(bitadrb_t Flags);           //Defined in SYS_MON.c
extern void Clear_US_Power_Status(void);                 //Defined in SYS_MON.c
extern void Clear_DS_Power_Status(void);                 //Defined in SYS_MON.c
extern void Clear_Comm_US_Failures(void);                //Defined in SYS_MON.c
extern void Clear_Comm_DS_Failures(void);                //Defined in SYS_MON.c
#endif
