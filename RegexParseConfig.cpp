<TokenDefine>
  /*������д����ս�����ڴʷ�����ʱ���Tag��ʾ*/
  //��ֹLexTag�ͷ��ս��������
  //��ʽ "LexTag" : "�ս����" ;
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
//��ʼ����ʽ�������ѡ���,Express�����������Ĳ���ʽͷ��
//��һ������ʽ�ǲ���ʽ��ʼ.
<FirstGrammar>
  Start => Express;
</FirstGrammar>
//���嶯���ĺ���ǩ���� void Func(int Index,int Postion,int StreamIndex,vector<shared_ptr<RegexToken>>& TokenStream)
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
						  //����ֱ�Ӱ��ӽڵ㶪��,ֻҪһ��SumNumber
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
//��ParseClass�ⲿ��ȫ��������õ�����
<Global>
////���������NFA �ṹ
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
  //����NFA���б��ӳ���
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

//״̬���ۺ�����class��Ա
// ����Val�Ƕ�����,������Ǵ���,TagҲ�Ƕ�����,������Ƿ��ս�������ս������ ParseTag��ǿ����ö��
<PropertyMember>
//string Val;
// ParseTag Tag;

//��CharSet���Ƿ�ȡ���鿴��λ
int a;
bool Opps_Greedy;

//ͨ������
int RightNode;
int LeftNode;
int ChildNumber;
</PropertyMember>