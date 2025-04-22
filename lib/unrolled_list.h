#include <memory>

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T> >
class unrolled_list {
private:
    struct Node;
    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<NodeAllocator>;

    struct Node {
        Node *next = nullptr;
        Node *prev = nullptr;
        size_t size = 0;
        std::aligned_storage_t<sizeof(T), alignof(T)> elements[NodeMaxSize];

        T *element_ptr(size_t i) {
            return reinterpret_cast<T *>(elements) + i;
        }

        const T *element_ptr(size_t i) const {
            return reinterpret_cast<const T *>(elements) + i;
        }
    };

    Node *head = nullptr;
    Node *tail = nullptr;
    size_t total_size = 0;
    Allocator alloc;
    NodeAllocator node_alloc;

    void copy_from(const unrolled_list &other) {
        for (const auto &element: other) {
            push_back(element);
        }
    }

public:
    using value_type = T;
    using allocator_type = Allocator;
    using reference = T &;
    using const_reference = const T &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

    template<bool IsConst>
    struct Iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = std::conditional_t<IsConst, const T *, T *>;
        using reference = std::conditional_t<IsConst, const T &, T &>;
        using difference_type = std::ptrdiff_t;

        Node *node = nullptr;
        size_t index = 0;
        const unrolled_list *parent = nullptr;

        Iterator() = default;

        Iterator(Node *n, size_t i, const unrolled_list *p) : node(n), index(i), parent(p) {
        }

        template<bool B = IsConst, typename = std::enable_if_t<B> >
        Iterator(const Iterator<false> &other) : node(other.node), index(other.index), parent(other.parent) {
        }

        reference operator*() const { return *node->element_ptr(index); }
        pointer operator->() const { return node->element_ptr(index); }

        Iterator &operator++() {
            if (!node) return *this;
            if (index + 1 < node->size) {
                ++index;
            } else {
                node = node->next;
                index = 0;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator &operator--() {
            if (!node) {
                node = parent->tail;
                if (node)
                    index = node->size - 1;
            } else if (index > 0) {
                --index;
            } else {
                node = node->prev;
                if (node)
                    index = node->size - 1;
            }
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        template<bool OtherConst>
        bool operator==(const Iterator<OtherConst> &other) const {
            return node == other.node && index == other.index;
        }

        template<bool OtherConst>
        bool operator!=(const Iterator<OtherConst> &other) const {
            return !(*this == other);
        }
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    explicit unrolled_list(const Allocator &a = Allocator())
        : alloc(a), node_alloc(a) {
    }

    unrolled_list(size_type count, const T &value, const Allocator &a = Allocator())
        : alloc(a), node_alloc(a) {
        insert(end(), count, value);
    }

    explicit unrolled_list(size_type count, const Allocator &a = Allocator())
        : alloc(a), node_alloc(a) {
        insert(end(), count, T());
    }

    template<typename InputIt>
    unrolled_list(InputIt first, InputIt last, const Allocator &a = Allocator())
        : alloc(a), node_alloc(a) {
        try {
            insert(end(), first, last);
        } catch (...) {
            clear();
            throw;
        }
    }

    unrolled_list(std::initializer_list<T> init, const Allocator &a = Allocator())
        : alloc(a), node_alloc(a) {
        insert(end(), init.begin(), init.end());
    }

    unrolled_list(const unrolled_list &other, const Allocator &a = Allocator())
        : alloc(a), node_alloc(a) {
        copy_from(other);
    }

    unrolled_list(unrolled_list &&other, const Allocator &a = Allocator()) noexcept
        : alloc(a), node_alloc(a) {
        swap(other);
    }

    ~unrolled_list() {
        clear();
    }

    unrolled_list &operator=(const unrolled_list &other) {
        if (this != &other) {
            unrolled_list tmp(other);
            swap(tmp);
        }
        return *this;
    }

    unrolled_list &operator=(unrolled_list &&other) noexcept(
        std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value ||
        std::allocator_traits<Allocator>::is_always_equal::value
    ) {
        if (this != &other) {
            if (std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
                alloc = std::move(other.alloc);
                node_alloc = std::move(other.node_alloc);
            }
            clear();
            head = other.head;
            tail = other.tail;
            total_size = other.total_size;
            other.head = nullptr;
            other.tail = nullptr;
            other.total_size = 0;
        }
        return *this;
    }

    // unrolled_list& operator=(std::initializer_list<T> il) {
    //     clear();
    //     insert(end(), il.begin(), il.end());
    //     return *this;
    // }

    void swap(unrolled_list &other) noexcept {
        using std::swap;
        swap(head, other.head);
        swap(tail, other.tail);
        swap(total_size, other.total_size);
    }

    friend void swap(unrolled_list &lhs, unrolled_list &rhs) noexcept {
        lhs.swap(rhs);
    }

    iterator begin() noexcept { return iterator(head, 0, this); }
    const_iterator begin() const noexcept { return const_iterator(head, 0, this); }
    const_iterator cbegin() const noexcept { return const_iterator(head, 0, this); }
    iterator end() noexcept { return iterator(nullptr, 0, this); }
    const_iterator end() const noexcept { return const_iterator(nullptr, 0, this); }
    const_iterator cend() const noexcept { return const_iterator(nullptr, 0, this); }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    size_type size() const noexcept { return total_size; }
    bool empty() const noexcept { return total_size == 0; }

    size_type max_size() const noexcept {
        return NodeTraits::max_size(node_alloc) * NodeMaxSize;
    }


    reference front() { return *begin(); }
    const_reference front() const { return *cbegin(); }

    reference back() {
        auto it = end();
        --it;
        return *it;
    }

    const_reference back() const {
        auto it = end();
        --it;
        return *it;
    }

    allocator_type get_allocator() const { return alloc; }

    void push_back(const T &value) {
        if (tail == nullptr || tail->size == NodeMaxSize) {
            Node *new_node = NodeTraits::allocate(node_alloc, 1);
            NodeTraits::construct(node_alloc, new_node);
            new_node->next = nullptr;
            new_node->prev = tail;
            if (tail)
                tail->next = new_node;
            tail = new_node;
            if (head == nullptr)
                head = tail;
        }
        T *place = tail->element_ptr(tail->size);
        new(place) T(value);
        ++tail->size;
        ++total_size;
    }

    template <typename InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        for (; first != last; ++first) {
            push_back(*first);
        }
    }

    void assign(size_type n, const T& t) {
        clear();
        for (size_type i = 0; i < n; ++i) {
            push_back(t);
        }
    }

    void assign(std::initializer_list<T> il) {
        assign(il.begin(), il.end());
    }


    void push_back(T &&value) {
        if (tail == nullptr || tail->size == NodeMaxSize) {
            Node *new_node = NodeTraits::allocate(node_alloc, 1);
            NodeTraits::construct(node_alloc, new_node);
            new_node->next = nullptr;
            new_node->prev = tail;
            if (tail)
                tail->next = new_node;
            tail = new_node;
            if (head == nullptr)
                head = tail;
        }
        T *place = tail->element_ptr(tail->size);
        new(place) T(std::move(value));
        ++tail->size;
        ++total_size;
    }

    void push_front(const T &value) {
        if (head == nullptr || head->size == NodeMaxSize) {
            Node *new_node = NodeTraits::allocate(node_alloc, 1);
            NodeTraits::construct(node_alloc, new_node);
            new_node->next = head;
            new_node->prev = nullptr;
            if (head)
                head->prev = new_node;
            head = new_node;
            if (tail == nullptr)
                tail = head;
        } else {
            for (size_t i = head->size; i > 0; --i) {
                T *dest = head->element_ptr(i);
                T *src = head->element_ptr(i - 1);
                new(dest) T(std::move(*src));
                src->~T();
            }
        }
        new(head->element_ptr(0)) T(value);
        ++head->size;
        ++total_size;
    }

    void push_front(T &&value) {
        if (head == nullptr || head->size == NodeMaxSize) {
            Node *new_node = NodeTraits::allocate(node_alloc, 1);
            NodeTraits::construct(node_alloc, new_node);
            new_node->next = head;
            new_node->prev = nullptr;
            if (head)
                head->prev = new_node;
            head = new_node;
            if (tail == nullptr)
                tail = head;
        } else {
            for (size_t i = head->size; i > 0; --i) {
                T *dest = head->element_ptr(i);
                T *src = head->element_ptr(i - 1);
                new(dest) T(std::move(*src));
                src->~T();
            }
        }
        new(head->element_ptr(0)) T(std::move(value));
        ++head->size;
        ++total_size;
    }

    void pop_back() noexcept(std::is_nothrow_destructible<T>::value) {
        if (tail && tail->size > 0) {
            size_t idx = tail->size - 1;
            tail->element_ptr(idx)->~T();
            --tail->size;
            --total_size;
            if (tail->size == 0) {
                Node *old = tail;
                tail = tail->prev;
                if (tail)
                    tail->next = nullptr;
                else
                    head = nullptr;
                NodeTraits::destroy(node_alloc, old);
                NodeTraits::deallocate(node_alloc, old, 1);
            }
        }
    }

    void pop_front() noexcept(std::is_nothrow_destructible<T>::value) {
        if (head && head->size > 0) {
            head->element_ptr(0)->~T();
            size_t count = head->size - 1;
            if (count > 0) {
                T *buffer = new T[count];
                try {
                    for (size_t i = 0; i < count; ++i)
                        buffer[i] = *head->element_ptr(i + 1);
                } catch (...) {
                    delete[] buffer;
                    throw;
                }
                try {
                    for (size_t i = 0; i < count; ++i) {
                        head->element_ptr(i + 1)->~T();
                        new(head->element_ptr(i)) T(buffer[i]);
                    }
                } catch (...) {
                    for (size_t i = 0; i < count; ++i) {
                        head->element_ptr(i)->~T();
                        new(head->element_ptr(i + 1)) T(buffer[i]);
                    }
                    delete[] buffer;
                    throw;
                }
                delete[] buffer;
            }
            --head->size;
            --total_size;
            if (head->size == 0) {
                Node *old = head;
                head = head->next;
                if (head)
                    head->prev = nullptr;
                else
                    tail = nullptr;
                NodeTraits::destroy(node_alloc, old);
                NodeTraits::deallocate(node_alloc, old, 1);
            }
        }
    }

    iterator erase(const_iterator cpos) noexcept {
        Node *node = cpos.node;
        if (!node) {
            return end();
        }
        size_t index = cpos.index;
        node->element_ptr(index)->~T();

        size_t count = node->size - index - 1;
        if (count > 0) {
            T buffer[NodeMaxSize];
            for (size_t i = 0; i < count; ++i) {
                buffer[i] = *node->element_ptr(index + 1 + i);
            }

            for (size_t i = 0; i < count; ++i) {
                node->element_ptr(index + 1 + i)->~T();
                new(node->element_ptr(index + i)) T(buffer[i]);
            }
        }
        --node->size;
        --total_size;

        iterator ret(node, index, this);
        if (node->size == 0) {
            Node *next_node = node->next;
            if (node->prev) node->prev->next = node->next;
            else head = node->next;
            if (node->next) node->next->prev = node->prev;
            else tail = node->prev;
            free_node(node);
            ret = iterator(next_node, 0, this);
        }
        return ret;
    }

    iterator erase(const_iterator first, const_iterator last) noexcept {
        iterator it(first.node, first.index, this);
        while (it != last) {
            it = erase(it);
        }
        return it;
    }

    iterator insert(const_iterator pos, const T &value) {
        if (pos == end()) {
            push_back(value);
            iterator it = end();
            --it;
            return it;
        }
        Node *node = pos.node;
        size_t index = pos.index;
        if (node->size <= NodeMaxSize) {
            for (size_t j = node->size; j > index; --j) {
                T *dest = node->element_ptr(j);
                T *src = node->element_ptr(j - 1);
                new(dest) T(std::move(*src));
                src->~T();
            }
            new(node->element_ptr(index)) T(value);
            ++node->size;
            ++total_size;
            return iterator(node, index, this);
        } else {
            Node *new_node = NodeTraits::allocate(node_alloc, 1);
            NodeTraits::construct(node_alloc, new_node);
            new_node->next = node->next;
            new_node->prev = node;
            if (node->next)
                node->next->prev = new_node;
            node->next = new_node;
            if (tail == node)
                tail = new_node;
            size_t mid = NodeMaxSize / 2;
            size_t num_to_move = node->size - mid;
            for (size_t j = 0; j < num_to_move; ++j) {
                T *src = node->element_ptr(mid + j);
                T *dest = new_node->element_ptr(j);
                new(dest) T(std::move(*src));
                src->~T();
            }
            new_node->size = num_to_move;
            node->size = mid;
            if (index <= mid) {
                for (size_t j = node->size; j > index; --j) {
                    T *dest = node->element_ptr(j);
                    T *src = node->element_ptr(j - 1);
                    new(dest) T(std::move(*src));
                    src->~T();
                }
                new(node->element_ptr(index)) T(value);
                ++node->size;
                ++total_size;
                return iterator(node, index, this);
            } else {
                size_t new_index = index - mid;
                for (size_t j = new_node->size; j > new_index; --j) {
                    T *dest = new_node->element_ptr(j);
                    T *src = new_node->element_ptr(j - 1);
                    new(dest) T(std::move(*src));
                    src->~T();
                }
                new(new_node->element_ptr(new_index)) T(value);
                ++new_node->size;
                ++total_size;
                return iterator(new_node, new_index, this);
            }
        }
    }

    iterator insert(const_iterator pos, size_type n, const T &value) {
        iterator it(pos.node, pos.index, this);
        for (size_type i = 0; i < n; ++i) {
            it = insert(pos, value);
            ++pos;
        }
        return it;
    }

    template<typename InputIt>
    void insert(const_iterator pos, InputIt first, InputIt last) {
        for (; first != last; ++first) {
            pos = insert(pos, *first);
            ++pos;
        }
    }

    // template<typename... Args>
    // iterator emplace(const_iterator pos, Args&&... args) {
    //     if (pos == end()) {
    //         push_back(T(std::forward<Args>(args)...));
    //         iterator it = end();
    //         --it;
    //         return it;
    //     } else {
    //         T temp(std::forward<Args>(args)...);
    //         return insert(pos, std::move(temp));
    //     }
    // }

    void clear() noexcept {
        Node *current = head;
        while (current) {
            for (size_t i = 0; i < current->size; ++i) {
                T *ptr = current->element_ptr(i);
                ptr->~T();
            }
            Node *next = current->next;
            NodeTraits::destroy(node_alloc, current);
            NodeTraits::deallocate(node_alloc, current, 1);
            current = next;
        }
        head = tail = nullptr;
        total_size = 0;
    }

    bool operator==(const unrolled_list &other) const {
        if (total_size != other.total_size)
            return false;
        return std::equal(begin(), end(), other.begin());
    }

    bool operator!=(const unrolled_list &other) const {
        return !(*this == other);
    }

private:
    void remove_node(Node *node) {
        if (node->prev)
            node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
        if (node == head)
            head = node->next;
        if (node == tail)
            tail = node->prev;
        NodeTraits::destroy(node_alloc, node);
        NodeTraits::deallocate(node_alloc, node, 1);
    }
};
