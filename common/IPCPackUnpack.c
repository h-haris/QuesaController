 /*  NAME:
        IPCPackUnpack.c

    DESCRIPTION:
        Used by QuesaOSXDeviceServer and ControllerCoreOSX.
		
		Implementation of Quesa controller API calls.
		
		Under MacOS X the communication between driver, device server and client
		is implemented via IPC. This source file defines some supporting functions
		to convert several Quesa or internal structures to and from Core Foundation objects.
      
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

#include "IPCPackUnpack.h"

#define PT_ELEMENTS	(3)
#define VC_ELEMENTS	(3)
#define QT_ELEMENTS	(4)

Boolean IPCPutControllerRef(CFMutableDictionaryRef dict, const void *key, TQ3ControllerRef *controllerRef)
{
	CFDataRef ControllerRefValue = CFDataCreate(kCFAllocatorDefault,(UInt8*)controllerRef,sizeof(TQ3ControllerRef));
	CFDictionarySetValue(dict,key,ControllerRefValue);
	CFRelease(ControllerRefValue);
	
	return true; //No error checking so far
};

Boolean IPCGetControllerRef(CFDictionaryRef dict, const void *key, TQ3ControllerRef *controllerRef)
{
	CFDataGetBytes(	(CFDataRef)CFDictionaryGetValue(dict,key),
						CFRangeMake(0,sizeof(TQ3ControllerRef)),
						(UInt8*)controllerRef);
						
	return true; //No error checking so far				
};

Boolean IPCPutBytes(CFMutableDictionaryRef dict, const void *key, size_t ref_size, const void *ref)
{
	CFDataRef RefValue = CFDataCreate(kCFAllocatorDefault,(UInt8*)ref,ref_size);
	CFDictionarySetValue(dict,key,RefValue);
	CFRelease(RefValue);
	
	return true; //No error checking so far
};

Boolean IPCGetBytes(CFDictionaryRef dict, const void *key, size_t ref_size, void *ref)
{
	CFDataGetBytes(	(CFDataRef)CFDictionaryGetValue(dict,key),
						CFRangeMake(0,ref_size),
						(UInt8*)ref);
						
	return true; //No error checking so far				
};

Boolean IPCPutTQ3Boolean(CFMutableDictionaryRef dict, const void *key, TQ3Boolean *src)
{
	if (*src == kQ3True)
		CFDictionarySetValue(dict, key, kCFBooleanTrue);
	else
		CFDictionarySetValue(dict, key, kCFBooleanFalse);
						
	return true; //No error checking so far
};

Boolean IPCGetTQ3Boolean(CFDictionaryRef dict, const void *key, TQ3Boolean *dest)
{
	*dest = (TQ3Boolean)CFBooleanGetValue((CFBooleanRef)CFDictionaryGetValue(dict, key));						
	return true; //No error checking so far				
};

Boolean IPCPutTQ3Uns32(CFMutableDictionaryRef dict, const void *key, TQ3Uns32 *src)
{
	CFNumberRef SrcRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, src);
	CFDictionarySetValue(dict, key, SrcRef);
	CFRelease(SrcRef);
		
	return true; //No error checking so far
};

Boolean IPCGetTQ3Uns32(CFDictionaryRef dict, const void *key, TQ3Uns32 *dest)
{
	Boolean result = CFNumberGetValue(	(CFNumberRef)CFDictionaryGetValue(dict, key),
										kCFNumberSInt32Type,
										dest);
														
	return result;				
};

Boolean IPCPutTQ3Point3D(CFMutableDictionaryRef dict, const void *key, const TQ3Point3D *src)
{
	CFNumberRef ptInput[PT_ELEMENTS];
	int i;
	
	ptInput[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->x));
	ptInput[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->y));
	ptInput[2] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->z));
	CFArrayRef ptRef = CFArrayCreate(kCFAllocatorDefault, (const void **)ptInput, PT_ELEMENTS, &kCFTypeArrayCallBacks);
	
	CFDictionarySetValue(dict, key, ptRef);
	
	for (i=0;i<PT_ELEMENTS;i++)
		CFRelease(ptInput[i]);
		
	CFRelease(ptRef);
		
	return true; //No error checking so far
};


Boolean IPCGetTQ3Point3D(CFDictionaryRef dict, const void *key, TQ3Point3D *dest)
{
	Boolean result = false;
	
	CFArrayRef pt = (CFArrayRef)CFDictionaryGetValue(dict, key);
	if (pt!=NULL)
	{
		//dest.x
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(pt, 0),
									kCFNumberFloatType,
									&(dest->x));
		//dest.y
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(pt, 1),
									kCFNumberFloatType,
									&(dest->y));
		//dest.z
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(pt, 2),
									kCFNumberFloatType,
									&(dest->z));
									
		//CFRelease(pt);//release not necessary!
		//why not? Because dict is owner of value, returns only a reference of it and will take care of everything
	}
	
	return result;				
};

Boolean IPCPutTQ3Vector3D(CFMutableDictionaryRef dict, const void *key, const TQ3Vector3D *src)
{
	CFNumberRef vcInput[VC_ELEMENTS];
	int i;
	
	vcInput[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->x));
	vcInput[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->y));
	vcInput[2] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->z));
	CFArrayRef vcRef = CFArrayCreate(kCFAllocatorDefault, (const void **)vcInput, VC_ELEMENTS, &kCFTypeArrayCallBacks);
	
	CFDictionarySetValue(dict, key, vcRef);
	
	for (i=0;i<VC_ELEMENTS;i++)
		CFRelease(vcInput[i]);
		
	CFRelease(vcRef);
		
	return true; //No error checking so far
};


Boolean IPCGetTQ3Vector3D(CFDictionaryRef dict, const void *key, TQ3Vector3D *dest)
{
	Boolean result = false;
	
	CFArrayRef vc = (CFArrayRef)CFDictionaryGetValue(dict, key);
	if (vc!=NULL)
	{
		//dest.x
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(vc, 0),
									kCFNumberFloatType,
									&(dest->x));
		//dest.y
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(vc, 1),
									kCFNumberFloatType,
									&(dest->y));
		//dest.z
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(vc, 2),
									kCFNumberFloatType,
									&(dest->z));
									
		//CFRelease(vc);//release not necessary; 
		//why not? Because dict is owner of value, returns only a reference of it and will take care of everything
	}
	
	return result;				
};

Boolean IPCPutTQ3Quaternion(CFMutableDictionaryRef dict, const void *key, const TQ3Quaternion *src)
{
	CFNumberRef qtInput[QT_ELEMENTS];
	int i;
	
	qtInput[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->x));
	qtInput[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->y));
	qtInput[2] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->z));
	qtInput[3] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(src->w));
	CFArrayRef qtRef = CFArrayCreate(kCFAllocatorDefault, (const void **)qtInput, QT_ELEMENTS, &kCFTypeArrayCallBacks);
	
	CFDictionarySetValue(dict, key, qtRef);
	
	for (i=0;i<QT_ELEMENTS;i++)
		CFRelease(qtInput[i]);
		
	CFRelease(qtRef);
		
	return true; //No error checking so far
};


Boolean IPCGetTQ3Quaternion(CFDictionaryRef dict, const void *key, TQ3Quaternion *dest)
{
	Boolean result = false;
	
	CFArrayRef qt = (CFArrayRef)CFDictionaryGetValue(dict, key);
	if (qt!=NULL)
	{
		//dest.x
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(qt, 0),
									kCFNumberFloatType,
									&(dest->x));
		//dest.y
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(qt, 1),
									kCFNumberFloatType,
									&(dest->y));
		//dest.z
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(qt, 2),
									kCFNumberFloatType,
									&(dest->z));
		//dest.w
		result = CFNumberGetValue(	(CFNumberRef)CFArrayGetValueAtIndex(qt, 3),
									kCFNumberFloatType,
									&(dest->w));
																
		//CFRelease(qt);//release not necessary; 
		//why not? Because dict is owner of value, returns only a reference of it and will take care of everything
	}
	
	return result;				
};


