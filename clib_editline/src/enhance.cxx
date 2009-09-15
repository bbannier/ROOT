#include "TROOT.h"
#include "el.h"
#include "TRegexp.h"
#include "TClassTable.h"
#include "TInterpreter.h"
#include <stack>
#include <set>

using namespace std;

void setKeywordColors(const char* colorTab, const char* colorBracket, const char* colorBadBracket);
int selectColor(const char* str);
void highlightKeywords(EditLine* el);
int matchParentheses(EditLine* el);
void colorWord(EditLine* el, int first, int num, int color);
void colorBrackets(EditLine* el, int open, int close, int color);
char** rl_complete2ROOT(const char*, int, int);

struct KeywordInLine {
   KeywordInLine(const TString& w, Long64_t hash, Ssiz_t pos):
      fWord(w),
      fHash(hash),
      fPosBegin(pos) {}

   TString fWord;
   Long64_t fHash;
   Ssiz_t fPosBegin;
};

// int values for colour highlighting
int color_class = 4;           // NCurses COLOR_BLUE
int color_type = 4;            // NCurses COLOR_BLUE
int color_bracket = 2;         // NCurses COLOR_GREEN
int color_badbracket = 1;      // NCurses COLOR_RED

/**
 *   Sets the colours to use for highlighting keywords (types and classnames),
 *   matching bracket pairs, mismatched brackets and tab completion.
 *   Overrides the default colour settings:
 *   class and type: 4 (blue)
 *   bracket pair:   2 (green)
 *   bad bracket:   1 (red)
 */
void
setKeywordColors(const char* colorType, const char* colorBracket, const char* colorBadBracket) {
   int col = -1;

   col = selectColor(TString(colorType));

   if (col > -1) {
      color_class = col;
   }
   color_type = color_class;

   col = selectColor(TString(colorBracket));

   if (col > -1) {
      color_bracket = col;
   }

   col = selectColor(TString(colorBadBracket));

   if (col > -1) {
      color_badbracket = col;
   }
} // setKeywordColors


/*
 * Return an ncurses colour integer based on the colour specified in the string.
 */
int
selectColor(const char* s) {
   TString str(s);
   int ret = 0;
   if (str.Contains("bold", TString::kIgnoreCase)
       || str.Contains("light", TString::kIgnoreCase))
      ret = 0x20;
   if (str.Contains("under", TString::kIgnoreCase))
      ret += 0x40;
   if (str.Contains("black", TString::kIgnoreCase)) {
      return ret;      // nCurses COLOR_BLACK
   } else if (str.Contains("red", TString::kIgnoreCase)) {
      return ret + 1;      // nCurses COLOR_RED
   } else if (str.Contains("green", TString::kIgnoreCase)) {
      return ret + 2;      // nCurses COLOR_GREEN
   } else if (str.Contains("yellow", TString::kIgnoreCase)) {
      return ret + 3;      // nCurses COLOR_YELLOW
   } else if (str.Contains("blue", TString::kIgnoreCase)) {
      return ret + 4;      // nCurses COLOR_BLUE
   } else if (str.Contains("magenta", TString::kIgnoreCase)) {
      return ret + 5;      // nCurses COLOR_MAGENTA
   } else if (str.Contains("cyan", TString::kIgnoreCase)) {
      return ret + 6;      // nCurses COLOR_CYAN
   } else if (str.Contains("white", TString::kIgnoreCase)) {
      return ret + 7;      // nCurses COLOR_WHITE
   } else {
      return -1;
   }
} // selectColor


/*
 *      Use gRoot to establish keywords known to root.
 *
 */
void
highlightKeywords(EditLine* el) {
   typedef std::set<int> HashSet_t;
   static HashSet_t sHashedKnownTypes;

   TString sBuffer(el->el_line.buffer, el->el_line.lastchar - el->el_line.buffer);

   TString keyword;
   Ssiz_t posNextTok = 0;
   Ssiz_t posPrevTok = 0;

   // regular expression inverse of match expression to find end of match
   while (sBuffer.Tokenize(keyword, posNextTok, "[^a-zA-Z0-9_]")) {
      Ssiz_t toklen = posNextTok - posPrevTok;

      if (posNextTok == -1) {
         toklen = sBuffer.Length() - posPrevTok;
      }
      TString tok = sBuffer(posPrevTok, toklen);
      Ssiz_t pos = posPrevTok + tok.Index(keyword);
      int color = -1;

      if (gROOT->GetListOfTypes()->FindObject(keyword)) {
         color = color_type;
      } else if (gInterpreter->CheckClassInfo(keyword, kFALSE)) {
         color = color_class;
      }
      colorWord(el, pos, keyword.Length(), color);
      posPrevTok = posNextTok;
   }
} // highlightKeywords


/** if buffer has content, check each char to see if it is an opening bracket,
    if so, check for its closing one and return the indices to both
 * alt:
 * check each char for a match against each type of open and close bracket
 * if open found, push index onto a seperate stack for each type of bracket
 * if close found, pop previous value off relevant stack
 * and pass both pointers to highlight()
 */
int
matchParentheses(EditLine* el) {
   static const int amtBrackets = 3;
   int bracketPos = -1;
   int foundParenIdx = -1;
   char bTypes[amtBrackets][2];

   bTypes[0][0] = '(';
   bTypes[0][1] = ')';
   bTypes[1][0] = '{';
   bTypes[1][1] = '}';
   bTypes[2][0] = '[';
   bTypes[2][1] = ']';
   //static char bTypes[] = "(){}[]"; with strchr(bTypes, sBuffer[bracketPos])

   // CURRENT STUFF
   // create a string of the buffer contents
   TString sBuffer = "";

   for (char* c = el->el_line.buffer; c < el->el_line.lastchar; c++) {
      sBuffer.Append(*c);
   }

   // check whole buffer for any highlighted brackets and remove colour info
   for (int i = 0; i < (el->el_line.lastchar - el->el_line.buffer); i++) {
      if (el->el_line.bufcolor[i].foreColor == color_bracket || el->el_line.bufcolor[i].foreColor == color_badbracket) {
         el->el_line.bufcolor[i] = -1;                      // reset to default colours
         term__repaint(el, i);
      }
   }

   // char* stack for pointers to locations of brackets
   stack<int> locBrackets;

   if (sBuffer.Length() > 0) {
      int cursorPos = el->el_line.cursor - el->el_line.buffer;
      bracketPos = cursorPos;

      // check against each bracket type
      int bIndex = 0;

      for (bIndex = 0; bIndex < amtBrackets; bIndex++) {
         // if current char is equal to opening bracket, push onto stack
         if (sBuffer[bracketPos] == bTypes[bIndex][0]) {
            locBrackets.push(bracketPos);
            foundParenIdx = 0;
            break;
         } else if (sBuffer[bracketPos] == bTypes[bIndex][1]) {
            locBrackets.push(bracketPos);
            foundParenIdx = 1;
            break;
         }
      }

      // current cursor char is not an open bracket, and there is a previous char to check
      if (foundParenIdx == -1 && bracketPos > 0) {
         //check previously typed char for being a closing bracket
         bracketPos--;
         // check against each bracket type
         bIndex = 0;

         for (bIndex = 0; bIndex < amtBrackets; bIndex++) {
            // if current char is equal to closing bracket, push onto stack
            if (sBuffer[bracketPos] == bTypes[bIndex][1]) {
               locBrackets.push(bracketPos);
               foundParenIdx = 1;
               break;
            }
         }
      }

      // no bracket found on either current or previous char, return.
      if (foundParenIdx == -1) {
         return foundParenIdx;
      }

      // iterate through remaining letters until find a matching closing bracket
      // if another open bracket of the same type is found, push onto stack
      // and pop on next closing bracket match
      int step = 1;

      if (foundParenIdx == 1) {
         step = -1;
      }

      for (int i = bracketPos + step; i >= 0 && i < sBuffer.Length(); i += step) {
         //if current char is equal to another opening bracket, push onto stack
         if (sBuffer[i] == bTypes[bIndex][foundParenIdx]) {
            // push index of bracket
            locBrackets.push(i);
         }
         //if current char is equal to closing bracket
         else if (sBuffer[i] == bTypes[bIndex][1 - foundParenIdx]) {
            // pop previous opening bracket off stack
            locBrackets.pop();

            // if previous opening was the last entry, then highlight match
            if (locBrackets.empty()) {
               colorBrackets(el, bracketPos, i, color_bracket);
               break;
            }
         }
      }

      if (!locBrackets.empty()) {
         colorBrackets(el, bracketPos, bracketPos, color_badbracket);
      }
   }

   return foundParenIdx;
} // matchParentheses


/**
 *      Highlight a word within the buffer.
 *      Requires the start and end index of the word, and the color pair index (class or type).
 *      Writes colour info for each char in range to el->el_line.bufcol.
 *      Background colour is set to the same as the current terminal background colour.
 *      Foreground (text) colour is set according to the type of word being highlighted (e.g. class or type).
 */
void
colorWord(EditLine* el, int first, int num, int textColor) {
   int bgColor = -1;            // default background
   bool anyChange = false;

   // add colour information to el.
   for (int index = first; index < first + num; ++index) {
      bool changed = el->el_line.bufcolor[index].foreColor != textColor;
      anyChange |= changed;
      el->el_line.bufcolor[index].foreColor = textColor;
      el->el_line.bufcolor[index].backColor = bgColor;

      if (changed) {
         term__repaint(el, index);
      }
   }

   if (anyChange) {
      term__setcolor(-1);
   }
} // colorWord


/*
 *      Set the colour information in the editline buffer,
 *      Then call repaint to repaint the chars with the new colour information
 */
void
colorBrackets(EditLine* el, int open, int close, int textColor) {
   int bgColor = -1;            // default background

   el->el_line.bufcolor[open].foreColor = textColor;
   el->el_line.bufcolor[open].backColor = bgColor;
   term__repaint(el, open);

   el->el_line.bufcolor[close].foreColor = textColor;
   el->el_line.bufcolor[close].backColor = bgColor;
   term__repaint(el, close);

   term__setcolor(-1);
}
