#ifndef RS_RESULT_HPP
#define RS_RESULT_HPP

#include <vector>
#include <iostream>
#include <sstream>
#include "rosettastd.h"

#define ENSURE_IPOS_IN_RANGE(x, y) { if(x<0 || x>=y) return -1; }
#define MAX_DATA_POSITION (2^24) //2^24 means 16M 额外提供16m的数据流供分词法使用


namespace Rosetta {

    typedef std::uint32_t annote_t;

    template <typename T>
    class SwResult {
    public:
        SwResult(int iSize)
            : _iLastPos(-1)
            , _annote_pos_index(NULL)
            , _size(iSize)
        {
            _annote_index = new BYTE[iSize];
            _annote_data_stream = new std::stringstream(std::ios::in | std::ios::out | std::ios::binary);
            if(_size > 4*4096)
                _annotes.reserve(_size*1.2);
            else
                _annotes.reserve(_size*2);
        }
        ~SwResult() {
            delete[] _annote_index;
            if(_annote_pos_index) delete[] _annote_pos_index;
            delete _annote_data_stream;
        }

        void reset() {
            memset(_annote_index, 0, sizeof(BYTE)*_size);
            if(_annote_pos_index)
                memset(_annote_pos_index, 0, sizeof(int)*_size);

            delete _annote_data_stream;
            _annote_data_stream = new std::stringstream(std::ios::in | std::ios::out | std::ios::binary);

            _annotes.clear();

            _iLastPos = -1;
        }

        //API
        int setAnnote(int iPos, T annote) {
            /*
             * 更新自身
             * 更新 iPos -> annote 的索引
             */
            typename std::vector<T>::iterator it = _annotes.begin();
            if(!_annote_pos_index)
                _annote_pos_index = new int[_size](); // new & init with 0

            _annote_index[iPos]++;
            int iOffset = _annote_pos_index[iPos];
            for(int i = iPos; i< _size; i++)
                _annote_pos_index[i] ++;
            _annotes.insert(it+iOffset, annote);
            return 0;
        }

        /*
         *  当 dataBuffer > 16M 时 return -2;
         */
        int setDataAnnote(int iPos, BYTE* pBuf, int iSize) {
            Rosetta::annote_t annote = 0xFF << 24;
            long pos = _annote_data_stream->tellp();
            if(pos > MAX_DATA_POSITION )
                return -2; //data buffer is full.
            annote |= (Rosetta::annote_t)pos;
            setAnnote(iPos, annote);
            _annote_data_stream->write((char*)pBuf, iSize);
            return 0;
        }

        //Batch API
        /*
         * 批处理接口, 保证 iPos 为升序;
         */
        int appendAnnote(int iPos, std::uint32_t annote) {
            ENSURE_IPOS_IN_RANGE(iPos, _size);
            _annote_index[iPos] ++;


        }

        int appendDataAnnote(int iPos, BYTE* pBuf, int iSize) {
            return 0;
        }

        size_t getSize() {
            // 返回序列化后,需要占用的字节数
            // iCount + count_index + annotes + data
            return sizeof(std::uint32_t) + sizeof(BYTE)* _size
                    + sizeof(T)*_annotes.size()
                    + _annote_data_stream->tellp();
        }

        int write(BYTE* pBuf, int iSize) {
            if(iSize < getSize()) return -1; //insufficent buffer.
            BYTE* ptr = pBuf;
            *ptr = _size;
            ptr += sizeof(_size);

            memcpy(ptr, _annote_index, sizeof(BYTE)*_size);
            ptr += sizeof(BYTE)*_size;

            memcpy(ptr, _annotes.data(), sizeof(T)*_annotes.size());
            ptr += sizeof(T)*_annotes.size();

            _annote_data_stream->read((char*)ptr, iSize - (ptr-pBuf));
            return 0;
        }

    public:
        // Debug Interface
        const BYTE * _get_annote_index() {
            return _annote_index;
        }

        const int * _get_annote_offset_index() {
            return _annote_pos_index;
        }

        const std::vector<T>& _get_annotes() {
            return _annotes;
        }

    private:
        int _iLastPos;
        int* _annote_pos_index; //create if needs.
    private:
        BYTE* _annote_index;
        int _size;
        std::stringstream* _annote_data_stream;
        std::vector<T> _annotes;
    };

    // should be uint32 | uint64
    typedef SwResult<annote_t> SimpleSwResult;
}

#endif // RS_RESULT_HPP
