// @(#)root/graf:$Id$
// Author: Nicolas Brun, Olivier Couet, Oleksandr Grebenyuk

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "Riostream.h"
#include "TROOT.h"
#include "TClass.h"
#include "TLatex.h"
#include "TMath.h"
#include "TVirtualPad.h"
#include "TVirtualPS.h"

#ifdef R__SUNCCBUG
const Double_t kPI = 3.14159265358979323846;
#else
const Double_t kPI = TMath::Pi();
#endif
const Int_t kLatex      = BIT(10);
const Int_t kPrintingPS = BIT(11); //set in TPad.h

ClassImp(TLatex)


//______________________________________________________________________________
/* Begin_Html
<center><h2>TLatex : to draw Mathematical Formula</h2></center>

TLatex's purpose is to write mathematical equations.
The syntax is very similar to Latex's.
It provides several functionalities:
<ul>
<li><a href="#L1">  Subscripts and Superscripts</a></li>
<li><a href="#L2">  Fractions</a></li>
<li><a href="#L3">  Splitting Lines</a></li>
<li><a href="#L4">  Roots</a></li>
<li><a href="#L5">  Mathematical Symbols</a></li>
<li><a href="#L6">  Delimiters</a></li>
<li><a href="#L7">  Greek Letters</a></li>
<li><a href="#L8">  Accents</a></li>
<li><a href="#L9">  Changing Style</a></li>
<li><a href="#L10"> Alignment Rules</a></li>
<li><a href="#L11"> Character Adjustement</a></li>
<li><a href="#L12"> Italic and Boldface</a></li>
<li><a href="#L13"> Examples</a></li>
</ul>

When the font precision (see <tt>TAttText</tt>) is low (0 or 1), TLatex is
painted as a normal TText, the control characters are not interpreted.

<a name="L1"></a><h3>Subscripts and Superscripts</h3>
Subscripts and superscripts are made with the <tt>_</tt> and <tt>^</tt>
commands. These commands can be combined to make complicated subscript and
superscript expressions. You may adjust the display of subscripts and
superscripts by using the two functions <tt>SetIndiceSize(Double_t)</tt>,
which set relative size of subscripts and superscripts, and
<tt>SetLimitIndiceSize(Int_t)</tt>, which set limits for text resizing of
subscipts and superscripts.
<p>Examples:
End_Html
   x^{2y} :    Begin_Latex x^{2y}    End_Latex x_{2y} :    Begin_Latex x^{2y}    End_Latex
   x^{y^{2}} : Begin_Latex x^{y^{2}} End_Latex x^{y_{1}} : Begin_Latex x^{y_{1}} End_Latex
   x^{y}_{1} : Begin_Latex x^{y}_{1} End_Latex x_{1}^{y} : Begin_Latex x_{1}^{y} End_Latex

Begin_Html
The subscripts and superscripts operators apply not only on one character but
on all the "normal text" preceding them. In the following example the second
<tt>E</tt> is lower than the first one because the operator <tt>_</tt> is
applied on <tt>/f</tt> which has a descending part, and not only on <tt>f</tt>
which as no descending part.
End_Html
   f_{E}/f_{E} : Begin_Latex f_{E}/f_{E} End_Latex

Begin_Html
   To make sure the second operator <tt>_</tt> applies only on <tt>f</tt>
   a dummy operator <tt>^{}</tt> should be introduced to separate the <tt>f</tt>
   from the <tt>/</tt>.
End_Html
   f_{E}/^{}f_{E} : Begin_Latex f_{E}/^{}f_{E} End_Latex

Begin_Html
<a name="L2"></a><h3>Fractions</h3>
Fractions denoted by the <tt>/</tt> symbol are made in the obvious way.
The <tt>#frac</tt> command is used for large fractions in displayed formula;
it has two arguments: the numerator and the denominator.
<p>Examples:
End_Html
   x = #frac{y+z/2}{y^{2}+1} : Begin_Latex x = #frac{y+z/2}{y^{2}+1} End_Latex

Begin_Html
<a name="L3"></a><h3>Splitting Lines</h3>
Text can be split in two lines via the command <tt>#splitline</tt>.
<p>Examples:
End_Html
   #splitline{21 April 2003}{14:02:30} : Begin_Latex #splitline{21 April 2003}{14:02:30} End_Latex

Begin_Html
<a name="L4"></a><h3>Roots</h3>
The <tt>#sqrt</tt> command produces the square root of its argument; it has
an optional first argument for other roots.
<p>Examples:
End_Html
   #sqrt{10} : Begin_Latex #sqrt{10} End_Latex #sqrt[3]{10} : Begin_Latex #sqrt[3]{10} End_Latex

Begin_Html
<a name="L5"></a><h3>Mathematical Symbols</h3>
TLatex can display dozens of special mathematical symbols. A few of them, such
as <tt>+</tt> and <tt>></tt> , are produced by typing the corresponding
keyboard character. Others are obtained with the commands in the following
table:
End_Html
Begin_Macro(source)
mathsymbols.C
End_Macro

Begin_Html
<a name="L6"></a><h3>Delimiters</h3>
TLatex provides 4 kinds of proportional delimiters:
<pre>
   #[]{....} or "a la" Latex #left[.....#right] : big square brackets
   #{}{....} or              #left{.....#right} : big curly brackets
   #||{....} or              #left|.....#right| : big absolute value symbols
   #(){....} or              #left(.....#right) : big parentheses
</pre>

<a name="L7"></a><h3>Greek Letters</h3>
The command to produce a lowercase Greek letter is obtained by adding a
<tt>#</tt> to the name of the letter. For an uppercase Greek letter, just
capitalize the first letter of the command name. Some letters have two
representations. The name of the second one (the "variation") starts with "var".
The following table gives the complete list:
End_Html
Begin_Macro(source)
greekletters.C
End_Macro

Begin_Html
<a name="L8"></a><h3>Accents</h3>
Several kind of accents are available:
End_Html
   #hat    = Begin_Latex #hat{a} End_Latex
   #check  = Begin_Latex #check{a} End_Latex
   #acute  = Begin_Latex #acute{a} End_Latex
   #grave  = Begin_Latex #grave{a} End_Latex
   #dot    = Begin_Latex #dot{a} End_Latex
   #ddot   = Begin_Latex #ddot{a} End_Latex
   #tilde  = Begin_Latex #tilde{a} End_Latex

Begin_Html
The special sign: <tt>#slash</tt> draws a slash on top of the text between brackets:
End_Html
   #slash{E}_{T} : Begin_Latex #slash{E}_{T} End_Latex

Begin_Html
Bar and vectors sign are done the following way:
End_Html
   #bar{a}: Begin_Latex #bar{a} End_Latex
   #vec{a}: Begin_Latex #vec{a} End_Latex

Begin_Html
<a name="L9"></a><h3>Changing Style</h3>
One can change the font, the text color, or the text size at any time using :
<tt>#font[font-number]{...}</tt>, <tt>#color[color-number]{...}</tt>
and <tt>#scale[scale-factor]{...}</tt>
<p>Examples:
End_Html
   #font[12]{Times Italic} and #font[22]{Times bold} : Begin_Latex #font[12]{Times Italic} and #font[22]{Times bold} End_Latex
   #color[2]{Red} and #color[4]{Blue} : Begin_Latex #color[2]{Red} and #color[4]{Blue} End_Latex
   #scale[1.2]{Bigger} and #scale[0.8]{Smaller} : Begin_Latex #scale[1.2]{Bigger} and #scale[0.8]{Smaller} End_Latex

Begin_Html
<a name="L10"></a><h3>Alignment Rules</h3>
The <tt>TText</tt> alignment rules apply to the <tt>TLatex</tt> objects with one exception
concerning the vertical alignment:
<ul>
<li> if the vertical alignment = 1 , subscripts are not taken into account </li>
<li> if the vertical alignment = 0 , the text is aligned to the box surrounding
                                     the full text with sub and superscripts</li>
</ul>
This is illustrated by the following example:
End_Html
Begin_Macro(source)
{
   TCanvas Tlva("Tlva","Tlva",500,500);
   Tlva.SetGrid();
   Tlva.DrawFrame(0,0,1,1);
   const char *longstring = "K_{S}... K^{*0}... #frac{2s}{#pi#alpha^{2}}
   #frac{d#sigma}{dcos#theta} (e^{+}e^{-} #rightarrow f#bar{f} ) =
   #left| #frac{1}{1 - #Delta#alpha} #right|^{2} (1+cos^{2}#theta)";

   TLatex latex;
   latex.SetTextSize(0.025);
   latex.SetTextAlign(13);  //align at top
   latex.DrawLatex(.2,.9,"K_{S}");
   latex.DrawLatex(.3,.9,"K^{*0}");
   latex.DrawLatex(.2,.8,longstring);

   latex.SetTextAlign(12);  //centered
   latex.DrawLatex(.2,.6,"K_{S}");
   latex.DrawLatex(.3,.6,"K^{*0}");
   latex.DrawLatex(.2,.5,longstring);

   latex.SetTextAlign(11);  //default bottom alignment
   latex.DrawLatex(.2,.4,"K_{S}");
   latex.DrawLatex(.3,.4,"K^{*0}");
   latex.DrawLatex(.2,.3,longstring);

   latex.SetTextAlign(10);  //special bottom alignment
   latex.DrawLatex(.2,.2,"K_{S}");
   latex.DrawLatex(.3,.2,"K^{*0}");
   latex.DrawLatex(.2,.1,longstring);

   latex.SetTextAlign(12);
   latex->SetTextFont(72);
   latex->DrawLatex(.1,.80,"13");
   latex->DrawLatex(.1,.55,"12");
   latex->DrawLatex(.1,.35,"11");
   latex->DrawLatex(.1,.18,"10");
   return Tlva;
}
End_Macro

Begin_Html
<a name="L11"></a><h3>Character Adjustement</h3>
The two commands <tt>#kern</tt> and <tt>#lower</tt> enable a better control
over character placement. The command <tt>#kern[(Float_t)dx]{text}</tt> moves
the output string horizontally by the fraction <tt>dx</tt> of its length.
Similarly, <tt>#lower[(Float_t)dy]{text}</tt> shifts the text up or down by
the fraction <tt>dy</tt> of its height.
<p>Examples:
End_Html
Positive k#kern[0.3]{e}#kern[0.3]{r}#kern[0.3]{n}#kern[0.3]{i}#kern[0.3]{n}#kern[0.3]{g}:
Begin_Latex Positive k#kern[0.3]{e}#kern[0.3]{r}#kern[0.3]{n}#kern[0.3]{i}#kern[0.3]{n}#kern[0.3]{g} End_Latex

Negative k#kern[-0.3]{e}#kern[-0.3]{r}#kern[-0.3]{n}#kern[-0.3]{i}#kern[-0.3]{n}#kern[-0.3]{g}:
Begin_Latex Negative k#kern[-0.3]{e}#kern[-0.3]{r}#kern[-0.3]{n}#kern[-0.3]{i}#kern[-0.3]{n}#kern[-0.3]{g} End_Latex

Vertical a#lower[0.2]{d}#lower[0.4]{j}#lower[0.1]{u}#lower[-0.1]{s}#lower[-0.3]{t}#lower[-0.4]{m}#lower[-0.2]{e}#lower[0.1]{n}t:
Begin_Latex Vertical a#lower[0.2]{d}#lower[0.4]{j}#lower[0.1]{u}#lower[-0.1]{s}#lower[-0.3]{t}#lower[-0.4]{m}#lower[-0.2]{e}#lower[0.1]{n}t End_Latex

Begin_Html
<a name="L12"></a><h3>Italic and Boldface</h3>
Text can be turned italic or boldface using the commands
<tt>#it</tt> and <tt>#bf</tt>.
<p>Examples:
End_Html
#bf{bold}, #it{italic}, #bf{#it{bold italic}}, #bf{#bf{unbold}}}:
Begin_Latex #bf{bold}, #it{italic}, #bf{#it{bold italic}}, #bf{#bf{unbold}} End_Latex

abc#alpha#beta#gamma, #it{abc#alpha#beta#gamma}:
Begin_Latex abc#alpha#beta#gamma, #it{abc#alpha#beta#gamma} End_Latex

Begin_Html
<a name="L13"></a><h3>Examples</h3>
End_Html
Begin_Macro(source)
{
   TCanvas ex1("ex1","Latex",500,600);
   TLatex Tl;
   Tl.SetTextAlign(12);
   Tl.SetTextSize(0.04);
   Tl.DrawLatex(0.1,0.8,"1)   C(x) = d #sqrt{#frac{2}{#lambdaD}}  #int^{x}_{0}cos(#frac{#pi}{2}t^{2})dt");
   Tl.DrawLatex(0.1,0.6,"2)   C(x) = d #sqrt{#frac{2}{#lambdaD}}  #int^{x}cos(#frac{#pi}{2}t^{2})dt");
   Tl.DrawLatex(0.1,0.4,"3)   R = |A|^{2} = #frac{1}{2}(#[]{#frac{1}{2}+C(V)}^{2}+#[]{#frac{1}{2}+S(V)}^{2})");
   Tl.DrawLatex(0.1,0.2,"4)   F(t) = #sum_{i=-#infty}^{#infty}A(i)cos#[]{#frac{i}{t+i}}");
   return ex1;
}
End_Macro
Begin_Macro(source)
{
   TCanvas ex2("ex2","Latex",500,300);
   TLatex Tl;
   Tl.SetTextAlign(23);
   Tl.SetTextSize(0.08);
   Tl.DrawLatex(0.5,0.95,"e^{+}e^{-}#rightarrowZ^{0}#rightarrowI#bar{I}, q#bar{q}");
   Tl.DrawLatex(0.5,0.75,"|#vec{a}#bullet#vec{b}|=#Sigmaa^{i}_{jk}+b^{bj}_{i}");
   Tl.DrawLatex(0.5,0.5,"i(#partial_{#mu}#bar{#psi}#gamma^{#mu}+m#bar{#psi}=0#Leftrightarrow(#Box+m^{2})#psi=0");
   Tl.DrawLatex(0.5,0.3,"L_{em}=eJ^{#mu}_{em}A_{#mu} , J^{#mu}_{em}=#bar{I}#gamma_{#mu}I , M^{j}_{i}=#SigmaA_{#alpha}#tau^{#alphaj}_{i}");
   return ex2;
}
End_Macro
Begin_Macro(source)
{
   TCanvas ex3("ex3","Latex",500,300);
   TPaveText pt(.1,.1,.9,.9);
   pt.AddText("#frac{2s}{#pi#alpha^{2}}  #frac{d#sigma}{dcos#theta} (e^{+}e^{-} #rightarrow f#bar{f} ) = ");
   pt.AddText("#left| #frac{1}{1 - #Delta#alpha} #right|^{2} (1+cos^{2}#theta");
   pt.AddText("+ 4 Re #left{ #frac{2}{1 - #Delta#alpha} #chi(s) #[]{#hat{g}_{#nu}^{e}#hat{g}_{#nu}^{f}
   (1 + cos^{2}#theta) + 2 #hat{g}_{a}^{e}#hat{g}_{a}^{f} cos#theta) } #right}");
   pt.SetLabel("Born equation");
   pt.Draw();
   return ex3;
}
End_Macro
*/


//______________________________________________________________________________
TLatex::TLatex()
{
   // Default constructor.

   fFactorSize  = 1.5;
   fFactorPos   = 0.6;
   fLimitFactorSize = 3;
   fError       = 0;
   fShow        = kFALSE;
   fPos=fTabMax = 0;
   fOriginSize  = 0.04;
   fTabSize     = 0;
   fItalic      = kFALSE;
   SetLineWidth(2);
}


//______________________________________________________________________________
TLatex::TLatex(Double_t x, Double_t y, const char *text)
       :TText(x,y,text)
{
   // Normal constructor.

   fFactorSize  = 1.5;
   fFactorPos   = 0.6;
   fError       = 0;
   fShow        = kFALSE;
   fPos         = 0;
   fTabMax      = 0;
   fOriginSize  = 0.04;
   fTabSize     = 0;
   fItalic      = kFALSE;
   fLimitFactorSize = 3;
   SetLineWidth(2);
}


//______________________________________________________________________________
TLatex::~TLatex()
{
   // Destructor.
}


//______________________________________________________________________________
TLatex::TLatex(const TLatex &text) : TText(text), TAttLine(text)
{
   // Copy constructor.

   fFactorSize  = 1.5;
   fFactorPos   = 0.6;
   fShow        = kFALSE;
   fPos         = 0;
   fTabMax      = 0;
   fOriginSize  = 0.04;
   fItalic      = kFALSE;
   fLimitFactorSize = 3;
   ((TLatex&)text).Copy(*this);
}

//______________________________________________________________________________
TLatex& TLatex::operator=(const TLatex& lt)
{
   //assignment operator
   if(this!=&lt) {
      TText::operator=(lt);
      TAttLine::operator=(lt);
      fFactorSize=lt.fFactorSize;
      fFactorPos=lt.fFactorPos;
      fLimitFactorSize=lt.fLimitFactorSize;
      fError=lt.fError;
      fShow=lt.fShow;
      fTabSize=lt.fTabSize;
      fOriginSize=lt.fOriginSize;
      fTabSize=lt.fTabSize;
      fTabSize=lt.fTabSize;
      fItalic=lt.fItalic;
   }
   return *this;
}

//______________________________________________________________________________
void TLatex::Copy(TObject &obj) const
{
   // Copy this TLatex object to another TLatex.

   ((TLatex&)obj).fFactorSize  = fFactorSize;
   ((TLatex&)obj).fFactorPos   = fFactorPos;
   ((TLatex&)obj).fLimitFactorSize  = fLimitFactorSize;
   ((TLatex&)obj).fError       = fError;
   ((TLatex&)obj).fShow        = fShow;
   ((TLatex&)obj).fTabSize     = 0;
   ((TLatex&)obj).fOriginSize  = fOriginSize;
   ((TLatex&)obj).fTabMax      = fTabMax;
   ((TLatex&)obj).fPos         = fPos;
   ((TLatex&)obj).fItalic      = fItalic;
   TText::Copy(obj);
   TAttLine::Copy(((TAttLine&)obj));
}


//______________________________________________________________________________
TLatexFormSize TLatex::Anal1(TextSpec_t spec, const Char_t* t, Int_t length)
{
   // Analyse function.

   return Analyse(0,0,spec,t,length);
}


//______________________________________________________________________________
TLatexFormSize TLatex::Analyse(Double_t x, Double_t y, TextSpec_t spec, const Char_t* t, Int_t length)
{
   //  Analyse and paint the TLatex formula
   //
   //  It is called twice : first for calculating the size of
   //  each portion of the formula, then to paint the formula.
   //  When analyse finds an operator or separator, it calls
   //  itself recursively to analyse the arguments of the operator.
   //  when the argument is an atom (normal text), it calculates
   //  the size of it and return it as the result.
   //  for example : if the operator #frac{arg1}{arg2} is found :
   //  Analyse(arg1) return the size of arg1 (width, up, down)
   //  Analyse(arg2) return the size of arg2
   //  now, we know the size of #frac{arg1}{arg2}  :
   //  width = max(width_arg1, width_arg2)
   //  up = up_arg1 + down_arg1
   //  down = up_arg2 + down_arg2
   //  so, when the user wants to paint a fraction at position (x,y),
   //  the rect used for the formula is : (x,y-up,x+width,y+down)
   //
   // return size of zone occupied by the text/formula
   // t : chain to be analyzed
   // length : number of chars in t.

   const char *tab[] = { "alpha","beta","chi","delta","varepsilon","phi","gamma","eta","iota","varphi","kappa","lambda",
                "mu","nu","omicron","pi","theta","rho","sigma","tau","upsilon","varomega","omega","xi","psi","zeta",
                "Alpha","Beta","Chi","Delta","Epsilon","Phi","Gamma","Eta","Iota","vartheta",
                "Kappa","Lambda","Mu","Nu","Omicron","Pi","Theta","Rho","Sigma","Tau",
                "Upsilon","varsigma","Omega","Xi","Psi","Zeta","varUpsilon","epsilon"};

   const char *tab2[] = { "leq","/","infty","voidb","club","diamond","heart",
                 "spade","leftrightarrow","leftarrow","uparrow","rightarrow",
                 "downarrow","circ","pm","doublequote","geq","times","propto",
                 "partial","bullet","divide","neq","equiv","approx","3dots",
                 "cbar","topbar","downleftarrow","aleph","Jgothic","Rgothic","voidn",
                 "otimes","oplus","oslash","cap","cup","supset","supseteq",
                 "notsubset","subset","subseteq","in","notin","angle","nabla",
                 "oright","ocopyright","trademark","prod","surd","upoint","corner","wedge",
                 "vee","Leftrightarrow","Leftarrow","Uparrow","Rightarrow",
                 "Downarrow","diamond","LT","void1","copyright","void3","sum",
                 "arctop","lbar","arcbottom","topbar","void8", "bottombar","arcbar",
                 "ltbar","AA","aa","void06","GT","int","forall","exists" };

   const char *tab3[] = { "bar","vec","dot","hat","ddot","acute","grave","check","tilde","slash"};

   if (fError != 0) return TLatexFormSize(0,0,0);

   Int_t nBlancDeb=0,nBlancFin=0,l_nBlancDeb=0,l_nBlancFin=0;
   Int_t i,k;
   Int_t min=0, max=0;
   Bool_t cont = kTRUE;
   while(cont) {
      // count leading blanks
      //while(nBlancDeb+nBlancFin<length && t[nBlancDeb]==' ') nBlancDeb++;

      if (nBlancDeb==length) return TLatexFormSize(0,0,0); // empty string

      // count trailing blanks
      //while(nBlancDeb+nBlancFin<length && t[length-nBlancFin-1]==' ') nBlancFin++;

      if (nBlancDeb==l_nBlancDeb && nBlancFin==l_nBlancFin) cont = kFALSE;

      // remove characters { }
      if (t[nBlancDeb]=='{' && t[length-nBlancFin-1]=='}') {
         Int_t nBrackets = 0;
         Bool_t sameBrackets = kTRUE;
         for(i=nBlancDeb;i<length-nBlancFin;i++) {
            if (t[i] == '{' && !(i>0 && t[i-1] == '@')) nBrackets++;
            if (t[i] == '}' && t[i-1]!= '@') nBrackets--;
            if (nBrackets==0 && i<length-nBlancFin-2) {
               sameBrackets=kFALSE;
               break;
            }
         }

         if (sameBrackets) {
            // begin and end brackets match
            nBlancDeb++;
            nBlancFin++;
            if (nBlancDeb+nBlancFin==length) return TLatexFormSize(0,0,0); // empty string
            cont = kTRUE;
         }

      }

      l_nBlancDeb = nBlancDeb;
      l_nBlancFin = nBlancFin;
   }

   // make a copy of the current processed chain of characters
   // removing leading and trailing blanks
   length -= nBlancFin+nBlancDeb; // length of string without blanks
   Char_t* text = new Char_t[length+1];
   strncpy(text,t+nBlancDeb,length);
   text[length] = 0;

   // compute size of subscripts and superscripts
   Double_t indiceSize = spec.fSize/fFactorSize;
   if(indiceSize<fOriginSize/TMath::Exp(fLimitFactorSize*TMath::Log(fFactorSize))-0.001f)
      indiceSize = spec.fSize;
   // substract 0.001 because of rounding errors
   TextSpec_t specNewSize = spec;
   specNewSize.fSize       = indiceSize;

   // recherche des operateurs
   Int_t opPower         = -1;   // Position of first ^ (power)
   Int_t opUnder         = -1;   // Position of first _ (indice)
   Int_t opFrac          = -1;   // Position of first \frac
   Int_t opSqrt          = -1;   // Position of first \sqrt
   Int_t nBrackets       = 0;    // Nesting level in { }
   Int_t nCroch          = 0;    // Nesting level in [ ]
   Int_t opCurlyCurly    = -1;   // Position of first }{
   Int_t opSquareCurly   = -1;   // Position of first ]{
   Int_t opCloseCurly    = -2;   // Position of first }
   Int_t opColor         = -1;   // Position of first \color
   Int_t opFont          = -1;   // Position of first \font
   Int_t opScale         = -1;   // Position of first \scale
   Int_t opGreek         = -1;   // Position of a Greek letter
   Int_t opSpec          = -1;   // position of a special character
   Int_t opAbove         = -1;   // position of a vector/overline
   Int_t opSquareBracket = 0 ;   // position of a "[]{" operator (#[]{arg})
   Int_t opBigCurly      = 0 ;   // position of a "{}{" operator (big curly bracket #{}{arg})
   Int_t opAbs           = 0 ;   // position of a "||{" operator (absolute value) (#||{arg})
   Int_t opParen         = 0 ;   // position of a "(){" operator (big parenthesis #(){arg})
   Int_t abovePlace      = 0 ;   // true if subscripts must be written above and not after
   Int_t opBox           = 0 ;   // position of #Box
   Int_t opPerp          = 0;    // position of #perp
   Int_t opOdot          = 0;    // position of #odot
   Int_t opHbar          = 0;    // position of #hbar
   Int_t opParallel      = 0;    // position of #parallel
   Int_t opSplitLine     = -1;   // Position of first \splitline
   Int_t opKern          = -1;   // Position of first #kern
   Int_t opLower         = -1;   // Position of first #lower
   Int_t opBf            = -1;   // Position of first #bf
   Int_t opIt            = -1;   // Position of first #it
   Bool_t opFound = kFALSE;
   Bool_t quote1 = kFALSE, quote2 = kFALSE ;

   for(i=0;i<length;i++) {
      switch (text[i]) {
         case '\'' : quote1 = !quote1 ; break ;
         case '"'  : quote2  = !quote2 ; break ;
      }
      //if (quote1 || quote2) continue ;
      switch (text[i]) {
         case '{':
            if (nCroch==0) {
               if (!(i>0 && text[i-1] == '@')) nBrackets++;
            }
            break;
         case '}':
            if (nCroch==0) {
               if (!(i>0 && text[i-1] == '@')) nBrackets--;
               if (nBrackets==0) {
                  if (i<length-1) if (text[i+1]=='{' && opCurlyCurly==-1) opCurlyCurly=i;
                  if (i<length-2) {
                     if (text[i+1]!='{' && !(text[i+2]=='{' && (text[i+1]=='^' || text[i+1]=='_'))
                     && opCloseCurly==-2) opCloseCurly=i;
                  }
                  else if (i<length-1) {
                     if (text[i+1]!='{' && opCloseCurly==-2) opCloseCurly=i;
                  }
                  else if (opCloseCurly==-2) opCloseCurly=i;
               }
            }
            break;
         case '[':
            if (nBrackets==0) {
               if (!(i>0 && text[i-1] == '@')) nCroch++;
            }
            break;
         case ']':
            if (nBrackets==0) {
               if (!(i>0 && text[i-1] == '@')) nCroch--;
               if (nCroch<0) {
                  // more "]" than "["
                  fError = "Missing \"[\"";
                  delete [] text;
                  return TLatexFormSize(0,0,0);
               }
            }
            break;
      }
      if (length>i+1) {
         Char_t buf[3];
         strncpy(buf,&text[i],2);
         if (strncmp(buf,"^{",2)==0) {
            if (opPower==-1 && nBrackets==0 && nCroch==0) opPower=i;
            if (i>3) {
               Char_t buf1[5];
               strncpy(buf1,&text[i-4],4);
               if (strncmp(buf1,"#int",4)==0) {
                  abovePlace = 1;
                  if (i>4 && opCloseCurly==-2) opCloseCurly=i-5;
               }
               if (strncmp(buf1,"#sum",4)==0) {
                  abovePlace = 2;
                  if (i>4 && opCloseCurly==-2) opCloseCurly=i-5;
               }
            }
         }
         if (strncmp(buf,"_{",2)==0) {
            if (opUnder==-1 && nBrackets==0 && nCroch==0) opUnder=i;
            if (i>3) {
               Char_t buf2[5];
               strncpy(buf2,&text[i-4],4);
               if (strncmp(buf2,"#int",4)==0) {
                  abovePlace = 1;
                  if (i>4 && opCloseCurly==-2) opCloseCurly=i-5;
               }
               if (strncmp(buf2,"#sum",4)==0) {
                  abovePlace = 2;
                  if (i>4 && opCloseCurly==-2) opCloseCurly=i-5;
               }
            }
         }
         if (strncmp(buf,"]{",2)==0)
            if (opSquareCurly==-1 && nBrackets==0 && nCroch==0) opSquareCurly=i;
      }
      // detect other operators
      if (text[i]=='\\' || (text[i]=='#' && !opFound && nBrackets==0 && nCroch==0)) {

         if (length>i+10 ) {
            Char_t buf[11];
            strncpy(buf,&text[i+1],10);
            if (strncmp(buf,"splitline{",10)==0) {
               opSplitLine=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
         }
         if (length>i+8 ) {
            Char_t buf[9];
            strncpy(buf,&text[i+1],8);
            if (!opParallel && strncmp(buf,"parallel",8)==0) {
               opParallel=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
         }
         if (length>i+6) {
            Char_t buf[7];
            strncpy(buf,&text[i+1],6);
            if (strncmp(buf,"lower[",6)==0 || strncmp(buf,"lower{",6)==0) {
               opLower=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue ;
            }
            if (strncmp(buf,"scale[",6)==0 || strncmp(buf,"scale{",6)==0) {
               opScale=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue ;
            }
            if (strncmp(buf,"color[",6)==0 || strncmp(buf,"color{",6)==0) {
               opColor=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue ;
            }
         }
         if (length>i+5 ) {
            Char_t buf[6];
            strncpy(buf,&text[i+1],5);
            if (strncmp(buf,"frac{",5)==0) {
               opFrac=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (strncmp(buf,"sqrt{",5)==0 || strncmp(buf,"sqrt[",5)==0) {
               opSqrt=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (strncmp(buf,"font{",5)==0 || strncmp(buf,"font[",5)==0) {
               opFont=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (strncmp(buf,"kern[",5)==0 || strncmp(buf,"kern{",5)==0) {
               opKern=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue ;
            }
         }
         if (length>i+4 ) {
            Char_t buf[5];
            strncpy(buf,&text[i+1],4);
            if (!opOdot && strncmp(buf,"odot",4)==0) {
               opOdot=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (!opHbar && strncmp(buf,"hbar",4)==0) {
               opHbar=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (!opPerp && strncmp(buf,"perp",4)==0) {
               opPerp=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
         }
         if (length>i+3) {
            Char_t buf[4];
            strncpy(buf,&text[i+1],3);
            buf[3] = 0;
            if (strncmp(buf,"[]{",3)==0) {
               opSquareBracket=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (strncmp(buf,"{}{",3)==0 ) {
               opBigCurly=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (strncmp(buf,"||{",3)==0) {
               opAbs=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (strncmp(buf,"(){",3)==0) {
               opParen=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (!opBox && strncmp(buf,"Box",3)==0) {
               opBox=1; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue;
            }
            if (strncmp(buf,"bf[",3)==0 || strncmp(buf,"bf{",3)==0) {
               opBf=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue ;
            }
            if (strncmp(buf,"it[",3)==0 || strncmp(buf,"it{",3)==0) {
               opIt=i; opFound = kTRUE;
               if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               continue ;
            }
         }
         for(k=0;k<54;k++) {
            if (!opFound && UInt_t(length)>i+strlen(tab[k])) {
               if (strncmp(&text[i+1],tab[k],strlen(tab[k]))==0) {
                  opGreek=k;
                  opFound = kTRUE;
                  if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               }
            }
         }
         for(k=0;k<10;k++) {
            if (!opFound && UInt_t(length)>i+strlen(tab3[k])) {
               if (strncmp(&text[i+1],tab3[k],strlen(tab3[k]))==0) {
                  opAbove=k;
                  opFound = kTRUE;
                  if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               }
            }
         }
         UInt_t lastsize = 0;
         if (!opFound)
         for(k=0;k<82;k++) {
            if ((opSpec==-1 || strlen(tab2[k])>lastsize) && UInt_t(length)>i+strlen(tab2[k])) {
               if (strncmp(&text[i+1],tab2[k],strlen(tab2[k]))==0) {
                  lastsize = strlen(tab2[k]);
                  opSpec=k;
                  opFound = kTRUE;
                  if (i>0 && opCloseCurly==-2) opCloseCurly=i-1;
               }
            }
         }
      }
   }

   TLatexFormSize fs1;
   TLatexFormSize fs2;
   TLatexFormSize fs3;
   TLatexFormSize result;

   // analysis of operators found
   if (opCloseCurly>-1 && opCloseCurly<length-1) { // separator } found
      if(!fShow) {
         fs1 = Anal1(spec,text,opCloseCurly+1);
         fs2 = Anal1(spec,text+opCloseCurly+1,length-opCloseCurly-1);
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Analyse(x+fs1.Width(),y,spec,text+opCloseCurly+1,length-opCloseCurly-1);
         Analyse(x,y,spec,text,opCloseCurly+1);
      }
      result = fs1+fs2;
   }

   else if (opPower>-1 && opUnder>-1) { // ^ and _ found
      min = TMath::Min(opPower,opUnder);
      max = TMath::Max(opPower,opUnder);
      Double_t xfpos = 0. ; //GetHeight()*spec.fSize/5.;
      Double_t prop=1, propU=1; // scale factor for #sum & #int
      switch (abovePlace) {
         case 1 :
            prop = .8 ; propU = 1.75 ; // Int
            break;
         case 2:
            prop = .9 ; propU = 1.75 ; // Sum
            break;
      }
     // propU acts on upper number
     // when increasing propU value, the upper indice position is higher
     // when increasing prop values, the lower indice position is lower

      if (!fShow) {
         Int_t ltext = min ;
         if (min >= 2 && strncmp(&text[min-2],"{}",2)==0) {
            // upper and lower indice before the character
            // like with chemical element
            sprintf(&text[ltext-2],"I ") ;
            ltext-- ;
         }
         fs1 = Anal1(spec,text,ltext);
         fs2 = Anal1(specNewSize,text+min+1,max-min-1);
         fs3 = Anal1(specNewSize,text+max+1,length-max-1);
         Savefs(&fs1);
         Savefs(&fs2);
         Savefs(&fs3);
      } else {
         fs3 = Readfs();
         fs2 = Readfs();
         fs1 = Readfs();
         Double_t pos = 0;
         if (!abovePlace) {
            Double_t addW = fs1.Width()+xfpos, addH1, addH2;
            if (opPower<opUnder) {
               addH1 = -fs1.Over()*(fFactorPos)-fs2.Under();
               addH2 = fs1.Under()+fs3.Over()*(fFactorPos);
            } else {
               addH1 = fs1.Under()+fs2.Over()*(fFactorPos);
               addH2 = -fs1.Over()*(fFactorPos)-fs3.Under();
            }
            Analyse(x+addW,y+addH2,specNewSize,text+max+1,length-max-1);
            Analyse(x+addW,y+addH1,specNewSize,text+min+1,max-min-1);
         } else {
            Double_t addW1, addW2, addH1, addH2;
            Double_t m = TMath::Max(fs1.Width(),TMath::Max(fs2.Width(),fs3.Width()));
            pos = (m-fs1.Width())/2;
            if (opPower<opUnder) {
               addH1 = -fs1.Over()*propU-fs2.Under();
               addW1 = (m-fs2.Width())/2;
               addH2 = fs1.Under()*prop+fs3.Over();
               addW2 = (m-fs3.Width())/2;
            } else {
               addH1 = fs1.Under()*prop+fs2.Over();
               addW1 = (m-fs2.Width())/2;
               addH2 = -fs1.Over()*propU-fs3.Under();
               addW2 = (m-fs3.Width())/2;
            }

            Analyse(x+addW2,y+addH2,specNewSize,text+max+1,length-max-1);
            Analyse(x+addW1,y+addH1,specNewSize,text+min+1,max-min-1);
         }

         if (min >= 2 && strncmp(&text[min-2],"{}",2)==0) {
            sprintf(&text[min-2],"  ") ;
            Analyse(x+pos,y,spec,text,min-1);
         } else {
            Analyse(x+pos,y,spec,text,min);
         }
      }

      if (!abovePlace) {
         if (opPower<opUnder) {
            result.Set(fs1.Width()+xfpos+TMath::Max(fs2.Width(),fs3.Width()),
                       fs1.Over()*fFactorPos+fs2.Height(),
                       fs1.Under()+fs3.Height()-fs3.Over()*(1-fFactorPos));
         } else {
            result.Set(fs1.Width()+xfpos+TMath::Max(fs2.Width(),fs3.Width()),
                       fs1.Over()*fFactorPos+fs3.Height(),
                       fs1.Under()+fs2.Height()-fs2.Over()*(1-fFactorPos));
         }
      } else {
         if (opPower<opUnder) {
            result.Set(TMath::Max(fs1.Width(),TMath::Max(fs2.Width(),fs3.Width())),
                       fs1.Over()*propU+fs2.Height(),fs1.Under()*prop+fs3.Height());
         } else {
            result.Set(TMath::Max(fs1.Width(),TMath::Max(fs2.Width(),fs3.Width())),
                       fs1.Over()*propU+fs3.Height(),fs1.Under()*prop+fs2.Height());
         }
      }
   }
   else if (opPower>-1) { // ^ found
      Double_t prop=1;
      Double_t xfpos = 0. ; //GetHeight()*spec.fSize/5. ;
      switch (abovePlace) {
         case 1 : //int
            prop = 1.75 ; break ;
         case 2 : // sum
            prop = 1.75;  break ;
      }
      // When increasing prop, the upper indice position is higher
      if (!fShow) {
         Int_t ltext = opPower ;
         if (ltext >= 2 && strncmp(&text[ltext-2],"{}",2)==0) {
            // upper and lower indice before the character
            // like with chemical element
            sprintf(&text[ltext-2],"I ") ;
            ltext-- ;
         }
         fs1 = Anal1(spec,text,ltext);
         fs2 = Anal1(specNewSize,text+opPower+1,length-opPower-1);
         Savefs(&fs1);
         Savefs(&fs2);
      } else {
         fs2 = Readfs();
         fs1 = Readfs();
         Int_t pos = 0;
         if (!abovePlace){
            Double_t over = fs1.Over();
            if (over <= 0) over = 1.5*fs2.Over();
            Analyse(x+fs1.Width()+xfpos,y-over*fFactorPos-fs2.Under(),specNewSize,text+opPower+1,length-opPower-1);
         } else {
            Int_t pos2=0;
            if (fs2.Width()>fs1.Width())
               pos=Int_t((fs2.Width()-fs1.Width())/2);
            else
               pos2=Int_t((fs1.Width()-fs2.Width())/2);

            Analyse(x+pos2,y-fs1.Over()*prop-fs2.Under(),specNewSize,text+opPower+1,length-opPower-1);
         }
         if (opPower >= 2 && strncmp(&text[opPower-2],"{}",2)==0) {
            sprintf(&text[opPower-2],"  ") ;
            Analyse(x+pos,y,spec,text,opPower-1);
         } else {
            Analyse(x+pos,y,spec,text,opPower);
         }
      }

      if (!abovePlace)
         result.Set(fs1.Width()+xfpos+fs2.Width(),
                    fs1.Over()*fFactorPos+fs2.Over(),fs1.Under());
      else
         result.Set(TMath::Max(fs1.Width(),fs2.Width()),fs1.Over()*prop+fs2.Height(),fs1.Under());

   }
   else if (opUnder>-1) { // _ found
      Double_t prop = .9; // scale factor for #sum & #frac
      Double_t xfpos = 0.;//GetHeight()*spec.fSize/5. ;
      Double_t fpos = fFactorPos ;
      // When increasing prop, the lower indice position is lower
      if(!fShow) {
         Int_t ltext = opUnder ;
         if (ltext >= 2 && strncmp(&text[ltext-2],"{}",2)==0) {
            // upper and lower indice before the character
            // like with chemical element
            sprintf(&text[ltext-2],"I ") ;
            ltext-- ;
         }
         fs1 = Anal1(spec,text,ltext);
         fs2 = Anal1(specNewSize,text+opUnder+1,length-opUnder-1);
         Savefs(&fs1);
         Savefs(&fs2);
      } else {
         fs2 = Readfs();
         fs1 = Readfs();
         Int_t pos = 0;
         if (!abovePlace)
            Analyse(x+fs1.Width()+xfpos,y+fs1.Under()+fs2.Over()*fpos,specNewSize,text+opUnder+1,length-opUnder-1);
         else {
            Int_t pos2=0;
            if (fs2.Width()>fs1.Width())
               pos=Int_t((fs2.Width()-fs1.Width())/2);
            else
               pos2=Int_t((fs1.Width()-fs2.Width())/2);

            Analyse(x+pos2,y+fs1.Under()*prop+fs2.Over(),specNewSize,text+opUnder+1,length-opUnder-1);
         }
         if (opUnder >= 2 && strncmp(&text[opUnder-2],"{}",2)==0) {
            sprintf(&text[opUnder-2],"  ") ;
            Analyse(x+pos,y,spec,text,opUnder-1);
         } else {
            Analyse(x+pos,y,spec,text,opUnder);
         }
      }
      if (!abovePlace)
         result.Set(fs1.Width()+xfpos+fs2.Width(),fs1.Over(),
                    fs1.Under()+fs2.Under()+fs2.Over()*fpos);
      else
         result.Set(TMath::Max(fs1.Width(),fs2.Width()),fs1.Over(),fs1.Under()*prop+fs2.Height());
   }
   else if (opBox) {
      Double_t square = GetHeight()*spec.fSize/2;
      if (!fShow) {
         fs1 = Anal1(spec,text+4,length-4);
      } else {
         fs1 = Analyse(x+square,y,spec,text+4,length-4);
         Double_t adjust = GetHeight()*spec.fSize/20;
         Double_t x1 = x+adjust ;
         Double_t x2 = x-adjust+square ;
         Double_t y1 = y;
         Double_t y2 = y-square+adjust;
         DrawLine(x1,y1,x2,y1,spec);
         DrawLine(x2,y1,x2,y2,spec);
         DrawLine(x2,y2,x1,y2,spec);
         DrawLine(x1,y2,x1,y1,spec);
      }
      result = fs1 + TLatexFormSize(square,square,0);
   }
   else if (opOdot) {
      Double_t square = GetHeight()*spec.fSize/2;
      if (!fShow) {
         fs1 = Anal1(spec,text+5,length-5);
      } else {
         fs1 = Analyse(x+1.3*square,y,spec,text+5,length-5);
         Double_t adjust = GetHeight()*spec.fSize/20;
         Double_t r1 = 0.62*square;
         Double_t y1 = y-0.3*square-adjust;
         DrawCircle(x+0.6*square,y1,r1,spec) ;
         DrawCircle(x+0.6*square,y1,r1/100,spec) ;
      }
      result = fs1 + TLatexFormSize(square,square,0);
   }
   else if (opHbar) {
      Double_t square = GetHeight()*spec.fSize/2;
      if (!fShow) {
         fs1 = Anal1(spec,text+5,length-5);
      } else {
         fs1 = Analyse(x+square,y,spec,text+5,length-5);
         TText hbar;
         hbar.SetTextFont(12);
         hbar.SetTextColor(fTextColor);
         hbar.SetTextSize(spec.fSize);
         hbar.SetTextAngle(fTextAngle);
         Double_t xOrigin = (Double_t)gPad->XtoAbsPixel(fX);
         Double_t yOrigin = (Double_t)gPad->YtoAbsPixel(fY);
         Double_t angle   = kPI*spec.fAngle/180.;
         Double_t xx = gPad->AbsPixeltoX(Int_t((x-xOrigin)*TMath::Cos(angle)+(y-yOrigin)*TMath::Sin(angle)+xOrigin));
         Double_t yy = gPad->AbsPixeltoY(Int_t((x-xOrigin)*TMath::Sin(-angle)+(y-yOrigin)*TMath::Cos(angle)+yOrigin));
         hbar.PaintText(xx,yy,"h");
         DrawLine(x,y-0.8*square,x+0.75*square,y-square,spec);
      }
      result = fs1 + TLatexFormSize(square,square,0);
   }
   else if (opPerp) {
      Double_t square = GetHeight()*spec.fSize/1.4;
      if (!fShow) {
         fs1 = Anal1(spec,text+5,length-5);
      } else {
         fs1 = Analyse(x+0.5*square,y,spec,text+5,length-5);
         Double_t x0 = x  + 0.50*square;
         Double_t x1 = x0 - 0.48*square;
         Double_t x2 = x0 + 0.48*square;
         Double_t y1 = y  + 0.6*square;
         Double_t y2 = y1 - 1.3*square;
         DrawLine(x1,y1,x2,y1,spec);
         DrawLine(x0,y1,x0,y2,spec);
      }
      result = fs1;
   }
   else if (opParallel) {
      Double_t square = GetHeight()*spec.fSize/1.4;
      if (!fShow) {
         fs1 = Anal1(spec,text+9,length-9);
      } else {
         fs1 = Analyse(x+0.5*square,y,spec,text+9,length-9);
         Double_t x1 = x + 0.15*square;
         Double_t x2 = x + 0.45*square;
         Double_t y1 = y + 0.3*square;
         Double_t y2 = y1- 1.3*square;
         DrawLine(x1,y1,x1,y2,spec);
         DrawLine(x2,y1,x2,y2,spec);
      }
      result = fs1 + TLatexFormSize(square,square,0);
   }
   else if (opGreek>-1) {
      TextSpec_t newSpec = spec;
      newSpec.fFont = fItalic ? 152 : 122;
      char letter = 97 + opGreek;
      Double_t yoffset = 0.; // Greek letter too low
      if (opGreek>25) letter -= 58;
      if (opGreek == 52) letter = '\241'; //varUpsilon
      if (opGreek == 53) letter = '\316'; //epsilon
      if (!fShow) {
         fs1 = Anal1(newSpec,&letter,1);
         fs2 = Anal1(spec,text+strlen(tab[opGreek])+1,length-strlen(tab[opGreek])-1);
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Analyse(x+fs1.Width(),y,spec,text+strlen(tab[opGreek])+1,length-strlen(tab[opGreek])-1);
         Analyse(x,y-yoffset,newSpec,&letter,1);
      }
      fs1.AddOver(TLatexFormSize(0,yoffset,0)) ;
      result = fs1+fs2;
   }

   else if (opSpec>-1) {
      TextSpec_t newSpec = spec;
      newSpec.fFont = fItalic ? 152 : 122;
      char letter = '\243' + opSpec;
      if(opSpec == 75 || opSpec == 76) {
         newSpec.fFont = GetTextFont();
         if (opSpec == 75) letter = '\305'; // AA Angstroem
         if (opSpec == 76) letter = '\345'; // aa Angstroem
      }
      if(opSpec == 80 || opSpec == 81) {
         if (opSpec == 80) letter = '\042'; // #forall
         if (opSpec == 81) letter = '\044'; // #exists
      }
      Double_t props, propi;
      props = 1.8 ; // scale factor for #sum(66)
      propi = 2.3 ; // scale factor for  #int(79)

      if (opSpec==66 ) {
         newSpec.fSize = spec.fSize*props;
      } else if (opSpec==79) {
         newSpec.fSize = spec.fSize*propi;
      }
      if (!fShow) {
         fs1 = Anal1(newSpec,&letter,1);
         if (opSpec == 79 || opSpec == 66)
            fs1.Set(fs1.Width(),fs1.Over()*0.45,fs1.Over()*0.45);

         fs2 = Anal1(spec,text+strlen(tab2[opSpec])+1,length-strlen(tab2[opSpec])-1);
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Analyse(x+fs1.Width(),y,spec,text+strlen(tab2[opSpec])+1,length-strlen(tab2[opSpec])-1);
         if (opSpec!=66 && opSpec!=79)
            Analyse(x,y,newSpec,&letter,1);
         else {
               Analyse(x,y+fs1.Under()/2.,newSpec,&letter,1);
         }
      }
      result = fs1+fs2;
   }
   else if (opAbove>-1) {
      if (!fShow) {
         fs1 = Anal1(spec,text+strlen(tab3[opAbove])+1,length-strlen(tab3[opAbove])-1);
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Analyse(x,y,spec,text+strlen(tab3[opAbove])+1,length-strlen(tab3[opAbove])-1);
         Double_t sub = GetHeight()*spec.fSize/14;
         Double_t x1 , y1 , x2, y2, x3, x4;
         switch(opAbove) {
         case 0: // bar
            Double_t ypos  ;
            ypos = y-fs1.Over()-sub ;//-GetHeight()*spec.fSize/4. ;
            DrawLine(x,ypos,x+fs1.Width(),ypos,spec);
            break;
         case 1: // vec
            Double_t y0 ;
            y0 = y-sub-fs1.Over() ;
            y1 = y0-GetHeight()*spec.fSize/8 ;
            x1 = x+fs1.Width() ;
            DrawLine(x,y1,x1,y1,spec);
            DrawLine(x1,y1,x1-GetHeight()*spec.fSize/4,y0-GetHeight()*spec.fSize/4,spec);
            DrawLine(x1,y1,x1-GetHeight()*spec.fSize/4,y0,spec);
            break;
         case 2: // dot
            x1 = x+fs1.Width()/2-3*sub/4 ;
            x2 = x+fs1.Width()/2+3*sub/4 ;
            y1 = y-sub-fs1.Over() ;
            DrawLine(x1,y1,x2,y1,spec);
            break;
         case 3: // hat
            x2 = x+fs1.Width()/2 ;
            y1 = y -9*sub;
            y2 = y1-2*sub;
            x1 = x2-fs1.Width()/3 ;
            x3 = x2+fs1.Width()/3 ;
            DrawLine(x1,y1,x2,y2,spec);
            DrawLine(x2,y2,x3,y1,spec);
            break;
         case 4: // ddot
            x1 = x+fs1.Width()/2-9*sub/4 ;
            x2 = x+fs1.Width()/2-3*sub/4 ;
            x3 = x+fs1.Width()/2+9*sub/4 ;
            x4 = x+fs1.Width()/2+3*sub/4 ;
            y1 = y-sub-fs1.Over() ;
            DrawLine(x1,y1,x2,y1,spec);
            DrawLine(x3,y1,x4,y1,spec);
            break;
         case 5: // acute
            x1 = x+fs1.Width()/2;
            y1 = y +sub -fs1.Over() ;
            x2 = x1 +3*sub;
            y2 = y1 -2.5*sub;
            DrawLine(x1,y1,x2,y2,spec);
            break;
         case 6: // grave
            x1 = x+fs1.Width()/2-sub;
            y1 = y-sub-fs1.Over() ;
            x2 = x1 +2*sub;
            y2 = y1 +2*sub;
            DrawLine(x1,y1,x2,y2,spec);
            break;
         case 7: // check
            x1 = x+fs1.Width()/2 ;
            x2 = x1 -2*sub ;
            x3 = x1 +2*sub ;
            y1 = y-sub-fs1.Over() ;
            DrawLine(x2,y-3*sub-fs1.Over(),x1,y1,spec);
            DrawLine(x3,y-3*sub-fs1.Over(),x1,y1,spec);
            break;
         case 8: // tilde
            x2 = x+fs1.Width()/2 ;
            y2 = y -fs1.Over() ;
            {
               // tilde must be drawn separately on screen and on PostScript
               // because an adjustment is required along Y for PostScript.
               TVirtualPS *saveps = gVirtualPS;
               if (gVirtualPS) gVirtualPS = 0;
               Double_t sinang  = TMath::Sin(spec.fAngle/180*kPI);
               Double_t cosang  = TMath::Cos(spec.fAngle/180*kPI);
               Double_t xOrigin = (Double_t)gPad->XtoAbsPixel(fX);
               Double_t yOrigin = (Double_t)gPad->YtoAbsPixel(fY);
               Double_t xx  = gPad->AbsPixeltoX(Int_t((x2-xOrigin)*cosang+(y2-yOrigin)*sinang+xOrigin));
               Double_t yy  = gPad->AbsPixeltoY(Int_t((x2-xOrigin)*-sinang+(y2-yOrigin)*cosang+yOrigin));
               TText tilde;
               tilde.SetTextFont(fTextFont);
               tilde.SetTextColor(fTextColor);
               tilde.SetTextSize(0.9*spec.fSize);
               tilde.SetTextAlign(22);
               tilde.SetTextAngle(fTextAngle);
               tilde.PaintText(xx,yy,"~");
               if (saveps) {
                  y2 -= 4*sub;
                  xx  = gPad->AbsPixeltoX(Int_t((x2-xOrigin)*cosang+(y2-yOrigin)*sinang+xOrigin));
                  yy  = gPad->AbsPixeltoY(Int_t((x2-xOrigin)*-sinang+(y2-yOrigin)*cosang+yOrigin));
                  gVirtualPS = saveps;
                  gVirtualPS->SetTextAlign(22);
                  gVirtualPS->Text(xx, yy, "~");
               }
            }
            break;
         case 9: // slash
            x1 = x + 0.8*fs1.Width();
            y1 = y -fs1.Over() ;
            x2 = x + 0.3*fs1.Width();
            y2 = y1 + 1.2*fs1.Height();
            DrawLine(x1,y1,x2,y2,spec);
            break;
         }
      }
      Double_t div = 3;
      if (opAbove==1) div=4;
      result.Set(fs1.Width(),fs1.Over()+GetHeight()*spec.fSize/div,fs1.Under());
   }
   else if (opSquareBracket) { // operator #[]{arg}
      Double_t l = GetHeight()*spec.fSize/4;
      Double_t l2 = l/2 ;
      if (!fShow) {
         fs1 = Anal1(spec,text+3,length-3);
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Analyse(x+l2+l,y,spec,text+3,length-3);
         DrawLine(x+l2,y-fs1.Over(),x+l2,y+fs1.Under(),spec);
         DrawLine(x+l2,y-fs1.Over(),x+l2+l,y-fs1.Over(),spec);
         DrawLine(x+l2,y+fs1.Under(),x+l2+l,y+fs1.Under(),spec);
         DrawLine(x+l2+fs1.Width()+2*l,y-fs1.Over(),x+l2+fs1.Width()+2*l,y+fs1.Under(),spec);
         DrawLine(x+l2+fs1.Width()+2*l,y-fs1.Over(),x+l2+fs1.Width()+l,y-fs1.Over(),spec);
         DrawLine(x+l2+fs1.Width()+2*l,y+fs1.Under(),x+l2+fs1.Width()+l,y+fs1.Under(),spec);
      }
      result.Set(fs1.Width()+3*l,fs1.Over(),fs1.Under());
   }
   else if (opParen) {  // operator #(){arg}
      Double_t l = GetHeight()*spec.fSize/4;
      Double_t radius2,radius1 , dw, l2 = l/2 ;
      Double_t angle = 35 ;
      if (!fShow) {
         fs1 = Anal1(spec,text+3,length-3);
         Savefs(&fs1);
         radius2 = fs1.Height() ;
         radius1 = radius2  * 2 / 3;
         dw = radius1*(1 - TMath::Cos(kPI*angle/180)) ;
      } else {
         fs1 = Readfs();
         radius2 = fs1.Height();
         radius1 = radius2  * 2 / 3;
         dw = radius1*(1 - TMath::Cos(kPI*angle/180)) ;
         Double_t x1 = x+l2+radius1 ;
         Double_t x2 = x+5*l2+2*dw+fs1.Width()-radius1 ;
         Double_t y1 = y - (fs1.Over() - fs1.Under())/2. ;
         DrawParenthesis(x1,y1,radius1,radius2,180-angle,180+angle,spec) ;
         DrawParenthesis(x2,y1,radius1,radius2,360-angle,360+angle,spec) ;
         Analyse(x+3*l2+dw,y,spec,text+3,length-3);
      }
     // result = TLatexFormSize(fs1.Width()+3*l,fs1.Over(),fs1.Under());
      result.Set(fs1.Width()+3*l+2*dw,fs1.Over(),fs1.Under());
   }
   else if (opAbs) {  // operator #||{arg}
      Double_t l = GetHeight()*spec.fSize/4;
      Double_t l2 = l/2 ;
      if (!fShow) {
         fs1 = Anal1(spec,text+3,length-3);
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Analyse(x+l2+l,y,spec,text+3,length-3);
         DrawLine(x+l2,y-fs1.Over(),x+l2,y+fs1.Under(),spec);
         DrawLine(x+l2+fs1.Width()+2*l,y-fs1.Over(),x+l2+fs1.Width()+2*l,y+fs1.Under(),spec);
      }
      result.Set(fs1.Width()+3*l,fs1.Over(),fs1.Under());
   }
   else if (opBigCurly) { // big curly bracket  #{}{arg}
      Double_t l = GetHeight()*spec.fSize/4;
      Double_t l2 = l/2 ;
      Double_t l8 , ltip;

      if (!fShow) {
         fs1 = Anal1(spec,text+3,length-3);
         l8 = fs1.Height()/8 ;
         ltip = TMath::Min(l8,l) ;
         l = ltip ;
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Double_t y2 = y + (fs1.Under()-fs1.Over())/2 ;
         l8 = fs1.Height()/8 ;
         ltip = TMath::Min(l8,l) ;
         l = ltip ;
         Analyse(x+l+ltip+l2,y,spec,text+3,length-3);
         // Draw open curly bracket
         // Vertical lines
         DrawLine(x+l2+ltip,y-fs1.Over(),x+l2+ltip,y2-ltip,spec);
         DrawLine(x+l2+ltip,y2+ltip,x+l2+ltip,y+fs1.Under(),spec);
         // top and bottom lines
         DrawLine(x+l2+ltip,y-fs1.Over(),x+l2+ltip+l,y-fs1.Over(),spec);
         DrawLine(x+l2+ltip,y+fs1.Under(),x+l2+ltip+l,y+fs1.Under(),spec);
         // < sign
         DrawLine(x+l2,y2,x+l2+ltip,y2-ltip,spec);
         DrawLine(x+l2,y2,x+l2+ltip,y2+ltip,spec);

         // Draw close curly bracket
         // vertical lines
         DrawLine(x+l2+ltip+fs1.Width()+2*l,y-fs1.Over(),x+l2+ltip+fs1.Width()+2*l,y2-ltip,spec);
         DrawLine(x+l2+ltip+fs1.Width()+2*l,y2+ltip,x+l2+ltip+fs1.Width()+2*l,y+fs1.Under(),spec);
         // Top and bottom lines
         DrawLine(x+l2+fs1.Width()+l+ltip,y-fs1.Over(),x+l2+ltip+fs1.Width()+2*l,y-fs1.Over(),spec);
         DrawLine(x+l2+fs1.Width()+l+ltip,y+fs1.Under(),x+l2+ltip+fs1.Width()+2*l,y+fs1.Under(),spec);
         // > sign
         DrawLine(x+l2+ltip+2*l+fs1.Width(),y2-ltip,x+l2+2*l+2*ltip+fs1.Width(),y2,spec);
         DrawLine(x+l2+ltip+2*l+fs1.Width(),y2+ltip,x+l2+2*l+2*ltip+fs1.Width(),y2,spec);
      }
      result.Set(fs1.Width()+3*l+2*ltip,fs1.Over(),fs1.Under()) ;;
   }
   else if (opFrac>-1) { // \frac found
      if (opCurlyCurly==-1) { // }{ not found
         // arguments missing for \frac
         fError = "Missing denominator for #frac";
         return TLatexFormSize(0,0,0);
      }
      Double_t height = GetHeight()*spec.fSize/8;
      if (!fShow) {
         fs1 = Anal1(spec,text+opFrac+6,opCurlyCurly-opFrac-6);
         fs2 = Anal1(spec,text+opCurlyCurly+2,length-opCurlyCurly-3);
         Savefs(&fs1);
         Savefs(&fs2);
      } else {
         fs2 = Readfs();
         fs1 = Readfs();
         Double_t addW1,addW2;
         if (fs1.Width()<fs2.Width()) {
            addW1 = (fs2.Width()-fs1.Width())/2;
            addW2 = 0;
         } else {
            addW1 = 0;
            addW2 = (fs1.Width()-fs2.Width())/2;
         }
         Analyse(x+addW2,y+fs2.Over()-height,spec,text+opCurlyCurly+2,length-opCurlyCurly-3);  // denominator
         Analyse(x+addW1,y-fs1.Under()-3*height,spec,text+opFrac+6,opCurlyCurly-opFrac-6); //numerator

         DrawLine(x,y-2*height,x+TMath::Max(fs1.Width(),fs2.Width()),y-2*height,spec);
      }

      result.Set(TMath::Max(fs1.Width(),fs2.Width()),fs1.Height()+3*height,fs2.Height()-height);

   }
   else if (opSplitLine>-1) { // \splitline found
      if (opCurlyCurly==-1) { // }{ not found
         // arguments missing for \splitline
         fError = "Missing second line for #splitline";
         return TLatexFormSize(0,0,0);
      }
      Double_t height = GetHeight()*spec.fSize/8;
      if (!fShow) {
         fs1 = Anal1(spec,text+opSplitLine+11,opCurlyCurly-opSplitLine-11);
         fs2 = Anal1(spec,text+opCurlyCurly+2,length-opCurlyCurly-3);
         Savefs(&fs1);
         Savefs(&fs2);
      } else {
         fs2 = Readfs();
         fs1 = Readfs();
         Analyse(x,y+fs2.Over()-height,spec,text+opCurlyCurly+2,length-opCurlyCurly-3);  // second line
         Analyse(x,y-fs1.Under()-3*height,spec,text+opSplitLine+11,opCurlyCurly-opSplitLine-11); //first line
      }

      result.Set(TMath::Max(fs1.Width(),fs2.Width()),fs1.Height()+3*height,fs2.Height()-height);

   }
   else if (opSqrt>-1) { // \sqrt found
      if (!fShow) {
         if (opSquareCurly>-1) {
            // power nth  #sqrt[n]{arg}
            fs1 = Anal1(specNewSize,text+opSqrt+6,opSquareCurly-opSqrt-6);
            fs2 = Anal1(spec,text+opSquareCurly+1,length-opSquareCurly-1);
            Savefs(&fs1);
            Savefs(&fs2);
            result.Set(fs2.Width()+ GetHeight()*spec.fSize/10+TMath::Max(GetHeight()*spec.fSize/2,(Double_t)fs1.Width()),
                       fs2.Over()+fs1.Height()+GetHeight()*spec.fSize/4,fs2.Under());
         } else {
            fs1 = Anal1(spec,text+opSqrt+5,length-opSqrt-5);
            Savefs(&fs1);
            result.Set(fs1.Width()+GetHeight()*spec.fSize/2,fs1.Over()+GetHeight()*spec.fSize/4,fs1.Under());
         }
      } else {
         if (opSquareCurly>-1) { // ]{
            fs2 = Readfs();
            fs1 = Readfs();
            Double_t pas = TMath::Max(GetHeight()*spec.fSize/2,(Double_t)fs1.Width());
            Double_t pas2 = pas + GetHeight()*spec.fSize/10;
            Double_t y1 = y-fs2.Over() ;
            Double_t y2 = y+fs2.Under() ;
            Double_t y3 = y1-GetHeight()*spec.fSize/4;
            Analyse(x+pas2,y,spec,text+opSquareCurly+1,length-opSquareCurly-1);
            Analyse(x,y-fs2.Over()-fs1.Under(),specNewSize,text+opSqrt+6,opSquareCurly-opSqrt-6); // indice
            DrawLine(x,y1,x+pas,y2,spec);
            DrawLine(x+pas,y2,x+pas,y3,spec);
            DrawLine(x+pas,y3,x+pas2+fs2.Width(),y3,spec);
         } else {
            fs1 = Readfs();
            Double_t x1 = x+GetHeight()*spec.fSize*2/5 ;
            Double_t x2 = x+GetHeight()*spec.fSize/2+fs1.Width() ;
            Double_t y1 = y-fs1.Over() ;
            Double_t y2 = y+fs1.Under() ;
            Double_t y3 = y1-GetHeight()*spec.fSize/4;

            Analyse(x+GetHeight()*spec.fSize/2,y,spec,text+opSqrt+6,length-opSqrt-7);

            Short_t lineW = GetLineWidth();
            Double_t dx = (y2-y3)/8;
            SetLineWidth(TMath::Max(2,(Int_t)(dx/2)));
            DrawLine(x-dx,y1,x1-dx,y2,spec);
            SetLineWidth((Int_t)(dx/4));
            DrawLine(x1-dx,y2,x1,y3,spec);
            DrawLine(x1,y3,x2,y3,spec);
            SetLineWidth(lineW);
         }
      }
   }
   else if (opColor>-1) { // \color found
      if (opSquareCurly==-1) {
         // color number is not specified
         fError = "Missing color number. Syntax is #color[(Int_t)nb]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      TextSpec_t newSpec = spec;
      Char_t *nb = new Char_t[opSquareCurly-opColor-6];
      strncpy(nb,text+opColor+7,opSquareCurly-opColor-7);
      nb[opSquareCurly-opColor-7] = 0;
      if (sscanf(nb,"%d",&newSpec.fColor) < 1) {
         delete[] nb;
         // color number is invalid
         fError = "Invalid color number. Syntax is #color[(Int_t)nb]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      delete[] nb;
      if (!fShow) {
         result = Anal1(newSpec,text+opSquareCurly+1,length-opSquareCurly-1);
      } else {
         Analyse(x,y,newSpec,text+opSquareCurly+1,length-opSquareCurly-1);
      }
   }
   else if (opFont>-1) { // \font found
      if (opSquareCurly==-1) {
         // font number is not specified
         fError = "Missing font number. Syntax is #font[nb]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      TextSpec_t newSpec = spec;
      Char_t *nb = new Char_t[opSquareCurly-opFont-5];
      strncpy(nb,text+opFont+6,opSquareCurly-opFont-6);
      nb[opSquareCurly-opFont-6] = 0;
      if (sscanf(nb,"%d",&newSpec.fFont) < 1) {
         delete[] nb;
         // font number is invalid
         fError = "Invalid font number. Syntax is #font[(Int_t)nb]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      delete[] nb;
      if (!fShow) {
         result = Anal1(newSpec,text+opSquareCurly+1,length-opSquareCurly-1);
      } else {
         Analyse(x,y,newSpec,text+opSquareCurly+1,length-opSquareCurly-1);
      }
   }
   else if (opKern>-1) { // #kern found
      if (opSquareCurly==-1) {
         // horizontal shift is not specified
         fError = "Missing horizontal shift number. Syntax is #kern[dx]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      Char_t *dxc = new Char_t[opSquareCurly-opKern-5];
      strncpy(dxc,text+opKern+6,opSquareCurly-opKern-6);
      dxc[opSquareCurly-opKern-6] = 0;
      Float_t dx = 0;
      if (sscanf(dxc,"%f",&dx) < 1) {
         delete[] dxc;
         // horizontal shift number is invalid
         fError = "Invalid horizontal shift number. Syntax is #kern[(Float_t)dx]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      delete[] dxc;
      if (!fShow) {
         fs1 = Anal1(spec,text+opSquareCurly+1,length-opSquareCurly-1);
         Savefs(&fs1);
         Double_t ddx = dx * fs1.Width();
         result = TLatexFormSize(fs1.Width() + ddx, fs1.Over(), fs1.Under());
      } else {
         fs1 = Readfs();
         Double_t ddx = dx * fs1.Width();
         Analyse(x + ddx,y,spec,text+opSquareCurly+1,length-opSquareCurly-1);
      }
   }
   else if (opLower>-1) { // #lower found
      if (opSquareCurly==-1) {
         // vertical shift is not specified
         fError = "Missing vertical shift number. Syntax is #lower[dy]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      Char_t *dyc = new Char_t[opSquareCurly-opLower-6];
      strncpy(dyc,text+opLower+7,opSquareCurly-opLower-7);
      dyc[opSquareCurly-opLower-7] = 0;
      Float_t dy = 0;
      if (sscanf(dyc,"%f",&dy) < 1) {
         delete[] dyc;
         // vertical shift number is invalid
         fError = "Invalid vertical shift number. Syntax is #lower[(Float_t)dy]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      delete[] dyc;
      if (!fShow) {
         fs1 = Anal1(spec,text+opSquareCurly+1,length-opSquareCurly-1);
         Savefs(&fs1);
         Double_t ddy = dy * (fs1.Over() + fs1.Under());
         result = TLatexFormSize(fs1.Width(), fs1.Over() + ddy, fs1.Under() + ddy);
      } else {
         fs1 = Readfs();
         Double_t ddy = dy * (fs1.Over() + fs1.Under());
         Analyse(x,y + ddy,spec,text+opSquareCurly+1,length-opSquareCurly-1);
      }
   }
   else if (opScale>-1) { // \scale found
      if (opSquareCurly==-1) {
         // scale factor is not specified
         fError = "Missing scale factor. Syntax is #scale[(Double_t)nb]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      TextSpec_t newSpec = spec;
      Char_t *nb = new Char_t[opSquareCurly-opScale-6];
      strncpy(nb,text+opScale+7,opSquareCurly-opScale-7);
      nb[opSquareCurly-opScale-7] = 0;
      if (sscanf(nb,"%lf",&newSpec.fSize) < 1) {
         delete[] nb;
         // scale factor is invalid
         fError = "Invalid scale factor. Syntax is #factor[(Double_t)nb]{ ... }";
         return TLatexFormSize(0,0,0);
      }
      newSpec.fSize *= spec.fSize;
      delete[] nb;
      if (!fShow) {
         result = Anal1(newSpec,text+opSquareCurly+1,length-opSquareCurly-1);
      } else {
         Analyse(x,y,newSpec,text+opSquareCurly+1,length-opSquareCurly-1);
      }
   }
   else if (opBf>-1) { // operator #bf{arg}
      TextSpec_t newSpec = spec;
      Int_t lut[] = {3, 13, 1, 6, 7, 4, 5, 10, 11, 8, 9, 12, 2, 14, 15};
      Int_t fontId = (newSpec.fFont/10);
      if ((fontId >= 1) && (fontId <= (Int_t)(sizeof(lut)/sizeof(lut[0])))) fontId = lut[fontId-1];
      newSpec.fFont = fontId*10 + newSpec.fFont%10;
      if (!fShow) {
         fs1 = Anal1(newSpec,text+3,length-3);
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Analyse(x,y,newSpec,text+3,length-3);
      }
      result = fs1;
   }
   else if (opIt>-1) { // operator #it{arg}
      TextSpec_t newSpec = spec;
      Int_t lut[] = {13, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 15, 1, 14, 12};
      Int_t fontId = (newSpec.fFont/10);
      if ((fontId >= 1) && (fontId <= (Int_t)(sizeof(lut)/sizeof(lut[0])))) fontId = lut[fontId-1];
      newSpec.fFont = fontId*10 + newSpec.fFont%10;
      fItalic = !fItalic;
      if (!fShow) {
         fs1 = Anal1(newSpec,text+3,length-3);
         Savefs(&fs1);
      } else {
         fs1 = Readfs();
         Analyse(x,y,newSpec,text+3,length-3);
      }
      fItalic = !fItalic;
      result = fs1;
   }
   else { // no operators found, it is a character string
      SetTextSize(spec.fSize);
      SetTextAngle(spec.fAngle);
      SetTextColor(spec.fColor);
      SetTextFont(spec.fFont);
      SetTextAlign(11);
      TAttText::Modify();
      UInt_t w=0,h=0;

      Int_t leng = strlen(text) ;

      quote1 = quote2 = kFALSE ;
      Char_t *p ;
      for (i=0 ; i<leng ; i++) {
         switch (text[i]) {
            case '\'' : quote1 = !quote1 ; break ; // single quote symbol not correctly interpreted when PostScript
            case '"'  : quote2 = !quote2 ;  break ;
         }
         //if (quote1 || quote2) continue ;
         if (text[i] == '@') {  // @ symbol not correctly interpreted when PostScript
            p = &text[i] ;
            if ( *(p+1) == '{' || *(p+1) == '}' || *(p+1) == '[' || *(p+1) == ']') {
               while (*p != 0) {
                  *p = *(p+1) ; p++ ;
               }
               leng-- ;
            }
         }
      }
      text[leng] = 0 ;

      if (fShow) {
         // paint the Latex sub-expression per sub-expression
         Double_t xOrigin = (Double_t)gPad->XtoAbsPixel(fX);
         Double_t yOrigin = (Double_t)gPad->YtoAbsPixel(fY);
         Double_t angle   = kPI*spec.fAngle/180.;
         Double_t xx = gPad->AbsPixeltoX(Int_t((x-xOrigin)*TMath::Cos(angle)+(y-yOrigin)*TMath::Sin(angle)+xOrigin));
         Double_t yy = gPad->AbsPixeltoY(Int_t((x-xOrigin)*TMath::Sin(-angle)+(y-yOrigin)*TMath::Cos(angle)+yOrigin));
         gPad->PaintText(xx,yy,text);
      } else {
         GetTextExtent(w,h,text);
         Double_t width = w;
         UInt_t a,d;
         GetTextAscentDescent(a, d, text);
         fs1.Set(width,a,d);
      }

      result = fs1;
   }

   delete[] text;

   return result;
}


//______________________________________________________________________________
TLatex *TLatex::DrawLatex(Double_t x, Double_t y, const char *text)
{
   // Make a copy of this object with the new parameters
   // And copy object attributes

   TLatex *newtext = new TLatex(x, y, text);
   TAttText::Copy(*newtext);
   newtext->SetBit(kCanDelete);
   if (TestBit(kTextNDC)) newtext->SetNDC();
   newtext->AppendPad();
   return newtext;
}


//______________________________________________________________________________
void TLatex::DrawLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2, TextSpec_t spec)
{
   // Draw a line in a Latex formula

   Double_t sinang  = TMath::Sin(spec.fAngle/180*kPI);
   Double_t cosang  = TMath::Cos(spec.fAngle/180*kPI);
   Double_t xOrigin = (Double_t)gPad->XtoAbsPixel(fX);
   Double_t yOrigin = (Double_t)gPad->YtoAbsPixel(fY);
   Double_t xx  = gPad->AbsPixeltoX(Int_t((x1-xOrigin)*cosang+(y1-yOrigin)*sinang+xOrigin));
   Double_t yy  = gPad->AbsPixeltoY(Int_t((x1-xOrigin)*-sinang+(y1-yOrigin)*cosang+yOrigin));

   Double_t xx2 = gPad->AbsPixeltoX(Int_t((x2-xOrigin)*cosang+(y2-yOrigin)*sinang+xOrigin));
   Double_t yy2 = gPad->AbsPixeltoY(Int_t((x2-xOrigin)*-sinang+(y2-yOrigin)*cosang+yOrigin));

   SetLineColor(spec.fColor);
   TAttLine::Modify();
   gPad->PaintLine(xx,yy,xx2,yy2);
}


//______________________________________________________________________________
void TLatex::DrawCircle(Double_t x1, Double_t y1, Double_t r, TextSpec_t spec )
{
   // Draw an arc of ellipse in a Latex formula (right or left parenthesis)

   if (r < 1) r = 1;
   Double_t sinang  = TMath::Sin(spec.fAngle/180*kPI);
   Double_t cosang  = TMath::Cos(spec.fAngle/180*kPI);
   Double_t xOrigin = (Double_t)gPad->XtoAbsPixel(fX);
   Double_t yOrigin = (Double_t)gPad->YtoAbsPixel(fY);

   const Int_t np = 40;
   Double_t dphi = 2*kPI/np;
   Double_t x[np+3], y[np+3];
   Double_t angle,dx,dy;

   SetLineColor(spec.fColor);
   TAttLine::Modify();  //Change line attributes only if necessary

   for (Int_t i=0;i<=np;i++) {
      angle = Double_t(i)*dphi;
      dx    = r*TMath::Cos(angle) +x1 -xOrigin;
      dy    = r*TMath::Sin(angle) +y1 -yOrigin;
      x[i]  = gPad->AbsPixeltoX(Int_t( dx*cosang+ dy*sinang +xOrigin));
      y[i]  = gPad->AbsPixeltoY(Int_t(-dx*sinang+ dy*cosang +yOrigin));
   }
   gPad->PaintPolyLine(np+1,x,y);
}


//______________________________________________________________________________
void TLatex::DrawParenthesis(Double_t x1, Double_t y1, Double_t r1, Double_t r2,
                     Double_t  phimin, Double_t  phimax, TextSpec_t spec )
{
   // Draw an arc of ellipse in a Latex formula (right or left parenthesis)

   if (r1 < 1) r1 = 1;
   if (r2 < 1) r2 = 1;
   Double_t sinang  = TMath::Sin(spec.fAngle/180*kPI);
   Double_t cosang  = TMath::Cos(spec.fAngle/180*kPI);
   Double_t xOrigin = (Double_t)gPad->XtoAbsPixel(fX);
   Double_t yOrigin = (Double_t)gPad->YtoAbsPixel(fY);

   const Int_t np = 40;
   Double_t dphi = (phimax-phimin)*kPI/(180*np);
   Double_t x[np+3], y[np+3];
   Double_t angle,dx,dy ;

   SetLineColor(spec.fColor);
   TAttLine::Modify();  //Change line attributes only if necessary

   for (Int_t i=0;i<=np;i++) {
      angle = phimin*kPI/180 + Double_t(i)*dphi;
      dx    = r1*TMath::Cos(angle) +x1 -xOrigin;
      dy    = r2*TMath::Sin(angle) +y1 -yOrigin;
      x[i]  = gPad->AbsPixeltoX(Int_t( dx*cosang+dy*sinang +xOrigin));
      y[i]  = gPad->AbsPixeltoY(Int_t(-dx*sinang+dy*cosang +yOrigin));
   }
   gPad->PaintPolyLine(np+1,x,y);
}


//______________________________________________________________________________
void TLatex::Paint(Option_t *)
{
   // Paint.

   Double_t xsave = fX;
   Double_t ysave = fY;
   if (TestBit(kTextNDC)) {
      fX = gPad->GetX1() + xsave*(gPad->GetX2() - gPad->GetX1());
      fY = gPad->GetY1() + ysave*(gPad->GetY2() - gPad->GetY1());
      PaintLatex(fX,fY,GetTextAngle(),GetTextSize(),GetTitle());
   } else {
      PaintLatex(gPad->XtoPad(fX),gPad->YtoPad(fY),GetTextAngle(),GetTextSize(),GetTitle());
   }
   fX = xsave;
   fY = ysave;
}


//______________________________________________________________________________
void TLatex::PaintLatex(Double_t x, Double_t y, Double_t angle, Double_t size, const Char_t *text1)
{
   // Main drawing function
   //
   // Warning: Unlike most others "XYZ::PaintXYZ" methods, PaintLatex modifies
   //          the TLatex data members.

   TAttText::Modify();  // Change text attributes only if necessary.

   // Do not use Latex if font is low precision.
   if (fTextFont%10 < 2) {
      if (gVirtualX) gVirtualX->SetTextAngle(angle);
      if (gVirtualPS) gVirtualPS->SetTextAngle(angle);
      gPad->PaintText(x,y,text1);
      return;
   }

   TString newText = text1;
   if( newText.Length() == 0) return;

   Double_t saveSize = size;
   Int_t saveFont = fTextFont;
   if (fTextFont%10 > 2) {
      UInt_t w = TMath::Abs(gPad->XtoAbsPixel(gPad->GetX2()) -
                            gPad->XtoAbsPixel(gPad->GetX1()));
      UInt_t h = TMath::Abs(gPad->YtoAbsPixel(gPad->GetY2()) -
                            gPad->YtoAbsPixel(gPad->GetY1()));
      if (w < h)
         size = size/w;
      else
         size = size/h;
      SetTextFont(10*(saveFont/10) + 2);
   }
   if (gVirtualPS) gVirtualPS->SetBit(kLatex);

   fError = 0 ;
   if (CheckLatexSyntax(newText)) {
      cout<<"\n*ERROR<TLatex>: "<<fError<<endl;
      cout<<"==> "<<text1<<endl;
      return ;
   }
   fError = 0 ;

   Int_t length = newText.Length() ;
   const Char_t *text = newText.Data() ;

   fX=x;
   fY=y;
   x = gPad->XtoAbsPixel(x);
   y = gPad->YtoAbsPixel(y);
   fShow = kFALSE ;
   TLatexFormSize fs = FirstParse(angle,size,text);

   fOriginSize = size;

   // Get current line attributes.
   Short_t lineW = GetLineWidth();
   Int_t lineC = GetLineColor();

   TextSpec_t spec;
   spec.fAngle = angle;
   spec.fSize  = size;
   spec.fColor = GetTextColor();
   spec.fFont  = GetTextFont();
   Short_t halign = fTextAlign/10;
   Short_t valign = fTextAlign - 10*halign;
   TextSpec_t newSpec = spec;
   if (fError != 0) {
      cout<<"*ERROR<TLatex>: "<<fError<<endl;
      cout<<"==> "<<text<<endl;
   } else {
      fShow = kTRUE;
      newSpec.fSize = size;

      switch (valign) {
         case 0: y -= fs.Under() ; break;
         case 1: break;
         case 2: y += fs.Height()*0.5-fs.Under(); y++; break;
         case 3: y += fs.Over() ; break;
      }
      switch (halign) {
         case 2: x -= fs.Width()/2  ; break;
         case 3: x -= fs.Width()    ; break;
      }
      Analyse(x,y,newSpec,text,length);
   }

   SetTextSize(saveSize);
   SetTextAngle(angle);
   SetTextFont(saveFont);
   SetTextColor(spec.fColor);
   SetTextAlign(valign+10*halign);
   SetLineWidth(lineW);
   SetLineColor(lineC);
   delete[] fTabSize;

   if (gVirtualPS) gVirtualPS->ResetBit(kLatex);
}


//______________________________________________________________________________
Int_t TLatex::CheckLatexSyntax(TString &text)
{
   // Check if the Latex syntax is correct

   const Char_t *kWord1[] = {"{}^{","{}_{","^{","_{","#scale{","#color{","#font{","#sqrt{","#[]{","#{}{","#||{",
                       "#bar{","#vec{","#dot{","#hat{","#ddot{","#acute{","#grave{","#check{","#tilde{","#slash{","#bf{","#it{",
                       "\\scale{","\\color{","\\font{","\\sqrt{","\\[]{","\\{}{","\\||{","#(){","\\(){",
                       "\\bar{","\\vec{","\\dot{","\\hat{","\\ddot{","\\acute{","\\grave{","\\check{","\\bf{","\\it{"}; // check for }
   const Char_t *kWord2[] = {"#scale[","#color[","#font[","#sqrt[","#kern[","#lower[","\\scale[","\\color[","\\font[","\\sqrt[","\\kern[","\\lower["}; // check for ]{ + }
   const Char_t *kWord3[] = {"#frac{","\\frac{","#splitline{","\\splitline{"}; // check for }{ then }
   const Char_t *kLeft1[] = {"#left[","\\left[","#left{","\\left{","#left|","\\left|","#left(","\\left("};
   const Char_t *kLeft2[] = {"#[]{","#[]{","#{}{","#{}{","#||{","#||{","#(){","#(){"};
   const Char_t *kRight[] = {"#right]","\\right]","#right}","\\right}","#right|","\\right|","#right)","\\right)"};
   const Int_t lkWord1[]  = {4,4,2,2,7,7,6,6,4,4,4,5,5,5,5,6,7,7,7,7,7,4,4,7,7,6,6,4,4,4,4,4,5,5,5,5,6,7,7,7,4,4};
   const Int_t lkWord2[]  = {7,7,6,6,6,7,7,7,6,6,6,7} ;
   const Int_t lkWord3[]  = {6,6,11,11} ;
   Int_t nkWord1 = 42, nkWord2 = 12, nkWord3 = 4;
   Int_t i,k ;
   Int_t nLeft1 , nRight , nOfLeft, nOfRight;
   Int_t lLeft1 = 6 ;
   Int_t lLeft2 = 4 ;
   Int_t lRight = 7 ;
   nLeft1  = nRight   = 8 ;
   nOfLeft = nOfRight = 0 ;

   Char_t buf[11] ; for (i=0;i<11;i++) buf[i]=0;
   Bool_t opFound ;
   Int_t  opFrac = 0;
   Int_t length = text.Length() ;

   Int_t nOfCurlyBracket, nOfKW1, nOfKW2, nOfKW3, nOfSquareCurly, nOfCurlyCurly ;
   Int_t nOfExtraCurly = 0 , nOfExtraSquare = 0;
   Int_t nOfSquareBracket = 0 ;
   Int_t error = 0  ;
   Bool_t quote1 = kFALSE , quote2 = kFALSE;

   // first find and replace all occurences of "kLeft1" keyword by "kLeft2" keyword,
   // and all occurences of "kRight" keyword by "}".
   i = 0 ;
   while (i < length) {
      // The string in 'buf' does not need to be null terminated,
      // we will only check with strncmp.
      strncpy(buf,&text[i],TMath::Min(7,length-i));
      opFound = kFALSE ;
      for (k = 0 ; k < nLeft1 ; k++) {
         if (strncmp(buf,kLeft1[k],lLeft1)==0) {
            nOfLeft++ ;
            i+=lLeft1 ;
            opFound = kTRUE ;
            break ;
         }
      }
      if (opFound) continue ;

      for(k=0;k<nRight;k++) {
         if (strncmp(buf,kRight[k],lRight)==0) {
            nOfRight++ ;
            i+=lRight ;
            opFound = kTRUE ;
            break ;
         }
      }
      if (!opFound) i++ ;
   }
   if (nOfLeft != nOfRight) {
      printf(" nOfLeft = %d, nOfRight = %d\n",nOfLeft,nOfRight) ;
      error = 1 ;
      fError = "Operators \"#left\" and \"#right\" don't match !" ;
      goto ERROR_END ;
   }

   for (k = 0 ; k < nLeft1 ; k++) {
      text.ReplaceAll(kLeft1[k],lLeft1,kLeft2[k],lLeft2) ;
   }
   for (k = 0 ; k < nRight ; k++) {
      text.ReplaceAll(kRight[k],lRight,"}",1) ;
   }
   length = text.Length() ;

   i = nOfCurlyBracket = nOfKW1 = nOfKW2 = nOfKW3 = nOfSquareCurly = nOfCurlyCurly =0 ;
   while (i< length){
         switch (text[i]) {
            case '"' : quote1 = !quote1 ; break ;
            case '\'': quote2 = !quote2 ; break ;
         }
         // The string in 'buf' does not need to be null terminated,
         // we will only check with strncmp
         strncpy(buf,&text[i],TMath::Min(11,length-i));
         opFound = kFALSE ;

         for(k=0;k<nkWord1;k++) {
            if (strncmp(buf,kWord1[k],lkWord1[k])==0) {
               nOfKW1++ ;
               i+=lkWord1[k] ;
               opFound = kTRUE ;
               nOfCurlyBracket++ ;
               break ;
            }
         }
         if (opFound) continue ;

         for(k=0;k<nkWord2;k++) {
            if (strncmp(buf,kWord2[k],lkWord2[k])==0) {
               nOfKW2++ ;
               i+=lkWord2[k] ;
               opFound = kTRUE ;
               nOfSquareBracket++;
               break ;
            }
         }
         if (opFound) continue ;

         for(k=0;k<nkWord3;k++) {
            if (strncmp(buf,kWord3[k],lkWord3[k])==0) {
               nOfKW3++ ;
               i+=lkWord3[k] ;
               opFound = kTRUE ;
               opFrac++ ;
               nOfCurlyBracket++ ;
               break ;
            }
         }
         if (opFound) continue ;
         if (strncmp(buf,"}{",2) == 0 && opFrac) {
               opFrac-- ;
               nOfCurlyCurly++ ;
               i+= 2;
         }
         else if (strncmp(buf,"]{",2) == 0 && nOfSquareBracket) {
               nOfSquareCurly++ ;
               i+= 2 ;
               nOfCurlyBracket++ ;
               nOfSquareBracket-- ;
         }
         else if (strncmp(buf,"@{",2) == 0 || strncmp(buf,"@}",2) == 0) {
               i+= 2 ;
         }
         else if (strncmp(buf,"@[",2) == 0 || strncmp(buf,"@]",2) == 0) {
               i+= 2 ;
         }
         else if (text[i] == ']' ) {  // not belonging to a key word, add @ in front
               text.Insert(i,"@") ;
               length++ ;
               i+=2 ;
               nOfExtraSquare-- ;
         }
         else if (text[i] == '[' ) {  // not belonging to a key word, add @ in front
               text.Insert(i,"@") ;
               length++ ;
               i+=2 ;
               nOfExtraSquare++ ;
         }
         else if (text[i] == '{' ) {  // not belonging to a key word, add @ in front
               text.Insert(i,"@") ;
               length++ ;
               i+=2 ;
               nOfExtraCurly++ ;
         }
         else if (text[i] == '}' ) {
            if ( nOfCurlyBracket) {
               nOfCurlyBracket-- ;
               i++ ;
            } else  { // extra }, add @ in front
               text.Insert(i,"@") ;
               length++ ;
               i+=2 ;
               nOfExtraCurly-- ;
            }
         } else {
            i++ ;
            buf[1] = 0 ;
         }
   }

   if (nOfKW2 != nOfSquareCurly) {
      error = 1 ;
      fError = "Invalid number of \"]{\"" ;
   }
   else if (nOfKW3 != nOfCurlyCurly) {
      error = 1 ;
      fError = "Error in syntax of  \"#frac\"" ;
   }
   else if (nOfCurlyBracket  < 0) {
      error = 1 ;
      fError = "Missing \"{\"" ;
   }
   else if (nOfCurlyBracket  > 0) {
      error = 1 ;
      fError = "Missing \"}\"" ;
   }
   else if (nOfSquareBracket  < 0) {
      error  = 1 ;
      fError = "Missing \"[\"" ;
   }
   else if (nOfSquareBracket  > 0) {
      error = 1 ;
      fError = "Missing \"]\"" ;
   }

   ERROR_END:
   return error ;
}


//______________________________________________________________________________
TLatexFormSize TLatex::FirstParse(Double_t angle, Double_t size, const Char_t *text)
{
   // First parsing of the analyse sequence

   fError   = 0;
   fTabMax  = 100;
   fTabSize = new FormSize_t[fTabMax];
   // we assume less than 100 parts in one formula
   // we will reallocate if necessary.
   fPos        = 0;
   fShow       = kFALSE;
   fOriginSize = size;

   //get current line attributes
   Short_t lineW = GetLineWidth();
   Int_t lineC = GetLineColor();

   TextSpec_t spec;
   spec.fAngle = angle;
   if (fTextFont%10 == 3) {
      Double_t hw = TMath::Max((Double_t)gPad->XtoPixel(gPad->GetX2()),
                               (Double_t)gPad->YtoPixel(gPad->GetY1()));
      spec.fSize = size/hw;
   } else {
      spec.fSize  = size;
   }
   spec.fColor = GetTextColor();
   spec.fFont  = GetTextFont();
   Short_t halign = fTextAlign/10;
   Short_t valign = fTextAlign - 10*halign;

   TLatexFormSize fs = Anal1(spec,text,strlen(text));

   SetTextSize(size);
   SetTextAngle(angle);
   SetTextFont(spec.fFont);
   SetTextColor(spec.fColor);
   SetTextAlign(valign+10*halign);
   SetLineWidth(lineW);
   SetLineColor(lineC);
   return fs;
}


//______________________________________________________________________________
Double_t TLatex::GetHeight() const
{
   // Return height of current pad in pixels

   Double_t w = gPad->GetAbsWNDC()*Double_t(gPad->GetWw());
   Double_t h = gPad->GetAbsHNDC()*Double_t(gPad->GetWh());
   if (w < h)
      return w;
   else
      return h;
}


//______________________________________________________________________________
Double_t TLatex::GetXsize()
{
   // Return size of the formula along X in pad coordinates

   if (!gPad) return 0;
   TString newText = GetTitle();
   if( newText.Length() == 0) return 0;
   fError = 0 ;
   if (CheckLatexSyntax(newText)) {
      cout<<"\n*ERROR<TLatex>: "<<fError<<endl;
      cout<<"==> "<<GetTitle()<<endl;
      return 0;
   }
   fError = 0 ;

   const Char_t *text = newText.Data() ;
   Double_t angle_old = GetTextAngle();
   TLatexFormSize fs = FirstParse(0,GetTextSize(),text);
   SetTextAngle(angle_old);
   delete[] fTabSize;
   return TMath::Abs(gPad->AbsPixeltoX(Int_t(fs.Width())) - gPad->AbsPixeltoX(0));
}


//______________________________________________________________________________
void TLatex::GetBoundingBox(UInt_t &w, UInt_t &h, Bool_t angle)
{
   // Return text size in pixels

   if (!gPad) return;
   TString newText = GetTitle();
   if( newText.Length() == 0) return;
   fError = 0 ;
   if (CheckLatexSyntax(newText)) {
      cout<<"\n*ERROR<TLatex>: "<<fError<<endl;
      cout<<"==> "<<GetTitle()<<endl;
      return;
   }
   fError = 0 ;

   if (angle) {
      Int_t cBoxX[4], cBoxY[4];
      Int_t ptx, pty;
      if (TestBit(kTextNDC)) {
         ptx = gPad->UtoPixel(fX);
         pty = gPad->VtoPixel(fY);
      } else {
         ptx = gPad->XtoAbsPixel(gPad->XtoPad(fX));
         pty = gPad->YtoAbsPixel(gPad->YtoPad(fY));
      }
      GetControlBox(ptx, pty, fTextAngle, cBoxX, cBoxY);
      Int_t x1 = cBoxX[0];
      Int_t x2 = cBoxX[0];
      Int_t y1 = cBoxY[0];
      Int_t y2 = cBoxY[0];
      for (Int_t i=1; i<4; i++) {
         if (cBoxX[i] < x1) x1 = cBoxX[i];
         if (cBoxX[i] > x2) x2 = cBoxX[i];
         if (cBoxY[i] < y1) y1 = cBoxY[i];
         if (cBoxY[i] > y2) y2 = cBoxY[i];
      }
      w = x2-x1;
      h = y2-y1;
   } else {
      const Char_t *text = newText.Data() ;
      TLatexFormSize fs = FirstParse(GetTextAngle(),GetTextSize(),text);
      delete[] fTabSize;
      w = (UInt_t)fs.Width();
      h = (UInt_t)fs.Height();
   }
}


//______________________________________________________________________________
Double_t TLatex::GetYsize()
{
   // Return size of the formula along Y in pad coordinates

   if (!gPad) return 0;
   TString newText = GetTitle();
   if( newText.Length() == 0) return 0;
   fError = 0 ;
   if (CheckLatexSyntax(newText)) {
      cout<<"\n*ERROR<TLatex>: "<<fError<<endl;
      cout<<"==> "<<GetTitle()<<endl;
      return 0;
   }
   fError = 0 ;

   const Char_t *text = newText.Data() ;
   TLatexFormSize fs = FirstParse(0,GetTextSize(),text);
   delete[] fTabSize;
   return TMath::Abs(gPad->AbsPixeltoY(Int_t(fs.Height())) - gPad->AbsPixeltoY(0));
}


//______________________________________________________________________________
TLatexFormSize TLatex::Readfs()
{
   // Read fs in fTabSize

   fPos--;
   TLatexFormSize result(fTabSize[fPos].fWidth,fTabSize[fPos].fOver,fTabSize[fPos].fUnder);
   return result;
}


//______________________________________________________________________________
void TLatex::Savefs(TLatexFormSize *fs)
{
   // Save fs values in array fTabSize

   fTabSize[fPos].fWidth  = fs->Width();
   fTabSize[fPos].fOver   = fs->Over();
   fTabSize[fPos].fUnder  = fs->Under();
   fPos++;
   if (fPos>=fTabMax) {
      // allocate more memory
      FormSize_t *temp = new FormSize_t[fTabMax+100];
      // copy array
      memcpy(temp,fTabSize,fTabMax*sizeof(FormSize_t));
      fTabMax += 100;
      // free previous array
      delete [] fTabSize;
      // swap pointers
      fTabSize = temp;
   }
}


//______________________________________________________________________________
void TLatex::SavePrimitive(ostream &out, Option_t * /*= ""*/)
{
   // Save primitive as a C++ statement(s) on output stream out

   char quote = '"';
   if (gROOT->ClassSaved(TLatex::Class())) {
      out<<"   ";
   } else {
      out<<"   TLatex *";
   }
   TString s = GetTitle();
   s.ReplaceAll("\"","\\\"");
   out<<"   tex = new TLatex("<<fX<<","<<fY<<","<<quote<<s.Data()<<quote<<");"<<endl;
   if (TestBit(kTextNDC)) out<<"tex->SetNDC();"<<endl;

   SaveTextAttributes(out,"tex",11,0,1,62,0.05);
   SaveLineAttributes(out,"tex",1,1,1);

   out<<"   tex->Draw();"<<endl;
}


//______________________________________________________________________________
void TLatex::SetIndiceSize(Double_t factorSize)
{
   // Set relative size of subscripts and superscripts

   fFactorSize = factorSize;
}


//______________________________________________________________________________
void TLatex::SetLimitIndiceSize(Int_t limitFactorSize)
{
   // Set limit for text resizing of subscipts and superscripts

   fLimitFactorSize = limitFactorSize;
}
