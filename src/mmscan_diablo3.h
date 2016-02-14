#ifndef _MMSCAN_DIABLO3_H_
#define _MMSCAN_DIABLO3_H_

#include "mmscan.h"
#include <stdio.h>

#define MMSCAN_DIABLO3_FILENAME_LEN 256
#define MMSCAN_DIABLO3_ACCOUNT_LEN 64
#define MMSCAN_DIABLO3_PASSWORD_LEN 64
#define MMSCAN_DIABLO3_IP_LEN 16
#define MMSCAN_DIABLO3_PROXYUSERNAME_LEN 16
#define MMSCAN_DIABLO3_PROXYPASSWORD_LEN 16
#define MMSCAN_DIABLO3_URL_LEN 256
/*
#define MMSCAN_DIABLO3_ACCOUNTFILEINFO_COUNT_MAX 1024
*/
#define MMSCAN_DIABLO3_ACCOUNTINFO_COUNT_MAX (1024 * 10 * 10 * 5)
#define MMSCAN_DIABLO3_PROXYINFO_COUNT_MAX 1024
#define MMSCAN_DIABLO3_LOGINGATEINFO_COUNT_MAX 10

#define MMSCAN_ACCOUNTINFO_STATE_SCANNING_DIABLO3 1
#define MMSCAN_ACCOUNTINFO_STATE_SCANNED_OK_DIABLO3 4
#define MMSCAN_ACCOUNTINFO_STATE_SCANNED_PASSWD_ERROR_DIABLO3 5
#define MMSCAN_ACCOUNTINFO_STATE_SCANNED_AUTH_ERROR_DIABLO3 6
#define MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOCK_ERROR_DIABLO3 7
#define MMSCAN_ACCOUNTINFO_STATE_UNSCAN_DIABLO3 8
#define MMSCAN_ACCOUNTINFO_STATE_UNSCAN_SECURITYCODE_ERROR_DIABLO3 9
#define MMSCAN_ACCOUNTINFO_STATE_UNSCAN_PROXY_ERROR_DIABLO3 10
#define MMSCAN_ACCOUNTINFO_STATE_SCANNED_LOGGED_DIABLO3 110


#define MMSCAN_PROXYINFO_STATE_OK_DIABLO3 1
#define MMSCAN_PROXYINFO_STATE_ERROR_DIABLO3 2
#define MMSCAN_PROXYINFO_STATE_SECURITECODE_ERROR_DIABLO3 3

#define MMSCAN_PROXYINFO_TYPE_PROXY 1
#define MMSCAN_PROXYINFO_TYPE_LOCAL 2
#define MMSCAN_PROXYINFO_TYPE_HTTPPROXY 3

#define MMSCAN_WORK_PARAM_RESULT_ERROR 0
#define MMSCAN_WORK_PARAM_RESULT_OK 1
#define MMSCAN_WORK_PARAM_RESULT_PROXY_ERROR 2
#define MMSCAN_WORK_PARAM_RESULT_SECURITYCODE_ERROR 3
#define MMSCAN_WORK_PARAM_RESULT_USERPASS_ERROR 4
#define MMSCAN_WORK_PARAM_RESULT_LOCK_ERROR 5
#define MMSCAN_WORK_PARAM_RESULT_AUTH_ERROR 6
#define MMSCAN_WORK_PARAM_RESULT_OPERATION_TIMEOUT_ERROR 7
#define MMSCAN_WORK_PARAM_RESULT_VALIDATION_ERROR 8
#define MMSCAN_WORK_PARAM_RESULT_AGREEMENT_ERROR 9
#define MMSCAN_WORK_PARAM_RESULT_UNKNOWN_ERROR 10

#define MMSCAN_DIRECTORY_RESULT "result"
#define MMSCAN_RESULT_ACCOUNT_OK MMSCAN_DIRECTORY_RESULT "/result_ok.txt"
#define MMSCAN_RESULT_ACCOUNT_ERROR MMSCAN_DIRECTORY_RESULT "/result_error.txt"
#define MMSCAN_RESULT_ACCOUNT_AUTH MMSCAN_DIRECTORY_RESULT "/result_auth.txt"
#define MMSCAN_RESULT_ACCOUNT_LOCK MMSCAN_DIRECTORY_RESULT "/result_lock.txt"
#define MMSCAN_RESULT_ACCOUNT_UNSCAN MMSCAN_DIRECTORY_RESULT "/result_unscan.txt"
#define MMSCAN_RESULT_ACCOUNT_VALIDATION MMSCAN_DIRECTORY_RESULT "/result_validation.txt"
#define MMSCAN_RESULT_ACCOUNT_AGREEMENT MMSCAN_DIRECTORY_RESULT "/result_agreement.txt"
#define MMSCAN_RESULT_ACCOUNT_UNKNOWN MMSCAN_DIRECTORY_RESULT "/result_unknown.txt"

#define MMSCAN_WRITER_PARAM_BUFFER_LEN 102400
/*
struct mmscan_diablo3_accountfileinfo;
typedef struct mmscan_diablo3_accountfileinfo mmscan_diablo3_accountfileinfo;
typedef struct mmscan_diablo3_accountfileinfo* pmmscan_diablo3_accountfileinfo;
*/

struct mmscan_accountinfo_diablo3;
typedef struct mmscan_accountinfo_diablo3 mmscan_accountinfo_diablo3;
typedef struct mmscan_accountinfo_diablo3* pmmscan_accountinfo_diablo3;
struct mmscan_proxyinfo_diablo3;
typedef struct mmscan_proxyinfo_diablo3 mmscan_proxyinfo_diablo3;
typedef struct mmscan_proxyinfo_diablo3* pmmscan_proxyinfo_diablo3;
struct mmscan_logingateinfo_diablo3;
typedef struct mmscan_logingateinfo_diablo3 mmscan_logingateinfo_diablo3;
typedef struct mmscan_logingateinfo_diablo3* pmmscan_logingateinfo_diablo3;
struct mmscan_param_diablo3;
typedef struct mmscan_param_diablo3 mmscan_param_diablo3;
typedef struct mmscan_param_diablo3* pmmscan_param_diablo3;
struct mmscan_work_param_diablo3;
typedef struct mmscan_work_param_diablo3 mmscan_work_param_diablo3;
typedef struct mmscan_work_param_diablo3* pmmscan_work_param_diablo3;
struct mmscan_writer_param_diablo3;
typedef struct mmscan_writer_param_diabo3 mmscan_writer_param_diabo3;
typedef struct mmscan_writer_param_diabo3* pmmscan_writer_param_diabo3;


/*
struct mmscan_diablo3_accountfileinfo
{
    char ac_accountfile[MMSCAN_DIABLO3_FILENAME_LEN];
    FILE* pst_file;
    int n_state;
};
*/

struct mmscan_accountinfo_diablo3
{
    char ac_account[MMSCAN_DIABLO3_ACCOUNT_LEN];
    char ac_password[MMSCAN_DIABLO3_PASSWORD_LEN];
    int n_state;
};

struct mmscan_proxyinfo_diablo3
{
    char ac_ip[MMSCAN_DIABLO3_IP_LEN];
    unsigned short us_port;
    char ac_username[MMSCAN_DIABLO3_PROXYUSERNAME_LEN];
    char ac_password[MMSCAN_DIABLO3_PROXYPASSWORD_LEN];
    int n_state;
    int n_type;
};

struct mmscan_logingateinfo_diablo3
{
    char ac_loginurl[MMSCAN_DIABLO3_URL_LEN];
    char ac_loginhost[MMSCAN_DIABLO3_URL_LEN];
};

struct mmscan_param_diablo3
{
    unsigned int aun_ai_again[MMSCAN_DIABLO3_ACCOUNTINFO_COUNT_MAX];
    unsigned int un_ai_again_count;
    pmmscan_accountinfo_diablo3 apst_ai[MMSCAN_DIABLO3_ACCOUNTINFO_COUNT_MAX];
    unsigned int un_ai_count;
    unsigned int un_ai_cur;
    unsigned int un_ai_count_finish;
    unsigned int un_ai_count_iterate;
    pmmscan_proxyinfo_diablo3 apst_pi[MMSCAN_DIABLO3_PROXYINFO_COUNT_MAX];
    unsigned int un_pi_count;
    unsigned int un_pi_cur;
    unsigned int un_pi_flag_iterate;
    pmmscan_logingateinfo_diablo3 apst_li[MMSCAN_DIABLO3_LOGINGATEINFO_COUNT_MAX];
    unsigned int un_li_count;
    unsigned int un_li_cur;
    FILE* pst_file_ok;
    FILE* pst_file_error;
    FILE* pst_file_auth;
    FILE* pst_file_lock;
    FILE* pst_file_unscan;
    FILE* pst_file_validation;
    FILE* pst_file_agreement;
    FILE* pst_file_unknown;
};

struct mmscan_work_param_diablo3
{
    unsigned int un_idx_logingateinfo;
    unsigned int un_idx_accountinfo;
    unsigned int un_idx_proxyinfo;
    int n_result;
};

struct mmscan_writer_param_diabo3
{
    FILE* pst_file;
    char ac_buffer[MMSCAN_WRITER_PARAM_BUFFER_LEN];
    unsigned int un_len;
};

int mmscan_diablo3_run( pmmscan pst_scan, void* p_param );
int mmscan_diablo3_beforerun( pmmscan pst_scan, void** p_param, int argn, char** argv );
int mmscan_diablo3_afterrun( pmmscan pst_scan, void* p_param );

#endif