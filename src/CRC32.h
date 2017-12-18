/*****************************************************************************

    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :   CRC32
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
#ifndef _CRC32_H_
#define _CRC32_H_


#define CHECKSUM_LOCATION	0x2ABF0     /* System ID Location  0x200000L  to 0x200007L  */


typedef unsigned long int resultType;
#define POLYNOMIAL 0x04C11DB7
#define WIDTH (8 * sizeof(resultType))
#define MSb ((resultType)1 << (WIDTH - 1))

resultType crc32(resultType remainder);


#endif
