#ifndef _MMSCAN_H_
#define _MMSCAN_H_

#include "mmscan_base.h"

/* 怪异的问题，暂时无解，，，， 神啊。。。 单个线程或者主线程用sock5代理去连接https网页必定超时，，后期诡异的冒出来的，前期不存在这个问题！！！ */
#define MMSCAN_WORK_COUNT_MAX 15

#define MMSCAN_WORK_EXTRASPACE_LEN 32

#define MMSCAN_WORK_STATE_FREE 1
#define MMSCAN_WORK_STATE_FINISH 2
#define MMSCAN_WORK_STATE_RUNNING 3

struct mmscan;
typedef struct mmscan mmscan;
typedef struct mmscan* pmmscan;

struct mmscan_work;
typedef struct mmscan_work mmscan_work;
typedef struct mmscan_work* pmmscan_work;

typedef int (*pmmscan_run)( pmmscan pst_scan, void** pp_param );
typedef int (*pmmscan_beforerun)( pmmscan pst_scan, void* p_param, int argn, char** argv );
typedef int (*pmmscan_afterrun)( pmmscan pst_scan, void* p_param );
typedef int (*pmmscan_workrun)( pmmscan pst_scan, void* p_param, void* p_extraspace, unsigned int un_workid );

struct mmscan
{
    pmmscan_run pfn_run;
    pmmscan_beforerun pfn_beforerun;
    pmmscan_afterrun pfn_afterrun;
    void* p_param;
    int n_run;
    pmmscan_work apst_work[MMSCAN_WORK_COUNT_MAX];
    unsigned int un_workfreecount;
    unsigned int un_workfinishcount;
    HANDLE ah_workthreadrunning[MMSCAN_WORK_COUNT_MAX];
    unsigned int un_workthreadrunningcount;
};

struct mmscan_work
{
    pmmscan pst_scan_owner;
    pmmscan_workrun pfn_workrun;
    void* p_extraspace;
    unsigned int un_extraspacesize;
    HANDLE h_thread;
    unsigned int un_workid;
    int n_state;
};

pmmscan mmscan_create();
int mmscan_reset( pmmscan pst_scan );
int mmscan_destroy( pmmscan pst_scan );
int mmscan_setbeforerun( pmmscan pst_scan, pmmscan_beforerun pst_beforerun );
int mmscan_setafterrun( pmmscan pst_scan, pmmscan_afterrun pst_beforerun );
int mmscan_setrun( pmmscan pst_scan, pmmscan_run pfn_run );
int mmscan_run( pmmscan pst_scan, int argn, char** argv );
int mmscan_stop( pmmscan pst_scan );
int mmscan_getfreeworkcount( pmmscan pst_scan );
int mmscan_getrunningworkcount( pmmscan pst_scan );
int mmscan_getfinishworkcount( pmmscan pst_scan );
pmmscan_work mmscan_getfreework( pmmscan pst_scan );
pmmscan_work mmscan_getrunningwork( pmmscan pst_scan );
pmmscan_work mmscan_getfinishwork( pmmscan pst_scan );
int mmscan_startwork( pmmscan pst_scan, pmmscan_work pst_work );
int mmscan_stopwork( pmmscan pst_scan, pmmscan_work pst_work );

int mmscan_work_setrunfunc( pmmscan_work pst_work, pmmscan_workrun pfn_run );
void* mmscan_work_allocextraspace( pmmscan_work pst_work, unsigned int un_size );
void* mmscan_work_getextraspace( pmmscan_work pst_work, unsigned int un_size );

#endif