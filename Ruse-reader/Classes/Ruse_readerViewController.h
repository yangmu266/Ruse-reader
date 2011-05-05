//
//  Ruse_readerViewController.h
//  Ruse-reader
//
//  Created by tarlou on 11-4-30.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Ruse_PDFView.h"

@interface Ruse_readerViewController : UIViewController {
@public
	IBOutlet UIView* _mMainView;
	IBOutlet UIScrollView* _mScrollView;
	IBOutlet UIToolbar * Toolbar;
	IBOutlet UIBarButtonItem *Open;
	IBOutlet UIBarButtonItem *Filename;
	IBOutlet UISegmentedControl *Zoom;
	IBOutlet UIBarButtonItem *Pre_page;
	IBOutlet UIBarButtonItem *Next_page;
	IBOutlet UIBarButtonItem *Bookmark;
}

-(IBAction) setOpen;
-(void)setHidden:(bool) hide;
-(void)setTitle:(NSString *) str;

@end

