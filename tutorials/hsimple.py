#*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
#*-*
#*-*  This program creates :
#*-*    - a one dimensional histogram
#*-*    - a two dimensional histogram
#*-*    - a profile histogram
#*-*    - a memory-resident ntuple
#*-*
#*-*  These objects are filled with some random numbers and saved on a file.
#*-*
#*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

from ROOT import TCanvas, TFile, TProfile, TNtuple, TH1F, TH2F
from ROOT import gROOT, gBenchmark, gRandom, gSystem


gROOT.Reset()

# Create a new canvas.
c1 = TCanvas( 'c1', 'Dynamic Filling Example', 200, 10, 700, 500 )
c1.SetFillColor( 42 )
c1.GetFrame().SetFillColor( 21 )
c1.GetFrame().SetBorderSize( 6 )
c1.GetFrame().SetBorderMode( -1 )

# Create a new ROOT binary machine independent file.
# Note that this file may contain any kind of ROOT objects, histograms,
# pictures, graphics objects, detector geometries, tracks, events, etc..
# This file is now becoming the current directory.

hfile = gROOT.FindObject( 'hsimple.root' )
if hfile : hfile.Close()
hfile = TFile( 'hsimple.root', 'RECREATE', 'Demo ROOT file with histograms' )

# Create some histograms, a profile histogram and an ntuple
hpx    = TH1F( 'hpx', 'This is the px distribution', 100, -4, 4 )
hpxpy  = TH2F( 'hpxpy', 'py vs px', 40, -4, 4, 40, -4, 4 )
hprof  = TProfile( 'hprof', 'Profile of pz versus px', 100, -4, 4, 0, 20 )
ntuple = TNtuple( 'ntuple', 'Demo ntuple', 'px:py:pz:random:i' )

#  Set canvas/frame attributes (save old attributes)
hpx.SetFillColor( 48 )

gBenchmark.Start( 'hsimple' )

# Fill histograms randomly
gRandom.SetSeed()
gauss, rndm = gRandom.Gaus, gRandom.Rndm

kUPDATE = 1000
for i in xrange(25000) :
   px = gauss()
   py = gauss()
   pz = px*px + py*py
   random = rndm(1)
   hpx.Fill( px )
   hpxpy.Fill( px, py )
   hprof.Fill( px, pz )
   ntuple.Fill( px, py, pz, random, i )
   if i and i%kUPDATE == 0 :
      if i == kUPDATE : hpx.Draw()
      c1.Modified()
      c1.Update()
      if gSystem.ProcessEvents() :
         break

gBenchmark.Show( 'hsimple' )

# Save all objects in this file
hpx.SetFillColor( 0 )
hfile.Write()
hpx.SetFillColor( 48 )
c1.Modified()
c1.Update()
  
# Note that the file is automatically closed when application terminates
# or when the file destructor is called.
