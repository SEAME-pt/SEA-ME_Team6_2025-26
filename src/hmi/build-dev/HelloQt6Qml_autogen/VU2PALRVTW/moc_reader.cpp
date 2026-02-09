/****************************************************************************
** Meta object code from reading C++ file 'reader.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/backend/reader.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'reader.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
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
struct qt_meta_stringdata_CLASSReaderWorkerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSReaderWorkerENDCLASS = QtMocHelpers::stringData(
    "ReaderWorker",
    "speedReceived",
    "",
    "speed",
    "temperatureReceived",
    "temperature",
    "distanceReceived",
    "distance",
    "frontDistanceReceived",
    "frontDistance",
    "wheelSpeedReceived",
    "wheelSpeed",
    "voltageReceived",
    "voltage",
    "voltageLevelReceived",
    "status",
    "std::string",
    "source",
    "headingReceived",
    "heading",
    "connectionError",
    "error",
    "connected",
    "startReading",
    "stopReading"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSReaderWorkerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,
       4,    1,   89,    2, 0x06,    3 /* Public */,
       6,    1,   92,    2, 0x06,    5 /* Public */,
       8,    1,   95,    2, 0x06,    7 /* Public */,
      10,    1,   98,    2, 0x06,    9 /* Public */,
      12,    1,  101,    2, 0x06,   11 /* Public */,
      14,    2,  104,    2, 0x06,   13 /* Public */,
      18,    1,  109,    2, 0x06,   16 /* Public */,
      20,    1,  112,    2, 0x06,   18 /* Public */,
      22,    0,  115,    2, 0x06,   20 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      23,    0,  116,    2, 0x0a,   21 /* Public */,
      24,    0,  117,    2, 0x0a,   22 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    7,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double,   11,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 16,   15,   17,
    QMetaType::Void, QMetaType::Double,   19,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ReaderWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSReaderWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSReaderWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSReaderWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ReaderWorker, std::true_type>,
        // method 'speedReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'temperatureReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'distanceReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'frontDistanceReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'wheelSpeedReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'voltageReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'voltageLevelReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::string, std::false_type>,
        // method 'headingReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'connectionError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startReading'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopReading'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ReaderWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ReaderWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->speedReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->temperatureReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->distanceReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->frontDistanceReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->wheelSpeedReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 5: _t->voltageReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 6: _t->voltageLevelReceived((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::string>>(_a[2]))); break;
        case 7: _t->headingReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 8: _t->connectionError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->connected(); break;
        case 10: _t->startReading(); break;
        case 11: _t->stopReading(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ReaderWorker::*)(double );
            if (_t _q_method = &ReaderWorker::speedReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)(double );
            if (_t _q_method = &ReaderWorker::temperatureReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)(double );
            if (_t _q_method = &ReaderWorker::distanceReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)(double );
            if (_t _q_method = &ReaderWorker::frontDistanceReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)(double );
            if (_t _q_method = &ReaderWorker::wheelSpeedReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)(double );
            if (_t _q_method = &ReaderWorker::voltageReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)(bool , std::string );
            if (_t _q_method = &ReaderWorker::voltageLevelReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)(double );
            if (_t _q_method = &ReaderWorker::headingReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)(QString );
            if (_t _q_method = &ReaderWorker::connectionError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ReaderWorker::*)();
            if (_t _q_method = &ReaderWorker::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject *ReaderWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ReaderWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSReaderWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ReaderWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void ReaderWorker::speedReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ReaderWorker::temperatureReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ReaderWorker::distanceReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ReaderWorker::frontDistanceReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ReaderWorker::wheelSpeedReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ReaderWorker::voltageReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ReaderWorker::voltageLevelReceived(bool _t1, std::string _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ReaderWorker::headingReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ReaderWorker::connectionError(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ReaderWorker::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSReaderENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSReaderENDCLASS = QtMocHelpers::stringData(
    "Reader",
    "speedReceived",
    "",
    "speed",
    "temperatureReceived",
    "temperature",
    "distanceReceived",
    "distance",
    "frontDistanceReceived",
    "frontDistance",
    "wheelSpeedReceived",
    "wheelSpeed",
    "voltageReceived",
    "voltage",
    "voltageLevelReceived",
    "status",
    "std::string",
    "source",
    "headingReceived",
    "heading",
    "connectionError",
    "error",
    "connected"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSReaderENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x06,    1 /* Public */,
       4,    1,   77,    2, 0x06,    3 /* Public */,
       6,    1,   80,    2, 0x06,    5 /* Public */,
       8,    1,   83,    2, 0x06,    7 /* Public */,
      10,    1,   86,    2, 0x06,    9 /* Public */,
      12,    1,   89,    2, 0x06,   11 /* Public */,
      14,    2,   92,    2, 0x06,   13 /* Public */,
      18,    1,   97,    2, 0x06,   16 /* Public */,
      20,    1,  100,    2, 0x06,   18 /* Public */,
      22,    0,  103,    2, 0x06,   20 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    7,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double,   11,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 16,   15,   17,
    QMetaType::Void, QMetaType::Double,   19,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Reader::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSReaderENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSReaderENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSReaderENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Reader, std::true_type>,
        // method 'speedReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'temperatureReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'distanceReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'frontDistanceReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'wheelSpeedReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'voltageReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'voltageLevelReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::string, std::false_type>,
        // method 'headingReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'connectionError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Reader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Reader *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->speedReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->temperatureReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->distanceReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->frontDistanceReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->wheelSpeedReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 5: _t->voltageReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 6: _t->voltageLevelReceived((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::string>>(_a[2]))); break;
        case 7: _t->headingReceived((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 8: _t->connectionError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->connected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Reader::*)(double );
            if (_t _q_method = &Reader::speedReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Reader::*)(double );
            if (_t _q_method = &Reader::temperatureReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Reader::*)(double );
            if (_t _q_method = &Reader::distanceReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Reader::*)(double );
            if (_t _q_method = &Reader::frontDistanceReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Reader::*)(double );
            if (_t _q_method = &Reader::wheelSpeedReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Reader::*)(double );
            if (_t _q_method = &Reader::voltageReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Reader::*)(bool , std::string );
            if (_t _q_method = &Reader::voltageLevelReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Reader::*)(double );
            if (_t _q_method = &Reader::headingReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Reader::*)(QString );
            if (_t _q_method = &Reader::connectionError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (Reader::*)();
            if (_t _q_method = &Reader::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject *Reader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Reader::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSReaderENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Reader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Reader::speedReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Reader::temperatureReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Reader::distanceReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Reader::frontDistanceReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Reader::wheelSpeedReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Reader::voltageReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Reader::voltageLevelReceived(bool _t1, std::string _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Reader::headingReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Reader::connectionError(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Reader::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
