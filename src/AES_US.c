#include <xc.h>

#include "COMMON.h"
#include "COMM_US.h"
#include "AES.h"


static BYTE US_roundCounter;
static BYTE US_rcon;
BYTE US_iE;
BYTE US_iD;
ciper_message_info US_Ciper_Message_Info;
extern msg_info_t    Com1XmitObject;    /* COM1: Message Transmission Buffer etc., */
extern msg_info_t    Com1RecvObject;    /* COM1: Message Receiving Buffer etc., */
enciper_info US_Enciper_Info;
deciper_info US_Deciper_Info;
extern const BYTE STable[256];
extern const BYTE SiTable[256];
extern const BYTE x1[256];
extern const BYTE x2[256];
extern const BYTE x3[256];
extern const unsigned char E_Key[16];
extern const unsigned char D_Key[16];

void US_EncodeShiftRow(BYTE* block);
void US_DecodeShiftRow(BYTE* stateTable);
void US_EncKeySchedule(BYTE* key);
void US_DecKeySchedule(BYTE* key);
void Load_Encrypt_Key(void);
void Load_Decrypt_Key(void);

//29/06/10
void Initialise_US_Crypto_State(void)
{
 US_Enciper_Info.State = ENCRYPTION_IDLE;
 US_Deciper_Info.State = DECRYPTION_IDLE;
 US_Enciper_Info.Timeout_ms = 0;
 US_Deciper_Info.Timeout_ms = 0;
}

void Update_US_Data_Encode(void)
{
    switch(US_Enciper_Info.State)
         {
            case ENCRYPTION_IDLE:
                break;
            case START_ENCRYPTION:
                Load_Encrypt_Key();
            //  Load_Encrypt_Data();
                US_Encode_Key_Addition(Com1XmitObject.Msg_Buffer, US_Ciper_Message_Info.Encrypt_key);
                US_Enciper_Info.State = ENCRYPT_KEY_ADDITION;
                US_Enciper_Info.Timeout_ms = ENCRYPT_TIMEOUT;
                break;
            case ENCRYPT_KEY_ADDITION:
                US_Encode_Data(Com1XmitObject.Msg_Buffer, US_Ciper_Message_Info.Encrypt_key);
                if(US_roundCounter == 0)
                {
                 US_Enciper_Info.State = ENCRYPTION_COMLETED;
                }
                if(US_Enciper_Info.Timeout_ms == TIMEOUT_EVENT)
                {
                 US_Enciper_Info.State = ENCRYPT_FAILURE;
                }
                break;
            case ENCRYPT_FAILURE:
                break;
            case ENCRYPTION_COMLETED:
                break;
        }
}

void Load_Encrypt_Key(void)
{
 BYTE count;
 for(count = 0; count <= BLOCKSIZE; count++)
    {
    US_Ciper_Message_Info.Encrypt_key[count] = E_Key[count];
    }
}

void US_Encode_Key_Addition(BYTE* block, BYTE* key)
{

    BYTE i;
    /* initiate round counter = 10 */
    US_roundCounter = 10;

    US_rcon = 0x01;

    /* key addition */
    for(i=0;i<BLOCKSIZE;i++)
    {
        block[i] ^= key[i];
    }
}

void US_Encode_Data(BYTE* block, BYTE* key)
{

    /* s table substitution */
        for(US_iE=0;US_iE<BLOCKSIZE;US_iE++)
        {
            block[US_iE]=STable[block[US_iE]];
        }

        /*   encode shift row */
        US_EncodeShiftRow(block);

        /*   if round counter != 1 */
        if(US_roundCounter != 1)
        {
            BYTE aux,aux1,aux2,aux3;
            /*     mix column */
            for(US_iE=0;US_iE<16;US_iE+=4)
            {
                aux1= block[US_iE+0] ^ block[US_iE+1];
                aux3= block[US_iE+2]^block[US_iE+3];
                aux = aux1 ^ aux3;
                aux2= block[US_iE+2]^block[US_iE+1];

                aux1 = xtime(aux1);
                aux2 = xtime(aux2);
                aux3 = xtime(aux3);

                block[US_iE+0]= aux^aux1^block[US_iE+0];
                block[US_iE+1]= aux^aux2^block[US_iE+1];
                block[US_iE+2]= aux^aux3^block[US_iE+2];
                block[US_iE+3]= block[US_iE+0]^block[US_iE+1]^block[US_iE+2]^aux;
            }
        }
        /*   encode key schedule */
        US_EncKeySchedule(key);

        /*   key addition */
        for(US_iE=0;US_iE<BLOCKSIZE;US_iE++)
        {
            block[US_iE] ^= key[US_iE];
        }

        US_roundCounter--;
}

void US_EncodeShiftRow(BYTE* stateTable)
{
    BYTE temp;

    /* first row (row 0) unchanged */

    /* second row (row 1) shifted left by one */
    temp=stateTable[1];
    stateTable[1]=stateTable[5];
    stateTable[5]=stateTable[9];
    stateTable[9]=stateTable[13];
    stateTable[13]=temp;

    /* third row (row 2) shifted left by two */
    temp=stateTable[2];
    stateTable[2]=stateTable[10];
    stateTable[10]=temp;
    temp=stateTable[14];
    stateTable[14]=stateTable[6];
    stateTable[6]=temp;

    /* fourth row (row 3) shifted left by three (or right by one) */
    temp=stateTable[3];
    stateTable[3]=stateTable[15];
    stateTable[15]=stateTable[11];
    stateTable[11]=stateTable[7];
    stateTable[7]=temp;
}

void US_EncKeySchedule(BYTE* key)
{
    /* column 1 */
    key[0]^=STable[key[13]];
    key[1]^=STable[key[14]];
    key[2]^=STable[key[15]];
    key[3]^=STable[key[12]];

    key[0]^=US_rcon;
    US_rcon = xtime(US_rcon);

    /* column 2 */
    key[4]^=key[0];
    key[5]^=key[1];
    key[6]^=key[2];
    key[7]^=key[3];

    /* column 3 */
    key[8]^=key[4];
    key[9]^=key[5];
    key[10]^=key[6];
    key[11]^=key[7];

    /* column 4 */
    key[12]^=key[8];
    key[13]^=key[9];
    key[14]^=key[10];
    key[15]^=key[11];

}


void Update_US_Data_Decode(void)
{
        switch(US_Deciper_Info.State)
         {
            case DECRYPTION_IDLE:
                break;
            case START_DECRYPTION:
                Load_Decrypt_Key();
                US_Decode_Key_Addition(Com1RecvObject.Msg_Buffer, US_Ciper_Message_Info.Decrypt_key);
                US_Deciper_Info.State = DECRYPT_KEY_ADDITION;
                US_Deciper_Info.Timeout_ms = DECRYPT_TIMEOUT;
                break;
            case DECRYPT_KEY_ADDITION:
                US_Decode_Data(Com1RecvObject.Msg_Buffer, US_Ciper_Message_Info.Decrypt_key);
                if(US_roundCounter == 0)
                {
                 US_Deciper_Info.State = DECRYPTION_COMPLETED;
                }
                if(US_Deciper_Info.Timeout_ms == TIMEOUT_EVENT)
                {
                 US_Deciper_Info.State = DECRYPT_FAILURE;
                }
                break;
            case DECRYPT_FAILURE:
                break;
            case DECRYPTION_COMPLETED:
                break;
        }
}

void Load_Decrypt_Key(void)
{
 BYTE count;
 for(count = 0; count <= BLOCKSIZE; count++)
    {
    US_Ciper_Message_Info.Decrypt_key[count] = D_Key[count];
    }
}

void US_Decode_Key_Addition(BYTE* block, BYTE* key)
{
    BYTE i;

    /* reconstruct key */
    i=1;

    US_rcon = 0x36;

    /* initiate round counter = 10 */
    US_roundCounter = 10;

    /* key addition */
    for(i=0;i<BLOCKSIZE;i++)
    {
        block[i] ^= key[i];
    }

}

void US_Decode_Data(BYTE* block, BYTE* key)
{
    if( US_roundCounter != 10)
        {
            /* inverse mix column */
            /*     mix column */

            {
                BYTE temp0,temp1,temp2,temp3;
                for(US_iD=0;US_iD<16;US_iD+=4)
                {
                    temp3=x3[block[US_iD+0x00]]^x3[block[US_iD+0x01]]^x3[block[US_iD+0x02]]^x3[block[US_iD+0x03]]^block[US_iD+0x00]^block[US_iD+0x01]^block[US_iD+0x02]^block[US_iD+0x03];
                    temp0=x2[block[US_iD+0x00]]^x1[block[US_iD+0x00]]
                              ^x1[block[US_iD+0x01]]
                              ^x2[block[US_iD+0x02]]
                              ^temp3^block[US_iD+0x00];
                    temp1=x2[block[US_iD+0x01]]^x1[block[US_iD+0x01]]
                              ^x1[block[US_iD+0x02]]
                              ^x2[block[US_iD+0x03]]
                              ^temp3^block[US_iD+0x01];
                    temp2=x2[block[US_iD+0x02]]^x1[block[US_iD+0x02]]
                              ^x1[block[US_iD+0x03]]
                              ^x2[block[US_iD+0x00]]
                              ^temp3^block[US_iD+0x02];
                    temp3^=x2[block[US_iD+0x03]]^x1[block[US_iD+0x03]]
                              ^x1[block[US_iD+0x00]]
                              ^x2[block[US_iD+0x01]]
                              ^block[US_iD+0x03];
                    block[US_iD+0]=temp0;
                    block[US_iD+1]=temp1;
                    block[US_iD+2]=temp2;
                    block[US_iD+3]=temp3;
                }
            }

        }

        /* s table substitution */
        for(US_iD=0;US_iD<BLOCKSIZE;US_iD++)
        {
            block[US_iD]=SiTable[block[US_iD]];
        }

        /* decode shift row */
        US_DecodeShiftRow(block);

        /* decode key schedule */
        US_DecKeySchedule(key);

        for(US_iD=0;US_iD<BLOCKSIZE;US_iD++)
        {
            block[US_iD] ^= key[US_iD];
        }

        US_roundCounter--;
}

void US_DecodeShiftRow(BYTE* stateTable)
{
    BYTE temp;

    /* first row (row 0) unchanged */

    /* second row (row 1) shifted left by three (or right by one) */
    temp=stateTable[1];
    stateTable[1]=stateTable[13];
    stateTable[13]=stateTable[9];
    stateTable[9]=stateTable[5];
    stateTable[5]=temp;

    /* third row (row 2) shifted left by two */
    temp=stateTable[2];
    stateTable[2]=stateTable[10];
    stateTable[10]=temp;
    temp=stateTable[14];
    stateTable[14]=stateTable[6];
    stateTable[6]=temp;

    /* fourth row (row 3) shifted left by one */
    temp=stateTable[7];
    stateTable[7]=stateTable[11];
    stateTable[11]=stateTable[15];
    stateTable[15]=stateTable[3];
    stateTable[3]=temp;
}

void US_DecKeySchedule(BYTE* key)
{
        /* column 4 */
    key[12]^=key[8];
    key[13]^=key[9];
    key[14]^=key[10];
    key[15]^=key[11];

    /* column 3 */
    key[8]^=key[4];
    key[9]^=key[5];
    key[10]^=key[6];
    key[11]^=key[7];

    /* column 2 */
    key[4]^=key[0];
    key[5]^=key[1];
    key[6]^=key[2];
    key[7]^=key[3];

    /* column 1 */
    key[0]^=STable[key[13]];
    key[1]^=STable[key[14]];
    key[2]^=STable[key[15]];
    key[3]^=STable[key[12]];

    key[0]^= US_rcon;
    if(US_rcon &0x01)
    {
        US_rcon = 0x80;
    }
    else
    {
        US_rcon >>= 1;
    }

}

void US_AESCalcDecodeKey(BYTE* key)
{
    US_roundCounter = 10;

    US_rcon=0x01;
    do
    {
        US_EncKeySchedule(key);
        US_roundCounter--;
    }
    while(US_roundCounter>0);
}

void Decrement_US_Crypto_msTmr(void)
{
    if(US_Enciper_Info.Timeout_ms > 0)
    {
    US_Enciper_Info.Timeout_ms = US_Enciper_Info.Timeout_ms - 1;
    }
    if(US_Deciper_Info.Timeout_ms > 0)
    {
    US_Deciper_Info.Timeout_ms = US_Deciper_Info.Timeout_ms - 1;
    }
}

void Start_US_Data_Encryption(void)
{
 US_Enciper_Info.State = START_ENCRYPTION;
}

void Set_US_Encryption_Idle(void)
{
 US_Enciper_Info.State = ENCRYPTION_IDLE;
}

BYTE Get_US_Encryption_State(void)
{
 BYTE uchState;

 uchState = US_Enciper_Info.State;
 return(uchState);
}

void Start_US_Data_Decryption(void)
{
 US_Deciper_Info.State = START_DECRYPTION;
}

void Set_US_Decryption_Idle(void)
{
 US_Deciper_Info.State = DECRYPTION_IDLE;
}

BYTE Get_US_Decryption_State(void)
{
 BYTE uchState;

 uchState = US_Deciper_Info.State;
 return(uchState);
}
