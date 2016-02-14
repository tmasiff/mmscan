#ifndef _MMSCAN_BASE_H_
#define _MMSCAN_BASE_H_

#include <stdio.h>
#include <assert.h>
#include <curl/curl.h>
#include <Windows.h>

#define handle_error(c) if (!(c)) { printf("%s in func:%s @ line:%u\n", #c, __FUNCTION__, __LINE__ ); goto errorexit; }
#define handle_succ(c) if ((c)) { goto succexit; }

#endif