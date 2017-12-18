#include <xc.h>

#include "COMMON.h"
#include "COMM_DS.h"
#include "AES.h"


static BYTE DS_roundCounter;
static BYTE DS_rcon;
BYTE DS_iE;
BYTE DS_iD;
ciper_message_info DS_Ciper_Message_Info;
extern msg_info_t    Com2XmitObject;    /* COM1: Message Transmission Buffer etc., */
extern msg_info_t    Com2RecvObject;    /* COM1: Message Receiving Buffer etc., */
enciper_info DS_Enciper_Info;
deciper_info DS_Deciper_Info;
extern const BYTE STable[256];
extern const BYTE SiTable[256];
extern const BYTE x1[256];
extern const BYTE x2[256];
extern const BYTE x3[256];
extern const unsigned char E_Key[16];
extern const unsigned char D_Key[16];

void DS_EncodeShiftRow(BYTE* block);
void DS_DecodeShiftRow(BYTE* stateTable);
void DS_EncKeySchedule(BYTE* key);
void DS_DecKeySchedule(BYTE* key);
void Load_DS_Encrypt_Key(void);
void Load_DS_Decrypt_Key(void);


//29/06/10
void Initialise_DS_Crypto_State(void)
{
    DS_Enciper_Info.State = ENCRYPTION_IDLE;
    DS_Deciper_Info.State = DECRYPTION_IDLE;
    DS_Enciper_Info.Timeout_ms = 0;
    DS_Deciper_Info.Timeout_ms = 0;
}

void Update_DS_Data_Encode(void)
{
    switch(DS_Enciper_Info.State)
         {
            case ENCRYPTION_IDLE:
                break;
            case START_ENCRYPTION:
                Load_DS_Encrypt_Key();
                DS_Encode_Key_Addition(Com2XmitObject.Msg_Buffer, DS_Ciper_Message_Info.Encrypt_key);
                DS_Enciper_Info.State = ENCRYPT_KEY_ADDITION;
                DS_Enciper_Info.Timeout_ms = ENCRYPT_TIMEOUT;
                break;
            case ENCRYPT_KEY_ADDITION:
                DS_Encode_Data(Com2XmitObject.Msg_Buffer, DS_Ciper_Message_Info.Encrypt_key);
                if(DS_roundCounter == 0)
                {
                 DS_Enciper_Info.State = ENCRYPTION_COMLETED;
                }
                if(DS_Enciper_Info.Timeout_ms == TIMEOUT_EVENT)
                {
                 DS_Enciper_Info.State = ENCRYPT_FAILURE;
                }
                break;
            case ENCRYPT_FAILURE:
                Nop();
                break;
            case ENCRYPTION_COMLETED:
                break;
        }
}

void Load_DS_Encrypt_Key(void)
{
 BYTE count;
 for(count = 0; count <= BLOCKSIZE; count++)
    {
    DS_Ciper_Message_Info.Encrypt_key[count] = E_Key[count];
    }
}

void DS_Encode_Key_Addition(BYTE* block, BYTE* key)
{

    BYTE i;
    /* initiate round counter = 10 */
    DS_roundCounter = 10;

    DS_rcon = 0x01;

    /* key addition */
    for(i=0;i<BLOCKSIZE;i++)
    {
        block[i] ^= key[i];
    }
}

void DS_Encode_Data(BYTE* block, BYTE* key)
{

    /* s table substitution */
        for(DS_iE=0;DS_iE<BLOCKSIZE;DS_iE++)
        {
            block[DS_iE]=STable[block[DS_iE]];
        }

        /*   encode shift row */
        DS_EncodeShiftRow(block);

        /*   if round counter != 1 */
        if(DS_roundCounter != 1)
        {
            BYTE aux,aux1,aux2,aux3;
            /*     mix column */
            for(DS_iE=0;DS_iE<16;DS_iE+=4)
            {
                aux1= block[DS_iE+0] ^ block[DS_iE+1];
                aux3= block[DS_iE+2]^block[DS_iE+3];
                aux = aux1 ^ aux3;
                aux2= block[DS_iE+2]^block[DS_iE+1];

                aux1 = xtime(aux1);
                aux2 = xtime(aux2);
                aux3 = xtime(aux3);

                block[DS_iE+0]= aux^aux1^block[DS_iE+0];
                block[DS_iE+1]= aux^aux2^block[DS_iE+1];
                block[DS_iE+2]= aux^aux3^block[DS_iE+2];
                block[DS_iE+3]= block[DS_iE+0]^block[DS_iE+1]^block[DS_iE+2]^aux;
            }
        }
        /*   encode key schedule */
        DS_EncKeySchedule(key);

        /*   key addition */
        for(DS_iE=0;DS_iE<BLOCKSIZE;DS_iE++)
        {
            block[DS_iE] ^= key[DS_iE];
        }

        DS_roundCounter--;
}

void DS_EncodeShiftRow(BYTE* stateTable)
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

void DS_EncKeySchedule(BYTE* key)
{
    /* column 1 */
    key[0]^=STable[key[13]];
    key[1]^=STable[key[14]];
    key[2]^=STable[key[15]];
    key[3]^=STable[key[12]];

    key[0]^=DS_rcon;
    DS_rcon = xtime(DS_rcon);

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


void Update_DS_Data_Decode(void)
{
        switch(DS_Deciper_Info.State)
         {
            case DECRYPTION_IDLE:
                break;
            case START_DECRYPTION:
                Load_DS_Decrypt_Key();
                DS_Decode_Key_Addition(Com2RecvObject.Msg_Buffer, DS_Ciper_Message_Info.Decrypt_key);
                DS_Deciper_Info.State = DECRYPT_KEY_ADDITION;
                DS_Deciper_Info.Timeout_ms = DECRYPT_TIMEOUT;
                break;
            case DECRYPT_KEY_ADDITION:
                DS_Decode_Data(Com2RecvObject.Msg_Buffer, DS_Ciper_Message_Info.Decrypt_key);
                if(DS_roundCounter == 0)
                {
                 DS_Deciper_Info.State = DECRYPTION_COMPLETED;
                }
                if(DS_Deciper_Info.Timeout_ms == TIMEOUT_EVENT)
                {
                 DS_Deciper_Info.State = DECRYPT_FAILURE;
                }
                break;
            case DECRYPT_FAILURE:
                Nop();
                break;
            case DECRYPTION_COMPLETED:
                break;
        }
}

void Load_DS_Decrypt_Key(void)
{
 BYTE count;
 for(count = 0; count <= BLOCKSIZE; count++)
    {
    DS_Ciper_Message_Info.Decrypt_key[count] = D_Key[count];
    }
}

void DS_Decode_Key_Addition(BYTE* block, BYTE* key)
{
    BYTE i;

    /* reconstruct key */
    i=1;

    DS_rcon = 0x36;

    /* initiate round counter = 10 */
    DS_roundCounter = 10;

    /* key addition */
    for(i=0;i<BLOCKSIZE;i++)
    {
        block[i] ^= key[i];
    }

}

void DS_Decode_Data(BYTE* block, BYTE* key)
{
    if( DS_roundCounter != 10)
        {
            /* inverse mix column */
            /*     mix column */

            {
                BYTE temp0,temp1,temp2,temp3;
                for(DS_iD=0;DS_iD<16;DS_iD+=4)
                {
                    temp3=x3[block[DS_iD+0x00]]^x3[block[DS_iD+0x01]]^x3[block[DS_iD+0x02]]^x3[block[DS_iD+0x03]]^block[DS_iD+0x00]^block[DS_iD+0x01]^block[DS_iD+0x02]^block[DS_iD+0x03];
                    temp0=x2[block[DS_iD+0x00]]^x1[block[DS_iD+0x00]]
                              ^x1[block[DS_iD+0x01]]
                              ^x2[block[DS_iD+0x02]]
                              ^temp3^block[DS_iD+0x00];
                    temp1=x2[block[DS_iD+0x01]]^x1[block[DS_iD+0x01]]
                              ^x1[block[DS_iD+0x02]]
                              ^x2[block[DS_iD+0x03]]
                              ^temp3^block[DS_iD+0x01];
                    temp2=x2[block[DS_iD+0x02]]^x1[block[DS_iD+0x02]]
                              ^x1[block[DS_iD+0x03]]
                              ^x2[block[DS_iD+0x00]]
                              ^temp3^block[DS_iD+0x02];
                    temp3^=x2[block[DS_iD+0x03]]^x1[block[DS_iD+0x03]]
                              ^x1[block[DS_iD+0x00]]
                              ^x2[block[DS_iD+0x01]]
                              ^block[DS_iD+0x03];
                    block[DS_iD+0]=temp0;
                    block[DS_iD+1]=temp1;
                    block[DS_iD+2]=temp2;
                    block[DS_iD+3]=temp3;
                }
            }

        }

        /* s table substitution */
        for(DS_iD=0;DS_iD<BLOCKSIZE;DS_iD++)
        {
            block[DS_iD]=SiTable[block[DS_iD]];
        }

        /* decode shift row */
        DS_DecodeShiftRow(block);

        /* decode key schedule */
        DS_DecKeySchedule(key);

        for(DS_iD=0;DS_iD<BLOCKSIZE;DS_iD++)
        {
            block[DS_iD] ^= key[DS_iD];
        }

        DS_roundCounter--;
}

void DS_DecodeShiftRow(BYTE* stateTable)
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

void DS_DecKeySchedule(BYTE* key)
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

    key[0]^= DS_rcon;
    if(DS_rcon &0x01)
    {
        DS_rcon = 0x80;
    }
    else
    {
        DS_rcon >>= 1;
    }

}

void DS_AESCalcDecodeKey(BYTE* key)
{
    DS_roundCounter = 10;

    DS_rcon=0x01;
    do
    {
        DS_EncKeySchedule(key);
        DS_roundCounter--;
    }
    while(DS_roundCounter>0);
}

void Decrement_DS_Crypto_msTmr(void)
{
    if(DS_Enciper_Info.Timeout_ms > 0)
    {
     DS_Enciper_Info.Timeout_ms = DS_Enciper_Info.Timeout_ms - 1;
    }

    if(DS_Deciper_Info.Timeout_ms > 0)
    {
     DS_Deciper_Info.Timeout_ms = DS_Deciper_Info.Timeout_ms - 1;
    }
}

void Start_DS_Data_Encryption(void)
{
 DS_Enciper_Info.State = START_ENCRYPTION;
}

void Set_DS_Encryption_Idle(void)
{
 DS_Enciper_Info.State = ENCRYPTION_IDLE;
}

BYTE Get_DS_Encryption_State(void)
{
 BYTE uchState;

 uchState = DS_Enciper_Info.State;
 return(uchState);
}

void Start_DS_Data_Decryption(void)
{
 DS_Deciper_Info.State = START_DECRYPTION;
}

void Set_DS_Decryption_Idle(void)
{
 DS_Deciper_Info.State = DECRYPTION_IDLE;
}

BYTE Get_DS_Decryption_State(void)
{
 BYTE uchState;

 uchState = DS_Deciper_Info.State;
 return(uchState);
}
