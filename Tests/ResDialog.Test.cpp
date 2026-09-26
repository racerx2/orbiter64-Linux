// not upstream: Orbiter.rc compiled by rc2cpp.py and built into Qt widgets by ResDialog.cpp
#include <catch2/catch_test_macros.hpp>
#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QToolButton>
#include <cmath>
#include <cstring>
#include <strings.h>
#include "ResDialog.h"
#include "resource.h"
#include "resource2.h"

// stubs: ResDialog.cpp logs through Log.cpp and finds module tables through Util.cpp
static int nwarn = 0;
void *ModuleProc (void*, const char*) { return nullptr; }
void LogOut_Warning (const char*, const char*, int, const char*, ...) { nwarn++; }

static QApplication &App ()
{
	static int argc = 1;
	static char name[] = "ResDialog.Test", *argv[] = {name, nullptr};
	if (qEnvironmentVariableIsEmpty ("QT_QPA_PLATFORM")) qputenv ("QT_QPA_PLATFORM", "offscreen");
	static QApplication app (argc, argv);
	return app;
}

TEST_CASE("Orbiter.rc compiles to a resource table", "[resdialog]")
{
	App();
	const RESTABLE *t = oapiResourceTable (nullptr);
	REQUIRE(t);
	REQUIRE(t->ndlg > 50);
	REQUIRE(oapiFindResDialog (nullptr, IDD_MAIN));
	REQUIRE(oapiFindResDialog (nullptr, IDD_OPTIONS_VISUAL));
	REQUIRE(!oapiFindResDialog (nullptr, 99999));
	for (size_t i = 0; i < t->nimg; i++) {
		QImage *img = oapiLoadResImage (nullptr, t->img[i].id);
		INFO(t->img[i].name);
		REQUIRE(img);
		REQUIRE(!img->isNull());
		delete img;
	}
}

TEST_CASE("every dialog template builds all its controls", "[resdialog]")
{
	App();
	const RESTABLE *t = oapiResourceTable (nullptr);
	for (size_t i = 0; i < t->ndlg; i++) {
		const RESDIALOG *d = t->dlg + i;
		INFO(d->name);
		QWidget *w = oapiCreateResDialog (nullptr, d->id, nullptr);
		REQUIRE(w);
		REQUIRE(oapiResId (w) == d->id);
		int n = 0;
		for (QObject *o : w->children())
			if (o->isWidgetType() && o->property ("resId").isValid()) n++;
		REQUIRE(n == d->nctrl);
		delete w;
	}
}

TEST_CASE("IDD_MAIN matches its template", "[resdialog]")
{
	App();
	QWidget *dlg = oapiCreateResDialog (nullptr, IDD_MAIN, nullptr);
	REQUIRE(dlg);
	REQUIRE(dlg->windowTitle() == "OpenOrbiter Launchpad");
	double bx = dlg->property ("resBaseX").toDouble();
	int by = dlg->property ("resBaseY").toInt();
	REQUIRE(bx > 3.0);
	REQUIRE(dlg->width() == (int)std::lround (LAUNCHPAD_WIN_WIDTH*bx/4.0));
	REQUIRE(dlg->height() == (int)std::lround (333*by/8.0));
	QPushButton *launch = DlgItem<QPushButton> (dlg, IDLAUNCH);
	REQUIRE(launch);
	REQUIRE(launch->isDefault());
	REQUIRE(launch->text() == "&Launch Orbiter");
	QLabel *logo = DlgItem<QLabel> (dlg, IDC_LOGO);
	REQUIRE(logo);
	REQUIRE(!logo->pixmap().isNull());
	REQUIRE(!DlgItem<QCheckBox> (dlg, IDLAUNCH));
	delete dlg;
}

TEST_CASE("radio buttons group by WS_GROUP", "[resdialog]")
{
	App();
	QWidget *dlg = oapiCreateResDialog (nullptr, IDD_RECPLAY, nullptr);
	QRadioButton *att[2] = {DlgItem<QRadioButton> (dlg, IDC_REC_ATTECL), DlgItem<QRadioButton> (dlg, IDC_REC_ATTHOR)};
	QRadioButton *pos = DlgItem<QRadioButton> (dlg, IDC_REC_POSECL);
	REQUIRE((att[0] && att[1] && pos));
	pos->setChecked (true);
	att[0]->setChecked (true);
	att[1]->setChecked (true);
	REQUIRE(!att[0]->isChecked());
	REQUIRE(pos->isChecked());
	delete dlg;
}

TEST_CASE("labels grow into free space, not over their neighbours", "[resdialog]")
{
	App();
	QWidget *dlg = oapiCreateResDialog (nullptr, IDD_OPTIONS_VISUAL, nullptr);
	QCheckBox *cb = DlgItem<QCheckBox> (dlg, IDC_OPT_VIS_REFWATER);
	QCheckBox *next = DlgItem<QCheckBox> (dlg, IDC_OPT_VIS_RIPPLE);
	REQUIRE((cb && next));
	REQUIRE(cb->geometry().right() < next->geometry().left());
	REQUIRE((cb->width() >= cb->sizeHint().width() || cb->geometry().right() == next->geometry().left() - 2));
	delete dlg;
}

TEST_CASE("up-down control steps toward its upper limit", "[resdialog]")
{
	App();
	QWidget parent;
	QLineEdit *buddy = new QLineEdit (&parent);
	ResUpDown *ud = new ResUpDown (&parent, false, true, false);
	ud->SetBuddy (buddy);
	ud->SetRange (0, 10);
	ud->SetPos (5);
	REQUIRE(buddy->text() == "5");
	int last = 0;
	QObject::connect (ud, &ResUpDown::valueChanged, [&last](int, int delta) { last = delta; });
	QList<QToolButton*> bt = ud->findChildren<QToolButton*>();
	REQUIRE(bt.size() == 2);
	bt[0]->click();
	REQUIRE(ud->Pos() == 6);
	REQUIRE(last == 1);
	buddy->setText ("10");
	bt[0]->click();
	REQUIRE(ud->Pos() == 10);
	ud->SetRange (10, 0); // inverted range: "up" counts down
	ud->SetPos (5);
	bt[0]->click();
	REQUIRE(ud->Pos() == 4);
	REQUIRE(last == -1);
	ResUpDown *w = new ResUpDown (&parent, true, false, true);
	w->SetRange (0, 3);
	w->SetPos (3);
	w->findChildren<QToolButton*>()[0]->click();
	REQUIRE(w->Pos() == 0);
}

TEST_CASE("registered control classes replace placeholders", "[resdialog]")
{
	App();
	const RESTABLE *t = oapiResourceTable (nullptr);
	const RESDIALOG *dg = nullptr;
	int id = 0;
	for (size_t i = 0; i < t->ndlg && !dg; i++)
		for (int j = 0; j < t->dlg[i].nctrl; j++)
			if (t->dlg[i].ctrl[j].cls && !strcasecmp (t->dlg[i].ctrl[j].cls, "OrbiterCtrl_Gauge")) {
				dg = t->dlg + i, id = t->dlg[i].ctrl[j].id;
				break;
			}
	REQUIRE(dg);
	oapiRegisterResControl ("orbiterctrl_gauge", [](const RESCONTROL*, QWidget *parent) -> QWidget* { return new QProgressBar (parent); });
	QWidget *dlg = oapiCreateResDialog (nullptr, dg->id, nullptr);
	REQUIRE(DlgItem<QProgressBar> (dlg, id));
	delete dlg;
}
