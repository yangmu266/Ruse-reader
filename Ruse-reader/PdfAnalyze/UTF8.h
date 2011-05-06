// UTF8.h
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

#ifndef UTF8_H
#define UTF8_H

#include "config.h"
#include "CharTypes.h"

int mapUTF8(Unicode u, char *buf, int bufSize);

Unicode UTF8toUnicode(const char *buf, int bufSize, int &curPos);

int mapUCS2(Unicode u, char *buf, int bufSize);


#endif       /* UTF8_H */
