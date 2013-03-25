#include <duke/memory/Allocator.h>

#include <cassert> // assert
#include <cstring> // memset
// constant for block allocation
const unsigned char ChunkAllocatorBlocks = CHUNK_ALLOCATOR_BLOCKS;

struct ChunkAllocator;
// forward declaring

// Sentinel code
struct Sentinel {
	unsigned int magic;
	ChunkAllocator* pAllocator;
};

inline static Sentinel* getSentinelFromBlockStart(void* pBlockStart) {
	return reinterpret_cast<Sentinel*>(pBlockStart) - 1;
}

// boundary helpers
inline static size_t lowerPage(size_t ptr) {
	return (ptr >> PAGE_SIZE_BITS) << PAGE_SIZE_BITS;
}

inline static size_t upperPage(size_t ptr) {
	return ((ptr >> PAGE_SIZE_BITS) + 1) << PAGE_SIZE_BITS;
}

inline static bool alignedToPage(size_t ptr) {
	return ptr == lowerPage(ptr);
}

// size helpers
inline static size_t getChunkSize(size_t chunkSize) {
	// each chunk as a sentinel
	const size_t realChunkSize = chunkSize + sizeof(Sentinel);
	return alignedToPage(realChunkSize) ? realChunkSize : upperPage(realChunkSize);
}

inline static size_t getAllocationSize(size_t chunkSize) {
	const size_t allAlignedChunkSize = getChunkSize(chunkSize) * ChunkAllocatorBlocks;
	// adding one PAGE_SIZE to ensure we can offset and give aligned data
	return allAlignedChunkSize + PAGE_SIZE;
}

/**
 * Reserve CHUNK blocks of contiguous memory aligned on PAGE_SIZE
 * you can then allocate and deallocate CHUNK blocks of size ChunkSize
 */
struct ChunkAllocator {
public:
	const size_t m_ChunkSize;

private:
	unsigned char m_AvailableSlots;
	char* m_pData;
	bool m_UsedSlot[ChunkAllocatorBlocks];
	void* m_SlotData[ChunkAllocatorBlocks];

private:
	inline unsigned char findFreeSlot() const {
		unsigned char i = 0;
		for (; i < ChunkAllocatorBlocks; ++i)
			if (m_UsedSlot[i] == false)
				return i;
		assert(false);
		return i;
	}

	inline unsigned char findSlot(void* pData) const {
		unsigned char i = 0;
		for (; i < ChunkAllocatorBlocks; ++i)
			if (m_SlotData[i] == pData)
				return i;
		assert(false);
		return i;
	}

public:
	ChunkAllocator(size_t chunkSize) :
			m_ChunkSize(chunkSize), m_AvailableSlots(ChunkAllocatorBlocks), m_pData((char*) malloc(getAllocationSize(chunkSize))) {
		assert(PAGE_SIZE == (size_t) getPageSize());
		memset(m_UsedSlot, false, ChunkAllocatorBlocks);
		char* pBlockStart = reinterpret_cast<char*>(upperPage(reinterpret_cast<size_t>(m_pData)));
		const size_t realChunkSize = getChunkSize(chunkSize);
		for (int i = 0; i < ChunkAllocatorBlocks; ++i) {
			m_SlotData[i] = pBlockStart;
			Sentinel *pSentinel = getSentinelFromBlockStart(pBlockStart);
			pSentinel->pAllocator = this;
			pSentinel->magic = 0xDEADC0DE;
			pBlockStart += realChunkSize;
		}
	}
	~ChunkAllocator() {
		::free(m_pData);
	}
	inline bool isNotFull() const {
		return m_AvailableSlots != 0;
	}
	inline bool isNotEmpty() const {
		return m_AvailableSlots != ChunkAllocatorBlocks;
	}
	void* allocate() {
		assert(isNotFull());
		const unsigned char index = findFreeSlot();
		--m_AvailableSlots;
		m_UsedSlot[index] = true;
		return m_SlotData[index];
	}
	void free(void* pData) {
		assert(isNotEmpty());
		m_UsedSlot[findSlot(pData)] = false;
		++m_AvailableSlots;
	}
};

/**
 * Maintains a list of dynamically allocated Chunks
 */
#include <list>   // list
#include <memory> // unique_ptr
struct ChunkManager {
	ChunkManager(const size_t chunkSize) :
			m_ChunkSize(chunkSize) {
	}
	void* allocate() {
		auto pCurrent = m_Chunks.begin();
		const auto pEnd = m_Chunks.end();
		for (; pCurrent != pEnd; ++pCurrent)
			if ((*pCurrent)->isNotFull())
				break;
		if (pCurrent == pEnd) { // time to allocate a new chunk
			// putting chunk front so next time we will find it right away
			m_Chunks.emplace_front(new ChunkAllocator(m_ChunkSize));
			pCurrent = m_Chunks.begin();
		}
		return (*pCurrent)->allocate();
	}
private:
	const size_t m_ChunkSize;
	std::list<std::unique_ptr<ChunkAllocator> > m_Chunks;
};

#include <mutex>
#include <map>
struct BigAlignedBlock::BigAlignedBlockImpl {
	inline void* malloc(const size_t size) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		auto itr = m_SizeToManager.find(size);
		if (itr == m_SizeToManager.end())
			itr = m_SizeToManager.insert(std::make_pair(size, std::unique_ptr<ChunkManager>(new ChunkManager(size)))).first;
		return itr->second->allocate();

	}
	inline void free(void * pData) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		const Sentinel *pSentinel = getSentinelFromBlockStart(pData);
		assert(pSentinel->magic == 0xDEADC0DE);
		assert(pSentinel->pAllocator);
		pSentinel->pAllocator->free(pData);
	}
	std::mutex m_Mutex;
	std::map<size_t, std::unique_ptr<ChunkManager>> m_SizeToManager;
};

BigAlignedBlock::BigAlignedBlock() :
		pImpl(new BigAlignedBlockImpl()) {
}

BigAlignedBlock::~BigAlignedBlock() {
	delete pImpl;
}

void* BigAlignedBlock::malloc(const size_t size) const {
	return pImpl->malloc(size);
}

void BigAlignedBlock::free(void* ptr) const {
	if (ptr)
		pImpl->free(ptr);
}
