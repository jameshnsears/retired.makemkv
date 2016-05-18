/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2015 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#include "qtgui.h"
#include "dirselectbox.h"
#include "qtapp.h"
#include "image_defs.h"

CDirSelectBox::CDirSelectBox(CGUIApClient* ap_client,CDirSelectBox::Style BoxStyle,const QString &Name, QWidgetList widgets , QWidget *parent) : QGroupBox(Name,parent)
{
    client = ap_client;
    style = BoxStyle;
    lineEditDir = new QLineEdit();
    connect(lineEditDir,SIGNAL(textChanged(const QString &)),this,SLOT(SlotTextChanged(const QString &)));
    QGridLayout *lay = new QGridLayout();

    memset(radioButtons,0,sizeof(radioButtons));

    unsigned int row =0;
    for (QWidgetList::iterator it=widgets.begin();it!=widgets.end();it++)
    {
        QAbstractButton* btn;
        btn = qobject_cast<QAbstractButton *>(*it);
        radioButtons[row] = btn;
        lay->addWidget(btn,row,0);
        row++;
        connect(btn, SIGNAL(toggled(bool)) , this , SLOT(SlotRadioToggled()) );
    }

    lay->addWidget(lineEditDir,row,0);
    lay->setColumnStretch(0,2);

    static QIcon *set_folder_icon;
    static QIcon *set_out_folder_icon;
    static QIcon *set_file_icon;
    static bool  set_folder_icon_initialized=false;

    if (false==set_folder_icon_initialized)
    {
        set_folder_icon = createIconPixmaps(AP_IMG_OPENFOLDER2,AP_IMG_OPENFOLDER2_COUNT);
        set_out_folder_icon = createIconPixmaps(AP_IMG_OPENFOLDER,AP_IMG_OPENFOLDER_COUNT);
        set_file_icon = createIconPixmaps(AP_IMG_OPENFILE2,AP_IMG_OPENFILE_COUNT);
        set_folder_icon_initialized=true;
    }

    switch(style)
    {
    case DirBoxDir:
        toolButtonAction  = new QAction(*set_folder_icon, Name, this);
        break;
    case DirBoxOutDir:
        toolButtonAction  = new QAction(*set_out_folder_icon, UI_QSTRING(APP_IFACE_ACT_SETFOLDER_NAME), this);
        toolButtonAction->setStatusTip(UI_QSTRING(APP_IFACE_ACT_SETFOLDER_STIP));
        break;
    case DirBoxFile:
        toolButtonAction  = new QAction(*set_file_icon, Name, this);
        break;
    }
    connect(toolButtonAction, SIGNAL(triggered()), this, SLOT(SlotButtonPressed()));

    toolButtonSelect = new QToolButton();
    toolButtonSelect->setIconSize(adjustIconSize(toolButtonSelect->iconSize(),16));
    toolButtonSelect->setDefaultAction(toolButtonAction);
    lay->addWidget(toolButtonSelect,row,1);

    this->setLayout(lay);

    validState = false;
    EmitValidChanged();
}

void CDirSelectBox::setIndexValue(int Index)
{
    if (Index>=MaxRadioButtons) return;
    if (NULL==radioButtons[Index]) return;
    radioButtons[Index]->setChecked(true);
}

int CDirSelectBox::getIndexValue()
{
    for (int i=0;i<MaxRadioButtons;i++)
    {
        if (NULL==radioButtons[i]) return -1;
        if (radioButtons[i]->isChecked()) return i;
    }
    return -1;
}

void CDirSelectBox::SlotButtonPressed()
{
    QString dir;
    switch(style)
    {
    case DirBoxDir:
    case DirBoxOutDir:
        dir = QFileDialog::getExistingDirectory(
            this,
            UI_QSTRING(APP_IFACE_OPENFOLDER_TITLE),
            lineEditDir->text());
        break;
    case DirBoxFile:
        dir = QFileDialog::getOpenFileName(
            this,QString(),lineEditDir->text());
        break;
    }

    if ( (dir.isEmpty()) || (dir.isNull()) ) return;

    if (client && (style==DirBoxOutDir)) {
        client->SetSettingString(apset_path_DestDir,Utf16FromQString(dir));
    }

    setText(dir);
}

void CDirSelectBox::setText(const QString &Text)
{
    lineEditDir->setText(Text);
    EmitValidChanged();
    emit SignalChanged();
}

QAction* CDirSelectBox::selectDialogAction()
{
    return toolButtonAction;
}

bool CDirSelectBox::IsDirValid()
{
    return (false==lineEditDir->text().isEmpty());
}

QString CDirSelectBox::text()
{
    return lineEditDir->text();
}

void CDirSelectBox::SlotTextChanged(const QString &str)
{
    emit SignalChanged();
    EmitValidChanged();
}

void CDirSelectBox::EmitValidChanged()
{
    bool vs = IsDirValid();
    if (vs != validState)
    {
        validState = vs;
        emit SignalDirValidChanged();
    }
}

void CDirSelectBox::SlotRadioToggled()
{
    emit SignalIndexChanged();
}

void CDirSelectBox::setDirEnabled(bool Enabled)
{
    lineEditDir->setEnabled(Enabled);
    toolButtonAction->setEnabled(Enabled);
}



