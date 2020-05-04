/*
 * slip.c
 *
 *  Created on: 2020-4-10
 *      Author: lpk
 */
#include "slip.h"

/**
 * @brief  slip数据封包
 * @note   
 * @param  *buff_in: 要封包的数据
 * @param  size: 封包的数据长度
 * @param  *buff_out: 封包后的数据，输出参数
 * @retval 封包后的数据长度
 */
u32 SlipPacket(u8 *buff_in, u32 size, u8 *buff_out)
{
    u32 i = 0;
    u32 ret = 0;

    buff_out[ret++] = 0xc0;//清理噪音
    for(i = 0; i < size; i++){
        if(buff_in[i] == 0xc0){
            buff_out[ret++] = 0xdb;
            buff_out[ret++] = 0xdc;
        }else if(buff_in[i] == 0xdb){
            buff_out[ret++] = 0xdb;
            buff_out[ret++] = 0xdd;
        }else{
            buff_out[ret++] = buff_in[i];
        }
    }
    buff_out[ret++] = 0xc0;

    return ret;
}

/**
 * @brief  SLIP数据解码
 * @note   
 * @param  *buff_in: 要解码的数据
 * @param  size: 要解码的数据长度
 * @param  *buff_out: 解码输出的数据，输出参数
 * @param  *size_out: 解码后的数据长度，输出0表示未解码到有效数据包，输出参数
 * @retval 解码到一包数据后，buff_in指针的位置，
 * 当未解码到有效数据时，buff_in位置不做更改，防止破坏原数据包
 */
u8 *SlipUnpacket(u8 *buff_in, u32 size, u8 *buff_out, u32 *size_out)
{
    u32 i = 0;
    int state = 0;//解码状态

    *size_out = 0;
    for(i = 0; i < size; i++){
        if(buff_in[i] == 0xc0){//检测到结束符
            if(*size_out > 0){//数据长度大于0
                state = 1;
                i++;
                break;
            }
        }else if(buff_in[i] == 0xdb){//检测到特殊字符
            if(i + 1 >= size){//数据越界
                break;
            }
            if(buff_in[i + 1] == 0xdc){//检测到转义字符0xc0
                buff_out[(*size_out)++] = 0xc0;
                i++;
            }else if(buff_in[i + 1] == 0xdd){//检测到转义字符0xdb
                buff_out[(*size_out)++] = 0xdb;
                i++;
            }else{
                break;
            }
        }else{
            buff_out[(*size_out)++] = buff_in[i];
        }
    }

    if(state){//解析到整包数据
        buff_in += i;
    }else{
        *size_out = 0;
    }
    
    return buff_in;
}


