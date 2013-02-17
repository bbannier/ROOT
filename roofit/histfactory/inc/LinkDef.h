

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace RooStats;
#pragma link C++ namespace RooStats::HistFactory;

// for auto-loading namespaces
#ifdef USE_FOR_AUTLOADING
#pragma link C++ class RooStats::HistFactory;
#pragma link C++ class RooStats;
#endif            


#pragma link C++ class PiecewiseInterpolation- ;
#pragma link C++ class ParamHistFunc+ ;
#pragma link C++ class RooStats::HistFactory::LinInterpVar+ ;
#pragma link C++ class RooStats::HistFactory::FlexibleInterpVar+ ;
#pragma link C++ class RooStats::HistFactory::EstimateSummary+ ;
#pragma link C++ class RooStats::HistFactory::HistoToWorkspaceFactory+ ;
#pragma link C++ class RooStats::HistFactory::HistoToWorkspaceFactoryFast+ ;
#pragma link C++ class RooStats::HistFactory::RooBarlowBeestonLL+ ;  
#pragma link C++ class RooStats::HistFactory::HistFactorySimultaneous+ ;  
#pragma link C++ class RooStats::HistFactory::HistFactoryNavigation+ ;  

#pragma link C++ class RooStats::HistFactory::ConfigParser+ ;

#pragma link C++ class RooStats::HistFactory::Measurement+ ;
#pragma link C++ class RooStats::HistFactory::Channel+ ;
#pragma link C++ class RooStats::HistFactory::Sample+ ;
#pragma link C++ class RooStats::HistFactory::Data+ ;
#pragma link C++ class RooStats::HistFactory::Asimov+ ;

#pragma link C++ class RooStats::HistFactory::StatError+ ;
#pragma link C++ class RooStats::HistFactory::StatErrorConfig+ ;
#pragma link C++ class RooStats::HistFactory::PreprocessFunction+ ;


#pragma link C++ class std::vector< RooStats::HistFactory::Channel >+ ;
#pragma link C++ class std::vector< RooStats::HistFactory::Sample >+ ;

// make dictionary for all the C++ classes defined in these following files
#ifndef _WIN32   // on Windows rootcint -I seemes not to work 
#ifndef __CLING__
#pragma link C++ defined_in "RooStats/HistFactory/MakeModelAndMeasurementsFast.h"; 
#pragma link C++ defined_in "RooStats/HistFactory/Systematics.h"; 
#pragma link C++ defined_in "RooStats/HistFactory/HistFactoryModelUtils.h"; 
#endif
#else 
#pragma link C++ defined_in "roofit/histfactory/inc/RooStats/HistFactory/MakeModelAndMeasurementsFast.h"; 
#pragma link C++ defined_in "roofit/histfactory/inc/RooStats/HistFactory/Systematics.h"; 
#pragma link C++ defined_in "roofit/histfactory/inc/RooStats/HistFactory/HistFactoryModelUtils.h"; 
#endif

#endif
