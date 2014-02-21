#include "pytoken.h"
#include "pycsft.h"
#include "string"


#define PYSOURCE_DEBUG 0

template < bool IS_QUERY >
CSphTokenizer_Python<IS_QUERY>::CSphTokenizer_Python()
	:CSphTokenizer_UTF8<IS_QUERY>::CSphTokenizer_UTF8 ()
{
	//mimic the mmseg token
	//this->m_pAccumSeg = m_sAccumSeg;

	//m_iLastTokenBufferLen = 0;
	//m_iLastTokenLenMMSeg = 0;
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
		printf("#in init\n#python_path: %s\n", python_path);

	PyObject* obj = createPythonTokenizerObject(python_path);
	if ( obj )
	{
		if (PYSOURCE_DEBUG)
			printf("#got thef token obj successfully\n");
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
	//mimic mmseg. 为了查询解析, 需要更新父类的成员变量
	CSphTokenizer_UTF8<IS_QUERY>::SetBuffer(sBuffer, iLength);

	pyTokenProcess(this->_obj, sBuffer, iLength); //给python端设定好词汇,然后直接得到结果. 同步接口.

	pyGotAllResult(this->_obj, &SegmentOffset); //获取结果,目前填充所有的issegment值即可. 暂时不考虑同义词接口

	if (PYSOURCE_DEBUG){
		std::set<int>::iterator si;
		for (si = SegmentOffset.begin(); si != SegmentOffset.end(); si++){
			printf("%d, ", *si);
		}
	}


//	m_segoffset = 0;
//	m_segToken = (char*)m_pCur;

}

template < bool IS_QUERY >
bool CSphTokenizer_Python<IS_QUERY>::IsSegment(const BYTE * pCur)
{
	int offset = pCur - m_pBuffer;
	if (SegmentOffset.find(offset) != SegmentOffset.end() )
		return true;
	return false;
}

template < bool IS_QUERY >
BYTE * CSphTokenizer_Python<IS_QUERY>::GetToken()
{
//	m_iLastTokenLen = 0;

//	int iLength = SPH_MAX_WORD_LEN;
//	int* p_ilength = &iLength;
//	int ret = pyTokenGetToken(this->_obj, m_sAccumSeg, p_ilength);
//	if (ret)
//		return NULL;
//	{
//		m_sAccumSeg[iLength] = '\0'; // c风格的字符串.
//		printf( "%s/x ", m_sAccumSeg);
//		m_iLastTokenBufferLen = iLength;
//		return m_sAccumSeg;
//	}
	m_iLastTokenLenMMSeg = 0;
	//BYTE* tok = CSphTokenizer_UTF8::GetToken();
	while(!IsSegment(m_pCur) || m_pAccumSeg == m_sAccumSeg)
	{
		BYTE* tok = CSphTokenizer_UTF8::GetToken();
		if(!tok){
			m_iLastTokenLenMMSeg = 0;
			return NULL;
		}

		if(m_pAccumSeg == m_sAccumSeg)
			m_segToken = (char*)m_pTokenStart;

		if ( (m_pAccumSeg - m_sAccumSeg)<SPH_MAX_WORD_LEN )  {
			::memcpy(m_pAccumSeg, tok, m_iLastTokenBufferLen);
			m_pAccumSeg += m_iLastTokenBufferLen;
			m_iLastTokenLenMMSeg += m_iLastTokenLen;
		}
	}
	{
		*m_pAccumSeg = 0;
		m_iLastTokenBufferLen = m_pAccumSeg - m_sAccumSeg;
		m_pAccumSeg = m_sAccumSeg;

		//m_segToken = (char*)(m_pTokenEnd-m_iLastTokenBufferLen);
		return m_sAccumSeg;
	}
}

template < bool IS_QUERY >
const BYTE * CSphTokenizer_Python<IS_QUERY>::GetExtend()
{
//	int iLength = SPH_MAX_WORD_LEN;
//	int i_cnt_max_extend = 3; //get at most extend 3

//	int ret = pyTokenGetExtend(this->_obj, m_sAccumSeg, &iLength, &i_cnt_max_extend);
//	if (ret)
//		return NULL;
//	m_sAccumSeg[iLength] = '\0'; // c风格的字符串.
//	//printf( "%s/x ", m_sAccumSeg);
//	return m_sAccumSeg;
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



