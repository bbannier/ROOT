#include <iostream>
#include <fstream>

#include <TMath.h>
#include <Math/SpecFunc.h>

#include <TApplication.h>

#include <TCanvas.h>
#include <TH2F.h>
#include <TGraph.h>

const double MIN = 0;
const double MAX = 1;
const double INCREMENT = 0.01;
const int ARRAYSIZE = (int) (( MAX - MIN ) / INCREMENT);
inline int arrayindex(double i) { return ARRAYSIZE - (int) ( (MAX - i) / INCREMENT ) -1 ; };

using namespace std;

void drawPoints(Double_t x[ARRAYSIZE], Double_t y[ARRAYSIZE], int color, int style = 1)
{
   TGraph* g = new TGraph(ARRAYSIZE, x, y);
   g->SetLineColor(color);
   g->SetLineStyle(style);
   g->SetLineWidth(3);
   g->Draw("SAME");
}

void testSpecFuncBetaI() 
{
   Double_t x[ ARRAYSIZE ];
   Double_t yb[ ARRAYSIZE ];
   Double_t ymb[ ARRAYSIZE ];

//    ofstream outputFile ("values.txt");

   TCanvas* c1 = new TCanvas("c1", "Two Graphs", 600, 400); 
   TH2F* hpx = new TH2F("hpx", "Two Graphs(hpx)", ARRAYSIZE, MIN, MAX, ARRAYSIZE, 0, 5);
   hpx->SetStats(kFALSE);
   hpx->Draw();

   int color = 2;

   double b = 0.2, a= 0.9;
   cout << "** b = " << b << " **" << endl;
   for ( double i = MIN; i < MAX; i += INCREMENT )
   {
      cout << "i:"; cout.width(5); cout << i 
           << " index: "; cout.width(5); cout << arrayindex(i) 
           << " TMath::BetaIncomplete(x,a,b): "; cout.width(10); cout << TMath::BetaIncomplete(i,a,b)
           << " ROOT::Math::inc_beta(a,a,b): "; cout.width(10); cout << ROOT::Math::inc_beta(i,a,b)
           << endl;
      
      x[arrayindex(i)] = i;
      yb[arrayindex(i)] = TMath::BetaIncomplete(i,a,b);
      ymb[arrayindex(i)] = ROOT::Math::inc_beta(i,a,b);
   }
   
   drawPoints(x, yb, color++);
   drawPoints(x, ymb, color++, 7);

   c1->Show();

   cout << "Test Done!" << endl;

   return;
}

int main(int argc, char **argv) 
{
   TApplication theApp("App",&argc,argv);
   testSpecFuncBetaI();
   theApp.Run();

   return 0;
}
