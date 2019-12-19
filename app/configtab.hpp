#ifndef CONFIGTAB_H
#define CONFIGTAB_H

#include"global.h"
#include"mainwindow.hpp"

#include"dialogwidget.h"

const QString keyPlaceholder = QString("namespace.keyname");
const QString valuePlaceholder = QString("valueOfKey");

void MyMainWindow::makeConfigTabConnections(){
    connect(this->addConfigButton,SIGNAL(clicked(bool)),this,SLOT(addConfigEntry()));
    connect(this->deleteConfigButton,SIGNAL(clicked(bool)),this,SLOT(deleteConfigEntry()));
    connect(this->quickSetButton,SIGNAL(clicked(bool)),this,SLOT(editUsernameEmail()));
}

void MyMainWindow::refreshConfigTab(){
    qDebug() << "refresh config" << endl;
    auto table = this->configTable;
    auto configVariables = global_repo->config();
    table->setRowCount(0);
    for(auto c: configVariables){
        table->insertRow(table->rowCount());
        auto key = toq(c.name());
        auto value = toq(c.value());
        auto keyItem = new QTableWidgetItem(key);
        auto valueItem = new QTableWidgetItem(value);
        table->setItem(table->rowCount()-1,0,keyItem);
        table->setItem(table->rowCount()-1,1,valueItem);
        if(value == valuePlaceholder){
            valueItem->setBackgroundColor( QColor(255, 0, 0, 100) );
        }
    }
}

void MyMainWindow::updateConfigEntry(){
    qDebug() << "update config entry" << endl;
}

void MyMainWindow::addConfigEntry(){
    bool ok;
    QString input = QInputDialog::getText(NULL,tr("New Entry"),tr("new key name"),QLineEdit::Normal,keyPlaceholder,&ok);
    if(ok){
        auto key = input.toStdString();
        qDebug() << "add config entry" <<  ok << " " << input << endl;
        try{
            global_repo->config()[key] = valuePlaceholder.toStdString();
            emit(refreshConfigTab());
        }catch(...){
            QMessageBox::warning(NULL,tr("Key error"),tr("It should be the form of \"*.*\"!"));
        }
    }
}

void MyMainWindow::deleteConfigEntry(){
    auto rowIndex = this->configTable->currentRow();
    qDebug() << "deleteConfigEntry" << endl;
    auto key = this->configTable->item(rowIndex,0)->text();
    auto result = QMessageBox::warning(NULL,tr("delte entry"),tr("Really want to delete?"),QMessageBox::Ok | QMessageBox::Cancel);
    if(result == QMessageBox::Ok){
        qDebug() << "delete!!! " << key << endl;
        try{
            global_repo->config()[key.toStdString()].erase();
            emit(refreshConfigTab());
        }catch(...){
            QMessageBox::warning(NULL,tr("Key error"),tr("You cannot delete this, but you can change it value instead."));
        }
    }else{
        qDebug() << "delete abort." << endl;
    }
}

void MyMainWindow::updateUsernameEmail(QVector<QString> v){
    qDebug() << "updateUsernameEmail" << endl;
    auto config =  global_repo->config();
    config["user.name"] = v[0].toStdString();
    config["user.email"] = v[1].toStdString();
    emit(refreshConfigTab());
}

void MyMainWindow::editUsernameEmail(){
    QDialog* container = new QDialog();
    MyDialog *editDialog = new MyDialog(container);
    connect(editDialog,SIGNAL(finish(QVector<QString>)),this,SLOT(updateUsernameEmail(QVector<QString>)));
    container->exec();
}

#endif // CONFIGTAB_H
