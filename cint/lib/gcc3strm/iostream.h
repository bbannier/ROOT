// Copyright (C) 1997-1999, 2000 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

#ifndef _CPP_BACKWARD_IOSTREAM_H
#define _CPP_BACKWARD_IOSTREAM_H 1

#include <bits/std_iostream.h>

using std::iostream;
using std::ostream;
using std::istream;
using std::ios;
using std::streambuf;

using std::cout;
using std::cin;
using std::cerr;
using std::clog;
#ifdef _GLIBCPP_USE_WCHAR_T
using std::wcout;
using std::wcin;
using std::wcerr;
using std::wclog;
#endif

using std::ws;
using std::endl;
using std::ends;
using std::flush;

#endif

// Local Variables:
// mode:C++
// End:
