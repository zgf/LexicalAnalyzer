/*
CodeGen保留关键词 _Continue;
_Continue意思是跳过当前捕获的字符(跳过空白等)
*/

//<HeadFile>
//这里会直接拷贝到LexParse文件中
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
//using std::pair;
using std::shared_ptr;
using std::string;
using std::function;
using std::unordered_map;
//</HeadFile>
//<PreClassGlobal>
//</PreClassGlobal>



// LexTag是enum class类型

//<LexTag>



//</LexTag>

// <TokenClass>
// 需要token返回其他成员的另外添加
//Tken类中 自带成员
//  LexTag Tag;
string Val;
int FindIndex;


// <TokenClass>

//<ClassMember>
//这里的内容会直接拷贝到LexParse 类里面做私有成员


//行号从1开始
int Line;
//列号从1开始
int ColsIndex;
//动作表
//执行动作的函数签名
//函数签名是定死的,但是想扩展可以在类的私有成员上做手脚=.=以及token的扩展内容上做变化
//shared_ptr<Token>(int& ReStartIndex, wstring& CatchContent)
//unordered_map<wstring, function<shared_ptr<Token>(int& ReStartIndex, string& CatchContent)>>ActionMap;
//源码
string SrcStr;
//</ClassMember>

//<RegularTypeDefine>
//"模式别名" : "模式";

//</RegularTypeDefine>

//<Action>
//"模式" : {语义动作};
//推荐直接使用LexTag名做函数名
//模式别名 : {语义动作};
//</Action>
//<UserDefineFunction>
//用户自定义函数位置
//</UserDefineFunction>