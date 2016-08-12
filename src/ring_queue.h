/*========================================================================
#   FileName: ring_queue.h
#     Author: kevinlin
#      Email: linjiang1205@qq.com
#   History:  在session中使用了两个队列，为了持久化，需要固定长度，所以这里随手实现了一个循环队列
#               接口的实现尽量模仿std::queue
# LastChange: 2015-09-13 16:29:02
========================================================================*/
#ifndef _RING_QUEUE_H_
#define _RING_QUEUE_H_

template<typename T, int cap>
class RingQueue
{
    static const int kErrStorageFull = -1;
public:
    RingQueue():head_(0), tail_(0){}

    const T& back()const
    {
        return storage_[(tail_ + cap) % (cap + 1)];
    }

    //是否为空
    bool empty()const { return head_ == tail_; }

    //返回首元素
    const T& front()const
    {
        return storage_[head_];
    }

    size_t size()const
    {
        if(head_ <= tail_)
        {
            return tail_ - head_;
        }
        else
        {
            return tail_ + cap + 1 - head_;
        }
    }

    //弹出首元素
    void pop()
    {
        if(!empty())
        {
            head_ = (head_ + 1) % (cap + 1);
        }
    }

    //插入元素到末尾
    int push(const T& elem)
    {
        if(!full())
        {
            storage_[tail_] = elem;
            tail_ = (tail_ + 1) % (cap + 1);
            return 0;
        }
        else
        {
            return kErrStorageFull;
        }
    }

    void clear()
    {
        head_ = 0;
        tail_ = 0;
    }

private:
    bool full()const { return ((tail_ + 1) % (cap + 1) ) == head_; }

private:
    int head_;
    int tail_;
    T storage_[cap + 1];
};



#endif


