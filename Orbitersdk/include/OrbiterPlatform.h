// not upstream: Linux counterparts of the windows.h types the SDK headers use

#ifndef __ORBITERPLATFORM_H
#define __ORBITERPLATFORM_H

#include <cstdint>
#include <cstddef>

// integer types keep their Win32 names and sizes
typedef uint8_t   BYTE;
typedef uint8_t   UINT8;
typedef uint16_t  WORD;
typedef uint32_t  DWORD;
typedef int16_t   INT16;
typedef int32_t   LONG;
typedef unsigned int UINT;
typedef int       BOOL;
typedef uint32_t  COLORREF;
typedef intptr_t  INT_PTR;
typedef uintptr_t UINT_PTR;
typedef intptr_t  LONG_PTR;
typedef uintptr_t DWORD_PTR;
typedef uint64_t  DWORDLONG;
typedef int64_t   LONGLONG;
typedef int       INT;
typedef float     FLOAT;
#define VOID void

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// windef.h plain data structs, same layout
typedef struct tagRECT { LONG left, top, right, bottom; } RECT;
typedef struct tagPOINT { LONG x, y; } POINT;
typedef struct tagSIZE { LONG cx, cy; } SIZE;
typedef RECT *LPRECT;
typedef SIZE *LPSIZE;
typedef wchar_t *LPWSTR; // wchar_t is UTF-32 on Linux, UTF-16 on Windows
typedef char *PSTR;
typedef const char *PCSTR;

// COLORREF is 0x00bbggrr, as on Windows
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))

// Linux PATH_MAX; Windows' 260 is too short for Linux paths
#define MAX_PATH 4096

// handle types are native: HWND -> QWidget (dialogs) / QWindow (render window), HINSTANCE -> dlopen handle (void*),
// HDC -> QPainter, HFONT -> QFont, HPEN -> QPen, HBRUSH -> QBrush, HBITMAP -> QImage, window messages -> QEvent
class QWidget;
class QWindow;
class QEvent;
class QPainter;
class QFont;
class QPen;
class QBrush;
class QImage;

// replaces DLGPROC: called once after a dialog is built from its resource; the module connects its controls' Qt signals
typedef void (*DLGINIT)(QWidget *hDlg, void *context);

#endif // !__ORBITERPLATFORM_H
