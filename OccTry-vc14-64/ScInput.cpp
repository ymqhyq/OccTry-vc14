#include "StdAfx.h"
#include ".\scinput.h"


///////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////
//
ScInput::ScInput(void)
{
	this->_nVarType = INPUT_UNKNOWN;
	this->_szVal = "";
}

ScInput::~ScInput(void)
{
}

//接收字符串，返回值
BOOL	ScInput::InputString(int nVarType,LPCTSTR lpszVal)
{
	BOOL bret = TRUE;
	this->_nVarType = nVarType;
	this->_szVal = lpszVal;

	switch(this->_nVarType)
	{
	case INPUT_INTEGER://输入整形
		{
			_nVal = atoi(lpszVal);
		}
		break;
	case INPUT_DOUBLE://输入浮点数
		{
			_dVal = ::atof(lpszVal);
		}
		break;
	case INPUT_POINT3D://3维点
	case INPUT_VECTOR:
		{
			bret = FALSE;
			CString szInput = lpszVal;
			gp_Pnt pnt;
			int nPos = szInput.Find(",");
			if(nPos > 0)
			{
				CString szVal,szRest;
				szVal = szInput.Left(nPos);
				szRest = szInput.Right(szInput.GetLength() - nPos - 1);
				pnt.SetX(atof(szVal));
				nPos = szRest.Find(",");
				if(nPos > 0)
				{
					szVal = szRest.Left(nPos);
					szRest = szRest.Right(szRest.GetLength() - nPos - 1);
					pnt.SetY(atof(szVal));
					pnt.SetZ(atof(szRest));

					if(this->_nVarType == INPUT_POINT3D)
						this->_pnt = pnt;
					else
						this->_vec.SetCoord(pnt.X(),pnt.Y(),pnt.Z());
					bret = TRUE;
				}
			} 
		}
		break;
	case INPUT_POINT2D://2维点
		break;
	case INPUT_STRING:
		{
			_strVal = lpszVal;
			_strVal.TrimLeft();
			_strVal.TrimRight();
		}
		break;
	default:
		break;
	}

	return bret;
}

int		ScInput::IntVal() const
{
	ASSERT(_nVarType == INPUT_INTEGER);
	return _nVal;
}

double	ScInput::DblVal() const
{
	ASSERT(_nVarType == INPUT_DOUBLE);
	return this->_dVal;
}

gp_Pnt	ScInput::PntVal() const
{
	ASSERT(_nVarType == INPUT_POINT3D);
	return this->_pnt;
}

gp_Pnt2d	ScInput::Pnt2dVal() const
{
	ASSERT(_nVarType == INPUT_POINT2D);
	return this->_pnt2d;
}

gp_Vec		ScInput::VecVal() const
{
	ASSERT(_nVarType == INPUT_VECTOR);
	return this->_vec;
}

CString		ScInput::StringVal() const
{
	ASSERT(_nVarType == INPUT_STRING);
	return this->_strVal;
}

///////////////////////////////////////////////////////////
// 输入值
ScInputVar::ScInputVar()
{
	Reset();
}

ScInputVar::ScInputVar(int nVarType)
{
	Reset();
	this->_nVarType = nVarType;
}

ScInputVar::ScInputVar(double dVal)
{
	Reset();

	this->_nVarType = INPUT_DOUBLE;
	this->_dVal = dVal;
}

ScInputVar::ScInputVar(gp_Pnt pnt)
{
	Reset();

	this->_nVarType = INPUT_POINT3D;
	this->_pnt = pnt;
}

ScInputVar::ScInputVar(const ScInputVar& other)
{
	*this = other;
}
	
ScInputVar&		ScInputVar::operator=(const ScInputVar& other)
{
	this->_nVarType = other._nVarType;
	this->_lpData = other._lpData;
	switch(this->_nVarType)
	{
	case INPUT_INTEGER:
		this->_nVal = other._nVal;
		break;
	case INPUT_DOUBLE:
		this->_dVal = other._dVal;
		break;
	case INPUT_POINT3D:
		this->_pnt = other._pnt;
		break;
	case INPUT_POINT2D:
		this->_pnt2d = other._pnt2d;
		break;
	case INPUT_VECTOR:
		this->_vec = other._vec;
		break;
	case INPUT_STRING:
		this->_strVal = other._strVal;
		break;
	default:
		break;
	}
	return *this;
}

void	ScInputVar::Reset()
{
	this->_nVarType = INPUT_UNKNOWN;
	this->_lpData = NULL;
	//
	this->_nVal = 0;
	this->_dVal = 0.0;
	this->_pnt2d.SetCoord(0.0,0.0);
	this->_pnt.SetCoord(0.0,0.0,0.0);
	this->_vec.SetCoord(0.0,0.0,0.0);
	this->_strVal.Empty();
}

void	ScInputVar::Set(const ScInput& input)
{
	this->_nVarType = input.VarType();
	switch(this->_nVarType)
	{
	case INPUT_INTEGER:
		this->_nVal = input.IntVal();
		break;
	case INPUT_DOUBLE:
		this->_dVal = input.DblVal();
		break;
	case INPUT_POINT3D:
		this->_pnt = input.PntVal();
		break;
	case INPUT_POINT2D:
		this->_pnt2d = input.Pnt2dVal();
		break;
	case INPUT_VECTOR:
		this->_vec = input.VecVal();
		break;
	case INPUT_STRING:
		this->_strVal = input.StringVal();
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////
//
ScInputReq::ScInputReq()
{
	this->_nInputMode = eInputBar;//默认
	this->_nVarType = INPUT_UNKNOWN;
	this->_szDesc = "";
	this->_pCmd = NULL;
	this->_lpData = NULL;
	this->_bWCS = TRUE;
}

ScInputReq::ScInputReq(int nVarType,LPCTSTR lpszDesc,ScCommand *pCmd)//常用形式
{
	ASSERT(lpszDesc && pCmd);
	ASSERT((nVarType >= INPUT_INTEGER) && (nVarType < INPUT_UNKNOWN));
	this->_nInputMode = eInputBar;//默认
	this->_nVarType = nVarType;
	this->_szDesc = lpszDesc;
	this->_pCmd = pCmd;
	this->_lpData = NULL;
	this->_bWCS = TRUE;
}

ScInputReq::ScInputReq(const ScInputReq& other)
{
	*this = other;
}

ScInputReq&		ScInputReq::operator=(const ScInputReq& other)
{
	this->_nInputMode = other._nInputMode;
	this->_nVarType = other._nVarType;
	this->_szDesc = other._szDesc;
	this->_pCmd = other._pCmd;
	this->_lpData = other._lpData;
	this->_bWCS = other._bWCS;

	return *this;
}

BOOL	ScInputReq::IsValid() const
{
	if((this->_nInputMode != eInputBar) && 
		(this->_nInputMode != eInputDlg))
		return FALSE;

	if((this->_nVarType < INPUT_INTEGER) || 
		(this->_nVarType >= INPUT_UNKNOWN))
		return FALSE;

	if(!_pCmd || this->_szDesc.IsEmpty())
		return FALSE;

	return TRUE;
}




