#ifndef COMMITTAB_HPP
#define COMMITTAB_HPP

#include"global.h"
#include"mainwindow.hpp"
#include<QtAlgorithms>
//#include<algorithm>
#include<random>

// connections

void MyMainWindow::makeCommitTabConnections(){
    connect(this->orderByDateCommitButton,SIGNAL(clicked(bool)),this,SLOT(refreshCommitTab()));
    connect(this->orderByUsernameCommitButton,SIGNAL(clicked(bool)),this,SLOT(refreshCommitTab()));
    connect(this->searchCommitLineEdit,SIGNAL(textChanged(QString)),this,SLOT(refreshCommitTab()));
}

// slots

void MyMainWindow::refreshCommitTab(){
    qDebug() << "refresh commit";
    this->commitList->clear();
    MyMainWindow::filterCommit();
    qDebug() << "finish" << endl;
}

bool byDate(GIT_COMMIT &a,GIT_COMMIT &b){
    if(a.time() == b.time()){
        return a.author() < b.author();
    } else return a.time() > b.time();
}

bool byAuthor(GIT_COMMIT &a,GIT_COMMIT &b){
    if(a.author() == b.author()){
        return a.time() > b.time();
    }else return a.author() < b.author();
}

void sortCommit(QVector<GIT_COMMIT> &v, bool isByDate){
    if(isByDate){
        qSort(v.begin(),v.end(),byDate);
    }else{
        qSort(v.begin(),v.end(),byAuthor);
    }
}

void swap(GIT_COMMIT &a,GIT_COMMIT &b){
    std::swap(a._c,b._c);
    std::swap(a._id,b._id);
}

void MyMainWindow::filterCommit(){
    bool isByDate = this->orderByDateCommitButton->isChecked();
    QVector<GIT_COMMIT> v;
    int i = 0;
    QString keyword = this->searchCommitLineEdit->text();
    for(auto c : global_repo->commits()){
        auto searchFor = toq(c.message());
        if(keyword.length() == 0 || searchFor.indexOf(keyword) >=0 ){
            v.append(c);
        }
    }
    qDebug() << isByDate << endl;
    sortCommit(v,isByDate);
    for(auto c: v){
        auto author = toq(c.author());
        auto time = toq(c.time());
        auto message = toq(c.message());
        auto temp = author +" , " + time + " , " + message;
        new QListWidgetItem(temp,this->commitList);
    }
}

#endif // BRANCHTAB_HPP
