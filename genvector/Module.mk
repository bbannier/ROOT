# Module.mk for mathcore module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 20/6/2005

MODDIR       := genvector
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

GENVECTORDIR  := $(MODDIR)
GENVECTORDIRS := $(GENVECTORDIR)/src
GENVECTORDIRI := $(GENVECTORDIR)/inc

##### libGenvector #####
GENVECTORL    := $(MODDIRI)/Math/LinkDef_GenVector.h
GENVECTORL32  := $(MODDIRI)/Math/LinkDef_GenVector32.h
GENVECTORLINC :=  \
                $(MODDIRI)/Math/LinkDef_Point3D.h \
                $(MODDIRI)/Math/LinkDef_Vector3D.h \
                $(MODDIRI)/Math/LinkDef_Vector4D.h \
                $(MODDIRI)/Math/LinkDef_Rotation.h
GENVECTORDS   := $(MODDIRS)/G__GenVector.cxx
GENVECTORDS32 := $(MODDIRS)/G__GenVector32.cxx
GENVECTORDO   := $(GENVECTORDS:.cxx=.o)
GENVECTORDO32 := $(GENVECTORDS32:.cxx=.o)
GENVECTORDH   := $(GENVECTORDS:.cxx=.h)

GENVECTORDH1  :=  $(MODDIRI)/Math/Vector2D.h \
                 $(MODDIRI)/Math/Point2D.h \
                 $(MODDIRI)/Math/Vector3D.h \
                 $(MODDIRI)/Math/Point3D.h \
                 $(MODDIRI)/Math/Vector4D.h \
                 $(MODDIRI)/Math/Rotation3D.h \
                 $(MODDIRI)/Math/RotationZYX.h \
                 $(MODDIRI)/Math/RotationX.h \
                 $(MODDIRI)/Math/RotationY.h \
                 $(MODDIRI)/Math/RotationZ.h \
                 $(MODDIRI)/Math/LorentzRotation.h \
                 $(MODDIRI)/Math/Boost.h    \
                 $(MODDIRI)/Math/BoostX.h    \
                 $(MODDIRI)/Math/BoostY.h    \
                 $(MODDIRI)/Math/BoostZ.h    \
                 $(MODDIRI)/Math/EulerAngles.h \
                 $(MODDIRI)/Math/AxisAngle.h \
                 $(MODDIRI)/Math/Quaternion.h \
                 $(MODDIRI)/Math/Transform3D.h \
                 $(MODDIRI)/Math/Translation3D.h \
                 $(MODDIRI)/Math/Plane3D.h \
                 $(MODDIRI)/Math/VectorUtil_Cint.h  


GENVECTORDH132:=  $(MODDIRI)/Math/Vector2D.h \
	         $(MODDIRI)/Math/Point2D.h \
	         $(MODDIRI)/Math/Vector3D.h \
                 $(MODDIRI)/Math/Point3D.h \
                 $(MODDIRI)/Math/Vector4D.h \



GENVECTORAH   := $(filter-out $(MODDIRI)/Math/LinkDef%, $(wildcard $(MODDIRI)/Math/*.h))
GENVECTORGVH  := $(filter-out $(MODDIRI)/Math/GenVector/LinkDef%, $(wildcard $(MODDIRI)/Math/GenVector/*.h))
GENVECTORH    := $(GENVECTORAH) $(GENVECTORGVH)
GENVECTORS    := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
GENVECTORO    := $(GENVECTORS:.cxx=.o)

GENVECTORDEP  := $(GENVECTORO:.o=.d)  $(GENVECTORDO:.o=.d) $(GENVECTORDO32:.o=.d)

GENVECTORLIB  := $(LPATH)/libGenvector.$(SOEXT)
GENVECTORMAP  := $(GENVECTORLIB:.$(SOEXT)=.rootmap)

# used in the main Makefile
ALLHDRS      += $(patsubst $(MODDIRI)/Math/%.h,include/Math/%.h,$(GENVECTORH))
ALLLIBS      += $(GENVECTORLIB)
ALLMAPS      += $(GENVECTORMAP)

# include all dependency files
INCLUDEFILES += $(GENVECTORDEP)

##### local rules #####
include/Math/%.h: $(GENVECTORDIRI)/Math/%.h
		@(if [ ! -d "include/Math/GenVector" ]; then   \
		   mkdir -p include/Math/GenVector;       \
		fi)
		cp $< $@
# build lib genvector: use also obj  from math and fit directory 
$(GENVECTORLIB): $(GENVECTORO) $(GENVECTORDO) $(GENVECTORDO32) $(ORDER_) $(MAINLIBS)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)"  \
		   "$(SOFLAGS)" libGenvector.$(SOEXT) $@     \
		   "$(GENVECTORO) $(GENVECTORDO) $(GENVECTORDO32)"    \
		   "$(GENVECTORLIBEXTRA)"

$(GENVECTORDS):  $(GENVECTORDH1) $(GENVECTORL) $(GENVECTORLINC) $(ROOTCINTTMPDEP)
		@echo "Generating dictionary $@..."
		@echo "for files $(GENVECTORDH1)"
		$(ROOTCINTTMP) -f $@ -c $(GENVECTORDH1) $(GENVECTORL)
#		genreflex $(GENVECTORDIRS)/GenvectorDict.h  --selection_file=$(GENVECTORDIRS)/selection_Genvector.xml -o $(GENVECTORDIRS)/G__Genvector.cxx -I$(GENVECTORDIRI)

$(GENVECTORDS32):$(GENVECTORDH132) $(GENVECTORL) $(GENVECTORLINC) $(ROOTCINTTMPDEP)
		@echo "Generating dictionary $@..."
		@echo "for files $(GENVECTORDH132)"
		$(ROOTCINTTMP) -f $@ -c $(GENVECTORDH132) $(GENVECTORL32)

$(GENVECTORMAP): $(RLIBMAP) $(MAKEFILEDEP) $(GENVECTORL) $(GENVECTORLINC) $(GENVECTORL32)
		$(RLIBMAP) -o $(GENVECTORMAP) -l $(GENVECTORLIB) \
		   -d $(GENVECTORLIBDEPM) -c $(GENVECTORL) $(GENVECTORLINC) $(GENVECTORL32)

all-genvector:   $(GENVECTORLIB) $(GENVECTORMAP)

clean-genvector:
		@rm -f $(GENVECTORO) $(GENVECTORDO)

clean::         clean-genvector

distclean-genvector: clean-genvector
		@rm -f $(GENVECTORDEP) $(GENVECTORDS) $(GENVECTORDS32) $(GENVECTORDH) \
		   $(GENVECTORLIB) $(GENVECTORMAP)
		@rm -rf include/Math

distclean::     distclean-genvector

test-genvector:	all-genvector
		@cd $(GENVECTORDIR)/test; make

##### extra rules ######

