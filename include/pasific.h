#pragma once

#include <memory>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <boost/scope_exit.hpp>
#include <array>

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

    /**
     * Huge page allocation policy class.
     * When inherited (using the CRTP pattern), introduces a static factory
     * function, which allocates and constructs instances of its derived
     * class on a pre-allocated huge page(s). Huge pages are used via SYSV
     * shared memory syscalls.
     */
    template<class T>
    struct hugepage {

        /**
         * A non-intrusive factory function.
         * Returns a smart pointer to an instance of @a T which is allocated
         * and constructed on a pre-allocated huge page(s).
         * @return a std::unique_ptr<T, DeleterClosureType>>
         */
        static auto
        create()
        {
            bool success = false;
            // Huge pages must be pre-allocated and the user's gid configured
            // using the vm.nr_hugepages and into the vm.hugetlb_shm_group
            // kernel parameters, respectively. See the kernel documentation:
            // https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt
            const auto shmid = shmget(IPC_PRIVATE,
                                      sizeof(T),
                                      SHM_HUGETLB | IPC_CREAT |
                                      SHM_R | SHM_W);
            if(shmid < 0) {
                throw std::system_error(errno, std::system_category());
            }
            BOOST_SCOPE_EXIT_TPL(&success, shmid) {
                if(!success) (void)shmctl(shmid, IPC_RMID, nullptr);
            } BOOST_SCOPE_EXIT_END

            const auto addr = shmat(shmid, nullptr, 0);
            if(addr == (void*)-1) {
                throw std::system_error(errno, std::system_category());
            }
            BOOST_SCOPE_EXIT_TPL(&success, addr) {
                if(!success) (void)shmdt(addr);
            } BOOST_SCOPE_EXIT_END

            // Dodging the use of the class template std::function, which may
            // lead to dynamic memory allocation, which together with
            // unspecified evaluation order of function arguments, could
            // cause a resource leak here. Think: func(new X, new Y)
            const auto del = [shmid, addr](T* ptr) noexcept {
                ptr->~T();
                (void)shmdt(addr);
                (void)shmctl(shmid, IPC_RMID, nullptr);
            };
            auto ptr = std::unique_ptr<T, decltype(del)>(new(addr) T(), del);
            success = true;
            return ptr;
        }

    };

} // ns:detail

    template<class T, std::size_t N>
    struct ring : detail::hugepage<ring<T, N>> {

        using value_type = T;

        ring(const ring&) = delete;
        ring& operator=(const ring&) = delete;

        static constexpr std::size_t
        capacity()
        { return detail::pow2ceil(N); }

    private:

        friend struct detail::hugepage<ring<T, N>>;
        ring() = default;

        std::array<value_type, capacity()> e_;

    };

} // ns:pasific
