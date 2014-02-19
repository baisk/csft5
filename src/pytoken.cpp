#include "pytoken.h"
#include "pycsft.h"
#include "string"

#define PYSOURCE_DEBUG 1

template < bool IS_QUERY >
CSphTokenizer_Python<IS_QUERY>::CSphTokenizer_Python()
	:CSphTokenizer_UTF8<IS_QUERY>::CSphTokenizer_UTF8 ()
{
}

template < bool IS_QUERY >
CSphTokenizer_Python<IS_QUERY>::~CSphTokenizer_Python()
{
	if (PYSOURCE_DEBUG)
		printf("#in ~CSphTokenizer_Python\n");
	if (this->_obj)
	{
		DecreasePythonObject(this->_obj); //让python端维护ref_count
	}
}

template < bool IS_QUERY >
void CSphTokenizer_Python<IS_QUERY>::init(const char  *python_path)
{
	if (PYSOURCE_DEBUG)
		printf("#in init\n");
		printf("#python_path: %s\n", python_path );
	PyObject* obj = createPythonTokenizerObject(python_path);
	if ( obj )
	{
		if (PYSOURCE_DEBUG)
			printf("got the token obj successfully\n");
		this->_obj= obj;
		//Py_INCREF(this->_obj); 更改引用应该在cython端进行
	}else{
		printf("got the pytoken obj error\n");
	}
}

template < bool IS_QUERY >
void CSphTokenizer_Python<IS_QUERY>::SetBuffer(BYTE * sBuffer, int iLength)
{
	if (PYSOURCE_DEBUG)
		printf ("#in setbuffer\n");
	//CSphTokenizer_UTF8<IS_QUERY>::SetBuffer(sBuffer, iLength);
	pyTokenProcess(this->_obj, sBuffer, iLength); //给python端设定好词汇,然后直接得到结果. 同步接口.

}

template < bool IS_QUERY >
BYTE * CSphTokenizer_Python<IS_QUERY>::GetToken()
{
//	if (PYSOURCE_DEBUG)
//		printf ("#in gottoken\n");
	//return CSphTokenizer_UTF8<IS_QUERY>::GetToken();
	int iLength = SPH_MAX_WORD_LEN;
	int* p_ilength = &iLength;
	int ret = pyTokenGetToken(this->_obj, m_sAccumSeg, p_ilength);
	if (ret)
		return NULL;
	m_sAccumSeg[iLength] = '\0'; // c风格的字符串.
	//printf( "%s/x ", m_sAccumSeg);
	return m_sAccumSeg;
}

template < bool IS_QUERY >
ISphTokenizer *	CSphTokenizer_Python<IS_QUERY>::Clone( ESphTokenizerClone eMode ) const
{
	//ugly code. #fixme. need clear code
	if (PYSOURCE_DEBUG)
		printf ("#in Clone\n");
	CSphTokenizerBase * pClone;
	if ( eMode!=SPH_CLONE_INDEX )
	{
		CSphTokenizer_Python<true>* pClone_py = new CSphTokenizer_Python<true>();
		pClone_py->_obj = this->_obj; //引用计数?
		IncreasePythonObject(pClone_py->_obj); //和init时一样, copy保证引用计数要+1
		pClone = (CSphTokenizerBase*)pClone_py;
	}
	else
	{
		CSphTokenizer_Python<false>* pClone_py = new CSphTokenizer_Python<false>();
		pClone_py->_obj = this->_obj; //引用计数?
		IncreasePythonObject(pClone_py->_obj); //和init时一样, copy保证引用计数要+1
		pClone = (CSphTokenizerBase*)pClone_py;
	}

	pClone->CloneBase ( this, eMode );
	return pClone;
}



