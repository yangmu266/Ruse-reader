//
//  Ruse-PDFView.m
//  Ruse-reader
//
//  Created by tarlou on 11-5-3.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Ruse_PDFView.h"

@implementation Ruse_PDFView

- (id)initWithFilePath:(NSString *)filePath{
	pdfRef = [self createPDFFromExistFile:filePath];
	CGPDFPageRef pdfPage = CGPDFDocumentGetPage(pdfRef, 1);
	CGRect mediaRect = CGPDFPageGetBoxRect(pdfPage, kCGPDFMediaBox);
	self = [super initWithFrame:mediaRect];
		
	return self;
}

- (void)drawRect:(CGRect)rect {
	CGContextRef context = UIGraphicsGetCurrentContext();
	
//	[[UIColor colorWithRed:1.0 green:1.0 blue:0.8 alpha:1.0] set];
	[[UIColor whiteColor] set];
	CGContextFillRect(context, rect);
	
	CGContextScaleCTM(context, 1, -1);
	CGContextTranslateCTM(context, 0, -rect.size.height);
	
	page = CGPDFDocumentGetPage(pdfRef, 1);
	
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


@end
