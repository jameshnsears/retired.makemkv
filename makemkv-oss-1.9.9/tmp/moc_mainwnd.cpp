/****************************************************************************
** Meta object code from reading C++ file 'mainwnd.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../makemkvgui/src/mainwnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWnd[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      35,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x08,
      21,    8,    8,    8, 0x08,
      37,    8,    8,    8, 0x08,
      56,    8,    8,    8, 0x08,
      68,    8,    8,    8, 0x08,
      81,    8,    8,    8, 0x08,
      95,    8,    8,    8, 0x08,
     120,    8,    8,    8, 0x08,
     136,    8,    8,    8, 0x08,
     152,    8,    8,    8, 0x08,
     169,    8,    8,    8, 0x08,
     190,    8,    8,    8, 0x08,
     203,    8,    8,    8, 0x08,
     219,    8,    8,    8, 0x08,
     235,    8,    8,    8, 0x08,
     257,    8,    8,    8, 0x08,
     290,  284,    8,    8, 0x08,
     337,  332,    8,    8, 0x08,
     376,  332,    8,    8, 0x08,
     416,    8,    8,    8, 0x08,
     437,    8,    8,    8, 0x08,
     458,    8,    8,    8, 0x08,
     481,    8,    8,    8, 0x08,
     492,    8,    8,    8, 0x08,
     507,    8,    8,    8, 0x08,
     529,    8,    8,    8, 0x08,
     544,    8,    8,    8, 0x08,
     559,    8,    8,    8, 0x08,
     574,    8,    8,    8, 0x08,
     601,    8,    8,    8, 0x08,
     618,    8,    8,    8, 0x08,
     648,    8,    8,    8, 0x08,
     666,    8,    8,    8, 0x08,
     685,  681,    8,    8, 0x08,
     717,  713,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWnd[] = {
    "MainWnd\0\0SlotAbout()\0SlotOpenFiles()\0"
    "SlotOpenTestFile()\0SlotTimer()\0"
    "SlotOnIdle()\0SlotExiting()\0"
    "SlotOutputFolderEdited()\0SlotCloseDisk()\0"
    "SlotEjectDisk()\0SlotSaveAllMkv()\0"
    "SlotStartStreaming()\0SlotBackup()\0"
    "SlotCancelJob()\0SlotOpenDrive()\0"
    "SlotOpenDriveBigBtn()\0SlotTreeSelectionChanged()\0"
    "item,\0SlotTreeItemChanged(QTreeWidgetItem*,int)\0"
    "item\0SlotTreeItemExpanded(QTreeWidgetItem*)\0"
    "SlotTreeItemCollapsed(QTreeWidgetItem*)\0"
    "SlotToggleTreeItem()\0SlotSelectTreeItem()\0"
    "SlotUnselectTreeItem()\0SlotQuit()\0"
    "SlotSettings()\0SlotEmptyBoxChanged()\0"
    "SlotHelppage()\0SlotRegister()\0"
    "SlotPurchase()\0SlotInfoCboxIndexChanged()\0"
    "SlotInfoRevert()\0SlotProfileCboxIndexChanged()\0"
    "SlotNewInstance()\0SlotClearLog()\0key\0"
    "SlotInfoLineKeyPressed(int)\0url\0"
    "SlotLaunchUrl(QString)\0"
};

void MainWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWnd *_t = static_cast<MainWnd *>(_o);
        switch (_id) {
        case 0: _t->SlotAbout(); break;
        case 1: _t->SlotOpenFiles(); break;
        case 2: _t->SlotOpenTestFile(); break;
        case 3: _t->SlotTimer(); break;
        case 4: _t->SlotOnIdle(); break;
        case 5: _t->SlotExiting(); break;
        case 6: _t->SlotOutputFolderEdited(); break;
        case 7: _t->SlotCloseDisk(); break;
        case 8: _t->SlotEjectDisk(); break;
        case 9: _t->SlotSaveAllMkv(); break;
        case 10: _t->SlotStartStreaming(); break;
        case 11: _t->SlotBackup(); break;
        case 12: _t->SlotCancelJob(); break;
        case 13: _t->SlotOpenDrive(); break;
        case 14: _t->SlotOpenDriveBigBtn(); break;
        case 15: _t->SlotTreeSelectionChanged(); break;
        case 16: _t->SlotTreeItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: _t->SlotTreeItemExpanded((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 18: _t->SlotTreeItemCollapsed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 19: _t->SlotToggleTreeItem(); break;
        case 20: _t->SlotSelectTreeItem(); break;
        case 21: _t->SlotUnselectTreeItem(); break;
        case 22: _t->SlotQuit(); break;
        case 23: _t->SlotSettings(); break;
        case 24: _t->SlotEmptyBoxChanged(); break;
        case 25: _t->SlotHelppage(); break;
        case 26: _t->SlotRegister(); break;
        case 27: _t->SlotPurchase(); break;
        case 28: _t->SlotInfoCboxIndexChanged(); break;
        case 29: _t->SlotInfoRevert(); break;
        case 30: _t->SlotProfileCboxIndexChanged(); break;
        case 31: _t->SlotNewInstance(); break;
        case 32: _t->SlotClearLog(); break;
        case 33: _t->SlotInfoLineKeyPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 34: _t->SlotLaunchUrl((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWnd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWnd::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWnd,
      qt_meta_data_MainWnd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWnd))
        return static_cast<void*>(const_cast< MainWnd*>(this));
    if (!strcmp(_clname, "CApClient::INotifier"))
        return static_cast< CApClient::INotifier*>(const_cast< MainWnd*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 35)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 35;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
