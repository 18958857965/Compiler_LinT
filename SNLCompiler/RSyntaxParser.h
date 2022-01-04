/**
	�ݹ��½��﷨������
*/
#pragma once

#include <vector>
#include <map>
#include "SemanticParser.h"
#include "SyntaxParserCommon.h"
#include "LexicalAnalyzer.h"

using namespace std;
typedef enum { ProK, PheadK, DecK, TypeK, VarK, ProcDecK, StmLK, StmtK, ExpK }
NodeKind;
enum  DecKind { ArrayK, CharK, IntegerK, RecordK, IdK } ;
typedef enum { IfK, WhileK, AssignK, ReadK, WriteK, CallK, ReturnK } StmtKind;
typedef enum { OpK, ConstK, VariK } ExpKind;
enum ExpType  { Void, Integer, Boolean } ;
union Kind {//��¼�﷨���ڵ�ľ������ͣ�Ϊ������ṹ��

	DecKind dec; /*��¼�﷨���ڵ���������ͣ��� nodekind = DecK ʱ��Ч��
				ȡ ֵ ArrayK, CharK, IntegerK, RecordK, IdK��Ϊ�﷨���ڵ��������͡�*/
	StmtKind stmt;/*��¼�﷨���ڵ��������ͣ��� nodekind = StmtK ʱ��Ч��
					 ȡֵ IfK,WhileK,AssignK,ReadK,WriteK,CallK,ReturnK��Ϊ�� �����ڵ�������͡� */
	ExpKind  exp;/*��¼�﷨���ڵ�ı��ʽ���ͣ��� nodekind=ExpK ʱ��Ч,
				ȡֵ OpK,ConstK,VariK(������idk)��Ϊ�﷨���ڵ���ʽ����*/
};
struct ArrayAttr// ��¼�������͵����ԡ� 
{
	int  low;//�������ͱ�������¼������½硣 
	int up;//�������ͱ�������¼������Ͻ硣 
	DecKind childType;//��¼����ĳ�Ա���͡�
};
typedef enum {
	valparamtype, varparamtype
} Paramt;
struct ProAttr {
	Paramt paramt;
};
typedef enum {
	IdV, ArrayMembV, FieldMembV
} Varkind;
struct ExpAttr
{
	LexType op;
	//string op;
	CString val = _T("");
	//string val = "";
	Varkind varkind;
	ExpType type;         /* �������ͼ��  */
	//Type type;
};

struct Attr {//��¼�﷨���ڵ���������,Ϊ�ṹ�����͡� 
	ArrayAttr*arrayattr = new ArrayAttr;
	ProAttr*proattr = new ProAttr;
	ExpAttr*expattr = new ExpAttr;
	CString type_name = _T("");//������
	
};

struct  symbtable;
struct TreeNode {
	TreeNode*child[3] = { NULL };// ָ�����﷨���ڵ�ָ�룬Ϊ�﷨���ڵ�ָ�����͡� 
	TreeNode*sibling = NULL;// ָ���ֵ��﷨���ڵ�ָ�룬Ϊ�﷨���ڵ�ָ�����͡� 
	int  lineno = 0;//��¼Դ�����кţ�Ϊ�������͡� 
	NodeKind  nodekind;//��¼�﷨���ڵ����ͣ�
							  //ȡֵ ProK, PheadK, TypeK, VarK, ProcDecK, StmLK, DecK, StmtK, ExpK,Ϊ�﷨���ڵ����͡� 
	Kind *kind = new Kind();

	int idnum = 0;/*��¼һ���ڵ��еı�־���ĸ���.*/
	CString name[100] = { _T("") };
	//string *name = new string[100];/*�ַ������飬�����Ա�ǽڵ��еı�־�������֡�*/
	struct symbtable * table[100];//ָ�����飬�����Ա�ǽڵ��еĸ�����־���ڷ��ű��е���ڡ�
	string type_name = "";//��¼�����������ڵ�Ϊ�������ͣ��������������ͱ�־����ʾʱ��Ч��
	Attr*attr = new Attr;
};




class RSyntaxParser
{
public:
	RSyntaxParser();
	RSyntaxParser(vector<Token> tokens);
	~RSyntaxParser();
	/* ��̬����indentno�ں���printTree��  ���ڴ洢��ǰ������������,��ʼΪ0 */
	int indentno = 0;
	int Errorflag = 1;//1  right              0  false
	bool Error = true;
	//vector<RTreeNode*> mChildNodes;
	vector<Token> mTokenList;
	vector<ParseLog> mParseLog;
	//map<NodeType, CString> mNodeType2Str;
	CString temp_name = _T("");
	//string temp_name = "";
	int mTokenPtr;
	int mCurLine;
	LexicalAnalyzer mLexicalAnalyzer;

	TreeNode* mSyntaxTree;

	void NextToken();
	Token GetCurToken();
	bool Match(LexType type);

	//void InitMap();

	TreeNode* Parse();
	TreeNode* Program();
	TreeNode* ProgramHead();
	TreeNode* DeclarePart();
	TreeNode* ProcDec();
	TreeNode* ProcDeclaration();
	//TreeNode* ProcDecMore();
	TreeNode* ProcDecPart();
	TreeNode* ProcBody();
	void ParamList(TreeNode* t);
	TreeNode* ParamDecList();
	TreeNode* ParamMore();
	TreeNode* Param();
	void FormList(TreeNode* t);
	void FidMore(TreeNode* t);
	TreeNode* VarDec();
	TreeNode* VarDeclaration();
	TreeNode* VarDecList();
	void VarIdMore(TreeNode* t);
	void VarIdList(TreeNode* t);
	TreeNode* TypeDecMore();
	TreeNode* StmList();
	TreeNode* StmMore();
	TreeNode* Stm();
	TreeNode* AssCall();
	TreeNode* AssignmentRest();
	TreeNode* ConditionalStm();
	TreeNode* LoopStm();
	TreeNode* InputStm();
	TreeNode* OutputStm();
	TreeNode* ReturnStm();
	TreeNode* CallStmRest();
	TreeNode* ActParamList();
	TreeNode* ActParamMore();
	//TreeNode* RelExp();
	//TreeNode* OtherRelE();
	TreeNode* Exp();
	//TreeNode* OtherTerm();
	TreeNode* Term();
	TreeNode* Factor();
	//TreeNode* OtherFactor();
	TreeNode* Variable();
	void VariMore(TreeNode* t);
	TreeNode* FieldVar();
	void FieldVarMore(TreeNode* t);
	//TreeNode* CmpOp();
	//TreeNode* AddOp();
	//TreeNode* MultOp();
	TreeNode* ProgramBody();
	//TreeNode* ProgramName();
	TreeNode* VarDecMore();
	TreeNode* TypeDec();
	TreeNode* TypeDeclaration();
	TreeNode* TypeDecList();
	void TypeId(TreeNode* t);
	
	void TypeDef(TreeNode* t);
	void BaseType(TreeNode* t);
	void StructureType(TreeNode* t);
	void ArrayType(TreeNode* t);
	void RecType(TreeNode* t);
	TreeNode* FieldDecList();
	TreeNode* FieldDecMore();
	void IdList(TreeNode* t);
	void IdMore(TreeNode* t);
	

	//TreeNode* GetMatchedTerminal(Token t);

	CString GetSyntaxTreeStr(CString lftstr, CString append, TreeNode* r);
	TreeNode* newDecNode1(NodeKind kind);
	TreeNode* newDecNode2();
	TreeNode* newProcNode();
	TreeNode* newStmlNode();
	TreeNode* newStmtNode(StmtKind kind);
	TreeNode* newExpNode(ExpKind kind);
	TreeNode* Simple_exp();


	CString Treemessage = _T("");
	void printTree(TreeNode* t);
	void printTab(int tabnum);
	void printSpaces();
	void ReleaseTree(TreeNode* r);
};

