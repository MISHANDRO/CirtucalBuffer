#pragma once

template<typename T>
class CAllocator {
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;

    value_type* allocate(difference_type n) {
        return static_cast<value_type*>(operator new(n * sizeof(value_type)));
    }

    void deallocate(value_type* data, difference_type n) {
        operator delete(data, n * sizeof(value_type));
    }
};
