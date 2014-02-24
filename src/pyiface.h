#ifndef PYIFACECSFT_H
#define PYIFACECSFT_H

#include <vector>
#include "sphinx.h"
#include "sphinxutils.h"

/*
 * 在 C++ 和 Python 层 中间, 提供一个适配层
 * C++ Object member function                           Cython Side
 *  -> Pass Cython instance as a parameter.
 *  -> Cython API with C-level function proto-type
 *                                                      -> Recast To Cython Class Object
 *                                                      -> Call object's method.
 *  -> The return value.
 */

//------ Python Configure Block -------
class PySphConfig
{
    /* The Python Interface of CSphConfig
     *  - 不可以删除系统中已经存在的配置节
     *  - 可以对已经存在的配置节 添加 | 改写?
     *  - 可以创建新配置节
     *  - 不可以删除自己的配置节
     */
public:
    PySphConfig(CSphConfig& conf);

public:
    bool hasSection(const char* sType, const char* sName);
    bool addSection(const char* sType, const char* sName);
    bool addKey(const char* sType, const char* sName, const char* sKey, char* sValue);

    const CSphString &		GetErrorMessage () const	{ return m_sError; }
protected:
    //char			m_sError [ 1024 ];
    CSphString	m_sError;
private:
    CSphConfig& _conf;
};

class IConfProvider
{
public:
  IConfProvider();
  virtual ~IConfProvider();
  /*
   *
   */
  virtual int process(CSphConfig & hConf) = 0;
  virtual const CSphString & GetErrorMessage () const;
};

class ConfProviderWrap : public IConfProvider
{
public:
  PyObject *obj;
  // RunFct fct;

  ConfProviderWrap(PyObject *obj):obj(obj) {} //, RunFct fct);
  virtual ~ConfProviderWrap();

  virtual int process(CSphConfig & hConf);
};


//------ Python Data Source Block -------
typedef CSphVector<CSphString> CSphStringList;

void initColumnInfo(CSphColumnInfo& info, const char* sName, const char* sType);
void setColumnBitCount(CSphColumnInfo& info, int iBitCount);
int  getColumnBitCount(CSphColumnInfo& info);
void setColumnAsMVA(CSphColumnInfo& info, bool bJoin);
int addFieldColumn(CSphSchema* pSchema, CSphColumnInfo& info);
int  getSchemaFieldCount(CSphSchema* pSchema);
CSphColumnInfo* getSchemaField(CSphSchema* pSchema, int iIndex);

uint32_t getCRC32(const char* data, size_t iLength);

uint32_t getConfigValues(const CSphConfigSection & hSource, const char* sKey, CSphStringList& value);

CSphSource * SpawnSourcePython ( const CSphConfigSection & hSource, const char * sSourceName);


class PySphMatch
{
public:
    PySphMatch():_m(NULL),_s(NULL) {}
    /*
     *  虽然接口上支持 CSphSource, 实际只能传入 CSphSource_Python2;
     */
    void bind(CSphSource* s, CSphMatch* m) { _s = s; _m = m; }
public:
    inline void setDocID(uint64_t id) {     _m->m_iDocID = (SphDocID_t)id;   }
    inline uint64_t getDocID() {    return _m->m_iDocID;    }
    int  getAttrCount();
    int  getFieldCount();
    void setAttr ( int iIndex, SphAttr_t uValue ) ;
    void setAttrFloat ( int iIndex, float fValue );
    void setAttrInt64( int iIndex, int64_t uValue ) ;

    //void pushMva( int iIndex, std::vector<DWORD>& values);
    int pushMva( int iIndex, std::vector<int64_t>& values, bool bMva64);

    // string related
    void setAttrString( int iIndex, const char* s);
    void setField( int iIndex, const char* utf8_str);

protected:
private:
    CSphMatch* _m;
    CSphSource* _s;
};

//------ Python Tokenizer Block -------


//------ Python Cache Block -------


//------ Python Query Block -------

#endif // PYIFACECSFT_H
