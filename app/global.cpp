#include"global.h"
QString toq(std::string s){
    return QString::fromStdString(s);
}
QString toq(int s){
    return QString::fromStdString(std::to_string(s));
}
