#! ../cint
/* /% C++ %/ */
/***********************************************************************
 * cint (C/C++ interpreter)
 ************************************************************************
 * C++ Script testcint.cxx
 ************************************************************************
 * Description:
 *  Automatic test suite of cint
 ************************************************************************
 * Copyright(c) 2002~2004  Masaharu Goto 
 *
 * For the licensing terms see the file COPYING
 *
 ************************************************************************/
// Usage:
//  $ cint testall.cxx

#include <stdio.h>
#include "../inc/configcint.h"

#ifndef G__VISUAL // ??? fprintf crashes if stdfunc.dll is loaded ???
#include <stdlib.h>
#include <string.h>
#endif

#ifdef DEBUG2
#ifndef DEBUG
#define DEBUG
#endif
#endif

char* debug;
char* mkcintoption = "";

char* cintoption = "";
enum ELanguage {
   kLangUnknown, kLangC, kLangCXX
};

//////////////////////////////////////////////////////////////////////
// run system command
//////////////////////////////////////////////////////////////////////
int clear(const char *fname) {
  FILE *fp = fopen(fname,"w");
  fclose(fp);
}

//////////////////////////////////////////////////////////////////////
// remove a file
//////////////////////////////////////////////////////////////////////
int exist(const char *fname) {
  FILE* fp = fopen(fname,"r");
  if(fp) {
    fclose(fp);
    return(1);
  }
  else {
    return 0;
  }
}

//////////////////////////////////////////////////////////////////////
// remove a file
//////////////////////////////////////////////////////////////////////
int rm(const char* fname) {
#if 1
  int stat;
  do {
    stat = remove(fname);
  } while(exist(fname));
  return(stat);
#else
  return(remove(fname));
#endif
}

//////////////////////////////////////////////////////////////////////
// display file
//////////////////////////////////////////////////////////////////////
int cat(FILE* fout,const char *fname) {
  FILE *fp = fopen(fname,"r");
  char b1[500];
  while(fgets(b1,400,fp)) {
    fprintf(fout,"%s",b1);
  }
  fclose(fp);
}

//////////////////////////////////////////////////////////////////////
// run system command
//////////////////////////////////////////////////////////////////////
int run(const char* com) {
#ifdef DEBUG
  printf("%s\n",com);
#endif
  return(system(com));
}

//////////////////////////////////////////////////////////////////////
// check difference of 2 output files
//////////////////////////////////////////////////////////////////////
int readahead(FILE* fp,const char *b,int ahead=10) {
  int a=0,result=0;
  int i;
  char *c;
  char buf[500];
  fpos_t p;
  fgetpos(fp,&p);
  for(i=0;i<ahead;i++) {
    if(!fp) break;
    c=fgets(buf,400,fp); 
    ++a; 
    if(!c) break;
    if(strcmp(b,buf)==0) {
      result = a;
      break;
    }
  }
  fsetpos(fp,&p);
  return result;
}

void outdiff(FILE *fp,FILE *fpi,int a,char *b,int& l,const char *m) {
  int i;
  char *c;
  //fprintf(fp,"outdiff %d %d\n",a,l);
  for(i=0;i<a;i++) {
    fprintf(fp,"%3d%s %s",l,m,b);
    if(!fpi) break;
    c=fgets(b,400,fpi); 
    ++l; 
    if(!c) break;
  }
}

void checkdiff(FILE* fp,FILE* fp1,FILE* fp2,const char *b1,const char *b2
	       ,int& l1,int& l2,const char *m1,const char *m2) {
  int a1,a2;

  //fprintf(fp,"checkdiff %d %d\n",l1,l2);
  a1 = readahead(fp1,b2);
  a2 = readahead(fp2,b1);

  if(a1&&a2) {
    if(a1<=a2) outdiff(fp,fp1,a1,b1,l1,m1);
    else       outdiff(fp,fp2,a2,b2,l2,m2);
  }
  else if(a1) {
    outdiff(fp,fp1,a1,b1,l1,m1);
  }
  else if(a2) {
    outdiff(fp,fp2,a2,b2,l2,m2);
  }
  else {
    fprintf(fp,"%3d%s %s",l1,m1,b1);
    fprintf(fp,"%3d%s %s",l2,m2,b2);
  }
}

int diff(const char *title,const char *f1,const char *f2,const char *out
	 ,const char *macro="",const char *m1=">",const char *m2="<") {
  FILE *fp = fopen(out,"a");
  FILE *fp1= fopen(f1,"r");
  FILE *fp2= fopen(f2,"r");
  char b1[500];
  char b2[500];
  char *c1;
  char *c2;
  int l1=0;
  int l2=0;

  fprintf(fp,"%s %s\n",title,macro);

  for(;;) {
    if(fp1) { c1=fgets(b1,400,fp1); ++l1; } else c1=0;
    if(fp2) { c2=fgets(b2,400,fp2); ++l2; } else c2=0;
    if(c1&&c2) {
      if(strcmp(b1,b2)) {
#ifndef G__VISUAL
	checkdiff(fp,fp1,fp2,b1,b2,l1,l2,m1,m2);
#else
	fprintf(fp,"%3d%s %s",l1,m1,b1);
	fprintf(fp,"%3d%s %s",l2,m2,b2);
#endif
      }
    }
    else if(c1) {
      fprintf(fp,"%3d%s %s",l1,m1,b1);
    }
    else if(c2) {
      fprintf(fp,"%3d%s %s",l2,m2,b2);
    }
    else {
      break;
    }
  }

  if(fp2) fclose(fp2);
  if(fp1) fclose(fp1);
  if(fp)  fclose(fp);
}

int ediff(const char *title, const char *macro, const char *dfile, const char *compiled = "compiled") {

  FILE *fp = fopen(dfile,"a");
  fprintf(fp,"%s %s\n",title,macro);
  fclose(fp);

  char com[4000]; 
  sprintf(com,"\\diff --old-group-format=\"%s %s:%%c'\\012'%%<\" --new-group-format=\"%s interpreted:%%c'\\012'%%>\" --unchanged-line-format=\"\" --old-line-format=\" %%3dn: %%L\" --new-line-format=\" %%3dn: %%L\" %s interpreted>> %s", title, compiled, title, compiled, dfile);
  return run(com);

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// compare compiled and interpreted result
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int ci(ELanguage lang, const char *sname, const char *dfile, 
        const char *cflags="", const char *exsname="", const char *cintopt="") {
  if(debug && strcmp(debug,sname)) return;

  printf("%s %s %s\n",sname,cflags,exsname);

  char exename[200];
  strcpy(exename,sname);
  char* posExt=strrchr(exename,'.');
  if (posExt)
    strcpy(posExt,".exe");

  // compile source
  const char* comp = 0;
  const char* flags = 0;
  const char* macros = 0;
  const char* ldflags = 0;
  const char* link = "";
  if (lang == kLangC) {
     comp = G__CFG_CC;
     flags = G__CFG_CFLAGS;
     ldflags = G__CFG_LDFLAGS;
     macros = G__CFG_CMACROS;
  } else if (lang == kLangCXX) {
     comp = G__CFG_CXX;
     flags = G__CFG_CXXFLAGS;
     ldflags = G__CFG_LDFLAGS;
     macros = G__CFG_CXXMACROS;
  } else {
     printf("ERROR in ci: language is not set!\n");
     return;
  }
  #if defined(G__WIN32)
  link = "/link";
  #endif
  char com[4000]; 
  sprintf(com,"%s -Dcompiled %s %s %s %s %s %s%s %s %s",  comp, cflags, flags, macros,
     sname, exsname, G__CFG_COUTEXE, exename, link, ldflags );
  run(com);

  // run compiled program
#if defined(G__WIN32)
  sprintf(com, ".\\%s > compiled", exename);
#else
  sprintf(com, "./%s > compiled", exename);
#endif
  run(com);
#ifdef DEBUG2
  run(exename);
#endif

#ifndef DEBUG
  rm(exename);
#endif

#if defined(G__WIN32) || defined(G__CYGWIN)
  if (posExt)
    strcpy(posExt, ".obj");
  rm(exename);
#endif
#ifdef G__BORLAND
  if (posExt)
    strcpy(posExt, ".tds");
  rm(exename);
#endif

  // run interpreted program
  sprintf(com, "cint %s -Dinterp %s %s %s %s > interpreted", cintoption, cintopt, cflags, exsname, sname);
  run(com);

  //diff(sname,"compiled","interpreted",dfile,cflags,"c","i");
  //sprintf(com,"\\diff --old-group-format=\"%s compiled:%%c'\\012'%%<\" --new-group-format=\"%s interpreted:%%c'\\012'%%>\" --unchanged-line-format=\"\" --old-line-format=\" %%3dn: %%L\" --new-line-format=\" %%3dn: %%L\" compiled interpreted>> %s", sname, sname, dfile);
  //int ret = run(com);
  int ret = ediff(sname,cflags,dfile);
  
  //for(int i=0;i<100000;i++) ; // wait for a while
  //\diff  --line-format=' %3dn: %L' --old-group-format="t1134.cxx interpreted:%c'\012'%<" --new-group-format="t1134.cxx compiled:%c'\012'%>" --unchanged-group-format=""  interpreted compiled
#ifndef DEBUG
  rm("compiled");
  rm("interpreted");
#endif
  return ret;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// compare compiled and interpreted result
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int io(const char *sname,const char* old ,const char *dfile
	,const char *macro=""){

  if(debug && strcmp(debug,sname)) return;

  printf("%s\n",sname);

  // run interpreted program
  char com[500];
  sprintf(com,"cint %s %s %s > interpreted",cintoption,macro,sname);
  run(com);

  int ret = ediff(sname,"",dfile,old);
  //diff(sname,old,"interpreted",dfile,"","o","i");

  rm("interpreted");
  return ret;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// make sure that dictionary can be compiled
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void mkc(ELanguage lang,const char *sname,const char *dfile
	 ,const char *macro="",const char *src="") {

  if(debug && strcmp(debug,sname)) return;

  printf("%s\n",sname);

  // run interpreted program

  char com[500];
  sprintf(com,"makecint -mk Makefile %s -dl test.dll %s -H %s %s"
	  ,mkcintoption,macro,sname,src);
  run(com);

  sprintf(com,"make -f Makefile");
  run(com);
  sprintf(com,"-DHNAME=\\\"%s\\\" -DDNAME=\\\"test.dll\\\"",sname);
  ci(lang,"mkcmain.cxx",dfile,com);

#ifndef DEBUG
  run("make -f Makefile clean");
#endif
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// make sure that dictionary can be compiled and program runs
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int mkco(ELanguage lang,const char *sname,const char *hname
	  ,const char *old
	  ,const char *dfile,const char *macro="",const char *src=""
	  ,const char *cintopt=""){

  if(debug && strcmp(debug,sname)) return;

  printf("%s\n",sname);

  // run interpreted program

  char com[500];
  if(lang == kLangCXX) {
    sprintf(com,"makecint -mk Makefile %s -dl test.dll %s %s -H %s %s"
	    ,mkcintoption,cintopt,macro,hname,src);
  }
  else {
    sprintf(com,"makecint -mk Makefile %s -dl test.dll %s %s -h %s %s"
	    ,mkcintoption,cintopt,macro,hname,src);
  }
  run(com);

  sprintf(com,"make -f Makefile");
  run(com);

  char imacro[500];
  sprintf(imacro,"%s -Dmakecint",macro);
  int ret = io(sname,old,dfile,imacro);

#ifndef DEBUG
  run("make -f Makefile clean");
#endif
  return ret;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// make sure that dictionary can be compiled and program runs
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int mkci(ELanguage lang,const char *sname,const char *hname
	  ,const char *dfile,const char *macro="",const char *src=""
	  ,const char *cintopt=""){

  if(debug && strcmp(debug,sname)) return 0;

  printf("%s\n",sname);

  // run interpreted program

  char com[500];
  if(lang==kLangCXX) {
    sprintf(com,"makecint -mk Makefile %s -dl test.dll %s %s -H %s %s"
	    ,mkcintoption,cintopt,macro,hname,src);
  }
  else {
    sprintf(com,"makecint -mk Makefile %s -dl test.dll %s %s -h %s %s"
	    ,mkcintoption,cintopt,macro,hname,src);
  }
  run(com);

  sprintf(com,"make -f Makefile");
  run(com);

  char imacro[500];
  sprintf(imacro,"%s -Dmakecint",macro);
  int ret = ci(lang,sname,dfile,imacro,"",cintopt);

#ifndef DEBUG
  run("make -f Makefile clean");
#endif
  return ret;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// make sure that dictionary can be compiled and program runs
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int mkciN(ELanguage lang,const char *sname
	   ,const char *hname1,const char *dfile
	   ,const char *macro=""
	   ,const char *hname2="",const char *hname3=""){

  if(debug && strcmp(debug,sname)) return 0;

  printf("%s\n",sname);

  // run interpreted program

  char com[500];
  sprintf(com,"makecint -mk Makefile1 %s -dl test1.dll %s -H %s"
	  ,mkcintoption,macro,hname1);
  run(com);

  sprintf(com,"make -f Makefile1");
  run(com);

  if(hname2[0]) {
    sprintf(com,"makecint -mk Makefile2 %s -dl test2.dll %s -H %s"
	    ,mkcintoption,macro,hname2);
    run(com);
    sprintf(com,"make -f Makefile2");
    run(com);
  }

  if(hname3[0]) {
    sprintf(com,"makecint -mk Makefile3 %s -dl test3.dll %s -H %s"
	    ,mkcintoption,macro,hname3);
    run(com);
    sprintf(com,"make -f Makefile3");
    run(com);
  }

  char imacro[500];
  sprintf(imacro,"%s -Dmakecint2",macro);
  int ret = ci(lang,sname,dfile,imacro);

#ifndef DEBUG
  run("make -f Makefile1 clean");
  rm("Makefile1");
  if(hname2[0]) {
    run("make -f Makefile2 clean");
    rm("Makefile2");
  }
  if(hname3[0]) {
    run("make -f Makefile3 clean");
    rm("Makefile3");
  }
#endif

  return ret;
}

//////////////////////////////////////////////////////////////////////
// test series of files with enumerated suffix
//////////////////////////////////////////////////////////////////////
int testn(ELanguage lang,const char *hdr,int *num,const char *ext
	  ,const char *dfile,const char *macro="") {


  char sname[100];

  int i=0;
  while(-1!=num[i]) {
    sprintf(sname,"%s%d%s",hdr,num[i],ext);
    ci(lang,sname,dfile,macro);
    ++i;
  }
}


//////////////////////////////////////////////////////////////////////
// runt
//////////////////////////////////////////////////////////////////////
int runt(const char *dfile) {
  char basename[20];
  for(int i=1;i<1000;i++) {
    sprintf(basename,"t%d",i);
  }
}

// void stopthis(int x=0) { exit(0); } // did not work

//////////////////////////////////////////////////////////////////////
// testsuite main program
//////////////////////////////////////////////////////////////////////
int main(int argc,char** argv) {

  char *difffile="testdiff.txt";

  //signal(SIGINT,stopthis);

  int ret = 0;
  int i;
  for(i=1;i<argc;i++) {
    if(strcmp("-d",argv[i])==0 && !strstr(argv[i+1],".cxx")) 
      difffile=argv[++i];
    else if(strcmp("-c",argv[i])==0 && !strstr(argv[i+1],".cxx"))
      cintoption = argv[++i];
    else if(strcmp("-m",argv[i])==0 && !strstr(argv[i+1],".cxx"))
      mkcintoption = argv[++i];
    else if(strcmp("-?",argv[i])==0) {
      fprintf(stderr,"%s <-d [difffile]> <-c [cintoption]> <-m [makecintoption]> <[testcase.cxx]>\n",argv[0]);
      return 0;
    }
    else debug = argv[i];
  }

  clear(difffile);
  remove("test.dll");
  remove("test1.dll");
  remove("test2.dll");
  remove("test3.dll");
  remove("G__cpp_test.cxx");
  remove("G__cpp_test.h");

#ifndef NEWTEST
  int cpp[] = {0, 1, 2, 3, 4, 5, 6, 8, -1};
  testn(kLangCXX,"cpp",cpp,".cxx",difffile);

  ret += ci(kLangCXX,"refassign.cxx",difffile);
  ret += ci(kLangCXX,"ostream.cxx",difffile);    // cout << pointer
  ret += ci(kLangCXX,"setw0.cxx",difffile);      // VC++6.0 setbase()

  int inherit[] = { 0, 1, 2, -1 };
  testn(kLangCXX,"inherit",inherit,".cxx",difffile);

  int virtualfunc[] = { 0, 1, 2, -1 };
  testn(kLangCXX,"virtualfunc",virtualfunc,".cxx",difffile);

  int oprovld[] = { 0, 2, -1 };
  testn(kLangCXX,"oprovld",oprovld,".cxx",difffile);

  ret += ci(kLangCXX,"constary.cxx",difffile);
  ret += ci(kLangCXX,"const.cxx",difffile);
  ret += ci(kLangCXX,"scope0.cxx",difffile);
  ret += ci(kLangCXX,"idxscope0.cxx",difffile);
  ret += ci(kLangCXX,"access0.cxx",difffile);
  ret += ci(kLangCXX,"staticmem0.cxx",difffile);
  ret += ci(kLangCXX,"staticmem1.cxx",difffile);
  ret += ci(kLangCXX,"staticary.cxx",difffile);
  ret += ci(kLangCXX,"minexam.cxx",difffile);
  ret += ci(kLangCXX,"btmplt.cxx",difffile);

  int loopcompile[] = { 1, 2, 3, 4, 5, -1 };
  testn(kLangCXX,"loopcompile",loopcompile,".cxx",difffile);

  ret += ci(kLangCXX,"mfstatic.cxx",difffile);
  ret += ci(kLangCXX,"new0.cxx",difffile);

#if defined(G__MSC_VER)&&(G__MSC_VER<=1200)
  int template[] = { 0, 1, 2, 4, 6, -1 };
#else
  int template[] = { 0, 1, 2, 4, 5, 6, -1 };
#endif
  testn(kLangCXX,"template",template,".cxx",difffile);
  ret += io("template3.cxx","template3.old",difffile);

  ret += ci(kLangCXX,"minherit0.cxx",difffile);
  ret += ci(kLangCXX,"enumscope.cxx",difffile);
  ret += ci(kLangCXX,"baseconv0.cxx",difffile);
  ret += ci(kLangCXX,"friend0.cxx",difffile);
  ret += ci(kLangCXX,"anonunion.cxx",difffile);
  ret += ci(kLangCXX,"init1.cxx",difffile);
  ret += ci(kLangCXX,"init2.cxx",difffile);
  ret += ci(kLangCXX,"include.cxx",difffile);
  ret += ci(kLangCXX,"eh1.cxx",difffile);
  ret += ci(kLangCXX,"ifs.cxx",difffile);
  ret += ci(kLangCXX,"bitfield.cxx",difffile);
  ret += ci(kLangCXX,"cout1.cxx",difffile);
  ret += ci(kLangCXX,"longlong.cxx",difffile);
  ret += ci(kLangCXX,"explicitdtor.cxx",difffile);//fails due to base class dtor

  int nick[] = { 3, 4, -1 };
  testn(kLangCXX,"nick",nick,".cxx",difffile);

  ret += ci(kLangCXX,"nick4.cxx",difffile,"-DDEST");

  int telea[] = { 0,1,2,3,5,6,7, -1 };
  testn(kLangCXX,"telea",telea,".cxx",difffile);

  ret += ci(kLangCXX,"fwdtmplt.cxx",difffile);
  ret += ci(kLangCXX,"VPersonTest.cxx",difffile);
  ret += ci(kLangCXX,"convopr0.cxx",difffile);
  ret += ci(kLangCXX,"nstmplt1.cxx",difffile);
  ret += ci(kLangCXX,"aoki0.cxx",difffile);
  ret += ci(kLangCXX,"borg1.cxx",difffile);
  ret += ci(kLangCXX,"borg2.cxx",difffile);
  ret += ci(kLangCXX,"bruce1.cxx",difffile);
  ret += ci(kLangCXX,"fons3.cxx",difffile);
  ret += ci(kLangCXX,"Test0.cxx",difffile,"","MyString.cxx");
  ret += ci(kLangCXX,"Test1.cxx",difffile,"","Complex.cxx MyString.cxx");
  ret += ci(kLangCXX,"delete0.cxx",difffile);
  ret += ci(kLangCXX,"pb19.cxx",difffile);

#ifdef AUTOCC
  ret += ci(kLangCXX,"autocc.cxx",difffile);
  system("rm G__*");
#endif

  ret += ci(kLangCXX,"maincmplx.cxx",difffile,"","complex1.cxx");
  ret += ci(kLangCXX,"funcmacro.cxx",difffile); 

  ret += ci(kLangCXX,"template.cxx",difffile); 
  ret += mkci(kLangCXX,"template.cxx","template.h",difffile);

  ret += ci(kLangCXX,"vbase.cxx",difffile); 
  ret += mkci(kLangCXX,"vbase.cxx","vbase.h",difffile);

  ret += ci(kLangCXX,"vbase1.cxx",difffile); 
  ret += mkci(kLangCXX,"vbase1.cxx","vbase1.h",difffile);

  //
  //
  //

  ret += ci(kLangCXX,"t215.cxx",difffile); 

  ret += ci(kLangCXX,"t358.cxx",difffile); 



  ret += ci(kLangCXX,"t488.cxx",difffile); 
  ret += ci(kLangCXX,"t516.cxx",difffile); 

  ret += ci(kLangCXX,"t603.cxx",difffile);
  ret += ci(kLangCXX,"t627.cxx",difffile);
  ret += mkci(kLangCXX,"t627.cxx","t627.h",difffile);
  ret += ci(kLangCXX,"t630.cxx",difffile);
  ret += ci(kLangCXX,"t633.cxx",difffile);
  ret += mkci(kLangCXX,"t633.cxx","t633.h",difffile);
  ret += ci(kLangCXX,"t634.cxx",difffile);

  ret += ci(kLangCXX,"t674.cxx",difffile,"-DINTERPRET"); 

#if !defined(G__WIN32) && !defined(G__CYGWIN) && !defined(G__APPLE)
  ret += ci(kLangCXX,"t676.cxx",difffile); //recursive call stack too deep for Visual C++
#endif
  ret += mkci(kLangCXX,"t694.cxx","t694.h",difffile);
  ret += ci(kLangCXX,"t694.cxx",difffile,"-DINTERPRET"); //fails due to default param
  ret += ci(kLangCXX,"t695.cxx",difffile); //fails due to tmplt specialization
  ret += mkci(kLangCXX,"t705.cxx","t705.h",difffile);
  ret += ci(kLangCXX,"t705.cxx",difffile,"-DINTERPRET");
  ret += ci(kLangCXX,"t714.cxx",difffile);
  ret += io("t733.cxx","t733.old",difffile);
#if !defined(G__WIN32) || defined(FORCEWIN32)
  //NOT WORKING: in debug mode on WINDOWS! 
  ret += ci(kLangCXX,"t749.cxx",difffile);
#endif
  ret += ci(kLangCXX,"t751.cxx",difffile);
  ret += ci(kLangCXX,"t764.cxx",difffile);
  ret += ci(kLangCXX,"t767.cxx",difffile);
  ret += ci(kLangCXX,"t776.cxx",difffile);
  ret += ci(kLangCXX,"t777.cxx",difffile);
  ret += ci(kLangCXX,"t784.cxx",difffile);
  ret += ci(kLangCXX,"t825.cxx",difffile);
  ret += ci(kLangCXX,"t910.cxx",difffile);
  ret += ci(kLangCXX,"t916.cxx",difffile);
#if !defined(G__VISUAL) || defined(FORCEWIN32)
  ret += io("t927.cxx","t927.old",difffile);
#endif
#if !defined(G__WIN32) || defined(FORCEWIN32)
  ret += mkciN(kLangCXX,"t928.cxx","t928.h",difffile,"","t928a.h","t928b.h");
#endif
  ret += ci(kLangCXX,"t930.cxx",difffile);
  ret += ci(kLangCXX,"t938.cxx",difffile);
  ret += ci(kLangCXX,"t958.cxx",difffile);
  ret += ci(kLangCXX,"t959.cxx",difffile);
  ret += mkci(kLangCXX,"t961.cxx","t961.h",difffile); //mkc(kLangCXX,"t961.h",difffile);
                                            //Borland C++5.5 has a problem
                                            //with reverse_iterator::reference
  ret += ci(kLangCXX,"t963.cxx",difffile);
#ifdef G__P2F
  ret += mkci(kLangCXX,"t966.cxx","t966.h",difffile);
#endif
  ret += mkci(kLangCXX,"t968.cxx","t968.h",difffile); // problem with BC++5.5 & VC++6.0
  ret += mkci(kLangCXX,"t970.cxx","t970.h",difffile);
  ret += mkciN(kLangCXX,"t972.cxx","t972a.h",difffile,"","t972b.h");

#if !defined(G__WIN32) || defined(FORCEWIN32)
  ret += mkci(kLangCXX,"t980.cxx","t980.h",difffile);
#endif
#if !defined(G__WIN32) || defined(FORCEWIN32)
  ret += ci(kLangCXX,"t986.cxx",difffile,"-DTEST");
#endif
  ret += mkci(kLangCXX,"t987.cxx","t987.h",difffile);
  ret += mkciN(kLangCXX,"t991.cxx","t991a.h",difffile,"","t991b.h","t991c.h");
  ret += mkci(kLangCXX,"t992.cxx","t992.h",difffile);  // problem gcc3.2
  ret += mkci(kLangCXX,"maptest.cxx","maptest.h",difffile); // problem icc
  ret += mkci(kLangC,"t993.c","t993.h",difffile); 
  ret += mkci(kLangCXX,"t995.cxx","t995.h",difffile); 
  ret += mkci(kLangCXX,"t996.cxx","t996.h",difffile); 
  ret += ci(kLangCXX,"t998.cxx",difffile); 
  ret += mkci(kLangCXX,"t1002.cxx","t1002.h",difffile); 
  ret += ci(kLangCXX,"t1004.cxx",difffile); 
  ret += ci(kLangCXX,"t1011.cxx",difffile); 
  ret += mkci(kLangCXX,"t1011.cxx","t1011.h",difffile); 
  ret += ci(kLangCXX,"t1015.cxx",difffile); 
  ret += ci(kLangCXX,"t1016.cxx",difffile); 
  ret += mkci(kLangCXX,"t1016.cxx","t1016.h",difffile); 
  ret += ci(kLangCXX,"t1023.cxx",difffile); 
  ret += ci(kLangCXX,"t1024.cxx",difffile); 
  ret += mkci(kLangCXX,"t1024.cxx","t1024.h",difffile); 
#if !defined(G__WIN32) || defined(FORCEWIN32)
  ret += mkci(kLangCXX,"t1025.cxx","t1025.h",difffile); 
#endif
  ret += ci(kLangCXX,"t1026.cxx",difffile); // problem with BC++5.5
  ret += mkci(kLangCXX,"t1026.cxx","t1026.h",difffile); 
  ret += io("t1027.cxx","t1027.old",difffile);
  //ci(kLangCXX,"t1027.cxx",difffile); // problem with BC++5.5
  //mkci(kLangCXX,"t1027.cxx","t1027.h",difffile); 
  ret += ci(kLangCXX,"t1032.cxx",difffile); 
  ret += ci(kLangCXX,"t1032.cxx",difffile); 
#if !defined(G__WIN32) || defined(FORCEWIN32)
  ret += io("t1034.cxx","t1034.old",difffile); // sizeof(long double)==12
#endif
  ret += ci(kLangCXX,"t1035.cxx",difffile);  
  ret += mkci(kLangCXX,"t1035.cxx","t1035.h",difffile); 
  ret += ci(kLangCXX,"t1036.cxx",difffile);  
  ret += mkci(kLangCXX,"t1040.cxx","t1040.h",difffile); // gcc3.2 has problem 
  ret += io("t1042.cxx","t1042.old",difffile);

#if !defined(G__WIN32) || defined(FORCEWIN32)
  ret += ci(kLangCXX,"t1046.cxx",difffile); 
  ret += mkci(kLangCXX,"t1046.cxx","t1046.h",difffile); 
#endif
  ret += ci(kLangCXX,"t1047.cxx",difffile); 
  ret += mkci(kLangCXX,"t1047.cxx","t1047.h",difffile); 
  ret += ci(kLangCXX,"t1048.cxx",difffile); 
  ret += ci(kLangCXX,"t1157.cxx",difffile); 
  ret += ci(kLangCXX,"t1158.cxx",difffile); 
  ret += ci(kLangCXX,"t1160.cxx",difffile); 
  ret += ci(kLangCXX,"aryinit0.cxx",difffile); 
  ret += ci(kLangCXX,"aryinit1.cxx",difffile); 
  ret += ci(kLangCXX,"t1164.cxx",difffile); 
  ret += ci(kLangCXX,"t1165.cxx",difffile); 
  ret += ci(kLangCXX,"t1178.cxx",difffile); 
  ret += mkci(kLangCXX,"t1187.cxx","t1187.h",difffile); 
  ret += ci(kLangCXX,"t1192.cxx",difffile);  
  ret += mkci(kLangCXX,"t1193.cxx","t1193.h",difffile); 
  ret += ci(kLangCXX,"t1203.cxx",difffile);  
  ret += ci(kLangCXX,"t1205.cxx",difffile);  
  ret += mkci(kLangCXX,"t1205.cxx","t1205.h",difffile); 
  ret += ci(kLangCXX,"t1213.cxx",difffile);  
  ret += ci(kLangCXX,"t1214.cxx",difffile);  
  ret += ci(kLangCXX,"t1215.cxx",difffile);  
  ret += mkci(kLangCXX,"t1215.cxx","t1215.h",difffile); 
  ret += ci(kLangCXX,"t1221.cxx",difffile);  
  ret += ci(kLangCXX,"t1222.cxx",difffile);  
  ret += ci(kLangCXX,"t1223.cxx",difffile);  
  ret += ci(kLangCXX,"t1224.cxx",difffile);  
  ret += io("t1228.cxx","t1228.old",difffile);

  ret += mkci(kLangCXX,"t1048.cxx","t1048.h",difffile,"-I.. -I../src");
  ret += ci(kLangCXX,"t1049.cxx",difffile); 
  ret += ci(kLangCXX,"t1054.cxx",difffile); 
  ret += ci(kLangCXX,"t1055.cxx",difffile); 
  ret += mkci(kLangCXX,"t1061.cxx","t1061.h",difffile);
#if !defined(G__WIN32) || defined(FORCEWIN32)
  ret += mkci(kLangCXX,"t1062.cxx","t1062.h",difffile); 
#endif
  ret += ci(kLangCXX,"t1067.cxx",difffile); 
  ret += mkci(kLangCXX,"t1067.cxx","t1067.h",difffile); 
  ret += ci(kLangCXX,"t1068.cxx",difffile); 
  ret += mkci(kLangCXX,"t1068.cxx","t1068.h",difffile); 
  ret += ci(kLangCXX,"t1079.cxx",difffile);
  ret += mkci(kLangCXX,"t1079.cxx","t1079.h",difffile); 
  ret += ci(kLangCXX,"t1084.cxx",difffile);
  ret += ci(kLangCXX,"t1085.cxx",difffile);
  ret += ci(kLangCXX,"t1086.cxx",difffile);
  ret += ci(kLangCXX,"t1088.cxx",difffile);
  ret += ci(kLangCXX,"t1094.cxx",difffile);
  ret += ci(kLangCXX,"t1101.cxx",difffile); 
  ret += mkci(kLangCXX,"t1115.cxx","t1115.h",difffile); 
  ret += ci(kLangCXX,"t1124.cxx",difffile); 
  ret += ci(kLangCXX,"t1125.cxx",difffile); 
  ret += ci(kLangCXX,"t1126.cxx",difffile); 

#if !defined(G__APPLE)
  // This not work on macos because of var_arg
  ret += ci(kLangCXX,"t1127.cxx",difffile); 
  ret += mkci(kLangCXX,"t1127.cxx","t1127.h",difffile);  // 
#endif

  ret += ci(kLangCXX,"t1128.cxx",difffile);  // looks to me gcc3.2 has a bug
  ret += ci(kLangCXX,"t1129.cxx",difffile);  // g++3.2 fails
  ret += ci(kLangCXX,"t1134.cxx",difffile);  
  ret += ci(kLangCXX,"t1136.cxx",difffile);  
  ret += ci(kLangCXX,"t1140.cxx",difffile);  

  ret += ci(kLangCXX,"t1144.cxx",difffile);  
  ret += ci(kLangCXX,"t1144.cxx",difffile,"","","-Y0");  
  ret += ci(kLangCXX,"t1144.cxx",difffile,"","","-Y1");  

  ret += ci(kLangCXX,"t1148.cxx",difffile);  

  ret += mkciN(kLangCXX,"t1247.cxx","t1247.h",difffile,"","t1247a.h");
  ret += mkci(kLangCXX,"t1276.cxx","t1276.h",difffile); 

  ret += mkci(kLangCXX,"t1277.cxx","t1277.h",difffile); // works only with gcc2.96

#define PROBLEM
#if defined(PROBLEM) && (!defined(G__WIN32) || defined(FORCEWIN32))
  ret += mkci(kLangCXX,"t674.cxx","t674.h",difffile); // Problem with VC++6.0

  ret += ci(kLangCXX,"t648.cxx",difffile); // long long has problem with BC++5.5
                                 // also with VC++6.0 bug different

  ret += mkci(kLangCXX,"t977.cxx","t977.h",difffile); // VC++ problem is known

  ret += ci(kLangCXX,"t980.cxx",difffile); // problem with BC++5.5

#if (G__GNUC==2)
  ret += mkci(kLangCXX,"t1030.cxx","t1030.h",difffile); // works only with gcc2.96
  ret += mkci(kLangCXX,"t1031.cxx","t1031.h",difffile); // works only with gcc2.96
  //mkci(kLangCXX,"t1030.cxx","t1030.h",difffile,"","","-Y0"); 
  //mkci(kLangCXX,"t1031.cxx","t1031.h",difffile,"","","-Y0"); 
#endif

#endif

  ret += ci(kLangCXX,"t1278.cxx",difffile);
  ret += ci(kLangCXX,"t1279.cxx",difffile);
  ret += ci(kLangCXX,"t1280.cxx",difffile);
  ret += ci(kLangCXX,"t1281.cxx",difffile);
  ret += ci(kLangCXX,"t1282.cxx",difffile);
  ret += ci(kLangCXX,"t1283.cxx",difffile);

#endif // NEWTEST



  printf("Summary==================================================\n");
  cat(stdout,difffile);
  printf("=========================================================\n");

#ifndef DEBUG
  rm("Makefile");
#endif
  
  return (ret>0);
}
//////////////////////////////////////////////////////////////////////

/*
 * Local Variables:
 * c-tab-always-indent:nil
 * c-indent-level:2
 * c-continued-statement-offset:2
 * c-brace-offset:-2
 * c-brace-imaginary-offset:0
 * c-argdecl-indent:0
 * c-label-offset:-2
 * compile-command:"make -k"
 * End:
 */
