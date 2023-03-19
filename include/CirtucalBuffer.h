#pragma once

#include <initializer_list>
#include <memory>
#include <stdexcept>
#include "Iterator.h"
//#include "Allocator.h"


template<typename T, typename Alloc=std::allocator<T>>
class CCirtucalBuffer {
public:

    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;

    using allocator_type = Alloc;

    explicit CCirtucalBuffer(size_t n)
        : capacity_(n + 1)
        , alloc_(Alloc())
    {
        data_ = alloc_.allocate(capacity_);
    }

    explicit CCirtucalBuffer(size_t n, const T& val)
        : capacity_(n + 1)
        , alloc_(Alloc())
    {
        data_ = alloc_.allocate(capacity_);
        for (int i = 0; i < n; i++) {
            push_back(val);
        }
    }

    template<typename SmthIterator>
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
            push_back(i);
        }
    }

    CCirtucalBuffer& operator=(const std::initializer_list<value_type>& il) {
        assign(il);
        return *this;
    }

    CCirtucalBuffer& operator=(const CCirtucalBuffer& other) {
        swap(other);
        return *this;
    }

    bool operator==(const CCirtucalBuffer& other) const {
        return size() == size() && std::equal(begin(), end(), other.begin());
    }

    bool operator!=(const CCirtucalBuffer& other) const {
        return !operator==(other);
    }

    void swap(const CCirtucalBuffer& other) {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(write_pos_, other.write_pos_);
        std::swap(reader_pos_, other.reader_pos_);
        std::swap(alloc_, other.alloc_);
    }

    size_t max_size() {
        return std::allocator_traits<Alloc>::max_size(alloc_);
    }

    allocator_type get_allocator() const {
        return alloc_;
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

    ~CCirtucalBuffer() {
        alloc_.deallocate(data_, capacity_);
    }

    void push_back(const value_type& val) {
        *(data_ + write_pos_) = val;
        write_pos_ = getIncrement(write_pos_);

        if (write_pos_ == reader_pos_) {
            reader_pos_ = getIncrement(reader_pos_);
        }
    }

    void push_front(const value_type& val) {
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

        reader_pos_ = getDecrement(reader_pos_);
    }

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

    [[nodiscard]] bool empty() const {
        return write_pos_ == reader_pos_;
    }

    void clear() {
        while (!empty()) {
            pop_front();
        }
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

    [[nodiscard]] reference operator[](size_t n) const {
        if (n >= size()) {
            throw std::out_of_range("");
        }

        size_t num = (reader_pos_ + n) % capacity_;
        return *(data_ + num);
    }

    [[nodiscard]] reference at(size_t n) const {
        return operator[](n);
    }

    void resize(size_t n) {
        if (n + 1 <= capacity_){
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

            return;
        }

        changeCapacityIfMore(n);
    }

    void assign(const std::initializer_list<value_type>& il) {
        changeCapacityWithoutSave(il.size());

        for (const_reference i: il) {
            push_back(i);
        }
    }

    template<typename SmthIterator>
    void assign(SmthIterator first_, SmthIterator last_) {
        changeCapacityWithoutSave(std::distance(first_, last_));

        first_--;
        while (++first_ != last_) {
            push_back(*first_);
        }
    }

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

    iterator insert(iterator Where, const_reference val) {
        changeCapacityIfMore(size() + 1);
        write_pos_++;

        iterator cur = end() - 1;
        while (cur - 1 != Where) {
            *cur = *(cur - 1);
            ++cur;
        }

        return overwrite(Where, val);
    }

    iterator insert(iterator Where, size_t n, const_reference val) {
        changeCapacityIfMore(size() + n);
        write_pos_ += n;

        iterator cur = end();
        while (cur - n != Where) {
            *cur = *(cur - n);
            ++cur;
        }

        return overwrite(Where, n, val);
    }

    template<class SmthIterator>
    iterator insert(iterator Where, SmthIterator first, SmthIterator last) {
        size_t n = std::distance(first, last);
        changeCapacityIfMore(size() + n);
        write_pos_ += n;

        iterator cur = end();
        while (cur - n != Where) {
            *cur = *(cur - n);
            ++cur;
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
    template<class SmthIterator>
    iterator overwrite(iterator Where, SmthIterator first, SmthIterator last) {
        while (first < last) {
            *(Where++) = first++;
        }

        return Where;
    }

    // insert without exchange
    iterator overwrite(iterator Where, const std::initializer_list<value_type>& il) {
        return overwrite(Where, il.begin(), il.end());
    }

private:

    size_t getIncrement(size_t pos) {
        --pos %= capacity_;
        return pos;
    }

    size_t getDecrement(size_t pos) {
        pos = (pos == 0) ? (capacity_ - 1) : --pos;
        return pos;
    }

    void changeCapacityWithoutSave(size_t n) {
        if (n + 1 > capacity_) {
            alloc_.deallocate(data_, capacity_);
            capacity_ = n + 1;
            data_ = alloc_.allocate(capacity_);

            write_pos_ = 0;
            reader_pos_ = 0;
        }
    }

    void changeCapacityIfMore(size_t n) {
        if (n + 1 > capacity_) {
            pointer new_data_ = alloc_.allocate(n + 1);
            std::copy(begin(), end(), new_data_);

            alloc_.deallocate(data_);
            capacity_ = n + 1;
            data_ = new_data_;

            write_pos_ = (write_pos_ < reader_pos_) ? capacity_ - (reader_pos_ - write_pos_) : write_pos_ - reader_pos_;
            reader_pos_ = 0;
        }
    }

    size_t write_pos_ = 0;
    size_t reader_pos_ = 0;

    pointer data_;
    size_t capacity_;

    std::allocator<value_type> alloc_;

};


template<typename T>
std::ostream& operator<<(std::ostream& os, const CCirtucalBuffer<T>& buffer) {
    for (size_t i = 0; i < buffer.size(); i++) {
        os << buffer[i] << " ";
    }

    return os;
}
