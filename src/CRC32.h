/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    CRC32
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
#ifndef _CRC32_H_
#define _CRC32_H_


#define CHECKSUM_LOCATION	0x2ABF0     /* System ID Location  0x200000L  to 0x200007L  */


typedef unsigned long int resultType;
#define POLYNOMIAL 0x04C11DB7
#define WIDTH (8 * sizeof(resultType))
#define MSb ((resultType)1 << (WIDTH - 1))

resultType crc32(resultType remainder);


#endif
