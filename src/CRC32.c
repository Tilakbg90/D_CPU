/*****************************************************************************
    Project             :
    Equipment Version   :
    Version             :
    Revision            :
    Module Version      :
    Component name      :CRC32
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
    Functions   :UINT32 Crc32(INT32 lLength)                                                                       *
*****************************************************************************/
#include "CRC32.h"
#include <libpic30.h>

/*
 * Look-up for CRC-32
 *
 * Table Type : Reflected form
 * Polynomial : 0xEDB88320 (reflection of 0x04C11DB7)
 *    Remarks : Initialise the variable to 0xFFFFFFFF, computed CRC-32 using this table and XOR this
 *              computed CRC-32 with 0xFFFFFFFF to give checksum.
 */

/*********************************************************************
Component name      :CRC32
Module Name         :UINT32 Crc32(const BYTE *puchAddress, INT32 lLength)
Created By          :
Date Created        :
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract            :Compute 32 bit Cyclic redundancy code

Allocated Requirements  :

Design Requirements     :


Interfaces
    Calls           :   Nil

    Called by       :   RESTORE.c   -   Check_Flash()

Input Variables         Name                        Type
    Global          :   None

    Local           :   *puchAddress                BYTE
                        lLength                     INT32
                        ulCrc                       UINT32

Output Variables        Name                        Type
    Global          :   None

    Local           :   ulCrc                       UINT32

Signal Variables


Macro definitions used:     Macro                           Value
                            None
References          :

Derived Requirements:

Algorithm           :1.Initialise the variable to 0xFFFFFFFF
                     2.computed CRC-32 using ulCRC32Table[256] table and XOR this
                    with 0xFFFFFFFF to give checksum.


************************************************************************/
unsigned char Prog_Val_Temp[4] = {0,0,0,0};
resultType crc32(resultType remainder) {

    unsigned char bitp;
    unsigned char track;
    _prog_addressT src_addr=0;

    while(src_addr < CHECKSUM_LOCATION) 
    {
        src_addr = _memcpy_p2d24(&Prog_Val_Temp[0],src_addr,3);
        for(track =0;track<4;track++)
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
