/*  NAME:
        IPCTracker.h

    DESCRIPTION:
        Implementation of Quesa API Controller Core Library.
		
        This source file implements methods to send messages to trackers which
		are associated to controllers.
				
		These methods are used by the database.
		
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


#include <Carbon/Carbon.h>

TQ3Status IPCTracker_callNotification	(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef);
											
TQ3Status IPCTracker_changeButtons		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											TQ3Uns32 buttons, 
											TQ3Uns32 buttonMask);
											
TQ3Status IPCTracker_getActivation		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3Boolean *active);
											
TQ3Status IPCTracker_getPosition		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3Point3D *position);
											
TQ3Status IPCTracker_setPosition		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											const TQ3Point3D *position);
											
TQ3Status IPCTracker_movePosition		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											const TQ3Vector3D *delta);
											
TQ3Status IPCTracker_getOrientation		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3Quaternion *orientation);
											
TQ3Status IPCTracker_setOrientation		(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											const TQ3Quaternion *orientation);
											
TQ3Status IPCTracker_moveOrientation	(	CFStringRef theTrackerUUID, 
											CFStringRef theTrackerPortName, 
											TQ3ControllerRef controllerRef, 
											const TQ3Quaternion *delta);
