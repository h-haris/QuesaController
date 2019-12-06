/*  NAME:
        ControllerDB.c

    DESCRIPTION:
        Implementation of Quesa API Controller Core Library.
		
        This source file implements the database and methods of the device
		server to handle every registered controller driver.
	  
    COPYRIGHT:
        Copyright (c) 1999-2005, Quesa Developers. All rights reserved.

        For the current release of Quesa, please see:

            <http://www.quesa.org/>
        
        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions
        are met:
        
            o Redistributions of source code must retain the above copyright
              notice, this list of conditions and the following disclaimer.
        
            o Redistributions in binary form must reproduce the above
              copyright notice, this list of conditions and the following
              disclaimer in the documentation and/or other materials provided
              with the distribution.
        
            o Neither the name of Quesa nor the names of its contributors
              may be used to endorse or promote products derived from this
              software without specific prior written permission.
        
        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
        TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
        PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
        LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
        NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
        SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    ___________________________________________________________________________
*/

//=============================================================================
//      Include files
//-----------------------------------------------------------------------------
#include <CoreFoundation/CoreFoundation.h>

#include "E3Prefix.h"				
#include "ControllerDB.h"
#include "IPCMessageIDs.h"
#include "IPCTracker.h"
#include "IPCPackUnpack.h"
#include "IPCDriver.h"






//=============================================================================
//      Internal constants
//-----------------------------------------------------------------------------
// Internal constants go here





//=============================================================================
//      Internal types
//-----------------------------------------------------------------------------
// Internal types go here

/*
typedef struct TQ3ControllerData {
    char                                        *signature;
    TQ3Uns32                                    valueCount;
    TQ3Uns32                                    channelCount;
    TQ3ChannelGetMethod                         channelGetMethod;
    TQ3ChannelSetMethod                         channelSetMethod;
} TQ3ControllerData;
*/

/*
typedef struct TC3ChannelPrivateData
{
	void 				*data;		//pointer to head of data (arrayadress)
	TQ3Boolean 			isActive;	//proposal
	TQ3Uns32 			size;		//datasize
} TC3ChannelPrivateData;//unused
*/

typedef struct TC3ControllerPrivateData
{
	TQ3ControllerData		publicData;
	TQ3Uns32	 			theButtons;
	TQ3Uns32 				serialNumber;
	CFStringRef				driverPortName;
	CFStringRef				trackerPortName;
	CFStringRef				trackerUUID;
	float					*valuesRef;		//pointer to field of float-values
	TQ3Boolean				isActive;
	TQ3Boolean				isDecommissioned;
	//return reasonable default values if referenced but decommissioned - not fully implemented
	void			 		*nextPrivateData;	//NULL, if no next controller (linked list)
} TC3ControllerPrivateData;



//=============================================================================
//      Internal macros
//-----------------------------------------------------------------------------
// Internal macros go here

/*
Details for implementation of moving the system cursor under MacOS X can be found 
in CGRemoteOperation.h! see: CGPostMouseEvent
*/

//=============================================================================
//      Internal variables
//-----------------------------------------------------------------------------
// Internal variables go here

TQ3Uns32 						controllerListSerialNumber = 0;
TC3ControllerPrivateDataPtr 	controllerListAnchor = NULL;

CFMutableDictionaryRef			controllerStateDict = NULL;

#pragma mark -

//=============================================================================
//      Internal function prototypes
//-----------------------------------------------------------------------------
// Internal function prototypes go here

//=============================================================================
//      Internal functions
//-----------------------------------------------------------------------------
// Internal functions go here



TQ3Boolean
ControllerDB_refinlist(TQ3ControllerRef controllerRef)
{
	TC3ControllerPrivateDataPtr currentObj;
	
	currentObj=controllerListAnchor;
	
	while (currentObj!=NULL)
	{
		if (controllerRef==currentObj) return kQ3True;
		currentObj=(TC3ControllerPrivateDataPtr)currentObj->nextPrivateData;
	}
	return kQ3False;
}

#pragma mark -
//=============================================================================
//      Public functions
//-----------------------------------------------------------------------------
//      ControllerDB_GetListChanged : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetListChanged(TQ3Boolean *listChanged, TQ3Uns32 *serialNumber)
{
 	if (controllerListSerialNumber!=*serialNumber)
	{
		*listChanged=kQ3True;
		*serialNumber=controllerListSerialNumber;
	}
	else
		*listChanged=kQ3False;
	
	return(kQ3Success);
}




//=============================================================================
//      ControllerDB_Next : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_Next(TQ3ControllerRef controllerRef, TQ3ControllerRef *nextControllerRef)
{
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (theController==NULL)
	{
		*nextControllerRef=(TQ3ControllerRef)controllerListAnchor;
		return(kQ3Success);
	}
	else
	{
		if (ControllerDB_refinlist(controllerRef)==kQ3True)
		{
			*nextControllerRef=theController->nextPrivateData;
			return(kQ3Success);
		}
		else return kQ3Failure;
	}
}





//=============================================================================
//      ControllerDB_New : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// used on Server/Driver Side
//-----------------------------------------------------------------------------
TQ3ControllerRef
ControllerDB_New(const TQ3ControllerData *controllerData)
{
	TC3ControllerPrivateDataPtr newCtrl,currentObj;
	TQ3Boolean found;
	size_t ValuesSize;
	
	newCtrl=NULL;
	currentObj = controllerListAnchor;
	
	found = kQ3False;
	
	//Do a search in list, if signature of new controller is already present
	while (currentObj!=NULL)
	{
		if (strcmp(currentObj->publicData.signature,controllerData->signature)==0) 
		{
			found = kQ3True;
			break;
		};
		currentObj = (TC3ControllerPrivateDataPtr)currentObj->nextPrivateData;
	};
	
	if (found==kQ3True)
	{
		/*
		Signature was found. Set newCtrl to found object for later reactivation
		Any other parameters will be ignored!
		*/
		newCtrl=currentObj;
	}
	else
	{
		/*
		Signature wasn't found. Create new private data
		*/
		if ((controllerData->valueCount > kQ3MaxControllerValues ) || (controllerData->channelCount > kQ3MaxControllerChannels ))
			return NULL;
		
		newCtrl=(TC3ControllerPrivateDataPtr)malloc(sizeof(TC3ControllerPrivateData));
		//who clears allocated memory, when there is no running "Device Driver" any more?
						
		if (newCtrl==NULL) return NULL;	//- Return on Error: NULL
		
		//general Init
		//newCtrl->trackerObject=NULL;
		newCtrl->trackerPortName=NULL;	
		newCtrl->trackerUUID=NULL;
		
		newCtrl->valuesRef=NULL;
		
		newCtrl->publicData.valueCount=controllerData->valueCount;
		newCtrl->publicData.channelCount=controllerData->channelCount;
		newCtrl->publicData.channelGetMethod=controllerData->channelGetMethod;
		newCtrl->publicData.channelSetMethod=controllerData->channelSetMethod;
		
		//- allocate array of values
		if (newCtrl->publicData.valueCount>0)
		{
			if (newCtrl->publicData.valueCount > kQ3MaxControllerValues) 
				newCtrl->publicData.valueCount = kQ3MaxControllerValues;
			
			ValuesSize=sizeof(float)*newCtrl->publicData.valueCount;
						
			newCtrl->valuesRef=(float*)malloc(ValuesSize);
			if (newCtrl->valuesRef==NULL)
			{
				free(newCtrl);
				return NULL;
			}
			else
				memset(newCtrl->valuesRef,0x00,ValuesSize);
		}
		else
			newCtrl->valuesRef=NULL;
		
		//- deep copy signature string
		newCtrl->publicData.signature=(char*)malloc(1+strlen(controllerData->signature));
		if (newCtrl->publicData.signature==NULL)
		{
			if (newCtrl->valuesRef!=NULL)
					free(newCtrl->valuesRef);
			free(newCtrl);
			return NULL;
		}
		strcpy(newCtrl->publicData.signature,controllerData->signature);
		
		//insert in List
		newCtrl->nextPrivateData=NULL;
		
		currentObj = controllerListAnchor;
		if (currentObj==NULL)
		{
			//lock
			controllerListAnchor=newCtrl;	//copy-on-write
			//unlock
		}
		else
		{
			while (currentObj->nextPrivateData!=NULL)
			{
				currentObj = (TC3ControllerPrivateDataPtr)currentObj->nextPrivateData;
			}
			currentObj->nextPrivateData=newCtrl;
		}	
	}
	
	newCtrl->theButtons=0;
	newCtrl->serialNumber=1;
	newCtrl->isDecommissioned=kQ3False;

	ControllerDB_SetActivation(newCtrl, kQ3True);
	controllerListSerialNumber++;
	
	return(newCtrl);	// Return on Success: list element
}



//=============================================================================
//      ControllerDB_SetDriverPortName : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// private function
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_SetDriverPortName(TQ3ControllerRef controllerRef,CFStringRef thePortName)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			theController->driverPortName=thePortName;
			status = kQ3Success;
		}
	return(status);
}

//=============================================================================
//      ControllerDB_Decommission : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// used on Server/Driver Side
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_Decommission(TQ3ControllerRef controllerRef)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = ControllerDB_SetActivation(theController,kQ3False);
			theController->isDecommissioned=kQ3True;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_SetActivation : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// QD3D:notification function of associated tracker might get called!			
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_SetActivation(TQ3ControllerRef controllerRef, TQ3Boolean active)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			theController->isActive = active;
			//lock
			controllerListSerialNumber++;		//copy-on-write
			//unlock
			if (theController->trackerUUID!=NULL)
				IPCTracker_callNotification(theController->trackerUUID,
											theController->trackerPortName,
											theController);
			status = kQ3Success;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_GetActivation : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetActivation(TQ3ControllerRef controllerRef, TQ3Boolean *active)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			*active = theController->isActive;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_GetSignature : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetSignature(TQ3ControllerRef controllerRef, char *signature, TQ3Uns32 numChars)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			//src: theController->publicData.signature
			//dst: signature
			//size: numChars
			//but NULL terminated
			strncpy(signature,theController->publicData.signature,numChars-1);
			signature[numChars-1] = '\0';
			status = kQ3Success;
		}
	return(status);
}

TQ3Status
ControllerDB_GetCFSignature(TQ3ControllerRef controllerRef, CFStringRef *signature)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			*signature = CFStringCreateWithCString(	kCFAllocatorDefault,
													theController->publicData.signature, //should be null-terminated
													kCFStringEncodingASCII);
			status = kQ3Success;
		}
	return(status);
}



//=============================================================================
//      ControllerDB_SetChannel : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
//ControllerDB_SetChannel(TQ3ControllerRef controllerRef, TQ3Uns32 channel, const void *data, TQ3Uns32 dataSize)
ControllerDB_SetChannel(TQ3ControllerRef controllerRef, CFMutableDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
			if (theController->publicData.channelSetMethod!=NULL)
			{
				//insert Method into dictionary
				CFDataRef MethodRef = CFDataCreate(	kCFAllocatorDefault, 
													(UInt8*)&theController->publicData.channelSetMethod,
													sizeof(TQ3ChannelSetMethod));
				CFDictionarySetValue(dict,CFSTR(k3MethodRef),MethodRef);
				CFRelease(MethodRef);
				
				status = IPCDriver_Send(m3ControllerDriver_SetChannel,theController->driverPortName,dict,returnDict);
			}	
	return(status);
}





//=============================================================================
//      ControllerDB_GetChannel : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
//ControllerDB_GetChannel(TQ3ControllerRef controllerRef, TQ3Uns32 channel, void *data, TQ3Uns32 *dataSize)
ControllerDB_GetChannel(TQ3ControllerRef controllerRef, CFMutableDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
			if (theController->publicData.channelGetMethod!=NULL)
			{
				//insert Method into dictionary
				CFDataRef MethodRef = CFDataCreate(	kCFAllocatorDefault, 
													(UInt8*)&theController->publicData.channelGetMethod,
													sizeof(TQ3ChannelGetMethod));
				CFDictionarySetValue(dict,CFSTR(k3MethodRef),MethodRef);
				CFRelease(MethodRef);
				
				status = IPCDriver_Send(m3ControllerDriver_GetChannel,theController->driverPortName,dict,returnDict);
			}	
	return(status);
}






//=============================================================================
//      ControllerDB_GetValueCount : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetValueCount(TQ3ControllerRef controllerRef, TQ3Uns32 *valueCount)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			*valueCount=theController->publicData.valueCount;
			status = kQ3Success;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_SetTracker : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
//QD3D: notification function of old and new associated tracker might get called!
//-----------------------------------------------------------------------------
/*
TQ3Status
ControllerDB_SetTracker(TQ3ControllerRef controllerRef, TQ3TrackerObject tracker)
*/
TQ3Status
ControllerDB_SetTracker(TQ3ControllerRef controllerRef, CFStringRef theTrackerUUID, CFStringRef theTrackerPortName)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			
			//notification function of old...
			if (theController->trackerUUID!=NULL)
				IPCTracker_callNotification(theController->trackerUUID,
											theController->trackerPortName,
											theController);
			
			//... and new associated tracker might get called!
			if (theController->trackerPortName!=NULL)
				CFRelease(theController->trackerPortName);
			theController->trackerPortName=theTrackerPortName;
			
			if (theController->trackerUUID!=NULL)
				CFRelease(theController->trackerUUID);
			theController->trackerUUID=theTrackerUUID;
			if (theController->trackerUUID!=NULL)
				IPCTracker_callNotification(theController->trackerUUID,
											theController->trackerPortName,
											theController);
			/*
			else
				assign_to_SystemCursorTracker(controllerRef);
				//very platform dependant! No Moving of system cursor/mouse pointer planned so far!
			*/
			status = kQ3Success;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_HasTracker : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_HasTracker(TQ3ControllerRef controllerRef, TQ3Boolean *hasTracker)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	TQ3Boolean TrackerIsActive;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			if (theController->trackerUUID!=NULL)
			{
				status = IPCTracker_getActivation(	theController->trackerUUID,
													theController->trackerPortName,
													&TrackerIsActive);
				if ((TrackerIsActive==kQ3True)&&(theController->isActive==kQ3True))
					*hasTracker = kQ3True;
				else
					*hasTracker = kQ3False;
			}
			else *hasTracker = kQ3False;
			status = kQ3Success;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_Track2DCursor : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// used on Server/Driver Side
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_Track2DCursor(TQ3ControllerRef controllerRef, TQ3Boolean *track2DCursor)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			if ((theController->trackerUUID!=NULL)||(theController->isActive==kQ3False))
				*track2DCursor=kQ3False;
			else
				*track2DCursor=kQ3True;
			status = kQ3Success;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_Track3DCursor : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// used on Server/Driver Side
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_Track3DCursor(TQ3ControllerRef controllerRef, TQ3Boolean *track3DCursor)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			if ((theController->trackerUUID!=NULL)||(theController->isActive==kQ3False))
				*track3DCursor=kQ3False;
			else
				*track3DCursor=kQ3True;
			status = kQ3Success;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_GetButtons : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetButtons(TQ3ControllerRef controllerRef, TQ3Uns32 *buttons)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			*buttons = theController->theButtons;
			status = kQ3Success;
		}
	return(status);
}





//=============================================================================
//      ControllerDB_SetButtons : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// QD3D:notification function of associated tracker might get called!
// used on Server/Driver Side
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_SetButtons(TQ3ControllerRef controllerRef, TQ3Uns32 buttons)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	TQ3Uns32 buttonMask;	
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			
			if (theController->isActive==kQ3True)
			{
				buttonMask=theController->theButtons^buttons;
				theController->theButtons = buttons;
				
				if (theController->trackerUUID!=NULL)
				{
					status = IPCTracker_changeButtons(	theController->trackerUUID,
														theController->trackerPortName,											
														theController,//Controller is used by Tracker Notification function
														buttons,
														buttonMask);
				}
				/*
				else
					//modify System Cursor Tracker
					//very platform dependant! No moving/modifying of system cursor/mouse pointer planned so far!
				*/
			}
		}
	return(status);
}





//=============================================================================
//      ControllerDB_GetTrackerPosition : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetTrackerPosition(TQ3ControllerRef controllerRef, TQ3Point3D *position)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			if ((theController->isActive==kQ3True)&&(theController->trackerUUID!=NULL))
				status = IPCTracker_getPosition(	theController->trackerUUID,
													theController->trackerPortName,
													position);
			else
			{
				//return position of system cursor tracker - not yet implemented!
				position->x=position->y=position->z=0.0;	//not the best style
			}
		}
	return(status);
}





//=============================================================================
//      ControllerDB_SetTrackerPosition : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// QD3D:notification function of associated tracker might get called!
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_SetTrackerPosition(TQ3ControllerRef controllerRef, const TQ3Point3D *position)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			if (theController->isActive==kQ3True)
			{
				if (theController->trackerUUID!=NULL)
				{
					status = IPCTracker_setPosition(	theController->trackerUUID,
														theController->trackerPortName,
														theController,//Controller is used by Tracker Notification function
														position);
				}
				/*
				}
				else
					//move System Cursor Tracker
					//very platform dependant! No moving/modifying of system cursor/mouse pointer planned so far!
				*/
			}
		}
	return(status);
}





//=============================================================================
//      ControllerDB_MoveTrackerPosition : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// QD3D:notification function of associated tracker might get called!
// used on Server/Driver Side
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_MoveTrackerPosition(TQ3ControllerRef controllerRef, const TQ3Vector3D *delta)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			if (theController->isActive==kQ3True)
			{
				if (theController->trackerUUID!=NULL)
				{
					status = IPCTracker_movePosition(	theController->trackerUUID,
														theController->trackerPortName,
														theController,//Controller is used by Tracker Notification function
														delta);
				}
				/*
				else
					//move System Cursor Tracker
					//very platform dependant! No moving/modifying of system cursor/mouse pointer planned so far!
				*/
			}
		}
	return(status);
}





//=============================================================================
//      ControllerDB_GetTrackerOrientation : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetTrackerOrientation(TQ3ControllerRef controllerRef, TQ3Quaternion *orientation)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			if ((theController->isActive==kQ3True)&&(theController->trackerUUID!=NULL))
				status = IPCTracker_getOrientation(	theController->trackerUUID,
													theController->trackerPortName,
													orientation);
			else
			{
				//return position of system cursor tracker - not yet implemented!
				//here: set orientation to indentity quaternion
				orientation->w=1.0;
				orientation->x=orientation->y=orientation->z=0.0;	//not the best style
			}
		}
	return(status);
}





//=============================================================================
//      ControllerDB_SetTrackerOrientation : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// QD3D:notification function of associated tracker might get called!
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_SetTrackerOrientation(TQ3ControllerRef controllerRef, const TQ3Quaternion *orientation)
{		
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			if (theController->isActive==kQ3True)
			{
				if (theController->trackerUUID!=NULL)
				{	
					status = IPCTracker_setOrientation(	theController->trackerUUID,
														theController->trackerPortName,
														theController,//Controller is used by Tracker Notification function
														orientation);
				}
				/*
				else
					//set System Cursor Tracker
					//very platform dependant! No moving/modifying of system cursor/mouse pointer planned so far!
				*/
			}
		}
	return(status);
}





//=============================================================================
//      ControllerDB_MoveTrackerOrientation : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// QD3D:notification function of associated tracker might get called!
// used on Server/Driver Side
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_MoveTrackerOrientation(TQ3ControllerRef controllerRef, const TQ3Quaternion *delta)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			if (theController->isActive==kQ3True)
			{
				if (theController->trackerUUID!=NULL)
				{
					status = IPCTracker_moveOrientation(	theController->trackerUUID,
															theController->trackerPortName,
															theController,//Controller is used by Tracker Notification function
															delta);
				}
				/*
				else
					//move System Cursor Tracker
					//very platform dependant! No moving/modifying of system cursor/mouse pointer planned so far!
				*/
			}
		}
	return(status);
}





//=============================================================================
//      ControllerDB_GetValues : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetValues(TQ3ControllerRef controllerRef, TQ3Uns32 valueCount, float *values, TQ3Boolean *changed, TQ3Uns32 *serialNumber)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			if (theController->isActive==kQ3True)
			{
				*changed=kQ3False;
				// copy
				if (*serialNumber!=theController->serialNumber)
				{
					if (theController->publicData.valueCount>0)
					{
						TQ3Uns32	maxCount,index;
						if (theController->publicData.valueCount > valueCount)
							maxCount=valueCount;
						else
							maxCount=theController->publicData.valueCount;
						
						for (index=0; index<maxCount; index++)
							values[index]=theController->valuesRef[index];
					}
					*changed=kQ3True;
					if (serialNumber!=NULL)
						*serialNumber=theController->serialNumber;
				}
			}
			else
			if (*serialNumber!=theController->serialNumber) 
				*serialNumber=theController->serialNumber;
		}
	return(status);
}

//=============================================================================
//      ControllerDB_GetValuesRaw : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_GetValuesRaw(TQ3ControllerRef controllerRef, TQ3Uns32 *valueCount, float *values, TQ3Uns32 *serialNumber)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			if (theController->isActive==kQ3True)
			{
				if (theController->publicData.valueCount>0)
				{
					TQ3Uns32	maxCount,index;
					if (theController->publicData.valueCount > *valueCount)
						maxCount=*valueCount;
					else
						maxCount=theController->publicData.valueCount;
					
					*valueCount=maxCount;
					
					for (index=0; index<maxCount; index++)
						values[index]=theController->valuesRef[index];
				}
			}
			*serialNumber=theController->serialNumber;	
		}
	return(status);
}



//=============================================================================
//      ControllerDB_SetValues : One-line description of the method.
//-----------------------------------------------------------------------------
//		Note : More detailed comments can be placed here if required.
//
// Deviation from Apple-doc: Controller serialNumber is incremented!
// used on Server/Driver Side
//-----------------------------------------------------------------------------
TQ3Status
ControllerDB_SetValues(TQ3ControllerRef controllerRef, const float *values, TQ3Uns32 valueCount)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
			if (theController->publicData.valueCount>0)
			{
				TQ3Uns32	maxCount,index;
				if (theController->publicData.valueCount > valueCount)
					maxCount=valueCount;
				else
					maxCount=theController->publicData.valueCount;
				
				for (index=0; index<maxCount;index++)
					theController->valuesRef[index]=values[index];
				
				theController->serialNumber++;	//This fits better to functionality of ControllerDB_GetValues
				status = kQ3Success;
			}
	return(status);
};

#pragma mark -

TQ3Status
ControllerDB_StateNew(TQ3ControllerRef controllerRef, CFStringRef *CtrlStateKey)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			//Do what to do:
			//-Create UUID
			CFUUIDRef StateUUID = CFUUIDCreate(kCFAllocatorDefault);
			
			//-Create and return UUID-key
			*CtrlStateKey = CFUUIDCreateString(kCFAllocatorDefault,StateUUID);
			
			//-Create Dictionary if not available
			if (controllerStateDict==NULL)
				controllerStateDict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
																	&kCFTypeDictionaryKeyCallBacks,
																	&kCFTypeDictionaryValueCallBacks);
			
			//-clean up
			if (StateUUID)
				CFRelease(StateUUID);
		}
	return(status);
}

TQ3Status
ControllerDB_StateDelete(TQ3ControllerRef controllerRef, CFStringRef CtrlStateKey)
{
	TQ3Status status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if (theController!=NULL)
		{
			status = kQ3Success;
			//Do what to do:
			if (controllerStateDict)
			{
				//-Release Array at CtrlStateKey
				CFDictionaryRemoveValue (controllerStateDict,CtrlStateKey);
			}
		}
	return(status);
}

TQ3Status
ControllerDB_StateSaveAndReset(TQ3ControllerRef controllerRef, CFStringRef CtrlStateKey)
{
	TQ3Status					status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	UInt8						channelData[kQ3ControllerSetChannelMaxDataSize];
	UInt32						channelDataSize;
	
	CFMutableDictionaryRef		dict,returnDict;
	
	Boolean result;
	
	//create dictionaries
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
										
	returnDict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);									
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if ((theController!=NULL)&&(dict!=NULL))
		{
			//Do what to do:
			//-pack
			//--Set method
			result = IPCPutBytes(dict, CFSTR(k3SetMethodRef), sizeof(TQ3ChannelSetMethod), &theController->publicData.channelSetMethod);
			//--Get method
			result = IPCPutBytes(dict, CFSTR(k3GetMethodRef), sizeof(TQ3ChannelGetMethod), &theController->publicData.channelGetMethod);
			//--controllerRef
			result = IPCPutControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
			//--channelCount
			result = IPCPutTQ3Uns32(dict, CFSTR(k3ChannelCount), &theController->publicData.channelCount);	
			
			//try sending
			status = IPCDriver_Send(m3ControllerDriver_StateSaveAndReset,theController->driverPortName,dict,returnDict);
			if (status!=kQ3Failure)
			{
				//-Methods return dictionary
				CFDictionaryRef methodsReturnRef = (CFDictionaryRef)CFDictionaryGetValue(returnDict, CFSTR(k3MethodsReturn));
				//unpack and replace
				//-Array with channels
				CFArrayRef channelArrayRef = (CFArrayRef)CFDictionaryGetValue(methodsReturnRef, CFSTR(k3ChannelsData));
				//CFRetain(channelArrayRef);
				CFDictionarySetValue(controllerStateDict,CtrlStateKey,channelArrayRef);  
			}
			//Do clean up
			CFRelease(dict);
		
			if (returnDict)
				CFRelease(returnDict);
		}
		
	return(status);
}

TQ3Status
ControllerDB_StateRestore(TQ3ControllerRef controllerRef, CFStringRef CtrlStateKey)
{
	TQ3Status					status = kQ3Failure;
	TC3ControllerPrivateDataPtr theController = (TC3ControllerPrivateDataPtr)controllerRef;
	UInt8						channelData[kQ3ControllerSetChannelMaxDataSize];
	UInt32						channelDataSize;
	
	CFMutableDictionaryRef		dict,returnDict;
	
	Boolean result;
	
	//create dictionaries
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
										
	returnDict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	
	if (ControllerDB_refinlist(controllerRef)==kQ3True)
		if ((theController!=NULL)&&(dict!=NULL))
		{
			//Do what to do:
			//-pack
			//--Set method
			result = IPCPutBytes(dict, CFSTR(k3SetMethodRef), sizeof(TQ3ChannelSetMethod), &theController->publicData.channelSetMethod);
			//--controllerRef
			result = IPCPutControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
			//--channelCount
			result = IPCPutTQ3Uns32(dict, CFSTR(k3ChannelCount), &theController->publicData.channelCount);	
			//--Array with channels
			CFArrayRef channelArrayRef = (CFArrayRef)CFDictionaryGetValue(controllerStateDict,CtrlStateKey);
			CFDictionarySetValue(dict, CFSTR(k3ChannelsData),channelArrayRef); 
			
			//try sending
			status = IPCDriver_Send(m3ControllerDriver_StateRestore,theController->driverPortName,dict,returnDict);
			
			//Do clean up
			CFRelease(dict);
		
			if (returnDict)
				CFRelease(returnDict);
		}
		
	return(status);
}
