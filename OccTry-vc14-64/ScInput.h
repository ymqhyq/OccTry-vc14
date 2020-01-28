#pragma once

//�����֧�ֵ���������
enum{
	INPUT_INTEGER,//��������
	INPUT_DOUBLE,//���븡����
	INPUT_POINT3D,//3ά��
	INPUT_POINT2D,//2ά��
	INPUT_VECTOR,//��������
	INPUT_STRING,//�����ַ���
	INPUT_UNKNOWN,
};

class ScCommand;


///////////////////////////////////////////////////////////
// ��������ַ������д���ת��Ϊ��Ӧ������ֵ��
class ScInput
{
public:
	ScInput(void);
	~ScInput(void);

	//�����ַ���������ֵ
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
// �����ֵ
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
	int				_nVarType;//��������
	void			*_lpData;//��������

	int				_nVal;
	double			_dVal;
	gp_Pnt			_pnt;
	gp_Pnt2d		_pnt2d;
	gp_Vec			_vec;
	CString			_strVal;
};

//////////////////////////////////////////////////////////
// ��������
class ScInputReq{
public:
	ScInputReq();
	ScInputReq(int nVarType,LPCTSTR lpszDesc,ScCommand *pCmd);//������ʽ
	ScInputReq(const ScInputReq& other);

	ScInputReq&		operator=(const ScInputReq& other);

	BOOL			IsValid() const;

public:
	//����ķ�ʽ
	enum{
		eInputBar,
		eInputDlg,
	};

public:
	int				_nInputMode;//���뷽ʽ������
	int				_nVarType;  //��������ֵ������
	CString			_szDesc;    //������Ϣ
	ScCommand		*_pCmd;     //���������
	void			*_lpData;   //cmd���������
	
	BOOL			_bWCS;		//��������Ƿ񷵻�WCSֵ��Ĭ������ֵΪUCS��???
};



