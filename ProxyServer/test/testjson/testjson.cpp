#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
using namespace std;

void func1(){
    json js;
    js["type"] = 2;
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, hwo old are you?";
    cout<<js<<endl;
    string sendBuf = js.dump();
    cout<<sendBuf.c_str()<<endl;
}

void func2(){
    json js;
    // 添加数组
    js["id"] = {1,2,3,4,5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello , I'm zhang san!";
    js["msg"]["li si"] = "hello , I'm li si!";
    // 等同于上面
  //  js["msg"] = {{"zhang san","hello , I'm zhang san!"},{"li si","hello , I'm li si!"}};
    cout<<js<<endl;
}

void func3(){
    json js;
    
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    js["list"] = vec;

    map<int,string> m;
    m.insert({1,"北京"});
    m.insert({2,"上海"});
    m.insert({3,"广州"});

    js["path"] = m;

    cout<<js<<endl;
}

string func4(){
    json js;
    
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    js["list"] = vec;

    map<int,string> m;
    m.insert({1,"北京"});
    m.insert({2,"上海"});
    m.insert({3,"广州"});

    js["path"] = m;
    string buf = js.dump();
    return buf;

}
int main(){
    string buf = func4();
    json jsbuf = json::parse(buf);
    cout<<jsbuf["list"]<<endl;
    cout<<jsbuf["list"][2]<<endl;
    cout<<jsbuf["path"]<<endl;
    cout<<jsbuf["path"][0]<<endl;
    cout<<jsbuf["path"][0][1]<<endl;
    return 0;
}