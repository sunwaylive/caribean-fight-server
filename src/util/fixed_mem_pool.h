/*
 * Author: sunway
 * Email: sunwayliving@gmail.com
 * Date: 2016-09-19
 * Tips: a better choice: tcmalloc from google, open source, faster
 * */

#ifndef UTIL_FIXED_MEM_POOL_H_
#define UTIL_FIXED_MEM_POOL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <type_traits>

struct LinkedList {
    size_t next;
    size_t prev;

    LinkedList() : next(0), prev(0) {
    }
};

/*
 Total size if fixed after initialization
--MemHeader
   max_size
   ...
   ...
--data
   ...
   ...
 * */

class FixedMemPool {
    public:
        //next = 0 means block is used, not in the free list
        //next = kInvalidPointer means this block is the end of free list
        //other values is used to link the free list, point to the next free node
        struct AllocBlock {
            size_t next;
            char node[0];
        };

        static const size_t kInvalidPointer = static_cast<size_t>(-1);
        static const size_t kNodeOffset = offsetof(AllocBlock, node);

        struct MemHeader {
            size_t max_size;
            uint32_t max_node_num;
            size_t used_size;
            uint32_t used_node_num;
            size_t node_size;
            size_t block_size;
            size_t free_list_offset;
        };

        FixedMemPool() : m_header(NULL), m_data(NULL) {
            memset(m_error_msg, 0, sizeof(m_error_msg));
            static_assert(sizeof(MemHeader) % 8 == 0, "MemHeader size MUST be 8*N");
        }

        virtual ~FixedMemPool() {
        }

    public:
        /*
        --Block
        -- next
        -- node
         */
        //block start address = node address - node offset from block start address
        AllocBlock* GetBlock(const void *node) const {
            return (AllocBlock*)((char*)node - kNodeOffset);
        }

        //m_data is the address right after MemHeader
        const AllocBlock* GetFirstBlock() const {
            const AllocBlock *block = (AllocBlock*)m_data;
            return IsValidBlock(block) ? block : NULL;
        }

        const AllocBlock* GetNextBlock(const AllocBlock *block) const {
            if (block == NULL) {
                return GetFirstBlock();
            }

            //block start address + block size
            const AllocBlock *next = (AllocBlock*)((char*)block + m_header->block_size);
            return IsValidBlock(next) ? next : NULL;
        }

        const AllocBlock* GetNextUsedBlock(const AllocBlock *block = NULL) const {
            while((block = GetNextBlock(block)) != NULL) {
                //0 means not in the free list
                if (block->next == 0) {
                    return block;
                }
            }

            return NULL;
        }

        const void* GetNextNode(const void* node = NULL) {
            const FixedMemPool::AllocBlock* block = NULL;
            if (node) {
                block = GetBlock(node);
            }

            block = GetNextUsedBlock(block);
            if (block) {
                return block->node;
            } else {
                return NULL;
            }
        }

    public:
        int32_t Init(void *mem, uint32_t max_node_num, size_t node_size, bool check_header = false);

        void* Alloc(bool zero = true);

        int32_t Free(void *node);

        inline const MemHeader* Header() const {
            return m_header;
        }

        inline const char* GetErrorMsg() const {
            return m_error_msg;
        }

        //get offset from header
        inline size_t GetOffsetFromHead(void *p) const {
            return (size_t)((intptr_t)p - (intptr_t)m_header);
        }

        //get the address of the pos offset from header
        inline void* GetAddrOfOffset(size_t pos) const {
            return (char*)m_header + pos;
        }

        /*compute memory needed to allocate max_node_num nodes*/
        static size_t GetAllocSize(uint32_t max_node_num, size_t node_size) {
            return sizeof(MemHeader) + max_node_num * GetBlockSize(node_size);
        }

    protected:
        static size_t GetBlockSize(size_t node_size) {
            //why?
            return (node_size + kNodeOffset + 7) & ~7;
        }

        //block size is based on node size, aligned by 8 bytes
        size_t GetBlockSize() {
            return GetBlockSize(m_header->node_size);
        }

        static bool CheckHeader(uint32_t max_node_num, size_t node_size, MemHeader *header) {
            size_t max_size = GetAllocSize(max_node_num, node_size);
            if ((header->node_size != node_size) || (header->block_size != GetBlockSize(node_size))
                || (header->max_node_num != max_node_num) || (header->max_size != max_size)
                || (sizeof(MemHeader) + header->used_node_num * header->block_size != header->used_size)
                || (header->used_size > max_size)) {
                    return false;
                }

            return true;
        }

        bool IsValidBlock(const AllocBlock *block) const {
            if (block < m_data) {
                printf("Debug: block: %p, m_date: %p\n", block, m_data);
                return false;
            }

            // %zu is used for size_t
            if ((size_t)block + m_header->block_size > (size_t)m_header + m_header->used_size) {
                printf("block end size: %zu, alloc_size: %zu",
                       (size_t)block + m_header->block_size, (size_t)m_header + m_header->used_size);
                return false;
            }

            if (GetOffsetFromHead(const_cast<AllocBlock*>(block)) % m_header->block_size
                != sizeof(MemHeader) % m_header->block_size) {
                printf("actual mod:%zu, theory mod: %zu\n",
                       GetOffsetFromHead(const_cast<AllocBlock*>(block)) % m_header->block_size,
                       sizeof(MemHeader) % m_header->block_size);
                return false;
            }

            return true;
        }

        //block->next != 0 means block is in free list
        bool IsUsedBlock(const AllocBlock *block) const {
            return IsValidBlock(block) && (block->next == 0);
        }

        //insert p to free list
        void LinkFreeBlock(AllocBlock *p) {
            p->next = m_header->free_list_offset;
            //only here modifies free_list_offset
            m_header->free_list_offset = GetOffsetFromHead(p);
        }

        //this function is called in Alloc
        //after Init(), m_header->free_list_offset is kInvalidPointer, so NULL is returned
        //and Alloc() will get a new block from the pool
        AllocBlock* GetFreeBlock() {
            AllocBlock *p = NULL;
            //m_header->free_list_offset is always kInvalidPointer if no block is freed
            if(m_header->free_list_offset == kInvalidPointer) {
                p = NULL;
            } else {
                //we directly used the offset to get the address
                p = (AllocBlock*)GetAddrOfOffset(m_header->free_list_offset);
                //remove p in the free list
                m_header->free_list_offset = p->next;
            }

            return p;
        }

    private:
        MemHeader *m_header;
        void      *m_data;
        char      m_error_msg[256];
};

#endif
