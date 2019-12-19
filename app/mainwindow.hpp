#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "global.h"
#include "libgit2pp.h"
#include "ui_mainwindow.h"

//#include "configtab.hpp"
//#include "committab.hpp"

class MyMainWindow: QObject, public Ui::MainWindow
{
    Q_OBJECT

public:
    MyMainWindow(){}
    MyMainWindow(QMainWindow *p){
        this->setupUi(p);
        makeSettings();
        makeAllConnections();
    }
    GIT_REPO *global_repo = nullptr;

signals:
    void repoReload();

private slots:
    void openRepo(){
        QString path = QFileDialog::getExistingDirectory(nullptr,"caption",".");
        if(path.length() == 0){
            return ;
        }
        loadRepo(path);
    }
    void loadRepo(QString path){
        qDebug() << path;
        try{
            auto repo = new GIT_REPO(path.toStdString());
            qDebug() << "load repo successfully" << endl;
            this->global_repo = repo;
        }catch(GIT_EXCEPTION_CANT_FIND){
            auto msgBox = new QMessageBox();
            msgBox->setText("Not repo.");
            msgBox->setInformativeText("Initialise as a repo?");
            msgBox->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            int result = msgBox->exec();
            qDebug() << "choose " << result;
            if(QMessageBox::Ok == result){
                try{
                    GIT_REPO(GIT_REPO::create_t::_create, path.toStdString());
                    auto repo = new GIT_REPO(path.toStdString());
                    qDebug() << repo << endl;
                    this->global_repo = repo;
                }catch(...){
                    qDebug() << "something strange happened" << endl;
                    quit();
                }
            }else{
                // user choose to do not initialise new repo
                return;
            }
        }
        if(global_repo){
        emit repoReload();
        }else{
            this->tabWidget->setEnabled(false);
        }
    }
    void quit(){
        exit(0);
    }

    void refreshTabs(){
        this->tabWidget->setEnabled(true);
        refreshBranchTab();
        refreshCommitTab();
        refreshConfigTab();
    }

    void refreshBranchTab();
    void refreshCommitTab();
    void refreshConfigTab();

    // branch
    void addBranch();
    void deleteBranch();
    void checkoutBranch();

    // commit
    void filterCommit();

    // config

    void updateConfigEntry();
    void updateUsernameEmail(QVector<QString>);
    void addConfigEntry();
    void deleteConfigEntry();
    void editUsernameEmail();

private:
    void makeSettings(){
        this->tabWidget->setEnabled(false);
        this->configTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        this->saveConfigButton->setVisible(false);
    }
    void makeAllConnections(){
        // gloabl actions
        connect(this->actionOpen,SIGNAL(triggered(bool)),this,SLOT(openRepo()));
        connect(this->actionQuit,SIGNAL(triggered(bool)),this,SLOT(quit()));

        connect(this,SIGNAL(repoReload()),this,SLOT(refreshTabs()));

        makeBranchTabConnections();
        makeCommitTabConnections();
        makeConfigTabConnections();
    }

    void makeBranchTabConnections();
    void makeConfigTabConnections();
    void makeCommitTabConnections();
};

#endif
