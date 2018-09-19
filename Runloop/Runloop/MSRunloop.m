//
//  MSRunloop.m
//  Runloop
//
//  Created by guoxiaoliang on 2018/9/18.
//  Copyright © 2018年 Apple. All rights reserved.
//

#import "MSRunloop.h"

@implementation MSRunloop
{
    int timeoutCount;
    CFRunLoopObserverRef observer;
    NSMutableArray *_backtrace;
@public
    dispatch_semaphore_t semaphore;
    CFRunLoopActivity activity;
}
static void runLoopObserverCallBack(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info)
{
   
}
//CFRunLoopTimerRef timer, void *info
static void runLoopTimerCallBack(CFRunLoopTimerRef timer, void *info)
{

}
// Creating a run loop observer
- (void)threadMain
{
    // The application uses garbage collection, so no autorelease pool is needed.
    NSRunLoop* myRunLoop = [NSRunLoop currentRunLoop];
    
    // Create a run loop observer and attach it to the run loop.
    CFRunLoopObserverContext  context = {0, (__bridge void *)(self), NULL, NULL, NULL};
    CFRunLoopObserverRef    observer = CFRunLoopObserverCreate(kCFAllocatorDefault,
                                                               kCFRunLoopAllActivities, YES, 0, &runLoopObserverCallBack, &context);
    
    if (observer)
    {
        CFRunLoopRef    cfLoop = [myRunLoop getCFRunLoop];
        CFRunLoopAddObserver(cfLoop, observer, kCFRunLoopDefaultMode);
    }
    
    // Create and schedule the timer.
    [NSTimer scheduledTimerWithTimeInterval:0.1 target:self
                                   selector:@selector(doFireTimer:) userInfo:nil repeats:YES];
    
    NSInteger    loopCount = 10;
    do
    {
        // Run the run loop 10 times to let the timer fire.
        [myRunLoop runUntilDate:[NSDate dateWithTimeIntervalSinceNow:1]];
        loopCount--;
    }
    while (loopCount);
}

//Main thread launch method
//主线程启动方法
- (void)launchThread
{
    NSPort* myPort = [NSMachPort port];
    if (myPort)
    {
        // This class handles incoming port messages.
        [myPort setDelegate:self];
        
        // Install the port as an input source on the current run loop.
        [[NSRunLoop currentRunLoop] addPort:myPort forMode:NSDefaultRunLoopMode];
        
        // Detach the thread. Let the worker release the port.
        [NSThread detachNewThreadSelector:@selector(LaunchThreadWithPort:)
                                 toTarget:[MSWorkerClass class] withObject:myPort];
    }
}
//Running a run loop
- (void)skeletonThreadMain
{
    // Set up an autorelease pool here if not using garbage collection.
    BOOL done = NO;
    
    // Add your sources or timers to the run loop and do any other setup.
    
    do
    {
        // Start the run loop but return after each source is handled.
        SInt32    result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 10, YES);
        
        // If a source explicitly stopped the run loop, or if there are no
        // sources or timers, go ahead and exit.
        if ((result == kCFRunLoopRunStopped) || (result == kCFRunLoopRunFinished))
        done = YES;
        
        // Check for any other exit conditions here and set the
        // done variable as needed.
    }
    while (!done);
    
    // Clean up code here. Be sure to release any allocated autorelease pools.
}

//Creating and scheduling timers using NSTimer
- (void)creatTimer {
    NSRunLoop* myRunLoop = [NSRunLoop currentRunLoop];
    
    // Create and schedule the first timer.
    NSDate* futureDate = [NSDate dateWithTimeIntervalSinceNow:1.0];
    NSTimer* myTimer = [[NSTimer alloc] initWithFireDate:futureDate
                                                interval:0.1
                                                  target:self
                                                selector:@selector(myDoFireTimer1:)
                                                userInfo:nil
                                                 repeats:YES];
    [myRunLoop addTimer:myTimer forMode:NSDefaultRunLoopMode];
    
    // Create and schedule the second timer.
    [NSTimer scheduledTimerWithTimeInterval:0.2
                                     target:self
                                   selector:@selector(myDoFireTimer2:)
                                   userInfo:nil
                                    repeats:YES];
}
// Creating and scheduling a timer using Core Foundation
- (void)creatCoreTimer {
    
    CFRunLoopRef runLoop = CFRunLoopGetCurrent();
    CFRunLoopTimerContext context = {0, NULL, NULL, NULL, NULL};
    CFRunLoopTimerRef timer = CFRunLoopTimerCreate(kCFAllocatorDefault, 0.1, 0.3, 0, 0,
                                                   &runLoopTimerCallBack, &context);
    
    CFRunLoopAddTimer(runLoop, timer, kCFRunLoopCommonModes);

}
@end
