/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

#include <mach/port.h>
#include <mach/message.h>

#define LIBMACH_OPTIONS    (MACH_SEND_INTERRUPT|MACH_RCV_INTERRUPT)
/*
 http://web.mit.edu/darwin/src/modules/xnu/osfmk/man/mach_msg.html
 mach_msg_return_t   mach_msg
 (mach_msg_header_t                msg,
 mach_msg_option_t             option,
 mach_msg_size_t            send_size,
 mach_msg_size_t        receive_limit,
 mach_port_t             receive_name,
 mach_msg_timeout_t           timeout,
 mach_port_t                   notify);
 
 mach_msg_return_t   mach_msg_overwrite
 (mach_msg_header_t*          send_msg,
 mach_msg_option_t             option,
 mach_msg_size_t            send_size,
 mach_msg_size_t        receive_limit,
 mach_port_t             receive_name,
 mach_msg_timeout_t           timeout,
 mach_port_t                   notify,
 mach_msg_header_t       *receive_msg,
 mach_msg_size_t     receive_msg_size);
 
System Trap / Function - Send and/or receive a message from the target port.
 发送/接受信息
 
 msg:
 [pointer to in/out structure containing random and reply rights] A message buffer used by mach_msg both for send and receive. This must be naturally aligned.
 [指向包含随机和回复权限的输入/输出结构的指针] mach_msg用于发送和接收的消息缓冲区。这必须是自然对齐的。
 send_msg:
 [pointer to in structure containing random and reply rights] The mes- sage buffer to be sent. This must be naturally aligned
 要发送的消息缓冲区。这必须是自然对齐的。
 option:
 [in scalar] Message options are bit values, combined with bitwise-or. One or both of MACH_SEND_MSG and MACH_RCV_MSG should be used. Other options act as modifiers.
 消息选项是位值，与按位或组合。应使用MACH_SEND_MSG和MACH_RCV_MSG中的一个或两个。其他选项充当修饰符。
 send_size:
 [in scalar] When sending a message, specifies the size of the message buffer to be sent (the size of the header and body) in bytes. Otherwise zero should be supplied.
 发送消息时，指定要发送的消息缓冲区的大小（标题和正文的大小），以字节为单位。否则应提供零。
 receive_limit:
 [in scalar] When receiving a message, specifies the maximum size of the msg or receive_msg buffer in bytes. Otherwise zero should be sup- plied.
 接收消息时，指定msg或receive_msg缓冲区的最大大小（以字节为单位）。否则应该提供零。
 receive_name:
 [in random right] When receiving a message, specifies the port or port set. Otherwise MACH_PORT_NULL should be supplied.
 接收消息时，指定端口或端口集。否则应提供MACH_PORT_NULL。
 timeout:
 [in scalar] When using the MACH_SEND_TIMEOUT and MACH_RCV_TIMEOUT options, specifies the time in milliseconds to wait before giving up. Otherwise MACH_MSG_TIMEOUT_NONE should be supplied.
 使用MACH_SEND_TIMEOUT和MACH_RCV_TIMEOUT选项时，指定放弃前等待的时间（以毫秒为单位）。否则应提供MACH_MSG_TIMEOUT_NONE。
 notify:
 [in notify receive right] When using the MACH_SEND_CANCEL and MACH_RCV_NOTIFY options, specifies the port used for the notification. Otherwise MACH_PORT_NULL should be supplied.
 使用MACH_SEND_CANCEL和MACH_RCV_NOTIFY选项时，指定用于通知的端口。否则应提供MACH_PORT_NULL。
 receive_msg:
 [pointer to in/out structure] A message buffer into which a message (header and body) will be received. This must be naturally aligned. By default (mach_msg), any received message will overwrite the send message buffer. This buffer is in/out only if the MACH_RCV_OVERWRITE option is used; otherwise this buffer is out only.
 将接收消息（标题和正文）的消息缓冲区。这必须是自然对齐的。默认情况下（mach_msg），任何收到的消息都将覆盖发送消息缓冲区。仅当使用MACH_RCV_OVERWRITE选项时，此缓冲区才会进/出; 否则此缓冲区仅出局。
 receive_msg_size:
 [in scalar] When using the MACH_RCV_OVERWRITE option, specifies the size (in bytes) of the receive "message" that is to be used by mach_msg to indicate the disposition of received out-of-line regions.
 使用MACH_RCV_OVERWRITE选项时，指定mach_msg用于指示接收到的外线区域的处理的接收“消息”的大小（以字节为单位） 。
 返回值:
 The send operation can generate the following return codes. These return codes imply that the call did nothing:
 
 MACH_SEND_MSG_TOO_SMALL
 The specified send_size was smaller than the minimum size for a message.
 MACH_SEND_NO_BUFFER
 A resource shortage prevented the kernel from allocating a message buffer.
 MACH_SEND_INVALID_DATA
 The supplied message buffer was not readable.
 MACH_SEND_INVALID_HEADER
 The msgh_bits value was invalid.
 MACH_SEND_INVALID_DEST
 The msgh_remote_port value was invalid.
 MACH_SEND_INVALID_NOTIFY
 When using MACH_SEND_CANCEL, the notify argument did not denote a valid receive right.
 MACH_SEND_INVALID_REPLY
 The msgh_local_port value was invalid.
 MACH_SEND_INVALID_TRAILER
 The trailer to be sent does not correspond to the current kernel format, or the sending task does not have the privilege to supply the message attributes.
 These return codes imply that some or all of the message was destroyed:
 
 MACH_SEND_INVALID_MEMORY
 The message body specified out-of-line data that was not readable.
 MACH_SEND_INVALID_RIGHT
 The message body specified a port right which the caller didn't possess.
 MACH_SEND_INVALID_TYPE
 A kernel processed descriptor was invalid.
 MACH_SEND_MSG_TOO_SMALL
 The last data item in the message ran over the end of the message.
 These return codes imply that the message was returned to the caller with a pseudo-receive operation:
 
 MACH_SEND_TIMED_OUT
 The timeout interval expired.
 MACH_SEND_INTERRUPTED
 A software interrupt occurred.
 This return code implies that the message was queued:
 
 MACH_MSG_SUCCESS
 The message was queued.
 The receive operation can generate the following return codes. These return codes imply that the call did not de-queue a message:
 
 MACH_RCV_INVALID_NAME
 The specified receive_name was invalid.
 MACH_RCV_IN_SET
 The specified port was a member of a port set.
 MACH_RCV_TIMED_OUT
 The timeout interval expired.
 MACH_RCV_INTERRUPTED
 A software interrupt occurred.
 MACH_RCV_PORT_DIED
 The caller lost the rights specified by receive_name.
 MACH_RCV_PORT_CHANGED
 receive_name specified a receive right which was moved into a port set during the call.
 MACH_RCV_TOO_LARGE
 When using MACH_RCV_LARGE, the message was larger than receive_limit. The message is left queued, and its actual size is returned in the message header/message body.
 MACH_RCV_SCATTER_SMALL
 When using MACH_RCV_LARGE with MACH_RCV_OVERWRITE, one or more scatter list descriptors specified an overwrite region smaller than the corresponding incoming region. The message is left queued, and the proper descriptors are returned in the message header/message body.
 MACH_RCV_INVALID_TRAILER
 The trailer type desired, or the number of trailer elements desired, is not supported by the kernel.
 These return codes imply that a message was de-queued and destroyed:
 
 MACH_RCV_HEADER_ERROR
 A resource shortage prevented the reception of the port rights in the message header.
 MACH_RCV_INVALID_NOTIFY
 When using MACH_RCV_NOTIFY, the notify argument did not denote a valid receive right.
 MACH_RCV_INVALID_DATA
 The specified message buffer was not writable.
 MACH_RCV_TOO_LARGE
 When not using MACH_RCV_LARGE, a message larger than receive_limit was de-queued and destroyed.
 MACH_RCV_SCATTER_SMALL
 When not using MACH_RCV_LARGE with MACH_RCV_OVERWRITE, one or more scatter list descriptors specified an overwrite region smaller than the corresponding incoming region. The message was de-queued and destroyed.
 MACH_RCV_OVERWRITE_ERROR
 A region specified by a receive overwrite descriptor (MACH_RCV_OVERWRITE) was not allocated or could not be written.
 MACH_RCV_INVALID_TYPE
 When using MACH_RCV_OVERWRITE, one or more scatter list descriptors did not have the type matching the corresponding incoming message descriptor or had an invalid copy (disposition) field.
 MACH_RCV_LIMITS
 The combined size of all out-of-line memory regions or the total num- ber of port rights in the message exceeds the limit set for the port. These return codes imply that a message was received:
 MACH_RCV_BODY_ERROR
 A resource shortage prevented the reception of a port right or out-of- line memory region in the message body.
 MACH_MSG_SUCCESS
 A message was received.
 消息出队后可能会发生资源短缺，同时将端口权限和外部内存区域传输到接收任务。在这种情况下，mach_msg调用返回MACH_RCV_HEADER_ERROR或MACH_RCV_BODY_ERROR。这些返回码总是带有额外的位（按位或按位），表明资源短缺的性质：
 
 MACH_MSG_IPC_SPACE
 任务的IPC名称空间中没有空间用于其他端口名称。
 MACH_MSG_VM_SPACE
 任务的VM地址空间中没有空间用于外部存储区域。
 MACH_MSG_IPC_KERNEL
 内核资源短缺阻止了端口权限的接收。
 MACH_MSG_VM_KERNEL
 内核资源短缺阻止了外部存储区域的接收。
 */
mach_msg_return_t
mach_msg(msg, option, send_size, rcv_size, rcv_name, timeout, notify)
mach_msg_header_t *msg;
mach_msg_option_t option;
mach_msg_size_t send_size;
mach_msg_size_t rcv_size;
mach_port_t rcv_name;
mach_msg_timeout_t timeout;
mach_port_t notify;
{
    mach_msg_return_t mr;
    
    /*
     * Consider the following cases:
     *    1) Errors in pseudo-receive (eg, MACH_SEND_INTERRUPTED
     *    plus special bits).
     *    2) Use of MACH_SEND_INTERRUPT/MACH_RCV_INTERRUPT options.
     *    3) RPC calls with interruptions in one/both halves.
     *
     * We refrain from passing the option bits that we implement
     * to the kernel.  This prevents their presence from inhibiting
     * the kernel's fast paths (when it checks the option value).
     */
    
    mr = mach_msg_overwrite_trap(msg, option &~ LIBMACH_OPTIONS,
                                 send_size, rcv_size, rcv_name,
                                 timeout, notify, MACH_MSG_NULL, 0);
    if (mr == MACH_MSG_SUCCESS)
    return MACH_MSG_SUCCESS;
    
    if ((option & MACH_SEND_INTERRUPT) == 0)
    while (mr == MACH_SEND_INTERRUPTED)
    mr = mach_msg_overwrite_trap(msg,
                                 option &~ LIBMACH_OPTIONS,
                                 send_size, rcv_size, rcv_name,
                                 timeout, notify, MACH_MSG_NULL, 0);
    
    if ((option & MACH_RCV_INTERRUPT) == 0)
    while (mr == MACH_RCV_INTERRUPTED)
    mr = mach_msg_overwrite_trap(msg,
                                 option &~ (LIBMACH_OPTIONS|MACH_SEND_MSG),
                                 0, rcv_size, rcv_name,
                                 timeout, notify, MACH_MSG_NULL, 0);
    
    return mr;
}

mach_msg_return_t
mach_msg_overwrite(msg, option, send_size, rcv_limit, rcv_name, timeout, 
                   notify, rcv_msg, rcv_msg_size)
mach_msg_header_t *msg;
mach_msg_option_t option;
mach_msg_size_t send_size;
mach_msg_size_t rcv_limit;
mach_port_t rcv_name;
mach_msg_timeout_t timeout;
mach_port_t notify;
mach_msg_header_t *rcv_msg;
mach_msg_size_t rcv_msg_size;
{
    mach_msg_return_t mr;
    
    /*
     * Consider the following cases:
     *    1) Errors in pseudo-receive (eg, MACH_SEND_INTERRUPTED
     *    plus special bits).
     *    2) Use of MACH_SEND_INTERRUPT/MACH_RCV_INTERRUPT options.
     *    3) RPC calls with interruptions in one/both halves.
     *
     * We refrain from passing the option bits that we implement
     * to the kernel.  This prevents their presence from inhibiting
     * the kernel's fast paths (when it checks the option value).
     */
    
    mr = mach_msg_overwrite_trap(msg, option &~ LIBMACH_OPTIONS,
                                 send_size, rcv_limit, rcv_name,
                                 timeout, notify, rcv_msg, rcv_msg_size);
    if (mr == MACH_MSG_SUCCESS)
    return MACH_MSG_SUCCESS;
    
    if ((option & MACH_SEND_INTERRUPT) == 0)
    while (mr == MACH_SEND_INTERRUPTED)
    mr = mach_msg_overwrite_trap(msg,
                                 option &~ LIBMACH_OPTIONS,
                                 send_size, rcv_limit, rcv_name,
                                 timeout, notify, rcv_msg, rcv_msg_size);
    
    if ((option & MACH_RCV_INTERRUPT) == 0)
    while (mr == MACH_RCV_INTERRUPTED)
    mr = mach_msg_overwrite_trap(msg,
                                 option &~ (LIBMACH_OPTIONS|MACH_SEND_MSG),
                                 0, rcv_limit, rcv_name,
                                 timeout, notify, rcv_msg, rcv_msg_size);
    
    return mr;
}

