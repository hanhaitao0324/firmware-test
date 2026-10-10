/*----------------------------------------------------------------------------
 *      R T L  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    RTX_CONFIG.H
 *      Purpose: Exported functions of RTX_Config.c
 *      Rev.:    V3.22
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#if (defined(__TARGET_ARCH_6S_M) || defined(__TARGET_ARCH_7_M))
#define __RTX_CONFIG_H__
#endif

#ifndef __RTX_CONFIG_H__
#define __RTX_CONFIG_H__

#ifndef __task
 #define __task
#endif

/*----------------------------------------------------------------------------
 *      General Data Types
 *---------------------------------------------------------------------------*/

#ifndef NULL
 #ifdef __cplusplus              // EC++
   #define NULL    0
 #else
   #define NULL    ((void *) 0)
 #endif
#endif

#ifdef __cplusplus               // EC++
extern "C"  {
#endif

#ifndef __RTL_EXT__
 #define __RTL_EXT__
 typedef char               S8;
 typedef unsigned char      U8;
 typedef short              S16;
 typedef unsigned short     U16;
 typedef int                S32;
 typedef unsigned int       U32;
 typedef long long          S64;
 typedef unsigned long long U64;
 typedef unsigned char      BIT;
 typedef unsigned int       BOOL;
 #ifndef __TRUE
  #define __TRUE        1
 #endif
 #ifndef __FALSE
  #define __FALSE       0
 #endif
#endif

/*
 * Function Pointer.
 */

typedef void (*FUNCP)(void);
typedef void (*FUNCPP)(void *);

/*----------------------------------------------------------------------------
 *      Task Control Block including CPU-specific task context storage
 *---------------------------------------------------------------------------*/

/* Values for 'cb_type' */
#define TCB             0
#define MCB             1
#define SCB             2
#define MUCB            3
#define HCB             4

/* Values for 'state'   */
#define INACTIVE        0
#define READY           1
#define RUNNING         2
#define WAIT_DLY        3
#define WAIT_ITV        4
#define WAIT_OR         5
#define WAIT_AND        6
#define WAIT_SEM        7
#define WAIT_MBX        8
#define WAIT_MUT        9

typedef struct OS_TCB {
   /* General part: identical for all implementations.                       */
   U8     cb_type;                /* Control Block Type                      */
   U8     state;                  /* Task state                              */
   U8     prio;                   /* Execution priority                      */
   U8     task_id;                /* Task ID value for optimized TCB access  */
   struct OS_TCB *p_lnk;          /* Link pointer for ready/sem. wait list   */
   struct OS_TCB *p_rlnk;         /* Link pointer for sem./mbx lst backwards */
   struct OS_TCB *p_dlnk;         /* Link pointer for delay list             */
   struct OS_TCB *p_blnk;         /* Link pointer for delay list backwards   */
   U16    delta_time;             /* Time until time out                     */
   U16    interval_time;          /* Time interval for periodic waits        */
   U16  volatile events;          /* Event flags volatile (changed by ISRs)  */
   U16    waits;                  /* Wait flags                              */
   void   *p_msg;                 /* Direct message passing when task waits  */
   U8     ret_val;                /* Return value upon completion of a wait  */

   /* Hardware dependant part: specific for ARM processor                    */
   U8     full_ctx;               /* Full or reduced context storage         */
   U16    priv_stack;             /* Private stack size, 0= system assigned  */
   U32    tsk_stack;              /* Current task Stack pointer (R13)        */
   U32    *stack;                 /* Pointer to Task Stack memory block      */

   /* Task entry point used for uVision debugger                             */
   FUNCP  ptask;                  /* Task entry address                      */
} *P_TCB;
#define TCB_FCTX        33        /* 'full_ctx' offset                       */
#define TCB_TSTACK      36        /* 'tsk_stack' offset                      */

/* Attention: component types and order must be the same as with 'OS_TCB' !! */
/* Used to build reduced list heads of all AR list types.                    */
typedef struct OS_XCB {
   U8     cb_type;                /* Control Block Type                      */
   struct OS_TCB *p_lnk;          /* Link pointer for ready/sem. wait list   */
   struct OS_TCB *p_rlnk;         /* Link pointer for sem./mbx lst backwards */
   struct OS_TCB *p_dlnk;         /* Link pointer for delay list             */
   struct OS_TCB *p_blnk;         /* Link pointer for delay list backwards   */
   U16    delta_time;             /* Time until time out                     */
} *P_XCB;

/* Attention: component types and order of 'cb_type' and 'p_lnk'must be the  */
/* same as with 'OS_TCB'. Used to build reduced list heads of AR list types. */
typedef struct OS_MCB {
   U8     cb_type;                /* Control Block Type                      */
   U8     isr_st;                 /* State flag variable for isr functions   */
   struct OS_TCB *p_lnk;          /* Chain of tasks waiting for message      */
   U16    first;                  /* Index of the message list begin         */
   U16    last;                   /* Index of the message list end           */
   U16    msg_count;              /* Actual number of stored messages        */
   U16    max_count;              /* Maximum number of stored messages       */
   void   *msg[1];                /* FIFO for Message pointers 1st element   */
} *P_MCB;

typedef struct OS_SCB {
   U8     cb_type;                /* Control Block Type                      */
   U16  volatile tokens;          /* 'volatile' can be changed by ISRs       */
   struct OS_TCB *p_lnk;          /* Chain of tasks waiting for tokens       */
} *P_SCB;

typedef struct OS_MUCB {
   U8     cb_type;                /* Control Block Type                      */
   U8     prio;                   /* Owner task default priority             */
   U16    level;                  /* Call nesting level                      */
   struct OS_TCB *p_lnk;          /* Chain of tasks waiting for mutex        */
   struct OS_TCB *owner;          /* Mutex owner task                        */
} *P_MUCB;

typedef struct OS_TMR {
   struct OS_TMR  *next;          /* Link pointer to Next timer              */
   U16    tcnt;                   /* Timer delay count                       */
   U16    info;                   /* User defined call info                  */
} *P_TMR;

/* Attention: component types and order must be the same as with "OS_TMR" !! */
/* Used to build reduced list head for user timers.                          */
typedef struct OS_XTMR {
   struct OS_TMR  *next;
   U16    tcnt;
} *P_XTMR;

typedef struct OS_PSFE {          /* Post Service Fifo Entry                 */
   void  *id;                     /* Object Identification                   */
   U32    arg;                    /* Object Argument                         */
} *P_PSFE;

typedef struct OS_PSQ {           /* Post Service Queue                      */
   U16    first;                  /* Fifo Head Index                         */
   U16    last;                   /* Fifo Tail Index                         */
   struct OS_PSFE q[16];          /* Fifo Content                            */
} *P_PSQ;

typedef struct OS_PSH {           /* Push Request                            */
   U32    cnt;                    /* HW timer snapshot                       */
   BIT    req;                    /* Push Service Request                    */
   BIT    flag;                   /* Push Service Flag                       */
} *P_PSH;

#ifndef OS_R_TMO
 /* Task Identification number. */
 typedef U32 OS_TID;

 /* Function return of system calls returning an object identification */
 typedef void *OS_ID;

 /* Function return of system calls indicating an event or completion state */
 typedef U32 OS_RESULT;

 /* Return codes */
 #define OS_R_TMO        0x01
 #define OS_R_EVT        0x02
 #define OS_R_SEM        0x03
 #define OS_R_MBX        0x04
 #define OS_R_MUT        0x05

 #define OS_R_OK         0x00
 #define OS_R_NOK        0xff
#endif

/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/

extern void  os_init_context (P_TCB p_TCB, U8 priority,
                              FUNCP task_body, U8 full_context);
extern void  os_init_mem (void);
extern P_TCB os_alloc_TCB (void);
extern void *os_alloc_stk (void);
extern void  os_free_TCB (P_TCB p_TCB);
extern void  os_set_env (P_TCB p_TCB);
extern void  os_switch_tasks (P_TCB p_new) __swi(0);
extern void  os_clock_demon (void) __task;
extern void  tsk_lock (void);
extern void  tsk_unlock (void);
extern void  os_psh_req (void);
extern void  os_tmr_call (U16  info);
extern P_TMR os_alloc_TMR (void);
extern void  os_free_TMR (P_TMR timer);
extern void  os_idle_demon (void) __task;
extern void  os_tmr_init (void);
extern void  os_tmr_reload (void);
extern void  os_tmr_force_irq (void);
extern U32   os_tmr_inspect_cnt (void);
extern BOOL  os_tmr_inspect_ovf (void);
extern void  os_put_rdy_first (P_TCB p_task);
extern void  os_put_prio (P_XCB p_CB, P_TCB p_task);
extern P_TCB os_get_first (P_XCB p_CB);
extern void  os_init_robin (void);
extern void  os_chk_robin (void);
extern BOOL  tstclrb (BIT *bp);
extern OS_TID os_get_TID (P_TCB task);

/*----------------------------------------------------------------------------
 *      Interface functions for fixed memory block management system
 *---------------------------------------------------------------------------*/

#define BOX_ALIGN_8     0x80000000

extern int  _init_box  (void *box_mem, U32 box_size, U32 blk_size);
extern void *_alloc_box (void *box_mem)  __swi (1);
extern void *_calloc_box (void *box_mem);
extern int  _free_box (void *box_mem, void *box)  __swi (2);

#define _declare_box(pool,size,cnt)   U32 pool[(((size)+3)/4)*(cnt) + 3]
#define _declare_box8(pool,size,cnt)  U64 pool[(((size)+7)/8)*(cnt) + 2]
#define _init_box8(pool,size,bsize)   _init_box (pool,size,(bsize) | BOX_ALIGN_8)

#ifdef __cplusplus               // EC++
}
#endif

/*----------------------------------------------------------------------------
 *      Interface functions for RT Agent task monitoring
 *---------------------------------------------------------------------------*/

__weak void rt_init (void);
__weak void rt_post_taskswitch (U32 task_id);
__weak void rt_notify (P_TCB p_tcb, BOOL create);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

#endif

