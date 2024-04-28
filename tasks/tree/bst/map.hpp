
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

class EraseNotExistElementException : std::exception {
public:
    explicit EraseNotExistElementException(const std::string& text) : error_message_(text) {
    }

    const char* what() const noexcept override {
        return error_message_.data();
    }

private:
    std::string_view error_message_;
};
template <typename Key, typename Value, typename Compare = std::less<Key>>

// We use the 'const key' in a std::pair,
// because we don't have the right to change the key,
// the tree will cease to be ordered -> UB

class Map {

private:
    class Node {
        friend class Map;

        explicit Node(std::pair<const Key, Value> node)
            : key_(node.first), value_(node.second), left_(nullptr), right_(nullptr){};

    private:
        const Key key_;
        Value value_;
        Node* left_ = nullptr;
        Node* right_ = nullptr;
    };

public:
    Map() : root_(nullptr), size_(0), comp_() {
    }

    Value& operator[](const Key& key) {
        Node* current = FindNode(key);
        if (current) {  // if key already exists, update value
            return current->value_;
        } else {  // create new node
            std::pair<const Key, Value> pair(key, Value());
            this->Insert(pair);
            return FindNode(key)->value_;
        }
    }

    // bool IsEqual(const Key& first_key, const Key& second_key) const {
    //   return first_key == second_key;
    // }

    inline bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(Map& a) {
        static_assert(std::is_same<decltype(this->comp_), decltype(a.comp_)>::value,
                      "The compare function types are different");
        Node* tmp_root = a.GetRoot();
        size_t tmp_size = a.Size();

        a.root_ = this->root_;
        a.size_ = this->size_;

        this->root_ = tmp_root;
        this->size_ = tmp_size;
    }

    Node* FindMinNode(Node* node) {  // Find min node in right subtree
        while (node->left_ != nullptr) {
            node = node->left_;
        }
        return node;
    };

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> values;
        if (this->IsEmpty()) {
            return values;
        }

        if (is_increase) {
            TraversalInOrder(root_, values);
        } else {
            TraversalInReverseOrder(root_, values);
        }

        return values;
    }

    void TraversalInOrder(Node* current, std::vector<std::pair<const Key, Value>>& values) const {
        if (current == nullptr) {
            return;
        }
        TraversalInOrder(current->left_, values);
        values.push_back({current->key_, current->value_});
        TraversalInOrder(current->right_, values);
    }

    void TraversalInReverseOrder(Node* current, std::vector<std::pair<const Key, Value>>& values) const {
        if (current == nullptr) {
            return;
        }
        TraversalInReverseOrder(current->right_, values);
        values.push_back({current->key_, current->value_});
        TraversalInReverseOrder(current->left_, values);
    }

    void Insert(const std::pair<const Key, Value>& val) {
        if (root_ == nullptr) {
            root_ = new Node(val);
            ++size_;
            return;
        }
        Node* current = root_;
        while (true) {
            if (comp_(val.first, current->key_)) {
                if (current->left_ == nullptr) {
                    current->left_ = new Node(val);
                    ++size_;
                    break;
                } else {
                    current = current->left_;
                }
            } else {
                if (current->right_ == nullptr) {
                    current->right_ = new Node(val);
                    ++size_;
                    break;
                } else {
                    current = current->right_;
                }
            }
        }
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        for (auto elem : values) {
            Insert(elem);
        }
    }

    void Erase(const Key& key) {
        Node* current = root_;
        Node* parent = nullptr;

        // static bool in_recursion = false;

        while (current != nullptr) {
            if (!comp_(key, current->key_) && !comp_(current->key_, key)) {
                break;
            }
            parent = current;
            if (comp_(key, current->key_)) {
                current = current->left_;
            } else {
                current = current->right_;
            }
        }

        if (current == nullptr) {
            throw EraseNotExistElementException("");
        }

        // if (!in_recursion) {
        //   --size_;
        //   in_recursion = true;
        // }

        if (current->left_ == nullptr && current->right_ == nullptr) {
            if (parent == nullptr) {
                root_ = nullptr;
            } else if (parent->left_ == current) {
                parent->left_ = nullptr;
            } else {
                parent->right_ = nullptr;
            }
            delete current;
            --size_;
            return;
        }

        if (current->left_ != nullptr && current->right_ == nullptr) {
            if (parent == nullptr) {
                root_ = current->left_;
            } else if (parent->left_ == current) {
                parent->left_ = current->left_;
            } else {
                parent->right_ = current->left_;
            }
            delete current;
            --size_;
            return;
        }

        if (current->left_ == nullptr && current->right_ != nullptr) {
            if (parent == nullptr) {
                root_ = current->right_;
            } else if (parent->left_ == current) {
                parent->left_ = current->right_;
            } else {
                parent->right_ = current->right_;
            }
            delete current;
            --size_;
            return;
        }

        if ((current->left_ != nullptr && current->right_ != nullptr)) {
            Node* min_node = FindMinNode(current->right_);
            Node* node_to_replace = new Node({min_node->key_, min_node->value_});

            if (current->right_ == min_node) {
                node_to_replace->right_ = min_node->right_;
            }
            node_to_replace->left_ = current->left_;

            this->Erase(min_node->key_);

            if (parent == nullptr) {
                root_ = node_to_replace;
            } else if (parent->left_ == current) {
                parent->left_ = node_to_replace;
            } else {
                parent->right_ = node_to_replace;
            }
            delete current;
        }
    }

    void Clear(Node* node) noexcept {
        if (node == nullptr) {
            return;
        }
        Clear(node->left_);
        Clear(node->right_);
        delete node;
    }

    void Clear() noexcept {
        Clear(root_);
        root_ = nullptr;
        size_ = 0;
    }

    Node* FindNode(const Key& key) const {
        Node* current = root_;
        while (current != nullptr) {
            if (current->key_ == key) {
                return current;
            } else {
                if (comp_(key, current->key_)) {
                    current = current->left_;
                } else {
                    current = current->right_;
                }
            }
        }
        return nullptr;
    }

    Node* GetRoot() {
        return this->root_;
    }

    bool Find(const Key& key) const {
        Node* current = root_;
        while (current != nullptr) {
            if (current->key_ == key) {
                return true;
            } else {
                if (comp_(key, current->key_)) {
                    current = current->left_;
                } else {
                    current = current->right_;
                }
            }
        }
        return false;
    }

    ~Map() {
        this->Clear();
        root_ = nullptr;
    }

private:
    Node* root_ = nullptr;
    size_t size_ = 0;
    Compare comp_;
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std

// commit mamamama