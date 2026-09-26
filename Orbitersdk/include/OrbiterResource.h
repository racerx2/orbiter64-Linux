// not upstream: resource tables compiled from .rc scripts (cmake/rc2cpp.py) and the Qt dialogs built from them

#ifndef __ORBITERRESOURCE_H
#define __ORBITERRESOURCE_H

#include "OrbiterAPI.h"

// control kinds (rc2cpp.py classifies each .rc control into one of these)
enum RESKIND {
	RES_STATIC, RES_STATICIMAGE, RES_STATICFRAME, RES_BUTTON, RES_CHECKBOX, RES_RADIOBUTTON, RES_GROUPBOX,
	RES_EDIT, RES_COMBOBOX, RES_LISTBOX, RES_SCROLLBAR, RES_UPDOWN, RES_TRACKBAR, RES_TREEVIEW, RES_TABCONTROL,
	RES_PROGRESS, RES_RICHEDIT, RES_LISTVIEW, RES_CUSTOM
};

enum RESIMAGEKIND { RES_BITMAP, RES_ICON, RES_PNG };

// one control of a dialog template; coordinates in dialog units, styles keep the Win32 bit values of the .rc
struct RESCONTROL {
	int kind;
	int id;
	const char *text;     // UTF-8, or nullptr
	const char *cls;      // window class for RES_CUSTOM, else nullptr
	int imgid;            // image resource for image statics, else -1
	int x, y, cx, cy;
	DWORD style, exstyle;
	const char *idname;   // symbolic id from the .rc, or nullptr
};

struct RESDIALOG {
	int id;
	const char *name;
	const char *caption;
	const char *font;
	int fontsize, weight, italic;
	int x, y, cx, cy;
	DWORD style, exstyle;
	int nctrl;
	const RESCONTROL *ctrl;
};

struct RESIMAGE {
	int kind;
	int id;
	const char *name;
	const unsigned char *data;
	int size;
};

struct RESTABLE {
	size_t ndlg;
	const RESDIALOG *dlg;
	size_t nimg;
	const RESIMAGE *img;
};

class QWidget;
class QImage;

// resource table of a module (dlopen handle, via its generated oapiModuleResources), or of Orbiter for hModule == 0
OAPIFUNC const RESTABLE *oapiResourceTable (void *hModule);
OAPIFUNC const RESDIALOG *oapiFindResDialog (void *hModule, int resId);
OAPIFUNC const RESIMAGE *oapiFindResImage (void *hModule, int resId);

// image resource as a QImage (LoadBitmap/LoadIcon counterpart); caller owns the image
OAPIFUNC QImage *oapiLoadResImage (void *hModule, int resId);

// builds the Qt widgets of a dialog template (CreateDialogParam counterpart, without the message procedure)
OAPIFUNC QWidget *oapiCreateResDialog (void *hModule, int resId, QWidget *parent);

// dialog control by resource id (GetDlgItem counterpart)
OAPIFUNC QWidget *oapiResDlgItem (QWidget *hDlg, int id);

// resource id of a dialog or control widget (GetDlgCtrlID counterpart), 0 if none
OAPIFUNC int oapiResId (const QWidget *hWnd);

// custom control classes (RegisterClass counterpart for controls the .rc names by class)
typedef QWidget *(*RESCTRLFACTORY)(const RESCONTROL *ctrl, QWidget *parent);
OAPIFUNC void oapiRegisterResControl (const char *cls, RESCTRLFACTORY create);

#ifdef QT_WIDGETS_LIB
#include <QWidget>
// typed control lookup: DlgItem<QComboBox>(hDlg, IDC_X)
template<class T> inline T *DlgItem (QWidget *hDlg, int id)
{
	return qobject_cast<T*>(oapiResDlgItem (hDlg, id));
}
#endif

#endif // !__ORBITERRESOURCE_H
