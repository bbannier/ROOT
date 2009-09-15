#ifndef _OSS_PATH_H
#define _OSS_PATH_H
/******************************************************************************/
/*                                                                            */
/*                         X r d O s s P a t h . h h                          */
/*                                                                            */
/* (c) 2008 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

//         $Id$

#include <string.h>

class XrdOssPath
{
public:

struct fnInfo
{
const char *Path;
const char *Sfx; // Of length sfxLen!
      char *Slash;
      int   Plen;
};

static const char xChar  = '%';

static const int  sfxLen = 4;

static int   Convert(char *dst, int dln, const char *oldP, const char *newP);

static char *genPath(const char *inPath, const char *cgrp, char *sfx);

static char *genPFN(fnInfo &Info, char *buff, int blen, const char *Path=0);

static char *genPFN(char *dst, int dln, const char *src);

static int   getCname(const char *path, char *Cache, char *lbuf=0, int lbsz=0);

static
inline int   isXA(const char *path) {return path[strlen(path)-1] == xChar;}

enum theSfx {isBase =0, isAnew =1,
             sfxMigF=1, isFail=2,  isLock =3, isPin =4, isStage=5, sfxMigL=5,
             sfxMemF=5, isMkeep=6, isMlock=7, isMmap=8,            sfxMemL=8,
             isPfn=9,   sfxLast=9, sfxNum =10
            };

static
const char *Sfx[sfxNum];

static const int chkMem = 0x01;
static const int chkMig = 0x02;
static const int chkPfn = 0x04;
static const int chkAll = 0x07;

static theSfx pathType(const char *Path, int chkWhat=chkAll);

static void  Trim2Base(char *eP);

             XrdOssPath() {}
            ~XrdOssPath() {}

private:
static char *bin2hex(char *inbuff, int dlen, char *buff);
static int   Init(char *pfnPfx);

static char h2c[16];
};
#endif
