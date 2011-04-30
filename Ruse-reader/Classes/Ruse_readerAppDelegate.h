//
//  Ruse_readerAppDelegate.h
//  Ruse-reader
//
//  Created by tarlou on 11-4-30.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class Ruse_readerViewController;

@interface Ruse_readerAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    Ruse_readerViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet Ruse_readerViewController *viewController;

@end

