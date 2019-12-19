#include<iostream>
#include<Qt>

#include"branchtab.hpp"
#include"committab.hpp"
#include"configtab.hpp"

int main(int argc, char* argv[]) {
    //initialise the app
    QApplication app(argc, argv);

    //initialize a container for our mainwindow
    QMainWindow* mainwindow_container = new QMainWindow();

    //load our mainwindow
    MyMainWindow mainwindow(mainwindow_container);

    //show the app mainwindow
    mainwindow_container->show();

    //execute the whole app
    return app.exec();
}
