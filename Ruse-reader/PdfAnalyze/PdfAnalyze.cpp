/*
 *  PdfAnalyze.cpp
 *  PaperReader
 *
 *  Created by Guangda Hu on 11-4-22.
 *  Copyright 2011 Ruse. All rights reserved.
 *
 */

#include "GlobalParams.h"
#include "PDFDocFactory.h"
#include "UTF8.h"
#include "UnicodeTypeTable.h"
#include "GooString.h"
#include "GooList.h"
#include "PdfAnalyze.h"
#include <ctype.h>

// If text contain only space, do nothing and return 0.
// Otherwise, append text to str after skipping leading spaces, and return 1.
static inline int appUnicode(GooString &str, int pos, int end, Unicode *text, UnicodeMap *uMap) {
	while (pos < end && isspace(text[pos])) ++pos;
	if (pos >= end) return 0;
	char buf[8];
	if (uMap)
		for (; pos < end; ++pos) {
			int n = uMap->mapUnicode(text[pos], buf, sizeof (buf));
			str.append(buf, n);
		}
	return 1;
}

PdfAnalyze::PdfAnalyze(const char *fileName, const char *ownerPW, const char *userPW) {
	globalParams = new GlobalParams();
	uMap = globalParams->getTextEncoding();
	infoDict = NULL;
	numPages = 0;
	pages = NULL;
	lastText = NULL;
	refList = new GooList();
	for (int i = 0; i < PDFANALYZE_CACHE_SIZE; ++i) {
		wordCache[i] = NULL;
		cacheCnt[i] = 0;
		refCache[i] = NULL;
	}

	GooString gooFileName(fileName), *ownerPassword = NULL, *userPassword = NULL;
	if (ownerPW != NULL)
		ownerPassword = new GooString(ownerPW);
	if (userPW != NULL)
		userPassword = new GooString(userPW);
	doc = PDFDocFactory().createPDFDoc(gooFileName, ownerPassword, userPassword);
	if (ownerPassword) delete ownerPassword;
	if (userPassword) delete userPassword;
	if (!doc->isOk()) {
		delete doc;
		doc = NULL;
		return;
	}

	doc->getDocInfo(&info);
	if (info.isDict())
		infoDict = info.getDict();

	numPages = doc->getNumPages();
	pages = new TextPage * [numPages];
	for (int i = 0; i < numPages; ++i)
		pages[i] = NULL;

	refType = -1;
	lastRef = NULL;
	genReference();
}

const char * PdfAnalyze::pdfInfo(const char *key) {
	if (infoDict == NULL) return NULL;
	lastDict.clear();
	Object obj;
	if (infoDict->lookup(key, &obj)->isString()) {
		GooString *s = obj.getString();
		if ((s->getChar(0) & 0xff) == 0xfe && (s->getChar(1) & 0xff) == 0xff) {
			Unicode u;
			char buf[8];
			int n;
			for (int i = 2; i < s->getLength(); i += 2) {
				u = ((s->getChar(i) & 0xff) << 8) | (s->getChar(i) & 0xff);
				n = uMap->mapUnicode(u, buf, sizeof (buf));
				lastDict.append(buf, n);
			}
		} else
			lastDict.append(s);
		obj.free();
		return lastDict.getCString();
	} else {
		obj.free();
		return NULL;
	}
}

TextPage * PdfAnalyze::touchPage(int page) {
	if (doc && page >= 1 && page <= numPages) {
		if (pages[page - 1] == NULL) {
			TextOutputDev *textOut = new TextOutputDev(NULL, gTrue, gFalse, gFalse);
			if (!textOut->isOk()) return NULL;
			doc->displayPage(textOut, page, PDFANALYZE_RESOLUTION, PDFANALYZE_RESOLUTION, 0, gFalse, gFalse, gFalse);
			pages[page - 1] = textOut->takeText();
			delete textOut;
		}
		return pages[page - 1];
	}
	return NULL;
}

const char * PdfAnalyze::wordQuery(int page, double x, double y, bool ref, double *xMin, double *yMin,
		double *xMax, double *yMax, const TextWord ** pWord) {
	TextPage *curPage = touchPage(page);
	if (curPage == NULL) return NULL;
	x *= curPage->pageWidth;
	y *= curPage->pageHeight;
	for (int i = 0; i < PDFANALYZE_CACHE_SIZE; ++i)
		if (cacheCnt[i] > 0 && wordCache[i]->xMin <= x && wordCache[i]->xMax >= x &&
				wordCache[i]->yMin <= y && wordCache[i]->yMax >= y && wordCachePage[i] == page) {
			if (xMin) *xMin = wordCache[i]->xMin / curPage->pageWidth;
			if (yMin) *yMin = wordCache[i]->yMin / curPage->pageHeight;
			if (xMax) *xMax = wordCache[i]->xMax / curPage->pageWidth;
			if (yMax) *yMax = wordCache[i]->yMax / curPage->pageHeight;
			if (pWord) *pWord = wordCache[i];
			++cacheCnt[i];
			if (refCache[i] && ref) return refCache[i];
			else return queryCache[i].getCString();
		}

	for (WordIterator wordIter(curPage); *wordIter; ++wordIter)
		if ((*wordIter)->xMin <= x && (*wordIter)->xMax >= x && (*wordIter)->yMin <= y && (*wordIter)->yMax >= y) {
			if (xMin) *xMin = (*wordIter)->xMin / curPage->pageWidth;
			if (yMin) *yMin = (*wordIter)->yMin / curPage->pageHeight;
			if (xMax) *xMax = (*wordIter)->xMax / curPage->pageWidth;
			if (yMax) *yMax = (*wordIter)->yMax / curPage->pageHeight;
			if (pWord) *pWord = *wordIter;
			int minCnt = cacheCnt[0], r = 0;
			for (int i = 1; i < PDFANALYZE_CACHE_SIZE; ++i)
				if (cacheCnt[i] < minCnt) {
					minCnt = cacheCnt[i];
					r = i;
				}
			cacheCnt[r] = 1;
			wordCache[r] = *wordIter;
			wordCachePage[r] = page;
			int normLen;
			Unicode *norm = unicodeNormalizeNFKC((*wordIter)->text, (*wordIter)->len, &normLen, NULL);
			queryCache[r].clear();
			appUnicode(queryCache[r], 0, normLen, norm, uMap);
			gfree(norm);
			if (queryCache[r].getChar(0) == '[' && queryCache[r].getChar(queryCache[r].getLength() - 1) == ']') {
				queryCache[r].setChar(queryCache[r].getLength() - 1, '\0');
				refCache[r] = getRef(queryCache[r].getCString() + 1);
				queryCache[r].setChar(queryCache[r].getLength() - 1, ']');
			}
			else refCache[r] = NULL;
			if (refCache[r] && ref) return refCache[r];
			else return queryCache[r].getCString();
		}
	return NULL;
}

const char * PdfAnalyze::textQuery(int page, double xMin, double yMin, double xMax, double yMax) {
	TextPage * curPage = touchPage(page);
	if (curPage == NULL) return NULL;
	if (lastText) delete lastText;
	lastText = curPage->getText(xMin * curPage->pageWidth, yMin * curPage->pageHeight, 
			xMax * curPage->pageWidth, yMax * curPage->pageHeight);
	return lastText->getCString();
}

bool PdfAnalyze::findText(int page, const char *s, bool startAtTop, bool stopAtBottom,
		bool startAtLast, bool stopAtLast, bool caseSensitive, bool backward,
		double *xMin, double *yMin, double *xMax, double *yMax) {
	TextPage * curPage = touchPage(page);
	if (curPage == NULL) return false;
	int len = strlen(s);
	Unicode *us = new Unicode[len];
	int i = 0, j = 0;
	while (i < len)
		us[j++] = UTF8toUnicode(s, len, i);
	*xMin *= curPage->pageWidth;
	*yMin *= curPage->pageHeight;
	*xMax *= curPage->pageWidth;
	*yMax *= curPage->pageHeight;
	bool rtn = curPage->findText(us, j, startAtTop, stopAtBottom, startAtLast, stopAtLast,
			caseSensitive, backward, xMin, yMin, xMax, yMax);
	*xMin /= curPage->pageWidth;
	*yMin /= curPage->pageHeight;
	*xMax /= curPage->pageWidth;
	*yMax /= curPage->pageHeight;
	return rtn;
}

const char * PdfAnalyze::getRef(const char * name) {
	for (int i = 0; i < refList->getLength(); ++i) {
		const char *p = name, *q = ((ReferenceItem *) refList->get(i))->name.getCString();
		for (; *p && *q; ++p, ++q)
			if (*p != *q) break;
		if (*p == '\0' && *q == '\0')
			return ((ReferenceItem *) refList->get(i))->ref.getCString();
	}
	return NULL;
}

bool PdfAnalyze::checkCharNum(TextWord *word) {
	// Unicode digits are also '0', '1', ..., '9'.
	int i;
	for (i = 0; i < word->len; ++i)
		if (!isdigit(word->text[i])) break;
	if (i == word->len || (i == word->len - 1 && word->text[i] == '.')) return true;
	return false;
}

bool PdfAnalyze::checkCharRef(TextWord *word) {
	// Allows "reference", "refereces", "refrence", "refrences", capital or litte letters.
	// Unicode letters are also 'a' ~ 'z' and 'A' ~ 'Z'.
	const char * charRef1 = "ref", * charRef2 = "rence";
	int i, j;
	for (i = 0; charRef1[i] && i < word->len; ++i)
		if (word->text[i] != charRef1[i] && word->text[i] != (charRef1[i] + 'A' - 'a'))
			return false;
	if (charRef1[i]) return false;
	if (i < word->len && (word->text[i] == 'e' || word->text[i] == 'E')) ++i;
	for (j = 0; charRef2[j] && i < word->len; ++i, ++j)
		if (word->text[i] != charRef2[j] && word->text[i] != (charRef2[j] + 'A' - 'a'))
			return false;
	if (charRef2[j]) return false;
	if (i == word->len || (i == word->len - 1 && (word->text[i] == 's' || word->text[i] == 'S')))
		return true;
	return false;
}

int PdfAnalyze::checkRefItem(TextLine *line) {
	if (!line->normalized)
		line->normalized = unicodeNormalizeNFKC(line->text, line->len, &line->normalized_len, &line->normalized_idx);
	int i = 0;
	while (i < line->normalized_len && (isspace(line->normalized[i]))) ++i;
	if (i >= line->normalized_len) return 0;
	if (line->normalized[i] == '[') {
		if (++i >= line->normalized_len) return 0;
		if (isdigit(line->normalized[i])) {
			if (refType >= 0 && refType != 0) return 0;
			int index = 0, begin = i, end;
			do index = index * 10 + (line->normalized[i++] - '0'); while (i < line->normalized_len && isdigit(line->normalized[i]));
			if (i >= line->normalized_len || line->normalized[i] != ']') return 0;
			end = i;
			if ((lastRef == NULL && index != 1) || (lastRef && index != lastRef->index + 1))
				return 0;
			refType = 0;
			lastRef = new ReferenceItem;
			refList->append(lastRef);
			lastRef->index = index;
			appUnicode(lastRef->name, begin, end, line->normalized, uMap);
			return 2 - appUnicode(lastRef->ref, end + 1, line->normalized_len, line->normalized, uMap);
		} else if (isalpha(line->normalized[i])) {
			if (refType >= 0 && refType != 1) return 0;
			int begin = i, end;
			do ++i; while (i < line->normalized_len && line->normalized[i] != ']');
			if (i >= line->normalized_len) return 0;
			end = i;
			ReferenceItem *pRef = new ReferenceItem;
			appUnicode(pRef->name, begin, end, line->normalized, uMap);
			if (getRef(pRef->name.getCString())) {
				delete pRef;
				return 0;
			}
			refType = 1;
			pRef->index = lastRef ? lastRef->index + 1 : 1;
			lastRef = pRef;
			refList->append(lastRef);
			return 2 - appUnicode(lastRef->ref, end + 1, line->normalized_len, line->normalized, uMap);
		} else return 0;
	} else if (isdigit(line->normalized[i])) {
		if (refType >= 0 && refType != 2) return 0;
		int index = 0, begin = i, end;
		do index = index * 10 + (line->normalized[i++] - '0'); while (i < line->normalized_len && isdigit(line->normalized[i]));
		if (i >= line->normalized_len || line->normalized[i] != '.') return 0;
		end = i;
		if ((lastRef == NULL && index != 1) || (lastRef && index != lastRef->index + 1))
			return 0;
		refType = 2;
		lastRef = new ReferenceItem;
		refList->append(lastRef);
		lastRef->index = index;
		appUnicode(lastRef->name, begin, end, line->normalized, uMap);
		return 2 - appUnicode(lastRef->ref, end + 1, line->normalized_len, line->normalized, uMap);
	} else return 0;
}

void PdfAnalyze::genReference() {
	if (doc == NULL || numPages <= 0) return;
	LineIterator refLine;
	int i;
	for (i = numPages; i > 0; --i) {
		if (touchPage(i) == NULL) continue;
		double normFontSize = 0;
		double charNum = 0;
		for (WordIterator wordIter(pages[i - 1]); *wordIter; ++wordIter) {
			normFontSize += (*wordIter)->fontSize;
			charNum += (*wordIter)->len;
		}
		normFontSize /= charNum;
		for (refLine.reset(pages[i - 1]); *refLine; ++refLine) {
			TextWord *word = (*refLine)->words;
			if (word == NULL || word->fontSize <= normFontSize / charNum) continue;
			TextWord *nextWord = word->next;
			if ((nextWord == NULL && checkCharRef(word)) ||
					(nextWord && nextWord->next == NULL && checkCharNum(word) && checkCharRef(nextWord)))
				break;
		}
		if (*refLine) break;
	}
	int needAppend = 0;
	while (*refLine) {
		int rtn = checkRefItem(*refLine);
		if (rtn > 0) needAppend = rtn;
		else if (needAppend) {
			lastRef->ref.append(' ');
			needAppend -= appUnicode(lastRef->ref, 0, (*refLine)->normalized_len, (*refLine)->normalized, uMap);
		}
		++refLine;
		if (*refLine == NULL && i < numPages) refLine.reset(pages[i++]);
	}
}

PdfAnalyze::~PdfAnalyze() {
	if (globalParams) delete globalParams;
	if (uMap) uMap->decRefCnt();
	if (doc) delete doc;
	info.free();
	if (pages) {
		for (int i = 0; i < numPages; ++i)
			if (pages[i]) pages[i]->decRefCnt();
		delete [] pages;
	}
	if (lastText) delete lastText;
	deleteGooList(refList, ReferenceItem);
	//Object::memCheck(stderr);
	//gMemReport(stderr);
}
/*
ReferenceItem::ReferenceItem(LineIterator &lineIter) {
	GooString tmp;
	bool working = false;
	GooString *cur = &authors;
	UnicodeMap *uMap = globalParams->getTextEncoding();
	for (; *lineIter; ++lineIter) {
		if (!checkLine(*lineIter)) continue;

		if ((*lineIter)->normalized && uMap) {
			tmp.clear();
			char buf[8];
			for (int i = 0; i < (*lineIter)->normalized_len; ++i) {
				int n = uMap->mapUnicode((*lineIter)->normalized[i], buf, sizeof (buf));
				tmp.append(buf, n);
			}
		}
		char *p = tmp.getCString();
		if (!working && !checkRef(p)) continue;
		working = true;
		for (; *p; ++p) {
			if (*p != '.' || ((p[-1] >= 'A' && p[-1] <= 'Z') || (p[-1] >= 'a' && p[-1] <= 'z')) && (p[-2] == ' ' || p[-2] == '\t'))
				cur->append(*p);
			else if (cur == &authors) cur = &title;
			else {
				++lineIter;
				return;
			}
		}
	}
}
 */