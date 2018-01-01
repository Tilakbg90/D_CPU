/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    RELAYDRV
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
#ifndef _RELAYDRV_H_
#define _RELAYDRV_H_

/***************Output siganl to  drive the preparatory relay ************/

#define PREPARATORY_RELAY_B_PORT        LATGbits.LATG8      //To drive the Preparatory A relay
#define PREPARATORY_RELAY_A_PORT        LATDbits.LATD15     //To drive the Preparatory B relay


/***************Output siganl to  drive the preparatory relay ************/

/********** Macros related to PWM ***************************************/

    /* 25% Duty Cycle for 30KHz - 40-MHz Fosc */
#define TURN_ON_PWM_B                   6
    /* 25% Duty Cycle for 60KHz - 40-MHz Fosc */
#define TURN_ON_PWM_A                   6
/* 25% Duty Cycle for 30KHz - 40-MHz Fosc */
#define TURN_OFF_PWM_B              0
/* 25% Duty Cycle for 60KHz - 40-MHz Fosc */
#define TURN_OFF_PWM_A              0

/* 25% Duty Cycle for 30KHz - 40-MHz Fosc */
#define PWM_DUTY_CYCLE_B                (32)
/* 25% Duty Cycle for 60KHz - 40-MHz Fosc */
#define PWM_DUTY_CYCLE_A                (32)
    /* 30KHz PWM - 40-MHz Fosc */
#define PWM_PERIOD_B                    (82)
    /* 30KHz PWM - 40-MHz Fosc */
#define PWM_PERIOD_A                    (82)

/********** Macros related to PWM ***************************************/


/**********Macros to energize and deenergise the preparatory relay ******/

    /* Macro to energise the preparatory relay */
#define TURN_ON_PREPARATORY_RELAY   (1)
    /* macro to deenergise the preparatory relay */
#define TURN_OFF_PREPARATORY_RELAY  (0)

/**********Macros to energize and deenergise the preparatory relay ******/

/*************Structure to hold relay security information *************/

typedef struct {
        INT16 Rand_Generator_Seed;          //Random number
        INT16 Key;                          //Securiy Key to energise the relay
} relay_security_info_t;

/*************Structure to hold relay security information *************/

extern INT16 Get_Relay_Energising_Key(void);        //Defined in RELAYDRV.c
extern void Energise_Preparatory_Relay_A(void);     //Defined in RELAYDRV.c
extern void Energise_Preparatory_Relay_B(void);     //Defined in RELAYDRV.c
extern void Energise_Vital_Relay_A(INT16 iKey);          //Defined in RELAYDRV.c
extern void Energise_Vital_Relay_B(INT16 iKey);          //Defined in RELAYDRV.c
extern void DeEnergise_Preparatory_Relay_A(void);   //Defined in RELAYDRV.c
extern void DeEnergise_Preparatory_Relay_B(void);   //Defined in RELAYDRV.c
extern void DeEnergise_Vital_Relay_A(void);         //Defined in RELAYDRV.c
extern void DeEnergise_Vital_Relay_B(void);         //Defined in RELAYDRV.c

#endif
