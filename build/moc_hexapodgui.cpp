/****************************************************************************
** Meta object code from reading C++ file 'hexapodgui.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/hexapodgui.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hexapodgui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KeyPressEater_t {
    QByteArrayData data[1];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KeyPressEater_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KeyPressEater_t qt_meta_stringdata_KeyPressEater = {
    {
QT_MOC_LITERAL(0, 0, 13) // "KeyPressEater"

    },
    "KeyPressEater"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KeyPressEater[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void KeyPressEater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KeyPressEater::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_KeyPressEater.data,
    qt_meta_data_KeyPressEater,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KeyPressEater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KeyPressEater::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KeyPressEater.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KeyPressEater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_HexapodGui_t {
    QByteArrayData data[23];
    char stringdata0[346];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HexapodGui_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HexapodGui_t qt_meta_stringdata_HexapodGui = {
    {
QT_MOC_LITERAL(0, 0, 10), // "HexapodGui"
QT_MOC_LITERAL(1, 11, 15), // "positionChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 12), // "coSysChanged"
QT_MOC_LITERAL(4, 41, 10), // "newMessage"
QT_MOC_LITERAL(5, 52, 7), // "message"
QT_MOC_LITERAL(6, 60, 5), // "error"
QT_MOC_LITERAL(7, 66, 23), // "updateStabilizationPlot"
QT_MOC_LITERAL(8, 90, 18), // "newPositionMessage"
QT_MOC_LITERAL(9, 109, 14), // "positionString"
QT_MOC_LITERAL(10, 124, 14), // "displayMessage"
QT_MOC_LITERAL(11, 139, 15), // "displayPosition"
QT_MOC_LITERAL(12, 155, 18), // "updateCoSysDisplay"
QT_MOC_LITERAL(13, 174, 22), // "updateDisplayPositions"
QT_MOC_LITERAL(14, 197, 18), // "update_plotsWindow"
QT_MOC_LITERAL(15, 216, 19), // "setUserDefinedCoSys"
QT_MOC_LITERAL(16, 236, 12), // "setZeroCoSys"
QT_MOC_LITERAL(17, 249, 10), // "moveToZero"
QT_MOC_LITERAL(18, 260, 21), // "moveToDisplayPosition"
QT_MOC_LITERAL(19, 282, 11), // "setDeadTime"
QT_MOC_LITERAL(20, 294, 18), // "setIntegrationTime"
QT_MOC_LITERAL(21, 313, 12), // "setMaxMotion"
QT_MOC_LITERAL(22, 326, 19) // "setProportionalGain"

    },
    "HexapodGui\0positionChanged\0\0coSysChanged\0"
    "newMessage\0message\0error\0"
    "updateStabilizationPlot\0newPositionMessage\0"
    "positionString\0displayMessage\0"
    "displayPosition\0updateCoSysDisplay\0"
    "updateDisplayPositions\0update_plotsWindow\0"
    "setUserDefinedCoSys\0setZeroCoSys\0"
    "moveToZero\0moveToDisplayPosition\0"
    "setDeadTime\0setIntegrationTime\0"
    "setMaxMotion\0setProportionalGain"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HexapodGui[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  114,    2, 0x06 /* Public */,
       3,    0,  115,    2, 0x06 /* Public */,
       4,    2,  116,    2, 0x06 /* Public */,
       4,    1,  121,    2, 0x26 /* Public | MethodCloned */,
       7,    0,  124,    2, 0x06 /* Public */,
       8,    1,  125,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    2,  128,    2, 0x0a /* Public */,
      10,    1,  133,    2, 0x2a /* Public | MethodCloned */,
      11,    1,  136,    2, 0x0a /* Public */,
      12,    0,  139,    2, 0x0a /* Public */,
      13,    0,  140,    2, 0x0a /* Public */,
      14,    0,  141,    2, 0x0a /* Public */,
      15,    0,  142,    2, 0x0a /* Public */,
      16,    0,  143,    2, 0x0a /* Public */,
      17,    0,  144,    2, 0x0a /* Public */,
      18,    0,  145,    2, 0x0a /* Public */,
      19,    0,  146,    2, 0x0a /* Public */,
      20,    0,  147,    2, 0x0a /* Public */,
      21,    0,  148,    2, 0x0a /* Public */,
      22,    0,  149,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    5,    6,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    5,    6,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void HexapodGui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HexapodGui *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->positionChanged(); break;
        case 1: _t->coSysChanged(); break;
        case 2: _t->newMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->newMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->updateStabilizationPlot(); break;
        case 5: _t->newPositionMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->displayMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->displayMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->displayPosition((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->updateCoSysDisplay(); break;
        case 10: _t->updateDisplayPositions(); break;
        case 11: _t->update_plotsWindow(); break;
        case 12: _t->setUserDefinedCoSys(); break;
        case 13: _t->setZeroCoSys(); break;
        case 14: _t->moveToZero(); break;
        case 15: _t->moveToDisplayPosition(); break;
        case 16: _t->setDeadTime(); break;
        case 17: _t->setIntegrationTime(); break;
        case 18: _t->setMaxMotion(); break;
        case 19: _t->setProportionalGain(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HexapodGui::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HexapodGui::positionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HexapodGui::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HexapodGui::coSysChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (HexapodGui::*)(QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HexapodGui::newMessage)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (HexapodGui::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HexapodGui::updateStabilizationPlot)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (HexapodGui::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HexapodGui::newPositionMessage)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject HexapodGui::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_HexapodGui.data,
    qt_meta_data_HexapodGui,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HexapodGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HexapodGui::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HexapodGui.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int HexapodGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void HexapodGui::positionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void HexapodGui::coSysChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void HexapodGui::newMessage(QString _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 4
void HexapodGui::updateStabilizationPlot()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void HexapodGui::newPositionMessage(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
