/*
 * Rational.h
 *
 *  Created on: Jan 2, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef RATIONAL_H_
#define RATIONAL_H_

#include <ostream>
#include <stdexcept>
#include <cassert>

namespace math {

template<typename T>
T gcd(T a, T b) {
	if (b == 0)
		return a;
	return gcd(b, a % b);
}

}  // namespace math

struct bad_rational: std::domain_error {
	bad_rational() :
			std::domain_error("bad rational: zero denominator") {
	}
};

template<typename T>
struct rational {
	typedef T value_type;
	rational() :
			num(0), den(1) {
	}
	rational(value_type num) :
			num(num), den(1) {
	}
	rational(value_type num, value_type den) :
			num(num), den(den) {
		normalize();
	}
	// Default copy constructor and assignment are fine

	// Add assignment from IntType
	rational& operator=(value_type n) {
		return assign(n, 1);
	}

	// Assign in place
	rational& assign(value_type n, value_type d) {
		num = n;
		den = d;
		normalize();
		return *this;
	}

	inline value_type numerator() const {
		return num;
	}
	inline value_type denominator() const {
		return den;
	}

	// Arithmetic assignment operators
	rational& operator+=(const rational& r) {
		// This calculation avoids overflow, and minimises the number of expensive
		// calculations. Thanks to Nickolay Mladenov for this algorithm.
		//
		// Proof:
		// We have to compute a/b + c/d, where gcd(a,b)=1 and gcd(b,c)=1.
		// Let g = gcd(b,d), and b = b1*g, d=d1*g. Then gcd(b1,d1)=1
		//
		// The result is (a*d1 + c*b1) / (b1*d1*g).
		// Now we have to normalize this ratio.
		// Let's assume h | gcd((a*d1 + c*b1), (b1*d1*g)), and h > 1
		// If h | b1 then gcd(h,d1)=1 and hence h|(a*d1+c*b1) => h|a.
		// But since gcd(a,b1)=1 we have h=1.
		// Similarly h|d1 leads to h=1.
		// So we have that h | gcd((a*d1 + c*b1) , (b1*d1*g)) => h|g
		// Finally we have gcd((a*d1 + c*b1), (b1*d1*g)) = gcd((a*d1 + c*b1), g)
		// Which proves that instead of normalizing the result, it is better to
		// divide num and den by gcd((a*d1 + c*b1), g)

		// Protect against self-modification
		const value_type r_num = r.num;
		const value_type r_den = r.den;

		value_type g = math::gcd(den, r_den);
		den /= g;  // = b1 from the calculations above
		num = num * (r_den / g) + r_num * den;
		g = math::gcd(num, g);
		num /= g;
		den *= r_den / g;

		positiveDenominator();
		return *this;
	}
	rational& operator-=(const rational& r) {
		// Protect against self-modification
		const value_type r_num = r.num;
		const value_type r_den = r.den;

		// This calculation avoids overflow, and minimises the number of expensive
		// calculations. It corresponds exactly to the += case above
		value_type g = math::gcd(den, r_den);
		den /= g;
		num = num * (r_den / g) - r_num * den;
		g = math::gcd(num, g);
		num /= g;
		den *= r_den / g;

		positiveDenominator();
		return *this;
	}
	rational& operator*=(const rational& r) {
		// Protect against self-modification
		const value_type r_num = r.num;
		const value_type r_den = r.den;

		// Avoid overflow and preserve normalization
		const value_type gcd1 = math::gcd(num, r_den);
		const value_type gcd2 = math::gcd(r_num, den);
		num = (num / gcd1) * (r_num / gcd2);
		den = (den / gcd2) * (r_den / gcd1);

		positiveDenominator();
		return *this;
	}
	rational& operator/=(const rational& r) {

		// Protect against self-modification
		const value_type r_num = r.num;
		const value_type r_den = r.den;

		// Avoid repeated construction
		const value_type zero(0);

		// Trap division by zero
		if (r_num == zero)
			throw bad_rational();
		if (num == zero)
			return *this;

		// Avoid overflow and preserve normalization
		const value_type gcd1 = math::gcd(num, r_num);
		const value_type gcd2 = math::gcd(r_den, den);
		num = (num / gcd1) * (r_den / gcd2);
		den = (den / gcd2) * (r_num / gcd1);

		positiveDenominator();
		return *this;
	}

	rational& operator+=(value_type i) {
		return operator +=(rational(i));
	}
	rational& operator-=(value_type i) {
		return operator -=(rational(i));
	}
	rational& operator*=(value_type i) {
		return operator *=(rational(i));
	}
	rational& operator/=(value_type i) {
		return operator /=(rational(i));
	}

	rational operator+(const rational &r) const {
		return rational(*this) += r;
	}
	rational operator-(const rational &r) const {
		return rational(*this) -= r;
	}
	rational operator*(const rational &r) const {
		return rational(*this) *= r;
	}
	rational operator/(const rational &r) const {
		return rational(*this) /= r;
	}

	// Increment and decrement
	const rational& operator++() {
		// This can never denormalise the fraction
		num += den;
		return *this;
	}
	const rational& operator--() {
		// This can never denormalise the fraction
		num -= den;
		return *this;
	}

	// Operator not
	bool operator!() const {
		return !num;
	}
	// Comparison operators
	bool operator<(const rational& r) const {
		// Avoid repeated construction
		value_type const zero(0);

		// This should really be a class-wide invariant.  The reason for these
		// checks is that for 2's complement systems, INT_MIN has no corresponding
		// positive, so negating it during normalization keeps it INT_MIN, which
		// is bad for later calculations that assume a positive denominator.
		assert(den > zero);
		assert(r.den > zero);

		// Determine relative order by expanding each value to its simple continued
		// fraction representation using the Euclidian GCD algorithm.
		struct {
			value_type n, d, q, r;
		} ts = { num, den, num / den, num % den }, rs = { r.num, r.den, r.num / r.den, r.num % r.den };
		unsigned reverse = 0u;

		// Normalize negative moduli by repeatedly adding the (positive) denominator
		// and decrementing the quotient.  Later cycles should have all positive
		// values, so this only has to be done for the first cycle.  (The rules of
		// C++ require a nonnegative quotient & remainder for a nonnegative dividend
		// & positive divisor.)
		while (ts.r < zero) {
			ts.r += ts.d;
			--ts.q;
		}
		while (rs.r < zero) {
			rs.r += rs.d;
			--rs.q;
		}

		// Loop through and compare each variable's continued-fraction components
		while (true) {
			// The quotients of the current cycle are the continued-fraction
			// components.  Comparing two c.f. is comparing their sequences,
			// stopping at the first difference.
			if (ts.q != rs.q) {
				// Since reciprocation changes the relative order of two variables,
				// and c.f. use reciprocals, the less/greater-than test reverses
				// after each index.  (Start w/ non-reversed @ whole-number place.)
				return reverse ? ts.q > rs.q : ts.q < rs.q;
			}

			// Prepare the next cycle
			reverse ^= 1u;

			if ((ts.r == zero) || (rs.r == zero)) {
				// At least one variable's c.f. expansion has ended
				break;
			}

			ts.n = ts.d;
			ts.d = ts.r;
			ts.q = ts.n / ts.d;
			ts.r = ts.n % ts.d;
			rs.n = rs.d;
			rs.d = rs.r;
			rs.q = rs.n / rs.d;
			rs.r = rs.n % rs.d;
		}

		// Compare infinity-valued components for otherwise equal sequences
		if (ts.r == rs.r) {
			// Both remainders are zero, so the next (and subsequent) c.f.
			// components for both sequences are infinity.  Therefore, the sequences
			// and their corresponding values are equal.
			return false;
		} else {
#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4800)
#endif
			// Exactly one of the remainders is zero, so all following c.f.
			// components of that variable are infinity, while the other variable
			// has a finite next c.f. component.  So that other variable has the
			// lesser value (modulo the reversal flag!).
			return (ts.r != zero) != static_cast<bool>(reverse);
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
		}
	}
	bool operator<=(const rational& r) const {
		// Trap equality first
		if (*this == r)
			return true;

		// Otherwise, we can use operator<
		return operator<(r);
	}
	bool operator>=(const rational& r) const {
		return !operator<(r);
	}
	bool operator>(const rational& r) const {
		// Trap equality first
		if (*this == r)
			return false;

		// Otherwise, we can use operator<
		return !operator<(r);
	}
	bool operator==(const rational& r) const {
		return ((num == r.num) && (den == r.den));
	}
	inline bool operator!=(const rational& r) const {
		return !operator==(r);
	}

	bool operator<(value_type i) const {
		// Avoid repeated construction
		value_type const zero(0);

		// Break value into mixed-fraction form, w/ always-nonnegative remainder
		assert(den > zero);
		value_type q = num / den, r = num % den;
		while (r < zero) {
			r += den;
			--q;
		}

		// Compare with just the quotient, since the remainder always bumps the
		// value up.  [Since q = floor(n/d), and if n/d < i then q < i, if n/d == i
		// then q == i, if n/d == i + r/d then q == i, and if n/d >= i + 1 then
		// q >= i + 1 > i; therefore n/d < i iff q < i.]
		return q < i;
	}
	bool operator>(value_type i) const {
		// Trap equality first
		if (*this == i)
			return false;

		// Otherwise, we can use operator<
		return !operator<(i);
	}
	inline bool operator==(value_type i) const {
		return ((den == value_type(1)) && (num == i));
	}
	inline bool operator!=(value_type i) const {
		return !operator==(i);
	}

	friend std::ostream& operator<<(std::ostream& stream, const rational &r) {
		if (r.den == 1)
			return stream << r.num;
		return stream << r.num << "/" << r.den;
	}
private:
	value_type num;
	value_type den;
	void normalize() {
		value_type zero(0);
		if (den == zero)
			throw bad_rational();
		if (num == zero) {
			den = value_type(1);
			return;
		}
		const value_type g = math::gcd(num, den);

		num /= g;
		den /= g;

		positiveDenominator();

		assert(test_invariant());
	}
	inline void positiveDenominator() {
		// Ensure that the denominator is positive
		if (den < value_type(0)) {
			num = -num;
			den = -den;
		}
	}
	inline bool test_invariant() const {
		return (den > value_type(0)) && (math::gcd(num, den) == value_type(1));
	}
};

#endif /* RATIONAL_H_ */
