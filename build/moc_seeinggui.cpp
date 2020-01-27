/****************************************************************************
** Meta object code from reading C++ file 'seeinggui.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/seeinggui.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'seeinggui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SeeingGui_t {
    QByteArrayData data[18];
    char stringdata0[228];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SeeingGui_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SeeingGui_t qt_meta_stringdata_SeeingGui = {
    {
QT_MOC_LITERAL(0, 0, 9), // "SeeingGui"
QT_MOC_LITERAL(1, 10, 15), // "newSeeingValues"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 13), // "newDIMMValues"
QT_MOC_LITERAL(4, 41, 15), // "debugDIMMSample"
QT_MOC_LITERAL(5, 57, 26), // "datacontainers::DIMMsample"
QT_MOC_LITERAL(6, 84, 6), // "sample"
QT_MOC_LITERAL(7, 91, 10), // "newMessage"
QT_MOC_LITERAL(8, 102, 7), // "message"
QT_MOC_LITERAL(9, 110, 5), // "error"
QT_MOC_LITERAL(10, 116, 19), // "debugGaussianSeeing"
QT_MOC_LITERAL(11, 136, 7), // "cv::Mat"
QT_MOC_LITERAL(12, 144, 5), // "image"
QT_MOC_LITERAL(13, 150, 14), // "displayMessage"
QT_MOC_LITERAL(14, 165, 13), // "debugGaussian"
QT_MOC_LITERAL(15, 179, 15), // "setWindowRadius"
QT_MOC_LITERAL(16, 195, 16), // "updateGaussPlots"
QT_MOC_LITERAL(17, 212, 15) // "updateDIMMPlots"

    },
    "SeeingGui\0newSeeingValues\0\0newDIMMValues\0"
    "debugDIMMSample\0datacontainers::DIMMsample\0"
    "sample\0newMessage\0message\0error\0"
    "debugGaussianSeeing\0cv::Mat\0image\0"
    "displayMessage\0debugGaussian\0"
    "setWindowRadius\0updateGaussPlots\0"
    "updateDIMMPlots"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SeeingGui[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,
       4,    1,   76,    2, 0x06 /* Public */,
       7,    2,   79,    2, 0x06 /* Public */,
       7,    1,   84,    2, 0x26 /* Public | MethodCloned */,
      10,    1,   87,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    2,   90,    2, 0x08 /* Private */,
      13,    1,   95,    2, 0x28 /* Private | MethodCloned */,
      14,    1,   98,    2, 0x08 /* Private */,
      15,    0,  101,    2, 0x08 /* Private */,
      16,    0,  102,    2, 0x08 /* Private */,
      17,    0,  103,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    8,    9,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, 0x80000000 | 11,   12,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    8,    9,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SeeingGui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SeeingGui *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newSeeingValues(); break;
        case 1: _t->newDIMMValues(); break;
        case 2: _t->debugDIMMSample((*reinterpret_cast< datacontainers::DIMMsample(*)>(_a[1]))); break;
        case 3: _t->newMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->newMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->debugGaussianSeeing((*reinterpret_cast< const cv::Mat(*)>(_a[1]))); break;
        case 6: _t->displayMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->displayMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->debugGaussian((*reinterpret_cast< const cv::Mat(*)>(_a[1]))); break;
        case 9: _t->setWindowRadius(); break;
        case 10: _t->updateGaussPlots(); break;
        case 11: _t->updateDIMMPlots(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SeeingGui::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SeeingGui::newSeeingValues)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SeeingGui::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SeeingGui::newDIMMValues)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SeeingGui::*)(datacontainers::DIMMsample );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SeeingGui::debugDIMMSample)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SeeingGui::*)(QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SeeingGui::newMessage)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SeeingGui::*)(const cv::Mat & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SeeingGui::debugGaussianSeeing)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SeeingGui::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_SeeingGui.data,
    qt_meta_data_SeeingGui,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SeeingGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SeeingGui::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SeeingGui.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int SeeingGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void SeeingGui::newSeeingValues()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SeeingGui::newDIMMValues()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SeeingGui::debugDIMMSample(datacontainers::DIMMsample _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SeeingGui::newMessage(QString _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 5
void SeeingGui::debugGaussianSeeing(const cv::Mat & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
