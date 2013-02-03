#include <gtest/gtest.h>

#include <duke/memory/PageSize.h>

TEST(Allocation,PageSize) {
	EXPECT_EQ(PAGE_SIZE, getPageSize());
}

#include <duke/memory/Allocator.h>
#include <memory>
#include <vector>

TEST(Allocation,Alignement) {
	std::vector<std::unique_ptr<Allocator>> allocators;
	allocators.emplace_back(new AlignedMalloc());
	allocators.emplace_back(new Malloc());
	allocators.emplace_back(new New());
	allocators.emplace_back(new BigAlignedBlock());
	for (const auto &pAllocator : allocators) {
		EXPECT_NE(0, pAllocator->alignment());
		void *pData = pAllocator->malloc(1);
		EXPECT_NE(nullptr, pData);
		EXPECT_TRUE(reinterpret_cast<size_t>(pData) % pAllocator->alignment()==0);
		pAllocator->free(pData);
	}
}

#include <algorithm>
TEST(Allocation,BigAllocator) {
	BigAlignedBlock allocator;
	// allocator should give back same memory
	void * const pFirstData = allocator.malloc(1);
	allocator.free(pFirstData);
	void * const pSecondData = allocator.malloc(1);
	allocator.free(pSecondData);
	EXPECT_EQ(pFirstData, pSecondData);

	/**
	 * allocator should get back same memory
	 * on several allocation from the same chunk
	 */
	std::set<void*> allAdresses;
	std::vector<void*> data;
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < CHUNK_ALLOCATOR_BLOCKS; ++i)
			data.push_back(allocator.malloc(1));
		allAdresses.insert(data.begin(), data.end());
		std::random_shuffle(data.begin(), data.end());
		for (; !data.empty(); data.pop_back())
			allocator.free(data.back());
	}
	EXPECT_EQ(CHUNK_ALLOCATOR_BLOCKS, allAdresses.size());
}

TEST(Allocation,freeNullPtr) {
	std::vector<std::unique_ptr<Allocator>> allocators;
	allocators.emplace_back(new AlignedMalloc());
	allocators.emplace_back(new Malloc());
	allocators.emplace_back(new New());
	allocators.emplace_back(new BigAlignedBlock());
	for (const auto &pAllocator : allocators) {
		pAllocator->free(nullptr);
	}
}

struct DebugAllocator: public Allocator {
	DebugAllocator() :
			allocated(false), freed(false), size(0) {
	}
	virtual void* malloc(const size_t _size) const {
		assert(!allocated);
		allocated = true;
		size = _size;
		return nullptr;
	}
	virtual void free(void* ptr) const {
		assert(!freed);
		freed = true;
	}
	virtual size_t alignment() const {
		return 1;
	}
	virtual const char * name() const {
		return "DebugAllocator";
	}
	mutable bool allocated;
	mutable bool freed;
	mutable size_t size;
};

TEST(Allocation, sharedalloc) {
	DebugAllocator allocator;

	ASSERT_FALSE(allocator.allocated);
	ASSERT_FALSE(allocator.freed);
	{
		auto pMemory = make_shared_memory<char>(1000, allocator);
		ASSERT_TRUE(allocator.allocated);
		ASSERT_FALSE(allocator.freed);
		ASSERT_EQ(1000, allocator.size);
	}
	ASSERT_TRUE(allocator.allocated);
	ASSERT_TRUE(allocator.freed);
}

#include <chrono>
const size_t imageSize = 1280 * 1024 * 3;
const auto benchduration = std::chrono::milliseconds(200);
static std::chrono::nanoseconds benchmark(const Allocator &allocator) {
	using namespace std::chrono;

	const auto start = steady_clock::now();
	size_t count = 0;
	for (; steady_clock::now() - start < benchduration; ++count) {
		const auto pData = allocator.malloc(imageSize);
		allocator.free(pData);
	}
	return duration_cast<nanoseconds>(benchduration) / count;
}
#include <map>
TEST(Allocation, DISABLED_benchmark) {
	using namespace std;

	vector<unique_ptr<Allocator>> allocators;
	allocators.emplace_back(new AlignedMalloc());
	allocators.emplace_back(new Malloc());
	allocators.emplace_back(new New());
	allocators.emplace_back(new BigAlignedBlock());

	map<std::chrono::nanoseconds, string> countPerAllocator;
	cout << "Number of milliseconds per allocation (" << imageSize << " bytes) over " << benchduration.count() << " ms" << endl;
	for (const auto &pAlloc : allocators) {
		countPerAllocator[benchmark(*pAlloc)] = pAlloc->name();
		cout << '*';
		cout.flush();
	}
	cout << endl;
	for (const auto &pair : countPerAllocator) {
		cout << double(pair.first.count()) / 1000 << " ms\t" << pair.second << endl;
	}
}
