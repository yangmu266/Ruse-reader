// UTF8.cc
// Modified by Guangda Hu, 2011
// Derived from Poppler project
//========================================================================
//
// UTF8.h
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2008 Koji Otani <sho@bbr.jp>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include "config.h"
#include "UTF8.h"


Unicode UTF8toUnicode(const char *buf, int bufSize, int &curPos) {
	if (curPos >= bufSize) {
		/* error */
		return 0;
	}

	Unicode rtn;
	int nBytes;
	if ((buf[curPos] & 0x80) == 0) {
		rtn = (Unicode)(buf[curPos] & 0x7f);
		nBytes = 1;
	} else if ((buf[curPos] & 0x40) == 0) {
		/* error */
		rtn = (Unicode)(buf[curPos] & 0x3f);
		nBytes = -1;
	} else if ((buf[curPos] & 0x20) == 0) {
		rtn = (Unicode)(buf[curPos] & 0x1f);
		nBytes = 2;
	} else if ((buf[curPos] & 0x10) == 0) {
		rtn = (Unicode)(buf[curPos] & 0x0f);
		nBytes = 3;
	} else if ((buf[curPos] & 0x08) == 0) {
		rtn = (Unicode)(buf[curPos] & 0x07);
		nBytes = 4;
	}
	/* below are higher than 0x10ffff, not RFC 3629 */
	else if ((buf[curPos] & 0x04) == 0) {
		rtn = (Unicode)(buf[curPos] & 0x03);
		nBytes = 5;
	} else if ((buf[curPos] & 0x02) == 0) {
		rtn = (Unicode)(buf[curPos] & 0x01);
		nBytes = 6;
	}

	int i;
	for (i = curPos + 1; i < bufSize; ++i) {
		if ((buf[i] & 0x80) == 1 && (buf[i] & 0x40) == 0)
			rtn = (rtn << 6) | (buf[i] & 0x3f);
		else
			break;
	}

	if (i != curPos + nBytes)
		/* error */ ;
	curPos = i;
	return rtn;
}


int mapUTF8(Unicode u, char *buf, int bufSize) {
	if (u <= 0x0000007f) {
		if (bufSize < 1) {
			return 0;
		}
		buf[0] = (char)u;
		return 1;
	} else if (u <= 0x000007ff) {
		if (bufSize < 2) {
			return 0;
		}
		buf[0] = (char)(0xc0 + (u >> 6));
		buf[1] = (char)(0x80 + (u & 0x3f));
		return 2;
	} else if (u <= 0x0000ffff) {
		if (bufSize < 3) {
			return 0;
		}
		buf[0] = (char)(0xe0 + (u >> 12));
		buf[1] = (char)(0x80 + ((u >> 6) & 0x3f));
		buf[2] = (char)(0x80 + (u & 0x3f));
		return 3;
	} else if (u <= 0x0010ffff) {
		if (bufSize < 4) {
			return 0;
		}
		buf[0] = (char)(0xf0 + (u >> 18));
		buf[1] = (char)(0x80 + ((u >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((u >> 6) & 0x3f));
		buf[3] = (char)(0x80 + (u & 0x3f));
		return 4;
	} else {
		return 0;
	}
}

int mapUCS2(Unicode u, char *buf, int bufSize) {
	if (u <= 0xffff) {
		if (bufSize < 2) {
			return 0;
		}
		buf[0] = (char)((u >> 8) & 0xff);
		buf[1] = (char)(u & 0xff);
		return 2;
	} else if (u < 0x110000) {
		Unicode uu;
		
		/* using surrogate pair */
		if (bufSize < 4) {
			return 0;
		}
		uu = ((u - 0x10000) >> 10) + 0xd800;
		buf[0] = (char)((uu >> 8) & 0xff);
		buf[1] = (char)(uu & 0xff);
		uu = (u & 0x3ff)+0xdc00;
		buf[2] = (char)((uu >> 8) & 0xff);
		buf[3] = (char)(uu & 0xff);
		return 4;
	} else {
		return 0;
	}
}
