#pragma once

#include <deque>
#include <exception>
#include <string>

class StackIsEmptyException : std::exception {
public:
    explicit StackIsEmptyException(const std::string& text) : error_message_(text) {
    }

    const char* what() const noexcept override {
        return error_message_.data();
    }

private:
    std::string_view error_message_;
};

template <typename T, typename Container = std::deque<T>>

class Stack {
public:
    Stack(){};

    explicit Stack(const Container& cont) : container_(cont){};

    explicit Stack(Container&& cont) : container_(std::move(cont)){};

    Stack(const Stack& other) : container_(other.container_){};

    Stack(Stack&& other) : container_(std::move(other.container_)){};

    Stack& operator=(const Stack& other) {
        this->container_ = other.container_;
        return *this;
    }

    Stack& operator=(Stack&& other) {
        this->container_ = std::move(other.container_);
        return *this;
    }

    void Push(const T& value) {
        container_.push_back(value);
    }

    void Pop() {
        if (container_.empty()) {
            throw StackIsEmptyException("");
        }
        container_.pop_back();
    }

    T& Top() {
        if (!container_.empty()) {
            return container_.back();
        }
        throw StackIsEmptyException("");
    }

    bool Empty() const {
        return container_.empty();
    }

    size_t Size() const {
        return container_.size();
    }

    ~Stack() = default;

private:
    Container container_;
};
