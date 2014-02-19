#ifndef PYTOKEN_H
#define PYTOKEN_H

#include "sphinx.h"
#include "rs_result.h" //暂时没有编码解码的过程
#include "py_layer.h"

template < bool IS_QUERY >
class CSphTokenizer_Python : public CSphTokenizer_UTF8 <IS_QUERY>
{

public:
	CSphTokenizer_Python ();
	~CSphTokenizer_Python ();
	//void bind(PyObject *obj); //绑定python的分词器. 暂时无效
	void init(const char* python_path);
	virtual void				SetBuffer ( BYTE * sBuffer, int iLength );
	virtual BYTE *				GetToken ();
	virtual ISphTokenizer *		Clone ( ESphTokenizerClone eMode ) const;

protected:
	BYTE m_sAccumSeg [ 3*SPH_MAX_WORD_LEN+3 ];
public:
	PyObject * _obj; //改为public, 否则在clone函数中无法复制_obj
};

#endif // PYTOKEN_H
