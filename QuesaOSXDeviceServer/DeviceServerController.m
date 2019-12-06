/*  NAME:
        DeviceServerController.m

    DESCRIPTION:
        Implementation of Quesa API Controller Core Library.
		
        Implementation of the DeviceServerController object under MacOS X.
		
		An instance of this object will hold the message port to the database.
		
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


#import "DeviceServerController.h"

#import "IPCMessageIDs.h"
#import "IPCController.h"

@implementation DeviceServerController

- init
{
	self = [super init];
	if( !self ) return self;

	//CFMessageport
	CFMessagePortContext	context;
	
	context.version = 0;
	context.info = self;//used as reference to self; could be NULL, because IPCControllerDispatcher doesn't use it
	context.retain = NULL;
	context.release = NULL;
	context.copyDescription = NULL;
   
	theServerPortRef = CFMessagePortCreateLocal(NULL, CFSTR(kQuesa3DeviceServer), IPCControllerDispatcher, &context, NULL);
	theLoopSource = CFMessagePortCreateRunLoopSource(NULL, theServerPortRef, 0);
	CFRunLoopAddSource([[NSRunLoop currentRunLoop] getCFRunLoop], theLoopSource, kCFRunLoopDefaultMode);

	return self;
}

- (void)dealloc
{
	//CFMessageport
	CFRunLoopRemoveSource([[NSRunLoop currentRunLoop] getCFRunLoop], theLoopSource, kCFRunLoopDefaultMode);
	CFRelease(theLoopSource);
	CFRelease(theServerPortRef);
	
	[super dealloc];
}

@end
