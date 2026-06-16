#pragma once

#include <cstdint>
#include <vector>

namespace bc
{
    template <std::size_t ChunkSize, std::size_t ChunksPerBlock = 8, bool EnableDebug = false>
    class RawMemoryPool
    {
    protected:
        // Provide 2 types of Chunk according to EnableDebug
        template <bool ChunkEnableDebug>
        struct Chunk;
        template <>
        struct Chunk<true>
        {
            Chunk(Chunk* next = nullptr) :
                m_pNext(next), m_chunkID(++s_chunkID)
            {
                LOG_INFO("create chunk #", m_chunkID, " ", this, " -> ", next, "\n");
            }
            union
            {
                Chunk<true>* m_pNext;
                uint8_t m_storage[ChunkSize];
            };

            // Debug members
            static constexpr std::size_t s_chunkID = 0;
            std::size_t m_chunkID;
        };

        template <>
        struct Chunk<false>
        {
            Chunk(Chunk* next = nullptr) :
                m_pNext(next)
            {
            }

            union
            {
                Chunk<false>* m_pNext;
                uint8_t m_storage[ChunkSize];
            };
        };

        using ChunkType = Chunk<EnableDebug>;
        using ChunkPtrType = ChunkType*;
        static constexpr auto kNumChunksExceptTail = ChunksPerBlock - 1;

    public:
        // Chunk size should not less than the size of ptr
        //static_assert(ChunkSize >= sizeof(void*));
        //static_assert(ChunksPerBlock > 0);

        RawMemoryPool()
        {
            // Empty constructor is enough for now
        }

        ~RawMemoryPool()
        {
            // Free all memory usages when destruct
            for (auto block : m_allocatedBlocks)
                std::free(block);
        }

        // Disable copy
        RawMemoryPool(const RawMemoryPool&) = delete;
        RawMemoryPool& operator=(const RawMemoryPool&) = delete;

        // Enable move
        RawMemoryPool(RawMemoryPool&& other) noexcept
        {
            // Call move assignment
            this->operator=(std::move(other));
        }
        RawMemoryPool& operator=(RawMemoryPool&& other) noexcept
        {
            m_numChunks = other.m_numChunks;
            m_pNextChunk = other.m_pNextChunk;
            m_allocatedBlocks = std::move(other.m_allocatedBlocks);
            other.m_numChunks = 0;
            other.m_pNextChunk = nullptr;
            return *this;
        }

        // Reserve a number of blocks before allocate
        void ReserveBlocks(std::size_t blocks)
        {
            m_allocatedBlocks.reserve(blocks);
            while (blocks > m_allocatedBlocks.size())
                m_pNextChunk = AllocateBlock(m_pNextChunk);
        }

        // Find the available chunk, if there is no one available create a new block.
        void* AllocateRaw()
        {
            if (!m_pNextChunk)
                m_pNextChunk = AllocateBlock(nullptr);

            ChunkPtrType chunk = m_pNextChunk;
            m_pNextChunk = chunk->m_pNext;
            ++m_numChunks;
            if constexpr (EnableDebug)
            {
                LOG_INFO("AllocateRaw chunk #", chunk->m_chunkID, "\n");
            }
            return chunk;
        }

        // Put the ptr back to the pool in order to reuse without re-allocate memory
        void FreeRaw(void* ptr)
        {
            ChunkPtrType chunk = reinterpret_cast<ChunkPtrType>(ptr);

            chunk->m_pNext = m_pNextChunk;
            m_pNextChunk = chunk;
            --m_numChunks;
            if constexpr (EnableDebug)
            {
                LOG_INFO("FreeRaw chunk #", m_pNextChunk->m_chunkID, " ", ptr, "\n");
            }
        }

        bool IsEmpty() const
        {
            return !GetNumChunks();
        }
        bool IsFull() const
        {
            return GetNumChunks() == GetChunkCapacity();
        }
        std::size_t GetNumChunks() const
        {
            return m_numChunks;
        }
        std::size_t GetNumBlocks() const
        {
            return m_allocatedBlocks.size();
        }
        std::size_t GetChunkCapacity() const
        {
            return GetNumBlocks() * ChunksPerBlock;
        }

    protected:
        ChunkPtrType AllocateBlock(ChunkPtrType tail)
        {
            ChunkPtrType pBlock = reinterpret_cast<ChunkPtrType>(std::malloc(sizeof(ChunkType) * ChunksPerBlock));
            if (!pBlock)
                throw std::bad_alloc();

            m_allocatedBlocks.emplace_back(pBlock);

            // Initialize all chunks in this new blocks except the tail
            ChunkPtrType pChunk = pBlock;
            for (std::size_t i = 0; i < kNumChunksExceptTail; ++i)
            {
                new (pChunk) ChunkType(pChunk + 1);
                pChunk = pChunk->m_pNext;
            }

            // Initialize tail
            new (pChunk) ChunkType(tail);

            return pBlock;
        }

        std::size_t m_numChunks = 0;
        ChunkPtrType m_pNextChunk = nullptr;

        std::vector<ChunkPtrType> m_allocatedBlocks;
    };

    // A Typed Memory Pool extend RawMemoryPool to enable class construction and destruction
    template <typename TypeInPool, std::size_t ChunksPerBlock = 8, bool EnableDebug = false>
    class TypedMemoryPool : public RawMemoryPool<sizeof(TypeInPool), ChunksPerBlock, EnableDebug>
    {
    public:
        using RawMemoryPoolType = RawMemoryPool<sizeof(TypeInPool), ChunksPerBlock, EnableDebug>;

    public:
        // Allocate new memory and initialize with type
        template <typename... Args>
        TypeInPool* AllocateWithType(Args&&... constructorArgs)
        {
            auto* pRaw = RawMemoryPoolType::AllocateRaw();

            // Placement new to call constructor in allocated address.
            auto* out = new (pRaw) TypeInPool(std::forward<Args>(constructorArgs)...);
            return out;
        }

        // Destruct with destructor of the given type and free into pool.
        void FreeWithDestructor(TypeInPool** ppType)
        {
            if (!ppType)
                return;
            auto* pType = *ppType;
            if (pType)
            {
                // Call destructor explicitly
                pType->~TypeInPool();
                RawMemoryPoolType::FreeRaw(pType);
                *ppType = nullptr;
            }
        }
    };
}

