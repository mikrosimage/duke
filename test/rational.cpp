#include <gtest/gtest.h>

#include <duke/math/Rational.hpp>

typedef boost::rational<int64_t> Rational;

TEST(Rational,default) {
	EXPECT_EQ(0, Rational().numerator());
	EXPECT_EQ(1, Rational().denominator());
	EXPECT_EQ(5, Rational(5).numerator());
	EXPECT_EQ(1, Rational(5).denominator());
}

TEST(Rational,normalize) {
	Rational r(6, 4);
	EXPECT_EQ(3, r.numerator());
	EXPECT_EQ(2, r.denominator());
}

TEST(Rational,assign) {
	Rational r;
	r = 2;
	EXPECT_EQ(2, r.numerator());
	EXPECT_EQ(1, r.denominator());
	r = Rational(1, 3);
	EXPECT_EQ(1, r.numerator());
	EXPECT_EQ(3, r.denominator());
}

TEST(Rational,comparisons) {
	EXPECT_EQ(Rational(), Rational());
	EXPECT_EQ(Rational(2), Rational(6,3));
	EXPECT_EQ(Rational(2), 2);
	EXPECT_NE(Rational(1), Rational(0));
	EXPECT_NE(Rational(1), 0);
	EXPECT_LT(Rational(1), 2);
	EXPECT_LT(Rational(1), Rational(2));
	EXPECT_LE(Rational(1), 2);
	EXPECT_LE(Rational(1), Rational(2));
	EXPECT_LE(Rational(1), 1);
	EXPECT_LE(Rational(1), Rational(1));
	EXPECT_GT(Rational(1), -1);
	EXPECT_GT(Rational(1), Rational(-1));
	EXPECT_GE(Rational(1), -1);
	EXPECT_GE(Rational(1), Rational(-1));
	EXPECT_GE(Rational(1), 1);
	EXPECT_GE(Rational(1), Rational(1));
}

TEST(Rational,arthmetic) {
	Rational r;
	r += 1;
	EXPECT_EQ(Rational(1), r);
	r -= 1;
	EXPECT_EQ(Rational(0), r);
	r = 2;
	r *= 2;
	EXPECT_EQ(Rational(4), r);
	r /= 2;
	EXPECT_EQ(Rational(2), r);
	r *= Rational(1, 3);
	EXPECT_EQ(Rational(2, 3), r);
	r /= Rational(3);
	EXPECT_EQ(Rational(2, 9), r);
}
