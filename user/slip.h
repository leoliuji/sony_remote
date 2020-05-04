/*
 * slip.h
 *
 *  Created on: 2020-4-10
 *      Author: lpk
 */

#ifndef SLIP_SLIP_H_
#define SLIP_SLIP_H_

#include "string.h"
#include "lpk_type.h"
#include "lpk_util.h"

/**
 * @brief  slip数据封包
 * @note   
 * @param  *buff_in: 要封包的数据
 * @param  size: 封包的数据长度
 * @param  *buff_out: 封包后的数据，输出参数
 * @retval 封包后的数据长度
 */
u32 SlipPacket(u8 *buff_in, u32 size, u8 *buff_out);

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
u8 *SlipUnpacket(u8 *buff_in, u32 size, u8 *buff_out, u32 *size_out);

#endif /* SLIP_SLIP_H_ */
