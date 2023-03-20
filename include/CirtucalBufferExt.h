#pragma once

#include "CirtucalBuffer.h"

template<typename T, typename Alloc=std::allocator<T>>
class CCirtucalBufferExt : public CCirtucalBuffer<T> {
public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;

    using allocator_type = Alloc;

    explicit CCirtucalBufferExt(size_t n)
        : CCirtucalBuffer<T>(n)
    {}

    explicit CCirtucalBufferExt(size_t n, const T& val)
        : CCirtucalBuffer<T>(n, val)
    {}

    template<typename SmthIterator>
    explicit CCirtucalBufferExt(SmthIterator first, SmthIterator last)
        : CCirtucalBuffer<T>(first, last)
    {}

    CCirtucalBufferExt(const std::initializer_list<value_type>& il)
        : CCirtucalBuffer<T>(il)
    {}

    void push_back(const_reference val) override {
        twiceCapacity();
        CCirtucalBuffer<T>::push_back(val);
    }

    void push_front(const value_type& val) {
        twiceCapacity();
        CCirtucalBuffer<T>::push_front(val);
    }

private:

    void twiceCapacity() {
        if (this->full()) {
            iterator end_ = this->end();
            this->changeCapacityIfMore(this->capacity_ * 2 - 2, end_);
        }
    }

};
