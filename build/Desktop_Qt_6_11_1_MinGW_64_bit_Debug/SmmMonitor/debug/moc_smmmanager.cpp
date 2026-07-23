/****************************************************************************
** Meta object code from reading C++ file 'smmmanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../SmmMonitor/smmmanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'smmmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
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
struct qt_meta_tag_ZN10SmmManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto SmmManager::qt_create_metaobjectdata<qt_meta_tag_ZN10SmmManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SmmManager",
        "saturationChanged",
        "",
        "newSaturation",
        "pulseRateChanged",
        "newPulseRate",
        "isSignalWeakChanged",
        "isSignalWeak",
        "beepVoiceChanged",
        "beepVoice",
        "pulseSearchChanged",
        "pulseSearch",
        "isPortConnectedChanged",
        "isPortConnected",
        "readData",
        "sendNextHandshakeByte",
        "onWatchdogTimeout",
        "connectToModule",
        "portName",
        "initializeBiolightModule",
        "getHistoryModel",
        "QSqlQueryModel*",
        "filterHistoryByDate",
        "startDate",
        "endDate",
        "clearFilter",
        "clearHistory",
        "deleteHistoryByDateRange",
        "saturation",
        "pulseRate"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'saturationChanged'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'pulseRateChanged'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'isSignalWeakChanged'
        QtMocHelpers::SignalData<void(bool)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 },
        }}),
        // Signal 'beepVoiceChanged'
        QtMocHelpers::SignalData<void(bool)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 9 },
        }}),
        // Signal 'pulseSearchChanged'
        QtMocHelpers::SignalData<void(bool)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 11 },
        }}),
        // Signal 'isPortConnectedChanged'
        QtMocHelpers::SignalData<void(bool)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'readData'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'sendNextHandshakeByte'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWatchdogTimeout'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Method 'connectToModule'
        QtMocHelpers::MethodData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'initializeBiolightModule'
        QtMocHelpers::MethodData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'getHistoryModel'
        QtMocHelpers::MethodData<QSqlQueryModel *()>(20, 2, QMC::AccessPublic, 0x80000000 | 21),
        // Method 'filterHistoryByDate'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 23 }, { QMetaType::QString, 24 },
        }}),
        // Method 'clearFilter'
        QtMocHelpers::MethodData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'clearHistory'
        QtMocHelpers::MethodData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'deleteHistoryByDateRange'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 23 }, { QMetaType::QString, 24 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'saturation'
        QtMocHelpers::PropertyData<int>(28, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
        // property 'pulseRate'
        QtMocHelpers::PropertyData<int>(29, QMetaType::Int, QMC::DefaultPropertyFlags, 1),
        // property 'isSignalWeak'
        QtMocHelpers::PropertyData<bool>(7, QMetaType::Bool, QMC::DefaultPropertyFlags, 2),
        // property 'beepVoice'
        QtMocHelpers::PropertyData<bool>(9, QMetaType::Bool, QMC::DefaultPropertyFlags, 3),
        // property 'pulseSearch'
        QtMocHelpers::PropertyData<bool>(11, QMetaType::Bool, QMC::DefaultPropertyFlags, 4),
        // property 'isPortConnected'
        QtMocHelpers::PropertyData<bool>(13, QMetaType::Bool, QMC::DefaultPropertyFlags, 5),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SmmManager, qt_meta_tag_ZN10SmmManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SmmManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10SmmManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10SmmManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10SmmManagerE_t>.metaTypes,
    nullptr
} };

void SmmManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SmmManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->saturationChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->pulseRateChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->isSignalWeakChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->beepVoiceChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->pulseSearchChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->isPortConnectedChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->readData(); break;
        case 7: _t->sendNextHandshakeByte(); break;
        case 8: _t->onWatchdogTimeout(); break;
        case 9: _t->connectToModule((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->initializeBiolightModule(); break;
        case 11: { QSqlQueryModel* _r = _t->getHistoryModel();
            if (_a[0]) *reinterpret_cast<QSqlQueryModel**>(_a[0]) = std::move(_r); }  break;
        case 12: _t->filterHistoryByDate((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 13: _t->clearFilter(); break;
        case 14: _t->clearHistory(); break;
        case 15: _t->deleteHistoryByDateRange((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SmmManager::*)(int )>(_a, &SmmManager::saturationChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SmmManager::*)(int )>(_a, &SmmManager::pulseRateChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SmmManager::*)(bool )>(_a, &SmmManager::isSignalWeakChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SmmManager::*)(bool )>(_a, &SmmManager::beepVoiceChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SmmManager::*)(bool )>(_a, &SmmManager::pulseSearchChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SmmManager::*)(bool )>(_a, &SmmManager::isPortConnectedChanged, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->saturation(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->pulseRate(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->isSignalWeak(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->beepVoice(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->pulseSearch(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->isPortConnected(); break;
        default: break;
        }
    }
}

const QMetaObject *SmmManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SmmManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10SmmManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SmmManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SmmManager::saturationChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void SmmManager::pulseRateChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void SmmManager::isSignalWeakChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SmmManager::beepVoiceChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void SmmManager::pulseSearchChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void SmmManager::isPortConnectedChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
