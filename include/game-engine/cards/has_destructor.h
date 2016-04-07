#ifndef HAS_DESTRUCTOR_H
#define HAS_DESTRUCTOR_H

#include <type_traits>

/*! The template `has_destructor<T>` exports a
boolean constant `value that is true iff `T` has
a public destructor.

N.B. A compile error will occur if T has non-public destructor.
*/
template< typename T>
struct has_destructor
{
	typedef char test_ret_if_yes;
	typedef long test_ret_if_no;

	/* Has destructor :) */
	template <typename A>
	static test_ret_if_yes test(decltype(std::declval<A>().~A()) *) {
		return test_ret_if_yes();
	}

	/* Has no destructor :( */
	template<typename A>
	static test_ret_if_no test(...) {
		return test_ret_if_no();
	}

	/* This will be either `test_ret_if_yes` or `test_ret_if_no` */
	typedef decltype(test<T>(0)) type;
};

#endif // EOF