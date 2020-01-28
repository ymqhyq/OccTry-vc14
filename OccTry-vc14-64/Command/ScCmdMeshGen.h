#pragma once

#include "ScCommand.h"

class ScCmdMeshGen 
{
public:
	ScCmdMeshGen(void);
	~ScCmdMeshGen(void);
};

//////////////////////////////////////////////////////////////////////////
// ������ת��Ϊ����
class ScCmdMeshConvertToMesh : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);


protected:
	BOOL					HasSelObj();
	BOOL					DoConvert();

};