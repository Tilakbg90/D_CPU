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
    Functions   :UINT32 Crc32(INT32 lLength)                                                                       *
*****************************************************************************/
#include <libpic30.h>
#include "COMMON.h"
#include "CRC32.h"
BYTE Prog_Val_Temp[4*10] = {0,0,0,0};
/*********************************************************************************
*File name 			:crc32.c
*Function Name		:resultType crc32(resultType remainder)
*Created By			:EM003
*Date Created		:
*Modification History:
*					Rev No			Date		Description
*					 --				 --				--
*Tracability:
*		SRS()    	:
*
*Abstract			:
*Algorithm			:CRC32 with polynomial 0x04C11DB7
*Description		:resultType is unsigned long int 
*Input Element		:Initial offset which is zero
*Output Element		:resultType
*
**********************************************************************************/
resultType crc32(resultType remainder) {
    BYTE bitp ,track;
    _prog_addressT src_addr=0;

    while(src_addr < CHECKSUM_LOCATION) 
    {
        src_addr = _memcpy_p2d24(&Prog_Val_Temp[0],src_addr,3*10);
        for(track =0;track<((3*10)+1);track++)
        {
            remainder ^= ((resultType)Prog_Val_Temp[track] << (WIDTH - 8));
            for (bitp = 8; bitp > 0; bitp--) {
                if (remainder & MSb) {
                    remainder = (remainder << 1) ^ POLYNOMIAL;
                } else {
                    remainder <<= 1;
                }
            }
        }
    }
    return remainder;
}
