/*
CodeGen保留关键词 _Continue;
_Continue意思是跳过当前捕获的字符(跳过空白等)
*/

<HeadFile>
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
</HeadFile>
<PreClassGlobal>

</PreClassGlobal>



// TokenTag是enum class类型

<TokenTag>
Id,
Number,
Include,
HeadFile,
Add,
NullSpace,
</TokenTag>

 <TokenClass>
// 需要token返回成员和token应该有的函数自行添加
//Tken类中 自带成员
class Token
{
	/*内容自行填写*/
public:
	TokenTag Tag;
	string Val;
	int FindIndex;
public:
	Token(const TokenTag& tTag,const string& tVal,const int tFindIndex) :Tag(tTag), Val(tVal), FindIndex(tFindIndex)
	{

	}
};



</TokenClass>

<ClassMember>
//这里的内容会直接拷贝到LexParse 类里面做私有成员
//vector<shared_ptr<Token>>TokenStream;

//动作表
//执行动作的函数签名
//函数签名是定死的,但是想扩展可以在类的私有成员上做手脚=.=以及token的扩展内容上做变化
//shared_ptr<Token>(int& FindIndex, string& CatchContent,string& SrcStr)
//
</ClassMember>

<TypeDefine>
//"模式别名" : "模式";
letter : "[A-Za-z]";
delim : "[\t\n]";
ws     : "{delim}+";
digit  : "[0-9]";
id     : "{letter}({letter}|{digit})*";
number : "{digit}+";
include: "#include";
headfile: "<{id}.h>";
add: "\+";
</TypeDefine>

<Action>
// Continue是预定义的,当语义动作为{Contimue};说明不捕获,直接跳过该模式匹配内容
//"模式" : {语义动作};
"\"{id}.h\"" : {return shared_ptr<Token>(new Token(TokenTag::HeadFile, CatchContent, FindIndex)); };
//模式别名 : {语义动作};
headfile: {return shared_ptr<Token>(new Token(TokenTag::HeadFile, CatchContent, FindIndex)); };
id:{return shared_ptr<Token>(new Token(TokenTag::Id, CatchContent, FindIndex)); };
ws:{Continue};
number: {return shared_ptr<Token>(new Token(TokenTag::Number, CatchContent, FindIndex)); };
include: {return shared_ptr<Token>(new Token(TokenTag::Include, CatchContent, FindIndex)); };
add: {return shared_ptr<Token>(new Token(TokenTag::Add, CatchContent, FindIndex)); };
</Action>
<UserDefineFunction>
//用户自定义函数位置
</UserDefineFunction>