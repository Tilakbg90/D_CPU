/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   RESET
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
#ifndef _RESET_H_
#define _RESET_H_


/**********Input signal to accept the reset *********************************/

    /* Macros to accept the reset for section A related configurations*/
#define B1_RST_BUTTON_PORT  PORTGbits.RG12

#define B2_RST_BUTTON_PORT  PORTGbits.RG13

        /* Macros to accept the reset for section B related configurations*/
#define A1_RST_BUTTON_PORT  PORTGbits.RG14

#define A2_RST_BUTTON_PORT  PORTGbits.RG15

/**********Input signal to accept the reset *********************************/


/**********Reset button tiemout ***************************************/
    /* Minimum time the reset button to be pressed is 500ms */
#define RESET_BUTTON_MIN_TIMEOUT        (200)

    /* Maximum time the reset button to be pressed is 10s.If reset button
     is pressed for more than 10s it will go to reste button struck state */
#define RESET_BUTTON_MAX_TIMEOUT        (10000)

   /* Maximum Time limit for both the system has to be resetted simultaneously   */

#define COOPERATIVE_RESET_MAX_TIMEOUT       (6000)

/**********Reset button timeout ***************************************/

    /* Macro to check the reset button pressed or not */
#define RESET_BUTTON_PRESSED            (0)


/*********************** Reset Button states *****************************/

/*********************** Dac System Modes *****************************/

#define NO_OF_LOCAL_UNIT_STATES   (7)

#define NO_OF_REMOTE_UNIT_STATES   (8)

/*********************** Dac System Modes *****************************/

typedef enum {
    /* State to indicate as to reset process is not yet started */
            RESET_BUTTON_POLLING_NOT_STARTED = 0,

    /* State to start the reset button polling  */
            WAIT_FOR_RESET_BUTTON_PRESS,

    /* State to check whether reset button is pressed or not and to assign
     to maximum wait state */
            RESET_BUTTON_PRESSED_MIN_WAIT,

    /* State to check the reset button acceptance or reset button stuck */
            RESET_BUTTON_PRESSED_MAX_WAIT,

    /* State to indicate that the reset process is complete */
            RESET_BUTTON_PRESS_ACCEPTED,
    /* State to check for the reset button if stuck than wait for the reset
     button to be pressed */
            RESET_BUTTON_STUCK

} reset_button_state_type_t;

/************************ Reset Button states ****************************/

/************************ SF Reset States ****************************/

typedef enum {

    /* State to indicate that the SF unit is waiting for reset */
            SF_WAIT_FOR_RESET = 0,

    /* State to wait for the reset from DS (DS1 & DS2) for SF unit*/
            SF_WAIT_FOR_DS_TO_RESET,

    /* State to wait for the reset from DS1 for SF unit */
            SF_WAIT_FOR_DS1_TO_RESET,

    /* State to wait for the reset from DS2 for SF unit */
            SF_WAIT_FOR_DS2_TO_RESET,

    /* State to wait for the peer to reset for SF unit */
            SF_WAIT_FOR_PEER_TO_RESET,

    /* State to indicate that the reset sequence is completed for SF unit */
            SF_RESET_SEQUENCE_COMPLETED

} sf_reset_type_t;

/************************ SF Reset States ****************************/

/************************ CF Reset States ****************************/
typedef enum {
    /* State to indicate that CF unit is waiting for reset */
            CF_DS_WAIT_FOR_RESET = 0,
    /*  State to wait for DS reset for CF unit */
            CF_WAIT_FOR_DS_TO_RESET,
    /*  State to wait for DS1 reset for CF unit */
            CF_WAIT_FOR_DS1_TO_RESET,
    /*  State to wait for DS2 reset for CF unit */
            CF_WAIT_FOR_DS2_TO_RESET,
    /* State to wait for peer to reset for CF unit */
            CF_DS_WAIT_FOR_PEER_TO_RESET,
    /* State to indicate that the reset sequence is completed for CF_DS unit*/
            CF_DS_RESET_SEQUENCE_COMPLETED
} cf_ds_reset_type_t;

typedef enum {
    /* State to indicate that CF unit is waiting for reset */
            CF_US_WAIT_FOR_RESET = 0,
    /*  State to wait for US reset for CF unit */
            CF_WAIT_FOR_US_TO_RESET,
    /* State to wait for US1 reset for CF unit */
            CF_WAIT_FOR_US1_TO_RESET,
    /* State to wait for US2 reset for CF unit */
            CF_WAIT_FOR_US2_TO_RESET,
    /* State to wait for peer to reset for CF unit */
            CF_US_WAIT_FOR_PEER_TO_RESET,
    /* State to indicate that the reset sequence is completed for CF_US unit*/
            CF_US_RESET_SEQUENCE_COMPLETED
} cf_us_reset_type_t;

/************************ CF Reset States ****************************/

/************************ CF Reset States ****************************/


/************************ EF Reset States ****************************/

typedef enum {
    /* State to indicate that EF unit is waiting for reset */
            EF_WAIT_FOR_RESET = 0,

    /*  State to wait for US reset for EF */
            EF_WAIT_FOR_US_TO_RESET,

    /*  State to wait for US1 reset for EF */
            EF_WAIT_FOR_US1_TO_RESET,

    /* State to wait for US2 reset for EF */
            EF_WAIT_FOR_US2_TO_RESET,

    /*  State to wait for peer reset for EF */
            EF_WAIT_FOR_PEER_TO_RESET,

    /* State to indicate that the reset sequence is complete for EF */
            EF_RESET_SEQUENCE_COMPLETED

} ef_reset_type_t;

/************************ EF Reset States ****************************/
/************************ DAC Unit States ****************************/
typedef enum{

         WAITING_FOR_RESET_MODE = 1,
         RESET_APPLIED_MODE,
         WAIT_FOR_PILOT_TRAIN_MODE,
         SYSTEM_CLEAR_MODE,
         SYSTEM_OCCUPIED_MODE,
         SYSTEM_ERROR_MODE,
         DP3_MODE
}dac_unit_status;

/************************ DAC Unit States ****************************/

/************************ DAC Section States ****************************/
typedef enum{

         ZERO_MODE = 0,
         WAITING_FOR_RESET_AT_BOTH_UNITS,
         RESET_APPLIED_AT_LOCAL_UNIT,
         RESET_APPLIED_AT_REMOTE_UNIT,
         RESET_APPLIED_AT_BOTH_UNITS,
         SECTION_WAIT_FOR_PILOT_TRAIN,
         SECTION_CLEAR_AT_BOTH_UNITS,
         SECTION_OCCUPIED_AT_BOTH_UNITS,
         SECTION_ERROR_AT_BOTH_UNITS,
         ERROR_RESET_APPLIED_AT_LOCAL_UNIT,
         ERROR_RESET_APPLIED_AT_REMOTE_UNIT,
         ERROR_LOCAL_UNIT_WAITING_FOR_RESET,
         ERROR_REMOTE_UNIT_WAITING_FOR_RESET
}dac_system_status;

/************************ DAC Section States ****************************/

/************************ Structure to hold the DS section local and remote States ****************************/
typedef struct{
              dac_unit_status Local_Unit;
              dac_unit_status Remote_Unit;
              struct{
                  unsigned Remote_Reset_Applied  :1;
                  unsigned Remote_Preparatory_State  :1;
                  unsigned   :6;
                 }Flags;
}ds_section_mode;
/************************ Structure to hold the DS section local and remote States ****************************/


/************************ Structure to hold the US section local and remote States ****************************/
typedef struct{
              dac_unit_status Local_Unit;
              dac_unit_status Remote_Unit;
               struct{
                  unsigned Remote_Reset_Applied  :1;
                  unsigned Remote_Preparatory_State  :1;
                  unsigned   :6;
                 }Flags;
}us_section_mode;
/************************ Structure to hold the DS section local and remote States ****************************/


/******* Union to hold the SF/CF/EF/DE rset state and reset flags ********/

typedef union {
    struct  {
    /* To assign various reset state for SF unit */
            sf_reset_type_t US_State;
            sf_reset_type_t DS_State;
            struct {
        /* Flag to check the DS1 reset for SF unit */
                unsigned DS1_has_been_Reset :1;
        /* Flag to check the DS2 reset for SF unit */
                unsigned DS2_has_been_Reset :1;
                unsigned : 3;

        /* Flag to check the peer CPU reset for SF unit */
                unsigned Peer_CPU_has_been_Reset1 :1;
                unsigned : 2;
                } Flags;
            } SF ;

        struct  {
    /* To assign various reset state for CF unit */
            cf_us_reset_type_t  US_State;
            cf_ds_reset_type_t  DS_State;
            struct {
        /* Flag to check DS1 reset for CF unit */
                unsigned DS1_has_been_Reset  :1;
        /* Flag to check DS2 reset for CF unit */
                unsigned DS2_has_been_Reset  :1;
        /* Flag to check US1 reset for CF unit */
                unsigned US1_has_been_Reset  :1;
        /* Flag to check US2 reset for CF unit */
                unsigned US2_has_been_Reset  :1;
        /* Flag to check peer CPU reset for CF DS unit */
                unsigned Peer_CPU_has_been_Reset :1;
        /* Flag to check peer CPU reset for CF US unit */
                unsigned Peer_CPU_has_been_Reset1 :1;
                unsigned                     :2;
                } Flags;
            } CF ;
    struct  {
    /* To assign various reset state for EF unit  */
            ef_reset_type_t US_State;
            ef_reset_type_t DS_State;
            struct {
                unsigned : 2;
        /* Flag to check US1 reset for EF unit */
                unsigned US1_has_been_Reset :1;
        /* Flag to check US2 reset for EF unit */
                unsigned US2_has_been_Reset :1;
        /* Flag to check peer CPU reset for EF unit*/
                unsigned Peer_CPU_has_been_Reset :1;
                unsigned : 3;
                } Flags;
            } EF ;


} reset_info_t;

extern reset_info_t Reset_Info  __attribute__ ((persistent));

/******* Union to hold the SF/CF/EF/DE rset state and reset flags ********/

/*************** Structure to hold the reset button state *******************/

typedef struct  {
    /* Various reset states assigned for the complete reset process */
            reset_button_state_type_t State;
    /*  Timeout */
            UINT16  Timeout_ms;
            UINT16  Timeout_300s;   /*added for 5min cooperative reset*/

} reset_button_info_t;

/*************** Structure to hold the reset button state *******************/


extern void Initialise_SF_Reset_Monitor(void);      //Defined in RESET.c
extern void Initialise_CF_Reset_Monitor(void);      //Defined in RESET.c
extern void Initialise_EF_Reset_Monitor(void);      //Defined in RESET.c

extern void Start_SF_Reset_Monitor(void);           //Defined in RESET.c
extern void Start_CF_Reset_Monitor(void);           //Defined in RESET.c
extern void Start_EF_Reset_Monitor(void);           //Defined in RESET.c

extern void Update_SF_Reset_Monitor_State(void);    //Defined in RESET.c
extern void Update_CF_Reset_Monitor_State(void);    //Defined in RESET.c
extern void Update_EF_Reset_Monitor_State(void);    //Defined in RESET.c

extern void Decrement_Reset_msTmr(void);            //Defined in RESET.c
extern void Decrement_Reset_300sTmr(void);          //Defined in RESET.c
extern void Decrement_Reset_300sTmr2(void);         //Defined in RESET.c
extern BYTE Get_SF_Reset_State(void);               //Defined in RESET.c
extern BYTE Get_CF_DS_Reset_State(void);            //Defined in RESET.c
extern BYTE Get_CF_US_Reset_State(void);            //Defined in RESET.c
extern BYTE Get_EF_Reset_State(void);               //Defined in RESET.c

extern BYTE Get_EF_US1_Reset_State(void);           //Defined in RESET.c
extern BYTE Get_EF_US2_Reset_State(void);           //Defined in RESET.c
extern BYTE Get_SF_DS1_Reset_State(void);           //Defined in RESET.c
extern BYTE Get_SF_DS2_Reset_State(void);           //Defined in RESET.c
extern BYTE Get_CF_DS1_Reset_State(void);           //Defined in RESET.c
extern BYTE Get_CF_DS2_Reset_State(void);           //Defined in RESET.c
extern BYTE Get_CF_US1_Reset_State(void);           //Defined in RESET.c
extern BYTE Get_CF_US2_Reset_State(void);           //Defined in RESET.c
extern void Clear_Reset_Info(void);                 //Defined in RESET.c

extern void Clear_Reset_State(void);                //Defined in RESET.c

extern void Clear_Local_Reset_Flag(void);           //Defined in RESET.c
extern void Clear_Local_Reset2_Flag(void);          //Defined in RESET.c
extern void Post_DS1_has_been_Reset(BYTE uchData);          //Defined in RESET.c
extern void Post_DS2_has_been_Reset(BYTE uchData);          //Defined in RESET.c
extern void Post_US1_has_been_Reset(BYTE uchData);          //Defined in RESET.c
extern void Post_US2_has_been_Reset(BYTE uchData);          //Defined in RESET.c
extern void Post_Peer_CPU_Reset    (BYTE uchData);              //Defined in RESET.c
extern void Post_Peer_CPU_Reset1   (BYTE uchData);             //Defined in RESET.c
extern void Update_DS_Section_Remote_Reset(void);   //Defined in RESET.c
extern void Update_US_Section_Remote_Reset(void);   //Defined in RESET.c
extern void Update_DS_Section_Remote_Preparatory(void);   //Defined in RESET.c
extern void Update_US_Section_Remote_Preparatory(void);   //Defined in RESET.c
extern void Clear_DS_Remote_Reset_Flag(void);        //Defined in RESET.c
extern void Clear_US_Remote_Reset_Flag(void);         //Defined in RESET.c

#endif
