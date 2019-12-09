 /*  NAME:
        IPCController.c

    DESCRIPTION:
        Implementation of Quesa API Controller Core Library.
		
        This source file implements the local message port and methods of the 
		device server to receive messages from the driver and to client.

		It dispatches the messages, unpacks received data, calls the database
		methods and packs data to be returned to the caller.
	  
    COPYRIGHT:
        Copyright (c) 1999-2019, Quesa Developers. All rights reserved.

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


#include <CoreFoundation/CoreFoundation.h>

#include "IPCController.h"

#include "IPCPackUnpack.h"
#include "IPCMessageIDs.h"
#include "ControllerDB.h"

TQ3Status	IpcController_GetListChanged(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 	status;	//resulting status after calling controller database
	TQ3Boolean 	listChanged;
	TQ3Uns32	serialNumber;
	
	Boolean 	result;
	
	//Get Parameters from dict
	//ListChanged
	result = IPCGetTQ3Boolean(dict, CFSTR(k3ListChanged), &listChanged);
	
	//serialNumber
	result = IPCGetTQ3Uns32(dict, CFSTR(k3SerNum), &serialNumber);
	
	//-Do call
	status = ControllerDB_GetListChanged(&listChanged, &serialNumber);
	
	//Put Results into returnDict
	//ListChanged
	result = IPCPutTQ3Boolean(returnDict, CFSTR(k3ListChanged), &listChanged);
	
	//serialNumber
	result = IPCPutTQ3Uns32(returnDict, CFSTR(k3SerNum), &serialNumber);
		
	return(status);
};


TQ3Status	IpcController_Next(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status status;	//resulting status after calling controller database
	TQ3ControllerRef controllerRef;
	TQ3ControllerRef nextControllerRef;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//-Do call
	status = ControllerDB_Next(controllerRef, &nextControllerRef);
	
	//Put Results into returnDict
	//nextControllerRef
	IPCPutControllerRef(returnDict, CFSTR(k3CtrlRef), &nextControllerRef);
		
	return(status);
};//done


TQ3Status	IpcController_New(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3ControllerRef 	controllerRef;
	TQ3ControllerData	controllerData;
	
	int 				numChars = 256;
	Boolean 			result;
	
	//Get structure from dict
	//controllerData
	result = IPCGetBytes(dict, CFSTR(k3CtrlData), sizeof(TQ3ControllerData), &controllerData);
								
	//Get seperate signature
	controllerData.signature = (char*)malloc(numChars);
	CFStringRef signatureRef = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(k3Signature));
	result = CFStringGetCString(signatureRef,
								controllerData.signature,
								numChars,
								kCFStringEncodingASCII);
	
	//DriverPortName														
	CFStringRef DriverPortNameRef = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(k3DriverPortName));//check for NULL needed!!!
	if (DriverPortNameRef)
		CFRetain(DriverPortNameRef);
	
	//-Do call
	controllerRef = ControllerDB_New(&controllerData);
	
	//...and set driver port name
	ControllerDB_SetDriverPortName(controllerRef, DriverPortNameRef);
	
	//Put Results into returnDict
	//controllerRef
	IPCPutControllerRef(returnDict, CFSTR(k3CtrlRef), &controllerRef);
	
	return(kQ3Success);
};//signature?


TQ3Status	IpcController_Decommission(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;

	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//-Do call
	status = ControllerDB_Decommission(controllerRef);
	
	//No Results for returnDict
	return(status);
};//done


TQ3Status	IpcController_SetActivation(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Boolean 			active;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
							
	//active
	IPCGetTQ3Boolean(dict, CFSTR(k3Active), &active);
	
	//-Do call
	status = ControllerDB_SetActivation(controllerRef, active);
		
	//No Results for returnDict
	return(status);
};//done


TQ3Status	IpcController_GetActivation(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Boolean 			active;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
							
	//-Do call
	status = ControllerDB_GetActivation(controllerRef, &active);
	
	//Put Results into returnDict
	//active
	IPCPutTQ3Boolean(returnDict, CFSTR(k3Active), &active);
					
	return(status);
};//done


TQ3Status	IpcController_GetSignature(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	CFStringRef			signature;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef), &controllerRef);
							
	//-Do call
	status = ControllerDB_GetCFSignature(controllerRef, &signature);
	
	//Put Results into returnDict
	//signature
	CFDictionarySetValue(returnDict, CFSTR(k3Signature), signature);
	CFRelease(signature);
				
	return(status);
};//done

TQ3Status	IpcController_SetChannel(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 				status;	//resulting status after calling controller database
	TQ3ControllerRef 		controllerRef;
	CFMutableDictionaryRef	workDict;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	workDict = CFDictionaryCreateMutableCopy (kCFAllocatorDefault, 0, dict);
	
	//-Do call
	status = ControllerDB_SetChannel(controllerRef, workDict, returnDict);

	//Put Results into returnDict
					
	return(status);
};//done; needs documentation

TQ3Status	IpcController_GetChannel(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 				status;	//resulting status after calling controller database
	TQ3ControllerRef 		controllerRef;
	CFMutableDictionaryRef	workDict;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
							
	workDict = CFDictionaryCreateMutableCopy (kCFAllocatorDefault,0,dict);
	
	//-Do call
	status = ControllerDB_GetChannel(controllerRef,workDict,returnDict);

	//Put Results into returnDict
					
	return(status);
};//done; needs documentation

TQ3Status	IpcController_GetValueCount(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Uns32 			valueCount;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
							
	//-Do call
	status = ControllerDB_GetValueCount(controllerRef,&valueCount);
	
	//Put Results into returnDict
	//valueCount
	IPCPutTQ3Uns32(returnDict, CFSTR(k3ValueCnt), &valueCount);
					
	return(status);
};//done


TQ3Status	IpcController_SetTracker(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	CFStringRef			trackerPortName;	
	CFStringRef			trackerUUID;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
					
	//trackerPortName
	trackerPortName = (CFStringRef)CFDictionaryGetValue(dict,CFSTR(k3TrackerPortName));
	CFRetain(trackerPortName);	//trackerPortName is needed after releasing dict
	
	//trackerUUID - may be NULL, if key wasn't found in the dictionary				
	trackerUUID = (CFStringRef)CFDictionaryGetValue(dict,CFSTR(k3TrackerUUID));
	if (trackerUUID!=NULL)
		CFRetain(trackerUUID);		//trackerUUID is needed after releasing dict
							
	//-Do call
	status = ControllerDB_SetTracker(controllerRef,trackerUUID,trackerPortName);
	
	//No Results for returnDict
	return(status);
};//special; needs documentation


TQ3Status	IpcController_HasTracker(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Boolean 			hasTracker;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
							
	//-Do call
	status = ControllerDB_HasTracker(controllerRef,&hasTracker);
	
	//Put Results into returnDict
	//hasTracker
	IPCPutTQ3Boolean(returnDict, CFSTR(k3hasTracker), &hasTracker);	
				
	return(status);
};//done


TQ3Status	IpcController_Track2DCursor(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Boolean 			track2DCursor;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
							
	//-Do call
	status = ControllerDB_Track2DCursor(controllerRef,&track2DCursor);
	
	//Put Results into returnDict
	//track2DCursor
	IPCPutTQ3Boolean(returnDict, CFSTR(k3Track2DCrsr), &track2DCursor);	
				
	return(status);
};//done


TQ3Status	IpcController_Track3DCursor(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Boolean 			track3DCursor;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
							
	//-Do call
	status = ControllerDB_Track3DCursor(controllerRef,&track3DCursor);
	
	//Put Results into returnDict
	//track3DCursor
	IPCPutTQ3Boolean(returnDict, CFSTR(k3Track3DCrsr), &track3DCursor);	
				
	return(status);
};//done


TQ3Status	IpcController_GetButtons(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Uns32 			buttons;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
							
	//-Do call
	status = ControllerDB_GetButtons(controllerRef,&buttons);
	
	//Put Results into returnDict
	//buttons
	IPCPutTQ3Uns32(returnDict, CFSTR(k3Buttons), &buttons);
						
	return(status);
};//done


TQ3Status	IpcController_SetButtons(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Uns32 			buttons;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
							
	//buttons
	IPCGetTQ3Uns32(dict, CFSTR(k3Buttons), &buttons);
														
	//-Do call
	status = ControllerDB_SetButtons(controllerRef,buttons);
	
	return(status);
};//done


TQ3Status	IpcController_GetTrackerPosition(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Point3D 			position;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef), &controllerRef);
							
	//-Do call
	status = ControllerDB_GetTrackerPosition(controllerRef, &position);
	
	//Put Results into returnDict
	//position
	IPCPutTQ3Point3D(returnDict, CFSTR(k3Position), &position);
		
	return(status);
};//done


TQ3Status	IpcController_SetTrackerPosition(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Point3D 			position;
	
	Boolean 			result;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
	
	//position
	result = IPCGetTQ3Point3D(dict, CFSTR(k3Position), &position);
	
	//-Do call
	if (result==true)
		status = ControllerDB_SetTrackerPosition(controllerRef,&position);
	
	return(status);
};//done


TQ3Status	IpcController_MoveTrackerPosition(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Vector3D 		delta;
	
	Boolean 			result;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
	
	//delta
	result = IPCGetTQ3Vector3D(dict, CFSTR(k3DeltaPos), &delta);
	
	//-Do call
	if (result==true)
		status = ControllerDB_MoveTrackerPosition(controllerRef,&delta);
	
	return(status);
};//done


TQ3Status	IpcController_GetTrackerOrientation(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Quaternion 		orientation;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
							
	//-Do call
	status = ControllerDB_GetTrackerOrientation(controllerRef,&orientation);
	
	//Put Results into returnDict
	//orientation
	IPCPutTQ3Quaternion(returnDict, CFSTR(k3Orient), &orientation);
		
	return(status);
};//done


TQ3Status	IpcController_SetTrackerOrientation(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Quaternion 		orientation;
	
	Boolean 			result;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//orientation
	result = IPCGetTQ3Quaternion(dict, CFSTR(k3Orient), &orientation);
	
	//-Do call
	if (result==true)
		status = ControllerDB_SetTrackerOrientation(controllerRef,&orientation);
	
	return(status);
};//done


TQ3Status	IpcController_MoveTrackerOrientation(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Quaternion 		delta;
	
	Boolean 			result;
	
	//Get Parameters from dict
	//controllerRef 
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//delta
	result = IPCGetTQ3Quaternion(dict, CFSTR(k3DeltaOrient), &delta);
	
	//-Do call
	if (result==true)
		status = ControllerDB_MoveTrackerOrientation(controllerRef,&delta);
	
	return(status);
};//done


TQ3Status	IpcController_GetValues(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Uns32 			valueCount;
	float				values[kQ3MaxControllerValues];
	TQ3Boolean 			privActive;
	TQ3Uns32 			serialNumber;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//valueCount
	IPCGetTQ3Uns32(dict, CFSTR(k3ValueCount), &valueCount);
								
	//-Do calls
	//--public
	//--ControllerDB_GetValuesRaw: valueCount: r/w; values: w; serialNumber: w
	status = ControllerDB_GetValuesRaw(controllerRef,&valueCount,values,&serialNumber);
	//--private; helper
	status = ControllerDB_GetActivation(controllerRef,&privActive);
	
	//Put Results into returnDict
	//active
	IPCPutTQ3Boolean(returnDict, CFSTR(k3Active), &privActive);
	
	//privValueCount
	IPCPutTQ3Uns32(returnDict, CFSTR(k3PrivValueCount), &valueCount);
	
	//serialNumber
	IPCPutTQ3Uns32(returnDict, CFSTR(k3SerNum), &serialNumber);
	
	//values
	if (valueCount>0)// valueCount was checked for sanity by ControllerDB_GetValuesRaw!
	{
		TQ3Uns32	index;
		
		CFNumberRef *valuesOutput = (CFNumberRef*)malloc(valueCount*sizeof(CFNumberRef));
		for (index=0; index<valueCount; index++)
			valuesOutput[index]= CFNumberCreate(kCFAllocatorDefault,kCFNumberFloatType,&values[index]);
		CFArrayRef valuesRef = CFArrayCreate(kCFAllocatorDefault,(const void**)valuesOutput,valueCount,&kCFTypeArrayCallBacks);
		for (index=0; index<valueCount; index++)
			CFRelease(valuesOutput[index]);
		free(valuesOutput);
		CFDictionarySetValue(returnDict,CFSTR(k3Values),valuesRef);
		CFRelease(valuesRef);
	}
				
	return(status);
};//done


TQ3Status	IpcController_SetValues(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	TQ3Uns32 			valueCount;
	float 				*values;
	
	Boolean 			result;
	int					index;
	
	//Get Parameters from dict
	//controllerRef
	IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//valueCount
	IPCGetTQ3Uns32(dict, CFSTR(k3ValueCount), &valueCount);
	
	//values
	values = (float*)malloc(valueCount * sizeof(float));
	
	CFArrayRef valAr = (CFArrayRef)CFDictionaryGetValue(dict,CFSTR(k3Values));
	if (valAr!=NULL)
	{
		for (index=0; index<valueCount; index++)
			result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(valAr,index),
										kCFNumberFloatType,
										&values[index]);
		//Do call
		status = ControllerDB_SetValues(controllerRef,values,valueCount);
	}
	
	free(values);
	
	return(status);
};//done


#pragma mark -

TQ3Status	IpcControllerState_New(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	CFStringRef			ctrlStateUUIDString;
	
	Boolean 			result;
	
	//Get Parameters from dict
	//controllerRef
	result =  IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//-Do call
	if (result==true)
	{
		ControllerDB_StateNew(controllerRef, &ctrlStateUUIDString);
	}
	
	//Put Results into dictionary
	//-ctrlStateUUIDString 
	CFDictionarySetValue(returnDict, CFSTR(k3CtrlStateUUID), ctrlStateUUIDString);
	
    status = kQ3Success;
	return(status);
};//done

TQ3Status	IpcControllerState_Delete(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	CFStringRef			ctrlStateUUIDString;
	
	Boolean 			result;
	
	//Get Parameters from dict
	//-controllerRef
	result = IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//-ctrlStateUUID - may be NULL, if key wasn't found in the dictionary				
	ctrlStateUUIDString = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(k3CtrlStateUUID));//bug?
	
	//-Do call
	if (result==true)
	{
		ControllerDB_StateDelete(controllerRef, ctrlStateUUIDString);
	}
	
	status = kQ3Success;
    return(status);
};//done

TQ3Status	IpcControllerState_SaveAndReset(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	CFStringRef			ctrlStateUUIDString;
	
	Boolean 			result;
	
	//Get Parameters from dict
	//-controllerRef
	result = IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//-ctrlStateUUID - may be NULL, if key wasn't found in the dictionary				
	ctrlStateUUIDString = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(k3CtrlStateUUID));//bug?
	
	//-Do call
	if (result==true)
	{
		status = ControllerDB_StateSaveAndReset(controllerRef, ctrlStateUUIDString);
	}
	
	return(status);
};//done

TQ3Status	IpcControllerState_Restore(CFDictionaryRef dict, CFMutableDictionaryRef returnDict)
{
	//Controller Parameter
	TQ3Status 			status = kQ3Failure;	//resulting status after calling controller database
	TQ3ControllerRef 	controllerRef;
	CFStringRef			ctrlStateUUIDString;
	
	Boolean 			result;
	
	//Get Parameters from dict
	//-controllerRef
	result = IPCGetControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
	
	//-ctrlStateUUID - may be NULL, if key wasn't found in the dictionary				
	ctrlStateUUIDString = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(k3CtrlStateUUID));//bug?
	
	//-Do call
	if (result==true)
	{
		status = ControllerDB_StateRestore(controllerRef, ctrlStateUUIDString);
	}
	
	return(status);
};//done

#pragma mark -

CFDataRef IPCControllerDispatcher ( CFMessagePortRef local, SInt32 msgid, CFDataRef data, void *info)
{
	TQ3Status 				status;
	
	CFDataRef 				returnData;
	
	CFDictionaryRef			dict;
	CFMutableDictionaryRef	returnDict;
	
	CFStringRef				propertyListError;
	
	dict=(CFDictionaryRef)CFPropertyListCreateFromXMLData(	kCFAllocatorDefault, 
															data, 
															kCFPropertyListImmutable, 
															&propertyListError);
	if (propertyListError)
	{
		//Do error handling
		
		CFRelease(propertyListError);
	}
	//nothing else to be released! dict will be in used over function; the data parameter will be deallocated at exit!
	
	//create dictionary for return parameters
	returnDict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
											&kCFTypeDictionaryKeyCallBacks,
											&kCFTypeDictionaryValueCallBacks);
	
	if (returnDict)
	{
		//Dispatch msgid to local functions
		switch(msgid)
		{
			case m3Controller_GetListChanged:
				status = IpcController_GetListChanged(dict,returnDict);
				break;
			case m3Controller_Next:
				status = IpcController_Next(dict,returnDict);
				break;
			case m3Controller_New:
				status = IpcController_New(dict,returnDict);
				break;
			case m3Controller_Decommission:
				status = IpcController_Decommission(dict,returnDict);
				break;
			case m3Controller_SetActivation:
				status = IpcController_SetActivation(dict,returnDict);
				break;
			case m3Controller_GetActivation:
				status = IpcController_GetActivation(dict,returnDict);
				break;
			case m3Controller_GetSignature:
				status = IpcController_GetSignature(dict,returnDict);
				break;
			case m3Controller_SetChannel:
				status = IpcController_SetChannel(dict,returnDict);
				break;
			case m3Controller_GetChannel:
				status = IpcController_GetChannel(dict,returnDict);
				break;
			case m3Controller_GetValueCount:
				status = IpcController_GetValueCount(dict,returnDict);
				break;
			case m3Controller_SetTracker:
				status = IpcController_SetTracker(dict,returnDict);
				break;
			case m3Controller_HasTracker:
				status = IpcController_HasTracker(dict,returnDict);
				break;
			case m3Controller_Track2DCursor:
				status = IpcController_Track2DCursor(dict,returnDict);
				break;
			case m3Controller_Track3DCursor:
				status = IpcController_Track3DCursor(dict,returnDict);
				break;
			case m3Controller_GetButtons:
				status = IpcController_GetButtons(dict,returnDict);
				break;
			case m3Controller_SetButtons:
				status = IpcController_SetButtons(dict,returnDict);
				break;
			case m3Controller_GetTrackerPosition:
				status = IpcController_GetTrackerPosition(dict,returnDict);
				break;
			case m3Controller_SetTrackerPosition:
				status = IpcController_SetTrackerPosition(dict,returnDict);
				break;
			case m3Controller_MoveTrackerPosition:
				status = IpcController_MoveTrackerPosition(dict,returnDict);
				break;
			case m3Controller_GetTrackerOrientation:
				status = IpcController_GetTrackerOrientation(dict,returnDict);
				break;
			case m3Controller_SetTrackerOrientation:
				status = IpcController_SetTrackerOrientation(dict,returnDict);
				break;
			case m3Controller_MoveTrackerOrientation:
				status = IpcController_MoveTrackerOrientation(dict,returnDict);
				break;
			case m3Controller_GetValues:
				status = IpcController_GetValues(dict,returnDict);
				break;
			case m3Controller_SetValues:
				status = IpcController_SetValues(dict,returnDict);
				break;
			case m3ControllerState_New:
				status = IpcControllerState_New(dict,returnDict);
				break;
			case m3ControllerState_Delete:
				status = IpcControllerState_Delete(dict,returnDict);
				break;
			case m3ControllerState_SaveAndReset:
				status = IpcControllerState_SaveAndReset(dict,returnDict);
				break;
			case m3ControllerState_Restore:
				status = IpcControllerState_Restore(dict,returnDict);
				break;	
			default:
			break;
		}
	}
	if (dict)
		CFRelease(dict);
	
	//status
	IPCPutTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		
	//returnDict to CFDataRef
	returnData= CFPropertyListCreateXMLData(kCFAllocatorDefault,returnDict);
	
	if (returnDict)
		CFRelease(returnDict);
	
	return returnData;
};

