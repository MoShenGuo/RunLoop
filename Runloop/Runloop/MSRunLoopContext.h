//
//  MSRunLoopContext.h
//  Runloop
//
//  Created by guoxiaoliang on 2018/9/18.
//  Copyright © 2018年 Apple. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSRunLoopSource.h"
// RunLoopContext is a container object used during registration of the input source.
//RunLoopContext是一个容器对象中使用登记的输入源。
@interface MSRunLoopContext : NSObject
{
    CFRunLoopRef        runLoop;
    MSRunLoopSource *      source;
}
@property (readonly) CFRunLoopRef runLoop;
@property (readonly) MSRunLoopSource *source;

- (id)initWithSource:(MSRunLoopSource *)src andLoop:(CFRunLoopRef)loop;
@end
