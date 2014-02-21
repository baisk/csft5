#ifndef PYTOKEN_H
#define PYTOKEN_H

#include "sphinx.h"
#include "rs_result.h" //暂时没有编码解码的过程
#include "py_layer.h"


#include "set"
//#include "rs_result.h" //result set 结果集
//#include "rs_result.hpp"


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
	virtual const BYTE *GetExtend(); //暂时提供一个和mmseg类似的, 作为查询扩展的接口

	//virtual const char *	GetBufferPtr () const {	return (const char *) m_pCur;}
	//virtual const char *	GetTokenStart () const	{return m_segToken;	}
	virtual int	 GetLastTokenLen () const { return m_iLastTokenLen; }
	virtual bool				IsSegment(const BYTE * pCur);

protected:
	BYTE m_sAccumSeg [ 3*SPH_MAX_WORD_LEN+3 ];
	BYTE *				m_pAccumSeg;							///< current accumulator position
	int					m_iLastTokenLen;			///< last token length, in codepoints

public:
	PyObject * _obj; //改为public, 否则在clone函数中无法复制_obj
	std::set<int> SegmentOffset; //暂时是一个set存储偏移量, 之后改成rs_result结果集
	//Rosetta::SimpleSwResult *rs;

};

#endif // PYTOKEN_H
