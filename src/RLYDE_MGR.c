/*****************************************************************************

	Project			: 	Single Section Digital Axle Counter
	Version			: 	2.0
	Revision		:	5
	Module Version	:	0
	Component name	: 	RLYDE_MGR
	Target MCU		: 	PIC 18F8722
    Compiler		:
	Author			:	Mohan Kumar	and Vijay Kumar
	Date			:	09/02/2005
	Company Name	: 	G G Tronics India Private Limited,Bangalore
	Modification History:
					|-------------|---------------|-----------------|-------------|------------------------------|
					|	Rev No	  |		PR	 	  |	ATR				|	Date	  |	Description					 |
					|-------------|---------------|-----------------|-------------|------------------------------|




	Functions	: 	void Initialise_Relay_DE_Mgr(void)
					void Start_Relay_DE_Mgr(void)
					void Update_Relay_DE_State(void)
					void Process_Relay_DE_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
					void Process_Relay_DE_Local_Direction(BYTE uchDirection)
					void Process_Relay_DE_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
					void Process_Relay_DE_Peer_Direction( BYTE uchDirection)
					void Update_DE_LU1_Fwd_Count(UINT16 uiCount)
					void Update_DE_LU2_Fwd_Count(UINT16 uiCount)
					void Update_DE_LU1_Rev_Count(UINT16 uiCount)
					void Update_DE_LU2_Rev_Count(UINT16 uiCount)
					void Update_DE_LU1_Direction(BYTE Direction)
					void Update_DE_LU2_Direction(BYTE Direction)
					BOOL Reset_Allowed_For_DE()
					void Set_Relay_DE_DAC_Defective(void)
					BYTE Get_Relay_DE_State(void)
					void Clear_Relay_DE_Counts(void)

*****************************************************************************/

#include "COMMON.h"
#include "RELAYDRV.h"
#include "RLYDE_MGR.h"
#include "RELAYMGR.h"
#include "RESET.h"
#include "ERROR.h"
#include "AXLE_MON.h"

extern /*near*/ dac_status_t Status;					/* from DAC_MAIN.c */
extern /*near*/ dip_switch_info_t DIP_Switch_Info;		/* from DAC_MAIN.c */
relay_de_info_t Relay_DE_Info;


void Clear_Relay_DE_Counts(void);
void Update_DE_LU1_Fwd_Count(UINT16);
void Update_DE_LU2_Fwd_Count(UINT16);
void Update_DE_LU1_Rev_Count(UINT16);
void Update_DE_LU2_Rev_Count(UINT16);
void Update_DE_LU1_Direction(BYTE);
void Update_DE_LU2_Direction(BYTE);
void Start_Relay_DE_Mgr(void);
BOOL Reset_Allowed_For_DE(void);
void  Process_Relay_DE_Peer_Direction( BYTE uchDirection);
void  Process_Relay_DE_Peer_Direction( BYTE uchDirection);
void Process_Relay_DE_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount);
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Initialise_Relay_DE_Mgr(void)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Initialize the Relay DE info state to "RELAY_MANAGER_NOT_STARTED"

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	RELAYMGR.C-Initialise_Relay_Mgr()


Input Variables						Name				Type
	Global			:	RELAY_MANAGER_NOT_STARTED		Enumerator

	Local			:	None

Output Variables					Name				Type
	Global			:	Relay_DE_Info.State				Enumertor

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None
References			:

Derived Requirements:
************************************************************************/
void Initialise_Relay_DE_Mgr(void)
{
	Relay_DE_Info.State = RELAY_MANAGER_NOT_STARTED;
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Start_Relay_DE_Mgr(void)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Once the initialization over Start the relay manager by setting Relay DE state to
					"TRAIN_IN_SECTION".Call Display_Clear_status to display "CL" on 7 segment display.
					 and clear the down stream Local counts, reset flags and reset state


Design Requirements:

Interfaces
	Calls			:	RELAYMGR.C-Display_Clear_Status()
						AXLE_MON.C-Clear_DS_Local_Counts()
						RLYDE_MGR.C-Clear_Relay_DE_Counts()
						RESET.C-Clear_Reset_Info()

	Called by		:	DAC_MAIN.C-Main()

Input Variables						Name				Type
	Global			:	TRAIN_IN_SECTION				Enumerator

	Local			:	None

Output Variables					Name				Type
	Global			:	Relay_DE_Info.State				Enumerator

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None

References			:

Derived Requirements:
************************************************************************/
void Start_Relay_DE_Mgr(void)
{
	Relay_DE_Info.State = TRAIN_IN_SECTION;
	Display_Clear_Status();
	Clear_DS_Local_Counts();			/*clear local counts */
	Clear_Relay_DE_Counts();
	Clear_Reset_Info();					/* Clear the reset flags and reset state */

}

/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Update_Relay_DE_State(void)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:When train enters the section DeEnergize the vital relay and
					 Energize the relay when train leave the section


Design Requirements:


Abstract			:When train enters the section DeEnergize the vital relay and
					 Energize the relay when train leave the section
Interfaces
	Calls			:	RELAYDRV.C-DeEnergise_Vital_Relay_B()
						RELAYMGR.C-Display_Occupied_Status()
						RESET.C-Clear_Local_Reset_Flag()
						ERROR.C-Set_Error_Status_Bit()
						RELAYMGR.C-Declare_DAC_Defective()
						RELAYDRV.C-Get_Relay_Energising_Key()
						RELAYDRV.C-Energise_Vital_Relay_B()
						AXLE_MON.C-Clear_DS_AxleCount()
						RELAYMGR.C-Display_Clear_Status()


	Called by		:	RELAYMGR.C-Update_Relay_DE_Counts()

Input Variables						Name				Type
	Global			:	Relay_DE_Info.LU1_Fwd_Count		UINT16
						Relay_DE_Info.LU1_Rev_Count		UINT16
						Relay_DE_Info.LU2_Fwd_Count		UINT16
						Relay_DE_Info.LU2_Rev_Count		UINT16
						TRAIN_IN_SECTION				Enumerator
						Relay_DE_Info.State				Enumerator
						Relay_DE_Info.LU1_Direction 	Byte
						Relay_DE_Info.LU2_Direction		Byte
						REVERSE_DIRECTION				Enumerator
						NO_TRAIN_IN_SECTION				Enumerator


	Local			:	uiAuthorisationKey				UINT16

Output Variables					Name				Type
	Global			:	Relay_DE_Info.State				Enumerator
						Status.Flags.Direct_Out_Count	Bit

	Local			:	None
Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|


Macros defined		:					Macro 					Value
										DIRECT_OUT_COUNT_ID		 43
										SET_HIGH				  1

References			:

Derived Requirements:
Algorithm			:1.	In the "NO_TRAIN_IN_SECTION" state if Local Unit CPU1 forward
						or Local unit CPU2 forward count or Local unit CPU1 reverse count
						or Local unit CPU2 reverse count is not equal to zero means train
					   	enter the section,,deenergize the Vital relay B,assign next
						"TRAIN_IN_SECTION" to DE info state.Display "OC" on 7 segment,
						Clear the all local reset flags
					 2.	In the "TRAIN_IN_SECTION" state check for Local unit CPU1 or
						Local unit CPU2 direction if it is REVERSE direction there is no
						in counts, only direct count, set DIRECT_OUT_COUNT_ERROR bit, declare
						DAC defective
					 3.	Check for the Local unit CPU1 forward count or Local unit CPU2 forward
						count or Local unit CPU1 reverse count or Local unit CPU2 reverse count,
						if that is  zero means section is cleared,Get the Authrosation
						key and energize the Vital relay A and display the "CL" on 7 segmnet and
						clear the Down stream axle counts
************************************************************************/
void Update_Relay_DE_State(void)
{
	UINT16 uiAuthorisationKey;

	switch (Relay_DE_Info.State)
	{
		case NO_TRAIN_IN_SECTION:
			if (Relay_DE_Info.LU1_Fwd_Count != 0 ||	Relay_DE_Info.LU2_Fwd_Count != 0 ||
				Relay_DE_Info.LU1_Rev_Count != 0 || Relay_DE_Info.LU2_Rev_Count != 0 )
			{
				/* Train Occupied the section */
				DeEnergise_Vital_Relay_B();
				Relay_DE_Info.State = TRAIN_IN_SECTION;
				Display_Occupied_Status();
				Clear_Local_Reset_Flag();
			}
			break;
		case TRAIN_IN_SECTION:
			if(Relay_DE_Info.LU1_Direction  == REVERSE_DIRECTION ||
			   Relay_DE_Info.LU2_Direction  == REVERSE_DIRECTION )
		    {
				/* no in-counts, only out-counts,system goes to error state*/
				  Set_Error_Status_Bit(DIRECT_OUT_COUNT_ID);
				  Status.Flags.Direct_Out_Count = SET_HIGH;
				  Declare_DAC_Defective();
				  break;
		    }
			if (Relay_DE_Info.LU1_Fwd_Count == 0 &&	Relay_DE_Info.LU2_Fwd_Count == 0 &&
				Relay_DE_Info.LU1_Rev_Count == 0 &&	Relay_DE_Info.LU2_Rev_Count == 0 )
			{
				/* Train cleared the section */
				uiAuthorisationKey = Get_Relay_Energising_Key();
				Energise_Vital_Relay_B(uiAuthorisationKey);
				Relay_DE_Info.State = NO_TRAIN_IN_SECTION;
				Clear_DS_AxleCount();
				Display_Clear_Status();
			}
			break;
		 default:
			    break;

	}
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Process_Relay_DE_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Check for CPU1 or CPU2,if it is CPU1 update the Local Unit Forward and
					reverse count in CPU1 else update in CPU2

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	RLYDE_MGR.C-Update_DE_LU1_Fwd_Count()
						RLYDE_MGR.C-Update_DE_LU1_Rev_Count()
						RLYDE_MGR.C-Update_DE_LU2_Fwd_Count()
						RLYDE_MGR.C-Update_DE_LU2_Rev_Count()

	Called by		:	RELAYMGR.C-Update_Relay_DE_Counts()


Input Variables						Name				Type
	Global			:	DIP_Switch_Info.Flags.Is_CPU1	Bit

	Local			:	uiFwdAxleCount					UINT16
						uiRevAxleCount					UINT16

Output Variables					Name				Type
	Global			:	None

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|
Macros defined		:					Macro 					Value
										SET_HIGH				  1

References			:

Derived Requirements:
************************************************************************/
void Process_Relay_DE_Local_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{
		if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
		{
		Update_DE_LU1_Fwd_Count(uiFwdAxleCount);
		Update_DE_LU1_Rev_Count(uiRevAxleCount);
		}
		else
		{
		Update_DE_LU2_Fwd_Count(uiFwdAxleCount);
		Update_DE_LU2_Rev_Count(uiRevAxleCount);
		}
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Process_Relay_DE_Local_Direction(BYTE uchDirection)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:If the Is_CPU1 flag is set update the direction in CPU1 else
					 update the diretcion in CPU2

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	RLYDE_MGR.C-Update_DE_LU1_Direction()
						RLYDE_MGR.C-Update_DE_LU2_Direction()


	Called by		:	RELAYMGR.C-Update_Relay_DE_Counts()


Input Variables						Name				Type
	Global			:	DIP_Switch_Info.Flags.Is_CPU1	Bit

	Local			:	uchDirection					BYTE

Output Variables					Name				Type
	Global			:	None

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										SET_HIGH				  1

References			:

Derived Requirements:
************************************************************************/
void Process_Relay_DE_Local_Direction(BYTE uchDirection)
{
	if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
	{
		Update_DE_LU1_Direction((BYTE) uchDirection);
	}
	else
	{
		Update_DE_LU2_Direction((BYTE) uchDirection);
	}
}

/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Process_Relay_DE_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Update the Peer axle count.If Is_CPU1 flag is set update the Local unit
					 Forward and Reverse count in CPU2 else update axle count in CPU1

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	RLYDE_MGR.C-Update_DE_LU2_Fwd_Count()
						RLYDE_MGR.C-Update_DE_LU2_Rev_Count()
						RLYDE_MGR.C-Update_DE_LU1_Fwd_Count()
						RLYDE_MGR.C-Update_DE_LU1_Rev_Count()

	Called by		:	COMM_SM.C-Process_Interprocess_Message()


Input Variables						Name				Type
	Global			:	DIP_Switch_Info.Flags.Is_CPU1	Bit

	Local			:	uiFwdAxleCount					UINT16
						uiRevAxleCount					UINT16

Output Variables					Name				Type
	Global			:	None

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										SET_HIGH				  1
References			:

Derived Requirements:
************************************************************************/
void Process_Relay_DE_Peer_AxleCount(UINT16 uiFwdAxleCount,UINT16 uiRevAxleCount)
{
		if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
		{
			Update_DE_LU2_Fwd_Count(uiFwdAxleCount);
			Update_DE_LU2_Rev_Count(uiRevAxleCount);
		}
		else
		{
			Update_DE_LU1_Fwd_Count(uiFwdAxleCount);
			Update_DE_LU1_Rev_Count(uiRevAxleCount);
		}
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void  Process_Relay_DE_Peer_Direction( BYTE uchDirection)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Update the Peer CPU direction.If Is_CPU1 flag is set update the direction
					 in CPU2 else update direction in CPU1

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	RLYDE_MGR.C-Update_DE_LU2_Direction()
						RLYDE_MGR.C-Update_DE_LU1_Direction()

	Called by		:

Input Variables						Name				Type
	Global			:	DIP_Switch_Info.Flags.Is_CPU1	Bit

	Local			:	uchDirection					BYTE

Output Variables					Name				Type
	Global			:	None

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										SET_HIGH				  1

References			:

Derived Requirements:
************************************************************************/
void  Process_Relay_DE_Peer_Direction( BYTE uchDirection)
{
	if(DIP_Switch_Info.Flags.Is_DAC_CPU1 == SET_HIGH)
	{
		Update_DE_LU2_Direction((BYTE) uchDirection);
	}
	else
	{
		Update_DE_LU1_Direction((BYTE) uchDirection);
	}
}

/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Update_DE_LU1_Fwd_Count(UINT16 uiCount)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Update Dead End the Local Unit CPU1 Forward count

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	RLYDE_MGR.C-Process_Relay_DE_Local_AxleCount()
						RLYDE_MGR.C-Process_Relay_DE_Peer_AxleCount()

Input Variables						Name				Type
	Global			:	None

	Local			:	uiCount							UINT16

Output Variables					Name				Type
	Global			:	Relay_DE_Info.LU1_Fwd_Count		UINT16

	Local			:	None
Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None

References			:

Derived Requirements:
***********************************************************************/
void Update_DE_LU1_Fwd_Count(UINT16 uiCount)
{
	Relay_DE_Info.LU1_Fwd_Count = uiCount;
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Update_DE_LU2_Fwd_Count(UINT16 uiCount)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Update Dead End the Local Unit CPU2 Forward count

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	RLYDE_MGR.C-Process_Relay_DE_Local_AxleCount()
						RLYDE_MGR.C-Process_Relay_DE_Peer_AxleCount()

Input Variables						Name				Type
	Global			:	None

	Local			:	uiCount							UINT16

Output Variables					Name				Type
	Global			:	Relay_DE_Info.LU2_Fwd_Count		UINT16

	Local			:	None
Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|


Macros defined		:					Macro 					Value
										None					None
References			:

Derived Requirements:
************************************************************************/
void Update_DE_LU2_Fwd_Count(UINT16 uiCount)
{
	Relay_DE_Info.LU2_Fwd_Count = uiCount;
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Update_DE_LU1_Rev_Count(UINT16 uiCount)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Update Dead End the Local Unit CPU1 Reverse count

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	RLYDE_MGR.C-Process_Relay_DE_Local_AxleCount()
						RLYDE_MGR.C-Process_Relay_DE_Peer_AxleCount()

Input Variables						Name				Type
	Global			:	None

	Local			:	uiCount							UINT16

Output Variables					Name				Type
	Global			:	Relay_DE_Info.LU1_Rev_Count 	UINT16

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None

References			:

Derived Requirements:
************************************************************************/
void Update_DE_LU1_Rev_Count(UINT16 uiCount)
{
	Relay_DE_Info.LU1_Rev_Count = uiCount;
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Update_DE_LU2_Rev_Count(UINT16 uiCount)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Update Dead End the Local Unit CPU2 Reverse count

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	RLYDE_MGR.C-Process_Relay_DE_Local_AxleCount()
						RLYDE_MGR.C-Process_Relay_DE_Peer_AxleCount()

Input Variables						Name				Type
	Global			:	None

	Local			:	uiCount							UINT16

Output Variables					Name				Type
	Global			:	Relay_DE_Info.LU2_Rev_Count		UINT16

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None
References			:

Derived Requirements:
***********************************************************************/
void Update_DE_LU2_Rev_Count(UINT16 uiCount)
{
	Relay_DE_Info.LU2_Rev_Count = uiCount;
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Update_DE_LU1_Direction(BYTE Direction)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Update the Dead End Local Unit CPU1 Direction

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	RLYDE_MGR.C-Process_Relay_DE_Local_Direction()
						RLYDE_MGR.C-Process_Relay_DE_Peer_Direction()


Input Variables						Name				Type
	Global			:	None

	Local			:	Direction						BYTE

Output Variables					Name				Type
	Global			:	Relay_DE_Info.LU1_Direction		Byte

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None

References			:
Derived Requirements:
***********************************************************************/
void Update_DE_LU1_Direction(BYTE Direction)
{
	Relay_DE_Info.LU1_Direction = Direction;
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Update_DE_LU2_Direction(BYTE Direction)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Update the Dead End Local Unit CPU2 Direction

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	RLYDE_MGR.C-Process_Relay_DE_Local_Direction()
						RLYDE_MGR.C-Process_Relay_DE_Peer_Direction()

Input Variables						Name				Type
	Global			:	None

	Local			:	Direction						BYTE

Output Variables					Name				Type
	Global			:	Relay_DE_Info.LU2_Direction		BYTE

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None
References			:

Derived Requirements:
************************************************************************/
void Update_DE_LU2_Direction(BYTE Direction)
{
	Relay_DE_Info.LU2_Direction = Direction;
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:BOOL Reset_Allowed_For_DE()
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:This function return TRUE when any DAC defective is there
					 or when shunting is there else this return FALSE

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	AXLE_MON.C-Get_Shunting_State()

	Called by		:	RESET.C-DE_Wait_for_Local_Reset()

Input Variables						Name				Type
	Global			:	DAC_DEFECTIVE					Enumerator
						Relay_DE_Info.State				Enumerator

	Local			:	None

Output Variables					Name				Type
	Global			:	None

	Local			:	ReturnValue						BOOL

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										TRUE					  1
										FALSE					  0

References			:

Derived Requirements:
************************************************************************/
BOOL Reset_Allowed_For_DE(void)
{
	BOOL ReturnValue = FALSE;

	if(	Relay_DE_Info.State == DAC_DEFECTIVE)
	{
		ReturnValue = (BOOL) TRUE;
		return(ReturnValue);
	}
	if(Get_Shunting_State())
	{
		ReturnValue = (BOOL) TRUE;
		return(ReturnValue);
	}
	return(ReturnValue);
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Set_Relay_DE_DAC_Defective(void)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:DeEnergise the Vital relay B and assign DAC DEFECTIVE to
					 relay DE info state

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	RELAYDRV.C-DeEnergise_Vital_Relay_B()

	Called by		:	RELAYMGR.C-Declare_DAC_Defective()
						RELAYMGR.C-Declare_DAC_Defective_DS()


Input Variables						Name				Type
	Global			:	DAC_DEFECTIVE					Enumerator

	Local			:	None

Output Variables					Name				Type
	Global			:	Relay_DE_Info.State				Enumerator

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None

References			:
Derived Requirements:
************************************************************************/
void Set_Relay_DE_DAC_Defective(void)
{
	DeEnergise_Vital_Relay_B();
	Relay_DE_Info.State = DAC_DEFECTIVE;
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:BYTE Get_Relay_DE_State(void)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Get the Dead End Relay status

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	COMM_SM.C-Check_Peer_Status()

Input Variables						Name				Type
	Global			:	None

	Local			:	None

Output Variables					Name				Type
	Global			:	Relay_DE_Info.State				Enumerator

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None

References			:

Derived Requirements:
Algorithm			:
************************************************************************/
BYTE Get_Relay_DE_State(void)
{
 return((BYTE)Relay_DE_Info.State);
}
/*********************************************************************
Component name 		:RLYA_MGR
Module Name			:void Clear_Relay_DE_Counts(void)
Created By			:
Date Created		:
Modification History:
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |   Rev No    |     PR        | ATR             |   Date      | Description                  |
                    |-------------|---------------|-----------------|-------------|------------------------------|
                    |             |               |                 |             |                              |
                    |             |               |                 |             |                              |
                    |-------------|---------------|-----------------|----------- -|------------------------------|
Abstract			:Clear the LU1 and LU2(Local unit1 and Local unit2)Forward
					 and Reverse counts

Allocated Requiremnts:

Design Requirements:

Interfaces
	Calls			:	None

	Called by		:	RLYDE_MGR.C-Start_Relay_DE_Mgr()

Input Variables						Name				Type
	Global			:	None

	Local			:	None

Output Variables					Name				Type
	Global			:	 Relay_DE_Info.LU1_Fwd_Count 	UINT16
 						 Relay_DE_Info.LU2_Fwd_Count	UINT16
						 Relay_DE_Info.LU1_Rev_Count 	UINT16
 						 Relay_DE_Info.LU2_Rev_Count	UINT16

	Local			:	None

Signal Variables

						|---------------------------------|--------------------|------------------------|
						|			Signal name		      | 	Input/Output   | 	 Portallocated		|
						|---------------------------------|--------------------|------------------------|
						|				None			  |			None	   |		None		    |
						|---------------------------------|--------------------|------------------------|

Macros defined		:					Macro 					Value
										None					None
References			:

Derived Requirements:
************************************************************************/
void Clear_Relay_DE_Counts(void)
{
/*clear local counts */
 Relay_DE_Info.LU1_Fwd_Count = 0;
 Relay_DE_Info.LU2_Fwd_Count = 0;
 Relay_DE_Info.LU1_Rev_Count = 0;
 Relay_DE_Info.LU2_Rev_Count = 0;
}
