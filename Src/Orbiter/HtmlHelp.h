// not upstream: help viewer for Orbiter's HTML help projects (stands in for the HtmlHelp API and compiled .chm files)
// A help file "dir/name.chm" is read from the folder "dir/name", which holds the project's pages and .hhp/.hhc files.

#ifndef __HTMLHELP_H
#define __HTMLHELP_H

class QWidget;

// HH_DISPLAY_TOPIC counterpart: opens the help window on a topic of a help file ("file.chm" or "file.chm::/topic.htm");
// topic may be NULL for the project's default topic. Returns false if the help project is not found.
bool HtmlHelp (QWidget *owner, const char *file, const char *topic);

#endif // !__HTMLHELP_H
