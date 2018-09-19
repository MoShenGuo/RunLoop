//
//  MSRunLoopSource.m
//  Runloop
//
//  Created by guoxiaoliang on 2018/9/18.
//  Copyright © 2018年 Apple. All rights reserved.
//

#import "MSRunLoopSource.h"
#import "MSRunLoopContext.h"
@interface MSRunLoopSource()
@end
@implementation MSRunLoopSource
{
    NSMutableArray *sourcesToPing;
}
// Scheduling a run loop source  调度运行循环来源
void RunLoopSourceScheduleRoutine (void *info, CFRunLoopRef rl, CFStringRef mode)
{
    MSRunLoopSource* obj = (__bridge MSRunLoopSource *)info;
    AppDelegate*   del = [UIApplication sharedApplication].delegate;//[AppDelegate sharedAppDelegate];
    MSRunLoopContext* theContext = [[MSRunLoopContext alloc] initWithSource:obj andLoop:rl];
    
    [del performSelectorOnMainThread:@selector(registerSource:)
                          withObject:theContext waitUntilDone:NO];
}
//Performing work in the input source 执行工作输入源
void RunLoopSourcePerformRoutine (void *info)
{
    MSRunLoopSource*  obj = (__bridge MSRunLoopSource *)info;
    [obj sourceFired];
}
//Invalidating an input source 移除输入源
void RunLoopSourceCancelRoutine (void *info, CFRunLoopRef rl, CFStringRef mode)
{
    MSRunLoopSource* obj = (__bridge MSRunLoopSource *)info;
    AppDelegate* del = [UIApplication sharedApplication].delegate;//[AppDelegate sharedAppDelegate];
    MSRunLoopSource* theContext = [[MSRunLoopContext alloc] initWithSource:obj andLoop:rl];
    
    [del performSelectorOnMainThread:@selector(removeSource:)
                          withObject:theContext waitUntilDone:YES];
}
//Registering and removing an input source with the application delegate
//注册和删除应用程序委托的输入源
- (void)registerSource:(MSRunLoopContext *)sourceInfo;
{
    [sourcesToPing addObject:sourceInfo];
}
- (void)removeSource:(MSRunLoopContext *)sourceInfo
{
    id    objToRemove = nil;
    
    for (MSRunLoopContext* context in sourcesToPing)
    {
        if ([context isEqual:sourceInfo])
        {
            objToRemove = context;
            break;
        }
    }
    
    if (objToRemove)
    [sourcesToPing removeObject:objToRemove];
}
//Installing the run loop source
- (id)init
{
    sourcesToPing = [NSMutableArray array];
    CFRunLoopSourceContext    context = {0, (__bridge void *)(self), NULL, NULL, NULL, NULL, NULL,
        &RunLoopSourceScheduleRoutine,
        RunLoopSourceCancelRoutine,
        RunLoopSourcePerformRoutine};
    
    runLoopSource = CFRunLoopSourceCreate(NULL, 0, &context);
    commands = [[NSMutableArray alloc] init];
    
    return self;
}
//Waking up the run loop  唤醒的循环运行
- (void)addToCurrentRunLoop
{
    CFRunLoopRef runLoop = CFRunLoopGetCurrent();
    CFRunLoopAddSource(runLoop, runLoopSource, kCFRunLoopDefaultMode);
}
- (void)fireCommandsOnRunLoop:(CFRunLoopRef)runloop
{
    CFRunLoopSourceSignal(runLoopSource);
    CFRunLoopWakeUp(runloop);
}
@end
