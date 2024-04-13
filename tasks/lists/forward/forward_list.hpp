#pragma once

#include <cstdlib>
#include <functional>
#include <iterator>
#include <optional>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class ForwardList {
private:
    class Node {
        friend class ForwardListIterator;
        friend class ForwardList;
        explicit Node(const T& value) : data_(value), next_(nullptr){};
        Node() : data_(std::nullopt), next_(nullptr){};

    private:
        std::optional<T> data_ = typename std::optional<T>::value_type();
        Node* next_ = nullptr;
    };

public:
    class ForwardListIterator {

        friend ForwardList;

    public:
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::forward_iterator_tag;

        inline bool operator==(ForwardListIterator other) const noexcept {
            return (this->current_node_ == other.current_node_);
        };

        inline bool operator!=(ForwardListIterator other) const noexcept {
            return (this->current_node_ != other.current_node_);
        };

        inline reference_type operator*() const {
            return (this->current_node_->data_.value());
        };

        ForwardListIterator& operator++() {
            if (current_node_ != nullptr) {
                current_node_ = current_node_->next_;
            }
            return *this;
        };

        ForwardListIterator operator++(int) {
            ForwardListIterator tmp_node = *this;
            current_node_ = current_node_->next_;
            return tmp_node;
        };

        Node* GetNode() {
            return this->current_node_;
        }

    private:
        explicit ForwardListIterator(Node* current_node_) : current_node_(current_node_){};

    private:
        Node* current_node_;
    };

public:
    ForwardList() : head_(nullptr) {
    }

    explicit ForwardList(size_t sz) : ForwardList() {
        for (size_t i = 0; i < sz; ++i) {
            this->PushFront(typename std::optional<T>::value_type());
        }
    }

    ForwardList(const std::initializer_list<T>& values) : ForwardList() {
        for (auto it = values.end(); it != values.begin(); --it) {
            if (it == values.end()) {
                continue;
            }
            this->PushFront(*it);
        }
        this->PushFront(*values.begin());
    }

    ForwardList(const ForwardList& other) {
        if (other.head_ != nullptr) {
            this->size_ = other.size_;
            head_ = new Node(other.head_->data_.value());
            Node* other_node = other.head_->next_;
            Node* current_node = head_;
            while (other_node != nullptr) {
                current_node->next_ = new Node(other_node->data_.value());
                current_node = current_node->next_;
                other_node = other_node->next_;
            }
        }
    }

    ForwardList& operator=(const ForwardList& other) {
        if (this != &other) {
            ForwardList copy = other;
            this->Swap(copy);
        }
        return *this;
    }

    ForwardListIterator Begin() const noexcept {
        return ForwardListIterator(head_);
    }

    ForwardListIterator End() const noexcept {
        return ForwardListIterator(nullptr);
    }

    inline T& Front() const {
        return (head_->data_.value());
    }

    inline bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(ForwardList& a) {
        Node* tmp_node = this->head_;
        this->head_ = a.head_;
        a.head_ = tmp_node;

        size_t tmp_size = this->size_;
        this->size_ = a.size_;
        a.size_ = tmp_size;
    }

    void EraseAfter(ForwardListIterator pos) {
        Node* node_in_pos = pos.GetNode();
        if (pos == End()) {
            return;
        }
        Node* tmp = node_in_pos->next_;
        node_in_pos->next_ = tmp->next_;
        delete tmp;
        --size_;
    }

    void InsertAfter(ForwardListIterator pos, const T& value) {
        Node* node_to_insert = new Node(value);
        Node* node_in_pos = pos.GetNode();

        Node* tmp = node_in_pos->next_;
        node_in_pos->next_ = node_to_insert;
        node_to_insert->next_ = tmp;
        ++size_;
    }

    ForwardListIterator Find(const T& value) const {
        Node* current_node = head_;
        while (current_node != nullptr) {
            if (current_node->data_.value() == value) {
                return ForwardListIterator(current_node);
            }
            current_node = current_node->next_;
        }
        return this->End();
    }

    void Clear() noexcept {
        while (!this->IsEmpty()) {
            this->PopFront();
        }
    }

    void PushFront(const T& value) {
        Node* new_node = new Node(value);
        new_node->next_ = head_;
        head_ = new_node;
        ++size_;
    }

    void PopFront() {
        if (this->IsEmpty()) {
            throw ListIsEmptyException("List is empty");
        } else {
            Node* tmp_node = head_;
            if (size_ == 1) {
                head_ = nullptr;
            } else {
                head_ = head_->next_;
            }
            delete tmp_node;
        }
        --size_;
    }

    ~ForwardList() {
        Node* current_node = head_;
        while (current_node != nullptr) {
            Node* tmp_node = current_node->next_;
            delete current_node;
            current_node = tmp_node;
        }
    }

private:
    Node* head_ = nullptr;
    size_t size_ = 0;
};

namespace std {
// Global swap overloading
template <typename T>
void swap(ForwardList<T>& a, ForwardList<T>& b) {
    a.Swap(b);
}
}  // namespace std
