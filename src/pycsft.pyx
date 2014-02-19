# -*- coding: UTF-8 -*-

cimport pycsft
cimport cpython.ref as cpy_ref
from cpython.ref cimport Py_INCREF, Py_DECREF, Py_XDECREF
import os, sys

import traceback
import cython

#from libcpp.string cimport string
from libc.string cimport const_char, const_uchar
"""
    定义
        Python 数据源
        Python 分词法
        Python 配置服务
        Python Cache 的 C++ <-> Python 的调用接口
"""
# Ref: http://stackoverflow.com/questions/1176136/convert-string-to-python-class-object
def __findPythonClass(sName):
    import importlib
    pos = sName.find('.')
    module_name = sName[:pos]
    cName = sName[pos+1:]
    try:
        m = importlib.import_module(module_name)
        c = getattr(m, cName)
        return c
    except ImportError, e:
        print e, "python class init error"
        #print 'can not find python class'
        #exit(0)
        return None

# Cython creator API
cdef public api void __setPythonPath(const char* sPath):
    import sys
    sPaths = [x.lower() for x in sys.path]
    sPath_ = os.path.abspath(sPath)
    if sPath_ not in sPaths:
        sys.path.append(sPath_)
    #print __findPythonClass('flask.Flask')

cdef public api cpy_ref.PyObject* __getPythonClassByName(const char* class_name):
    import sys
    sName = class_name
    clsType = __findPythonClass(sName)
    if clsType:
        return ( <cpy_ref.PyObject*>clsType)
    else:
        return NULL

"""
    Import C++ Interface
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

#cdef extern from "sphinxstd.h":
#    typedef unsigned char BYTE

## --- python cache ---

## --- python query ---

"""
    Define Python Wrap, for Python side.
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

## --- python cache ---

## --- python query ---

"""
    Define Python Wrap , for CPP side.
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

# cdef class PyTokenWrap(object):
#     """
#     提供一个 返回给pytoken的c风格的类, 方便指针转换
#     """
#     cdef object pytoken
#     def __init__(self, pytoken):
#         self.pytoken = pytoken

cdef class PyTokenWrap:
    cdef object pytoken
    def __init__(self, pytoken):
        self.pytoken = pytoken 

## --- python cache ---

## --- python query ---


"""
    Python Object's C API
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

cdef public api void pyTokenSetBuffer( cpy_ref.PyObject* pyobj, const_uchar* words, int ilength ):
    #cdef string words

    print "###here in pyTokenSetBuffer"
    pytoken = <object>pyobj
    pytoken1 = <object>pyobj
    pytoken2 = <object>pyobj
    print pytoken, sys.getrefcount(pytoken)

    pywords = words[:ilength]
    print '@@in cython', pywords

    pytoken.SetBuffer(pywords)
    terms = pytoken.GetToken()
    for i in terms:
        print i,

cdef public api void pyTokenEcho( cpy_ref.PyObject* pyobj ):
    print "in pyTokenEcho"
    

## --- python cache ---

## --- python query ---


"""
    Object creation function.
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

# cdef public api ISphTokenizer * createPythonTokenizerObject ( const char* python_path):
#     cdef CSphTokenizer_Python[False]* pyToken  #here is a indexer token IS_QUERY is false

#     clsType = __findPythonClass(python_path)
#     print "i got python token\n"
#     if clsType:
#         try:
#             obj = clsType()
#         except Exception, ex:
#             traceback.print_exc()
#             return NULL

#         pyToken = new CSphTokenizer_Python[False] ();
#         pyToken.bind(obj)
#         return <ISphTokenizer *> pyToken
#     else:
#         return NULL

cdef public api cpy_ref.PyObject* createPythonTokenizerObject( const char* python_path):
    cdef PyTokenWrap pywrap
    clsType = __findPythonClass(python_path)
    if clsType:
        print "i got python token class\n"
        try:
            obj=clsType()
            #print obj.test()
            #pywrap = new PyTokenWrap(obj) #把obj放置在一个wrap的class中, 避免指针转换 这是一个指针
            #pywrap = PyTokenWrap(obj)
            #print pywrap.pytoken.test()
            return <cpy_ref.PyObject*>obj  #convert the obj to PyObject*
            #return cython.address(pywrap)
        except Exception, e:
            #traceback.print_exc()
            print e
            return NULL
    return NULL


## --- python cache ---

## --- python query ---


#end of file
