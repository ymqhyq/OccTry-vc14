#pragma once

//////////////////////////////////////////////////////////////////////////
/*
* ����һ�����ζ��󣬿��������ߡ����桢������ߡ��������ȡ�
* ����򵥵������ߺ�������࣬���������solid��shell�ȡ�
*
**********************/

class FFDTool;
class FFDAlgo;

//�������ǰ�����ߡ�����ת���ͱ��κ����ϵȡ�
class FFDObj
{
public:
	FFDObj(void);
	virtual ~FFDObj(void);

	//��aShape���г�ʼ��
	virtual BOOL				InitShape(const TopoDS_Shape& aShape,
									int nucpts = 100,int nvcpts = 100,
									int nudeg = 3,int nvdeg = 3);

	TopoDS_Face					BuildNewFace(const Handle(Geom_Surface)& aNewSurf,const TopoDS_Face& aFace);

	TopoDS_Shape				SewNewFaces(const TopoDS_Compound& aC);						

	BOOL						IsValid() { return m_bValid; }
	TopoDS_Shape				NurbsShape() { return m_aNurbsShape; }

protected:
	TopoDS_Wire					BuildNewWire(const TopoDS_Wire& aWire,const Handle(Geom_Surface)& aNewSurf,
										const TopoDS_Face& aFace);

protected:
	TopoDS_Shape				m_aShape;//Ҫ���εĶ���
	TopoDS_Shape				m_aNurbsShape;//���κ�Ķ���
	TopoDS_Shape				m_aNewShape;//���κ�Ķ���

	BOOL						m_bValid;//�Ƿ��ʼ��
};

class FFDSurfObj : public FFDObj{
public:
	FFDSurfObj();
	virtual ~FFDSurfObj();

	//��aShape���г�ʼ��
	virtual BOOL				InitShape(const TopoDS_Shape& aShape,
									int nucpts = 100,int nvcpts = 100,
									int nudeg = 3,int nvdeg = 3);


	//�������
	int							Deform(FFDTool *pTool,FFDAlgo *pAlgo);

	
	

};