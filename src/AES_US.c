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

void US_EncodeShiftRow(void);
void US_DecodeShiftRow(void);
void US_EncKeySchedule(void);
void US_DecKeySchedule(void);
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
                US_Encode_Key_Addition();
                US_Enciper_Info.State = ENCRYPT_KEY_ADDITION;
                US_Enciper_Info.Timeout_ms = ENCRYPT_TIMEOUT;
                break;
            case ENCRYPT_KEY_ADDITION:
                US_Encode_Data();
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

void US_Encode_Key_Addition(void)
{

    BYTE i;
    /* initiate round counter = 10 */
    US_roundCounter = 10;

    US_rcon = 0x01;

    /* key addition */
    for(i=0;i<BLOCKSIZE;i++)
    {
        Com1XmitObject.Msg_Buffer[i] ^= US_Ciper_Message_Info.Encrypt_key[i];
//        block[i] ^= key[i];
    }
}

void US_Encode_Data(void)
{

    /* s table substitution */
        for(US_iE=0;US_iE<BLOCKSIZE;US_iE++)
        {
            Com1XmitObject.Msg_Buffer[US_iE]=STable[Com1XmitObject.Msg_Buffer[US_iE]];
        }

        /*   encode shift row */
        US_EncodeShiftRow();

        /*   if round counter != 1 */
        if(US_roundCounter != 1)
        {
            BYTE aux,aux1,aux2,aux3;
            /*     mix column */
            for(US_iE=0;US_iE<16;US_iE+=4)
            {
                aux1= Com1XmitObject.Msg_Buffer[US_iE+0] ^ Com1XmitObject.Msg_Buffer[US_iE+1];
                aux3= Com1XmitObject.Msg_Buffer[US_iE+2]^Com1XmitObject.Msg_Buffer[US_iE+3];
                aux = aux1 ^ aux3;
                aux2= Com1XmitObject.Msg_Buffer[US_iE+2]^Com1XmitObject.Msg_Buffer[US_iE+1];

                aux1 = xtime(aux1);
                aux2 = xtime(aux2);
                aux3 = xtime(aux3);

                Com1XmitObject.Msg_Buffer[US_iE+0]= aux^aux1^Com1XmitObject.Msg_Buffer[US_iE+0];
                Com1XmitObject.Msg_Buffer[US_iE+1]= aux^aux2^Com1XmitObject.Msg_Buffer[US_iE+1];
                Com1XmitObject.Msg_Buffer[US_iE+2]= aux^aux3^Com1XmitObject.Msg_Buffer[US_iE+2];
                Com1XmitObject.Msg_Buffer[US_iE+3]= Com1XmitObject.Msg_Buffer[US_iE+0]^Com1XmitObject.Msg_Buffer[US_iE+1]^Com1XmitObject.Msg_Buffer[US_iE+2]^aux;
            }
        }
        /*   encode key schedule */
        US_EncKeySchedule();

        /*   key addition */
        for(US_iE=0;US_iE<BLOCKSIZE;US_iE++)
        {
            Com1XmitObject.Msg_Buffer[US_iE] ^= US_Ciper_Message_Info.Encrypt_key[US_iE];
        }

        US_roundCounter--;
}

void US_EncodeShiftRow()
{
    BYTE temp;

    /* first row (row 0) unchanged */

    /* second row (row 1) shifted left by one */
    temp=Com1XmitObject.Msg_Buffer[1];
    Com1XmitObject.Msg_Buffer[1]=Com1XmitObject.Msg_Buffer[5];
    Com1XmitObject.Msg_Buffer[5]=Com1XmitObject.Msg_Buffer[9];
    Com1XmitObject.Msg_Buffer[9]=Com1XmitObject.Msg_Buffer[13];
    Com1XmitObject.Msg_Buffer[13]=temp;

    /* third row (row 2) shifted left by two */
    temp=Com1XmitObject.Msg_Buffer[2];
    Com1XmitObject.Msg_Buffer[2]=Com1XmitObject.Msg_Buffer[10];
    Com1XmitObject.Msg_Buffer[10]=temp;
    temp=Com1XmitObject.Msg_Buffer[14];
    Com1XmitObject.Msg_Buffer[14]=Com1XmitObject.Msg_Buffer[6];
    Com1XmitObject.Msg_Buffer[6]=temp;

    /* fourth row (row 3) shifted left by three (or right by one) */
    temp=Com1XmitObject.Msg_Buffer[3];
    Com1XmitObject.Msg_Buffer[3]=Com1XmitObject.Msg_Buffer[15];
    Com1XmitObject.Msg_Buffer[15]=Com1XmitObject.Msg_Buffer[11];
    Com1XmitObject.Msg_Buffer[11]=Com1XmitObject.Msg_Buffer[7];
    Com1XmitObject.Msg_Buffer[7]=temp;
}

void US_EncKeySchedule()
{
    /* column 1 */
    US_Ciper_Message_Info.Encrypt_key[0]^=STable[US_Ciper_Message_Info.Encrypt_key[13]];
    US_Ciper_Message_Info.Encrypt_key[1]^=STable[US_Ciper_Message_Info.Encrypt_key[14]];
    US_Ciper_Message_Info.Encrypt_key[2]^=STable[US_Ciper_Message_Info.Encrypt_key[15]];
    US_Ciper_Message_Info.Encrypt_key[3]^=STable[US_Ciper_Message_Info.Encrypt_key[12]];

    US_Ciper_Message_Info.Encrypt_key[0]^=US_rcon;
    US_rcon = xtime(US_rcon);

    /* column 2 */
    US_Ciper_Message_Info.Encrypt_key[4]^=US_Ciper_Message_Info.Encrypt_key[0];
    US_Ciper_Message_Info.Encrypt_key[5]^=US_Ciper_Message_Info.Encrypt_key[1];
    US_Ciper_Message_Info.Encrypt_key[6]^=US_Ciper_Message_Info.Encrypt_key[2];
    US_Ciper_Message_Info.Encrypt_key[7]^=US_Ciper_Message_Info.Encrypt_key[3];

    /* column 3 */
    US_Ciper_Message_Info.Encrypt_key[8]^=US_Ciper_Message_Info.Encrypt_key[4];
    US_Ciper_Message_Info.Encrypt_key[9]^=US_Ciper_Message_Info.Encrypt_key[5];
    US_Ciper_Message_Info.Encrypt_key[10]^=US_Ciper_Message_Info.Encrypt_key[6];
    US_Ciper_Message_Info.Encrypt_key[11]^=US_Ciper_Message_Info.Encrypt_key[7];

    /* column 4 */
    US_Ciper_Message_Info.Encrypt_key[12]^=US_Ciper_Message_Info.Encrypt_key[8];
    US_Ciper_Message_Info.Encrypt_key[13]^=US_Ciper_Message_Info.Encrypt_key[9];
    US_Ciper_Message_Info.Encrypt_key[14]^=US_Ciper_Message_Info.Encrypt_key[10];
    US_Ciper_Message_Info.Encrypt_key[15]^=US_Ciper_Message_Info.Encrypt_key[11];

}


void Update_US_Data_Decode(void)
{
        switch(US_Deciper_Info.State)
         {
            case DECRYPTION_IDLE:
                break;
            case START_DECRYPTION:
                Load_Decrypt_Key();
                US_Decode_Key_Addition();
                US_Deciper_Info.State = DECRYPT_KEY_ADDITION;
                US_Deciper_Info.Timeout_ms = DECRYPT_TIMEOUT;
                break;
            case DECRYPT_KEY_ADDITION:
                US_Decode_Data();
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

void US_Decode_Key_Addition()
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
        Com1RecvObject.Msg_Buffer[i] ^= US_Ciper_Message_Info.Decrypt_key[i];
    }

}

void US_Decode_Data(void)
{
    if( US_roundCounter != 10)
        {
            /* inverse mix column */
            /*     mix column */

            {
                BYTE temp0,temp1,temp2,temp3;
                for(US_iD=0;US_iD<16;US_iD+=4)
                {
                    temp3=x3[Com1RecvObject.Msg_Buffer[US_iD+0x00]]^x3[Com1RecvObject.Msg_Buffer[US_iD+0x01]]^x3[Com1RecvObject.Msg_Buffer[US_iD+0x02]]^x3[Com1RecvObject.Msg_Buffer[US_iD+0x03]]^Com1RecvObject.Msg_Buffer[US_iD+0x00]^Com1RecvObject.Msg_Buffer[US_iD+0x01]^Com1RecvObject.Msg_Buffer[US_iD+0x02]^Com1RecvObject.Msg_Buffer[US_iD+0x03];
                    temp0=x2[Com1RecvObject.Msg_Buffer[US_iD+0x00]]^x1[Com1RecvObject.Msg_Buffer[US_iD+0x00]]
                              ^x1[Com1RecvObject.Msg_Buffer[US_iD+0x01]]
                              ^x2[Com1RecvObject.Msg_Buffer[US_iD+0x02]]
                              ^temp3^Com1RecvObject.Msg_Buffer[US_iD+0x00];
                    temp1=x2[Com1RecvObject.Msg_Buffer[US_iD+0x01]]^x1[Com1RecvObject.Msg_Buffer[US_iD+0x01]]
                              ^x1[Com1RecvObject.Msg_Buffer[US_iD+0x02]]
                              ^x2[Com1RecvObject.Msg_Buffer[US_iD+0x03]]
                              ^temp3^Com1RecvObject.Msg_Buffer[US_iD+0x01];
                    temp2=x2[Com1RecvObject.Msg_Buffer[US_iD+0x02]]^x1[Com1RecvObject.Msg_Buffer[US_iD+0x02]]
                              ^x1[Com1RecvObject.Msg_Buffer[US_iD+0x03]]
                              ^x2[Com1RecvObject.Msg_Buffer[US_iD+0x00]]
                              ^temp3^Com1RecvObject.Msg_Buffer[US_iD+0x02];
                    temp3^=x2[Com1RecvObject.Msg_Buffer[US_iD+0x03]]^x1[Com1RecvObject.Msg_Buffer[US_iD+0x03]]
                              ^x1[Com1RecvObject.Msg_Buffer[US_iD+0x00]]
                              ^x2[Com1RecvObject.Msg_Buffer[US_iD+0x01]]
                              ^Com1RecvObject.Msg_Buffer[US_iD+0x03];
                    Com1RecvObject.Msg_Buffer[US_iD+0]=temp0;
                    Com1RecvObject.Msg_Buffer[US_iD+1]=temp1;
                    Com1RecvObject.Msg_Buffer[US_iD+2]=temp2;
                    Com1RecvObject.Msg_Buffer[US_iD+3]=temp3;
                }
            }

        }

        /* s table substitution */
        for(US_iD=0;US_iD<BLOCKSIZE;US_iD++)
        {
            Com1RecvObject.Msg_Buffer[US_iD]=SiTable[Com1RecvObject.Msg_Buffer[US_iD]];
        }

        /* decode shift row */
        US_DecodeShiftRow();

        /* decode US_Ciper_Message_Info.Decrypt_key schedule */
        US_DecKeySchedule();

        for(US_iD=0;US_iD<BLOCKSIZE;US_iD++)
        {
            Com1RecvObject.Msg_Buffer[US_iD] ^= US_Ciper_Message_Info.Decrypt_key[US_iD];
        }

        US_roundCounter--;
}

void US_DecodeShiftRow()
{
    BYTE temp;

    /* first row (row 0) unchanged */

    /* second row (row 1) shifted left by three (or right by one) */
    temp=Com1RecvObject.Msg_Buffer[1];
    Com1RecvObject.Msg_Buffer[1]=Com1RecvObject.Msg_Buffer[13];
    Com1RecvObject.Msg_Buffer[13]=Com1RecvObject.Msg_Buffer[9];
    Com1RecvObject.Msg_Buffer[9]=Com1RecvObject.Msg_Buffer[5];
    Com1RecvObject.Msg_Buffer[5]=temp;

    /* third row (row 2) shifted left by two */
    temp=Com1RecvObject.Msg_Buffer[2];
    Com1RecvObject.Msg_Buffer[2]=Com1RecvObject.Msg_Buffer[10];
    Com1RecvObject.Msg_Buffer[10]=temp;
    temp=Com1RecvObject.Msg_Buffer[14];
    Com1RecvObject.Msg_Buffer[14]=Com1RecvObject.Msg_Buffer[6];
    Com1RecvObject.Msg_Buffer[6]=temp;

    /* fourth row (row 3) shifted left by one */
    temp=Com1RecvObject.Msg_Buffer[7];
    Com1RecvObject.Msg_Buffer[7]=Com1RecvObject.Msg_Buffer[11];
    Com1RecvObject.Msg_Buffer[11]=Com1RecvObject.Msg_Buffer[15];
    Com1RecvObject.Msg_Buffer[15]=Com1RecvObject.Msg_Buffer[3];
    Com1RecvObject.Msg_Buffer[3]=temp;
}

void US_DecKeySchedule()
{
        /* column 4 */
    US_Ciper_Message_Info.Decrypt_key[12]^=US_Ciper_Message_Info.Decrypt_key[8];
    US_Ciper_Message_Info.Decrypt_key[13]^=US_Ciper_Message_Info.Decrypt_key[9];
    US_Ciper_Message_Info.Decrypt_key[14]^=US_Ciper_Message_Info.Decrypt_key[10];
    US_Ciper_Message_Info.Decrypt_key[15]^=US_Ciper_Message_Info.Decrypt_key[11];

    /* column 3 */
    US_Ciper_Message_Info.Decrypt_key[8]^=US_Ciper_Message_Info.Decrypt_key[4];
    US_Ciper_Message_Info.Decrypt_key[9]^=US_Ciper_Message_Info.Decrypt_key[5];
    US_Ciper_Message_Info.Decrypt_key[10]^=US_Ciper_Message_Info.Decrypt_key[6];
    US_Ciper_Message_Info.Decrypt_key[11]^=US_Ciper_Message_Info.Decrypt_key[7];

    /* column 2 */
    US_Ciper_Message_Info.Decrypt_key[4]^=US_Ciper_Message_Info.Decrypt_key[0];
    US_Ciper_Message_Info.Decrypt_key[5]^=US_Ciper_Message_Info.Decrypt_key[1];
    US_Ciper_Message_Info.Decrypt_key[6]^=US_Ciper_Message_Info.Decrypt_key[2];
    US_Ciper_Message_Info.Decrypt_key[7]^=US_Ciper_Message_Info.Decrypt_key[3];

    /* column 1 */
    US_Ciper_Message_Info.Decrypt_key[0]^=STable[US_Ciper_Message_Info.Decrypt_key[13]];
    US_Ciper_Message_Info.Decrypt_key[1]^=STable[US_Ciper_Message_Info.Decrypt_key[14]];
    US_Ciper_Message_Info.Decrypt_key[2]^=STable[US_Ciper_Message_Info.Decrypt_key[15]];
    US_Ciper_Message_Info.Decrypt_key[3]^=STable[US_Ciper_Message_Info.Decrypt_key[12]];

    US_Ciper_Message_Info.Decrypt_key[0]^= US_rcon;
    if(US_rcon &0x01)
    {
        US_rcon = 0x80;
    }
    else
    {
        US_rcon >>= 1;
    }

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
