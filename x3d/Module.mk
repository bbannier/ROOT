# Module.mk for x3d module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR       := x3d
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

X3DDIR       := $(MODDIR)
X3DDIRS      := $(X3DDIR)/src
X3DDIRI      := $(X3DDIR)/inc

##### libX3d #####
X3DL         := $(MODDIRI)/LinkDef.h
X3DDS        := $(MODDIRS)/G__X3D.cxx
X3DDO        := $(X3DDS:.cxx=.o)
X3DDH        := $(X3DDS:.cxx=.h)

X3DH1        := $(MODDIRI)/TViewerX3D.h $(MODDIRI)/TX3DFrame.h
X3DH2        := $(MODDIRI)/x3d.h
X3DH         := $(X3DH1) $(X3DH2)
X3DS1        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
X3DS2        := $(wildcard $(MODDIRS)/*.c)
X3DO         := $(X3DS1:.cxx=.o) $(X3DS2:.c=.o)

X3DDEP       := $(X3DO:.o=.d) $(X3DDO:.o=.d)

X3DLIB       := $(LPATH)/libX3d.$(SOEXT)
X3DMAP       := $(X3DLIB:.$(SOEXT)=.rootmap)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(X3DH))
ALLLIBS     += $(X3DLIB)
ALLMAPS     += $(X3DMAP)

# include all dependency files
INCLUDEFILES += $(X3DDEP)

##### local rules #####
include/%.h:    $(X3DDIRI)/%.h
		cp $< $@

$(X3DLIB):      $(X3DO) $(X3DDO) $(ORDER_) $(MAINLIBS) $(X3DLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libX3d.$(SOEXT) $@ "$(X3DO) $(X3DDO)" \
		   "$(X3DLIBEXTRA) $(XLIBS)"

$(X3DDS):       $(X3DH1) $(X3DL) $(X3DO) $(ROOTCINTNEW)
		@echo "Generating dictionary $@..."
		$(ROOTCINTNEW) -f $@ -o "$(X3DO)" -c $(X3DH1) $(X3DL)

$(X3DMAP):      $(RLIBMAP) $(MAKEFILEDEP) $(X3DL)
		$(RLIBMAP) -o $(X3DMAP) -l $(X3DLIB) \
		   -d $(X3DLIBDEPM) -c $(X3DL)

all-x3d:        $(X3DLIB) $(X3DMAP)

clean-x3d:
		@rm -f $(X3DO) $(X3DDO)

clean::         clean-x3d

distclean-x3d:  clean-x3d
		@rm -f $(X3DDEP) $(X3DDS) $(X3DDH) $(X3DLIB) $(X3DMAP)

distclean::     distclean-x3d
