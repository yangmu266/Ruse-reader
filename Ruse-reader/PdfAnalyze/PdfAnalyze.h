/*
 *  PdfAnalyze.h
 *  PaperReader
 *
 *  Created by Guangda Hu on 11-4-20.
 *  Copyright 2011 Ruse. All rights reserved.
 *
 */

#ifndef PDFANALYZE_H
#define PDFANALYZE_H

#include <cstdlib>
#include "TextOutputDev.h"
#include "PDFDoc.h"
#include "GooString.h"
#include "CharTypes.h"
#include "UnicodeMap.h"
#include "Dict.h"
#include "GooList.h"
#include "GlobalParams.h"

class PdfAnalyze;

struct ReferenceItem {
	int index;
	GooString name;
	GooString ref;
};

class LineIterator {
public:

	LineIterator(const TextPage *page = NULL) {
		reset(page);
	}

	LineIterator(const LineIterator & lineIter) {
		thisLine = lineIter.thisLine;
		thisBlk = lineIter.thisBlk;
		thisFlow = lineIter.thisFlow;
	}

	TextLine * operator*() const {
		return thisLine;
	}

	void reset(const TextPage *page) {
		if (page) {
			thisFlow = page->flows;
			thisBlk = thisFlow->blocks;
			thisLine = thisBlk->lines;
		} else {
			thisFlow = NULL;
			thisBlk = NULL;
			thisLine = NULL;
		}
	}

	LineIterator & operator++() {
		if (thisLine == NULL) return *this;
		if (thisLine->next) {
			thisLine = thisLine->next;
		} else if (thisBlk->next) {
			thisBlk = thisBlk->next;
			thisLine = thisBlk->lines;
		} else if (thisFlow->next) {
			thisFlow = thisFlow->next;
			thisBlk = thisFlow->blocks;
			thisLine = thisBlk->lines;
		} else {
			thisFlow = NULL;
			thisBlk = NULL;
			thisLine = NULL;
		}
		return *this;
	}

private:
	TextLine *thisLine;
	TextBlock *thisBlk;
	TextFlow *thisFlow;
};

class WordIterator {
public:

	WordIterator(const TextPage *page) : thisWord(NULL), lineIter(page) {
		if (*lineIter)
			thisWord = (*lineIter)->words;
	}

	WordIterator(const WordIterator &wordIter) : lineIter(wordIter.lineIter) {
		thisWord = wordIter.thisWord;
	}

	TextWord * operator*() const {
		return thisWord;
	}

	WordIterator & operator++() {
		if (thisWord == NULL) return *this;
		if (thisWord->next) {
			thisWord = thisWord->next;
		} else {
			++lineIter;
			if (*lineIter)
				thisWord = (*lineIter)->words;
			else
				thisWord = NULL;
		}
		return *this;
	}
private:
	TextWord *thisWord;
	LineIterator lineIter;
};

#define PDFANALYZE_RESOLUTION 72
#define PDFANALYZE_CACHE_SIZE 8

class PdfAnalyze {
public:

	/*
	 * Construct Function.
	 * filename: PDF file to open.
	 * ownerPW: owner password.
	 * userPW: user password.
	 */
	PdfAnalyze(const char *filename, const char *ownerPW = NULL, const char *userPW = NULL);
	
	/*
	 * Set encoding. No need to call this if use "UTF-8".
	 * Available encodings are "Latin1", "ASCII7", "Symbol", "ZapfDingbats", "UTF-8" and "UCS-2".
	 */
	void setEncoding(const char *encName) {
		globalParams->setTextEncoding(encName);
		if (uMap) uMap->decRefCnt();
		uMap = globalParams->getTextEncoding();
	}

	/*
	 * Set end of line. Candidates are "unix" (LF, default), "dos" (CR + LF), "mac" (CR).
	 */
	void setEOL(const char *eolName) {
		globalParams->setTextEOL(eolName);
	}

	/*
	 * Return the number of pages.
	 */
	int getNumPages() {
		return numPages;
	}

	/*
	 * Query PDF information dict.
	 * key: can be "Title", "Subject", "Keywords", "Author", "Creator", "Producer", "CreationData" or "LastModifiedData".
	 * If the dict is not usable, or key is not in dict, NULL is returned.
	 * For date information, the return string may start with "D:".
	 */
	const char * pdfInfo(const char *key);

	/*
	 * Query a word, and other information by provding non-NULL pointers.
	 * By setting ref = true to try to return a reference item if the word is [alnum], if no such reference, return the word as normally.
	 * The return string is normalized to NKFC (ready for search).
	 * See http://en.wikipedia.org/wiki/Unicode_equivalence#Normal_forms for full definition.
	 * NULL is returned if no word there.
	 */
	const char * wordQuery(int page, double x, double y, bool ref = true, double *xMin = NULL, double *yMin = NULL,
			double *xMax = NULL, double *yMax = NULL, const TextWord ** pWord = NULL);
	
	/*
	 * Query text in a rectangle, can be multiple line.
	 * Not normlized, there can be ligatures.
	 */
	const char * textQuery(int page, double xMin, double yMin, double xMax, double yMax);

	/*
	 * Search text in a page.
	 * 
	 * Start & End:
	 * If <startAtTop> is true, starts looking at the top of the page;
	 * else if <startAtLast> is true, starts looking immediately after the last find result;
	 * else starts looking at <xMin>,<yMin>.
	 * If <stopAtBottom> is true, stops looking at the bottom of the page;
	 * else if <stopAtLast> is true, stops looking just before the last find result;
	 * else stops looking at <xMax>,<yMax>.
	 * 
	 * The string will be copied and automatically NKFC-normalized.
	 * 
	 ***** Need to check if this supports finding string of multiple lines, because it returns a rectangle.
	 ***** Do we need another version that supports finding text across two lines/pages?
	 */
	bool findText(int page, const char *s, bool startAtTop, bool stopAtBottom, bool startAtLast, bool stopAtLast,
			bool caseSensitive, bool backward, double *xMin, double *yMin, double *xMax, double *yMax);
	
	/*
	 * Get the number of references.
	 */
	int getNumRef() {
		return refList->getLength();
	}

	/*
	 * Get reference by index (start from 1), return NULL if no such reference.
	 */
	const char * getRef(int index) {
		return (index >= 1 && refList && index <= refList->getLength()) ?
				((const ReferenceItem *) refList->get(index - 1))->ref.getCString() : NULL;
	}

	/*
	 * Get reference by "1" or "name".
	 * Please do not include "[]" or "." or other symbols.
	 * Return NULL if no such reference.
	 */
	const char * getRef(const char * name);

	~PdfAnalyze();

private:

	TextPage * touchPage(int page);

	bool checkCharNum(TextWord *word);
	bool checkCharRef(TextWord *word);
	int checkRefItem(TextLine *line);

	void genReference();

	UnicodeMap *uMap;

	PDFDoc *doc;

	Object info;
	Dict *infoDict;
	GooString lastDict;

	int numPages;
	TextPage **pages;

	int cacheCnt[PDFANALYZE_CACHE_SIZE];
	const TextWord *wordCache[PDFANALYZE_CACHE_SIZE];
	int wordCachePage[PDFANALYZE_CACHE_SIZE];
	GooString queryCache[PDFANALYZE_CACHE_SIZE];
	const char *refCache[PDFANALYZE_CACHE_SIZE];

	GooString *lastText;

	GooList *refList;
	int refType;
	ReferenceItem *lastRef;
};

#endif      /* PDFANALYZE_H */
