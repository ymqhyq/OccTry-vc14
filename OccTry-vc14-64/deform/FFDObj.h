#pragma once

//////////////////////////////////////////////////////////////////////////
/*
* 代表一个变形对象，可能是曲线、曲面、组合曲线、组合曲面等。
* 这里简单的以曲线和曲面分类，曲面包括了solid、shell等。
*
**********************/

class FFDTool;
class FFDAlgo;

//负责变形前的曲线、曲面转换和变形后的组合等。
class FFDObj
{
public:
	FFDObj(void);
	virtual ~FFDObj(void);

	//以aShape进行初始化
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
	TopoDS_Shape				m_aShape;//要变形的对象
	TopoDS_Shape				m_aNurbsShape;//变形后的对象
	TopoDS_Shape				m_aNewShape;//变形后的对象

	BOOL						m_bValid;//是否初始化
};

class FFDSurfObj : public FFDObj{
public:
	FFDSurfObj();
	virtual ~FFDSurfObj();

	//以aShape进行初始化
	virtual BOOL				InitShape(const TopoDS_Shape& aShape,
									int nucpts = 100,int nvcpts = 100,
									int nudeg = 3,int nvdeg = 3);


	//对象变形
	int							Deform(FFDTool *pTool,FFDAlgo *pAlgo);

	
	

};