#include "fixed_mem_pool.h"
#include <stdio.h>

int32_t FixedMemPool::Init(void* mem, uint32_t max_node_num, size_t node_size, bool check_header) {
    if (!mem) {
        return -1;
    }

    m_header = (MemHeader*)mem;

    if (!check_header) {
        //init head
        m_header->max_size = GetAllocSize(max_node_num, node_size);
        m_header->max_node_num = max_node_num;
        //we use the beginning of the memory pool to as head infomation
        m_header->used_size = sizeof(MemHeader);
        m_header->used_node_num = 0;
        //block size is based on node size aligned by 8 bytes
        m_header->block_size = GetBlockSize(node_size);
        //no block is freed
        m_header->free_list_offset = kInvalidPointer;

        m_data = (char*)m_header + sizeof(MemHeader);
    } else {
        if (!CheckHeader(max_node_num, node_size, m_header)) {
            snprintf(m_error_msg, sizeof(m_error_msg), "Memory pool head check error!");
            return -1;
        }

        m_data = (char*)m_header + sizeof(MemHeader);
    }

    return 0;
}

/*The allocate works in this way:
At the beginning, no block is allocated, we need to find the free address,
we use GetAddrOfOffset().
As time goes by, more and more blocks are allocated using the above method,
until when some blocks are freed by user, then GetFreeBlock() can get a
second hand block, we just need to clear the memory to put it into use again.
 */
void* FixedMemPool::Alloc(bool zero) {
    AllocBlock* p = GetFreeBlock();
    if (p == NULL) {
        //can not find free block, we need to allocate from the pool
        if (m_header->used_size + m_header->block_size <= m_header->max_size) {
            p = (AllocBlock*)GetAddrOfOffset(m_header->used_size);
            //a new block is used
            m_header->used_size += m_header->block_size;
            m_header->used_node_num++;
        } else {
            snprintf(m_error_msg, sizeof(m_error_msg), "Alloc() error, No space left in the pool");
        }
    } else if (!IsValidBlock(p)){
        snprintf(m_error_msg, sizeof(m_error_msg), "Alloc() error, wrong free block format");
        p = NULL;
    }

    if (p == NULL) {
        return NULL;
    }

    //if use former freed block, reset the memory
    if (zero) {
        memset(p, 0, m_header->block_size);
    }

    //0 mean p is not in the free list
    p->next = 0;
    return p->node;
}

int32_t FixedMemPool::Free(void* node) {
    AllocBlock* block = GetBlock(node);
    if (IsUsedBlock(block)) {
        LinkFreeBlock(block);
    } else {
        snprintf(m_error_msg, sizeof(m_error_msg),
                 "Free() error, invalid pointer %p", node);
        return -1;
    }

    return 0;
}
