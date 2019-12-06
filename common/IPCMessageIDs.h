/*  NAME:
        IPCMessageIDs.h

    DESCRIPTION:
        Header file with message ids used for IPC.
		
		Implementation of Quesa controller API calls.
		
		Under MacOS X the communication between driver, device server and client
		is implemented via IPC. This header defines the used messages and the names 
		of the remote message ports.
      
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

#ifndef IPCMessageIDs_HDR
#define IPCMessageIDs_HDR

#include <Carbon/Carbon.h>

//=============================================================================
//		C++ preamble
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

enum
{
	m3Controller_GetListChanged				= 1000,
	m3Controller_Next						= 1001,
	m3Controller_New						= 1002,
	m3Controller_Decommission				= 1003,
	m3Controller_SetActivation				= 1004,
	m3Controller_GetActivation				= 1005,
	m3Controller_GetSignature				= 1006,
	m3Controller_SetChannel					= 1007,
	m3Controller_GetChannel					= 1008,
	m3Controller_GetValueCount				= 1009,
	m3Controller_SetTracker					= 1010,
	m3Controller_HasTracker					= 1011,
	m3Controller_Track2DCursor				= 1012,
	m3Controller_Track3DCursor				= 1013,
	m3Controller_GetButtons					= 1014,
	m3Controller_SetButtons					= 1015,
	m3Controller_GetTrackerPosition			= 1016,
	m3Controller_SetTrackerPosition			= 1017,
	m3Controller_MoveTrackerPosition		= 1018,
	m3Controller_GetTrackerOrientation		= 1019,
	m3Controller_SetTrackerOrientation		= 1020,
	m3Controller_MoveTrackerOrientation		= 1021,
	m3Controller_GetValues					= 1022,
	m3Controller_SetValues					= 1023,
	m3ControllerDriver_SetChannel			= 1500,
	m3ControllerDriver_GetChannel			= 1501,
	m3ControllerDriver_StateSaveAndReset	= 1502,
	m3ControllerDriver_StateRestore			= 1503,
	m3ControllerState_New					= 1700,
	m3ControllerState_Delete				= 1701,
	m3ControllerState_SaveAndReset			= 1702,
	m3ControllerState_Restore				= 1703,
	m3Tracker_ChangeButtons					= 2000,
	m3Tracker_GetActivation					= 2001,
	m3Tracker_GetPosition					= 2002,
	m3Tracker_SetPosition					= 2003,
	m3Tracker_MovePosition					= 2004,
	m3Tracker_GetOrientation				= 2005,
	m3Tracker_SetOrientation				= 2006,
	m3Tracker_MoveOrientation				= 2007,
	m3Tracker_CallNotification				= 2100
};

#define kQuesa3DeviceServer 	"com.quesa.osx.3device.server"
#define kQuesa3DeviceDriver 	"com.quesa.osx.3device.driver"
#define kQuesa3DeviceTracker 	"com.quesa.osx.3device.tracker"

//Constants for controller values
#define k3CtrlRef			"E3CtrlRef"
#define k3Status			"E3Status"
#define k3ListChanged		"E3ListChanged"
#define k3SerNum			"E3SerNum"
#define k3Signature			"E3Signature"
#define k3CtrlData			"E3CtrlData"
#define k3Active			"E3Active"
#define k3Channel			"E3Channel"
#define k3ChannelCount		"E3ChannelCount"
#define k3ChannelsData		"E3ChannelsData"
#define k3DataSize			"E3DataSize"
#define k3Data				"E3Data"
#define k3MethodRef			"E3MethodRef"
#define k3SetMethodRef		"E3SetMethodRef"
#define k3GetMethodRef		"E3GetMethodRef"
#define k3MethodsReturn		"E3MethodsReturn"
#define k3ValueCnt			"E3ValueCnt"
#define k3hasTracker		"E3hasTracker"
#define k3Track2DCrsr		"E3Track2DCrsr"
#define k3Track3DCrsr		"E3Track3DCrsr"
#define k3Buttons			"E3Buttons"
#define k3ButtonMask		"E3ButtonMask"
#define k3Position			"E3Position"
#define k3DeltaPos			"E3DeltaPos"
#define k3Orient			"E3Orient"
#define k3DeltaOrient		"E3DeltaOrient"
#define k3Values			"E3Values"
#define k3ValueCount		"E3ValueCount"
#define k3PrivValueCount	"E3PrivValueCount"
#define k3ValuesChanged		"E3ValuesChanged"

#define k3CtrlStateUUID		"E3CtrlStateUUID"

//Constants for tracker values
#define k3TrackerUUID		"E3TrackerUUID"
#define k3TrackerPortName	"E3TrackerPortName"

//Constants for driver values
#define k3DriverUUID		"E3DriverUUID"
#define k3DriverPortName	"E3DriverPortName"

//=============================================================================
//		C++ postamble
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif
