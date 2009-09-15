#ifndef __OUC_SXEQ_HH__
#define __OUC_SXEQ_HH__
/******************************************************************************/
/*                                                                            */
/*                         X r d O u c S x e q . h h                          */
/*                                                                            */
/* (c) 2009 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
//         $Id$

class XrdOucSxeq
{
public:

static const int noWait = 0x0001;
static const int Share  = 0x0002;
static const int Unlink = 0x0004;

int    Release();

int    Serialize(int Opts=0);

int    lastError() {return lokRC;}

       XrdOucSxeq(const char *sfx, const char *sfx1=0, const char *Dir="/tmp/");
      ~XrdOucSxeq();

private:

char *lokFN;
int   lokFD;
int   lokUL;
int   lokRC;
};
#endif
