//
//  MSRunLoopSource.h
//  Runloop
//
//  Created by guoxiaoliang on 2018/9/18.
//  Copyright © 2018年 Apple. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "AppDelegate.h"
//The custom input source object definition
@class MSRunLoopContext;
@interface MSRunLoopSource : NSObject

{
    CFRunLoopSourceRef runLoopSource;
    NSMutableArray* commands;
}
- (id)init;
- (void)addToCurrentRunLoop;
- (void)invalidate;

// Handler method
- (void)sourceFired;

// Client interface for registering commands to process
- (void)addCommand:(NSInteger)command withData:(id)data;
- (void)fireAllCommandsOnRunLoop:(CFRunLoopRef)runloop;
// These are the CFRunLoopSourceRef callback functions.
void RunLoopSourceScheduleRoutine (void *info, CFRunLoopRef rl, CFStringRef mode);
void RunLoopSourcePerformRoutine (void *info);
void RunLoopSourceCancelRoutine (void *info, CFRunLoopRef rl, CFStringRef mode);

@end
