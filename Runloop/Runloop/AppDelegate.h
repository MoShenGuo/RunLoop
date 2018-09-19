//
//  AppDelegate.h
//  Runloop
//
//  Created by guoxiaoliang on 2018/9/18.
//  Copyright © 2018年 Apple. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>


@property (strong, nonatomic) UIWindow *window;

- (AppDelegate *)sharedAppDelegate;

@end

