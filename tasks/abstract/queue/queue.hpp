#pragma once

#include <deque>
#include <exception>
#include <string>

class QueueIsEmptyException : std::exception {
public:
    explicit QueueIsEmptyException(const std::string& text) : error_message_(text) {
    }

    const char* what() const noexcept override {
        return error_message_.data();
    }

private:
    std::string_view error_message_;
};

template <typename T, typename Container = std::deque<T>>

class Queue {
public:
    Queue(){};

    explicit Queue(const Container& cont) : container_(cont){};

    explicit Queue(Container&& cont) : container_(std::move(cont)){};

    Queue(const Queue& other) : container_(other.container_){};

    Queue(Queue&& other) : container_(std::move(other.container_)){};

    Queue& operator=(const Queue& other) {
        this->container_ = other.container_;
        return *this;
    }

    Queue& operator=(Queue&& other) {
        if (this != &other) {
            this->container_ = std::move(other.container_);
        }
        return *this;
    }

    void Push(const T& value) {
        container_.push_back(value);
    }

    void Pop() {
        if (container_.empty()) {
            throw QueueIsEmptyException("");
        }
        container_.pop_front();
    }

    T& Front() {
        if (!container_.empty()) {
            return container_.front();
        }
        throw QueueIsEmptyException("");
    }

    bool Empty() const {
        return container_.empty();
    }

    size_t Size() const {
        return container_.size();
    }

    ~Queue() = default;

private:
    Container container_;
};

// a
