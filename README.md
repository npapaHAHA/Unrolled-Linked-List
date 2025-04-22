# Lab Work 7: STL-Compatible Unrolled Linked List (C++)

This project implements a **STL-compatible container** based on the [Unrolled Linked List](https://en.wikipedia.org/wiki/Unrolled_linked_list) data structure. The container is generic, allocator-aware, and supports bi-directional iteration.

---

## 🎯 Objective

Implement a templated C++ container `UnrolledLinkedList<T, MaxNodeSize, Allocator>` with the following features:

- STL container compatibility (partial)
- Custom memory allocation
- Bi-directional iterators
- Strong exception safety
- Efficient operations on large collections

---

## 📦 Key Properties

| Feature                             | Status         |
|-------------------------------------|----------------|
| General Container                   | ✅ Supported   |
| Sequence Container                  | ✅ Partially   |
| Reversible Container                | ✅ Supported   |
| Allocator Aware                     | ✅ Supported   |
| Bidirectional Iterator              | ✅ Supported   |

**Partially supported** operations (from `SequenceContainer`):

- ❌ `emplace`, `emplace_front`, `emplace_back`
- ❌ `assign_range`, `prepend_range`
- ❌ `operator[]`

> These are explicitly excluded from the lab requirements.

---

## 🧠 Core Methods & Complexities

| Method       | Complexity (Amortized)        | Exception Guarantee |
|--------------|-------------------------------|---------------------|
| `insert`     | O(1) for 1 elem, O(M) for M    | Strong              |
| `erase`      | O(1) for 1 elem, O(M) for M    | `noexcept`          |
| `clear`      | O(N)                           | `noexcept`          |
| `push_back`  | O(1)                           | Strong              |
| `pop_back`   | O(1)                           | `noexcept`          |
| `push_front` | O(1)                           | Strong              |
| `pop_front`  | O(1)                           | `noexcept`          |

---

## 🧪 Testing

- Unit tests are implemented using [Google Test](https://google.github.io/googletest/)
- Provided tests cover:
  - Iterator compliance
  - Element insertion and removal
  - Edge cases
  - Exception safety

> You are encouraged to add your own test cases for full coverage.

---

## ⚠️ Constraints

- **Standard containers (`std::vector`, `std::list`, etc.) are not allowed**
- You must manually manage memory within each node using allocators
- Memory layout must conform to STL expectations (copyable, destructible, alloc-aware)

---

## 🧱 Design Requirements

- Each node holds up to `MaxNodeSize` elements
- Node splitting and merging must be efficient
- Iterator behavior must match STL conventions
- Container must work with standard STL algorithms

---

## 📚 Related Topics

This lab involves key C++ topics:

- **Custom Allocators**
- **Adapter Patterns**
- **Exception Safety**
- **Smart Memory Management**
- **STL Containers & Requirements**


---

## 👨‍💻 Author

1st-year CS student @ ITMO University  
GitHub: [npapaHAHA](https://github.com/npapaHAHA)
