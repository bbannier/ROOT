// Copyright (C) 2008, Rene Brun and Fons Rademakers.
// All rights reserved. License: MIT/X11, see LICENSE
// Author: Axel Naumann, 2008

#include "Inuit/Widgets/UI.h"
#include "Inuit/Drivers/TerminalDriver.h"

using namespace Inuit;
void UI::ProcessInputs() {
   TerminalDriver& td = GetTerminalDriver();
   td.SetManagedMode(true);
   Draw();
   Pos pos = GetFocusedWidget()->GetPos();
   td.Goto(pos);
   while (!fQuitRequested) {
      std::list<Input> events;
      fInputDriver.ProcessAvailableInputs(events, true);

      td.SetManagedMode(false);
      for (std::list<Input>::const_iterator iI = events.begin();
         iI != events.end(); ++iI) {
         Event e = fInputFormat.InterpretInput(*iI);
         if (!GetFocusedWidget()->HandleEvent(e))
            PassToHandlers(*iI);
      }
      if (!fQuitRequested)
         td.SetManagedMode(true);
      td.Update();
   }
}

bool UI::PassToHandlers(const Input& e) {
   for (std::set<std::pair<InputHandler_t, void*> >::const_iterator iH = fHandlers.begin();
      iH != fHandlers.end(); ++iH)
      if ((iH->first)(*this, e, iH->second))
         return true;
   return false;
}

