#pragma once

#include "ScCommand.h"
#include <AIS_SequenceOfInteractive.hxx>

class ScCmdEdit
{
public:
	ScCmdEdit(void);
	~ScCmdEdit(void);
};

//////////////////////////////////////////////////////////////////////
//ɾ�����ɾ��һ����������
class ScCmdDelete : public ScCommand{
public:
	
	//�����ͽ�������
	virtual	  int		Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int		End();
};