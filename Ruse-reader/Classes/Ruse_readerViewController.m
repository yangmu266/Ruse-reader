//
//  Ruse_readerViewController.m
//  Ruse-reader
//
//  Created by tarlou on 11-4-30.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Ruse_readerViewController.h"
//#import "Ruse_scrollDelegate.h"
#import "PDFScrollView.h"

@implementation Ruse_readerViewController

-(id)init{
	self = [super init];
	if (self)
	{
		Toolbar = [[UIToolbar alloc] init];
		Open = [[UIBarButtonItem alloc] init];
		//Open.action = @selector(setOpen);
		Filename = [[UIBarButtonItem alloc] init];
		[Filename Enabled];
		Zoom = [[UISegmentedControl alloc] init];
		Pre_page = [[UIBarButtonItem alloc] init];
		Next_page = [[UIBarButtonItem alloc] init];
		Bookmark = [[UIBarButtonItem alloc] init];
		//Search = [[UISearchDisplayController alloc] init];
	}
	return self;
}

-(void)setHidden:(bool)hide
{
	Toolbar.hidden = hide;
/*	Open.hidden = hide;
	Filename.hidden = hide;
	Bookmark.hidden = hide;
	Next_page.hidden = hide;
	Pre_page.hidden = hide;*/
	Zoom.hidden = hide;
}

-(void)setTitle:(NSString *)str
{
	Filename.title = str;
}

-(IBAction) setOpen
{
}
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

    [super loadView];
	
	// Create our PDFScrollView and add it to the view controller.
    
	
//	NSString *filePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"test2.pdf"];
//	PDFScrollView *sv = [[PDFScrollView alloc] initWithFrame:[[self view] bounds]];
    CGRect rect = [_mMainView bounds];
    CGRect bounds = CGRectMake(rect.origin.x, rect.origin.y, rect.size.height, rect.size.width);
    PDFScrollView *sv = [[PDFScrollView alloc] initWithFilePath:@"test2.pdf" withView:self withFrame:bounds];
//    _mScrollView.maximumZoomScale = 5.0;
//    _mScrollView.minimumZoomScale = 1.0;
    [_mMainView addSubview: sv];
    [sv release];
    

/*
	NSString *filePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"welcome.pdf"];
	
	Ruse_PDFView *pdfView = [[Ruse_PDFView alloc] initWithFilePath:filePath with:self];
	_mScrollView.contentSize = pdfView.frame.size;
	[_mScrollView addSubview:pdfView];
	_mScrollView.maximumZoomScale = 3.0;
	[_mScrollView setZoomScale:3.0 animated:YES];
	_mScrollView.zoomScale = 1;
	[pdfView release];
 */
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
