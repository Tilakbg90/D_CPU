/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   COMM_SM
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
#ifndef _COMM_SM_H_
#define _COMM_SM_H_


/************ Input signal for Master/slave selection *****************/
    /*  Master/Slave selection port */

#define SPI_SLAVE_SELECT            PORTDbits.RD11

/************ Input signal for Master/slave selection *****************/

/*********************** Default Values *************************************/

    /* SPI Clock Edge Select bit initailized to Data transmitted on
    falling edge of SCK */
#define SSPCON1_DEFAULT             0b00110100

    /* SPI Slave mode, clock = SCK pin, SS pin control enabled,Clock Polarity
    Select Pin assign to IDLE state for clock is a high level,Enable serial
    port and configures SCK, SDO,SDI and SS as serial pins */

#define SSPSTAT_DEFAULT             0b01000000

/*********************** Default Values **************************************/


/**************************SPI Process states ********************************/

typedef enum
            {
    /* This state to wait for SS low for slave selection */
            SPI_WAIT_FOR_SS_LOW =0,

    /* This state to transmit data byte to slaves */
            SPI_TRANSMIT_DATA_BYTES,

    /* This state to check the CRC for SPI communication */
            SPI_CHECK_CRC,

    /* This state to Update the axle counts & direction in peer CPU */
            PROCESS_RECEIVED_MESSAGE

            }SPI_State;
    /*These are the different states of the SPI for interprocess communication*/

/**************************SPI Process states ********************************/


/**************************** Timeouts **************************************/

    /* Macro to set the SPI communication scan rate */

#define SPI_SCAN_RATE               (4)

    /* Macro to set the maximum SPI retries is assigned to 5 */

#define MAXIMUM_SPI_RETRIES         (15)


/****************************** Timeouts *****************************************/

/*************** Structure to hold the SPI transmit object *********************/

typedef struct {

    /* To assign various states for SPI transmission */
            SPI_State State;

    /*  To take the message from the Transmitter message buffer */
            BYTE      Index;

    /*  Used as buffer to store the message to be transmitted */
            BYTE      Msg_Buffer[SPI_MESSAGE_LENGTH];

    /*  Timeout assign for SPI communication[50ms] */
            UINT16    Timeout;

} spi_transmit_object;

/*************** Structure to hold the SPI transmit object *******************/

/**************Structure to hold the SPI Receive object **********************/
typedef struct {
    /* To take the message from the Receive message buffer */
            BYTE    Index;

    /* This variable is Used as buffer to store the message that are received*/
            BYTE    Msg_Buffer[SPI_MESSAGE_LENGTH];

}spi_receive_object;

/**************Structure to hold the SPI Receive object **********************/

extern void Initialise_SPI(void);           //defined in COMM_SM.c
extern void Update_SPI_State(void);         //defined in COMM_SM.c
extern void Decrement_SPI_50msTmr(void);    //defined in COMM_SM.c
#endif
