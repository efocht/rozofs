/*
 Copyright (c) 2010 Fizians SAS. <http://www.fizians.com>
 This file is part of Rozofs.

 Rozofs is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published
 by the Free Software Foundation, version 2.

 Rozofs is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see
 <http://www.gnu.org/licenses/>.
 */
 
#ifndef ROZOFS_FUSE_THREAD_INTF_H
#define ROZOFS_FUSE_THREAD_INTF_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <rozofs/rozofs.h>
#include <rozofs/common/log.h>
#include <rozofs/core/af_unix_socket_generic.h>
#include <rozofs/core/rozofs_socket_family.h>
#include "rozofs_fuse_api.h"
#include "rozofs_sharedmem.h"

#define ROZOFS_MAX_FUSE_THREADS 8

typedef struct _rozofs_fuse_thread_stat_t {
  
  uint64_t            write_count;
  uint64_t            write_Byte_count;
  uint64_t            write_time;

} rozofs_fuse_thread_stat_t;
/*
** Fuse thread context
*/
typedef struct _rozofs_fuse_thread_ctx_t
{
  pthread_t                    thrdId; /* of fuse thread */
  int                          thread_idx;
  char                       * hostname;  
  int                          sendSocket;
  rozofs_fuse_thread_stat_t    stat;
} rozofs_fuse_thread_ctx_t;

extern rozofs_fuse_thread_ctx_t rozofs_fuse_thread_ctx_tb[];

/**
* Message sent/received in the af_unix disk sockets
*/

typedef enum _rozofs_fuse_thread_request_e {
  ROZOFS_FUSE_REPLY_BUF=1,
  ROZOFS_FUSE_THREAD_MAX_OPCODE
} rozofs_fuse_thread_request_e;

typedef struct _rozofs_fuse_thread_msg_t
{
  uint32_t            opcode;
  uint32_t            status;
  uint64_t            timeStart;
  uint64_t            timeResp;
  uint32_t            size;       /**< size of the buffer   */
  fuse_req_t          req ;       /**< fuse initial request   */
  void                *payload;   /**< pointer to the buffer payload   */
  void                *bufRef;   /**< shared memory buffer reference   */
} rozofs_fuse_thread_msg_t;

/*__________________________________________________________________________
* Initialize the fuse thread interface
*
* @param hostname    storio hostname (for simulation)
* @param nb_threads  Number of threads that can process the fuse replies
*
*  @retval 0 on success -1 in case of error
*/
int rozofs_fuse_thread_intf_create(char * hostname, int instance_id, int nb_threads) ;

/*__________________________________________________________________________
*/
/**
*  Send a reply buffer to a fuse thread
*
* @param fidCtx     FID context
* @param rpcCtx     pointer to the generic rpc context
* @param timeStart  time stamp when the request has been decoded
*
* @retval 0 on success -1 in case of error
*  
*/
int rozofs_thread_fuse_reply_buf(fuse_req_t req,
                                 char *payload,
				 uint32_t size,
				 void *bufRef,
				 uint64_t       timeStart);
/*
**__________________________________________________________________________
*/
/**
*  Thar API is intended to be used by a fuse thread for sending back a 
   fuse reply buffer response towards the main thread in order to release the 
   shared memory buffer
   
   @param thread_ctx_p: pointer to the thread context (contains the thread source socket )
   @param msg: pointer to the message that contains the disk response
   @param status : status of the disk operation
   
   @retval none
*/
void rozofs_fuse_th_send_response (rozofs_fuse_thread_ctx_t *thread_ctx_p, rozofs_fuse_thread_msg_t * msg, int status);

#endif
