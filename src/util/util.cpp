/**
 * @file    pet_util.h
 * @brief   一些封装好的使用工具函数
 * @author  jamieli@tencent.com
 * @date    2009-03-03
 */

#include <memory>
#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <cstdarg>
#include <sstream>
#include <cstdio>
#include <com_include/com_def.h>
#include "util.h"
#include "net/if.h"
#include "sys/ioctl.h"

#include "log/log.h"

using namespace std;

time_t TimeHelper::datetime_to_timet(const char* mysql_datetime)
{
    struct tm stTime = { 0 };
    strptime(mysql_datetime, "%Y-%m-%d %H:%M:%S", &stTime);
    return mktime(&stTime);
}

char* TimeHelper::timet_to_datetime(time_t time__)
{
    static char * buff = new char[21];
    memset(buff, 0, 21);
    buff[20] = 0;
    struct tm * ptm = localtime(&time__);
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", ptm);
    return buff;
}

int32_t NumberHelper::ARandRange(const int32_t a, const int32_t b)
{
    if(a >= b)
    {
        return a;
    }

    int32_t num_min = a;
    int32_t num_max = b + 1;

    int32_t r = ARand();

    int32_t result = (num_min + (int32_t)((num_max - num_min) * (r / (RAND_MAX + 1.0))));

    //int result = arand() % (b - a + 1) + a;

    return result;
}

std::string& StringFormat(std::string& buff, const std::string fmt_str, ...)
{
    size_t n = 256;

    if(buff.size() < n)
    {
        buff.resize(n);
    }
    else
    {
        n = buff.size();
    }

    //cout<<"------------------------"<<endl;
    while(1)
    {
        //std::cout<<"processing...., n="<<n<<std::endl;
        va_list ap;
        va_start(ap, fmt_str);
        const int final_n = vsnprintf(&buff[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if(final_n < 0) // 文档说了返回负值表示encoding error
        {
            //n += size_t(-final_n);
            buff = "encoding error";
            break;
        }

        if(static_cast<size_t>(final_n) >= n)
        {
            n += static_cast<size_t>(final_n) - n + 1;
            if(n > 4096) // 免得分配的内存太大不能控制
            {
                buff = "string too long, larger then 4096...";
                break;
            }
            buff.resize(n);
        }
        else
        {
            buff[final_n] = '\0';
            buff.resize(final_n);
            break;
        }
    }

    return buff;

}

const char* Binary2String(const char* source, size_t length, std::string& dest)
{
    dest.clear();
    dest.reserve(3*length);
    for(size_t i = 0; i < length; ++i)
    {
        char digit[4];
        // 这里不能用%02x，必须用%02hhx，否则会溢出造成死循环。详情见：
        // http://bbs.csdn.net/topics/80503352
        snprintf(digit, sizeof(digit), "%02hhX ", source[i]);
        //digit[2] = 0; //2015年8月26日 18:57:10 by kevin,  snprintf会自动在末尾加\0
        dest.append(digit);
    }

    return (0 == dest.size()) ? NULL : &dest[0];
}

const char *Binary2String(const char *pvBuff, int iSize)
{
    const int STR2HEX_MAX_BUFF_LEN = 10240;
    const int CHAR_NUM_PER_BYTE = 3;
    static char szStr2HexBuff[STR2HEX_MAX_BUFF_LEN * CHAR_NUM_PER_BYTE + 1] = {0};

    if (iSize > STR2HEX_MAX_BUFF_LEN)
    {
        iSize = STR2HEX_MAX_BUFF_LEN;
    }

    for (int i=0; i<iSize; i++)
    {
        snprintf(szStr2HexBuff + (i * CHAR_NUM_PER_BYTE), CHAR_NUM_PER_BYTE + 1, "%02hhX ", pvBuff[i]);
    }
    
    if (iSize == 0)
    {
        szStr2HexBuff[0] = 0;
    }

    return szStr2HexBuff;
}


/**
 * 字符串分割，写工具时可能会用。
 * 这种实现方法是我见过最优雅最简单的，出自这里：
 * http://stackoverflow.com/questions/599989/is-there-a-built-in-way-to-split-strings-in-c
 * @param str
 * @param delimiters
 * @param tokens
 */
void split(const std::string& str, const std::string& delimiters ,
        std::vector<std::string>& tokens)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void CStrTool::StringReplace(const string& sStr, const string& sOldSub,
    const string& sNewSub, bool bReplaceAll, string* psRes)
{
    if (sOldSub.empty())                
    { 
        psRes->append(sStr);  // if empty, append the given string.
        return;
    } 

    std::string::size_type iStartPos = 0;    
    std::string::size_type iPos;        
    do
    { 
        iPos = sStr.find(sOldSub, iStartPos);    
        if (iPos == string::npos)       
        {
            break;
        }
        psRes->append(sStr, iStartPos, iPos - iStartPos);
        psRes->append(sNewSub);         
        iStartPos = iPos + sOldSub.size();       
    } while (bReplaceAll);              
    psRes->append(sStr, iStartPos, sStr.length() - iStartPos);
}


string CStrTool::StringReplace(const string& sStr, const string& sOldSub,
                     const string& sNewSub, bool bReplaceAll) 
{
    string sRet;
    StringReplace(sStr, sOldSub, sNewSub, bReplaceAll, &sRet);
    return sRet;                        
}


uint64_t ntohll(uint64_t val)
{
    return (((uint64_t)htonl((int32_t )((val << 32) >> 32))) << 32)
            | (uint32_t)htonl((int32_t )(val >> 32));
}

uint64_t htonll(uint64_t val)
{
    return (((uint64_t)htonl((int32_t )((val << 32) >> 32))) << 32)
            | (uint32_t)htonl((int32_t )(val >> 32));
}

template<typename T>
std::string anytoStr(const T &t)
{
    ostringstream sBuffer;
    sBuffer << t;
    return sBuffer.str();
}

int WritePidFile( char* argv0 )
{
    string pid_file_path = "../bin/";
    string pid_file_name;
    pid_file_name.assign(basename(argv0));
    pid_file_name += ".pid";
    pid_file_path += pid_file_name;

    int pid_id = open(pid_file_path.c_str(), O_RDWR | O_CREAT, 0644);
    if (pid_id == -1)
    {
        return -1;
    }

    struct flock stLock;
    stLock.l_type = F_WRLCK;
    stLock.l_whence = SEEK_SET;
    stLock.l_start = 0;
    stLock.l_len = 0;

    int ret = fcntl(pid_id, F_SETLK, &stLock);
    if (ret == -1)
    {
        close(pid_id); // 加上这行免得coverity扫描告警
        return -1;
    }

    stringstream ss;
    ss << getpid() << endl;
    write(pid_id, ss.str().c_str(), ss.str().length());

    // 刻意不关闭文件,保证svr启动时,文件一直存在
    // close(pid_id);

    return 0;
}

int64_t TimeHelper::msbetween(const struct timespec& t1,
  const struct timespec& t2)
{
  if(t1 < t2)
  {
    return - ((t2.tv_sec - t1.tv_sec)*1000000 + (int64_t(t2.tv_nsec) - int64_t(t1.tv_nsec))/1000)/1000;
  }

  return ((t1.tv_sec - t2.tv_sec)*1000000 + (int64_t(t1.tv_nsec) - int64_t(t2.tv_nsec))/1000)/1000;
}

int64_t TimeHelper::msbetween(const struct timeval& t1,
  const struct timeval& t2)
{
  if(t1 < t2)
  {
    return - ((t2.tv_sec - t1.tv_sec)*1000000 + int64_t(t2.tv_usec) - int64_t(t1.tv_usec))/1000;
  }

  return ((t1.tv_sec - t2.tv_sec)*1000000 + int64_t(t1.tv_usec) - int64_t(t1.tv_usec))/1000;
}

uint32_t BKDRHash(const char *str)
{
    uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
    uint32_t hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return hash;
}

// 网上找到的一个算法检查溢出 by johnfu
/* Determine whether arguments can be added without overflow */
bool tadd_ok(int x, int y)
{
    #if 0
    return !(x<0&&y<0&&x+y>0 || x>0&&y>0&&x+y<0);
    #endif

    return !( ((x < 0) == (y < 0)) && ((x+y < 0)  !=  (x < 0 )) );
//    return  ((x < 0) != (y < 0)) || ((x+y < 0)  ==  (x < 0 )) ;
}
/* Determine whether arguments can be added without overflow */
bool uadd_ok(unsigned int x, unsigned int y)
{
    return !(x+y < x);
}

/* Determine whether argumnts can be substracted without overflow */
bool usub_ok(unsigned int x, unsigned int y)
{
    return !y || !uadd_ok(x, -y);
}

bool uadd_ok(unsigned int x, int y)
{
    if (y >= 0)
        return uadd_ok(x, (uint32_t)y);
    else
        return usub_ok(x, (uint32_t)abs(y));
}

unsigned int GetInnerIP()
{
    int s;
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return 0;
    }

    FileDescriptorGuard guard(s);

    char buf[BUFSIZ];
    struct ifconf ifc;
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(s, SIOCGIFCONF, (char *) &ifc) < 0)
    {
        return 0;
    }
    struct ifreq ifreq, *ifr;
    ifr = ifc.ifc_req;
    struct in_addr * p = NULL;
    unsigned int addr = 0;
    for (int len = ifc.ifc_len; len; len -= sizeof ifreq)
    {
        ifreq = *ifr;
        if (ioctl(s, SIOCGIFFLAGS, (char *) &ifreq) < 0)
        {
            return addr;
        }
        if ((ifreq.ifr_flags & IFF_UP) && ifr->ifr_addr.sa_family == AF_INET)
        {
            void* tmp_ptr = (void*)&ifr->ifr_addr;
            p = &(((struct sockaddr_in *)tmp_ptr)->sin_addr);
            unsigned char net = p->s_addr & 0xff;
            if (net != 127)
            {
                addr = p->s_addr;
                if (net == 10 || net == 172 || net == 192) return addr;
            }
        }
        ifr++;
    }

    return 0;
}

ShmKeyDump::ShmKeyDump() : init_(false) {}

int ShmKeyDump::Init(const string &proc_name, const string &bus_addr)
{
    string bak_shm_file_name;

    string time_str(TimeHelper::timet_to_datetime(time(NULL)));

    dump_file_name_ = proc_name + "_" + bus_addr + ".shm";
    bak_shm_file_name = dump_file_name_ + "." + time_str;

    if (access(dump_file_name_.c_str(), F_OK) != -1)
    {
        int ret = rename(dump_file_name_.c_str(), bak_shm_file_name.c_str());
        if(ret == -1)
        {
        	fprintf(stderr, "move file:%s fail", dump_file_name_.c_str());
        	return ret;
        }
    }

    int fd = open(dump_file_name_.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
    	fprintf(stderr, "create file:%s fail", dump_file_name_.c_str());
        return -1;
    }
    close(fd);
    init_ = true;
    return 0;
}

int ShmKeyDump::PushKey(key_t shm_key)
{
	if(!init_)
	{
		fprintf(stderr, "shm key dump not init");
		return -1001;
	}
    int fd = open(dump_file_name_.c_str(), O_RDWR | O_APPEND, 0644);
    if (fd == -1)
    {
    	fprintf(stderr, "push key open file:%s fail", dump_file_name_.c_str());
        return -2;
    }

    stringstream ss;
    ss << shm_key;
    string shm_key_str;
    ss >> shm_key_str;
    shm_key_str.append("\n");

    int n = write(fd, shm_key_str.c_str(), shm_key_str.size());
    close(fd);
    return n;
}

/**
 * 从电脑上连上来的客户端很多时候都没有openid，所以也没有gid，
 * 为了区分每个用户，在测试模式下将玩家的ip作为openid和gid，
 * dispatcher将数据包发送到zonesvr之前将account相关的部分替换掉。
 *                              franktang 2014-08-26
 * @param account
 */
void CheckOpenID(TFRAMEHEADACCOUNT& account, tdr_ip_t ip)
{
    if(TCONNAPI_ACCOUNT_NONE == account.wType)
    {
        account.ullUid = ::atoll(account.stValue.szSTRING);
        TRACE_LOG(0, 0, 0, "", "none pattern ullUid is 0, account type=%d, replaced openid to %s", account.wType, account.stValue.szSTRING);
    }
    else if(0 == account.ullUid)
    {
        struct in_addr addr;
        addr.s_addr = ip;
        TRACE_LOG(0, 0, 0, "", "old account string %s", account.stValue.szSTRING);
        strncpy(account.stValue.szSTRING, inet_ntoa(addr), sizeof(account.stValue.szSTRING));
        account.ullUid = ip;
        TRACE_LOG(0, 0, 0, "", "ullUid is 0, account type=%d, replaced openid to %s", account.wType, account.stValue.szSTRING);
    }
}

inline uint32_t HashOpenIDInner(const char *_p, int len)
{
    const unsigned char *p = (const unsigned char*)_p;
    uint32_t h = 0;

    for (int i = 0; i < len; i++ )
    {
        h += p[i];
        h += ( h << 10 );
        h ^= ( h >> 6 );
    }

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );
    return h;
}

uint64_t HashOpenID(const std::string& s)
{
    return ((uint64_t)HashOpenIDInner(s.c_str(), s.length() / 2) << 32) | HashOpenIDInner(s.c_str() + s.length() / 2, s.length() - s.length() / 2);
}


uint32_t GetPvpSkillIdOffset(uint32_t app_version)
{
    return (app_version > 0 && app_version < 113000000) ? PVP_SKILL_ID_OFFSET_V1 : PVP_SKILL_ID_OFFSET_V2;
}

string trim(const string& str)
{
    string::size_type pos = str.find_first_not_of(' ');
    if (pos == string::npos)
    {
        return str;
    }
    string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}
