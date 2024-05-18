#pragma once

#include <initializer_list>
#include <utility>
#include <memory>
#include <compare>

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
        using iterator_category = std::random_access_iterator_tag; // contiguous 
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;

        VectorIterator(const VectorIterator&): ptr_(nullptr) {};
        VectorIterator& operator=(const VectorIterator& other) {
            if (this != &other) {
                ptr_ = other.ptr_;
            }
            return *this;
        }

        inline bool operator!=(const VectorIterator& other) const {
            return ptr_ != other.ptr_;
        }

        inline bool operator==(const VectorIterator& other) const {
            return ptr_ == other.ptr_;
        }
        
        inline bool operator>(const VectorIterator& other) const {
            return (ptr_ > other.ptr_);
        }

        inline bool operator<(const VectorIterator& other) const {
            return (ptr_ < other.ptr_);
        }

        inline bool operator>=(const VectorIterator& other) const {
            return (ptr_ >= other.ptr_);
        }

        inline bool operator<=(const VectorIterator& other) const {
            return (ptr_ <= other.ptr_);
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
        VectorIterator(T* ptr): ptr_(ptr) {}
    };
public:
    Vector() {};
    Vector(Alloc alloc) : alloc_(AllocTraits::select_on_container_copy_construction(alloc)) {};
    Vector(size_t count) {
        Reserve(count > 10 ? count : 10);
        arr_ = AllocTraits::allocate(alloc_, cap_);
        size_ = count;
        // try {
        //     for (size_t i = 0; i < count; ++i) {
        //         EmplaceBack(T());
        //     }
        // } catch(...) {
        //     Clear();
        //     AllocTraits::deallocate(alloc_, arr_, count > 10 ? count : 10);
        // }
    }

    Vector(size_t count, const T& value): Vector(count) {
        // Reserve(count > 10 ? count : 10);
        for (size_t i = 0; i < count; ++i) {
            AllocTraits::construct(alloc_, arr_ + i, value);
        }
    }

    Vector(const Vector& other): Vector(other.alloc_) {
        Reserve(other.size_ > 10 ? other.size_ : 10);
        try {
            for (size_t i = 0; i < other.size_; ++i) {
                EmplaceBack(other.arr_[i]);
            }
        } catch(...) {
            Clear();
            AllocTraits::deallocate(alloc_, arr_, other.size_ > 10 ? other.size_ : 10);
        }
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        Alloc newalloc = AllocTraits::propagate_on_container_copy_assignment::value 
            ? other.alloc_ : alloc_;
        size_t i = 0;
        T* newarr = AllocTraits::allocate(newalloc, other.cap_ > 10 ? other.cap_ : 10); 
        try {
            for (; i < other.size_; ++i) {
                AllocTraits::construct(newalloc, newarr + i, other.arr_[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                AllocTraits::destroy(newalloc, newarr + j);
            }
            AllocTraits::deallocate(newalloc, newarr, other.cap_ > 10 ? other.cap_ : 10);
            throw;
        }
        Clear();
        AllocTraits::deallocate(alloc_, arr_, cap_);

        alloc_ = newalloc; // no throw
        arr_ = newarr;
        size_ = other.size_;
        cap_ = other.cap_ > 10 ? other.cap_ : 10;

        return *this;
    }




    Vector(Vector&& other): alloc_(std::move(other.alloc_)) {
        std::swap(arr_, other.arr_);
        std::swap(cap_, other.cap_);
        std::swap(size_, other.size_);
    }

    // для использования std::move
    Vector& operator=(Vector&& other) { // move-семантика  принимает r-value ссылку на другой объект  и возвращает l-value
        if (this == &other) {
            return *this;
        }
        Alloc new_alloc = AllocTraits::propagate_on_container_move_assignment::value ? other.alloc_ : alloc_;
        T* new_arr = AllocTraits::allocate(new_alloc, other.cap_ > 10 ? other.cap_ : 10);
        size_t i = 0;
        try {
            for (; i< other.size_; ++i) {
                AllocTraits::construct(new_alloc, new_arr + i, std::move_if_noexcept(other.arr_[i]));
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                AllocTraits::destroy(new_alloc, new_arr + j);
            }
            AllocTraits::deallocate(new_alloc, new_arr, other.cap_ > 10 ? other.cap_ : 10);
            throw;
        }
        Clear();
        AllocTraits::deallocate(alloc_, arr_, cap_);
        cap_ = other.cap_ > 10 ? other.cap_ : 10;
        size_ = other.size_;
        other.Clear();
        other.size_ = 0;
        arr_ = new_arr;
        return *this;
    }

    Vector(std::initializer_list<T> init) {
        this->Reserve(init.size() > 10 ? init.size() : 10);
        for (auto&& elem : init) {
            this -> PushBack(elem);
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

    const T& Front() const noexcept {
        return arr_[0];
    }

    T& Front() noexcept {
        return arr_[0];
    }

    const T& Back() const noexcept {
        return arr_[size_ - 1];
    }


    T& Back() noexcept {
        return arr_[size_ - 1];
    }

    const T* Data() const noexcept {
        return arr_;
    }

    T* Data() noexcept {
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
        if (arr_) {
            AllocTraits::deallocate(alloc_, arr_, cap_);
        }
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

    void Insert(size_t pos, T value) { // insert before pos
        if (pos > size_ + 1) {
            return;
        }
        if (pos < 0) {
            return;
        }       
        if (size_ + 1 > cap_) {
            this -> Reserve(size_ * 2);
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

    template <class... Args> // переменное количество шаблонных аргументов
    void EmplaceBack(Args&&... args) {
        if (cap_ == size_) {
            Reserve(cap_ > 0 ? cap_ * 2 : 10);
        }
        try {
            AllocTraits::construct(alloc_, arr_ + size_, std::forward<Args>(args)...);
        } catch(...) {
            throw;
        }
        ++size_;
    }

    void PopBack() {
        if (size_ != 0) {
            AllocTraits::destroy(alloc_, arr_ + (--size_));
        }
    }

    void Resize(size_t count, const T& value = T()) {
        if (count > cap_) {
            this -> Reserve(count);
        }
        for (size_t i = size_; i < count; ++i) {
            AllocTraits::construct(alloc_, arr_ + i, value);
        }
        size_ = count;
    }

    ~Vector() {
        Clear();
        AllocTraits::deallocate(alloc_, arr_, cap_);
    }

private:
    using AllocTraits = std::allocator_traits<Alloc>;
    T* arr_ = nullptr;
    size_t size_ = 0;
    size_t cap_ = 0;
    Alloc alloc_ = Alloc();
};



template <>
class Vector<void*, std::allocator<void*>> {
public:
    Vector() : arr_(nullptr), sz_(0u), cap_(0u) {
        // NOLINTNEXTLINE
    }

    Vector& operator=(const Vector& other) {
        Reserve(other.sz_);
        Clear();
        for (size_t index = 0u; index < other.sz_; ++index) {
            arr_[index] = other.arr_[index];
        }
        return *this;
    }

    Vector& operator=(Vector&& other) {
        Clear();
        std::swap(arr_, other.arr_);
        std::swap(sz_, other.sz_);
        std::swap(cap_, other.cap_);
        return *this;
    }

    size_t Size() const noexcept {
        return sz_;
    }

    bool IsEmpty() const noexcept {
        return sz_ == 0u;
    }

    void PushBack(void* ptr) {
        if (sz_ == cap_) {
            // NOLINTNEXTLINE
            Reserve(cap_ > 0 ? cap_ * 2 : 10u);
        }
        arr_[sz_] = ptr;
        ++sz_;
    }

    void* Front() const {
        return arr_[0u];
    }

    void* Back() const {
        return arr_[sz_ - 1u];
    }

    void Reserve(std::size_t newsz) {
        if (newsz <= cap_) {
            return;
        }
        std::size_t index = 0u;
        void** newarr = static_cast<void**>(malloc(newsz * sizeof(void*)));
        for (; index < sz_; ++index) {
            new (newarr + index) void*(arr_[index]);
        }
        free(arr_);
        cap_ = newsz;
        arr_ = newarr;
    }

    void Clear() noexcept {
        for (size_t index = 0; index < sz_; ++index) {
            if (arr_[index]) {
                free(arr_[index]);
            }
        }
        sz_ = 0u;
    }

    ~Vector() noexcept {
        Clear();
        free(arr_);
    }

private:
    void** arr_;
    size_t sz_;
    size_t cap_;
};
