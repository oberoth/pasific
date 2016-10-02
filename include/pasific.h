#pragma once

namespace pasific {

namespace detail {

    /**
     * Returns the nearest power of two greater than or equal to its argument.
     * @param n a positive integer
     * @return the nearest power of two greater than or equal to @a n
     */
    template<typename T>
    constexpr T
    pow2ceil(T n)
    {
        if(n < 1) {
            throw std::invalid_argument("not a natural number");
	}

	auto m = n - 1;
        // Set all less significant bits than the MSB set in m, if any...
        for(auto i = 1; i < sizeof(T) * 8; i <<= 1) {
            m |= m >> i;
	}
        // ...and m+1 is the next power of two, or n.
        return m + 1;
    }

} // ns:detail

} // ns:pasific
