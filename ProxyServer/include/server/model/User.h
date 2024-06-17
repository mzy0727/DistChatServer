#pragma once

#include <string>
#include <random>
using namespace std;

// User表的ORM类
class User{
public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline"){
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->state = state;
    }
    void setId(int id){
        this->id = id;
    }
    void setName(string name){
        this->name = name;
    }
    void setPwd(string pwd){
        this->password = pwd;
    }
    void setState(string state){
        this->state = state;
    }
    int getId(){
        return this->id;
    }
    string getName(){
        return this->name;
    }
    string getPwd(){
        return this->password;
    }
    string getState(){
        return this->state;
    }
        // 随机生成7-10位的QQ号
    static int generateRandomQQNumber() {
        // 使用随机设备作为种子
        std::random_device rd;
        std::mt19937 gen(rd());

        // 定义范围 [7, 10] 用于生成随机长度
        std::uniform_int_distribution<> lengthDist(7, 10);
        int length = lengthDist(gen);

        // 定义范围 [1, 9] 用于生成第一个数字
        std::uniform_int_distribution<> firstDigitDist(1, 9);
        int firstDigit = firstDigitDist(gen);

        // 定义范围 [0, 9] 用于生成其余数字
        std::uniform_int_distribution<> digitDist(0, 9);

        int qqNumber = firstDigit;
        for (int i = 1; i < length; ++i) {
            qqNumber = qqNumber * 10 + digitDist(gen);
        }
        return qqNumber;
    }
private:
    int id;
    string name;
    string password;
    string state;
};