//
//  MSWorkerClass.m
//  Runloop
//
//  Created by guoxiaoliang on 2018/9/18.
//  Copyright © 2018年 Apple. All rights reserved.
//工作类

#import "MSWorkerClass.h"
#define kCheckinMessage 100
#define kThreadStackSize        (8 *4096)
@implementation MSWorkerClass
//Setting up the thread structures
OSStatus ServerThreadEntryPoint(void* param)
{
    // Create the remote port to the main thread.
    CFMessagePortRef mainThreadPort;
    CFStringRef portName = (CFStringRef)param;
    
    mainThreadPort = CFMessagePortCreateRemote(NULL, portName);
    
    // Free the string that was passed in param.
    CFRelease(portName);
    
    // Create a port for the worker thread.
    CFStringRef myPortName = CFStringCreateWithFormat(NULL, NULL, CFSTR("com.MyApp.Thread-%d"), MPCurrentTaskID());
    
    // Store the port in this thread’s context info for later reference.
    CFMessagePortContext context = {0, mainThreadPort, NULL, NULL, NULL};
    Boolean shouldFreeInfo;
    Boolean shouldAbort = TRUE;
    
    CFMessagePortRef myPort = CFMessagePortCreateLocal(NULL,
                                                       myPortName,
                                                       &ProcessClientRequest,
                                                       &context,
                                                       &shouldFreeInfo);
    
    if (shouldFreeInfo)
    {
        // Couldn't create a local port, so kill the thread.
        MPExit(0);
    }
    
    CFRunLoopSourceRef rlSource = CFMessagePortCreateRunLoopSource(NULL, myPort, 0);
    if (!rlSource)
    {
        // Couldn't create a local port, so kill the thread.
        MPExit(0);
    }
    
    // Add the source to the current run loop.
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rlSource, kCFRunLoopDefaultMode);
    
    // Once installed, these can be freed.
    CFRelease(myPort);
    CFRelease(rlSource);
    
    // Package up the port name and send the check-in message.
    CFDataRef returnData = nil;
    CFDataRef outData;
    CFIndex stringLength = CFStringGetLength(myPortName);
    UInt8* buffer = CFAllocatorAllocate(NULL, stringLength, 0);
    
    CFStringGetBytes(myPortName,
                     CFRangeMake(0,stringLength),
                     kCFStringEncodingASCII,
                     0,
                     FALSE,
                     buffer,
                     stringLength,
                     NULL);
    
    outData = CFDataCreate(NULL, buffer, stringLength);
    
    CFMessagePortSendRequest(mainThreadPort, kCheckinMessage, outData, 0.1, 0.0, NULL, NULL);
    
    // Clean up thread data structures.
    CFRelease(outData);
    CFAllocatorDeallocate(NULL, buffer);
    
    // Enter the run loop.
    CFRunLoopRun();
}
//Receiving the checkin message
// Main thread port message handler
CFDataRef MainThreadResponseHandler(CFMessagePortRef local,
                                    SInt32 msgid,
                                    CFDataRef data,
                                    void* info)
{
    if (msgid == kCheckinMessage)
    {
        CFMessagePortRef messagePort;
        CFStringRef threadPortName;
        CFIndex bufferLength = CFDataGetLength(data);
        UInt8* buffer = CFAllocatorAllocate(NULL, bufferLength, 0);
        
        CFDataGetBytes(data, CFRangeMake(0, bufferLength), buffer);
        threadPortName = CFStringCreateWithBytes (NULL, buffer, bufferLength, kCFStringEncodingASCII, FALSE);
        
        // You must obtain a remote message port by name.
        messagePort = CFMessagePortCreateRemote(NULL, (CFStringRef)threadPortName);
        
        if (messagePort)
        {
            // Retain and save the thread’s comm port for future reference.
            AddPortToListOfActiveThreads(messagePort);
            
            // Since the port is retained by the previous function, release
            // it here.
            CFRelease(messagePort);
        }
        
        // Clean up.
        CFRelease(threadPortName);
        CFAllocatorDeallocate(NULL, buffer);
    }
    else
    {
        // Process other messages.
    }
    
    return NULL;
}
// Attaching a Core Foundation message port to a new thread
OSStatus MySpawnThread()
{
    // Create a local port for receiving responses.
    CFStringRef myPortName;
    CFMessagePortRef myPort;
    CFRunLoopSourceRef rlSource;
    CFMessagePortContext context = {0, NULL, NULL, NULL, NULL};
    Boolean shouldFreeInfo;
    
    // Create a string with the port name.
    myPortName = CFStringCreateWithFormat(NULL, NULL, CFSTR("com.myapp.MainThread"));
    
    // Create the port.
    myPort = CFMessagePortCreateLocal(NULL,
                                      myPortName,
                                      &MainThreadResponseHandler,
                                      &context,
                                      &shouldFreeInfo);
    
    if (myPort != NULL)
    {
        // The port was successfully created.
        // Now create a run loop source for it.
        rlSource = CFMessagePortCreateRunLoopSource(NULL, myPort, 0);
        
        if (rlSource)
        {
            // Add the source to the current run loop.
            CFRunLoopAddSource(CFRunLoopGetCurrent(), rlSource, kCFRunLoopDefaultMode);
            
            // Once installed, these can be freed.
            CFRelease(myPort);
            CFRelease(rlSource);
        }
    }
    
    // Create the thread and continue processing.
    MPTaskID        taskID;
    return(MPCreateTask(&ServerThreadEntryPoint,
                        (void*)myPortName,
                        kThreadStackSize,
                        NULL,
                        NULL,
                        NULL,
                        0,
                        &taskID));
}
//Launching the worker thread using Mach ports
+(void)LaunchThreadWithPort:(id)inData
{
    NSAutoreleasePool*  pool = [[NSAutoreleasePool alloc] init];
    
    // Set up the connection between this thread and the main thread.
    NSPort* distantPort = (NSPort*)inData;
    
    MSWorkerClass*  workerObj = [[self alloc] init];
    [workerObj sendCheckinMessage:distantPort];
    [distantPort release];
    
    // Let the run loop process things.
    do
    {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode
                                 beforeDate:[NSDate distantFuture]];
    }
    while (![workerObj shouldExit]);
    
    [workerObj release];
    [pool release];
}
// Worker thread check-in method
/// Sending the check-in message using Mach ports
- (void)sendCheckinMessage:(NSPort*)outPort
{
    // Retain and save the remote port for future use.
    [self setRemotePort:outPort];
    
    // Create and configure the worker thread port.
    NSPort* myPort = [NSMachPort port];
    [myPort setDelegate:self];
    [[NSRunLoop currentRunLoop] addPort:myPort forMode:NSDefaultRunLoopMode];
    
    // Create the check-in message.
    NSPortMessage* messageObj = [[NSPortMessage alloc] initWithSendPort:outPort
                                                            receivePort:myPort components:nil];
    
    if (messageObj)
    {
        // Finish configuring the message and send it immediately.
        [messageObj setMsgId:setMsgid:kCheckinMessage];
        [messageObj sendBeforeDate:[NSDate date]];
    }
}

// Handle responses from the worker thread.
- (void)handlePortMessage:(NSPortMessage *)portMessage
{
    unsigned int message = [portMessage msgid];
    NSPort* distantPort = nil;
    
    if (message == kCheckinMessage)
    {
        // Get the worker thread’s communications port.
        distantPort = [portMessage sendPort];
        
        // Retain and save the worker port for later use.
        [self storeDistantPort:distantPort];
    }
    else
    {
        // Handle other messages.
    }
}
//Registering a message port
- (void)registerMessagePort {
    NSPort* localPort = [[NSMessagePort alloc] init];
    
    // Configure the object and add it to the current run loop.
    [localPort setDelegate:self];
    [[NSRunLoop currentRunLoop] addPort:localPort forMode:NSDefaultRunLoopMode];
    
    // Register the port using a specific name. The name must be unique.
    NSString* localPortName = [NSString stringWithFormat:@"MyPortName"];
    [[NSMessagePortNameServer sharedInstance] registerPort:localPort
                                                      name:localPortName];

}
@end
