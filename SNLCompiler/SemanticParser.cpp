#include "pch.h"
#include "SemanticParser.h"
#include "LexicalAnalyzer.h"
#include "RSyntaxParser.h"





int SemanticParser::Analyze(TreeNode * t)
{
	
	/*scopeջ�Ĳ���*/
	Level = -1;
	intPtr = NULL; /*��ָ��һֱָ���������͵��ڲ���ʾ*/
	charPtr = NULL; /*��ָ��һֱָ���ַ����͵��ڲ���ʾ*/
	boolPtr = NULL; /*��ָ��һֱָ�򲼶����͵��ڲ���ʾ*/

	Error = false;
	
	SymbTable * entry = NULL;
	TreeNode * p = NULL;
	TreeNode * pp = t;
	/*�������ű�*/
	CreatTable();
	/*���������ڲ���ʾ��ʼ������*/
	initialize();
	/*�﷨���������ڵ�*/
	p = t->child[1];
	while (p != NULL)
	{
		switch (p->nodekind)
		{
		case  TypeK:     TypeDecPart(p->child[0]);  break;
		case  VarK:     VarDecPart(p->child[0]);   break;
		case  ProcDecK:  procDecPart(p);        break;
		default:
			ErrorPrompt(p->lineno, "", "no this node kind in syntax tree!\r\n");
			break;
		}
		p = p->sibling;/*ѭ������*/
	}
	/*������*/
	t = t->child[2];
	if (t->nodekind == StmLK)
		Body(t);
	/*�������ű�*/
	if (Level != -1)
		DestroyTable();
	return StoreNoff;
}

SymbTable * SemanticParser::NewTable(void)
{
	
	SymbTable * table =new SymbTable;
	
	if (table == NULL)
	{
		SMessage += _T("Out of memory error !\r\n");
		LogUtil::Error(_T("Out of memory error !"));
		
		Error = TRUE;
	}
		
	
	table->next = NULL;
	table->attrIR.kind = typeKind;
	table->attrIR.idtype = NULL;
	table->next = NULL;
	table->attrIR.More.VarAttr.isParam = false;
	
	/* ���ű�����ָ��table����NULL,�ڴ浥Ԫ�Ѿ��ɹ����� */
	return table;
}

void SemanticParser::CreatTable(void)
{
	Level = Level + 1;                 /*������һ*/
	scope[Level] = NULL;           /*�������µ�һ��scopeջ�Ŀռ�*/
	Off = 7;           /*ƫ�Ƴ�ʼ��  INITOFF */
}

void SemanticParser::DestroyTable(void)
{
	/*�������������ٱ�־ΪTURE�����﷨���������ķ��ű���ʾ����*/
/*if ((TraceTable)&&(Error==FALSE))
{
printTable();
getchar();
}*/
	Level = Level - 1;
}

int SemanticParser::Enter(string id, AttributeIR * attribP, SymbTable ** entry)
{
	int present = FALSE;
	int result = 1;
	SymbTable * curentry = scope[Level];
	SymbTable * prentry = scope[Level];
	if (scope[Level] == NULL)
	{
		curentry = NewTable();
		scope[Level] = curentry;
	}
	else
	{
		while (curentry != NULL)
		{
			prentry = curentry;
			result = strcmp(id.c_str(), curentry->idName.c_str());
			if (result == 0)
			{
				SMessage += _T("repetition declaration error !\r\n");
				LogUtil::Error(_T("repetition declaration error !"));
				
				Error = TRUE;
				present = TRUE;
			}
			else
				curentry = (prentry->next);
		}   /*�ڸò���ű��ڼ���Ƿ����ظ��������*/

		if (present == FALSE)
		{
			curentry = NewTable();
			prentry->next = curentry;
		}
	}
	/*����ʶ���������ԵǼǵ�����*/
	curentry->idName=id;
	curentry->attrIR.idtype = attribP->idtype;
	curentry->attrIR.kind = attribP->kind;
	switch (attribP->kind)
	{
	case  typeKind: break;
	case  varKind:
		curentry->attrIR.More.VarAttr.level = attribP->More.VarAttr.level;
		curentry->attrIR.More.VarAttr.off = attribP->More.VarAttr.off;
		curentry->attrIR.More.VarAttr.access = attribP->More.VarAttr.access;
		break;
	case  procKind:
		curentry->attrIR.More.ProcAttr.level = attribP->More.ProcAttr.level;
		curentry->attrIR.More.ProcAttr.param = attribP->More.ProcAttr.param;
		break;
	default:break;
	}
	(*entry) = curentry;
	return present;
}

int SemanticParser::FindEntry(string id, SymbTable ** entry)
{
	int present = FALSE;    /*����ֵ*/
	int result = 1;         /*��ʶ�����ֱȽϽ��*/
	int lev = Level; /*��ʱ��¼�����ı���*/
	SymbTable *  findentry = scope[lev];
	while ((lev != -1) && (present != TRUE))
	{
		while ((findentry != NULL) && (present != TRUE))
		{
			result = strcmp(id.c_str(), findentry->idName.c_str());
			if (result == 0)
				present = TRUE;
			/*�����ʶ��������ͬ���򷵻�TRUE*/
			else
				findentry = findentry->next;
			/*���û�ҵ�������������еĲ���*/
		}
		if (present != TRUE)
		{
			lev = lev - 1;
			findentry = scope[lev];
		}
	}/*����ڱ�����û�в鵽����ת����һ���ֲ��������м�������*/
	if (present != TRUE)
	{
		(*entry) = NULL;
	}
	else
		(*entry) = findentry;
	return present;
}

AttributeIR SemanticParser::FindAttr(SymbTable * entry)
{
	AttributeIR attrIr = entry->attrIR;
	return attrIr;
}

int SemanticParser::Compat(TypeIR * tp1, TypeIR * tp2)
{
	int  present;
	if (tp1 != tp2)
		present = FALSE;  /*���Ͳ���*/
	else
		present = TRUE;   /*���͵ȼ�*/
	return present;
}

TypeIR * SemanticParser::NewTy(TypeKind kind)
{
	
	TypeIR * table = new TypeIR;
	/* �����ڲ���ʾ����ָ��tableΪNULL,
		   δ�ܳɹ������ڴ浥Ԫ����ʾ��ʾ������Ϣ*/
	if (table == NULL)
	{
		SMessage += _T("Out of memory error !\r\n");
		LogUtil::Error(_T("Out of memory error !"));
		
		Error = TRUE;
	}
	/* �����ڲ���ʾ����ָ��table����NULL,�ڴ浥Ԫ�Ѿ��ɹ����� */
	else
		switch (kind)
		{
		case intTy:
		case charTy:
		case boolTy:
			table->kind = kind;
			table->size = 1;
			break;
		case arrayTy:
			table->kind = arrayTy;
			table->More.ArrayAttr.indexTy = NULL;
			table->More.ArrayAttr.elemTy = NULL;
			break;
		case recordTy:
			table->kind = recordTy;
			table->More.body = NULL;
			break;
		}
	return table;
}

fieldChain * SemanticParser::NewBody(void)
{
	fieldChain * Ptr = (fieldChain *)malloc(sizeof(fieldChain));
	if (Ptr == NULL)
	{
		SMessage += _T("Out of memory error !\r\n");
		LogUtil::Error(_T("Out of memory error !"));

		Error = TRUE;
	}
	else
	{
		Ptr->Next = NULL;
		Ptr->off = 0;
		Ptr->UnitType = NULL;
	}
	return Ptr;
}

ParamTable * SemanticParser::NewParam(void)
{
	ParamTable * Ptr = (ParamTable *)malloc(sizeof(ParamTable));
	if (Ptr == NULL)
	{
		
		SMessage += _T("Out of memory error !\r\n");
		LogUtil::Error(_T("Out of memory error !"));

		Error = TRUE;
	}
	else
	{
		Ptr->entry = NULL;
		Ptr->next = NULL;
	}
	return Ptr;
}

void SemanticParser::ErrorPrompt(int line, string name, string message)
{
	CString temp = _T("");

	temp.Format(_T("-->Line: %d"), line);//"-->Line: %d, %s %s"                      , name.c_str(), message.c_str()
	SMessage += temp;
	SMessage += _T(" ");
	temp = CA2T(name.c_str());
	SMessage += temp;
	SMessage += _T(" ");
	temp = CA2T(message.c_str());
	SMessage += temp;
	//LogUtil::Error(_T(">>>Line: %d, %s %s", line, name.c_str(), message.c_str()));
	
	Error = TRUE;
}

void SemanticParser::printTab(int tabnum)
{
	for (int i = 0; i < tabnum; i++)
		SMessage+=_T(" ");
}

bool SemanticParser::FindField(string Id, fieldChain * head, fieldChain ** Entry)
{
	bool  present = false;
	/*��¼��ǰ�ڵ�*/
	fieldChain *currentItem = head;
	/*�ӱ�ͷ��ʼ���������ʶ����ֱ���ҵ��򵽴��β*/
	while ((currentItem != NULL) && (present == false))
	{
		if (strcmp(currentItem->id.c_str(), Id.c_str()) == 0)
		{
			present = true;
			if (Entry != NULL)
				(*Entry) = currentItem;
		}
		else  currentItem = currentItem->Next;
	}
	return(present);
}

void SemanticParser::PrintFieldChain(fieldChain * currentP)
{
	CString temp = _T("");
	                                  
	SMessage += _T("\r\n--------------Field  chain--------------------\r\n");
	fieldChain  *t = currentP;
	while (t != NULL)
	{ /*�����ʶ������*/

		//%s  t->id.c_str()
		temp = CA2T(t->id.c_str());
		SMessage += temp;
		SMessage+=_T(":  ");
		/*�����ʶ����������Ϣ*/
		switch (t->UnitType->kind)
		{
		case  intTy:  SMessage += _T("intTy     ");   break;
		case  charTy: SMessage += _T("charTy    ");  break;
		case  arrayTy: SMessage += _T("arrayTy   "); break;
		case  recordTy:SMessage += _T("recordTy  "); break;
		default: SMessage += _T("error  type!  "); break;
		}
		temp.Format(_T("off = %d\r\n"), t->off);
		SMessage += temp;
		t = t->Next;
	}


}

void SemanticParser::PrintOneLayer(int level)
{
	SymbTable  *t = scope[level];


	CString temp = _T("");
	                             
	SMessage += _T("         ���ű��");
	temp.Format(_T(" %d "), level+1);
	SMessage += temp;
	SMessage += _T("��:\r\n");
	                            
	
	SMessage += _T("---------------------------------------\r\n");
	//string str = "123123";

	while (t != NULL)
	{ /*�����ʶ������*/
		CString tmp;
		tmp = CA2T(t->idName.c_str());
		SMessage += tmp;
		SMessage += _T(":   ");
		
		AttributeIR  *Attrib = &(t->attrIR);
		/*�����ʶ����������Ϣ�����̱�ʶ������*/
		if (Attrib->idtype != NULL)  /*���̱�ʶ��*/
			switch (Attrib->idtype->kind)
			{
			case  intTy:  SMessage += _T("intTy  ");   break;
			case  charTy: SMessage += _T("charTy  ");  break;
			case  arrayTy: SMessage += _T("arrayTy  "); break;
			case  recordTy:SMessage += _T("recordTy  "); break;
			default: SMessage += _T("error  type!  "); break;
			}
		/*�����ʶ������𣬲����ݲ�ͬ���������ͬ��������*/
		switch (Attrib->kind)
		{
		case  typeKind:
			SMessage += _T("typekind  "); break;
		case  varKind:
			SMessage += _T("varkind  ");
			temp.Format(_T("Level = %d  "), Attrib->More.VarAttr.level);
			SMessage += temp;
			temp.Format(_T("Offset = %d  "), Attrib->More.VarAttr.off);
			SMessage += temp;

			switch (Attrib->More.VarAttr.access)
			{
			case  dir:  SMessage += _T("dir  "); break;
			case  indir: SMessage += _T("indir  "); break;
			default:SMessage += _T("errorkind  ");  break;
			}
			break;
		case  procKind:
			SMessage += _T("funckind   ");
			temp.Format(_T("Level= %d  "), Attrib->More.ProcAttr.level);
			SMessage += temp;
			temp.Format(_T("Noff= %d  "), Attrib->More.ProcAttr.nOff);
			SMessage += temp;
			break;
		default:SMessage += _T("error  ");
		}
		SMessage += _T("\r\n");
		t = t->next;
	}
	SMessage += _T("---------------------------------------\r\n\r\n");
}

void SemanticParser::PrintSymbTable()
{
	int  level = 0;
	while (scope[level] != NULL)
	{
		PrintOneLayer(level);
		level++;
	}
}

void SemanticParser::initialize(void)
{
	intPtr = NewTy(intTy);
	charPtr = NewTy(charTy);
	boolPtr = NewTy(boolTy);
	/*scopeջ�ĸ���ָ����Ϊ��*/
	for (int i = 0; i < 1000; i++)
		scope[i] = NULL;
}

TypeIR * SemanticParser::TypeProcess(TreeNode * t, DecKind deckind)
{
	TypeIR  *  Ptr = NULL;
	switch (deckind)
	{
	case IdK:
		Ptr = nameType(t); break;         /*����Ϊ�Զ����ʶ��*/
	case ArrayK:
		Ptr = arrayType(t); break;        /*����Ϊ��������*/
	case RecordK:
		Ptr = recordType(t); break;       /*����Ϊ��¼����*/
	case IntegerK:
		Ptr = intPtr; break;              /*����Ϊ��������*/
	case CharK:
		Ptr = charPtr; break;             /*����Ϊ�ַ�����*/
	}
	return Ptr;
}

TypeIR * SemanticParser::nameType(TreeNode * t)
{
	TypeIR * Ptr = NULL;
	SymbTable * entry = NULL;
	int present;
	/*���ͱ�ʶ��Ҳ��Ҫ��ǰ�����*/
	string id = CT2A(t->attr->type_name.GetString());
	present = FindEntry(id, &entry);
	if (present == TRUE)
	{
		/*���ñ�ʶ���Ƿ�Ϊ���ͱ�ʶ��*/
		if (entry->attrIR.kind != typeKind) {
			string name = CT2A(t->attr->type_name.GetString());
			ErrorPrompt(t->lineno, name, "used before typed!\r\n");
		}
		else
			Ptr = entry->attrIR.idtype;
	}
	else/*û���ҵ��ñ�ʶ��*/
	{
		string name = CT2A(t->attr->type_name);
		ErrorPrompt(t->lineno, name, "type name is not declared!\r\n");
	}
	return Ptr;
}

TypeIR * SemanticParser::arrayType(TreeNode * t)
{
	TypeIR  * Ptr0 = NULL;
	TypeIR  * Ptr1 = NULL;
	TypeIR  * Ptr = NULL;
	/*��������Ͻ��Ƿ�С���½�*/
	if ((t->attr->arrayattr->low) > (t->attr->arrayattr->up))
	{
		ErrorPrompt(t->lineno, "", "array subscript error!\r\n");
		Error = TRUE;
	}
	else
	{
		Ptr0 = TypeProcess(t, IntegerK);
		/*�������ͷ��������������±�����*/
		Ptr1 = TypeProcess(t, t->attr->arrayattr->childType);
		/*�������ͷ�������������Ԫ������*/
		Ptr = NewTy(arrayTy);
		/*ָ��һ�´�����������Ϣ��*/
		Ptr->size = ((t->attr->arrayattr->up) - (t->attr->arrayattr->low) + 1)*(Ptr1->size);
		/*���㱾���ͳ���*/
		/*��д������Ϣ*/
		Ptr->More.ArrayAttr.indexTy = Ptr0;
		Ptr->More.ArrayAttr.elemTy = Ptr1;
		Ptr->More.ArrayAttr.low = t->attr->arrayattr->low;
		Ptr->More.ArrayAttr.up = t->attr->arrayattr->up;
	}
	return Ptr;
}

TypeIR * SemanticParser::recordType(TreeNode * t)
{
	TypeIR  * Ptr = NewTy(recordTy);  /*�½���¼���͵Ľڵ�*/
	t = t->child[0];                /*���﷨���Ķ��ӽڵ��ȡ����Ϣ*/
	fieldChain  *  Ptr2 = NULL;
	fieldChain  *  Ptr1 = NULL;
	fieldChain  *  body = NULL;
	while (t != NULL) 
	{
		/*��дptr2ָ������ݽڵ�
		 ���int a,b;���� */
		for (int i = 0; i < t->idnum; i++)
		{
			
			Ptr2 = NewBody();
			if (body == NULL)
				body = Ptr1 = Ptr2;
			/*��дPtr2�ĸ�����Ա����*/
			Ptr2->id= CT2A(t->name[i].GetString());
			Ptr2->UnitType = TypeProcess(t, t->kind->dec);
			Ptr2->Next = NULL;

			
			if (Ptr2 != Ptr1)/*��ô��ָ�����*/
			{
				/*����������ĵ�Ԫoff*/
				Ptr2->off = (Ptr1->off) + (Ptr1->UnitType->size);
				Ptr1->Next = Ptr2;
				Ptr1 = Ptr2;
			}
		}
		//�����ֵܽڵ�
		t = t->sibling;
	}
	/*�����¼�����ڲ��ṹ*/
	/*ȡPtr2��offΪ���������¼��size*/
	Ptr->size = Ptr2->off + (Ptr2->UnitType->size);
	/*�����������¼���͵�body����*/
	Ptr->More.body = body;
	return Ptr;
}

void SemanticParser::TypeDecPart(TreeNode * t)
{
	int present = FALSE;
	AttributeIR  attrIr;
	SymbTable * entry = NULL;
	/*��������Ϊ����*/
	attrIr.kind = typeKind;
	/*�����﷨�����ֵܽڵ�*/
	while (t != NULL)
	{
		/*���ü�¼���Ժ����������Ƿ��ظ����������ڵ�ַ*/
		string id = CT2A(t->name[0].GetString());
		present = Enter(id, &attrIr, &entry);
		if (present != FALSE)
		{
			id = CT2A(t->name[0].GetString());
			ErrorPrompt(t->lineno, id, "is repetation declared!\r\n");
			entry = NULL;
		}
		else
			entry->attrIR.idtype = TypeProcess(t, t->kind->dec);
		t = t->sibling;
	}
}

void SemanticParser::VarDecPart(TreeNode * t)
{
	//varDecList(t);
	AttributeIR  attrIr;
	int present = FALSE;
	SymbTable * entry = NULL;
	while (t != NULL) /*ѭ������*/
	{
		attrIr.kind = varKind;
		for (int i = 0; i < (t->idnum); i++)
		{
			attrIr.idtype = TypeProcess(t, t->kind->dec);
			/*�ж�ʶֵ�λ��Ǳ��acess(dir,indir)*/
			if (t->attr->proattr->paramt == varparamtype)
			{
				attrIr.More.VarAttr.access = indir;
				attrIr.More.VarAttr.level = Level;
				/*�����βε�ƫ��*/
				attrIr.More.VarAttr.off = Off;
				Off = Off + 1;
			}/*����Ǳ�Σ���ƫ�Ƽ�1*/
			else
			{
				attrIr.More.VarAttr.access = dir;
				attrIr.More.VarAttr.level = Level;
				/*����ֵ�ε�ƫ��*/
				if (attrIr.idtype != NULL)
				{
					attrIr.More.VarAttr.off = Off;
					Off = Off + (attrIr.idtype->size);
				}
			}/*���������Ϊֵ�Σ�ƫ�Ƽӱ������͵�size*/
			/*�ǼǸñ��������Լ�����,�������������ڲ�ָ��*/
			string id = CT2A(t->name[i].GetString());
			present = Enter(id, &attrIr, &entry);
			if (present != FALSE)
			{
				id = CT2A(t->name[i].GetString());
				ErrorPrompt(t->lineno, id, " is defined repetation!\r\n");
			}
			else
				t->table[i] = entry;
		}
		if (t != NULL)
			t = t->sibling;
	}
	/*��������������¼��ʱƫ�ƣ�����Ŀ���������ʱ��displayOff*/
	if (Level == 0)
	{
		//mainOff = Off;
		/*�洢������AR��display���ƫ�Ƶ�ȫ�ֱ���*/
		StoreNoff = Off;
	}
	/*����������������¼��ʱƫ�ƣ�����������д������Ϣ���noff��Ϣ*/
	else
		savedOff = Off;
}



void SemanticParser::procDecPart(TreeNode * t)
{
	TreeNode * p = t;
	SymbTable * entry = HeadProcess(t);   /*�������ͷ*/
	t = t->child[1];
	/*��������ڲ������������֣�������������*/
	while (t != NULL)
	{
		switch (t->nodekind)
		{
		case  TypeK:     
			TypeDecPart(t->child[0]);  break;
		case  VarK:     
			VarDecPart(t->child[0]);   break;
			/*������������к���������������ѭ��������дnoff��moff����Ϣ��*
			 *�ٴ�����������ѭ�����������޷�����noff��moff��ֵ��      */
		case  ProcDecK:  break;
		default:
			ErrorPrompt(t->lineno, "", "no this node kind in syntax tree!");
			break;
		}
		if (t->nodekind == ProcDecK)
			break;
		else
			t = t->sibling;
	}
	entry->attrIR.More.ProcAttr.nOff = savedOff;
	entry->attrIR.More.ProcAttr.mOff = entry->attrIR.More.ProcAttr.nOff + entry->attrIR.More.ProcAttr.level + 1;
	/*���̻��¼�ĳ��ȵ���nOff����display��ĳ���*
	 *diplay��ĳ��ȵ��ڹ������ڲ�����һ           */
	 /*����������������*/
	while (t != NULL)
	{
		procDecPart(t);
		t = t->sibling;
	}
	t = p;
	Body(t->child[2]);/*����Block*/
	/*�������ֽ�����ɾ�������β�ʱ���½����ķ��ű�*/
	if (Level != -1)
		DestroyTable();/*������ǰscope*/
}

SymbTable * SemanticParser::HeadProcess(TreeNode * t)
{
	AttributeIR attrIr;
	int present = FALSE;
	SymbTable  * entry = NULL;
	/*������*/
	attrIr.kind = procKind;
	attrIr.idtype = NULL;
	attrIr.More.ProcAttr.level = Level + 1;
	if (t != NULL)
	{
		/*�ǼǺ����ķ��ű���*/
		string id = CT2A(t->name[0].GetString());
		present = Enter(id, &attrIr, &entry);
		t->table[0] = entry;
		/*�����β�������*/
	}
	entry->attrIR.More.ProcAttr.param = ParaDecList(t);
	return entry;
}

ParamTable * SemanticParser::ParaDecList(TreeNode * t)
{
	TreeNode * p = NULL;
	ParamTable  * Ptr1 = NULL;
	ParamTable  * Ptr2 = NULL;
	ParamTable  * head = NULL;
	if (t != NULL)
	{
		if (t->child[0] != NULL)
			p = t->child[0];    /*���������ڵ�ĵ�һ�����ӽڵ�*/
		CreatTable(); /*�����µľֲ�����*/
		Off = 7;                /*�ӳ����еı�����ʼƫ����Ϊ8*/
		VarDecPart(p); /*������������*/
		SymbTable * Ptr0 = scope[Level];

		while (Ptr0 != NULL)         /*ֻҪ��Ϊ�գ��ͷ������ֵܽڵ�*/
		{
			/*�����βη��ű���ʹ�����������ű��param��*/
			Ptr2 = NewParam();
			if (head == NULL)
				head = Ptr1 = Ptr2;
			//Ptr0->attrIR.More.VarAttr.isParam = true;
			Ptr2->entry = Ptr0;
			Ptr2->next = NULL;
			if (Ptr2 != Ptr1)
			{
				Ptr1->next = Ptr2;
				Ptr1 = Ptr2;
			}
			Ptr0 = Ptr0->next;
		}
	}
	return head;   /*�����βη��ű��ͷָ��*/
}

void SemanticParser::Body(TreeNode * t)
{
	if (t->nodekind == StmLK)
	{
		TreeNode * p = t->child[0];
		while (p != NULL)
		{
			statement(p);  /*�������״̬������*/
			p = p->sibling;   /*���ζ����﷨��������е��ֵܽڵ�*/
		}
	}
}

void SemanticParser::statement(TreeNode * t)
{
	switch (t->kind->stmt)
	{
	case IfK: ifstatment(t); break;
	case WhileK: whilestatement(t); break;
	case AssignK: assignstatement(t); break;
	case ReadK:     readstatement(t); break;
	case WriteK: writestatement(t); break;
	case CallK:     callstatement(t); break;
	case ReturnK: returnstatement(t); break;
	default:
		ErrorPrompt(t->lineno, "", "statement type error!\r\n");
		break;
	}
}

TypeIR * SemanticParser::Expr(TreeNode * t, AccessKind * Ekind)
{
	int present = FALSE;
	SymbTable * entry = NULL;
	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;
	if (t != NULL)
		switch (t->kind->exp)
		{
		case ConstK:
			Eptr = TypeProcess(t, IntegerK);
			Eptr->kind = intTy;
			if (Ekind != NULL)
				(*Ekind) = dir;   /*ֱ�ӱ���*/
			break;
		case VariK:
			/*Var = id������*/
			if (t->child[0] == NULL)
			{
				/*�ڷ��ű��в��Ҵ˱�ʶ��*/
				string id = CT2A(t->name[0].GetString());
				present = FindEntry(id, &entry);
				t->table[0] = entry;
				if (present != FALSE)
				{   /*id���Ǳ���*/
					if (FindAttr(entry).kind != varKind)
					{
						id = CT2A(t->name[0]);
						ErrorPrompt(t->lineno, id, "is not variable error!\r\n");
						Eptr = NULL;
					}
					else
					{
						Eptr = entry->attrIR.idtype;
						if (Ekind != NULL)
							(*Ekind) = indir;  /*��ӱ���*/
					}
				}
				else /*��ʶ��������*/
				{
					id = CT2A(t->name[0]);
					ErrorPrompt(t->lineno, id, "is not declarations!\r\n");
				}
			}
			else/*Var = Var0[E]������*/
			{
				if (t->attr->expattr->varkind == ArrayMembV)
					Eptr = arrayVar(t);
				else /*Var = Var0.id������*/
					if (t->attr->expattr->varkind == FieldMembV)
						Eptr = recordVar(t);
			}
			break;
		case OpK:
			/*�ݹ���ö��ӽڵ�*/
			Eptr0 = Expr(t->child[0], NULL);
			if (Eptr0 == NULL)
				return NULL;
			Eptr1 = Expr(t->child[1], NULL);
			if (Eptr1 == NULL)
				return NULL;
			/*�����б�*/
			present = Compat(Eptr0, Eptr1);
			if (present != FALSE)
			{
				switch (t->attr->expattr->op)
				{
				case LT:
				case EQU:
					Eptr = boolPtr;
					break;  /*�������ʽ*/
				case PLUS:
				case MINUS:
				case LexType::MULTIPLY:
				case LexType::DIVIDE:
					Eptr = intPtr;
					break;  /*�������ʽ*/
				}
				if (Ekind != NULL)
					(*Ekind) = dir; /*ֱ�ӱ���*/
			}
			else
				ErrorPrompt(t->lineno, "", "operator is not compat!\r\n");
			break;
		}
	return Eptr;
}

TypeIR * SemanticParser::arrayVar(TreeNode * t)
{
	int present = FALSE;
	SymbTable * entry = NULL;
	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;
	/*�ڷ��ű��в��Ҵ˱�ʶ��*/
	string id = CT2A(t->name[0].GetString());
	present = FindEntry(id, &entry);
	t->table[0] = entry;
	/*�ҵ�*/
	if (present != FALSE)
	{
		/*Var0���Ǳ���*/
		if (FindAttr(entry).kind != varKind)
		{
			id = CT2A(t->name[0].GetString());
			ErrorPrompt(t->lineno, id, "is not variable error!\r\n");
			Eptr = NULL;
		}
		else/*Var0�����������ͱ���*/
			if (FindAttr(entry).idtype != NULL)
				if (FindAttr(entry).idtype->kind != arrayTy)
				{
					id = CT2A(t->name[0].GetString());
					ErrorPrompt(t->lineno, id, "is not array variable error !\r\n");
					Eptr = NULL;
				}
				else
				{
					/*���E�������Ƿ����±��������*/
					Eptr0 = entry->attrIR.idtype->More.ArrayAttr.indexTy;
					if (Eptr0 == NULL)
						return NULL;
					Eptr1 = Expr(t->child[0], NULL);//intPtr;
					if (Eptr1 == NULL)
						return NULL;
					present = Compat(Eptr0, Eptr1);
					if (present != TRUE)
					{
						ErrorPrompt(t->lineno, "", "type is not matched with the array member error !\r\n");
						Eptr = NULL;
					}
					else
						Eptr = entry->attrIR.idtype->More.ArrayAttr.elemTy;
				}
	}
	else {/*��ʶ��������*/
		id = CT2A(t->name[0].GetString());
		ErrorPrompt(t->lineno, id, "is not declarations!\r\n");
	}
	return Eptr;
}

TypeIR * SemanticParser::recordVar(TreeNode * t)
{
	int present = FALSE;
	int result = TRUE;
	SymbTable * entry = NULL;
	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;
	fieldchain * currentP = NULL;
	/*�ڷ��ű��в��Ҵ˱�ʶ��*/
	string id = CT2A(t->name[0].GetString());
	present = FindEntry(id, &entry);
	t->table[0] = entry;
	/*�ҵ�*/
	if (present != FALSE)
	{
		/*Var0���Ǳ���*/
		if (FindAttr(entry).kind != varKind)
		{
			id = CT2A(t->name[0].GetString());
			ErrorPrompt(t->lineno, id, "is not variable error!\r\n");
			Eptr = NULL;
		}
		else/*Var0���Ǽ�¼���ͱ���*/
			if (FindAttr(entry).idtype->kind != recordTy)
			{
				id = CT2A(t->name[0].GetString());
				ErrorPrompt(t->lineno, id, "is not record variable error !\r\n");
				Eptr = NULL;
			}
			else/*���id�Ƿ��ǺϷ�����*/
			{
				Eptr0 = entry->attrIR.idtype;
				currentP = Eptr0->More.body;
				while ((currentP != NULL) && (result != FALSE))
				{
					string temp = CT2A(t->name[0].GetString());
					result = strcmp(temp.c_str(), currentP->id.c_str());
					/*������*/
					if (result == FALSE)
						Eptr = currentP->UnitType;
					else
						currentP = currentP->Next;
				}
				if (currentP == NULL)
					if (result != FALSE)
					{
						string temp = CT2A(t->child[0]->name[0].GetString());
						ErrorPrompt(t->child[0]->lineno, temp,"is not field type!\r\n");
						Eptr = NULL;
					}
					else/*���id���������*/
						if (t->child[0]->child[0] != NULL)
							Eptr = arrayVar(t->child[0]);
			}
	}
	else {/*��ʶ��������*/
		string name = CT2A(t->name[0]);
		ErrorPrompt(t->lineno, name, "is not declarations!\r\n");
	}
	return Eptr;
}

void SemanticParser::assignstatement(TreeNode * t)
{
	SymbTable * entry = NULL;
	int present = FALSE;
	TypeIR * ptr = NULL;
	TypeIR * Eptr = NULL;
	TreeNode * child1 = NULL;
	TreeNode * child2 = NULL;
	child1 = t->child[0];
	child2 = t->child[1];
	if (child1->child[0] == NULL)
	{
		/*�ڷ��ű��в��Ҵ˱�ʶ��*/
		string id = CT2A(child1->name[0].GetString());
		present = FindEntry(id, &entry);
		if (present != FALSE)
		{   /*id���Ǳ���*/
			if (FindAttr(entry).kind != varKind)
			{
				string name = CT2A(child1->name[0].GetString());
				ErrorPrompt(child1->lineno, name, "is not variable error!\r\n");
				Eptr = NULL;
			}
			else
			{
				Eptr = entry->attrIR.idtype;
				child1->table[0] = entry;
			}
		}
		else {/*��ʶ��������*/

			string name = CT2A(child1->name[0].GetString());
			ErrorPrompt(child1->lineno, name, "is not declarations!\r\n");
		}
	}
	else/*Var0[E]������*/
	{
		if (child1->attr->expattr->varkind == ArrayMembV)
			Eptr = arrayVar(child1);
		else /*Var0.id������*/
			if (child1->attr->expattr->varkind == FieldMembV)
				Eptr = recordVar(child1);
	}
	if (Eptr != NULL)
	{
		if ((t->nodekind == StmtK) && (t->kind->stmt == AssignK))
		{
			/*����ǲ��Ǹ�ֵ������ ���͵ȼ�*/
			ptr = Expr(child2, NULL);
			if (!Compat(ptr, Eptr))
				ErrorPrompt(t->lineno, "", "ass_expression error!\r\n");
		}
		/*��ֵ����в��ܳ��ֺ�������*/
	}
}

void SemanticParser::callstatement(TreeNode * t)
{
	AccessKind  Ekind;
	int present = FALSE;
	SymbTable * entry = NULL;
	TreeNode * p = NULL;
	/*��id����������ű�*/
	string id = CT2A(t->child[0]->name[0].GetString());
	present = FindEntry(id, &entry);
	t->child[0]->table[0] = entry;
	/*δ�鵽��ʾ����������*/
	if (present == FALSE)
	{
		string name = CT2A(t->child[0]->name[0].GetString());
		ErrorPrompt(t->lineno, name, "function is not declarationed!\r\n");
	}
	else
		/*id���Ǻ�����*/
		if (FindAttr(entry).kind != procKind) {
			string name = CT2A(t->name[0].GetString());
			ErrorPrompt(t->lineno,name, "is not function name!\r\n");
		}
		else/*��ʵ��ƥ��*/
		{
			p = t->child[1];
			/*paramPָ���βη��ű�ı�ͷ*/
			ParamTable * paramP = FindAttr(entry).More.ProcAttr.param;
			while ((p != NULL) && (paramP != NULL))
			{
				SymbTable  * paraEntry = paramP->entry;
				TypeIR  * Etp = Expr(p, &Ekind);/*ʵ��*/
				/*�������ƥ��*/
				if ((FindAttr(paraEntry).More.VarAttr.access == indir) && (Ekind == dir))
					ErrorPrompt(p->lineno, "", "param kind is not match!\r\n");
				else
					/*�������Ͳ�ƥ��*/
					if ((FindAttr(paraEntry).idtype) != Etp)
						ErrorPrompt(p->lineno, "", "param type is not match!\r\n");
				p = p->sibling;
				paramP = paramP->next;
			}
			/*����������ƥ��*/
			if ((p != NULL) || (paramP != NULL))
				ErrorPrompt(t->child[1]->lineno, "", "param num is not match!\r\n");
		}
}

void SemanticParser::ifstatment(TreeNode * t)
{
	AccessKind * Ekind = NULL;
	TypeIR *  Etp = Expr(t->child[0], Ekind);
	if (Etp != NULL)
		/*�����������ʽ*/
		if (Etp->kind != boolTy)
			ErrorPrompt(t->lineno, "", "condition expressrion error!\r\n");  /*�߼����ʽ����*/
		else
		{
			TreeNode * p = t->child[1];
			/*����then������в���*/
			while (p != NULL)
			{
				statement(p);
				p = p->sibling;
			}
			t = t->child[2]; /*����������*/
			/*����else��䲻��*/
			while (t != NULL)
			{
				statement(t);
				t = t->sibling;
			}
		}
}

void SemanticParser::whilestatement(TreeNode * t)
{
	TypeIR *  Etp = Expr(t->child[0], NULL);
	if (Etp != NULL)
		/*�����������ʽ����*/
		if (Etp->kind != boolTy)
			ErrorPrompt(t->lineno, "", "condition expression error!\r\n");  /*�߼����ʽ����*/
		else
		{
			t = t->child[1];
			/*����ѭ������*/
			while (t != NULL)
			{
				statement(t);
				t = t->sibling;
			}
		}
}

void SemanticParser::readstatement(TreeNode * t)
{
	SymbTable * entry = NULL;
	int present = FALSE;
	/*��id����������ű�*/
	string id = CT2A(t->name[0].GetString());
	present = FindEntry(id, &entry);
	t->table[0] = entry;
	/*δ�鵽��ʾ����������*/
	if (present == FALSE) {
		string name = CT2A(t->name[0].GetString());
		ErrorPrompt(t->lineno, name, " is not declarationed!\r\n");
	}
	else if (entry->attrIR.kind != varKind) {/*���Ǳ�����ʶ������*/
		string name = CT2A(t->name[0].GetString());
		ErrorPrompt(t->lineno, name, " is not var name!\r\n ");
	}
}

void SemanticParser::writestatement(TreeNode * t)
{
	TypeIR * Etp = Expr(t->child[0], NULL);
	if (Etp != NULL)
		/*������ʽ����Ϊbool���ͣ�����*/
		if (Etp->kind == boolTy)
			ErrorPrompt(t->lineno, "", "exprssion type error!\r\n");
}

void SemanticParser::returnstatement(TreeNode * t)
{
	if (Level == 0)/*����������������������У�����*/
		ErrorPrompt(t->lineno, "", "return statement error!\r\n");
}



