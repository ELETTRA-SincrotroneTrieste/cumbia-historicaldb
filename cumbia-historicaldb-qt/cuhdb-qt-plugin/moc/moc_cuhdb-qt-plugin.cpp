/****************************************************************************
** Meta object code from reading C++ file 'cuhdb-qt-plugin.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../cuhdb-qt-plugin.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cuhdb-qt-plugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CuHdbQtPlugin_t {
    QByteArrayData data[1];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CuHdbQtPlugin_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CuHdbQtPlugin_t qt_meta_stringdata_CuHdbQtPlugin = {
    {
QT_MOC_LITERAL(0, 0, 13) // "CuHdbQtPlugin"

    },
    "CuHdbQtPlugin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CuHdbQtPlugin[] = {

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

void CuHdbQtPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject CuHdbQtPlugin::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_CuHdbQtPlugin.data,
    qt_meta_data_CuHdbQtPlugin,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CuHdbQtPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CuHdbQtPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CuHdbQtPlugin.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CuHdbPlugin_I"))
        return static_cast< CuHdbPlugin_I*>(this);
    if (!strcmp(_clname, "eu.elettra.qutils.CuHdbPlugin_I"))
        return static_cast< CuHdbPlugin_I*>(this);
    return QObject::qt_metacast(_clname);
}

int CuHdbQtPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}

QT_PLUGIN_METADATA_SECTION
static constexpr unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x30,  'o',  'r',  'g',  '.',  'q', 
    't',  '-',  'p',  'r',  'o',  'j',  'e',  'c', 
    't',  '.',  'Q',  't',  '.',  'Q',  'G',  'e', 
    'n',  'e',  'r',  'i',  'c',  'P',  'l',  'u', 
    'g',  'i',  'n',  'F',  'a',  'c',  't',  'o', 
    'r',  'y',  'I',  'n',  't',  'e',  'r',  'f', 
    'a',  'c',  'e', 
    // "className"
    0x03,  0x6d,  'C',  'u',  'H',  'd',  'b',  'Q', 
    't',  'P',  'l',  'u',  'g',  'i',  'n', 
    // "MetaData"
    0x04,  0xa1,  0x64,  'K',  'e',  'y',  's',  0x80, 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(CuHdbQtPlugin, CuHdbQtPlugin)

QT_WARNING_POP
QT_END_MOC_NAMESPACE
