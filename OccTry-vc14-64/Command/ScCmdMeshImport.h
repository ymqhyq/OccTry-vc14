#pragma once

#include "ScCommand.h"

class ScCmdMeshImport
{
public:
	ScCmdMeshImport(void);
	~ScCmdMeshImport(void);
};

class ScCmdMeshImportMs3d : public ScCommand{
public:
	ScCmdMeshImportMs3d();

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

};