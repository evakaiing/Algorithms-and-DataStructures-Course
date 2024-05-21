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

    explicit Stack(const Container& cont) : container(cont){};

    explicit Stack(Container&& cont) : container(std::move(cont)){};

    Stack(const Stack& other) : container(other.container){};

    Stack(Stack&& other) : container(std::move(other.container)){};

    Stack& operator=(const Stack& other) {
        this->container = other.container;
        return *this;
    }

    Stack& operator=(Stack&& other) {
        this->container = std::move(other.container);
        return *this;
    }

    void Push(const T& value) {
        container.push_back(value);
    }

    void Pop() {
        if (container.empty()) {
            throw StackIsEmptyException("");
        }
        container.pop_back();
    }

    T& Top() {
        if (!container.empty()) {
            return container.back();
        }
        throw StackIsEmptyException("");
    }

    bool Empty() const {
        return container.empty();
    }

    size_t Size() const {
        return container.size();
    }

    ~Stack() = default;

private:
    Container container;
};