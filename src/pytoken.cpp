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
	printf("in ~CSphTokenizer_Python\n");
	if (this->_obj)
	{
		//防止内存泄露, +1的obj必须减1
		Py_XDECREF(this->_obj);
	}
}

//template < bool IS_QUERY >
//void CSphTokenizer_Python<IS_QUERY>::bind(PyObject *obj)
//{
//	printf("token bind together");
//	this->_obj = obj;
//}

template < bool IS_QUERY >
void CSphTokenizer_Python<IS_QUERY>::init(const char  *python_path)
{
	printf("%s", python_path );
	//obj is a python class object pointer
	PyObject* obj = createPythonTokenizerObject(python_path);
	if ( obj )
	{
		printf("got the obj successfully\n");
		this->_obj= obj;
		//这里必须增加一个_obj的引用, 因为参数传递是一个借用引用, 不加1的话 _obj可能在运行中被gc
		Py_INCREF(this->_obj);
		Py_INCREF(this->_obj); // may cause memory leek
	}else{
		printf("got the pytoken obj error\n");
	}
}

template < bool IS_QUERY >
void CSphTokenizer_Python<IS_QUERY>::SetBuffer(BYTE * sBuffer, int iLength)
{
	printf ("in setbuffer\n");
	CSphTokenizer_UTF8<IS_QUERY>::SetBuffer(sBuffer, iLength);
	//std::string words;
	//words.assign((char*)sBuffer, iLength); //用iLength个字符串给words赋值

	//Py_INCREF(this->_obj); // may cause memory leek
	pyTokenSetBuffer(this->_obj, sBuffer, iLength); //给python端设定好词汇,然后直接得到结果. 同步接口.

}

template < bool IS_QUERY >
BYTE * CSphTokenizer_Python<IS_QUERY>::GetToken()
{
	printf ("in gottoken\n");
	return CSphTokenizer_UTF8<IS_QUERY>::GetToken();
}



