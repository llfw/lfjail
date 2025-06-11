/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_GUARD_HH
#define	LFJAIL_GUARD_HH

namespace lfjail {

/*
 * guard: invoke a callable when this object is destroyed; this is similar to
 * scope_exit from the library fundamentals TS, which LLVM doesn't implement.
 */
template<std::invocable F>
struct guard final {
	// Initialise the guard with a callable we will invoke later.
	guard(F func) : _func(std::move(func)) {}

	/*
	 * We are being destroyed, so call the callable.
	 * If the callable throws, std::terminate() will be called.
	 */
	~guard() {
		if (_func)
			std::invoke(*_func);
	}

	// Release the guard.  This turns the destructor into a no-op.
	void release() noexcept {
		_func.release();
	}

	// Not default-constructible or copyable.
	guard() = delete;
	guard(guard const &) = delete;
	guard(guard &&) noexcept = delete;
	guard &operator=(guard const &) = delete;
	guard &operator=(guard &&) noexcept = delete;

private:
	// The callable to be invoked when we are destroyed.
	std::optional<F> _func;
};

} // namespace lfjail

#endif	// !LFJAIL_GUARD_HH
