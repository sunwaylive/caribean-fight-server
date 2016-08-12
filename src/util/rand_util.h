/*
 * rand_util.h
 *
 *	抽奖中用到的随机，封装成统一稳定的函数，
 *	便于使用和测试。
 *
 *  Created on: Apr 6, 2015
 *      Author: frank
 */

#ifndef __LIBSRC_UTIL_RAND_UTILS_H__
#define __LIBSRC_UTIL_RAND_UTILS_H__

#include <vector>
#include <iostream>
#include <algorithm>
#include "util/util.h"

template<class T>
struct RandItem
{
	T data;
	size_t weight;
};

/**
 * 从N个元素里面随机一个出来，每个的概率不同。
 * 模板参数T所属于的结构体不管叫什么名字，只要有一个名字叫weight的成员变量就可以了，直接调这个函数。
 * 如果模板参数T不含有weight名字的成员，则可以先用类似上面定义的struct RandItem的结构体封装一次，
 * 再来调用这个函数。
 */
//template<class T>
//bool RandomOne(const std::vector<T>& v, T& result)
//{
	//if(v.empty()) return false;

	//std::vector<size_t> probs(v.size(), 0);
	//for(size_t i = 0; i < v.size(); ++i)
	//{
		//probs[i] = v[i].weight;
		//if(i > 0)
		//{
			//probs[i] += probs[i-1];
		//}
	//}

	//size_t total = probs[probs.size() - 1];
	//size_t value = NumberHelper::ARandRange(0, total);
	//auto it = std::lower_bound(probs.begin(), probs.end(), value);
	//result = v[std::distance(probs.begin(), it)];
	//return true;
//}

//2015年8月3日 19:18:52@kevin 另外一种N选1的随机算法, 解决上面的RandomOne算法中weight=0会被随机到的问题
template<class T>
bool RandomOne(const std::vector<T>& v, T& result)
{
	if(v.empty()) return false;

    int32_t total_weight = 0;
    size_t idx = 0;
    for(size_t i = 0; i < v.size(); ++i)
	{
        total_weight += v[i].weight;
        double rand = NumberHelper::Random01();
        if(rand * total_weight < v[i].weight)
        {
            idx = i;
        }
	}

	result = v[idx];
	return true;
}


/**
 * 从一个列表里面随机n个，每个独立计算，通过另一个vector返回。
 * 随机的最大值通过参数max_rand传递。
 */
template <class T>
void RandomN(const std::vector<T>& input, uint32_t max_rand, std::vector<T>& result)
{
	for(size_t i = 0; i < input.size(); ++i)
	{
		if(NumberHelper::ARandRange(1, max_rand) <= int32_t(input[i].weight))
		{
			result.push_back(input[i]);
		}
	}
}

#endif /* __LIBSRC_UTIL_RAND_UTILS_H__ */
