#ifndef C0C_MEMORY_POOL_H
#define C0C_MEMORY_POOL_H

#include <cstddef>
#include <vector>

class MemoryPool
{
public:
    MemoryPool() : m_allocated(0) {}
    virtual ~MemoryPool() {}
    MemoryPool(const MemoryPool &other) = delete;
    MemoryPool &operator=(const MemoryPool &other) = delete;
    virtual void *Alloc() = 0;
    virtual void Free(void *addr) = 0;
    virtual void Clear() = 0;

protected:
    size_t m_allocated;
};

template <class T>
class MemoryPoolImp : public MemoryPool
{
public:
    MemoryPoolImp() : m_root(nullptr) {}
    virtual ~MemoryPoolImp() {}
    MemoryPoolImp(const MemoryPool &other) = delete;
    MemoryPoolImp &operator=(MemoryPool &other) = delete;
    virtual void *Alloc();
    virtual void Free(void *addr);
    virtual void Clear();

private:
    enum { COUNT = (4 * 1024) / sizeof(T) };

    union Chunk
    {
        Chunk *m_next;
        char m_mem[sizeof(T)];
    };

    struct Block
    {
        Block()
        {
            for (size_t i = 0; i < COUNT - 1; ++i)
                m_chunks[i].m_next = &m_chunks[i + 1];
            m_chunks[COUNT - 1].m_next = nullptr;
        }
        Chunk m_chunks[COUNT];
    };

    std::vector<Block *> m_blocks;
    Chunk *m_root;
};

template <class T>
void *MemoryPoolImp<T>::Alloc()
{
    if (nullptr == m_root) {  // 空间不够，需要分配空间
        auto block = new Block();
        m_root = block->m_chunks;
        // 如果blocks实现为std::list, 那么push_back实际的overhead更大
        // 这也表明，即使我们不需要随机访问功能(那么std::vector的拷贝是一种overhead)，
        // 仍然倾向于使用std::vector，
        // 当然std::vector的指数级capacity增长会造成内存浪费。
        m_blocks.push_back(block);
    }

    auto ret = m_root;
    m_root = m_root->m_next;

    ++m_allocated;
    return ret;
}

template <class T>
void MemoryPoolImp<T>::Free(void *addr)
{
    if (nullptr == addr)
        return;

    auto chunk = static_cast<Chunk *>(addr);
    chunk->m_next = m_root;
    m_root = chunk;

    --m_allocated;
}

template <class T>
void MemoryPoolImp<T>::Clear()
{
    for (auto block : m_blocks)
        delete block;

    m_blocks.resize(0);
    m_root = nullptr;
    m_allocated = 0;
}

#endif  // !C0C_MEMORY_POOL_H
