/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2013 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <lgpl/aproxy.h>

#include "mainwnd.h"

#ifdef Q_OS_WIN
#include <objbase.h>
static void PlatformInitBeforeApp()
{
    CoInitializeEx(NULL,COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE);
}
void AppWait();
#else
#define PlatformInitBeforeApp()
#endif

int qMain(int argc, char **argv)
{
    PlatformInitBeforeApp();

    static CGUIApClient apc;
    QApplication app(argc, argv);

    if (argc>=3)
    {
        if (strcmp(argv[1],"debug")==0)
        {
            apc.EnableDebug(argv[2]);
        }
    }

#ifdef Q_WS_WIN
    QFont& txt_font = app.font("QMessageBox");
    app.setFont(txt_font,"QLabel");
    app.setFont(txt_font,"QComboBox");
    app.setFont(txt_font,"QTextEdit");
    app.setFont(txt_font,"QLineEdit");
    app.setFont(txt_font,"QDialogButtonBox");
    app.setFont(txt_font,"QAbstractItemView");
    QFont& menu_font = app.font("QMenu");
    app.setFont(menu_font,"QGroupBox");
#endif

#ifdef Q_WS_MAC
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    unsigned int errcode;
    if (false==apc.Init('G',"makemkvcon",&errcode))
    {
        QMessageBox::critical(NULL,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_INIT_FAILED));
        return 1;
    }

    AppGetInterfaceLanguageData(&apc);

    const char* appdir;
    char* appname = argv[0];
    char *aend = strrchr(appname,'/');
    if (!aend)
    {
        aend = strrchr(appname,'\\');
    }
    if (aend)
    {
        *aend =0;
        appdir = appname;
    } else {
        appdir = ".";
    }

    MainWnd mainWin(&apc,appdir);
    mainWin.show();
    int r = app.exec();

#ifdef APP_DEBUG_INPROC
    AppWait();
#endif

    apc.AppExiting();

    return r;
}

#if !defined(QT_NEEDS_QMAIN)
extern "C" int main(int argc, char **argv)
{
    return qMain(argc,argv);
}
#endif

