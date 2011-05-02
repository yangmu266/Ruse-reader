//
//  Ruse_readerViewController.m
//  Ruse-reader
//
//  Created by tarlou on 11-4-30.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Ruse_readerViewController.h"
#import "Ruse_scrollDelegate.h"

@implementation Ruse_readerViewController


/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];

	NSString *filePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"test2.pdf"];
	
	Ruse_PDFView *pdfView = [[Ruse_PDFView alloc] initWithFilePath:filePath];
	_mScrollView.contentSize = pdfView.frame.size;
	[_mScrollView addSubview:pdfView];
	_mScrollView.maximumZoomScale = 3.0;
	[_mScrollView setZoomScale:3.0 animated:YES];
	_mScrollView.zoomScale = 2.9;
	[pdfView release];
}


// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return interfaceOrientation == UIInterfaceOrientationLandscapeRight;
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}

@end
