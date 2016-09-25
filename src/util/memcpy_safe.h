/*
 * memcpy_safe.h
 *
 *  dbproxy因为memcpy越界发生过crash，重新定义一个函数，来防止这种情况发生。
 *  如果定义在util.h中，会存在于log循环引用的问题，所以单独定义在一个头文件中。
 *  Example:
 *  memcpy_safe(ctx_->cmd_ctx.context, bus_pkg.body, bus_pkg.pkg_head.body_len, __FILE__, __LINE__);
 *
 *  Created on: Mar 26, 2015
 *      Author: frank
 */

#ifndef MEMCPY_SAFE_H_
#define MEMCPY_SAFE_H_

#include <string.h>
#include "log/log.h"

/**
 * 用这个函数，防止邪恶的memcpy发生越界。如果越界，不会执行copy，而是打一行error日志
 * @param dest
 * @param src
 * @param copy_len
 * @param source_file 传入__FILE__
 * @param source_line 调用的时候，这个参数请传__LINE__，这样才知道哪里错了。
 * @return 返回值意义不大，一般不会去管。
 */
template<size_t length>
bool memcpy_safe(char (&dest)[length], const char* src, const size_t copy_len,
        const char* source_file,
        const int32_t source_line)
{
    if(copy_len > length)
    {
        ERR_LOG(0, 0, 0, "", "memcpy out of bounds! dest size is only %zu, but copy_len is %zu, source file=%s, line=%d",
                length, copy_len, source_file, source_line);
        return false;
    }
    memcpy(dest, src, copy_len);
    return true;
}



#endif /* MEMCPY_SAFE_H_ */
