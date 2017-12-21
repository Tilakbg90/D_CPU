/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   CRC16
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
#ifndef _CRC16_H_
#define _CRC16_H_
typedef enum{
COM2_RX,
COM2_TX,
SPI_RX,
SPI_TX,
COM1_RX,
COM1_TX,
} CRC16_PACK;

extern UINT16 Crc16(CRC16_PACK, INT16);

#endif
