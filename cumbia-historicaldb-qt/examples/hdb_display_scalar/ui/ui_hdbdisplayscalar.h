/********************************************************************************
** Form generated from reading UI file 'hdbdisplayscalar.ui'
**
** Created by: Qt User Interface Compiler version 5.14.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HDBDISPLAYSCALAR_H
#define UI_HDBDISPLAYSCALAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HdbDisplayScalar
{
public:

//     void setupUi(QWidget *HdbDisplayScalar) // expanded by cuuimake v.1.0.0
    void setupUi(QWidget *HdbDisplayScalar, CumbiaPool * cu_p, const CuControlsFactoryPool & m_ctrl_factory_pool)
    {
        if (HdbDisplayScalar->objectName().isEmpty())
            HdbDisplayScalar->setObjectName(QString::fromUtf8("HdbDisplayScalar"));
        HdbDisplayScalar->resize(400, 300);

        retranslateUi(HdbDisplayScalar);

        QMetaObject::connectSlotsByName(HdbDisplayScalar);
    } // setupUi

    void retranslateUi(QWidget *HdbDisplayScalar)
    {
        HdbDisplayScalar->setWindowTitle(QCoreApplication::translate("HdbDisplayScalar", "HdbDisplayScalar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HdbDisplayScalar: public Ui_HdbDisplayScalar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HDBDISPLAYSCALAR_H
