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
//���嶯���ĺ���ǩ���� void Func(vector<shared_ptr<Property>>& CatchStack,int StreamIndex,vector<shared_ptr<RegexToken>>& TokenStream))
<Grammar>
  CompleteCharSet      => CharSet 								{OneLeftNodeSetup(ParseTag::CompleteCharSet,CatchStack);}
                          | CharSet Repeat  					{TwoNodeSetup(ParseTag::CompleteCharSet,CatchStack);}
                          ;  
  Factor               => "(" Express ")" 						 {
  																	$$.Val = "";
																	$$.Tag = ParseTag::Factor;
																	$$.LeftNode = $2;
																	$$.ChildNumber = 1;
  																 }
                          | CompleteCharSet  					{OneLeftNodeSetup(ParseTag::Factor,CatchStack);}
                          | NormalStringComplete				{TwoNodeSetup(ParseTag::Factor,CatchStack);}
                          ;
  CompleteFactor       => Factor Repeat 						{TwoNodeSetup(ParseTag::CompleteFactor,CatchStack);}
                          | Factor  							{OneLeftNodeSetup(ParseTag::CompleteFactor,CatchStack);}
                          ;
  Term                 =>  CompleteFactor Term					{TwoNodeSetup(ParseTag::Term,CatchStack);}
                          | CompleteFactor						{OneLeftNodeSetup(ParseTag::Term,CatchStack);}
                          ;
  Express              =>Term  "|" Express						{TwoNodeSetup(ParseTag::ChoseSymbol,CatchStack);}
                          | Term								{
																	OneLeftNodeSetup(ParseTag::Express,CatchStack);
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
  RepeatRight          => SumNumber								{OneLeftNodeSetup(ParseTag::RepeatRight,CatchStack);}
                          | SumNumber "," SumNumber				{
																  $$.Val = "";
																  $$.Tag = ParseTag::RepeatRight;
																  $$.LeftNode = $3;
																  $$.RightNode = $1;
																  $$.ChildNumber = 2;
																}
                          ;
  RepeatEnd			   => "}"									{GetValue(ParseTag::Repeat_And_BackRefer_End,"}",CatchStack ); }
						  | "}?"								{GetValue(ParseTag::Repeat_End_Greedy,"}?",CatchStack);}
                          ;
						  //����ֱ�Ӱ��ӽڵ㶪��,ֻҪһ��SumNumber
  SumNumber			   => NumberChar SumNumber					{
																	$$.Val = $2.Val + $1.Val;
																	$$.Tag = ParseTag::SumNumber;
																	$$.ChildNumber = 0;
																 }
                          ;
  NormalStringComplete => NormalString Repeat					{
																	TwoNodeSetup(ParseTag::NormalStringComplete,CatchStack);
																}
                          | NormalString						{
																   OneLeftNodeSetup(ParseTag::NormalStringComplete,CatchStack);
																}
                          ;
  NormalString         => NormalChar NormalString				{
																	TwoNodeSetup(ParseTag::NormalString,CatchStack);
																}
                          | NormalChar							{
																	OneLeftNodeSetup(ParseTag::NormalString,CatchStack,$1.Val);
																}
                          ;
  NormalChar           => "NumberChar"							{
																	GetValue(ParseTag::NormalChar,CatchStack,$1.Val);
																}
					       | "RealWordChar"						{
																	GetValue(ParseTag::NormalChar,CatchStack,$1.Val);
																}
						   | "OtherChar"						{
																	GetValue(ParseTag::NormalChar,CatchStack,$1.Val);
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
																	TwoNodeSetup(ParseTag::CharSetString,CatchStack);
																}
						   ;
  CharSetUnit		   => NormalChar "-" NormalChar				{TwoNodeSetup(ParseTag::CharSetUnit,CatchStack);}
						  | NormalChar							{GetValue(ParseTag::CharSetUnit,CatchStack,$1.Val);}
						  ;

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

void OneLeftNodeSetup(ParseTag Tag, vector<shared_ptr<Property>>&CatchStack, string Value = "")
{
	$$.Val = Value;
	$$.Tag = ParseTag::Tag;
	$$.LeftNode = $1;
	$$.ChildNumber = 1;
}
void GetValue(ParseTag Tag, string Value, vector<shared_ptr<Property>>&CatchStack)
{
	$$.Val = Value;
	$$.Tag = Tag;
	$$.ChildNumber = 0;
}
void TwoNodeSetup(ParseTag Tag, vector<shared_ptr<Property>>&CatchStack, string Value = "")
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


bool Opps_Greedy;

//ͨ������
int RightNode;
int LeftNode;
int ChildNumber;
</PropertyMember>