/****************************************************************************
** Meta object code from reading C++ file 'QBreakpadHttpUploader.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../QBreakpadHttpUploader.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QBreakpadHttpUploader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.0. It"
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
struct qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS = QtMocHelpers::stringData(
    "QBreakpadHttpUploader",
    "finished",
    "",
    "answer",
    "uploadDump",
    "abs_file_path",
    "onUploadProgress",
    "sent",
    "total",
    "onError",
    "QNetworkReply::NetworkError",
    "err",
    "onUploadFinished"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS_t {
    uint offsetsAndSizes[26];
    char stringdata0[22];
    char stringdata1[9];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[11];
    char stringdata5[14];
    char stringdata6[17];
    char stringdata7[5];
    char stringdata8[6];
    char stringdata9[8];
    char stringdata10[28];
    char stringdata11[4];
    char stringdata12[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS_t qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS = {
    {
        QT_MOC_LITERAL(0, 21),  // "QBreakpadHttpUploader"
        QT_MOC_LITERAL(22, 8),  // "finished"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 6),  // "answer"
        QT_MOC_LITERAL(39, 10),  // "uploadDump"
        QT_MOC_LITERAL(50, 13),  // "abs_file_path"
        QT_MOC_LITERAL(64, 16),  // "onUploadProgress"
        QT_MOC_LITERAL(81, 4),  // "sent"
        QT_MOC_LITERAL(86, 5),  // "total"
        QT_MOC_LITERAL(92, 7),  // "onError"
        QT_MOC_LITERAL(100, 27),  // "QNetworkReply::NetworkError"
        QT_MOC_LITERAL(128, 3),  // "err"
        QT_MOC_LITERAL(132, 16)   // "onUploadFinished"
    },
    "QBreakpadHttpUploader",
    "finished",
    "",
    "answer",
    "uploadDump",
    "abs_file_path",
    "onUploadProgress",
    "sent",
    "total",
    "onError",
    "QNetworkReply::NetworkError",
    "err",
    "onUploadFinished"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQBreakpadHttpUploaderENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    1,   47,    2, 0x0a,    3 /* Public */,
       6,    2,   50,    2, 0x08,    5 /* Private */,
       9,    1,   55,    2, 0x08,    8 /* Private */,
      12,    0,   58,    2, 0x08,   10 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,    7,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject QBreakpadHttpUploader::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQBreakpadHttpUploaderENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<QBreakpadHttpUploader, std::true_type>,
        // method 'finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'uploadDump'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onUploadProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'onError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QNetworkReply::NetworkError, std::false_type>,
        // method 'onUploadFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void QBreakpadHttpUploader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QBreakpadHttpUploader *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->finished((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->uploadDump((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onUploadProgress((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        case 3: _t->onError((*reinterpret_cast< std::add_pointer_t<QNetworkReply::NetworkError>>(_a[1]))); break;
        case 4: _t->onUploadFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QNetworkReply::NetworkError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QBreakpadHttpUploader::*)(QString );
            if (_t _q_method = &QBreakpadHttpUploader::finished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *QBreakpadHttpUploader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QBreakpadHttpUploader::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQBreakpadHttpUploaderENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QBreakpadHttpUploader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QBreakpadHttpUploader::finished(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
