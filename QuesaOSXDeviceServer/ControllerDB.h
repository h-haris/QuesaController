/*  NAME:
        ControllerDB.h

    DESCRIPTION:
        Implementation of Quesa API Controller Core Library.
		
        This source file implements the methods prototypes of the device
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


#ifndef ControllerDB_CORE_HDR
#define ControllerDB_CORE_HDR

//=============================================================================
//      Include files
//-----------------------------------------------------------------------------
// Include files go here

#include <Carbon/Carbon.h>

//=============================================================================
//		C++ preamble
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

#define kQ3MaxControllerValues 		256
#define kQ3MaxControllerChannels 	32

typedef struct TC3TrackerInstanceData *TC3TrackerInstanceDataPtr;
typedef struct TC3ControllerPrivateData *TC3ControllerPrivateDataPtr;

//=============================================================================
//      Function prototypes
//-----------------------------------------------------------------------------
TQ3Status					ControllerDB_GetListChanged(TQ3Boolean *listChanged, TQ3Uns32 *serialNumber);
TQ3Status					ControllerDB_Next(TQ3ControllerRef controllerRef, TQ3ControllerRef *nextControllerRef);
TQ3ControllerRef			ControllerDB_New(const TQ3ControllerData *controllerData);
TQ3Status					ControllerDB_SetDriverPortName(TQ3ControllerRef controllerRef, CFStringRef thePortName);
TQ3Status					ControllerDB_Decommission(TQ3ControllerRef controllerRef);
TQ3Status					ControllerDB_SetActivation(TQ3ControllerRef controllerRef, TQ3Boolean active);
TQ3Status					ControllerDB_GetActivation(TQ3ControllerRef controllerRef, TQ3Boolean *active);
TQ3Status					ControllerDB_GetSignature(TQ3ControllerRef controllerRef, char *signature, TQ3Uns32 numChars);
TQ3Status					ControllerDB_GetCFSignature(TQ3ControllerRef controllerRef, CFStringRef *signature);
//TQ3Status					ControllerDB_SetChannel(TQ3ControllerRef controllerRef, TQ3Uns32 channel, const void *data, TQ3Uns32 dataSize);
TQ3Status					ControllerDB_SetChannel(TQ3ControllerRef controllerRef, CFMutableDictionaryRef dict, CFMutableDictionaryRef returnDict);
//TQ3Status					ControllerDB_GetChannel(TQ3ControllerRef controllerRef, TQ3Uns32 channel, void *data, TQ3Uns32 *dataSize);
TQ3Status					ControllerDB_GetChannel(TQ3ControllerRef controllerRef, CFMutableDictionaryRef dict, CFMutableDictionaryRef returnDict);
TQ3Status					ControllerDB_GetValueCount(TQ3ControllerRef controllerRef, TQ3Uns32 *valueCount);
//TQ3Status					ControllerDB_SetTracker(TQ3ControllerRef controllerRef, TQ3TrackerObject tracker);
TQ3Status					ControllerDB_SetTracker(TQ3ControllerRef controllerRef, CFStringRef trackerUUID, CFStringRef trackerPortName);
TQ3Status					ControllerDB_HasTracker(TQ3ControllerRef controllerRef, TQ3Boolean *hasTracker);
TQ3Status					ControllerDB_Track2DCursor(TQ3ControllerRef controllerRef, TQ3Boolean *track2DCursor);
TQ3Status					ControllerDB_Track3DCursor(TQ3ControllerRef controllerRef, TQ3Boolean *track3DCursor);
TQ3Status					ControllerDB_GetButtons(TQ3ControllerRef controllerRef, TQ3Uns32 *buttons);
TQ3Status					ControllerDB_SetButtons(TQ3ControllerRef controllerRef, TQ3Uns32 buttons);
TQ3Status					ControllerDB_GetTrackerPosition(TQ3ControllerRef controllerRef, TQ3Point3D *position);
TQ3Status					ControllerDB_SetTrackerPosition(TQ3ControllerRef controllerRef, const TQ3Point3D *position);
TQ3Status					ControllerDB_MoveTrackerPosition(TQ3ControllerRef controllerRef, const TQ3Vector3D *delta);
TQ3Status					ControllerDB_GetTrackerOrientation(TQ3ControllerRef controllerRef, TQ3Quaternion *orientation);
TQ3Status					ControllerDB_SetTrackerOrientation(TQ3ControllerRef controllerRef, const TQ3Quaternion *orientation);
TQ3Status					ControllerDB_MoveTrackerOrientation(TQ3ControllerRef controllerRef, const TQ3Quaternion *delta);
TQ3Status					ControllerDB_GetValues(TQ3ControllerRef controllerRef, TQ3Uns32 valueCount, float *values, TQ3Boolean *changed, TQ3Uns32 *serialNumber);
TQ3Status					ControllerDB_GetValuesRaw(TQ3ControllerRef controllerRef, TQ3Uns32 *valueCount, float *values, TQ3Uns32 *serialNumber);
TQ3Status					ControllerDB_SetValues(TQ3ControllerRef controllerRef, const float *values, TQ3Uns32 valueCount);

TQ3Status					ControllerDB_StateNew(TQ3ControllerRef controllerRef, CFStringRef *CtrlStateKey);
TQ3Status					ControllerDB_StateDelete(TQ3ControllerRef controllerRef, CFStringRef CtrlStateKey);
TQ3Status					ControllerDB_StateSaveAndReset(TQ3ControllerRef controllerRef, CFStringRef CtrlStateKey);
TQ3Status					ControllerDB_StateRestore(TQ3ControllerRef controllerRef, CFStringRef CtrlStateKey);


//=============================================================================
//		C++ postamble
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif
