/**
 * @file    pet_util.h
 * @brief   一些封装好的使用工具函数
 * @author  jamieli@tencent.com
 * @date    2009-03-03
 */

#ifndef __NARUTO_UTIL_H__
#define __NARUTO_UTIL_H__

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <stdint.h>
#include <sstream>
#include <tconnapi/tframehead.h>
#include "framework/singleton.h"

const uint32_t MAX_NAME_STR_LEN = 63;
const uint32_t MAX_FILE_PATH_LEN = 2047;     // 文件或者路径的最大长度
const uint32_t ONE_MINUTE_SECOND = 60;        //一分钟的秒数
const uint32_t ONE_HOUR_SECOND = 3600;        //一小时的秒数
const uint32_t ONE_DAY_SECOND = 24 * ONE_HOUR_SECOND; //一天的秒数
const uint32_t ONE_WEEK_SECOND = 7 * ONE_DAY_SECOND; //一周的秒数

using namespace std;

// 内核版的用g++编译有警告,所以下面来版C++的吧
/*
#define container_of(ptr, type, member) ({          \
    const decltype( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})
*/

template <class _Tp, class _Tm>
size_t offsetof_(const _Tm _Tp::*member)
{
    return (size_t)&(reinterpret_cast<_Tp*>(0)->*member);
}

template <class _Tp, class _Tm>
_Tp *container_of(_Tm *ptr, const _Tm _Tp::*member)
{
    // must_be_pod<_Tp>();

    static_assert(std::is_pod<_Tp>::value, "_Tp must be a POD");

    return (_Tp*)((char *)ptr - offsetof_(member));
}


//#define ARRAY_SIZE(a)       (sizeof(a)/sizeof(a[0]))

/**
 * 2015-03-13 类型安全的arraysize, 出处可以看这里：
 * http://stackoverflow.com/questions/6376000/how-does-this-array-size-template-work
 */
template<typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

/**
 * 2013-03-13 strncpy的文档说，如果源的字符数量大于目标的容量，则不会在末尾加上0结束符号，
 * 所以封一个保证一定会加结束符号的函数。
 * 这个函数来源于这里：
 * https://randomascii.wordpress.com/2013/04/03/stop-using-strncpy-already/
 */
template<size_t length>
void strcpy_safe(char (&dest)[length], const char* src)
{
    //Copy the string — don’t copy too many bytes.
    strncpy(dest, src, length);
    //Ensure null-termination.
    dest[length - 1] = 0;
}

/**
* @brief T型转换成字符串，只要T能够使用ostream对象用<<重载,即可以被该函数支持
* @param t 要转换的数据
* @return  转换后的字符串
*/
template<typename T>
std::string anytoStr(const T &t);

/**
 * BKDRHash 字符串hash算法。
 */
uint32_t BKDRHash(const char *str);

class FileDescriptorGuard
{
public:
    FileDescriptorGuard(int fd)
            : _fd(fd)
    {
    }
    ~FileDescriptorGuard()
    {
        if (_fd)
            close(_fd);
    }
    int _fd;
};

class FileStreamGuard
{
public:
    FileStreamGuard(FILE * f)
            : _f(f)
    {
    }
    ~FileStreamGuard()
    {
        if (_f)
            fclose(_f);
    }
    FILE * _f;
};

inline bool operator<(const struct timespec& t1, const struct timespec& t2)
{
    return (t1.tv_sec < t2.tv_sec) || (t1.tv_sec == t2.tv_sec && t1.tv_nsec < t2.tv_nsec);
}

inline bool operator<(const struct timeval& t1, const struct timeval& t2)
{
    return (t1.tv_sec < t2.tv_sec) || (t1.tv_sec == t2.tv_sec && t1.tv_usec < t2.tv_usec);
}

inline bool operator >(const struct timeval& lhs, const struct timeval& rhs)
{
    return (lhs.tv_sec > rhs.tv_sec) || ((lhs.tv_sec == rhs.tv_sec) && (lhs.tv_usec > rhs.tv_usec));
}

class TimeHelper
{
    /*
     * 所有都考虑时区
     */

public:
    //static unsigned long GetCurMSec()
    //毫秒
    static uint64_t GetCurMSec()
    {
        struct timeval tm;
        gettimeofday(&tm, NULL);
        return (tm.tv_sec * 1000 + tm.tv_usec / 1000);
    }

    //微秒
    static uint64_t GetCurUSec()
    {
        struct timeval tm;
        gettimeofday(&tm, NULL);
        return (tm.tv_sec * 1000000 + tm.tv_usec);
    }

    //获取当前时区时间
    static time_t TimeNow(time_t now)
    {
        return (now + 8 * ONE_HOUR_SECOND);
    }

    static tm TmNow(time_t now)
    {
        tm tm_now;
        localtime_r(&now, &tm_now);
        return tm_now;
    }
    //
    static uint32_t GetCurHour(time_t now)
    {
        return TimeNow(now) % ONE_DAY_SECOND / ONE_HOUR_SECOND;
    }
    //
    static uint32_t GetCurDay(time_t now)
    {
        return TimeNow(now) / ONE_DAY_SECOND;
    }

    static uint32_t GetCurWeek(time_t now)
    {
    	return (TimeNow(now) + 3*ONE_DAY_SECOND) / ONE_WEEK_SECOND;
    }

    static int32_t GetCurYear(time_t now)
    {
        /*
         *     struct tm
         {
         int tm_sec;//seconds 0-59
         int tm_min;//minutes 1-59
         int tm_hour;//hours 0-23
         int tm_mday;//day of the month 1-31
         int tm_mon;//months since jan 0-11
         int tm_year;//years from 1900
         int tm_wday;//days since Sunday, 0-6
         int tm_yday;//days since Jan 1, 0-365
         int tm_isdst;//Daylight Saving time indicator
         };
         *
         */
        struct tm tm_now;
        localtime_r(&now, &tm_now);
        return tm_now.tm_year;
    }
    static int32_t GetCurYearDay(time_t now)
    {
        struct tm tm_now;
        localtime_r(&now, &tm_now);
        return tm_now.tm_yday;
    }
    static int32_t GetCurWeekDay(time_t now)
    {
        struct tm tm_now;
        localtime_r(&now, &tm_now);
        return tm_now.tm_wday;
    }

    static int GetMonday(int Date)
    {
        struct tm t;
        time_t t_of_day;
        t.tm_year = Date / 10000 - 1900;
        t.tm_mon = Date / 100 % 100 - 1;
        t.tm_mday = Date % 100;
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        t.tm_isdst = 0;
        t_of_day = mktime(&t);
        struct tm *pTmp = localtime(&t_of_day);
        int day;
        int week = pTmp->tm_wday;
        if (week == 0)
            week = 7;
        day = week - 1;
        t_of_day -= day * 24 * 3600;
        return t_of_day;
    }

    static int32_t IsLeapYear(int tm_year)
    {
        if ((tm_year % 4 == 0 && tm_year % 100 != 0) || (tm_year % 400 == 0))
        {
            return 1;
        }
        return 0;
    }
    static std::string FormatTimeStr(time_t time_now)
    {
        std::string str;
        struct tm tm_now;
        localtime_r(&time_now, &tm_now);
        char temp_str[1024];
        sprintf(temp_str, "%04d-%02d-%02d %02d:%02d:%02d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
        str = temp_str;
        return str;
    }
    /*获取RTC*/
    static uint64_t get_tsc_value()
    {
        uint64_t val = 0;
        __asm__ __volatile__("rdtsc" : "=A" (val));
        return val;
    }

    static bool isSameWeek(time_t time1, time_t time2)
    {
        time_t one_week = 60 * 60 * 24 * 7;
        return time1 / one_week == time2 / one_week;
    }

    static bool isSameDayWithCurTime(time_t iCheckTime, time_t iCurTime)
    {
        time_t iLast = (time_t) iCheckTime;
        struct tm stCur;
        struct tm stLast;
        localtime_r(&iCurTime, &stCur);
        localtime_r(&iLast, &stLast);
        if (stCur.tm_mon == stLast.tm_mon && stCur.tm_mday == stLast.tm_mday && stCur.tm_year == stLast.tm_year)
        {
            return true;
        }
        else
        {
            return false;
        }
    }


    static inline int distanceCurTimeDay(time_t iCheckTime, time_t iCurTime)
   {
       time_t iLast = (time_t) iCheckTime;
       struct tm stCur;
       struct tm stLast;
       localtime_r(&iCurTime, &stCur);
       localtime_r(&iLast, &stLast);
       stCur.tm_hour = 0;
       stCur.tm_min = 0;
       stCur.tm_sec = 0;
       stLast.tm_hour = 0;
       stLast.tm_min = 0;
       stLast.tm_sec = 0;
       iCurTime = mktime(&stCur);
       iCheckTime = mktime(&stLast);
       int day = (iCurTime - iCheckTime) / 86400;
       return day;
   }

    //mysql time 和time_t互转
    static time_t datetime_to_timet(const char* mysql_datetime);
    static char* timet_to_datetime(time_t time__);

    // 计算t1-t2，结果为毫秒数
    static int64_t msbetween(const struct timespec& t1, const struct timespec& t2);
    static int64_t msbetween(const struct timeval& t1, const struct timeval& t2);

    // 获取当前的cpu时钟时间，不受任何校时的影响，用来计时。
    static struct timespec GetCurrentClockTime()
      {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts;
      }

    //YYYYMMDD
    static inline std::string GetDate(time_t t)
    {
        char   tmpbuf[128];
        struct tm * timeinfo;

        timeinfo = localtime(&t);
        strftime(tmpbuf, 128, "%Y%m%d", timeinfo);

        return std::string (tmpbuf);
    }

    static inline int GetDateIntByTime(time_t t)
    {
        return static_cast<unsigned int>(strtol(GetDate(t).c_str(), NULL, 10));
    }

};

template <class T> struct prop_extractor
{
    uint32_t operator()(const T& x) const { return x.prop; }
};

class NumberHelper
{
public:
    /* 用来代替rand的函数，用rand_r实现。*/
    static inline int32_t ARand()
    {
        static unsigned int s__ = time(NULL) + getpid();
        return rand_r(&s__);
    }

    /* random_shuffle 专用*/
    static inline int32_t arandom_shuffle_generator(int32_t i)
    {
        return ARand() % i;
    }

    /* 产生[a, b]之间的随机数，如果a>=b，直接返回a
     */
    static int32_t ARandRange(const int32_t a, int32_t b);

    //by kevin 产生[0-1)之前的随机数, 注意rand()函数的区间是[0,RAND_MAX], 所以要+1
    //测试了一下tr1::random中的一些做法，还是自己实现更高效。
    static inline double Random01()
    {
        return  ARand()/ (double(RAND_MAX) + 1 );
    }

    // N选1
    template<
    class T,
    class ExtractProp=prop_extractor<T>
    >
    static int ProbNChooseOne(const std::vector<T>& prob)
    {
        uint32_t sum = 0;
        ExtractProp _extractor;
        for (uint32_t i = 0; i < prob.size(); i++)
        {
            sum += _extractor(prob[i]);
        }
        uint32_t c = 0;
        uint32_t r = arandom_shuffle_generator(sum);

        for (uint32_t i = 0; i < prob.size(); i++)
        {
            c += _extractor(prob[i]);
            if (c > r)
            {
                return i;
            }
        }
        return prob.size() - 1;
    }
//    /*
//     * 规范数值大小
//     */
//    template<class T>
//    static void check_max_limit(T& a, int32_t max)
//    {
//        a = (a > max) ? max : a;
//    }
};

/**
 * 将二进制的内容打成16进制的string，调试的时候用
 * @param source
 * @param length
 * @param dest
 *
 * 2015-10-20 原先返回void，无法在trace日志中调用，所以将返回值修改称dest字符串的地址，
 * 便于日志输出。
 */
const char* Binary2String(const char* source, size_t length, std::string& dest);


//同上，将一块buffer以hex形式打印出来
//2015年10月20日 14:54:57@kevinlin
const char *Binary2String(const char *pvBuff, int iSize);

/**
 * 一个类似于sprintf的函数，但是参数是string，主要用来作为errstring之类的
 * 极少碰到的场合。该函数的实现从这里得到：
 * http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
 * @param buff 用来返回格式化好的字符串。buff之前的内容会被清空；
 * @param fmt_str
 * @return 和buff参数一样
 * 												franktang
 */
std::string& StringFormat(std::string& buff, const std::string fmt_str, ...);

/**
 * 字符串分割，写工具时可能会用。
 * 这种实现方法是我见过最优雅最简单的，出自这里：
 * http://stackoverflow.com/questions/599989/is-there-a-built-in-way-to-split-strings-in-c
 * @param str
 * @param delimiters
 * @param tokens
 */
void split(const std::string& str, const std::string& delimiters, std::vector<std::string>& tokens);

//2016年1月8日 18:11:01@kevin
//字符串工具。。说明我们的库太寒酸了。从水浒搬了个过来
class CStrTool
{
public:
    static void StringReplace(const string& sStr, const string& sOldSub,
                const string& sNewSub, bool bReplaceAll, string* psRes);

    static std::string StringReplace(const string& sStr, const string& sOldSub,
                const string& sNewSub, bool bReplaceAll);
};

uint64_t ntohll(uint64_t val);
uint64_t htonll(uint64_t val);

// 通用的写pid文件的工具函数
int WritePidFile(char* argv0);

// 网上找到的一个算法检查溢出 by johnfu
/* Determine whether arguments can be added without overflow */
bool tadd_ok(int x, int y);

/* Determine whether arguments can be added without overflow */
bool uadd_ok(unsigned int x, unsigned int y);

/* Determine whether argumnts can be substracted without overflow */
bool usub_ok(unsigned int x, unsigned int y);

bool uadd_ok(unsigned int x, int y);

unsigned int GetInnerIP();

class ShmKeyDump
{
	DECL_SINGLETON(ShmKeyDump);
public:
	int Init(const string &proc_name, const string &bus_addr);
	int PushKey(key_t shm_key);
private:
	string dump_file_name_;
	bool init_;
};

void CheckOpenID(TFRAMEHEADACCOUNT& account, tdr_ip_t ip);


uint64_t HashOpenID(const std::string& s);

uint32_t GetPvpSkillIdOffset(uint32_t app_version);

string trim(const string& str);

#endif//__NARUTO_UTIL_H__

