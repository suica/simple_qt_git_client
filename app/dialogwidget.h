#ifndef DIALOGWIDGET_H
#define DIALOGWIDGET_H

#include "global.h"
#include "ui_dialog.h"

class MyDialog: public QObject, public Ui::Dialog
{
    Q_OBJECT
public:
    MyDialog(QDialog *p){
        this->setupUi(p);
        p->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->okButton,SIGNAL(clicked(bool)),this,SLOT(sendData()));
        connect(this->okButton,SIGNAL(clicked(bool)),p,SLOT(close()));
        connect(this->cancelButton,SIGNAL(clicked(bool)),p,SLOT(close()));
    }
    //MyDialog(QDialog *p,QVector<QString> v = QVector<QString>()) {
        //MyDialog::MyDialog(p);
    //    itemPrototype = v;
    //    for(auto s : itemPrototype){
    //        addItem(s);
    //    }
    //}
signals:
    void finish(QVector<QString>);
private slots:
    void sendData(){
        qDebug() << "send data" << endl;
        //for(auto itemWidgets: itemWidgets){
        //    qDebug() << itemWidgets->text() << endl;
        //}
        QVector<QString> data;
        data.append(this->userName->text());
        data.append(this->userEmail->text());
        emit(finish(data));
    }
//private:
//    QVector<QString> itemPrototype;
//    QVector<QLineEdit*> itemWidgets;
//
//    void addItem(QString s){
//        auto label = new QLabel(s);
//        auto inputBox = new QLineEdit();
//        this->gridLayout->addWidget();
//        this->gridLayout->addWidget(inputBox);
//        itemWidgets.append(inputBox);
//    }
};

#endif // DIALOGWIDGET_H
