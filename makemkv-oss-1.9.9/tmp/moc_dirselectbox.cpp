/****************************************************************************
** Meta object code from reading C++ file 'dirselectbox.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../makemkvgui/src/dirselectbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dirselectbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CDirSelectBox[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      31,   14,   14,   14, 0x05,
      55,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      76,   14,   14,   14, 0x08,
     100,   96,   14,   14, 0x08,
     125,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CDirSelectBox[] = {
    "CDirSelectBox\0\0SignalChanged()\0"
    "SignalDirValidChanged()\0SignalIndexChanged()\0"
    "SlotButtonPressed()\0str\0"
    "SlotTextChanged(QString)\0SlotRadioToggled()\0"
};

void CDirSelectBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CDirSelectBox *_t = static_cast<CDirSelectBox *>(_o);
        switch (_id) {
        case 0: _t->SignalChanged(); break;
        case 1: _t->SignalDirValidChanged(); break;
        case 2: _t->SignalIndexChanged(); break;
        case 3: _t->SlotButtonPressed(); break;
        case 4: _t->SlotTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->SlotRadioToggled(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CDirSelectBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CDirSelectBox::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_CDirSelectBox,
      qt_meta_data_CDirSelectBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CDirSelectBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CDirSelectBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CDirSelectBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDirSelectBox))
        return static_cast<void*>(const_cast< CDirSelectBox*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int CDirSelectBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void CDirSelectBox::SignalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CDirSelectBox::SignalDirValidChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CDirSelectBox::SignalIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
