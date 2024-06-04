#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>

const size_t DEFAULT_SIZE_BLOCK = 10;
const size_t DEFAULT_COUNT_BLOCKS = 5;

class DequeIsEmptyException : std::exception {
public:
    explicit DequeIsEmptyException(const std::string& text) : error_message_(text) {
    }

    const char* what() const noexcept override {
        return error_message_.data();
    }

private:
    std::string_view error_message_;
};

template <typename T, typename Alloc = std::allocator<T>>
class Deque {
private:
    template <typename U>
    struct Pair {
        size_t external = 0;
        size_t internal = 0;

        Pair(size_t ext, size_t in) : external(ext), internal(in) {
        }
    };

    class DequeIterator {
    public:
        friend class Deque;
        // NOLINTNEXTLINE
        using iterator_category = std::random_access_iterator_tag;
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;

        struct Index {
            size_t start;
            size_t finish;
            explicit Index(const size_t& curr) : start(curr / DEFAULT_SIZE_BLOCK), finish(curr % DEFAULT_SIZE_BLOCK){};
        };

        bool operator==(const DequeIterator& other) const {
            return curr_ == other.curr_;
        }

        bool operator!=(const DequeIterator& other) const {
            return curr_ != other.curr_;
        }

        bool operator<(const DequeIterator& other) const {
            return other.curr_ - curr_ > 0;
        }

        bool operator>(const DequeIterator& other) const {
            return other.curr_ < curr_;
        }
        DequeIterator& operator+=(difference_type count) {
            curr_ += count;
            return curr_;
        }
        DequeIterator operator+(difference_type count) {
            DequeIterator temp = *this;
            return temp += count;
        }  // надо бы вынести в глобальный

        DequeIterator& operator-=(difference_type count) {
            return (*this += -count);
        }

        DequeIterator operator-(difference_type count) {
            DequeIterator temp = *this;
            return temp -= count;
        }

        DequeIterator& operator++() {
            ++curr_;
            return *this;
        }

        DequeIterator operator++(int) {
            DequeIterator copy = *this;
            ++(*this);
            return copy;
        }

        DequeIterator& operator--() {
            --curr_;
            return *this;
        }

        DequeIterator operator--(int) {
            DequeIterator copy = *this;
            --(*this);
            return copy;
        }

        reference_type operator*() {
            Index ind(curr_);
            // std::cout << ind.start << "^^" << ind.finish << '\n';
            // std::cout << "|" << curr_ / DEFAULT_SIZE_BLOCK << '|' << curr_ % DEFAULT_SIZE_BLOCK << '\n';
            return arr_[ind.start][ind.finish];
        }

        pointer_type operator->() {
            Index ind(curr_);
            return arr_[ind.start][ind.finish];
        }

    private:
        explicit DequeIterator(T** arr, size_t curr) : arr_(arr), curr_(curr){};

    private:
        T** arr_;
        size_t curr_;
    };

private:
    T** external_arr_ = nullptr;
    size_t size_ = 0;  // количество элементов в блоке
    Pair<T> start_ = {0, 0};
    Pair<T> end_ = {0, 0};
    size_t external_size_ = 0;  // размер внешнего массива

    using AllocTraits = std::allocator_traits<Alloc>;
    using PointerAlloc = typename AllocTraits::template rebind_alloc<T*>;
    using PointerAllocTraits = std::allocator_traits<PointerAlloc>;

    Alloc alloc_;
    PointerAlloc external_alloc_;

    void ResizeExternal(const size_t count) {
        size_t offset = (count - external_size_) / 2;
        size_t new_size = external_size_ + count;
        T** new_external_arr = PointerAllocTraits::allocate(external_alloc_, new_size);

        for (size_t i = 0; i < external_size_; ++i) {
            new_external_arr[i + offset] = external_arr_[i];
        }

        for (size_t i = 0; i < offset; ++i) {
            new_external_arr[i] = AllocTraits::allocate(alloc_, DEFAULT_SIZE_BLOCK);
        }

        for (size_t i = external_size_ + offset; i < new_size; ++i) {
            new_external_arr[i] = AllocTraits::allocate(alloc_, DEFAULT_SIZE_BLOCK);
        }

        PointerAllocTraits::deallocate(external_alloc_, external_arr_, external_size_);
        external_arr_ = new_external_arr;
        external_size_ = new_size;

        start_.external += offset;
        end_.external += offset;
    }

    void AllocateExternal(const size_t count) {
        external_arr_ = PointerAllocTraits::allocate(external_alloc_, count);
        for (size_t i = 0; i < count; ++i) {
            external_arr_[i] = AllocTraits::allocate(alloc_, DEFAULT_SIZE_BLOCK);
        }
        external_size_ = count;
    }

    void AllocateBlock(T*& block) {
        block = AllocTraits::allocate(alloc_, DEFAULT_SIZE_BLOCK);
    }

    void DeallocateBlock(T* block) {
        AllocTraits::deallocate(alloc_, block, DEFAULT_SIZE_BLOCK);
    }

    void DeallocateExternal() {
        for (size_t i = 0; i < external_size_; ++i) {
            if (external_arr_[i]) {
                DeallocateBlock(external_arr_[i]);
            }
        }
        PointerAllocTraits::deallocate(external_alloc_, external_arr_, external_size_);
    }

public:
    Deque(){};

    explicit Deque(size_t count) {
        size_t count_blocks = (count / DEFAULT_SIZE_BLOCK) + 1;
        AllocateExternal(count_blocks);
        start_ = {count_blocks / 2, 0};
        end_ = start_;
    }

    explicit Deque(size_t count, const T& value) {
        size_t count_blocks = count / DEFAULT_SIZE_BLOCK + 1;
        start_ = {count_blocks / 2, 0};
        end_ = start_;
        for (size_t i = 0; i < count; ++i) {
            this->PushBack(value);
        }
    }
    Deque(const Deque& other)
        : size_(other.size_), start_(other.start_), end_(other.end_), external_size_(other.external_size_) {
        alloc_ = AllocTraits::select_on_container_copy_construction(other.alloc_);
        AllocateExternal(external_size_);
        for (size_t i = 0; i < external_size_; ++i) {
            if (other.external_arr_[i]) {
                for (size_t j = 0; j < DEFAULT_SIZE_BLOCK; ++j) {
                    AllocTraits::construct(alloc_, &external_arr_[i][j], other.external_arr_[i][j]);
                }
            }
        }
    }

    Deque(Deque&& other)
        : external_arr_(other.external_arr_),
          size_(other.size_),
          start_(other.start_),
          end_(other.end_),
          external_size_(other.external_size_) {
        alloc_ = AllocTraits::select_on_container_copy_construction(other.alloc_);
        other.external_arr_ = nullptr;
        other.size_ = 0;
        other.external_size_ = 0;
        other.start_ = {0, 0};
        other.end_ = {0, 0};
    }

    Deque& operator=(const Deque& other) {
        if (this != &other) {
            this->Clear();
            Deque copy = other;
            alloc_ = AllocTraits::propagate_on_container_copy_assignment::value ? other.alloc_ : alloc_;
            std::swap(external_arr_, copy.external_arr_);
            std::swap(size_, copy.size_);
            std::swap(external_size_, copy.external_size_);
            std::swap(start_, copy.start_);
            std::swap(end_, copy.end_);
        }
        return *this;
    }

    Deque& operator=(Deque&& other) {
        if (this != &other) {
            alloc_ = AllocTraits::propagate_on_container_move_assignment::value ? other.alloc_ : alloc_;
            std::swap(external_arr_, other.external_arr_);
            std::swap(size_, other.size_);
            std::swap(external_size_, other.external_size_);
            std::swap(start_, other.start_);
            std::swap(end_, other.end_);

            other.external_arr_ = nullptr;
            other.size_ = 0;
            other.external_size_ = 0;
            other.start_ = {0, 0};
            other.end_ = {0, 0};
        }
        return *this;
    }

    Deque(std::initializer_list<T> values) {
        for (auto elem : values) {
            this->PushBack(elem);
        }
    }

    DequeIterator Begin() {
        return DequeIterator(external_arr_, start_.external * DEFAULT_SIZE_BLOCK + start_.internal);
    }

    DequeIterator End() {
        return DequeIterator(external_arr_, end_.external * DEFAULT_SIZE_BLOCK + end_.internal);
    }

    T& Front() {
        return external_arr_[start_.external][start_.internal];
    }

    T& Back() {
        return external_arr_[end_.external][end_.internal];
    }

    T& operator[](size_t ind) {
        size_t total_offset = start_.external * DEFAULT_SIZE_BLOCK + start_.internal + ind;
        size_t ext = total_offset / DEFAULT_SIZE_BLOCK;
        size_t itl = total_offset % DEFAULT_SIZE_BLOCK;
        if (ext >= external_size_ || itl >= DEFAULT_SIZE_BLOCK) {
            throw;
        }
        return external_arr_[ext][itl];
    }

    bool IsEmpty() const {
        return size_ == 0;
    }

    size_t Size() const {
        return size_;
    }

    void PushBack(const T& value) {
        if (size_ == 0) {
            AllocateExternal(DEFAULT_COUNT_BLOCKS);
            start_ = {DEFAULT_COUNT_BLOCKS / 2, 0};
            end_ = start_;
        } else if (end_.internal == DEFAULT_SIZE_BLOCK - 1) {
            if (end_.external == external_size_ - 1) {
                ResizeExternal(DEFAULT_COUNT_BLOCKS);
            }
            ++end_.external;
            end_.internal = 0;
        } else {
            ++end_.internal;
        }
        AllocTraits::construct(alloc_, &external_arr_[end_.external][end_.internal], value);
        ++size_;
    }

    void PushFront(const T& value) {
        if (start_.internal == 0) {
            if (start_.external == 0) {
                ResizeExternal(DEFAULT_COUNT_BLOCKS);
                start_.external += DEFAULT_COUNT_BLOCKS / 2;
                end_.external += DEFAULT_COUNT_BLOCKS / 2;
            }
            start_.internal = DEFAULT_SIZE_BLOCK;
            --start_.external;
        }
        --start_.internal;
        AllocTraits::construct(alloc_, &external_arr_[start_.external][start_.internal], value);
        ++size_;
    }

    void PopBack() {
        if (size_ == 0) {
            throw DequeIsEmptyException("");
        }
        AllocTraits::destroy(alloc_, &external_arr_[end_.external][end_.internal]);
        if (end_.internal == 0) {
            --end_.external;
            end_.internal = DEFAULT_SIZE_BLOCK - 1;
        } else {
            --end_.internal;
        }
        --size_;
    }
    void PopFront() {
        if (size_ == 0) {
            throw DequeIsEmptyException("");
        }
        AllocTraits::destroy(alloc_, &external_arr_[start_.external][start_.internal]);
        if (start_.internal == DEFAULT_SIZE_BLOCK - 1) {
            start_.internal = 0;
            ++start_.external;
        } else {
            ++start_.internal;
        }
        --size_;
    }

    void Clear() {
        if (external_arr_ != nullptr) {
            for (size_t i = 0; i < external_size_; ++i) {
                if (external_arr_[i] != nullptr) {
                    for (size_t j = 0; j < DEFAULT_SIZE_BLOCK; ++j) {
                        AllocTraits::destroy(alloc_, &external_arr_[i][j]);
                    }
                    AllocTraits::deallocate(alloc_, external_arr_[i], DEFAULT_SIZE_BLOCK);
                }
            }
            PointerAllocTraits::deallocate(external_alloc_, external_arr_, external_size_);
        }
        external_arr_ = nullptr;
        size_ = 0;
        external_size_ = 0;
        start_ = {0, 0};
        end_ = {0, 0};
    }

    ~Deque() {
        this->Clear();
    }
};
