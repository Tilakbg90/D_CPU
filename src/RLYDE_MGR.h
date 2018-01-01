/*****************************************************************************

	Project			    : 	Single Section Digital Axle Counter
	Version			    : 	2.0
	Revision		    :	5
	Module Version	    :	1.0
	Component name	    : 	RLYDE_MGR
	Target MCU		    : 	PIC24FJ256GB210
    Compiler            :   XC16 V1.31
    Author              :   S Venkata Krishna
    Date                :   15/12/2017
    Company Name        :   Insys Digital Systems Private Limited, Bangalore
	Modification History:
					|-------------|---------------|-----------------|-------------|------------------------------|
					|	Rev No	  |		PR	 	  |	ATR				|	Date	  |	Description					 |
					|-------------|---------------|-----------------|-------------|------------------------------|






*****************************************************************************/
#ifndef _RLYDE_MGR_H_
#define _RLYDE_MGR_H_
extern BOOL Reset_Allowed_For_DE(void);
extern void Start_Relay_DE_Mgr(void);
extern void Initialise_Relay_DE_Mgr(void);						//Defined in RLYDE_MGR.c
extern void Update_Relay_DE_State(void);						//Defined in RLYDE_MGR.c
extern void Set_Relay_DE_DAC_Defective(void);					//Defined in RLYDE_MGR.c
extern void Process_Relay_DE_Local_Direction(BYTE);				//Defined in RLYDE_MGR.c
extern void Process_Relay_DE_Local_AxleCount(UINT16,UINT16);	//Defined in RLYDE_MGR.c
extern void Process_Relay_DE_Peer_Direction( BYTE uchDirection);
extern void Process_Relay_DE_Peer_Direction( BYTE uchDirection);
extern void Process_Relay_DE_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);
#endif