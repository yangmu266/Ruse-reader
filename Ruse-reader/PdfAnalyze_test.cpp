#include <cstdio>
#include "PdfAnalyze/PdfAnalyze.h"

int main() {
	PdfAnalyze pdfAnalyze("/home/tarlou/wdcircuit.pdf");

	//// Output all words and their bounding boxes
	/*int n = pdfAnalyze.getNumPages();
	for (int i = 1; i <= n; i = n + 1) {
		WordIterator wordIter(pdfAnalyze.touchPage(i));
		for (; *wordIter; ++wordIter) {
			double xMin, yMin, xMax, yMax;
			(*wordIter)->getBBox(&xMin, &yMin, &xMax, &yMax);
			printf("%s %lf %lf %lf %lf\n", (*wordIter)->getText(), xMin, yMin, xMax, yMax);
		}
	}*/
	
	double xMin, yMin, xMax, yMax;
	pdfAnalyze.findText(1, "[1]", true, true, false, false, true, false, &xMin, &yMin, &xMax, &yMax);
	
	printf("%lf %lf %lf %lf\n", xMin, yMin, xMax, yMax);

	//// Query words
	for (int i = 0; i < 10; ++i) {
		int page;
		double x, y;
		scanf("%d %lf %lf", &page, &x, &y);
		const char * result = pdfAnalyze.wordQuery(page, x, y);
		printf("%s\n", result ? result: "NULL");
	}

	//// Parse Reference
	int n = pdfAnalyze.getNumRef();
	for (int i = 1; i <= n; ++i) {
		const char *pRef = pdfAnalyze.getRef(i);
		printf("%d: %s\n", i, pRef ? pRef : "NULL");
	}
	
	for (int i = 0; i < 10; ++i) {
		char str[100];
		scanf("%s", str);
		const char *pRef = pdfAnalyze.getRef(str);
		printf("%s\n", pRef ? pRef : "NULL");
	}
	return 0;
}
