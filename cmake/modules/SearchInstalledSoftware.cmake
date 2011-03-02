#---Check for installed packages-----------------------------------------------------

#---Check if (N)Curses is installed. If neither is installed switch off editline----- 
if(editline)
  message(STATUS "Looking for NCurses")
  set(CURSES_NEED_NCURSES TRUE)
  find_package(Curses)
  if(NOT CURSES_FOUND)
    message(STATUS "Looking for Curses")
    set(CURSES_NEED_NCURSES FALSE)
    find_package(Curses)
  endif()
  if(CURSES_FOUND)
    if(CURSES_HAVE_CURSES_H)
      set(CURSES_HEADER_FILE ${CURSES_HAVE_CURSES_H})
    endif()
    if(CURSES_HAVE_NCURSES_H)
      set(CURSES_HEADER_FILE ${CURSES_HAVE_NCURSES_H})
    endif()
  else()
    set(editline OFF)
  endif()
endif()

#---Check for Zlib ------------------------------------------------------------------
if(NOT builtin_zlib)
  message(STATUS "Looking for ZLib")
  find_Package(ZLIB)
  if(NOT ZLIB_FOUND)
    message(STATUS "Zlib not found. Switching on builtin_zlib option")
    set(builtin_zlib ON)
   endif()
endif()
if(builtin_zlib)
  set(ZLIB_LIBRARY "")
endif()

#---Check for Freetype---------------------------------------------------------------
if(NOT builtin_freetype)
  message(STATUS "Looking for Freetype")
  find_package(Freetype)
  if(FREETYPE_FOUND)
    set(FREETYPE_INCLUDE_DIR ${FREETYPE_INCLUDE_DIR_freetype2})
  else()
    message(STATUS "FreeType not found. Switching on builtin_freetype option")
    set(builtin_freetype ON) 	
  endif()
endif()
if(builtin_freetype)  
  set(FREETYPE_INCLUDE_DIR ${CMAKE_BINARY_DIR}/graf2d/freetype/freetype-2.3.12/include)
  if(WIN32)
    set(FREETYPE_LIBRARIES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/freetype.lib")     
  else()
    set(FREETYPE_LIBRARIES "-L${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -lfreetype")
  endif()
endif()


#---Check for PCRE-------------------------------------------------------------------
if(NOT builtin_pcre)
  message(STATUS "Looking for PCRE")
  find_package(PCRE)
  if(PCRE_FOUND)
  else()
    message(STATUS "PCRE not found. Switching on builtin_pcre option")
    set(builtin_pcre ON) 	
  endif() 
endif()
if(builtin_pcre)
  set(PCRE_INCLUDE_DIR ${CMAKE_BINARY_DIR}/core/pcre/pcre-7.8)
  if(WIN32)
    set(PCRE_LIBRARIES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libpcre.lib) 
  else()
    set(PCRE_LIBRARIES "-L${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -lpcre") 
  endif()
endif()

#---Check for X11 which is mandatory lib on Unix--------------------------------------
if(x11)
  message(STATUS "Looking for X11")
  find_package(X11 REQUIRED)
  if(X11_FOUND)
    list(REMOVE_DUPLICATES X11_INCLUDE_DIR)
    message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES: ${X11_LIBRARIES}")
  else()
    message(FATAL_ERROR "libX11 and X11 headers must be installed.")
  endif()
  if(X11_Xpm_FOUND)
    message(STATUS "X11_Xpm_INCLUDE_PATH: ${X11_Xpm_INCLUDE_PATH}")
    message(STATUS "X11_Xpm_LIB: ${X11_Xpm_LIB}")
  else()
    message(FATAL_ERROR "libXpm and Xpm headers must be installed.")
  endif()
  if(X11_Xft_FOUND)
    message(STATUS "X11_Xft_INCLUDE_PATH: ${X11_Xft_INCLUDE_PATH}")
    message(STATUS "X11_Xft_LIB: ${X11_Xft_LIB}")
  else()
    message(FATAL_ERROR "libXft and Xft headers must be installed.")
  endif()
  if(X11_Xext_FOUND)
    message(STATUS "X11_Xext_INCLUDE_PATH: ${X11_Xext_INCLUDE_PATH}")
    message(STATUS "X11_Xext_LIB: ${X11_Xext_LIB}")
  else()
    message(FATAL_ERROR "libXext and Xext headers must be installed.")
  endif()
endif()

#---Check for all kind of graphics includes needed by libAfterImage--------------------
if(asimage)
  set(ASEXTRA_LIBRARIES)
  find_Package(GIF)
  if(GIF_FOUND)
    set(ASEXTRA_LIBRARIES ${ASEXTRA_LIBRARIES} ${GIF_LIBRARIES})
  endif()
  find_Package(TIFF)
  if(TIFF_FOUND)
    set(ASEXTRA_LIBRARIES ${ASEXTRA_LIBRARIES} ${TIFF_LIBRARIES})
  endif()
  find_Package(PNG)
  if(PNG_FOUND)
    set(ASEXTRA_LIBRARIES ${ASEXTRA_LIBRARIES} ${PNG_LIBRARIES})
  endif()
  find_Package(JPEG)
  if(JPEG_FOUND)
    set(ASEXTRA_LIBRARIES ${ASEXTRA_LIBRARIES} ${JPEG_LIBRARIES})
  endif()
endif()

#---Check for GSL library---------------------------------------------------------------
if(mathmore)
  message(STATUS "Looking for GSL")
  find_package(GSL)
  if(NOT GSL_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "GSL package not found and mathmore component required")
    else()
      set(mathmore OFF)
      message(STATUS "GSL not found. Switching off mathmore option")
    endif()
  endif()
endif()

#---Check for Python installation-------------------------------------------------------
if(python)
  message(STATUS "Looking for Python")
  find_package(PythonLibs)
  if(NOT PYTHONLIBS_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "PythonLibs package not found and python component required")
    else()
      set(python OFF)
      message(STATUS "Python not found. Switching off python option")
    endif()
  endif()
endif()

#---Check for OpenGL installation-------------------------------------------------------
if(opengl)
  message(STATUS "Looking for OpenGL")
  find_package(OpenGL)
  if(NOT OPENGL_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "OpenGL package not found and opengl option required")
    else()
      set(opengl OFF)
      message(STATUS "OpenGL not found. Switching off opengl option")
    endif()
  endif()
  if(APPLE)
    find_path(OPENGL_INCLUDE_DIR GL/gl.h DOC "Include for OpenGL on OSX")
  endif()
endif()

#---Check for Qt installation-------------------------------------------------------
if(qt)
  message(STATUS "Looking for Qt4")
  find_package(Qt4 COMPONENTS QtCore QtGui)
  if(NOT QT4_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "Qt4 package not found and qt component required")
    else()
      set(qt OFF)
      message(STATUS "Qt4 not found. Switching off qt option")
    endif()
  endif()
endif()


#---Check for Bonjour installation-------------------------------------------------------
if(bonjour)
  message(STATUS "Looking for Bonjour")
  find_package(Bonjour)
  if(NOT BONJOUR_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "Bonjour/Avahi libraries not found and Bonjour component required")
    else()
      set(bonjour OFF)
      message(STATUS "Bonjour not found. Switching off bonjour option")
    endif()
  endif()
endif()


#---Check for krb5 Support-----------------------------------------------------------
if(krb5)
  message(STATUS "Looking for Kerberos 5")
  find_package(Kerberos5)
  if(NOT KRB5_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "Kerberos 5 libraries not found and they are required")
    else()
      set(krb5 OFF)
      message(STATUS "Kerberos 5 not found. Switching off krb5 option")
    endif()
  endif()
endif()

#---Check for XML Parser Support-----------------------------------------------------------
if(xml)
  message(STATUS "Looking for LibXml2")
  find_package(LibXml2)
  if(NOT LIBXML2_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "LibXml2 libraries not found and they are required (xml option enabled)")
    else()
      set(xml OFF)
      message(STATUS "LibXml2 not found. Switching off xml option")
    endif()
  endif()
endif()

#---Check for OpenSSL------------------------------------------------------------------
if(ssl)
  message(STATUS "Looking for OpenSSL")
  find_package(OpenSSL)
  if(NOT OPENSSL_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "OpenSSL libraries not found and they are required (ssl option enabled)")
    else()
      set(ssl OFF)
      message(STATUS "OpenSSL not found. Switching off ssl option")
    endif()
  endif()
endif()

#---Check for Castor-------------------------------------------------------------------
if(castor)
  message(STATUS "Looking for Castor")
  find_package(Castor)
  if(NOT CASTOR_FOUND)
    if(fail-on-missing)
      message(FATAL_ERROR "Castor libraries not found and they are required (castor option enabled)")
    else()
      set(castor OFF)
      message(STATUS "Castor not found. Switching off castor option")
    endif()
  endif()
endif()
