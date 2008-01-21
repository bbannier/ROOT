# Module.mk for auth module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: G. Ganis, 7/2005

MODDIR       := $(SRCDIR)/auth
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

AUTHDIR      := $(MODDIR)
AUTHDIRS     := $(AUTHDIR)/src
AUTHDIRI     := $(AUTHDIR)/inc

##### libRootAuth #####
RAUTHL       := $(MODDIRI)/LinkDefRoot.h
RAUTHDS      := $(subst $(SRCDIR)/,,$(MODDIRS))/G__RootAuth.cxx
RAUTHDO      := $(RAUTHDS:.cxx=.o)
RAUTHDH      := $(RAUTHDS:.cxx=.h)

RAUTHH       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
RAUTHS       := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))

RAUTHH       := $(filter-out $(MODDIRI)/DaemonUtils.h,$(RAUTHH))
RAUTHS       := $(filter-out $(MODDIRS)/DaemonUtils.cxx,$(RAUTHS))
RAUTHH       := $(filter-out $(MODDIRI)/AFSAuth.h,$(RAUTHH))
RAUTHH       := $(filter-out $(MODDIRI)/AFSAuthTypes.h,$(RAUTHH))
RAUTHS       := $(filter-out $(MODDIRS)/AFSAuth.cxx,$(RAUTHS))
RAUTHH       := $(filter-out $(MODDIRI)/TAFS.h,$(RAUTHH))
RAUTHS       := $(filter-out $(MODDIRS)/TAFS.cxx,$(RAUTHS))

RAUTHO       := $(subst $(SRCDIR)/,,$(RAUTHS:.cxx=.o))

#### for libSrvAuth (built in rpdutils/Module.mk) ####
DAEMONUTILSO := $(subst $(SRCDIR)/,,$(MODDIRS)/DaemonUtils.o)

RAUTHDEP     := $(RAUTHO:.o=.d) $(RAUTHDO:.o=.d) $(DAEMONUTILSO:.o=.d)

RAUTHLIB     := $(LPATH)/libRootAuth.$(SOEXT)
RAUTHMAP     := $(RAUTHLIB:.$(SOEXT)=.rootmap)

##### libAFSAuth #####
ifneq ($(AFSLIB),)
AFSAUTHL       := $(MODDIRI)/LinkDefAFS.h
AFSAUTHDS      := $(subst $(SRCDIR)/,,$(MODDIRS))/G__AFSAuth.cxx
AFSAUTHDO      := $(AFSAUTHDS:.cxx=.o)
AFSAUTHDH      := $(AFSAUTHDS:.cxx=.h)

AFSAUTHH     := $(MODDIRI)/AFSAuth.h $(MODDIRI)/AFSAuthTypes.h $(MODDIRI)/TAFS.h
AFSAUTHS     := $(MODDIRS)/AFSAuth.cxx $(MODDIRS)/TAFS.cxx

AFSAUTHO     := $(subst $(SRCDIR)/,,$(AFSAUTHS:.cxx=.o))

AFSAUTHDEP   := $(AFSAUTHO:.o=.d) $(AFSAUTHDO:.o=.d)

AFSAUTHLIB   := $(LPATH)/libAFSAuth.$(SOEXT)
AFSAUTHMAP   := $(AFSAUTHLIB:.$(SOEXT)=.rootmap)
endif

#### for libSrvAuth (built in rpdutils/Module.mk) ####
DAEMONUTILSO := $(subst $(SRCDIR)/,,$(MODDIRS)/DaemonUtils.o)

#### for rootd and proofd ####
RSAO         := $(subst $(SRCDIR)/,,$(AUTHDIRS)/rsaaux.o $(AUTHDIRS)/rsalib.o $(AUTHDIRS)/rsafun.o)
ifneq ($(AFSLIB),)
RSAO         += $(MODDIRS)/AFSAuth.o
endif

# Add SSL flags, if required
EXTRA_RAUTHFLAGS = $(EXTRA_AUTHFLAGS)
EXTRA_RAUTHLIBS  = $(CRYPTLIBS)
ifneq ($(SSLLIB),)
EXTRA_RAUTHFLAGS += $(SSLINCDIR:%=-I%)
EXTRA_RAUTHLIBS  += $(SSLLIBDIR) $(SSLLIB)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(MODDIRI)/%.h,include/%.h,$(RAUTHH)) \
                include/DaemonUtils.h
ALLLIBS      += $(RAUTHLIB)
ALLMAPS      += $(RAUTHMAP)
ifneq ($(AFSLIB),)
ALLHDRS      += $(patsubst $(MODDIRI)/%.h,include/%.h,$(AFSAUTHH))
ALLLIBS      += $(AFSAUTHLIB)
ALLMAPS      += $(AFSAUTHMAP)
endif

# include all dependency files
INCLUDEFILES += $(RAUTHDEP)
ifneq ($(AFSLIB),)
INCLUDEFILES += $(AFSAUTHDEP)
endif

##### local rules #####
include/%.h:    $(AUTHDIRI)/%.h
		cp $< $@

$(RAUTHLIB):    $(RAUTHO) $(RAUTHDO) $(ORDER_) $(MAINLIBS) $(RAUTHLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libRootAuth.$(SOEXT) $@ "$(RAUTHO) $(RAUTHDO)" \
		   "$(RAUTHLIBEXTRA) $(EXTRA_RAUTHLIBS)"

$(RAUTHDS):     $(RAUTHH) $(RAUTHL) $(ROOTCINTTMPEXE)
		@echo "Generating dictionary $@..."
		$(ROOTCINTTMP) -f $@ -c $(RAUTHH) $(RAUTHL)

$(RAUTHMAP):    $(RLIBMAP) $(MAKEFILEDEP) $(RAUTHL)
		$(RLIBMAP) -o $(RAUTHMAP) -l $(RAUTHLIB) \
		   -d $(RAUTHLIBDEPM) -c $(RAUTHL)

$(AFSAUTHLIB):  $(AFSAUTHO) $(AFSAUTHDO) $(ORDER_) $(MAINLIBS) $(AFSAUTHLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libAFSAuth.$(SOEXT) $@ \
		   "$(AFSAUTHO) $(AFSAUTHDO)" \
		   "$(AFSLIBDIR) $(AFSLIB) $(RESOLVLIB)"

$(AFSAUTHDS):   $(AFSAUTHH) $(AFSAUTHL) $(ROOTCINTTMPEXE)
		@echo "Generating dictionary $@..."
		$(ROOTCINTTMP) -f $@ -c $(AFSAUTHH) $(AFSAUTHL)

$(AFSAUTHMAP):  $(RLIBMAP) $(MAKEFILEDEP) $(AFSAUTHL)
		$(RLIBMAP) -o $(AFSAUTHMAP) -l $(AFSAUTHLIB) \
		   -d $(AFSAUTHLIBDEPM) -c $(AFSAUTHL)

all-auth:       $(RAUTHLIB) $(AFSAUTHLIB) $(RAUTHMAP) $(AFSAUTHMAP)

clean-auth:
		@rm -f $(RAUTHO) $(RAUTHDO) $(DAEMONUTILSO) $(AFSAUTHO) \
		       $(AFSAUTHDO)

clean::         clean-auth

distclean-auth: clean-auth
		@rm -f $(RAUTHDEP) $(RAUTHDS) $(RAUTHDH) $(RAUTHLIB) \
		       $(AFSAUTHDEP) $(AFSAUTHDS) $(AFSAUTHLIB) \
		       $(RAUTHMAP) $(AFSAUTHMAP)

distclean::     distclean-auth

##### extra rules ######
$(RAUTHO):      CXXFLAGS += $(EXTRA_RAUTHFLAGS)
$(RAUTHO):      PCHCXXFLAGS =
$(AFSAUTHO):    CXXFLAGS += $(AFSINCDIR) $(AFSEXTRACFLAGS)
$(AFSAUTHO):    PCHCXXFLAGS =
