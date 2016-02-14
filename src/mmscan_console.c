#include <stdio.h>
#include <curl/curl.h>

#include "mmscan.h"
#include "mmscan_diablo3.h"

int main( int argn, char** argv )
{
    int n_result = -1;
    pmmscan pst_scan = NULL;
    WSADATA c_data_wsa;

    WSAStartup( MAKEWORD( 2, 2 ), &c_data_wsa );
    handle_error( pst_scan = mmscan_create() );
    handle_error( mmscan_setbeforerun( pst_scan, &mmscan_diablo3_beforerun ) );
    handle_error( mmscan_setafterrun( pst_scan, &mmscan_diablo3_afterrun ) );
    handle_error( mmscan_setrun( pst_scan, &mmscan_diablo3_run ) );
    handle_error( mmscan_run( pst_scan, argn, argv ) );

    n_result = 0;
errorexit:
    if ( pst_scan )
    {
        mmscan_destroy( pst_scan );
        pst_scan = NULL;
    }
    return n_result;
}

#if 0
size_t mywriter( void* p_data, size_t size, size_t nmemb, void* userp );
size_t myheaderwriter( void* p_data, size_t size, size_t nmemb, void* userp );
#endif

#if 0
int main(int argn, char** argv)
{
    CURL *curl;
    CURLcode res;
    FILE* fp = NULL;
    FILE* fpheader = NULL;
    struct curl_slist* pst_header = NULL;


    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) 
    {
      /* First set the URL that is about to receive our POST. This URL can
         just as well be a https:// URL if that is what should receive the
         data. */

        if ( NULL == ( fp = fopen( "demo.html", "wb" ) ) ) return -1;
        if ( NULL == ( fpheader = fopen( "header.txt", "wb" ) ) ) return -1;

        pst_header = curl_slist_append( pst_header, "Host: tw.battle.net" );
        pst_header = curl_slist_append( pst_header, "Connection: keep-alive" );
        pst_header = curl_slist_append( pst_header, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.56 Safari/536.5" );
        pst_header = curl_slist_append( pst_header, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
        pst_header = curl_slist_append( pst_header, "Accept-Encoding: deflate" );
        pst_header = curl_slist_append( pst_header, "Accept-Language: zh-CN,zh;q=0.8" );
        pst_header = curl_slist_append( pst_header, "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3" );

        /*
        curl_easy_setopt( curl, CURLOPT_URL, "https://tw.battle.net/login/zh/" );
        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, mywriter );
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );
        curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0 );
        curl_easy_setopt( curl, CURLOPT_HTTPHEADER, pst_header );
        res = curl_easy_perform(curl);
        */
        fclose( fp );

        if ( NULL == ( fp = fopen( "demo1.html", "wb" ) ) ) return -1;
        curl_easy_setopt(curl, CURLOPT_URL, "https://tw.battle.net/login/zh/?ref=&app=");
        /* Now specify the POST data */
        curl_easy_setopt( curl, CURLOPT_HTTPHEADER, pst_header );
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "accountName=test@gmail.com&password=123");
        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, mywriter );
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );
        curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, myheaderwriter );
        curl_easy_setopt( curl, CURLOPT_HEADERDATA, fpheader );
        curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0 );
        curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1 );
        curl_easy_setopt( curl, CURLOPT_VERBOSE, 1 );
        curl_easy_setopt( curl, CURLOPT_COOKIEJAR, "cookie.txt");   // 设置从$cookie所指文件中读取cookie信息以发送
        curl_easy_setopt( curl, CURLOPT_COOKIEFILE, "cookie.txt");   // 设置将返回的cookie保存到$cookie所指文件

        /*
        curl_easy_setopt( curl, CURLOPT_PROXY, "10.20.104.175" );
        curl_easy_setopt( curl, CURLOPT_PROXYPORT, 1081 );
        curl_easy_setopt( curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5 );
        curl_easy_setopt( curl, CURLOPT_PROXYUSERNAME, "username" );
        curl_easy_setopt( curl, CURLOPT_PROXYUSERPWD, "password" );
        */

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
    
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose( fp );
    }


    return 0;
}

size_t mywriter( void* p_data, size_t size, size_t nmemb, void* userp )
{
    FILE* p_file = (FILE*)userp;
    fwrite( p_data, nmemb, size, p_file );
    return size * nmemb;
}

size_t myheaderwriter( void* p_data, size_t size, size_t nmemb, void* userp )
{
    FILE* p_file = (FILE*)userp;
    fwrite( p_data, nmemb, size, p_file );
    return size * nmemb;
}
#endif