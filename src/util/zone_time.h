/*
 * zone_time.h
 *
 *  Created on: 2014-11-8
 *      Author: johnyao
 */

#ifndef ZONE_TIME_H_
#define ZONE_TIME_H_

#include "util/util.h"

class CZoneTime
{
private:
    CZoneTime()
    {
        distanceTime = 0;
        RefreshTime();
        RefreshTimeZone();
    }
public:
    static CZoneTime& GetInst()
    {
        static CZoneTime s_inst;
        return s_inst;
    }

    ~CZoneTime()
    {

    }

    inline void RefreshTime()
    {   realUsec_= TimeHelper::GetCurUSec();
        realmsec_=realUsec_/1000;
        realTime_ = realUsec_ / 1000000 ;
        localtime_r(&realTime_, &real_st_time_);
        realdaysec_ = real_st_time_.tm_hour * 3600 + real_st_time_.tm_min * 60 + real_st_time_.tm_sec;

        usec_ = realUsec_+ ((int64_t)distanceTime*1000000);
        msec_=usec_/1000;
        time_ = usec_ / 1000000 ;
        localtime_r(&time_, &st_time_);
        daysec_ = st_time_.tm_hour * 3600 + st_time_.tm_min * 60 + st_time_.tm_sec;
    }

    inline const uint64_t & GetRealTimeMSec()
    {
        return realmsec_;
    }
    inline const uint64_t & GetMSec()
    {
        return msec_;
    }
    inline const uint64_t & GetRealUSec()
    {
        return realUsec_;
    }
    inline const uint64_t & GetUSec()
    {
        return usec_;
    }
    inline const time_t & GetRealTime()
   {
       return realTime_;
   }

    inline const time_t & GetTime()
    {
        return time_;
    }

    inline const int & GetRealDaySec()
    {
        return realdaysec_;
    }

    inline const int & GetDaySec()
    {
        return daysec_;
    }

    inline const struct tm & GetStructTime()
    {
        return st_time_;
    }

    uint32_t GetUintYMDH()
    {
        return (st_time_.tm_year - 100) * 1000000 + (st_time_.tm_mon + 1) * 10000 + (st_time_.tm_mday) * 100 + st_time_.tm_hour;
    }
    inline void setDistanceTime(int32_t dTime)
    {
        distanceTime = dTime;
    }

    inline int32_t getDistanceTime()
   {
      return distanceTime ;
   }

    inline int8_t getTimeZone()
    {
    	return time_zone_;
    }

    inline bool IsSameDay(time_t time1, time_t time2, uint32_t day_start_hour)
    {
        return (time1 + 8 * 3600 - day_start_hour * 3600) / 86400 == (time2 + 8 * 3600 - day_start_hour * 3600) / 86400;
    }

protected:
    inline void RefreshTimeZone()
    {
        time_t time_utc;
        struct tm tm_local;

        // Get the UTC time
        time(&time_utc);

        // Get the local time
        // Use localtime_r for threads safe
        localtime_r(&time_utc, &tm_local);

        time_t time_local;
        struct tm tm_gmt;

        // Change tm to time_t
        time_local = mktime(&tm_local);

        // Change it to GMT tm
        gmtime_r(&time_utc, &tm_gmt);

        time_zone_ = tm_local.tm_hour - tm_gmt.tm_hour;
        if (time_zone_ < -12) {
        	time_zone_ += 24;
        } else if (time_zone_ > 12) {
        	time_zone_ -= 24;
        }
    }

protected:
    //偏移时间
    int32_t distanceTime;
    time_t realTime_;
    uint64_t realUsec_;
    uint64_t realmsec_;
    int realdaysec_;

    time_t time_;
    uint64_t usec_;
    uint64_t msec_;
    int daysec_;
    struct tm st_time_;
    struct tm real_st_time_;

    int8_t time_zone_; // 时区 -12 ~ 12
};

// 20150723 frank 在住循环里面用来统计每步操作所消耗的时间的类
class TimeCounter
{
public:
	TimeCounter()
	{
		clock_gettime(CLOCK_MONOTONIC, &last_time_);
	}

	uint64_t MsToPrevTime() // 当前时间距离上一次时间的毫秒数
	{
		struct timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		uint64_t result = TimeHelper::msbetween(t, last_time_);
		last_time_ = t;
		return result;
	}
private:
	struct timespec last_time_;
};

#endif /* ZONE_TIME_H_ */
