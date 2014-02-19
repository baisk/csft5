# -*- coding: UTF-8 -*-

cimport pycsft
cimport cpython.ref as cpy_ref
from cpython.ref cimport Py_INCREF, Py_XINCREF, Py_DECREF, Py_XDECREF, Py_CLEAR
import os, sys

import traceback
import cython

#from libc.stdlib cimport memcpy

#from libc.string cimport const_char, const_uchar  #可以被BYTE代替(从c文件中拿到)
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

cdef extern from "sphinxstd.h":
    ctypedef unsigned char   BYTE

cdef extern from "string.h" nogil:
    void *memcpy  (void *TO, const void *FROM, size_t SIZE)

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

cdef class pyTokenWrap:  #需要一个python 的包装,减少python和c端的复杂度
    cdef object pytoken
    cdef object document #待分词的文档
    cdef object terms    #分完词的term列表, list, 每个item是一个分词结果
    cdef object terms_len #分词长度
    cdef object term_idx
    cdef object ret
    cdef object adv

    def __init__(self,pytoken, adv=False):
        self.pytoken = pytoken
        self.document = None
        self.terms = None
        self.terms_len = 0
        self.term_idx = 0
        self.adv =  adv

    def Process(self, pywords):
        self.document = pywords
        ret = self.pytoken.SetDocument(pywords)
        if not ret:
            print "!!!!!!!! pytoken set document error"

        if self.adv:
            self.terms = self.pytoken.GetAdvToken()
        else:
            self.terms = self.pytoken.GetToken()

        if not self.terms:
            print "!!!!!!!! get token from pytoken error"
        else:
            self._clear() #clear the terms
            self.terms_len = len(self.terms)
            self.term_idx = 0

    def GetTokenNext(self):
        if self.term_idx == self.terms_len:
            return None
        else:
            if self.adv:
                term = self.terms[self.term_idx][0]
            else:
                term = self.terms[self.term_idx]
            self.term_idx += 1
            return term
        return None

    def GetExtend(self): #返回的是上一个idx对应的扩展
        # 应该在 1-self.terms_len 之间
        if not self.adv: return None
        if self.term_idx < 1 or self.term_idx > self.terms_len:
            return None
        else:
            return self.terms[self.term_idx-1][1]  #默认第二项是同义词扩展

    def _clear(self):
        pass #暂时一些清理工作可以在python的分词法端去做. ? 以后的分词法接口肯定要求在外端做完. ?

## --- python cache ---

## --- python query ---


"""
    Python Object's C API
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

cdef public api void pyTokenProcess( cpy_ref.PyObject* pyobj, BYTE* words, int ilength ):
    cdef object pytokenwrap
    cdef object pywords

    pytokenwrap = <object>pyobj
    pywords = words[:ilength].decode('UTF-8', 'strict') #转成unicode内码, 如果不转,则交给分词法处理.
    pytokenwrap.Process(pywords)



cdef public api int pyTokenGetToken( cpy_ref.PyObject* pyobj, BYTE* words, int* ptr_len  ):
    cdef object pytokenwrap
    cdef object pyterm
    cdef BYTE* c_str_tmp
    cdef int c_str_len
    cdef int maxLength

    maxLength = ptr_len[0]
    pytokenwrap = <object>pyobj
    pyterm = pytokenwrap.GetTokenNext()
    if pyterm:
        if len(pyterm) > maxLength: pyterm = pyterm[:maxLength]  #最长取前ilength个, 这里的个数是unicode值,变成utf8会是3倍(最多)长.
        py_byte_string = pyterm.encode('UTF-8')  #必须设置一个python的obj 直接赋值给char* 会因为缺少引用计数而编译错误
        c_str_tmp = py_byte_string
        c_str_len = len(py_byte_string)
        memcpy(words, c_str_tmp, c_str_len) #把值复制给words 结果传递给sphinx那端
        ptr_len[0] =  c_str_len # 表明str的长度.
        return 0 #in c, 0 mean success
    else:
        return 1 #term is ended

#c和python之间的函数切换会带来巨大开销 !?
cdef public api int pyTokenGetExtend( cpy_ref.PyObject* pyobj, BYTE* words, int* ptr_len, int* p_cnt_len  ):
    cdef object pytokenwrap
    cdef object pyterm
    cdef BYTE* c_str_tmp
    cdef int c_str_len
    cdef int maxLength
    cdef int maxCount
    cdef object extends
    cdef int idx, idx_cnt

    maxLength = ptr_len[0]
    maxCount = p_cnt_len[0]
    pytokenwrap = <object>pyobj
    extends = pytokenwrap.GetExtend() #获得的都是上一个token对应的扩展
    if extends:
        idx = 0
        idx_cnt = 0
        for ex in extends[:maxCount]:
            py_byte_string = ex.encode('UTF-8')
            c_str_tmp = py_byte_string
            c_str_len = len(py_byte_string)
            if idx + c_str_len > maxLength:
                break
            memcpy(words+idx, c_str_tmp, c_str_len) #把值复制给words 结果传递给sphinx那端
            idx += c_str_len
            idx_cnt += 1

        ptr_len[0], p_cnt_len[0] = idx, idx_cnt        
        return 0
    else:
        return 1


## --- python cache ---

## --- python query ---

"""
    Object creation function.
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

cdef public api cpy_ref.PyObject* createPythonTokenizerObject( const char* python_path):
    cdef cpy_ref.PyObject* ptr
    clsType = __findPythonClass(python_path)
    if clsType:
        # print "i got python token class\n"
        try:
            obj=clsType()
            wrap = pyTokenWrap(obj, adv=True) #use token to init wrap, adv could use Extend
            ptr = <cpy_ref.PyObject*>wrap  #convert the obj to PyObject*
            #Py_XINCREF(ptr)  #hold the ref of the obj
            IncreasePythonObject(ptr)
            return ptr
        except Exception, e:
            print 'create python token warp error:', e
            return NULL
    return NULL

cdef public api void IncreasePythonObject( cpy_ref.PyObject* obj ):
    Py_XINCREF(obj)  #hold the ref of the obj

cdef public api void DecreasePythonObject( cpy_ref.PyObject* obj ):
    Py_CLEAR(obj);  #decrease the ref of the obj 推荐使用的函数clear

## --- python cache ---

## --- python query ---

#end of file
