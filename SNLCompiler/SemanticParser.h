#pragma once
#include <vector>
#include <map>
//#include"RSyntaxParser.h"
#include "SyntaxParserCommon.h"

#include "LexicalAnalyzer.h"
typedef  enum { typeKind, varKind, procKind }IdKind;
/*���������dir��ֱ�ӱ���(ֵ��)��indir��ʾ��ӱ���(���)*/
typedef  enum { dir, indir }AccessKind;
/*�βα�Ľṹ����*/
typedef struct  paramTable
{
	struct symbtable   *  entry;/*ָ����β����ڷ��ű��еĵ�ַ���*/
	struct paramTable  *  next;
}ParamTable;

struct typeIR;

typedef struct
{
	struct typeIR  * idtype; /*ָ���ʶ���������ڲ���ʾ*/
	IdKind    kind; /*��ʶ��������*/
	union
	{
		struct
		{
			AccessKind   access;   /*�ж��Ǳ�λ���ֵ��*/
			int          level;
			int          off;
			bool         isParam;  /*�ж��ǲ���������ͨ����*/
		}VarAttr;/*������ʶ��������*/
		struct
		{
			int         level;     /*�ù��̵Ĳ���*/
			ParamTable  * param;   /*������*/
			int         mOff;    /*���̻��¼�Ĵ�С*/

			int         nOff;      /*sp��display���ƫ����*/
			int         procEntry; /*���̵���ڵ�ַ*/
			int         codeEntry;/*������ڱ��,�����м��������*/

		}ProcAttr;/*��������ʶ��������*/
	}More;/*��ʶ���Ĳ�ͬ�����в�ͬ������*/
}AttributeIR;

/*��ʶ�������Խṹ����*/
typedef  enum { intTy, charTy, arrayTy, recordTy, boolTy }TypeKind;
//struct typeIR;
/*�����͵�Ԫ�ṹ����*/
typedef struct fieldchain
{
	string   id;              /*������*/
	int    off;                 /*���ڼ�¼�е�ƫ��*/
	struct typeIR  *  UnitType; /*���г�Ա������*/
	struct fieldchain  * Next;
}fieldChain;
/*���͵��ڲ��ṹ����*/
typedef   struct  typeIR
{
	int size;   /*������ռ�ռ��С*/
	TypeKind kind;
	union
	{
		struct
		{
			struct typeIR * indexTy;
			struct typeIR * elemTy;
			int    low;     /*��¼�������͵��½�*/
			int    up;      /*��¼�������͵��Ͻ�*/
		}ArrayAttr;
		fieldChain * body;  /*��¼�����е�����*/
	} More;
}TypeIR;

/*���ű�Ľṹ����*/
typedef struct  symbtable
{
	string  idName;
	AttributeIR  attrIR;
	struct symbtable  *  next;
}SymbTable;
struct TreeNode;
enum DecKind;
class RSyntaxParser;
class SemanticParser {
public:
	//SemanticParser();
	//~SemanticParser();
	int Error;//����׷�ٱ�־,
	CString SMessage=_T("");
	RSyntaxParser *syntaxparser=nullptr;
	vector<ParseLog> mParseLog;
	/*scopeջ�Ĳ���*/
	int Level;
	/*��ͬ��ı���ƫ��*/
	int Off;
	/*������display���ƫ��*/
	int StoreNoff;
	/*�����displayOff*/
	int  savedOff;
	SymbTable * scope[1000];   /*scopeջ*/
	TypeIR * intPtr; /*��ָ��һֱָ���������͵��ڲ���ʾ*/
	TypeIR * charPtr; /*��ָ��һֱָ���ַ����͵��ڲ���ʾ*/
	TypeIR * boolPtr; /*��ָ��һֱָ�򲼶����͵��ڲ���ʾ*/
	int Analyze(TreeNode * t);


	//////////
	//���ű�ʵ��
	////////////
	SymbTable * NewTable(void);
	void CreatTable(void);
	void DestroyTable(void);
	//�ǼǱ�ʶ��������
	int Enter(string id, AttributeIR * attribP, SymbTable ** entry);

	//Ѱ�ұ����ַ
	int FindEntry(string id, SymbTable ** entry);

	//���Բ�ѯ
	AttributeIR FindAttr(SymbTable * entry);

	//�Ƿ�����
	int Compat(TypeIR * tp1, TypeIR * tp2);

	//�����������ڲ���ʾ
	TypeIR * NewTy(TypeKind kind);

	//������ǰ�ռ�¼�������������
	fieldChain * NewBody(void);

	//������ǰ���β�����
	ParamTable * NewParam(void);

	//������ʾ
	void ErrorPrompt(int line, string name, string message);

	void printTab(int tabnum);
	
	//�Ƿ��ҵ���¼������
	bool  FindField(string Id, fieldChain  *head, fieldChain  **Entry);

	//��¼�������
	void   PrintFieldChain(fieldChain  *currentP);

	//���ű��һ��
	void  PrintOneLayer(int level);

	// ��ӡ���ɵķ��ű�
	void   PrintSymbTable();

	
	///////////
	//��������
	//////////


	//��ʼ�� int char bool
	void initialize(void);

	//���ͷ���
	TypeIR  * TypeProcess(TreeNode * t, DecKind deckind);

	//�ڷ��ű���Ѱ���Ѷ������������
	TypeIR * nameType(TreeNode * t);

	//�����������͵��ڲ���ʾ
	TypeIR  * arrayType(TreeNode * t);

	//�����¼���͵��ڲ���ʾ 
	TypeIR * recordType(TreeNode * t);
	
	//������������
	void TypeDecPart(TreeNode * t);
	
	//�����������
	void VarDecPart(TreeNode * t);
	
	
	//void varDecList(TreeNode * t);
	
	//�����������
	void  procDecPart(TreeNode * t);
	
	//������ͷ
	SymbTable  *  HeadProcess(TreeNode * t);
	
	//������ͷ��������
	ParamTable  * ParaDecList(TreeNode * t);
	
	//����ִ����
	void Body(TreeNode * t);
	
	//�������״̬ 
	void statement(TreeNode * t);
	
	//������ʽ
	TypeIR * Expr(TreeNode * t, AccessKind  *  Ekind);
	
	//��������������±�
	TypeIR * arrayVar(TreeNode * t);
	
	//�����¼����
	TypeIR * recordVar(TreeNode * t);
	
	//����ֵ���
	void assignstatement(TreeNode * t);
	
	//�������������
	void callstatement(TreeNode * t);
	
	//�����������
	void ifstatment(TreeNode * t);
	
	//����ѭ�����
	void whilestatement(TreeNode * t);
	
	//�����������
	void readstatement(TreeNode * t);
	
	//����������
	void writestatement(TreeNode * t);
	
	//�������������
	void returnstatement(TreeNode * t);
};
