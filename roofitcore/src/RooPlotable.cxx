/*****************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: RooFitCore
 *    File: $Id: RooPlotable.cc,v 1.3 2001/06/30 01:33:13 verkerke Exp $
 * Authors:
 *   DK, David Kirkby, Stanford University, kirkby@hep.stanford.edu
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu
 * History:
 *   29-Apr-2001 DK Created initial version
 *
 * Copyright (C) 2001 Stanford University
 *****************************************************************************/

// -- CLASS DESCRIPTION [PLOT] --
// Classes inheriting from this class can be plotted and printed, and can
// be dynamically cross-cast into TObject's.

// #include "BaBar/BaBar.hh"
#include "RooFitCore/RooPlotable.hh"
#include "TObject.h"
#include <iostream.h>

ClassImp(RooPlotable)
;

void RooPlotable::printToStream(ostream& os, PrintOption opt, TString indent) const {
  if(opt >= Verbose) {
    os << indent << "--- RooPlotable ---" << endl;
    os << indent << "  y-axis min = " << getYAxisMin() << endl
       << indent << "  y-axis max = " << getYAxisMax() << endl
       << indent << "  y-axis label \"" << getYAxisLabel() << "\"" << endl;
  }
}

TObject *RooPlotable::crossCast() {
  return dynamic_cast<TObject*>(this);
}
