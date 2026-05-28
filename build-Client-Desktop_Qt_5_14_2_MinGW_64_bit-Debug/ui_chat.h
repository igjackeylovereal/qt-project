/********************************************************************************
** Form generated from reading UI file 'chat.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHAT_H
#define UI_CHAT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Chat
{
public:
    QVBoxLayout *verticalLayout;
    QTextEdit *show_TE;
    QHBoxLayout *horizontalLayout;
    QLineEdit *input_LE;
    QPushButton *send_PB;

    void setupUi(QWidget *Chat)
    {
        if (Chat->objectName().isEmpty())
            Chat->setObjectName(QString::fromUtf8("Chat"));
        Chat->resize(489, 341);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../../Pointer/Lesson/Qt/chat.png"), QSize(), QIcon::Normal, QIcon::Off);
        Chat->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(Chat);
        verticalLayout->setSpacing(15);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(15, 15, 15, 15);
        show_TE = new QTextEdit(Chat);
        show_TE->setObjectName(QString::fromUtf8("show_TE"));
        QFont font;
        font.setFamily(QString::fromUtf8("Agency FB"));
        font.setPointSize(12);
        show_TE->setFont(font);
        show_TE->setReadOnly(true);

        verticalLayout->addWidget(show_TE);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(46);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        input_LE = new QLineEdit(Chat);
        input_LE->setObjectName(QString::fromUtf8("input_LE"));
        input_LE->setFont(font);

        horizontalLayout->addWidget(input_LE);

        send_PB = new QPushButton(Chat);
        send_PB->setObjectName(QString::fromUtf8("send_PB"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Agency FB"));
        font1.setPointSize(14);
        send_PB->setFont(font1);

        horizontalLayout->addWidget(send_PB);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Chat);

        QMetaObject::connectSlotsByName(Chat);
    } // setupUi

    void retranslateUi(QWidget *Chat)
    {
        Chat->setWindowTitle(QCoreApplication::translate("Chat", "\350\201\212\345\244\251", nullptr));
        send_PB->setText(QCoreApplication::translate("Chat", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Chat: public Ui_Chat {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHAT_H
