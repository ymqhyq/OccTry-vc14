#pragma once

//输入框支持的输入类型
enum{
	INPUT_INTEGER,//输入整形
	INPUT_DOUBLE,//输入浮点数
	INPUT_POINT3D,//3维点
	INPUT_POINT2D,//2维点
	INPUT_VECTOR,//输入向量
	INPUT_STRING,//输入字符串
	INPUT_UNKNOWN,
};

class ScCommand;


///////////////////////////////////////////////////////////
// 对输入的字符串进行处理，转换为相应的输入值。
class ScInput
{
public:
	ScInput(void);
	~ScInput(void);

	//接收字符串，返回值
	BOOL			InputString(int nVarType,LPCTSTR lpszVal);

	int				IntVal() const;
	double			DblVal() const;
	gp_Pnt			PntVal() const;
	gp_Pnt2d		Pnt2dVal() const;
	gp_Vec			VecVal() const;
	CString			StringVal() const;			

	int				VarType() const { return _nVarType; }

protected:
	int				_nVarType;
	CString			_szVal;
	
	int				_nVal;
	double			_dVal;
	gp_Pnt			_pnt;
	gp_Pnt2d		_pnt2d;
	gp_Vec			_vec;
	CString			_strVal;
};

///////////////////////////////////////////////////////////
// 输入的值
class ScInputVar{
public:
	ScInputVar();
	ScInputVar(int nVarType);
	ScInputVar(double dVal);
	ScInputVar(gp_Pnt pnt);
	ScInputVar(const ScInputVar& other);
	
	ScInputVar&		operator=(const ScInputVar& other);

	void			Reset();
	void			Set(const ScInput& input);

public:
	int				_nVarType;//输入类型
	void			*_lpData;//关联数据

	int				_nVal;
	double			_dVal;
	gp_Pnt			_pnt;
	gp_Pnt2d		_pnt2d;
	gp_Vec			_vec;
	CString			_strVal;
};

//////////////////////////////////////////////////////////
// 输入请求
class ScInputReq{
public:
	ScInputReq();
	ScInputReq(int nVarType,LPCTSTR lpszDesc,ScCommand *pCmd);//常用形式
	ScInputReq(const ScInputReq& other);

	ScInputReq&		operator=(const ScInputReq& other);

	BOOL			IsValid() const;

public:
	//输入的方式
	enum{
		eInputBar,
		eInputDlg,
	};

public:
	int				_nInputMode;//输入方式，两种
	int				_nVarType;  //请求输入值的类型
	CString			_szDesc;    //描述信息
	ScCommand		*_pCmd;     //请求的命令
	void			*_lpData;   //cmd的相关数据
	
	BOOL			_bWCS;		//点或向量是否返回WCS值，默认输入值为UCS。???
};



