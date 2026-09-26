// not upstream: Qt widgets built from the .rc dialog templates (see OrbiterResource.h)

#ifndef __RESDIALOG_H
#define __RESDIALOG_H

#include "OrbiterResource.h"
#include <QWidget>
#include <QTabBar>

class QToolButton;

// Orbiter's own resource table, generated from Orbiter.rc
const RESTABLE *OrbiterResources ();

// msctls_updown32 counterpart: two arrow buttons stepping an integer position, optionally shown in a buddy control
class ResUpDown: public QWidget {
	Q_OBJECT
public:
	ResUpDown (QWidget *parent, bool horizontal, bool setbuddyint, bool wrap);
	void SetBuddy (QWidget *buddy);
	QWidget *Buddy () const { return buddy; }
	void SetRange (int lower, int upper);
	void GetRange (int &lower, int &upper) const { lower = lo; upper = hi; }
	void SetPos (int pos);
	int Pos () const { return pos; }
signals:
	void valueChanged (int pos, int delta);
protected:
	void resizeEvent (QResizeEvent *event) override;
private:
	void Step (int dir);
	QToolButton *bt[2];
	QWidget *buddy = nullptr;
	int lo = 100, hi = 0, pos = 0; // Win32 default range: 100 .. 0
	bool horz, buddyint, wrap;
};

// SysTabControl32 counterpart: tab strip over a display area the owner fills with pages
class ResTabControl: public QWidget {
	Q_OBJECT
public:
	ResTabControl (QWidget *parent);
	QTabBar *Bar () const { return bar; }
	QRect DisplayRect () const; // TabCtrl_AdjustRect counterpart, in this control's coordinates
protected:
	void resizeEvent (QResizeEvent *event) override;
	void paintEvent (QPaintEvent *event) override;
private:
	QTabBar *bar;
};

#endif // !__RESDIALOG_H
