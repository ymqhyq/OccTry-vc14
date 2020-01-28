#include "StdAfx.h"
#include ".\ffdrmf.h"

//////////////////////////////////////////////////////////////////////////
//
FFDCurvePointFrame::FFDCurvePointFrame()
{
	_u = 0.0;
}

FFDCurvePointFrame::FFDCurvePointFrame(const FFDCurvePointFrame& other)
{
	*this = other;
}

FFDCurvePointFrame&	FFDCurvePointFrame::operator=(const FFDCurvePointFrame& other)
{
	_u = other._u;
	_O = other._O;
	_dT = other._dT;
	_dN = other._dN;
	_dB = other._dB;

	return *this;
}

void	FFDCurvePointFrame::Set(double u,const gp_Pnt& pnt,const gp_Dir& T,
								const gp_Dir& N,const gp_Dir& B)
{
	_u = u;
	_O = pnt;
	_dT = T;
	_dN = N;
	_dB = B;
}

//////////////////////////////////////////////////////////////////////////
//
FFDCurveRMF::FFDCurveRMF()
{
	this->m_nSamps = 64;//默认个数
	this->m_bSetStartNormal = FALSE;
}

BOOL	FFDCurveRMF::Init(const Handle(Geom_Curve)& aCur,double df,double dl,int nSamps)
{
	this->m_aCurve = aCur;
	m_nSamps = nSamps;
	m_df = df;
	m_dl = dl;
	m_dstep = (dl - df)/nSamps;//注意这里

	return TRUE;
}

//清除计算结果
void	FFDCurveRMF::Clear()
{
	m_frmArray.clear();
	this->m_bSetStartNormal = FALSE;
}

//设置初始法矢，主要应用于直线等初始法矢无法确定的情况
void	FFDCurveRMF::SetStartNormal(const gp_Dir& startN)
{
	ASSERT(startN.XYZ().Modulus() > Precision::Confusion());
	this->m_bSetStartNormal = TRUE;
	this->m_startNormal = startN;
}


BOOL	FFDCurveRMF::CalcLcsCoord(double u,const gp_Pnt& pnt,gp_Pnt& lcpnt)
{
	//先找到对于的frm
	UINT idx = (UINT)((u - m_df)/m_dstep);
	if(idx >= m_frmArray.size())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	FFDCurvePointFrame& frm = m_frmArray.at(idx);

	gp_Trsf wtol;
	gp_Ax3 ax(frm._O,frm._dN,frm._dT);
	wtol.SetTransformation(ax);//从wcs到lcs的转换
	lcpnt = pnt;
	lcpnt.Transform(wtol);//转到lcs中

	return TRUE;
}

BOOL	FFDCurveRMF::CalcDfmCoord(double u,const gp_Pnt& lcpnt,gp_Pnt& pnt)
{
	//先找到对于的frm
	UINT idx = (UINT)((u - m_df)/m_dstep);
	if(idx >= m_frmArray.size())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	FFDCurvePointFrame& frm = m_frmArray.at(idx);

	gp_Trsf ltow;
	gp_Ax3 ax(frm._O,frm._dN,frm._dT);
	ltow.SetTransformation(ax);//从wcs到lcs的转换
	ltow.Invert();

	pnt = lcpnt;
	pnt.Transform(ltow);//转到wcs中

	return TRUE;
}

//计算
BOOL	FFDCurveRMF::CalcRotateRMF()
{
	return CalcRMF(M_ROTATE);
}

//sloan方法计算
BOOL	FFDCurveRMF::CalcSloanRMF()
{
	return CalcRMF(M_SLOAN);
}

//1:旋转法，2：sloan方法
BOOL	FFDCurveRMF::CalcRMF(int nMethod)
{
	m_frmArray.clear();//先清空

	double t;
	gp_Dir T0,N0,B0,T,N,B;
	gp_Pnt pnt;
	FFDCurvePointFrame frm;

	int ix;
	for(ix = 0;ix <= m_nSamps;ix ++)
	{
		t = m_df + ix * m_dstep;
		if(ix == 0)
		{//初始计算
			gp_Vec vt,vtt;
			BOOL bOk = TRUE;
			try{
				m_aCurve->D2(t,pnt,vt,vtt);
				if(vt.SquareMagnitude() < 1e-10)//没切线
					return FALSE;
				T = gp_Dir(vt);
				if(vtt.SquareMagnitude() < 1e-10)
					bOk = FALSE;
				else
				{
					B0 = gp_Dir(vtt);
					B = T.Crossed(B0);
					if(B.XYZ().Modulus() < 1e-6)
					{
						gp_Ax2 axe(gp_Pnt(0,0,0),T);
						B.SetXYZ(axe.YDirection().XYZ());
					}
					N = B.Crossed(T);
				}
			}catch(Standard_Failure){//1阶连续
				bOk = FALSE;
			}

			if(!bOk)
			{
				try{
					m_aCurve->D1(t,pnt,vt);
				}catch(Standard_Failure){
					return FALSE;
				}
				if(vt.SquareMagnitude() < 1e-10)//没切线
					return FALSE;
				T = gp_Dir(vt);

				//如果有初始向量，使用初始向量
				if(m_bSetStartNormal)
				{
					N = m_startNormal;
					B = T.Crossed(N);
					if(B.XYZ().Modulus() > 1e-6)
						bOk = TRUE;
				}
				
				if(!bOk)
				{
					//任意轴初始化
					gp_Ax2 axe(gp_Pnt(0,0,0),T);
					B.SetXYZ(axe.YDirection().XYZ());
					N = B0.Crossed(T);
				}
			}

			frm.Set(t,pnt,T,N,B);
			m_frmArray.push_back(frm);
		}
		else
		{//使用旋转法计算
			if(nMethod == M_ROTATE)
			{
				if(!DoCalcRotateFrame(t,T0,N0,B0,T,N,B))
					return FALSE;
			}
			else if(nMethod == M_SLOAN)
			{
				if(!DoCalcSloanFrame(t,T0,N0,B0,T,N,B))
					return FALSE;
			}
		}

		T0 = T;N0 = N;B0 = B;
	}

	return TRUE;
}

BOOL	FFDCurveRMF::DoCalcRotateFrame(double t,
									   const gp_Dir& T0,const gp_Dir& N0,const gp_Dir& B0,
									   gp_Dir& T,gp_Dir& N,gp_Dir& B)
{
	gp_Pnt pnt;
	FFDCurvePointFrame frm;
	gp_Vec vt;

	try{
		m_aCurve->D1(t,pnt,vt);
	}catch(Standard_Failure){
		return FALSE;
	}

	if(vt.SquareMagnitude() < 1e-10)
		return FALSE;

	T = gp_Dir(vt);
	//求T0×T1=A
	gp_Dir A;
	A = T0.Crossed(T);
	if(A.XYZ().Modulus() < 1e-10)
	{
		T = T0;
		N = N0;
		B = B0;
	}
	else
	{
		//计算旋转矩阵
		double sqx = A.X() * A.X(),sqy = A.Y() * A.Y(),sqz = A.Z() * A.Z();
		double cos = T0.Dot(T),cosl = 1 - cos;
		double xycosl = A.X() * A.Y() * cosl;
		double yzcosl = A.Y() * A.Z() * cosl;
		double zxcosl = A.X() * A.Z() * cosl;
		double sin = sqrt(1 - cos * cos),xsin = A.X() * sin,
			ysin = A.Y() * sin,zsin = A.Z() * sin;

		gp_Mat m0(sqx + (1 - sqx) * cos,xycosl + zsin,zxcosl - ysin,
			xycosl - zsin,sqy + (1 - sqy) * cos, yzcosl + xsin,
			zxcosl + ysin,yzcosl - xsin,sqz + (1 - sqz) * cos);

		//计算新的N
		N.SetX(m0.Row(1).Dot(N0.XYZ()));
		N.SetY(m0.Row(2).Dot(N0.XYZ()));
		N.SetZ(m0.Row(3).Dot(N0.XYZ()));

		B = T.Crossed(N);
	}

	frm.Set(t,pnt,T,N,B);
	m_frmArray.push_back(frm);

	return TRUE;
}

BOOL	FFDCurveRMF::DoCalcSloanFrame(double t,
									  const gp_Dir& T0,const gp_Dir& N0,const gp_Dir& B0,
									  gp_Dir& T,gp_Dir& N,gp_Dir& B)
{
	gp_Pnt pnt;
	FFDCurvePointFrame frm;
	gp_Vec vt;

	try{
		m_aCurve->D1(t,pnt,vt);
	}catch(Standard_Failure){
		return FALSE;
	}

	if(vt.SquareMagnitude() < 1e-10)
		return FALSE;
	T = gp_Dir(vt);
	N = B0.Crossed(T);
	if(N.XYZ().Modulus() < 1e-10)
		return FALSE;//退化情况
	B = T.Crossed(N);

	frm.Set(t,pnt,T,N,B);
	m_frmArray.push_back(frm);

	return TRUE;
}
