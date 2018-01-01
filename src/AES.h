/*****************************************************************************

    Project             :    Single Section Digital Axle Counter
    Equipment Version   :    D01S001H001
    Version             :    1.0
    Revision            :    1
    Module Version      :    1.0
    Component name      :    AES
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
#ifndef _AES_h_
#define _AES_h_


#define BLOCKSIZE        (16)
#define DECRYPT_TIMEOUT  (10)
#define ENCRYPT_TIMEOUT  (5)
#define xtime(a) (((a)<0x80)?(BYTE)((a)<<1):((BYTE)((a)<<1)^0x1b) )



extern void US_Encode_Key_Addition(void);
extern void US_Encode_Data(void);
extern void US_Decode_Key_Addition(void);
extern void US_Decode_Data(void);
extern void Update_US_Data_Encode(void);
extern void Update_US_Data_Decode(void);
extern void Decrement_US_Crypto_msTmr(void);
extern void Initialise_US_Crypto_State(void);



extern void DS_Encode_Data(void);
extern void DS_Decode_Key_Addition(void);
extern void DS_Encode_Key_Addition(void);
extern void DS_Decode_Data(void);
extern void Update_DS_Data_Encode(void);
extern void Update_DS_Data_Decode(void);
extern void Decrement_DS_Crypto_msTmr(void);
extern void Initialise_DS_Crypto_State(void);


#endif





