/****************************************************************************
** Meta object code from reading C++ file 'file.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Client/file.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_File_t {
    QByteArrayData data[13];
    char stringdata0[200];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_File_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_File_t qt_meta_stringdata_File = {
    {
QT_MOC_LITERAL(0, 0, 4), // "File"
QT_MOC_LITERAL(1, 5, 14), // "uploadErrorBox"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 3), // "msg"
QT_MOC_LITERAL(4, 25, 19), // "on_mkdir_PB_clicked"
QT_MOC_LITERAL(5, 45, 19), // "on_flush_PB_clicked"
QT_MOC_LITERAL(6, 65, 17), // "on_del_PB_clicked"
QT_MOC_LITERAL(7, 83, 20), // "on_rename_PB_clicked"
QT_MOC_LITERAL(8, 104, 31), // "on_listWidget_itemDoubleClicked"
QT_MOC_LITERAL(9, 136, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(10, 153, 4), // "item"
QT_MOC_LITERAL(11, 158, 20), // "on_return_PB_clicked"
QT_MOC_LITERAL(12, 179, 20) // "on_upload_PB_clicked"

    },
    "File\0uploadErrorBox\0\0msg\0on_mkdir_PB_clicked\0"
    "on_flush_PB_clicked\0on_del_PB_clicked\0"
    "on_rename_PB_clicked\0"
    "on_listWidget_itemDoubleClicked\0"
    "QListWidgetItem*\0item\0on_return_PB_clicked\0"
    "on_upload_PB_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_File[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x0a /* Public */,
       4,    0,   57,    2, 0x08 /* Private */,
       5,    0,   58,    2, 0x08 /* Private */,
       6,    0,   59,    2, 0x08 /* Private */,
       7,    0,   60,    2, 0x08 /* Private */,
       8,    1,   61,    2, 0x08 /* Private */,
      11,    0,   64,    2, 0x08 /* Private */,
      12,    0,   65,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void File::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<File *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->uploadErrorBox((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->on_mkdir_PB_clicked(); break;
        case 2: _t->on_flush_PB_clicked(); break;
        case 3: _t->on_del_PB_clicked(); break;
        case 4: _t->on_rename_PB_clicked(); break;
        case 5: _t->on_listWidget_itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 6: _t->on_return_PB_clicked(); break;
        case 7: _t->on_upload_PB_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject File::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_File.data,
    qt_meta_data_File,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *File::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *File::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_File.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int File::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
