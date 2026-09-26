// not upstream: help viewer for Orbiter's HTML help projects (stands in for the HtmlHelp API and compiled .chm files)

#include "HtmlHelp.h"
#include "OrbiterAPI.h"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QPointer>
#include <QRegularExpression>
#include <QSplitter>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QUrl>
#include <string>
#include <strings.h>

// "html\\orbiter.chm" -> the resolved folder of the help project, empty if there is none
static QString HelpFolder (const std::string &chm)
{
	std::string f = chm;
	for (auto &c : f) if (c == '\\') c = '/';
	if (f.size() > 4 && !strcasecmp (f.c_str() + f.size() - 4, ".chm")) f.resize (f.size() - 4);
	QString dir = QString::fromStdString (oapiResolvePath (f.c_str()));
	return (QFileInfo (dir).isDir() ? QFileInfo (dir).absoluteFilePath() : QString());
}

// file inside a folder, matched case-insensitively
static QString FindFile (const QString &folder, const QString &name)
{
	return QString::fromStdString (oapiResolvePath ((folder + "/" + name).toUtf8().constData()));
}

class HelpWindow: public QWidget {
public:
	HelpWindow (QWidget *owner): QWidget (owner, Qt::Window)
	{
		resize (900, 640);
		QHBoxLayout *layout = new QHBoxLayout (this);
		layout->setContentsMargins (0, 0, 0, 0);
		QSplitter *split = new QSplitter (this);
		toc = new QTreeWidget (split);
		toc->setHeaderHidden (true);
		page = new QTextBrowser (split);
		page->setOpenLinks (false);
		split->addWidget (toc);
		split->addWidget (page);
		split->setStretchFactor (1, 1);
		split->setSizes ({240, 660});
		layout->addWidget (split);
		setAttribute (Qt::WA_DeleteOnClose);
		connect (toc, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem *item) {
			QString local = item->data (0, Qt::UserRole).toString();
			if (!local.isEmpty()) ShowTopic (local);
		});
		connect (page, &QTextBrowser::anchorClicked, this, [this](const QUrl &url) { FollowLink (url); });
	}

	bool Load (const QString &dir)
	{
		if (dir == folder) return true;
		folder = dir;
		toc->clear();
		deftopic.clear();
		QString contents;
		QDir d (folder);
		QStringList hhp = d.entryList ({"*.hhp"}, QDir::Files);
		if (!hhp.isEmpty()) { // project options: title, contents file, default topic
			QFile f (d.filePath (hhp.first()));
			if (f.open (QIODevice::ReadOnly)) {
				for (QString line : QString::fromLatin1 (f.readAll()).split ('\n')) {
					line = line.trimmed();
					if (line.startsWith ("Title=", Qt::CaseInsensitive)) setWindowTitle (line.mid (6));
					else if (line.startsWith ("Contents file=", Qt::CaseInsensitive)) contents = line.mid (14);
					else if (line.startsWith ("Default topic=", Qt::CaseInsensitive)) deftopic = line.mid (14);
				}
			}
		}
		if (contents.isEmpty()) {
			QStringList hhc = d.entryList ({"*.hhc"}, QDir::Files);
			if (!hhc.isEmpty()) contents = hhc.first();
		}
		if (!contents.isEmpty()) ReadContents (FindFile (folder, contents));
		page->setSearchPaths ({folder});
		return true;
	}

	void ShowTopic (const QString &topic)
	{
		QString t = (topic.isEmpty() ? deftopic : topic);
		while (t.startsWith ('/')) t.remove (0, 1);
		QString frag;
		int h = t.indexOf ('#');
		if (h >= 0) frag = t.mid (h+1), t.truncate (h);
		QUrl url = QUrl::fromLocalFile (FindFile (folder, t));
		if (!frag.isEmpty()) url.setFragment (frag);
		page->setSource (url);
	}

private:
	// sitemap: nested <UL> lists of <OBJECT type="text/sitemap"> entries with Name and Local parameters
	void ReadContents (const QString &path)
	{
		QFile f (path);
		if (!f.open (QIODevice::ReadOnly)) return;
		QString html = QString::fromLatin1 (f.readAll());
		static const QRegularExpression tag ("<\\s*(/?)\\s*(UL|OBJECT|param)\\b([^>]*)>", QRegularExpression::CaseInsensitiveOption);
		static const QRegularExpression attr ("(\\w+)\\s*=\\s*\"([^\"]*)\"");
		QList<QTreeWidgetItem*> stack;
		QTreeWidgetItem *last = nullptr;
		QString name, local;
		bool inobj = false;
		for (auto it = tag.globalMatch (html); it.hasNext(); ) {
			auto m = it.next();
			bool close = !m.captured (1).isEmpty();
			QString t = m.captured (2).toLower();
			if (t == "ul") {
				if (!close) stack.push_back (last);
				else if (!stack.isEmpty()) last = stack.takeLast();
			} else if (t == "object") {
				if (!close) {
					inobj = m.captured (3).contains ("text/sitemap", Qt::CaseInsensitive);
					name.clear(), local.clear();
				} else if (inobj) {
					QTreeWidgetItem *parent = (stack.isEmpty() ? nullptr : stack.back());
					QTreeWidgetItem *item = (parent ? new QTreeWidgetItem (parent) : new QTreeWidgetItem (toc));
					item->setText (0, name);
					item->setData (0, Qt::UserRole, local);
					last = item;
					inobj = false;
				}
			} else if (t == "param" && inobj) {
				QString pname, pvalue;
				for (auto a = attr.globalMatch (m.captured (3)); a.hasNext(); ) {
					auto am = a.next();
					if (!am.captured (1).compare ("name", Qt::CaseInsensitive)) pname = am.captured (2);
					else if (!am.captured (1).compare ("value", Qt::CaseInsensitive)) pvalue = am.captured (2);
				}
				if (!pname.compare ("Name", Qt::CaseInsensitive)) name = pvalue;
				else if (!pname.compare ("Local", Qt::CaseInsensitive)) local = pvalue;
			}
		}
	}

	// links into other help files ("other.chm::/topic.htm", "ms-its:other.chm::/topic.htm"), web links, and pages
	void FollowLink (const QUrl &url)
	{
		QString s = url.toString();
		int sep = s.indexOf (".chm::", 0, Qt::CaseInsensitive);
		if (sep >= 0) {
			QString chm = s.left (sep + 4), topic = s.mid (sep + 6);
			int colon = chm.lastIndexOf (':');
			if (colon > 1) chm = chm.mid (colon + 1); // ms-its: / mk:@MSITStore: prefixes
			if (chm.startsWith ("file://")) chm = QUrl (chm).toLocalFile();
			QString dir = HelpFolder (QFileInfo (QDir (folder + "/.."), chm).filePath().toStdString());
			if (dir.isEmpty()) dir = HelpFolder (chm.toStdString());
			if (!dir.isEmpty() && Load (dir)) ShowTopic (topic);
			return;
		}
		if (url.scheme() == "http" || url.scheme() == "https" || url.scheme() == "mailto") {
			QDesktopServices::openUrl (url);
			return;
		}
		page->setSource (url);
	}

	QTreeWidget *toc;
	QTextBrowser *page;
	QString folder;
	QString deftopic;
};

static QPointer<HelpWindow> g_help;

bool HtmlHelp (QWidget *owner, const char *file, const char *topic)
{
	if (!file) return false;
	std::string chm = file, t = (topic ? topic : "");
	size_t sep = chm.find ("::");
	if (sep != std::string::npos) { // "file.chm::/topic.htm"
		if (t.empty()) t = chm.substr (sep + 2);
		chm.resize (sep);
	}
	QString dir = HelpFolder (chm);
	if (dir.isEmpty()) return false;
	if (!g_help) g_help = new HelpWindow (owner ? owner->window() : nullptr);
	g_help->Load (dir);
	g_help->ShowTopic (QString::fromStdString (t));
	g_help->show();
	g_help->raise();
	g_help->activateWindow();
	return true;
}
