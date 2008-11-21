// @(#)root/hist:$Id$
// Author: Danilo.Piparo@cern.ch   01/06/2008

#include "assert.h"
#include <cmath>
#include <iostream>
#include <map>

#include "RooStats/HybridPlot.h"
#include "TStyle.h"
#include "TF1.h"
#include "TAxis.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLegend.h"
#include "TFile.h"


/// To build the THtml documentation
ClassImp(RooStats::HybridPlot)

using namespace RooStats;

//TRandom3 random_generator;

/*----------------------------------------------------------------------------*/

HybridPlot::HybridPlot(const char* name,
                       const  char* title,
                       std::vector<float> sb_vals,
                       std::vector<float> b_vals,
                       float m2lnQ_data,
                       int n_bins,
                       bool verbosity):
   TNamed(name,title),
   fSb_histo_shaded(NULL),
   fB_histo_shaded(NULL),
   fVerbose(verbosity)
{
   // constructor

   // Get the max and the min of the plots
   int n_toys=sb_vals.size();

   assert (n_toys >0);

   double max=-1e40;
   double min=1e40;

   // Extremes of the plot
   for (int i=0;i<n_toys;++i){
      if (sb_vals[i]>max)
         max=sb_vals[i];
      if (b_vals[i]>max)
         max=b_vals[i];
      if (sb_vals[i]<min)
         min=sb_vals[i];
      if (b_vals[i]<min)
         min=b_vals[i];
   }

   if (m2lnQ_data<min)
      min=m2lnQ_data;

   if (m2lnQ_data>max)
      max=m2lnQ_data;

   min*=1.1;
   max*=1.1;

   // Build the histos
   //int n_bins=100;

   fSb_histo = new TH1F ("SB_model",title,n_bins,min,max);
   fSb_histo->SetTitle(fSb_histo->GetTitle());
   fSb_histo->SetLineColor(kBlue);
   fSb_histo->GetXaxis()->SetTitle("-2lnQ");
   //fSb_histo->GetYaxis()->SetTitle("Entries");
   fSb_histo->SetLineWidth(2);

   fB_histo = new TH1F ("B_model",title,n_bins,min,max);
   fB_histo->SetTitle(fB_histo->GetTitle());
   fB_histo->SetLineColor(kRed);
   fB_histo->GetXaxis()->SetTitle("-2lnQ");
   //fB_histo->GetYaxis()->SetTitle("Entries");
   fB_histo->SetLineWidth(2);


   for (int i=0;i<n_toys;++i){
      fSb_histo->Fill(sb_vals[i]);
      fB_histo->Fill(b_vals[i]);
   }

   double histos_max_y=fSb_histo->GetMaximum();
   if (histos_max_y<fB_histo->GetMaximum())
      histos_max_y=fB_histo->GetMaximum();

   double line_hight=histos_max_y/n_toys;

   // Build the line of the measured -2lnQ
   fData_m2lnQ_line = new TLine(m2lnQ_data,0,m2lnQ_data,line_hight);
   fData_m2lnQ_line->SetLineWidth(3);
   fData_m2lnQ_line->SetLineColor(kBlack);

   // The legend
   double golden_section=(std::sqrt(5.)-1)/2;

   fLegend = new TLegend(0.75,0.95-0.2*golden_section,0.95,0.95);
   TString title_leg="-2lnQ distributions ";
   title_leg+=sb_vals.size();
   title_leg+=" toys";
   fLegend->SetName(title_leg.Data());
   fLegend->AddEntry(fSb_histo,"SB toy datasets");
   fLegend->AddEntry(fB_histo,"B toy datasets");
   fLegend->AddEntry((TLine*)fData_m2lnQ_line,"-2lnQ on Data","L");
   fLegend->SetFillColor(0);

}

/*----------------------------------------------------------------------------*/

HybridPlot::~HybridPlot(){
   // destructor 

   if (fSb_histo)
      delete fSb_histo;

   if (fB_histo)
      delete fB_histo;

   if (fData_m2lnQ_line)
      delete fData_m2lnQ_line;

   if (fLegend)
      delete fLegend;
}

/*----------------------------------------------------------------------------*/

void HybridPlot::Draw(const char* options){
   // draw on canvas

   SetCanvas(new TCanvas(GetName(),GetTitle()));
   GetCanvas()->cd();
   GetCanvas()->Draw(options);

   // We don't want the statistics of the histos
   gStyle->SetOptStat(0);

   // The histos

   if (fSb_histo->GetMaximum()>fB_histo->GetMaximum()){
      fSb_histo->DrawNormalized();
      fB_histo->DrawNormalized("same");
   }
   else{
      fB_histo->DrawNormalized();
      fSb_histo->DrawNormalized("same");
   }

   // Shaded
   fB_histo_shaded = (TH1F*)fB_histo->Clone("b_shaded");
   fB_histo_shaded->SetFillStyle(3005);
   fB_histo_shaded->SetFillColor(kRed);

   fSb_histo_shaded = (TH1F*)fSb_histo->Clone("sb_shaded");
   fSb_histo_shaded->SetFillStyle(3004);
   fSb_histo_shaded->SetFillColor(kBlue);


   // Empty the bins according to the data -2lnQ
   double data_m2lnq= fData_m2lnQ_line->GetX1();
   for (int i=0;i<fSb_histo->GetNbinsX();++i){
      if (fSb_histo->GetBinCenter(i)<data_m2lnq){
         fSb_histo_shaded->SetBinContent(i,0);
         fB_histo_shaded->SetBinContent(i,fB_histo->GetBinContent(i)/fB_histo->GetEntries());
      }
      else{
         fB_histo_shaded->SetBinContent(i,0);
         fSb_histo_shaded->SetBinContent(i,fSb_histo->GetBinContent(i)/fSb_histo->GetEntries());
      }
   }

   // Draw the shaded histos
   fB_histo_shaded->Draw("same");
   fSb_histo_shaded->Draw("same");

   // The line 
   fData_m2lnQ_line->Draw("same");

   // The legend
   fLegend->Draw("same");


}

/*----------------------------------------------------------------------------*/

//void HybridPlot::print(const char* options){
//    std::cout << "\nHybridPlot " << GetName() << std::endl;
//    }

/*----------------------------------------------------------------------------*/

void HybridPlot::DumpToFile (const char* RootFileName, const char* options){
   // All the objects are written to rootfile

   TFile ofile(RootFileName,options);
   ofile.cd();

   // The histos
   fSb_histo->Write();
   fB_histo->Write();

   // The shaded histos
   if (fB_histo_shaded!=NULL && fSb_histo_shaded!=NULL){
      fB_histo_shaded->Write();
      fSb_histo_shaded->Write();
   }

   // The line 
   fData_m2lnQ_line->Write("Measured -2lnQ line tag");

   // The legend
   fLegend->Write();

   ofile.Close();

}

/*----------------------------------------------------------------------------*/


// from Rsc.cxx




/**
   Perform 2 times a gaussian fit to fetch the center of the histo.
   To get the second fit range get an interval that tries to keep into account 
   the skewness of the distribution.
**/
double HybridPlot::GetHistoCenter(TH1* histo_orig, double n_rms, bool display_result){
   // Get the center of the histo
   
   TCanvas* c = new TCanvas();
   c->cd();

   TH1F* histo = (TH1F*)histo_orig->Clone();

   // get the histo x extremes
   double x_min = histo->GetXaxis()->GetXmin(); 
   double x_max = histo->GetXaxis()->GetXmax();

   // First fit!

   TF1* gaus = new TF1("mygaus", "gaus", x_min, x_max);

   gaus->SetParameter("Constant",histo->GetEntries());
   gaus->SetParameter("Mean",histo->GetMean());
   gaus->SetParameter("Sigma",histo->GetRMS());

   histo->Fit(gaus);

   // Second fit!
   double sigma = gaus->GetParameter("Sigma");
   double mean = gaus->GetParameter("Mean");

   delete gaus;

   std::cout << "Center is 1st pass = " << mean << std::endl;

   double skewness = histo->GetSkewness();

   x_min = mean - n_rms*sigma - sigma*skewness/2;
   x_max = mean + n_rms*sigma - sigma*skewness/2;;

   TF1* gaus2 = new TF1("mygaus2", "gaus", x_min, x_max);
   gaus2->SetParameter("Mean",mean);

   histo->Fit(gaus2,"L","", x_min, x_max);

   histo->Draw();
   gaus2->Draw("same");

   double center = gaus2->GetParameter("Mean");


   delete gaus2;
   delete histo;
   if (! display_result)
      delete c;

   return center;


}


/**
   We let an orizzontal bar go down and we stop when we have the integral 
   equal to the desired one.
**/

double* HybridPlot::GetHistoPvals (TH1* histo, double percentage){

   if (percentage>1){
      std::cerr << "Percentage greater or equal to 1!\n";
      return NULL;
   }

   // Get the integral of the histo
   double* h_integral=histo->GetIntegral();

   // Start the iteration
   std::map<int,int> extremes_map;

   for (int i=0;i<histo->GetNbinsX();++i){
      for (int j=0;j<histo->GetNbinsX();++j){
         double integral = h_integral[j]-h_integral[i];
         if (integral>percentage){
            extremes_map[i]=j;
            break;
         }
      }
   }

   // Now select the couple of extremes which have the lower bin content diff
   std::map<int,int>::iterator it;
   int a,b;
   double left_bin_center(0.),right_bin_center(0.);
   double diff=10e40;
   double current_diff;
   for (it = extremes_map.begin();it != extremes_map.end();++it){
      a=it->first;
      b=it->second;
      current_diff=fabs(histo->GetBinContent(a)-histo->GetBinContent(b));
      if (current_diff<diff){
         //std::cout << "a=" << a << " b=" << b << std::endl;
         diff=current_diff;
         left_bin_center=histo->GetBinCenter(a);
         right_bin_center=histo->GetBinCenter(b);
      }
   }

   double* d = new double[2];
   d[0]=left_bin_center;
   d[1]=right_bin_center;
   return d;
}

//----------------------------------------------------------------------------//
/**
   Get the median of an histogram.
**/
double HybridPlot::GetMedian(TH1* histo){

   //int xbin_median;
   Double_t* integral = histo->GetIntegral();
   int median_i = 0;
   for (int j=0;j<histo->GetNbinsX(); j++) 
      if (integral[j]<0.5) 
         median_i = j;

   double median_x = 
      histo->GetBinCenter(median_i)+
      (histo->GetBinCenter(median_i+1)-
       histo->GetBinCenter(median_i))*
      (0.5-integral[median_i])/(integral[median_i+1]-integral[median_i]);

   return median_x;
}


