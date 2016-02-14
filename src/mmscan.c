#include "mmscan.h"

static pmmscan_work _mmscan_work_create();
static int _mmscan_work_reset( pmmscan_work pst_work );
static int _mmscan_work_destroy( pmmscan_work pst_work );
static int _mmscan_work_setid( pmmscan_work pst_work, unsigned int un_workid );
static int _mmscan_work_setscan( pmmscan_work pst_work, pmmscan pst_sacan );
static int _mmscan_work_setrunfunc( pmmscan_work pst_work, pmmscan_workrun pfn_run );
static void* _mmscan_work_allocextraspace( pmmscan_work pst_work, unsigned int un_size );
static int _mmscan_work_start( pmmscan_work pst_work );
static int _mmscan_work_stop( pmmscan_work pst_work );
static pmmscan_work _mmscan_work_findworkbyhandle( pmmscan pst_scan, HANDLE h_thread );
static DWORD WINAPI _mmscan_work_thread( LPVOID lpParam );


pmmscan mmscan_create()
{
    int n_result = 0;
    pmmscan pst_scan = NULL;
    int i = 0;
    
    handle_error( pst_scan = (pmmscan)calloc( 1, sizeof(mmscan) ) );
    pst_scan->pfn_beforerun = NULL;
    pst_scan->pfn_afterrun = NULL;
    pst_scan->pfn_run = NULL;
    pst_scan->p_param = NULL;
    pst_scan->n_run = 0;
    pst_scan->un_workfreecount = MMSCAN_WORK_COUNT_MAX;
    pst_scan->un_workfinishcount = 0;
    pst_scan->un_workthreadrunningcount = 0;
    memset( pst_scan->apst_work, 0, MMSCAN_WORK_COUNT_MAX * sizeof(pmmscan_work) );
    for ( i = 0; i < MMSCAN_WORK_COUNT_MAX; i++ )
    {
        handle_error( pst_scan->apst_work[i] = _mmscan_work_create() );
        handle_error( _mmscan_work_setscan( pst_scan->apst_work[i], pst_scan ) );
        handle_error( _mmscan_work_setid( pst_scan->apst_work[i], i ) );
    }
    
    n_result = 1;
errorexit:
    if ( 0 == n_result )
    {
        if ( pst_scan )
        {
            mmscan_destroy( pst_scan );
            pst_scan = NULL;
        }
    }
    return pst_scan;
}

int mmscan_reset( pmmscan pst_scan )
{
    int n_result = 0;
    int i = 0;

    assert( pst_scan );
    for ( i = 0; i < MMSCAN_WORK_COUNT_MAX; i++ )
    {
        if ( pst_scan->apst_work[i] )
        {
            handle_error( _mmscan_work_reset( pst_scan->apst_work[i] ) );
        }
    }
    pst_scan->un_workfreecount = MMSCAN_WORK_COUNT_MAX;
    pst_scan->un_workfinishcount = 0;
    pst_scan->un_workthreadrunningcount = 0;
    pst_scan->pfn_beforerun = NULL;
    pst_scan->pfn_afterrun = NULL;
    pst_scan->pfn_run = NULL;
    pst_scan->p_param = NULL;
    pst_scan->n_run = 0;

    n_result = 1;
errorexit:
    return n_result;
}

int mmscan_destroy( pmmscan pst_scan )
{
    int n_result = 0;
    int i = 0;

    assert( pst_scan );
    handle_error( mmscan_reset( pst_scan ) );
    pst_scan->un_workfreecount = MMSCAN_WORK_COUNT_MAX;
    pst_scan->un_workfinishcount = 0;
    pst_scan->un_workthreadrunningcount = 0;
    for ( i = 0; i < MMSCAN_WORK_COUNT_MAX; i++ )
    {
        if ( pst_scan->apst_work[i] )
        {
            handle_error( _mmscan_work_destroy( pst_scan->apst_work[i] ) );
        }
    }
    free( pst_scan );
    pst_scan = NULL;

    n_result = 1;
errorexit:
    return n_result;
}

int mmscan_setbeforerun( pmmscan pst_scan, pmmscan_beforerun pst_beforerun )
{
    assert( pst_scan );
    pst_scan->pfn_beforerun = pst_beforerun;
    return 1;
}

int mmscan_setafterrun( pmmscan pst_scan, pmmscan_afterrun pst_afterrun )
{
    assert( pst_scan );
    pst_scan->pfn_afterrun = pst_afterrun;
    return 1;
}
int mmscan_setrun( pmmscan pst_scan, pmmscan_run pfn_run )
{
    assert( pst_scan );
    pst_scan->pfn_run = pfn_run;
    return 1;
}

int mmscan_run( pmmscan pst_scan, int argn, char** argv )
{
    int n_result = 0;
    pmmscan_work pst_work = NULL;
    int i = 0;

    assert( pst_scan );
    handle_error( 0 == pst_scan->n_run );
    pst_scan->n_run = 1;
    if ( pst_scan->pfn_beforerun )
    {
        handle_error( pst_scan->pfn_beforerun( pst_scan, &pst_scan->p_param, argn, argv ) );
    }
    while ( pst_scan->n_run )
    {
        handle_error( pst_scan->pfn_run );
        handle_error( pst_scan->pfn_run( pst_scan, pst_scan->p_param ) );

        if ( pst_scan->un_workthreadrunningcount == 0 && pst_scan->un_workfinishcount == 0 )
        {
            Sleep( 100 );
        }
        else
        {
            BOOL b_wait = 0;
            HANDLE h_wait = NULL;
            unsigned int un_idx = 0;

            b_wait = WaitForMultipleObjects( pst_scan->un_workthreadrunningcount, (CONST HANDLE*)pst_scan->ah_workthreadrunning, FALSE, 100 );
            if ( WAIT_TIMEOUT == b_wait )
            {
                continue;
            }
            handle_error( WAIT_FAILED != b_wait );
            handle_error( b_wait < WAIT_ABANDONED_0 || b_wait > (BOOL)(WAIT_ABANDONED_0 + pst_scan->un_workthreadrunningcount) );
            handle_error( b_wait >= WAIT_OBJECT_0 && b_wait < (BOOL)(WAIT_OBJECT_0 + pst_scan->un_workthreadrunningcount) );
            un_idx = b_wait - WAIT_OBJECT_0;
            h_wait = pst_scan->ah_workthreadrunning[un_idx];
            handle_error( pst_work = _mmscan_work_findworkbyhandle( pst_scan, h_wait ) );
            pst_work->n_state = MMSCAN_WORK_STATE_FINISH;
            pst_scan->un_workfinishcount++;

            assert( un_idx != pst_scan->un_workthreadrunningcount );
            if ( pst_scan->un_workthreadrunningcount - un_idx == 1 )
            {
                pst_scan->ah_workthreadrunning[un_idx] = NULL;
            }
            else
            {
                memcpy( &pst_scan->ah_workthreadrunning[un_idx], &pst_scan->ah_workthreadrunning[un_idx+1], sizeof(HANDLE) * ( pst_scan->un_workthreadrunningcount - un_idx - 1 ) );
            }
            pst_scan->un_workthreadrunningcount--;
        }
    }

    n_result = 1;
errorexit:
    if ( pst_scan->pfn_afterrun )
    {
        pst_scan->pfn_afterrun( pst_scan, pst_scan->p_param );
    }
    pst_scan->n_run = 0;
    return n_result;
}

int mmscan_stop( pmmscan pst_scan )
{
    int n_result = 0;

    assert( pst_scan );
    handle_error( pst_scan->n_run );
    pst_scan->n_run = 0;

    n_result = 1;
errorexit:
    return n_result;
}

int mmscan_getfreeworkcount( pmmscan pst_scan )
{
    assert( pst_scan );
    assert( pst_scan->un_workfreecount == MMSCAN_WORK_COUNT_MAX - pst_scan->un_workfinishcount - pst_scan->un_workthreadrunningcount );
    return (int)pst_scan->un_workfreecount;
}

int mmscan_getrunningworkcount( pmmscan pst_scan )
{
    assert( pst_scan );
    assert( pst_scan->un_workthreadrunningcount == MMSCAN_WORK_COUNT_MAX - pst_scan->un_workfreecount - pst_scan->un_workfinishcount );
    return pst_scan->un_workthreadrunningcount;
}

int mmscan_getfinishworkcount( pmmscan pst_scan )
{
    assert( pst_scan );
    assert( pst_scan->un_workfinishcount == MMSCAN_WORK_COUNT_MAX - pst_scan->un_workfreecount - pst_scan->un_workthreadrunningcount );
    return pst_scan->un_workfinishcount;
}

pmmscan_work mmscan_getfreework( pmmscan pst_scan )
{
    pmmscan_work pst_work = NULL;
    int i = 0;

    assert( pst_scan );
    for ( i = 0; i < MMSCAN_WORK_COUNT_MAX; i++ )
    {
        assert( pst_scan->apst_work[i] );
        if ( MMSCAN_WORK_STATE_FREE == pst_scan->apst_work[i]->n_state )
        {
            pst_work = pst_scan->apst_work[i];
            break;
        }
    }

    return pst_work;
}

pmmscan_work mmscan_getrunningwork( pmmscan pst_scan )
{
    pmmscan_work pst_work = NULL;
    int i = 0;

    assert( pst_scan );
    for ( i = 0; i < MMSCAN_WORK_COUNT_MAX; i++ )
    {
        assert( pst_scan->apst_work[i] );
        if ( MMSCAN_WORK_STATE_RUNNING == pst_scan->apst_work[i]->n_state )
        {
            pst_work = pst_scan->apst_work[i];
            break;
        }
    }

    return pst_work;
}

pmmscan_work mmscan_getfinishwork( pmmscan pst_scan )
{
    pmmscan_work pst_work = NULL;
    int i = 0;

    assert( pst_scan );
    for ( i = 0; i < MMSCAN_WORK_COUNT_MAX; i++ )
    {
        assert( pst_scan->apst_work[i] );
        if ( MMSCAN_WORK_STATE_FINISH == pst_scan->apst_work[i]->n_state )
        {
            pst_work = pst_scan->apst_work[i];
            break;
        }
    }

    return pst_work;
}

int mmscan_startwork( pmmscan pst_scan, pmmscan_work pst_work )
{
    int n_result = 0;

    assert( pst_scan );
    assert( pst_work );
    handle_error( MMSCAN_WORK_STATE_FREE == pst_work->n_state );
    handle_error( _mmscan_work_start( pst_work ) );
    pst_scan->un_workfreecount--;
    pst_scan->ah_workthreadrunning[pst_scan->un_workthreadrunningcount++] = pst_work->h_thread;

    n_result = 1;
errorexit:
    return n_result;
}

int mmscan_stopwork( pmmscan pst_scan, pmmscan_work pst_work )
{
    int n_result = 0;
    int n_finish = 0;
    int i  = 0;
    HANDLE h_thread = NULL;

    assert( pst_scan );
    assert( pst_work );
    handle_error( MMSCAN_WORK_STATE_FINISH == pst_work->n_state || MMSCAN_WORK_STATE_RUNNING == pst_work->n_state );
    n_finish = pst_work->n_state == MMSCAN_WORK_STATE_FINISH ? 1 : 0;
    h_thread = pst_work->h_thread;
    handle_error( _mmscan_work_stop( pst_work ) );
    pst_scan->un_workfreecount++;
    if ( n_finish )
    {
        pst_scan->un_workfinishcount--;
    }
    else
    {
        for ( i = 0; i < (int)pst_scan->un_workthreadrunningcount; i++ )
        {
            if ( pst_scan->ah_workthreadrunning[i] == h_thread )
            {
                break;
            }
        }
        if ( i != (int)pst_scan->un_workthreadrunningcount )
        {
            if ( (int)pst_scan->un_workthreadrunningcount - i == 1 )
            {
                pst_scan->ah_workthreadrunning[i] = NULL;
            }
            else
            {
                memcpy( &pst_scan->ah_workthreadrunning[i], &pst_scan->ah_workthreadrunning[i+1], sizeof(HANDLE) * ( pst_scan->un_workthreadrunningcount - i - 1 ) );
            }
            pst_scan->un_workthreadrunningcount--;
        }
    }

    n_result = 1;
errorexit:
    return n_result;
}

static pmmscan_work _mmscan_work_findworkbyhandle( pmmscan pst_scan, HANDLE h_thread )
{
    pmmscan_work pst_work = NULL;
    int i = 0;

    assert( pst_scan );
    for ( i = 0; i < MMSCAN_WORK_COUNT_MAX; i++ )
    {
        assert( pst_scan->apst_work[i] );
        if ( pst_scan->apst_work[i]->h_thread == h_thread )
        {
            pst_work = pst_scan->apst_work[i];
            break;
        }
    }

    return pst_work;
}

int mmscan_work_setrunfunc( pmmscan_work pst_work, pmmscan_workrun pfn_run )
{
    return _mmscan_work_setrunfunc( pst_work, pfn_run );
}

void* mmscan_work_allocextraspace( pmmscan_work pst_work, unsigned int un_size )
{
    return _mmscan_work_allocextraspace( pst_work, un_size );
}

void* mmscan_work_getextraspace( pmmscan_work pst_work, unsigned int un_size )
{
    void* p_extra = NULL;

    assert( pst_work );
    handle_error( un_size <= pst_work->un_extraspacesize );
    handle_error( p_extra = pst_work->p_extraspace );

errorexit:
    return p_extra;
}

static pmmscan_work _mmscan_work_create()
{
    int n_result = 0;
    pmmscan_work pst_work = NULL;

    handle_error( pst_work = (pmmscan_work)calloc( 1, sizeof(mmscan_work ) ) );
    pst_work->pst_scan_owner = NULL;
    pst_work->un_workid = 0;
    pst_work->h_thread = NULL;
    pst_work->n_state = MMSCAN_WORK_STATE_FREE;
    pst_work->pfn_workrun = NULL;
    pst_work->p_extraspace = NULL;
    pst_work->un_extraspacesize = 0;
    handle_error( pst_work->p_extraspace = (void*)calloc( 1, MMSCAN_WORK_EXTRASPACE_LEN) );
    pst_work->un_extraspacesize = MMSCAN_WORK_EXTRASPACE_LEN;

    n_result = 1;
errorexit:
    return pst_work;
}

static int _mmscan_work_reset( pmmscan_work pst_work )
{
    int n_result = 0;

    assert( pst_work );
    if ( MMSCAN_WORK_STATE_RUNNING == pst_work->n_state || MMSCAN_WORK_STATE_FINISH == pst_work->n_state )
    {
        handle_error( _mmscan_work_stop( pst_work ) );
    }
    assert( MMSCAN_WORK_STATE_FREE == pst_work->n_state );
    assert( NULL == pst_work->h_thread );
    pst_work->pst_scan_owner = NULL;
    pst_work->pfn_workrun = NULL;
    pst_work->un_workid = 0;

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_work_destroy( pmmscan_work pst_work )
{
    int n_result = 0;

    assert( pst_work );
    if ( MMSCAN_WORK_STATE_RUNNING == pst_work->n_state || MMSCAN_WORK_STATE_FINISH == pst_work->n_state )
    {
        handle_error( _mmscan_work_stop( pst_work ) );
    }
    if ( pst_work->p_extraspace )
    {
        free( pst_work->p_extraspace );
        pst_work->p_extraspace = NULL;
    }
    pst_work->un_extraspacesize = 0;
    free( pst_work );
    pst_work = NULL;

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_work_setid( pmmscan_work pst_work, unsigned int un_workid )
{
    assert( pst_work );
    pst_work->un_workid = un_workid;
    return 1;
}

static int _mmscan_work_setscan( pmmscan_work pst_work, pmmscan pst_sacan )
{
    assert( pst_work );
    pst_work->pst_scan_owner = pst_sacan;
    return 1;
}

static int _mmscan_work_setrunfunc( pmmscan_work pst_work, pmmscan_workrun pfn_run )
{
    assert( pst_work );
    pst_work->pfn_workrun = pfn_run;
    return 1;
}

static void* _mmscan_work_allocextraspace( pmmscan_work pst_work, unsigned int un_size )
{
    void* p_extra = NULL;

    if ( pst_work->p_extraspace && pst_work->un_extraspacesize >= un_size )
    {
        p_extra = pst_work->p_extraspace;
    }
    else
    {
        if ( pst_work->p_extraspace )
        {
            free( pst_work->p_extraspace );
            pst_work->p_extraspace = NULL;
        }
        pst_work->un_extraspacesize = 0;
        handle_error( pst_work->p_extraspace = calloc( 1, un_size ) );
        pst_work->un_extraspacesize = un_size;
        p_extra = pst_work->p_extraspace;
    }

errorexit:
    return p_extra;
}

static int _mmscan_work_start( pmmscan_work pst_work )
{
    int n_result = 0;

    assert( pst_work );
    handle_error( MMSCAN_WORK_STATE_FREE == pst_work->n_state );
    handle_error( NULL == pst_work->h_thread );
    pst_work->h_thread = CreateThread( NULL, 0, _mmscan_work_thread, pst_work, 0, NULL );
    handle_error( NULL != pst_work->h_thread );
    pst_work->n_state = MMSCAN_WORK_STATE_RUNNING;

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_work_stop( pmmscan_work pst_work )
{
    int n_result = 0;

    assert( pst_work );
    handle_error( MMSCAN_WORK_STATE_RUNNING == pst_work->n_state || MMSCAN_WORK_STATE_FINISH == pst_work->n_state );
    if ( MMSCAN_WORK_STATE_RUNNING == pst_work->n_state )
    {
        handle_error( TerminateThread( pst_work->h_thread, 0 ) );
    }
    else
    {
        handle_error( CloseHandle( pst_work->h_thread ) );
    }
    pst_work->h_thread = NULL;
    pst_work->n_state = MMSCAN_WORK_STATE_FREE;

    n_result = 1;
errorexit:
    return n_result;
}

static DWORD WINAPI _mmscan_work_thread( LPVOID lpParam )
{
    DWORD dw_result = 0;
    pmmscan_work pst_work = NULL;

    assert( lpParam );
    pst_work = (pmmscan_work)lpParam;
    if ( pst_work->pfn_workrun )
    {
        assert( pst_work->pst_scan_owner );
        handle_error( pst_work->pfn_workrun( pst_work->pst_scan_owner, pst_work->pst_scan_owner->p_param, pst_work->p_extraspace, pst_work->un_workid ) );
    }

    dw_result = 1;
errorexit:
    return dw_result;
}