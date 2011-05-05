//
//  Ruse-PDFView.m
//  Ruse-reader
//
//  Created by tarlou on 11-5-3.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Ruse_PDFView.h"
#import "Ruse_readerViewController.h"

@implementation Ruse_PDFView

- (id)initWithFilePath:(NSString *)filePath with:(id)view{
    currentPageNumber = 1;
	pdfRef = [self createPDFFromExistFile:filePath];
	CGPDFPageRef pdfPage = CGPDFDocumentGetPage(pdfRef, currentPageNumber);
	CGRect mediaRect = CGPDFPageGetBoxRect(pdfPage, kCGPDFMediaBox);
	rrvc = view;
	self = [super initWithFrame:mediaRect];
		
	return self;
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (touches == NULL) return;
	UITouch * touch = [[event allTouches] anyObject];
    CGPoint location = [touch locationInView:self];
	CGFloat x = location.x;
	CGFloat y = location.y;
	Ruse_readerViewController * r_rvc = rrvc;
	//[r_rvc setHidden: FALSE];
	NSString * str = [NSString stringWithFormat:@"%f %f",x,y]; 
    NSLog(@"%@", str);
	[r_rvc performSelectorOnMainThread:@selector(setTitle:) withObject:str waitUntilDone:YES];
//	[r_rvc setTitle: @"I am here"];
}

- (void)drawRect:(CGRect)rect {
	CGContextRef context = UIGraphicsGetCurrentContext();
	
//	[[UIColor colorWithRed:1.0 green:1.0 blue:0.8 alpha:1.0] set];
	[[UIColor whiteColor] set];
	CGContextFillRect(context, rect);
	
	CGContextScaleCTM(context, 1, -1);
	CGContextTranslateCTM(context, 0, -rect.size.height);
	
	page = CGPDFDocumentGetPage(pdfRef, currentPageNumber);
	
	CGContextDrawPDFPage(context, page);
}

- (CGPDFDocumentRef)createPDFFromExistFile:(NSString *)aFilePath{
	CFStringRef path;
	CFURLRef url;
	CGPDFDocumentRef document;
	path = CFStringCreateWithCString(NULL, [aFilePath UTF8String], kCFStringEncodingUTF8);
	url = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, NO);
	CFRelease(path);
	document = CGPDFDocumentCreateWithURL(url);
	CFRelease(url);
	int count = CGPDFDocumentGetNumberOfPages (document);
    if (count == 0) {
		return NULL;
    }
	return document;
}

- (void)dealloc {
    [super dealloc];
}	

-(void)reloadView{
	[self setNeedsDisplay];
}

- (void) changePageNumber: (int) number {
    currentPageNumber = number;
}

@end
