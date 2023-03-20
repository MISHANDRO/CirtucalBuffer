#pragma once


#include <cstddef>

template<typename T>
class Iterator {
public:
    typedef T value_type;
//    using value_type = T;
    using pointer = T *;
    using reference = T &;

    using difference_type = std::ptrdiff_t;

    explicit Iterator(pointer ptr, pointer begin, size_t capacity)
            : ptr_(ptr), begin_(begin), capacity_(capacity) {}

    [[nodiscard]] reference operator*() const {
        return *ptr_;
    }

    [[nodiscard]] pointer operator->() const {
        return ptr_;
    }

    Iterator &operator++() {
        ptr_++;
        if (ptr_ == begin_ + capacity_) {
            ptr_ = begin_;
        }

        return *this;
    }

    Iterator &operator--() {
        if (ptr_ == begin_) {
            ptr_ = begin_ + capacity_ - 1;
        } else {
            ptr_--;
        }

        return *this;
    }

    Iterator operator++(int) {
        Iterator tmp(*this);
        operator++();

        return tmp;
    }

    Iterator operator--(int) {
        Iterator tmp(*this);
        operator--();

        return tmp;
    }

    Iterator &operator+=(difference_type n) {
        if (n >= 0) {
            if (begin_ + capacity_ - ptr_ > n) {
                ptr_ += n;
            } else {
                ptr_ = begin_ + (n - (begin_ + capacity_ - ptr_));
            }
        } else {
            if (ptr_ - begin_ >= -n) {
                ptr_ += n;
            } else {
                ptr_ = begin_ + capacity_ + (n + (ptr_ - begin_));
            }
        }

        return *this;
    }

    Iterator &operator-=(difference_type n) {
        operator+=(-n);
        return *this;
    }

    Iterator operator+(const difference_type n) {
        Iterator tmp(*this);
        tmp += n;

        return tmp;
    }

    Iterator operator-(difference_type n) {
        Iterator tmp(*this);
        tmp -= n;

        return tmp;
    }

    [[nodiscard]] difference_type operator-(const Iterator &other) const {
        if (isSameSignature(other)) {
            return static_cast<difference_type>(this->ptr_ - other.ptr_);
        } else {
            return 0;
        }
    }

    [[nodiscard]] reference operator[](difference_type n) const {
        return *(operator+(n));
    }

    [[nodiscard]] bool operator==(const Iterator &other) const {
        return ptr_ == other.ptr_ && isSameSignature(other);
    }

    [[nodiscard]] bool operator!=(const Iterator &other) const {
        return ptr_ != other.ptr_ && isSameSignature(other);
    }

    [[nodiscard]] bool operator>(const Iterator &other) const {
        return ptr_ > other.ptr_ && isSameSignature(other);
    }

    [[nodiscard]] bool operator<(const Iterator &other) const {
        return ptr_ < other.ptr_ && isSameSignature(other);
    }

    [[nodiscard]]  bool operator>=(const Iterator &other) const {
        return ptr_ >= other.ptr_ && isSameSignature(other);
    }

    [[nodiscard]] bool operator<=(const Iterator &other) const {
        return ptr_ <= other.ptr_ && isSameSignature(other);
    }

private:

    [[nodiscard]] bool isSameSignature(const Iterator &other) const {
        return this->begin_ == other.begin_ && this->capacity_ == other.capacity_;
    }

    pointer begin_;
    size_t capacity_;
    pointer ptr_;
};


