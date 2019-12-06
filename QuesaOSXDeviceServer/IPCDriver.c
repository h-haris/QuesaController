/*  NAME:
        IPCDriver.c

    DESCRIPTION:
        Implementation of Quesa API Controller Core Library.
		
        This source file implements a method to send messages to the driver.
		
		Usage by GetChannel, SetChannel, StateSaveAndReset and StateRestore.
	  
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

#include "IPCDriver.h"
#include "IPCMessageIDs.h"

TQ3Status IPCDriver_Send( 	SInt32 msgid, 
							CFStringRef theDriverPortName, 
							CFMutableDictionaryRef dict, 
							CFMutableDictionaryRef returnDict)
{
	TQ3Status status = kQ3Failure;
	
	CFDataRef 				data,returnData;
	CFStringRef				propertyListError;
	
	CFMutableDictionaryRef 	workReturnDict = NULL;
	
	data= CFPropertyListCreateXMLData(kCFAllocatorDefault,dict);
	//what to release?
	
	CFMessagePortRef DriverPort=CFMessagePortCreateRemote(kCFAllocatorDefault, theDriverPortName);
	
	if (DriverPort!=NULL)
	{
		SInt32 ReqRes = CFMessagePortSendRequest(	DriverPort, msgid, data, 
									10, 10, kCFRunLoopDefaultMode,
									&returnData);
		
		if (ReqRes == kCFMessagePortSuccess)
		{
			if (returnData)
			{
				workReturnDict=(CFMutableDictionaryRef)CFPropertyListCreateFromXMLData(kCFAllocatorDefault, 
																					returnData, 
																					kCFPropertyListImmutable, 
																					&propertyListError);
				//was there an error? see propertyListError
				if (propertyListError)
					status=kQ3Failure;
				else
				{	
					status=kQ3Success;
					CFDictionaryAddValue(returnDict,CFSTR(k3MethodsReturn),workReturnDict);
				}
				/*
				if (propertyListError)
					status=kQ3Failure;
				else
					status=kQ3Success;
					
				CFDictionaryAddValue(dict,CFSTR(k3MethodsReturn),workReturnDict);
				*/
			}
			
			if (propertyListError)
				CFRelease(propertyListError);
			if (returnData)
				CFRelease(returnData);
		}	
	}
	if (DriverPort)
		CFRelease(DriverPort);
	if (data)
		CFRelease(data);
	
	return status;
};

