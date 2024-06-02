#include <iostream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;


string fun1() {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    json js = v;
    json js1;
    js1["hello"] = v;
    cout << js1 << endl;
    cout << js << endl;

    json js3;
    js3["halo"] = "caonima";
    cout << js3 << endl;
    string sendbuf = js1.dump();
    cout << "after dumped:" << sendbuf << endl;

    return sendbuf;

}
string fun2() {
    //用json输出这个：{"id":[1,2,3,4,5],msg":{"liu shuo":"hello china","zhang san":"hello world"},"name":"zhang san"}
    json js;
    js["id"] = {1,2,3,4,5};
    js["msg"]["liu hao"] = "hello ,china";
    js["msg"]["zhaang san"] = "hello world";
    js["name"] = "li si";
    // cout << js;
    return js.dump();
}
void fun3() {

}
int main() {
    string jsRcv = fun2();
    json js = json::parse(jsRcv);
    cout << js["id"];
    // string recbuf = fun1();
    // json js = json::parse(recbuf);
    // vector<int> v = js["hello"];
    // for (int& x : v) cout << x << "->";

    // cout << "type of recbuf:";
    // printTypeInfo(recbuf);

    return 0;
}