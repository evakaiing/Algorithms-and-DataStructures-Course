#pragma once

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class List {
private:
    class Node {
        friend class ListIterator;
        friend class List;
        Node() : data_(std::nullopt), next_(nullptr), previous_(nullptr){};
        explicit Node(const T& value) : data_(value), next_(nullptr), previous_(nullptr){};

    private:
        std::optional<T> data_ = typename std::optional<T>::value_type();
        Node* next_ = nullptr;
        Node* previous_ = nullptr;
    };

public:
    class ListIterator {
        friend List;

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
        using iterator_category = std::bidirectional_iterator_tag;

        inline bool operator==(const ListIterator& other) const {
            return node_ == other.node_;
        };

        inline bool operator!=(const ListIterator& other) const {
            return node_ != other.node_;
        };

        inline reference_type operator*() const {
            return node_->data_.value();
        };

        ListIterator& operator++() {
            node_ = node_->next_;
            return *this;
        };

        ListIterator operator++(int) {
            ListIterator temp_iterator = *this;
            ++(*this);
            return temp_iterator;
        };

        ListIterator& operator--() {
            node_ = node_->previous_;
            return *this;
        }

        ListIterator operator--(int) {
            ListIterator temp_iterator = *this;
            --(*this);
            return temp_iterator;
        };

        /*The overload of operator -> must either return a raw pointer,
        or return an object (by reference or by value) for which
        operator -> is in turn overloaded.*/

        // to (this.operator-> () -> method; указатель на объект, к которому требуется доступ

        inline pointer_type operator->() const {
            return this;  // need *this not &this
        };

        Node* GetNode() {
            return this->node_;
        }

    private:
        explicit ListIterator(Node* node) : node_(node){};

    private:
        Node* node_;
    };

public:
    List() : head_(nullptr), tail_(nullptr) {
        tail_next_ = new Node;
        tail_next_->previous_ = tail_;
    };

    explicit List(size_t sz) {
        for (size_t i = 0; i < sz; ++i) {
            Node* new_node = new Node(typename std::optional<T>::value_type());
            if (head_ == nullptr) {
                head_ = new_node;
                tail_ = new_node;
                if (tail_next_ == nullptr) {
                    tail_next_ = new Node;
                }
                tail_->next_ = tail_next_;
                tail_next_->previous_ = tail_;
            } else {
                new_node->previous_ = tail_;
                tail_->next_ = new_node;
                tail_ = new_node;
            }
        }
        tail_->next_ = tail_next_;
        tail_next_->previous_ = tail_;
    }
    List(const std::initializer_list<T>& values) {
        for (auto& elem : values) {
            this->PushBack(elem);
        }
    }

    List(const List& other) {
        for (auto it = other.Begin(); it != other.End(); ++it) {
            this->PushBack(*it);
        }
    }

    List& operator=(const List& other) {
        if (this != &other) {
            this->Clear();
            for (auto it = other.Begin(); it != other.End(); ++it) {
                this->PushBack(*it);
            }
        }
        return *this;
    }

    ListIterator Begin() const noexcept {
        return ListIterator(head_);
    }

    ListIterator End() const noexcept {
        return ListIterator(tail_next_);
    }

    inline T& Front() const {
        if (head_ == nullptr) {
            throw ListIsEmptyException("List is empty");
        }
        return head_->data_.value();
    }

    inline T& Back() const {
        if (head_ == nullptr) {
            throw ListIsEmptyException("List is empty");
        }
        return tail_->data_.value();
    }

    inline bool IsEmpty() const noexcept {
        return (head_ == nullptr);
    }

    inline size_t Size() const noexcept {
        size_t count_elems = 0;
        Node* current_node = head_;
        if (current_node != nullptr) {
            while (current_node != nullptr) {
                ++count_elems;
                current_node = current_node->next_;
            }
            return count_elems - 1;
        }
        return count_elems;
    }
    void Swap(List& a) {
        Node* tmp_head = this->head_;
        this->head_ = a.head_;
        a.head_ = tmp_head;

        Node* tmp_tail = this->tail_;
        this->tail_ = a.tail_;
        a.tail_ = tmp_tail;
    }

    ListIterator Find(const T& value) const {
        Node* current_node = head_;
        ListIterator pos(current_node);
        while (current_node != nullptr && current_node->data_.value() != value) {
            current_node = current_node->next_;
            ++pos;
        }
        if (current_node == nullptr) {
            throw ElementNotFoundException("Element wasn't found");
        } else {
            return pos;
        }
    }

    void Erase(ListIterator pos) {
        Node* node_in_current_pos = pos.GetNode();
        if (node_in_current_pos == nullptr) {
            return;
        } else if (node_in_current_pos->next_ == nullptr) {  // tail
            tail_ = node_in_current_pos->previous_;
            tail_->next_ = nullptr;
        } else if (node_in_current_pos->previous_ == nullptr) {  // head
            head_ = node_in_current_pos->next_;
            head_->previous_ = nullptr;
        } else {
            node_in_current_pos->previous_->next_ = node_in_current_pos->next_;
            node_in_current_pos->next_->previous_ = node_in_current_pos->previous_;
        }
        delete node_in_current_pos;
    }

    void Insert(ListIterator pos, const T& value) {
        Node* new_node = new Node(value);
        Node* node_in_current_pos = pos.GetNode();

        if (this->IsEmpty()) {
            head_ = new_node;
            tail_ = new_node;
            tail_next_ = new Node;
            tail_->next_ = tail_next_;
            tail_next_->previous_ = tail_;
        } else if (node_in_current_pos->previous_ == nullptr) {
            new_node->next_ = node_in_current_pos;
            node_in_current_pos->previous_ = new_node;
            head_ = new_node;
        } else if (node_in_current_pos->next_ == nullptr) {
            new_node->previous_ = node_in_current_pos;
            node_in_current_pos->next_ = new_node;
            tail_ = new_node;
            delete tail_next_;
            tail_next_ = new Node;
            tail_->next_ = tail_next_;
            tail_next_->previous_ = tail_;
        } else {
            new_node->previous_ = node_in_current_pos->previous_;
            new_node->next_ = node_in_current_pos;
            node_in_current_pos->previous_->next_ = new_node;
            node_in_current_pos->previous_ = new_node;
        }
    }

    void Clear() noexcept {
        if (head_ == nullptr && tail_ == nullptr) {
            return;
        } else {
            while (tail_ != nullptr) {
                Node* node_to_delete = tail_;
                tail_ = tail_->previous_;
                delete node_to_delete;
            }
        }
        head_ = nullptr;
        tail_ = nullptr;
        delete tail_next_;
        tail_next_ = nullptr;
    }

    void PushBack(const T& value) {
        Node* new_node = new Node(value);
        if (head_ == nullptr) {
            head_ = new_node;
            tail_ = new_node;
            if (tail_next_ == nullptr) {
                tail_next_ = new Node;
            }
            tail_->next_ = tail_next_;
            tail_next_->previous_ = tail_;
        } else {
            tail_->next_ = new_node;
            new_node->previous_ = tail_;
            tail_ = new_node;
            delete tail_next_;
            tail_next_ = new Node;
            tail_->next_ = tail_next_;
            tail_next_->previous_ = tail_;
        }
    }

    void PushFront(const T& value) {
        Node* new_node = new Node(value);
        if (head_ == nullptr) {
            head_ = new_node;
            tail_ = new_node;
            tail_next_ = new Node;
            tail_->next_ = tail_next_;
            tail_next_->previous_ = tail_;
        } else {
            head_->previous_ = new_node;
            new_node->next_ = head_;
            head_ = new_node;
        }
    }

    void PopBack() {
        if (head_ == nullptr) {
            throw ListIsEmptyException("List is empty");
        }
        if (head_ == tail_) {
            delete tail_;
            tail_ = nullptr;
            head_ = nullptr;
            delete tail_next_;
            tail_next_ = nullptr;
        } else {
            Node* tmp = tail_;
            tail_ = tail_->previous_;
            delete tmp;
            delete tail_next_;
            tail_next_ = new Node;
            tail_->next_ = tail_next_;
            tail_next_->previous_ = tail_;
        }
    }

    void PopFront() {
        if (head_ == nullptr) {
            throw ListIsEmptyException("List is empty");
        }
        if (head_ == tail_) {
            delete head_;
            head_ = nullptr;
            tail_ = nullptr;
            delete tail_next_;
            tail_next_ = nullptr;
        } else {
            head_ = head_->next_;
            delete head_->previous_;
            head_->previous_ = nullptr;
        }
    }

    ~List() {
        Node* current_node = tail_;
        while (current_node != nullptr) {
            Node* tmp_node = current_node;
            current_node = current_node->previous_;
            delete tmp_node;
        }
        delete tail_next_;
        head_ = nullptr;
        tail_ = nullptr;
        tail_next_ = nullptr;
    }

private:
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    Node* tail_next_ = nullptr;
};

namespace std {
// Global swap overloading
template <typename T>
// NOLINTNEXTLINE
void swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std
