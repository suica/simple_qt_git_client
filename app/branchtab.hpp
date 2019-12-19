#ifndef BRANCHTAB_HPP
#define BRANCHTAB_HPP

#include"global.h"
#include"mainwindow.hpp"

// connections

void MyMainWindow::makeBranchTabConnections(){
    connect(this->addBranchButton,SIGNAL(clicked(bool)),this,SLOT(addBranch()));
    connect(this->deleteBranchButton,SIGNAL(clicked(bool)),this,SLOT(deleteBranch()));
    connect(this->checkoutBranchButton,SIGNAL(clicked(bool)),this,SLOT(checkoutBranch()));
}

// slots

void MyMainWindow::refreshBranchTab(){
    qDebug() << "refresh branch" << global_repo;
    this->branchList->clear();
    for(auto branch : global_repo->branches()){
        auto q = toq(branch.name());
        new QListWidgetItem(q,this->branchList);
    }
    qDebug() << "finish" << endl;
}

void MyMainWindow::addBranch(){
    qDebug() << "add branch" << endl;
    bool ok;
    QString input = QInputDialog::getText(NULL,tr("New Branch"),tr("branch name"),QLineEdit::Normal,tr("newbranch"),&ok);
    if(ok){
        if(!input.isEmpty()){
            try{
                global_repo->branches().create(input.toStdString());
            }catch(...){
                QMessageBox::warning(NULL,tr("Error"),tr("222"));
            }
        }else{
            QMessageBox::warning(NULL,tr("??"),tr("222"));
        }
    }
    emit(refreshBranchTab());
}
void MyMainWindow::deleteBranch(){
    auto choice = QMessageBox::warning(NULL,tr("Delete Branch"),tr("Sure?"));
    auto chosenBranch = this->branchList->currentItem()->text();
    qDebug() << "delete branch" << chosenBranch << endl;
    if(choice == QMessageBox::Ok){
        try{
            global_repo->branches().erase(chosenBranch.toStdString());
        }catch(...){
            QMessageBox::warning(NULL,tr("Delete Branch"),tr("Please try to checkout other branch first."));
        }
    }
    emit(refreshBranchTab());
}

void MyMainWindow::checkoutBranch(){
    auto chosenBranch = this->branchList->currentItem()->text();
    qDebug() << "chekcout branch" <<chosenBranch << endl;
    try{
        //QDialog* tip = new QDialog(NULL,tr(""));
        //QMessageBox::warning(NULL,tr("Checkout branch"),tr("doing!"));
        QMessageBox msgBox(QMessageBox::Warning, tr("Checkout brnach"), tr("ing"), QMessageBox::NoButton);
        msgBox.setModal(false);
        msgBox.show();
        global_repo->checkout_branch(chosenBranch.toStdString());
        msgBox.close();
    }catch(...){
        QMessageBox::warning(NULL,tr("Checkout branch"),tr("orz"));
    }
    emit(refreshTabs());
}

#endif // BRANCHTAB_HPP
