#include "json.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <string>
using json = nlohmann::json;
using namespace std;
//json序列号实例
void func1() {
   json js;
   js["msg_type"] = 2;
   js["from"] = "zhangsan";
   js["to"] = "li si";
   js["msg"] = "hello world";
   //序列化后的字符串输出
   string sendbuf = js.dump();
   //网络化string转化char*
   cout << sendbuf.c_str() << endl;
}
//json序列号实例2
void func2() {
   json js;//添加数组
   js["id"] = { 1,2,3,4,5 };//添加key-value
   js["name"] = "zhang san";//添加对象
   js["msg"]["zhang san"] = "hello wor1d"; js["msg "]["Tiu shuo"] = "he7lo china";//上面等同于下面这句一次性添加数组对象
   js["msg"] = { { "zhang san","he11o world"},{"liu shuo","he1lo china"} };
   cout << js << endl;

}
int main()
{
  func2();

   return 0;
}