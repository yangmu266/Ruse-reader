//
//  Ruse-PDFView.h
//  Ruse-reader
//
//  Created by tarlou on 11-5-3.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface Ruse_PDFView : UIView {
	CGPDFDocumentRef pdfRef;
	CGPDFPageRef page;
}

- (id)initWithFilePath:(NSString *)filePath;
- (void)reloadView;
- (CGPDFDocumentRef)createPDFFromExistFile:(NSString *)aFilePath;

@end
