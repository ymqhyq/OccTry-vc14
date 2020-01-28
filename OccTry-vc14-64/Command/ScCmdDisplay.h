#pragma once

#include "ScCommand.h"

class ScCmdDisplay
{
public:
	ScCmdDisplay(void);
	~ScCmdDisplay(void);
};

//////////////////////////////////////////////////////////
// ���������ʵ��
// ʹ��AIS_TextureShapeʵ��
class ScCmdTexture : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

public:
	//��������
	BOOL					SetTexture(LPCTSTR lpszFile,BOOL bDone);
	
protected:
	enum{S_SHAPE,S_TEXTURE};

	BOOL					HasSelectedShape();
	int						DoTexture();

	BOOL					m_bModified;
};