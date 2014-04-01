<TokenDefine>
  /*这里是写入非终结符号在词法解析时候的Tag表示*/
  //禁止LexTag和非终结符号重名
  //形式 "LexTag" : "终结符号" ;
  "OtherChar " : "OtherChar";
  "NumberChar" : "NumberChar";
  "RealWordChar" : "RealWordChar";
  "SimpleUnNamedCatch_Start" : "(";
  "StringHead" : "^";
  "StringTail" : "$";
  "Mitipute_End" : ")";
  "Closures_Greedy" : "Closures_Greedy";
  "PositiveClosures_Greedy" : "PositiveClosures_Greedy";
  "ChoseClosures_Greedy" : "ChoseClosures_Greedy";
  "Closures_UnGreedy" : "Closures_UnGreedy";
  "PositiveClosures_UnGreedy" : "PositiveClosures_UnGreedy";
  "ChoseClosures_UnGreedy" : "ChoseClosures_UnGreedy";
  "Repeat_Start" : "{";
  "Repeat_And_BackRefer_End" : "}";
  "Repeat_End_Greedy" : "}?";
  "CharSet_Start" : "[";
  "CharSet_End" : "]";
  "CharSet_Back_Start" : "[^";
  "ChoseSymbol" : "|";
  "CharSetComponent" : "-";
  "Comma" : ",";
</TokenDefine>
//开始产生式不是随便选择的,Express可以生成最多的产生式头部
//第一个产生式是产生式开始.
<FirstGrammar>
  Start => Express;
</FirstGrammar>
//语义动作的函数签名是 void Func(int Index,int Postion,int StreamIndex,vector<shared_ptr<RegexToken>>& TokenStream)
<Grammar>
  CompleteCharSet      => CharSet 								{OneLeftNodeSetup(ParseTag::CompleteCharSet);}
                          | CharSet Repeat  					{TwoNodeSetup(ParseTag::CompleteCharSet);}
                          ;  
  Factor               => "(" Express ")" 						 {
  																	$$.Val = "";
																	$$.Tag = ParseTag::Factor;
																	$$.LeftNode = $2;
																	$$.ChildNumber = 1;
  																 }
                          | CompleteCharSet  					{OneLeftNodeSetup(ParseTag::Factor);}
                          | NormalStringComplete				{TwoNodeSetup(ParseTag::Factor);}
                          ;
  CompleteFactor       => Factor Repeat 						{TwoNodeSetup(ParseTag::CompleteFactor);}
                          | Factor  							{OneLeftNodeSetup(ParseTag::CompleteFactor);}
                          ;
  Term                 =>  CompleteFactor Term					{TwoNodeSetup(ParseTag::Term);}
                          | CompleteFactor						{OneLeftNodeSetup(ParseTag::Term);}
                          ;
  Express              =>Term  "|" Express						{TwoNodeSetup(ParseTag::ChoseSymbol);}
                          | Term								{
																	OneLeftNodeSetup(ParseTag::Express);
																}
                          ;
  Repeat               => "{" RepeatRight RepeatEnd				{
																	$$.Opps_Greedy = $1.Opps_Greedy;
																	$$.Tag = ParseTag::Repeat;
																	$$.LeftNode = $2;
																	$$.ChildNumber = 1;
																}
                          | "Closures_Greedy"					{
																	$$.Opps_Greedy = true;
																	$$.Tag = ParseTag::Repeat;
																	$$.Val = "*";
																	$$.ChildNumber = 0;
																}
                          | "Closures_UnGreedy"					{
																	$$.Opps_Greedy = false;
																	$$.Tag = ParseTag::Repeat;
																	$$.Val = "*";
																	$$.ChildNumber = 0;
																} 
                          | "PositiveClosures_UnGreedy" 		{
																	$$.Opps_Greedy = false;
																	$$.Tag = ParseTag::Repeat;
																	$$.Val = "+";
																	$$.ChildNumber = 0;
																} 
                          | "PositiveClosures_Greedy" 			{
																	$$.Opps_Greedy = true;
																	$$.Tag = ParseTag::Repeat;
																	$$.Val = "+";
																	$$.ChildNumber = 0;
																} 
                          | "ChoseClosures_UnGreedy"  			{
																	$$.Opps_Greedy = false;
																	$$.Tag = ParseTag::Repeat;
																	$$.Val = "?";
																	$$.ChildNumber = 0;
																} 
                          | "ChoseClosures_Greedy"  				{
																	$$.Opps_Greedy = true;
																	$$.Tag = ParseTag::Repeat;
																	$$.Val = "?";
																	$$.ChildNumber = 0;
																} 
                          ;
  RepeatRight          => SumNumber								{OneLeftNodeSetup(ParseTag::RepeatRight);}
                          | SumNumber "," SumNumber				{
																  $$.Val = "";
																  $$.Tag = ParseTag::RepeatRight;
																  $$.LeftNode = $3;
																  $$.RightNode = $1;
																  $$.ChildNumber = 2;
																}
                          ;
  RepeatEnd			   => "}"									{GetValue(ParseTag::Repeat_And_BackRefer_End,"}" ); }
						  | "}?"								{GetValue(ParseTag::Repeat_End_Greedy,"}?");}
                          ;
						  //这里直接把子节点丢掉,只要一个SumNumber
  SumNumber			   => NumberChar SumNumber					{
																	$$.Val = $2.Val + $1.Val;
																	$$.Tag = ParseTag::SumNumber;
																	$$.ChildNumber = 0;
																 }
                          ;
  NormalStringComplete => NormalString Repeat					{
																	TwoNodeSetup(ParseTag::NormalStringComplete);
																}
                          | NormalString						{
																   OneLeftNodeSetup(ParseTag::NormalStringComplete);
																}
                          ;
  NormalString         => NormalChar NormalString				{
																	TwoNodeSetup(ParseTag::NormalString);
																}
                          | NormalChar							{
																	OneLeftNodeSetup(ParseTag::NormalString,$1.Val);
																}
                          ;
  NormalChar           => "NumberChar"							{
																	GetValue(ParseTag::NormalChar,$1.Val);
																}
					       | "RealWordChar"						{
																	GetValue(ParseTag::NormalChar,$1.Val);
																}
						   | "OtherChar"						{
																	GetValue(ParseTag::NormalChar,$1.Val);
																 }				
						   
                           ;
  CharSet              => "[" CharSetString "]"					{
																    
																	$$.Opps_Greedy = false;
																	$$.Tag = ParseTag::CharSet;
																	$$.LeftNode = $1;
																	$$.ChildNumber = 1;
																}
                           | "[^" CharSetString "]"             {
																	
																	$$.Opps_Greedy = true;
																	$$.Tag = ParseTag::CharSet;
																	$$.LeftNode = $1;
																	$$.ChildNumber = 1;
																}
                           ;

  CharSetString		   => CharSetUnit CharSetString             {
																	TwoNodeSetup(ParseTag::CharSetString);
																}
						   ;
  CharSetUnit		   => NormalChar "-" NormalChar				{TwoNodeSetup(ParseTag::CharSetUnit);}
						  | NormalChar							{GetValue(ParseTag::CharSetUnit,$1.Val);}
						  ;
 /* CharSetString        => NormalString "-" CharSetString		{
																	$$.Val = "-";
																	$$.Tag = ParseTag::CharSetString;
																	$$.LeftNode = $3;
																	$$.RightNode = $1;
																	$$.ChildNumber = 2;
																}
                           | NormalString CharSetString			{
																	TwoNodeSetup(ParseTag::CharSetString);
																}
                           | NormalString						{
																	OneLeftNodeSetup(ParseTag::CharSetString);
																}
                           ;*/
</Grammar>
//在ParseClass外部的全局区域放置的内容
<Global>
////这里可以是NFA 结构
//class ASTNode
//{
//  //int Index;
// // bool Nullable;
//  //set<int> FirstPos;
// // set<int> LastPos;
//  //set<int> FollowPos;
//	string Data;
//	ParseTag Tag;
//  int ChildNumber;
//  int Left;
//  int Right;
//  ASTNode(string& tTarget, ParseTag tTag) :Data(tTarget), Tag(tTag)
//  {
//	
//  }
//  ASTNode(string& tTarget, ParseTag tTag, int LeftIndex, int RightIndex) :Data(tTarget), Left(LeftIndex), Right(RightIndex), Tag(tTag)
//  {
//	
//  }
//};
</Global>
<DataMember>
  //保存NFA的列表和映射表
  vector<ASTNode> NodeList;

</DataMember>
<UserDefineFunc>
void fun()
{

}
void OneLeftNodeSetup(ParseTag Tag,string Value = "")
{
	$$.Val = Value;
	$$.Tag = ParseTag::Tag;
	$$.LeftNode = $1;
	$$.ChildNumber = 1;
}
void GetValue(ParseTag Tag, string Value)
{
	$$.Val = Value;
	$$.Tag = Tag;
	$$.ChildNumber = 0;
}
void TwoNodeSetup(ParseTag Tag, string Value = "")
{
	$$.Val = Value;
	$$.Tag = Tag;
	$$.LeftNode = $2;
	$$.RightNode = $1;
	$$.ChildNumber = 2;
}
</UserDefineFunc>

//状态的综合属性class成员
// 这里Val是定死的,保存的是词素,Tag也是定死的,保存的是非终结符名和终结符号名 ParseTag是强类型枚举
<PropertyMember>
//string Val;
// ParseTag Tag;

//在CharSet中是否取反查看该位
int a;
bool Opps_Greedy;

//通用属性
int RightNode;
int LeftNode;
int ChildNumber;
</PropertyMember>