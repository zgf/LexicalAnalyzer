/*
CodeGen�����ؼ��� _Continue;
_Continue��˼��������ǰ������ַ�(�����հ׵�)
*/

//<HeadFile>
//�����ֱ�ӿ�����LexParse�ļ���
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



// LexTag��enum class����

//<LexTag>



//</LexTag>

// <TokenClass>
// ��Ҫtoken����������Ա���������
//Tken���� �Դ���Ա
//  LexTag Tag;
string Val;
int FindIndex;


// <TokenClass>

//<ClassMember>
//��������ݻ�ֱ�ӿ�����LexParse ��������˽�г�Ա


//�кŴ�1��ʼ
int Line;
//�кŴ�1��ʼ
int ColsIndex;
//������
//ִ�ж����ĺ���ǩ��
//����ǩ���Ƕ�����,��������չ���������˽�г�Ա�����ֽ�=.=�Լ�token����չ���������仯
//shared_ptr<Token>(int& ReStartIndex, wstring& CatchContent)
//unordered_map<wstring, function<shared_ptr<Token>(int& ReStartIndex, string& CatchContent)>>ActionMap;
//Դ��
string SrcStr;
//</ClassMember>

//<RegularTypeDefine>
//"ģʽ����" : "ģʽ";

//</RegularTypeDefine>

//<Action>
//"ģʽ" : {���嶯��};
//�Ƽ�ֱ��ʹ��LexTag����������
//ģʽ���� : {���嶯��};
//</Action>
//<UserDefineFunction>
//�û��Զ��庯��λ��
//</UserDefineFunction>