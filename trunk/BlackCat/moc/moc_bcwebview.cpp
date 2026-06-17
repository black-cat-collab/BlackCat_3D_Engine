/****************************************************************************
** Meta object code from reading C++ file 'bcwebview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "BlackCat/bcwebview.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bcwebview.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSBCWebViewENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSBCWebViewENDCLASS = QtMocHelpers::stringData(
    "BCWebView",
    "sigMousePress",
    "",
    "nKey",
    "point",
    "sigMouseDbClick",
    "sigMouseRelease",
    "sigWheel",
    "angleDelta",
    "sigKeyPress",
    "sigKeyRelease",
    "slotReload",
    "slotLoadStarted",
    "slotLoadProgress",
    "value",
    "slotLoadFinished",
    "bOK",
    "slotSelectionText",
    "slotProxyAuthenticationRequired",
    "requestUrl",
    "QAuthenticator*",
    "authenticator",
    "proxyHost",
    "slotPermissionRequested",
    "securityOrigin",
    "QWebEnginePage::Feature",
    "feature"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSBCWebViewENDCLASS_t {
    uint offsetsAndSizes[54];
    char stringdata0[10];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[6];
    char stringdata5[16];
    char stringdata6[16];
    char stringdata7[9];
    char stringdata8[11];
    char stringdata9[12];
    char stringdata10[14];
    char stringdata11[11];
    char stringdata12[16];
    char stringdata13[17];
    char stringdata14[6];
    char stringdata15[17];
    char stringdata16[4];
    char stringdata17[18];
    char stringdata18[32];
    char stringdata19[11];
    char stringdata20[16];
    char stringdata21[14];
    char stringdata22[10];
    char stringdata23[24];
    char stringdata24[15];
    char stringdata25[24];
    char stringdata26[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSBCWebViewENDCLASS_t::offsetsAndSizes) + ofs), len
Q_CONSTINIT static const qt_meta_stringdata_CLASSBCWebViewENDCLASS_t qt_meta_stringdata_CLASSBCWebViewENDCLASS = {
    {
        QT_MOC_LITERAL(0, 9),  // "BCWebView"
        QT_MOC_LITERAL(10, 13),  // "sigMousePress"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 4),  // "nKey"
        QT_MOC_LITERAL(30, 5),  // "point"
        QT_MOC_LITERAL(36, 15),  // "sigMouseDbClick"
        QT_MOC_LITERAL(52, 15),  // "sigMouseRelease"
        QT_MOC_LITERAL(68, 8),  // "sigWheel"
        QT_MOC_LITERAL(77, 10),  // "angleDelta"
        QT_MOC_LITERAL(88, 11),  // "sigKeyPress"
        QT_MOC_LITERAL(100, 13),  // "sigKeyRelease"
        QT_MOC_LITERAL(114, 10),  // "slotReload"
        QT_MOC_LITERAL(125, 15),  // "slotLoadStarted"
        QT_MOC_LITERAL(141, 16),  // "slotLoadProgress"
        QT_MOC_LITERAL(158, 5),  // "value"
        QT_MOC_LITERAL(164, 16),  // "slotLoadFinished"
        QT_MOC_LITERAL(181, 3),  // "bOK"
        QT_MOC_LITERAL(185, 17),  // "slotSelectionText"
        QT_MOC_LITERAL(203, 31),  // "slotProxyAuthenticationRequired"
        QT_MOC_LITERAL(235, 10),  // "requestUrl"
        QT_MOC_LITERAL(246, 15),  // "QAuthenticator*"
        QT_MOC_LITERAL(262, 13),  // "authenticator"
        QT_MOC_LITERAL(276, 9),  // "proxyHost"
        QT_MOC_LITERAL(286, 23),  // "slotPermissionRequested"
        QT_MOC_LITERAL(310, 14),  // "securityOrigin"
        QT_MOC_LITERAL(325, 23),  // "QWebEnginePage::Feature"
        QT_MOC_LITERAL(349, 7)   // "feature"
    },
    "BCWebView",
    "sigMousePress",
    "",
    "nKey",
    "point",
    "sigMouseDbClick",
    "sigMouseRelease",
    "sigWheel",
    "angleDelta",
    "sigKeyPress",
    "sigKeyRelease",
    "slotReload",
    "slotLoadStarted",
    "slotLoadProgress",
    "value",
    "slotLoadFinished",
    "bOK",
    "slotSelectionText",
    "slotProxyAuthenticationRequired",
    "requestUrl",
    "QAuthenticator*",
    "authenticator",
    "proxyHost",
    "slotPermissionRequested",
    "securityOrigin",
    "QWebEnginePage::Feature",
    "feature"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBCWebViewENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   92,    2, 0x06,    1 /* Public */,
       5,    2,   97,    2, 0x06,    4 /* Public */,
       6,    2,  102,    2, 0x06,    7 /* Public */,
       7,    2,  107,    2, 0x06,   10 /* Public */,
       9,    1,  112,    2, 0x06,   13 /* Public */,
      10,    1,  115,    2, 0x06,   15 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,  118,    2, 0x0a,   17 /* Public */,
      12,    0,  119,    2, 0x0a,   18 /* Public */,
      13,    1,  120,    2, 0x0a,   19 /* Public */,
      15,    1,  123,    2, 0x0a,   21 /* Public */,
      17,    0,  126,    2, 0x0a,   23 /* Public */,
      18,    3,  127,    2, 0x0a,   24 /* Public */,
      23,    2,  134,    2, 0x0a,   28 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QPoint,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::QPoint,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::QPoint,    3,    4,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QPoint,    4,    8,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 20, QMetaType::QString,   19,   21,   22,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 25,   24,   26,

       0        // eod
};

Q_CONSTINIT const QMetaObject BCWebView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWebEngineView::staticMetaObject>(),
    qt_meta_stringdata_CLASSBCWebViewENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBCWebViewENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBCWebViewENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BCWebView, std::true_type>,
        // method 'sigMousePress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPoint, std::false_type>,
        // method 'sigMouseDbClick'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPoint, std::false_type>,
        // method 'sigMouseRelease'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPoint, std::false_type>,
        // method 'sigWheel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPoint, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPoint, std::false_type>,
        // method 'sigKeyPress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'sigKeyRelease'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'slotReload'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotLoadStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotLoadProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'slotLoadFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'slotSelectionText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotProxyAuthenticationRequired'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAuthenticator *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'slotPermissionRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePage::Feature, std::false_type>
    >,
    nullptr
} };

void BCWebView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BCWebView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sigMousePress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2]))); break;
        case 1: _t->sigMouseDbClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2]))); break;
        case 2: _t->sigMouseRelease((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2]))); break;
        case 3: _t->sigWheel((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2]))); break;
        case 4: _t->sigKeyPress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->sigKeyRelease((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->slotReload(); break;
        case 7: _t->slotLoadStarted(); break;
        case 8: _t->slotLoadProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->slotLoadFinished((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->slotSelectionText(); break;
        case 11: _t->slotProxyAuthenticationRequired((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QAuthenticator*>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 12: _t->slotPermissionRequested((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QWebEnginePage::Feature>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BCWebView::*)(int , QPoint );
            if (_t _q_method = &BCWebView::sigMousePress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BCWebView::*)(int , QPoint );
            if (_t _q_method = &BCWebView::sigMouseDbClick; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BCWebView::*)(int , QPoint );
            if (_t _q_method = &BCWebView::sigMouseRelease; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BCWebView::*)(QPoint , QPoint );
            if (_t _q_method = &BCWebView::sigWheel; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (BCWebView::*)(int );
            if (_t _q_method = &BCWebView::sigKeyPress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (BCWebView::*)(int );
            if (_t _q_method = &BCWebView::sigKeyRelease; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *BCWebView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BCWebView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBCWebViewENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWebEngineView::qt_metacast(_clname);
}

int BCWebView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEngineView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void BCWebView::sigMousePress(int _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BCWebView::sigMouseDbClick(int _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void BCWebView::sigMouseRelease(int _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void BCWebView::sigWheel(QPoint _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void BCWebView::sigKeyPress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void BCWebView::sigKeyRelease(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
