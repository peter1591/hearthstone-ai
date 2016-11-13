#include <iostream>
#include <experimental/generator>

class ResumableState
{
public:
	template <typename T1, typename T2>
	ResumableState(T1&& t1, T2&& t2) : functor(std::forward<T1>(t1)), functor_it(std::forward<T2>(t2)) {}

	std::experimental::generator<int> functor;
	std::experimental::generator<int>::iterator functor_it;
};

class State
{
public:
	State() = default;

	State(const State & rhs) : input(rhs.input)
	{
		if (rhs.resumable_state) throw std::exception("State cannot be copied during a resumable call");
	}
	State& operator=(const State & rhs)
	{
		if (rhs.resumable_state) throw std::exception("State cannot be copied during a resumable call");

		this->input = rhs.input;
		return *this;
	}

	State(State&& rhs) = default;
	State & operator=(State&& rhs) = default;

public:
	int input;
	std::unique_ptr<ResumableState> resumable_state;
};

auto foo(State & state)
{
	std::cout << "step 1" << ", input = " << state.input << std::endl;
	yield 1;

	std::cout << "step 2" << ", input = " << state.input << std::endl;
	yield 2;

	std::cout << "step 3" << ", input = " << state.input << std::endl;
	yield 3;
}

int bar(State & state)
{
	if (!state.resumable_state)
	{
		// new start
		std::cout << "Creating a new instance" << std::endl;

		auto foo_ret = foo(state);
		auto foo_it = foo_ret.begin();
		state.resumable_state.reset(new ResumableState(std::move(foo_ret), std::move(foo_it)));

		std::cout << "Created a new instance" << std::endl;
		return *state.resumable_state->functor_it;
	}
	else
	{
		std::cout << "Continuing on previous instance" << std::endl;
	}
	
	++state.resumable_state->functor_it;
	if (state.resumable_state->functor_it == state.resumable_state->functor.end())
	{
		std::cout << "Previous instance ended!" << std::endl;
		state.resumable_state.reset(nullptr);
		return -1;
	}
	else
	{
		std::cout << "Continued on previous instance" << std::endl;
		return *state.resumable_state->functor_it;
	}
}

int main(void)
{
	State state;

	state.input = 1;
	std::cout << "ret1 = " << bar(state) << std::endl;
	state.input = 2;
	std::cout << "ret2 = " << bar(state) << std::endl;
	state.input = 3;
	std::cout << "ret3 = " << bar(state) << std::endl;
	state.input = 4;
	std::cout << "ret4 = " << bar(state) << std::endl;

	State state2 = state;

	state2.input = 5;
	std::cout << "ret1 = " << bar(state2) << std::endl;
	state2.input = 6;
	std::cout << "ret2 = " << bar(state2) << std::endl;
	state2.input = 7;
	std::cout << "ret3 = " << bar(state2) << std::endl;
	state2.input = 8;
	std::cout << "ret4 = " << bar(state2) << std::endl;

	return 0;
}