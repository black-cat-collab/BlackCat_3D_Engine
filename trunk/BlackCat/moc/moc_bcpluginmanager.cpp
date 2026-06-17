/****************************************************************************
** Meta object code from reading C++ file 'bcpluginmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "BlackCat/bcpluginmanager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bcpluginmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSBCPluginManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSBCPluginManagerENDCLASS = QtMocHelpers::stringData(
    "BCPluginManager",
    "sigReloadWeb",
    "",
    "sigRunJsFromCore",
    "strJson",
    "sigFromWebFun",
    "WebParam",
    "sigKillProcess",
    "strInfo",
    "strTitle",
    "slotFromWebFun",
    "slotRunJSFromCore",
    "slotTrigerMenu",
    "QAction*",
    "act",
    "slotKillProcess"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSBCPluginManagerENDCLASS_t {
    uint offsetsAndSizes[32];
    char stringdata0[16];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[8];
    char stringdata5[14];
    char stringdata6[9];
    char stringdata7[15];
    char stringdata8[8];
    char stringdata9[9];
    char stringdata10[15];
    char stringdata11[18];
    char stringdata12[15];
    char stringdata13[9];
    char stringdata14[4];
    char stringdata15[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSBCPluginManagerENDCLASS_t::offsetsAndSizes) + ofs), len
Q_CONSTINIT static const qt_meta_stringdata_CLASSBCPluginManagerENDCLASS_t qt_meta_stringdata_CLASSBCPluginManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 15),  // "BCPluginManager"
        QT_MOC_LITERAL(16, 12),  // "sigReloadWeb"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 16),  // "sigRunJsFromCore"
        QT_MOC_LITERAL(47, 7),  // "strJson"
        QT_MOC_LITERAL(55, 13),  // "sigFromWebFun"
        QT_MOC_LITERAL(69, 8),  // "WebParam"
        QT_MOC_LITERAL(78, 14),  // "sigKillProcess"
        QT_MOC_LITERAL(93, 7),  // "strInfo"
        QT_MOC_LITERAL(101, 8),  // "strTitle"
        QT_MOC_LITERAL(110, 14),  // "slotFromWebFun"
        QT_MOC_LITERAL(125, 17),  // "slotRunJSFromCore"
        QT_MOC_LITERAL(143, 14),  // "slotTrigerMenu"
        QT_MOC_LITERAL(158, 8),  // "QAction*"
        QT_MOC_LITERAL(167, 3),  // "act"
        QT_MOC_LITERAL(171, 15)   // "slotKillProcess"
    },
    "BCPluginManager",
    "sigReloadWeb",
    "",
    "sigRunJsFromCore",
    "strJson",
    "sigFromWebFun",
    "WebParam",
    "sigKillProcess",
    "strInfo",
    "strTitle",
    "slotFromWebFun",
    "slotRunJSFromCore",
    "slotTrigerMenu",
    "QAction*",
    "act",
    "slotKillProcess"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBCPluginManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x06,    1 /* Public */,
       3,    1,   63,    2, 0x06,    2 /* Public */,
       5,    1,   66,    2, 0x06,    4 /* Public */,
       7,    2,   69,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    1,   74,    2, 0x0a,    9 /* Public */,
      11,    1,   77,    2, 0x0a,   11 /* Public */,
      12,    1,   80,    2, 0x0a,   13 /* Public */,
      15,    2,   83,    2, 0x0a,   15 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QJsonValue,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::QJsonValue,    6,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,    9,

       0        // eod
};

Q_CONSTINIT const QMetaObject BCPluginManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSBCPluginManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBCPluginManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBCPluginManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BCPluginManager, std::true_type>,
        // method 'sigReloadWeb'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sigRunJsFromCore'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sigFromWebFun'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QJsonValue, std::false_type>,
        // method 'sigKillProcess'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotFromWebFun'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QJsonValue, std::false_type>,
        // method 'slotRunJSFromCore'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotTrigerMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'slotKillProcess'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>
    >,
    nullptr
} };

void BCPluginManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BCPluginManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sigReloadWeb(); break;
        case 1: _t->sigRunJsFromCore((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->sigFromWebFun((*reinterpret_cast< std::add_pointer_t<QJsonValue>>(_a[1]))); break;
        case 3: _t->sigKillProcess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->slotFromWebFun((*reinterpret_cast< std::add_pointer_t<QJsonValue>>(_a[1]))); break;
        case 5: _t->slotRunJSFromCore((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->slotTrigerMenu((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 7: _t->slotKillProcess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BCPluginManager::*)();
            if (_t _q_method = &BCPluginManager::sigReloadWeb; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BCPluginManager::*)(QString );
            if (_t _q_method = &BCPluginManager::sigRunJsFromCore; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BCPluginManager::*)(QJsonValue );
            if (_t _q_method = &BCPluginManager::sigFromWebFun; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BCPluginManager::*)(QString , QString );
            if (_t _q_method = &BCPluginManager::sigKillProcess; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *BCPluginManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BCPluginManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBCPluginManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IPluginManager"))
        return static_cast< IPluginManager*>(this);
    if (!strcmp(_clname, "bc::IEventProcessor"))
        return static_cast< bc::IEventProcessor*>(this);
    return QObject::qt_metacast(_clname);
}

int BCPluginManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void BCPluginManager::sigReloadWeb()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void BCPluginManager::sigRunJsFromCore(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void BCPluginManager::sigFromWebFun(QJsonValue _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void BCPluginManager::sigKillProcess(QString _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
