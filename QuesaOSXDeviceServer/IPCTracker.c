/*  NAME:
        IPCTracker.c

    DESCRIPTION:
        Implementation of Quesa API Controller Core Library.
		
        This source file implements method prototypes to send messages to trackers which
		are associated to controllers.
		
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


#include <CoreFoundation/CoreFoundation.h>

#include "IPCTracker.h"

#include "IPCMessageIDs.h"
#include "IPCPackUnpack.h"

TQ3Status IPCTracker_Send( 	SInt32 msgid, 
							CFStringRef theTrackerUUID, 
							CFStringRef theTrackerPortName, 
							CFMutableDictionaryRef dict, 
							CFMutableDictionaryRef *returnDict)
{
	TQ3Status status = kQ3Failure;
	
	CFDataRef 	data,returnData;
	CFStringRef	propertyListError;
	
	*returnDict=NULL;
	
	//insert TrackerUUID into dict
	CFDictionarySetValue(dict,CFSTR(k3TrackerUUID),theTrackerUUID); 
	
	data= CFPropertyListCreateXMLData(kCFAllocatorDefault,dict);
	//what to release?
	
	CFMessagePortRef TrackerPort=CFMessagePortCreateRemote(kCFAllocatorDefault, theTrackerPortName);
	if (TrackerPort!=NULL)
	{
		SInt32 ReqRes = CFMessagePortSendRequest(	TrackerPort, msgid, data, 
									10, 10, kCFRunLoopDefaultMode,
									&returnData);
		
		if (ReqRes == kCFMessagePortSuccess)
		{
			if (returnData)
			{
				*returnDict=(CFMutableDictionaryRef)CFPropertyListCreateFromXMLData(kCFAllocatorDefault, 
																					returnData, 
																					kCFPropertyListImmutable, 
																					&propertyListError);
				//was there an error? see propertyListError
				if (propertyListError)
					status=kQ3Failure;
				else	
					status=kQ3Success;
			}
			
			if (propertyListError)
				CFRelease(propertyListError);
			if (returnData)
				CFRelease(returnData);
		}	
	}
	if (TrackerPort)
		CFRelease(TrackerPort);
	if (data)
		CFRelease(data);
	
	return status;
};


TQ3Status IPCTracker_callNotification	(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//Put parameters into dict
		//controllerRef
		result = IPCPutControllerRef(dict,CFSTR(k3CtrlRef),&controllerRef);
				
		//try sending
		status = IPCTracker_Send(m3Tracker_CallNotification,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}


TQ3Status IPCTracker_changeButtons		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											TQ3Uns32 buttons, 
											TQ3Uns32 buttonMask)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//Put parameters into dict
		//controllerRef
		result = IPCPutControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
		
		//buttons
		result = IPCPutTQ3Uns32(dict, CFSTR(k3Buttons), &buttons);
				
		//buttonMask
		result = IPCPutTQ3Uns32(dict, CFSTR(k3ButtonMask), &buttonMask);
				
		//try sending
		status = IPCTracker_Send(m3Tracker_ChangeButtons,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}

TQ3Status IPCTracker_getActivation		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3Boolean *active)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//try sending
		status = IPCTracker_Send(m3Tracker_GetActivation,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get values from returnDict
			//active
			result = IPCGetTQ3Boolean(returnDict, CFSTR(k3Active), active);
			
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}

TQ3Status IPCTracker_getPosition		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3Point3D *position)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//try sending
		status = IPCTracker_Send(m3Tracker_GetPosition,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get values from returnDict
			//position
			result = IPCGetTQ3Point3D(returnDict, CFSTR(k3Position), position);
						
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}



TQ3Status IPCTracker_setPosition		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											const TQ3Point3D *position)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//Put parameters into dict
		//controllerRef
		result = IPCPutControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
		
		//position
		result = IPCPutTQ3Point3D(dict, CFSTR(k3Position), position);
		
		//try sending
		status = IPCTracker_Send(m3Tracker_SetPosition,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}


TQ3Status IPCTracker_movePosition		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											const TQ3Vector3D *delta)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//Put parameters into dict
		//controllerRef
		result = IPCPutControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
		
		//delta
		result = IPCPutTQ3Vector3D(dict, CFSTR(k3DeltaPos), delta);
		
		//try sending
		status = IPCTracker_Send(m3Tracker_MovePosition,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}

TQ3Status IPCTracker_getOrientation		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3Quaternion *orientation)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//try sending
		status = IPCTracker_Send(m3Tracker_GetOrientation,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get values from returnDict
			//orientation
			result = IPCGetTQ3Quaternion(returnDict, CFSTR(k3Orient), orientation);
						
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}

TQ3Status IPCTracker_setOrientation		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											const TQ3Quaternion *orientation)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//Put parameters into dict
		//controllerRef
		result = IPCPutControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
		
		//orientation
		result = IPCPutTQ3Quaternion(dict, CFSTR(k3Orient), orientation);
				
		//try sending
		status = IPCTracker_Send(m3Tracker_SetOrientation,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}


TQ3Status IPCTracker_moveOrientation	(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											const TQ3Quaternion *delta)
{
	TQ3Status status = kQ3Failure;
	CFMutableDictionaryRef dict,returnDict;
	
	Boolean result;
	
	//create dictionary
	dict = CFDictionaryCreateMutable(	kCFAllocatorDefault,0,
										&kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	if (dict)
	{
		//Put parameters into dict
		//controllerRef
		result = IPCPutControllerRef(dict, CFSTR(k3CtrlRef), &controllerRef);
		
		//orientation
		result = IPCPutTQ3Quaternion(dict, CFSTR(k3Orient), delta);
				
		//try sending
		status = IPCTracker_Send(m3Tracker_MoveOrientation,theTrackerUUID,theTrackerPortName,dict,&returnDict);
		if (status!=kQ3Failure)
		{
			//Get status from returnDict
			result = IPCGetTQ3Uns32(returnDict, CFSTR(k3Status), (TQ3Uns32*)&status);
		};
		//Do clean up
		CFRelease(dict);
		
		if (returnDict)
			CFRelease(returnDict);
	}
	return(status);
}
