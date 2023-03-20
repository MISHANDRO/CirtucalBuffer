#pragma once

#include <initializer_list>
#include <memory>
#include <stdexcept>
#include "Iterator.h"
#include "Allocator.h"

#include <iostream>
using namespace std;


template<typename T, typename Alloc=CAllocator<T>>
class CCirtucalBuffer {
public:

    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;

    using allocator_type = Alloc;

    explicit CCirtucalBuffer()
        : CCirtucalBuffer(0)
    {}

    explicit CCirtucalBuffer(size_t n)
        : capacity_(n + 1)
        , alloc_(Alloc())
    {
        data_ = alloc_.allocate(capacity_);
    }

    explicit CCirtucalBuffer(size_t n, const_reference val)
        : capacity_(n + 1)
        , alloc_(Alloc())
    {
        data_ = alloc_.allocate(capacity_);
        for (int i = 0; i < n; i++) {
            this->push_back(val);
        }
    }

    template<typename SmthIterator,
            typename = std::enable_if_t<std::is_constructible_v<value_type,
            typename std::iterator_traits<SmthIterator>::value_type>>>
    explicit CCirtucalBuffer(SmthIterator first, SmthIterator last)
        : capacity_(std::distance(first, last) + 1)
        , alloc_(Alloc())
    {
        data_ = alloc_.allocate(capacity_);
        std::copy(first, last, begin());
        write_pos_ = capacity_ - 1;
    }

    CCirtucalBuffer(const std::initializer_list<value_type>& il)
        : capacity_(il.size() + 1)
        , alloc_(Alloc())
    {
        data_ = alloc_.allocate(capacity_);
        for (const_reference i: il) {
            this->push_back(i);
        }
    }

    CCirtucalBuffer(const CCirtucalBuffer& other)
        : capacity_(other.capacity_)
        , alloc_(Alloc())
    {
        data_ = alloc_.allocate(capacity_);
        std::copy(other.begin(), other.end(), begin());
        write_pos_ = capacity_ - 1;
    }

    virtual ~CCirtucalBuffer() {
        alloc_.deallocate(data_, capacity_);
    }

    CCirtucalBuffer& operator=(const std::initializer_list<value_type>& il) {
        assign(il);
        return *this;
    }

    CCirtucalBuffer& operator=(const CCirtucalBuffer& other) {
        alloc_.deallocate(data_, capacity_);
        capacity_ = other.capacity_;
        data_ = alloc_.allocate(capacity_);

        write_pos_ = other.write_pos_;
        reader_pos_ = other.reader_pos_;

        std::copy(other.begin(), other.end(), begin());

        return *this;
    }

    bool operator==(const CCirtucalBuffer& other) const {
        return size() == size() && std::equal(begin(), end(), other.begin());
    }

    bool operator!=(const CCirtucalBuffer& other) const {
        return !operator==(other);
    }

    void swap(CCirtucalBuffer& other) {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(write_pos_, other.write_pos_);
        std::swap(reader_pos_, other.reader_pos_);
        std::swap(alloc_, other.alloc_);
    }

    virtual void push_back(const_reference val) {
        *(data_ + write_pos_) = val;
        write_pos_ = getIncrement(write_pos_);

        if (write_pos_ == reader_pos_) {
            reader_pos_ = getIncrement(reader_pos_);
        }
    }

    // extra
    virtual void push_front(const_reference val) {
        reader_pos_ = getDecrement(reader_pos_);
        *(data_ + reader_pos_) = val;

        if (write_pos_ == reader_pos_) {
            write_pos_ = getDecrement(write_pos_);
        }
    }

    void pop_front() {
        if (empty()) {
            throw std::out_of_range("buffer is empty");
        }

        reader_pos_ = getIncrement(reader_pos_);
    }

    // extra
    void pop_back() {
        if (empty()) {
            throw std::out_of_range("buffer is empty");
        }

        write_pos_ = getDecrement(write_pos_);
    }

    [[nodiscard]] reference front() const {
        if (empty()) {
            throw std::out_of_range("buffer is empty");
        }

        return *(data_ + reader_pos_);
    }

    // extra
    [[nodiscard]] reference back() const {
        if (empty()) {
            throw std::out_of_range("buffer is empty");
        }

        return *(data_ + getDecrement(write_pos_));
    }

    [[nodiscard]] iterator begin() const {
        return iterator(data_ + reader_pos_, data_, capacity_);
    }

    [[nodiscard]] iterator end() const {
        return iterator(data_ + write_pos_, data_, capacity_);
    }

    [[nodiscard]] const_iterator cbegin() const {
        return const_iterator(data_ + reader_pos_, data_, capacity_);
    }

    [[nodiscard]] const_iterator cend() const {
        return const_iterator(data_ + write_pos_, data_, capacity_);
    }

    size_t max_size() {
        return std::allocator_traits<Alloc>::max_size(alloc_);
    }

    allocator_type get_allocator() const {
        return alloc_;
    }

    [[nodiscard]] bool empty() const {
        return write_pos_ == reader_pos_;
    }

    [[nodiscard]] bool full() const {
        return write_pos_ == getDecrement(reader_pos_);
    }

    void clear() {
        write_pos_ = 0;
        reader_pos_ = 0;
    }

    [[nodiscard]] size_t size() const {
        if (write_pos_ >= reader_pos_) {
            return write_pos_ - reader_pos_;
        } else {
            return capacity_ - (reader_pos_ - write_pos_);
        }
    }

    [[nodiscard]] size_t capacity() const {
        return capacity_ - 1;
    }

    // extra
    [[nodiscard]] reference operator[](size_t n) const {
        if (n >= size()) {
            throw std::out_of_range("Out of range!");
        }

        size_t num = (reader_pos_ + n) % capacity_;
        return *(data_ + num);
    }

    [[nodiscard]] reference at(size_t n) const {
        return operator[](n);
    }

    // extra
    template<typename... Args>
    void emplace_back(Args&&... args) {
        push_back(value_type(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void emplace_front(Args&&... args) {
        push_front(value_type(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void emplace(iterator Where, Args&&... args) {
        insert(Where, value_type(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void emplace_overwrite(iterator Where, Args&&... args) {
        overwrite(Where, value_type(std::forward<Args>(args)...));
    }

    iterator erase(iterator Where) {
        iterator end_ = end();
        if (Where >= end()) {
            return end();
        }

        iterator cur = Where;
        iterator next = Where;

        while (++next != end_) {
            *cur = *next;
            ++cur;
        }
        write_pos_ = (write_pos_ == 0) ? capacity_ - 1 : write_pos_ - 1;

        return Where;
    }

    iterator erase(iterator first, iterator last) {
        if (first < begin() || last > end()) {
            return end();
        }

        iterator end_ = end();
        write_pos_ -= std::distance(first, last);
        if (write_pos_ < 0) {
            write_pos_ += capacity_;
        }

        iterator next = last;
        while (last != end_) {
            *first = *last;
            ++first;
            ++last;
        }

        return next;
    }

    // extra
    void resize(size_t n) {
        iterator end_ = end();
        changeCapacityIfMore(n, end_);

        if (n > size()) {
            size_t dist = n - size();
            for (size_t i = 0; i < dist; i++) {
                push_back(value_type());
            }
        } else {
            size_t dist = size() - n;
            for (size_t i = 0; i < dist; i++) {
                pop_back();
            }
        }
    }

    // extra
    void assign(const std::initializer_list<value_type>& il) {
        assign(il.begin(), il.end());
    }

    template<typename SmthIterator>
    void assign(SmthIterator first_, SmthIterator last_) {
        size_t n = std::distance(first_, last_);

        clear();
        if (n + 1 > capacity_) {
            alloc_.deallocate(data_, capacity_);
            capacity_ = n + 1;
            data_ = alloc_.allocate(capacity_);
        }

        first_--;
        while (++first_ != last_) {
            push_back(*first_);
        }
    }

    // extra
    iterator insert(iterator Where, const_reference val) {
        changeCapacityIfMore(size() + 1, Where);
        write_pos_++;

        iterator cur = end() - 1;
        while (cur - 1 >= Where) {
            *cur = *(cur - 1);
            --cur;
        }

        return overwrite(Where, val);
    }

    iterator insert(iterator Where, size_t n, const_reference val) {
        changeCapacityIfMore(size() + n, Where);
        write_pos_ += n;

        iterator cur = end() - 1;
        while (cur - n >= Where) {
            *cur = *(cur - n);
            --cur;
        }

        return overwrite(Where, n, val);
    }

    template<typename SmthIterator,
             typename = std::enable_if_t<std::is_constructible_v<value_type,
             typename std::iterator_traits<SmthIterator>::value_type>>>
    iterator insert(iterator Where, SmthIterator first, SmthIterator last) {
        size_t n = std::distance(first, last);
        changeCapacityIfMore(size() + n, Where);
        write_pos_ += n;

        iterator cur = end() - 1;
        while (cur - n >= Where) {
            *cur = *(cur - n);
            --cur;
        }

        return overwrite(Where, first, last);
    }

    iterator insert(iterator Where, const std::initializer_list<value_type>& il) {
        return insert(Where, il.begin(), il.end());
    }

    // insert without size exchange
    iterator overwrite(iterator Where, const_reference val) {
        *(Where) = val;
        return Where;
    }

    // insert without size exchange
    iterator overwrite(iterator Where, size_t n, const_reference val) {
        for (int i = 0; i < n; i++) {
            *(Where + i) = val;
        }

        return Where;
    }

    // insert without exchange
    template<typename SmthIterator,
             typename = std::enable_if_t<std::is_constructible_v<value_type,
             typename std::iterator_traits<SmthIterator>::value_type>>>
    iterator overwrite(iterator Where, SmthIterator first, SmthIterator last) {
        iterator cur = Where;
        while (first < last) {
            *(cur++) = *(first++);
        }

        return Where;
    }

    // insert without exchange
    iterator overwrite(iterator Where, const std::initializer_list<value_type>& il) {
        return overwrite(Where, il.begin(), il.end());
    }

    /////// for the ultra lazy ///////
    iterator insert(size_t Where, const_reference val) {
        return insert(begin() + Where, val);
    }

    iterator insert(size_t Where, size_t n, const_reference val) {
        return insert(begin() + Where, n, val);
    }

    template<typename SmthIterator,
             typename = std::enable_if_t<std::is_constructible_v<value_type,
             typename std::iterator_traits<SmthIterator>::value_type>>>
    iterator insert(size_t Where, SmthIterator first, SmthIterator last) {
        return insert(begin() + Where, first, last);
    }

    iterator insert(size_t Where, const std::initializer_list<value_type>& il) {
        return insert(begin() + Where, il.begin(), il.end());
    }

    // insert without size exchange
    iterator overwrite(size_t Where, const_reference val) {
        return overwrite(begin() + Where, val);
    }

    // insert without size exchange
    iterator overwrite(size_t Where, size_t n, const_reference val) {
        return overwrite(begin() + Where, n, val);
    }

    // insert without exchange
    template<typename SmthIterator,
             typename = std::enable_if_t<std::is_constructible_v<value_type,
             typename std::iterator_traits<SmthIterator>::value_type>>>
    iterator overwrite(size_t Where, SmthIterator first, SmthIterator last) {
        return overwrite(begin() + Where, first, last);
    }

    // insert without exchange
    iterator overwrite(size_t Where, const std::initializer_list<value_type>& il) {
        return overwrite(begin() + Where, il);
    }
    ///////////////////////////////////////

protected:

    [[nodiscard]] size_t getIncrement(size_t pos) const {
        ++pos %= capacity_;
        return pos;
    }

    [[nodiscard]] size_t getDecrement(size_t pos) const {
        pos = (pos == 0) ? (capacity_ - 1) : --pos;
        return pos;
    }

    void changeCapacityIfMore(size_t n, iterator& it) {
        if (n + 1 > capacity_) {
            pointer new_data_ = alloc_.allocate(n + 1);
            std::copy(begin(), end(), new_data_);

            if (it != end()) {
                it = Iterator(new_data_ + (it - begin()), new_data_, n + 1);
            }

            alloc_.deallocate(data_, capacity_);
            data_ = new_data_;

            write_pos_ = (write_pos_ < reader_pos_) ? capacity_ - (reader_pos_ - write_pos_) : write_pos_ - reader_pos_;
            capacity_ = n + 1;
            reader_pos_ = 0;
        }
    }

    size_t write_pos_ = 0;
    size_t reader_pos_ = 0;

    pointer data_;
    size_t capacity_;

    Alloc alloc_;

};


template<typename T>
std::ostream& operator<<(std::ostream& os, const CCirtucalBuffer<T>& buffer) {
    for (size_t i = 0; i < buffer.size(); i++) {
        os << buffer[i] << " ";
    }

    return os;
}
