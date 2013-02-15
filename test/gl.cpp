#include <gtest/gtest.h>

#include <duke/gl/GlObjects.h>

using namespace duke::gl;

struct DummyBindable: public duke::gl::IBindable {
	virtual void bind() const {
		++boundCount;
	}
	virtual void unbind() const {
		++unboundCount;
	}
	mutable size_t boundCount = 0;
	mutable size_t unboundCount = 0;
};

Binder<DummyBindable> scope_bind(const DummyBindable& bindable) {
	return Binder<DummyBindable>(&bindable);
}

TEST(GL, bindable) {
	DummyBindable bindable;
	EXPECT_EQ(0, bindable.boundCount);
	EXPECT_EQ(0, bindable.unboundCount);
	scope_bind(bindable);
	EXPECT_EQ(1, bindable.boundCount);
	EXPECT_EQ(1, bindable.unboundCount);
	{
		auto bound = scope_bind(bindable);
		EXPECT_EQ(2, bindable.boundCount);
		EXPECT_EQ(1, bindable.unboundCount);
	}
	EXPECT_EQ(2, bindable.boundCount);
	EXPECT_EQ(2, bindable.unboundCount);
}
