#ifndef RS_RESULT_H
#define RS_RESULT_H

#include "rosettastd.h"
#include "stdint.h"

/*
 *  处理分词结果的 C 语言标准
 *  分词结果分三个部分:
 *  - 索引区;  记录 iCode 对应有那几个标注
 *  - 标注区;  记录 标注的详细信息 32bit
 *  - 同义词区; 记录标注扩展出的同义词
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {} swresult;

// 创建新的结果集
void swr_init(int iSize, swresult** ppRs);
// 清除内存
void swr_release(swresult * pRs);

// 得到序列化后需要的大小
uint64_t swr_get_binary_size(swresult * pRs);

/*
 *  标注格式:
 *  8bit 词性标注信息, 实际使用 7 bit, 最高位如为 1, 则解释不同
 *  8bit 标注长度; 如果最高位为1, 这后续的 32bit 也是标注长度;
 *  8bit 标注类型: 英文e | 数字n | 成语i | 网址w | 电子邮件m | 地址a | 人名r | 组织机构o | 时间t
 *  8bit 高3 bit nBest 的掩码, 表示该 annote 被那个路径使用; 低 4 bit 词库 id (最多 16 个 )
 *
 *  当词性标注信息全为 1 时 余下的 24bit 表示在同义词区的偏移量, 不能超过 16M
 *  [del]同义词的标注, 在全部标注的最后面; 一位置, 只有一个同义词 annote[/del:不必要的限制]
 */
int swr_set_annote(swresult * pRs, int iPos, int iTermLength, uint8_t iNerTag, uint8_t iPoSpeech, uint8_t iDictID, uint8_t nBestMask);
int swr_set_synonyms(swresult * pRs, int iPos, const char* ppTerms[], const int ppTermsLength[], int iCount);

// 加载序列化后的二进制文件
int swr_load(swresult * pRs, BYTE* buf, uint64_t len);
// 用 nextToken 的方式读取分词结果
// 释放结果集

#ifdef __cplusplus
}
#endif

#endif // RS_RESULT_HPP
