#pragma once

#include <compare>
#include <initializer_list>
#include <memory>
#include <utility>

const size_t DEFAULT_CAPACITY = 10;

template <typename T, typename Alloc = std::allocator<T>>
class Vector {
private:
    friend class VectorIterator;

    class VectorIterator {
    public:
        friend Vector;

        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using iterator_category = std::contiguous_iterator_tag;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;

        VectorIterator(const VectorIterator&) : ptr_(nullptr){};

        VectorIterator& operator=(const VectorIterator& other) {
            if (this != &other) {
                ptr_ = other.ptr_;
            }
            return *this;
        }

        reference_type operator*() const {
            return *ptr_;
        }
        pointer_type operator->() const {
            return ptr_;
        }
        VectorIterator& operator++() {
            ++ptr_;
            return *this;
        }
        VectorIterator operator++(int) {
            VectorIterator copy = *this;
            ++ptr_;
            return copy;
        }

        VectorIterator& operator--() {
            --ptr_;
            return *this;
        }

        VectorIterator operator--(int) {
            VectorIterator copy = *this;
            --ptr_;
            return copy;
        }

        VectorIterator& operator+=(const difference_type& other) noexcept {
            ptr_ += other;
            return *this;
        }

        VectorIterator& operator-=(const difference_type& other) noexcept {
            ptr_ -= other;
            return *this;
        }

        VectorIterator operator-(const difference_type& other) const noexcept {
            return (VectorIterator(ptr_ - other));
        }

        VectorIterator operator+(const difference_type& other) const noexcept {
            return (VectorIterator(ptr_ + other));
        }

    private:
        T* ptr_;
        explicit VectorIterator(T* ptr) : ptr_(ptr) {
        }
    };

public:
    Vector(){};
    explicit Vector(size_t count) {
        this->Reserve(count > DEFAULT_CAPACITY ? count : DEFAULT_CAPACITY);
        size_ = count;
    }

    Vector(size_t count, const T& value) : Vector(count) {
        for (size_t i = 0; i < count; ++i) {
            AllocTraits::construct(alloc_, arr_ + i, value);
        }
    }

    Vector(const Vector& other) : Vector(other.size_) {
        alloc_ = AllocTraits::select_on_container_copy_construction(other.alloc_);
        for (size_t i = 0; i < other.size_; ++i) {
            AllocTraits::construct(alloc_, arr_ + i, other.arr_[i]);
        }
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        Alloc new_alloc = AllocTraits::propagate_on_container_copy_assignment::value ? other.alloc_ : alloc_;
        size_t i = 0;
        T* new_arr = AllocTraits::allocate(new_alloc, other.cap_ > DEFAULT_CAPACITY ? other.cap_ : DEFAULT_CAPACITY);
        try {
            for (; i < other.size_; ++i) {
                AllocTraits::construct(new_alloc, new_arr + i, other.arr_[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                AllocTraits::destroy(new_alloc, new_arr + j);
            }
            AllocTraits::deallocate(new_alloc, new_arr, other.cap_ > DEFAULT_CAPACITY ? other.cap_ : DEFAULT_CAPACITY);
            throw;
        }
        for (size_t i = 0; i < size_; ++i) {
            AllocTraits::destroy(alloc_, arr_ + i);
        }
        AllocTraits::deallocate(alloc_, arr_, cap_);

        arr_ = new_arr;
        size_ = other.size_;
        cap_ = other.cap_;
        alloc_ = new_alloc;  // no throw

        return *this;
    }

    Vector(Vector&& other) noexcept {
        this->Clear();
        alloc_ = AllocTraits::select_on_container_copy_construction(other.alloc_);
        std::swap(arr_, other.arr_);
        std::swap(cap_, other.cap_);
        std::swap(size_, other.size_);
    }

    // для использования std::move
    Vector& operator=(
        Vector&& other) {  // move-семантика  принимает r-value ссылку на другой объект  и возвращает l-value
        if (this == &other) {
            return *this;
        }
        Alloc new_alloc = AllocTraits::propagate_on_container_move_assignment::value ? other.alloc_ : alloc_;
        T* new_arr = AllocTraits::allocate(new_alloc, other.cap_ > DEFAULT_CAPACITY ? other.cap_ : DEFAULT_CAPACITY);
        size_t i = 0;
        try {
            for (; i < other.size_; ++i) {
                AllocTraits::construct(new_alloc, new_arr + i, std::move_if_noexcept(other.arr_[i]));
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                AllocTraits::destroy(new_alloc, new_arr + j);
            }
            AllocTraits::deallocate(new_alloc, new_arr, other.cap_ > DEFAULT_CAPACITY ? other.cap_ : DEFAULT_CAPACITY);
            throw;
        }
        for (size_t i = 0; i < size_; ++i) {
            AllocTraits::destroy(alloc_, arr_ + i);
        }
        AllocTraits::deallocate(alloc_, arr_, cap_);
        arr_ = new_arr;
        cap_ = other.cap_;
        size_ = other.size_;
        other.Clear();
        return *this;
    }

    Vector(std::initializer_list<T> init) {
        this->Reserve(init.size() > DEFAULT_CAPACITY ? init.size() : DEFAULT_CAPACITY);
        for (auto&& elem : init) {
            this->PushBack(elem);
        }
    }

    VectorIterator Begin() {
        return VectorIterator(arr_);
    }

    VectorIterator End() {
        return VectorIterator(arr_ + size_);
    }

    T& operator[](size_t pos) {
        return arr_[pos];
    }

    T& Front() const noexcept {
        return arr_[0];
    }

    T& Back() const noexcept {
        return arr_[size_ - 1];
    }

    T* Data() const noexcept {
        return arr_;
    }

    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return cap_;
    }

    void Reserve(size_t new_cap) {
        if (new_cap <= cap_) {
            return;
        }
        T* new_arr = AllocTraits::allocate(alloc_, new_cap);

        size_t i = 0;

        try {
            for (; i < size_; ++i) {
                // new (newarr + i) T(arr[i]);+++
                AllocTraits::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                AllocTraits::destroy(alloc_, new_arr + j);
            }
            AllocTraits::deallocate(alloc_, new_arr, new_cap);
            throw;
        }
        AllocTraits::deallocate(alloc_, arr_, cap_);
        arr_ = new_arr;
        cap_ = new_cap;
    }

    void Clear() noexcept {
        if (size_ == 0) {
            return;
        }
        for (size_t i = 0; i < size_; ++i) {
            AllocTraits::destroy(alloc_, arr_ + i);
        }
        size_ = 0;
    }

    void Insert(size_t pos, T value) {  // insert before pos
        if (pos > size_ + 1) {
            return;
        }
        if (pos < 0) {
            return;
        }
        if (size_ + 1 > cap_) {
            this->Reserve(size_ * 2);
        }
        for (size_t i = size_; i > pos; --i) {
            arr_[i] = arr_[i - 1];
        }
        ++size_;
        arr_[pos] = value;
    }

    void Erase(size_t begin_pos, size_t end_pos) {
        size_t num_to_erase = end_pos - begin_pos;
        if (num_to_erase <= 0) {
            return;
        }
        if (begin_pos > size_ || end_pos > size_) {
            return;
        }
        for (size_t i = begin_pos; i < size_ - num_to_erase; ++i) {
            arr_[i] = arr_[i + num_to_erase];
        }
        size_ -= num_to_erase;
    }

    void PushBack(const T& value) {
        this->EmplaceBack(value);
    }

    void PushBack(T&& value) {
        this->EmplaceBack(std::move(value));
    }

    template <class... Args>  // переменное количество шаблонных аргументов
    void EmplaceBack(Args&&... args) {
        if (cap_ == size_) {
            Reserve(cap_ > 0 ? cap_ * 2 : DEFAULT_CAPACITY);
        }
        AllocTraits::construct(alloc_, arr_ + size_, std::forward<Args>(args)...);
        ++size_;
    }

    void PopBack() {
        if (size_ != 0) {
            AllocTraits::destroy(alloc_, arr_ + (--size_));
        }
    }

    void Resize(size_t count, const T& value = T()) {
        if (count > cap_) {
            this->Reserve(count);
        }
        for (size_t i = size_; i < count; ++i) {
            AllocTraits::construct(alloc_, arr_ + i, value);
        }
        size_ = count;
    }

    ~Vector() {
        for (size_t i = 0; i < size_; ++i) {
            AllocTraits::destroy(alloc_, arr_ + i);
        }
        AllocTraits::deallocate(alloc_, arr_, cap_);
    }

private:
    T* arr_ = nullptr;
    size_t size_ = 0;
    size_t cap_ = 0;
    Alloc alloc_;

    using AllocTraits = std::allocator_traits<Alloc>;
};

template <>
class Vector<void*, std::allocator<void*>> {
public:
    Vector(){};
    Vector& operator=(const Vector& other) {
        this->Reserve(other.size_ > DEFAULT_CAPACITY ? other.size_ : DEFAULT_CAPACITY);
        for (size_t i = 0; i < other.size_; ++i) {
            arr_[i] = other.arr_[i];
        }
        return *this;
    }

    Vector& operator=(Vector&& other) {
        cap_ = other.cap_;
        size_ = other.size_;
        other.size_ = 0;
        arr_ = other.arr_;
        return *this;
    }

    void* Front() const {
        return arr_[0];
    }

    void* Back() const {
        return arr_[size_ - 1];
    }

    size_t Size() const {
        return size_;
    }

    bool IsEmpty() const {
        return (size_ == 0);
    }

    void Reserve(size_t new_cap) {
        if (new_cap <= cap_) {
            return;
        }
        void** new_arr = reinterpret_cast<void**>(malloc(new_cap * sizeof(void*)));
        for (size_t i = 0; i < size_; ++i) {
            new (new_arr + i) void*(arr_[i]);
        }
        free(arr_);
        cap_ = new_cap;
        arr_ = new_arr;
    }

    void Clear() noexcept {
        if (size_ == 0) {
            return;
        }
        for (size_t i = 0; i < size_; ++i) {
            if (arr_[i]) {
                free(arr_[i]);
            }
        }
        size_ = 0;
    }

    void PushBack(void* ptr) {
        if (size_ == cap_) {
            this->Reserve(cap_ > 0 ? cap_ * 2 : DEFAULT_CAPACITY);
        }
        arr_[size_] = ptr;
        ++size_;
    }

    ~Vector() noexcept {
        Clear();
        free(arr_);
        arr_ = nullptr;
    }

private:
    void** arr_ = nullptr;
    size_t size_ = 0;
    size_t cap_ = 0;
};
