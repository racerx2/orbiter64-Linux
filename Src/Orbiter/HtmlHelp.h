// not upstream: HtmlHelp API and .chm reader; a .chm here is a zip of the project's pages (cmake/hhc.py)

#ifndef __HTMLHELP_H
#define __HTMLHELP_H

#include <QTextBrowser>
#include <QUrl>

class QWidget;

// HH_DISPLAY_TOPIC counterpart: opens the help window on a topic of a help file ("file.chm" or "file.chm::/topic.htm");
// topic may be NULL for the project's default topic. Returns false if the help file is not found.
bool HtmlHelp (QWidget *owner, const char *file, const char *topic);

// page inside a help file as a URL the ChmBrowser loads: chm:<absolute .chm path>/<topic>
QUrl ChmUrl (const QString &chmfile, const QString &topic);

// "its:" / "ms-its:" / "mk:@MSITStore:" URLs ("its:Html\\Scenarios\\x.chm::/y.htm") -> ChmUrl, invalid if not a help page
QUrl ChmUrlFromIts (const QString &its);

// text browser that also shows the pages of help files
class ChmBrowser: public QTextBrowser {
public:
	using QTextBrowser::QTextBrowser;
	QVariant loadResource (int type, const QUrl &name) override;
};

#endif // !__HTMLHELP_H
