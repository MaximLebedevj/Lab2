#pragma once
#include <iterator>
#include <list>
#include <memory>
#include <set>
#include <vector>

#define RIGHT 1
#define LEFT 0

namespace fefu_laboratory_two {
template <typename T>
class Allocator {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  Allocator() noexcept = default;

  Allocator(const Allocator& other) noexcept {
    this->allocated_blocks_ = other.allocated_blocks_;
    this->has_link_ = true;
  }

  Allocator& operator=(const Allocator other) {
    this->allocated_blocks_ = other.allocated_blocks_;
    this->has_link_ = true;
    return *this;
  }

  ~Allocator() {
    if (has_link_) {
      return;
    }
    size_t allocated_blocks_size = allocated_blocks_.size();
    for (int i = 0; static_cast<size_t>(i) < allocated_blocks_size; ++i) {
      deallocate(allocated_blocks_.begin().operator*());
    }
  }

  pointer allocate(size_type number_of_elements) {
    auto allocated_pointer = new value_type[number_of_elements];
    allocated_blocks_.insert(allocated_pointer);
    return allocated_pointer;
  }

  void deallocate(pointer p) noexcept {
    if (allocated_blocks_.find(p) != allocated_blocks_.end()) {
      allocated_blocks_.erase(p);
      delete[] p;
    }
  }

  size_type allocatedElements() { return allocated_blocks_.size(); }

 private:
  std::set<pointer> allocated_blocks_;
  bool has_link_ = false;
};

template <typename T>
class Chunk {
 public:
  Chunk* prev;
  Chunk* next;
  size_t count;
  size_t size = 4;
  T* elements;
  bool right;

  Chunk(Allocator<T>* allocator, bool right_, int in_count = 0) {
    allocateArray(allocator);
    prev = nullptr;
    next = nullptr;
    count = in_count;
    right = right_;
  }

  void allocateArray(Allocator<T>*);

  ~Chunk() = default;
};

template <typename T>
void Chunk<T>::allocateArray(Allocator<T>* allocator) {
  elements = allocator->allocate(size);
}

template <typename ValueType>
class Deque_iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = ValueType;
  using difference_type = std::ptrdiff_t;
  using pointer = ValueType*;
  using reference = ValueType&;

  Deque_iterator() = default;

  Deque_iterator(Chunk<value_type>* position, int size = 0) noexcept {
    if (position == nullptr) {
      chunk = nullptr;
      end = true;
      iter_end = true;
      ptr_ = nullptr;
      this->position = size;
      return;
    } else {
      if (position->prev == nullptr) {
        tail = position;
      }
    }
    size_ = size;
    chunk = position;
    index_l = chunk->count - 1, index_r = 0;
    if (chunk->right) {
      this->ptr_ = &chunk->elements[index_r++];
      if (index_r == chunk->count) {
        if (chunk->next == nullptr) {
          head = chunk;
        }
        index_r = 0;
        chunk = chunk->next;
      }
    } else {
      this->ptr_ = &chunk->elements[index_l--];
      if (index_l < 0) {
        if (chunk->next == nullptr) {
          head = chunk;
        }
        chunk = chunk->next;
        index_l = chunk->count - 1;
      }
    }
  }

  Deque_iterator(const Deque_iterator& other) noexcept {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
  }

  Deque_iterator& operator=(const Deque_iterator& other) {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
    return *this;
  }

  ~Deque_iterator() = default;

  friend void swap(Deque_iterator<ValueType>& first,
                   Deque_iterator<ValueType>& second) {
    Deque_iterator<ValueType> temp(first);
    first = second;
    second = first;
  }

  friend bool operator==(const Deque_iterator<ValueType>& first,
                         const Deque_iterator<ValueType>& second) {
    if ((first.iter_end && second.iter_end) || first.ptr_ == second.ptr_) {
      return true;
    }
    return false;
  }

  friend bool operator!=(const Deque_iterator<ValueType>& first,
                         const Deque_iterator<ValueType>& second) {
    return !(first == second);
  }

  reference operator*() const {
    reference ref = *ptr_;
    return ref;
  }

  pointer operator->() const;

  Deque_iterator& operator++() {
    if (!end) {
      ++position;
      if (chunk->right == 1) {
        if (begin_right) {
          --index_r;
          r_plus_bounds();
        }
        this->ptr_ = &chunk->elements[index_r++];
        r_plus_bounds();
      } else {
        if (begin_left) {
          --index_l;
          if (index_l < 0) {
            if (chunk->next == nullptr) {
              head = chunk;
              end_left = true;
              end_right = false;
              index_l = 0;
              end = true;
            } else {
              chunk = chunk->next;
              index_l = chunk->count - 1;
            }
          }
        }
        this->ptr_ = &chunk->elements[index_l--];
        l_plus_bounds();
      }
    } else {
      iter_end = true;
    }
    begin_right = false;
    begin_left = false;
    begin = false;
    return *this;
  }

  Deque_iterator operator++(int) {
    Deque_iterator<ValueType> temp(*this);
    ++*this;
    return temp;
  }

  Deque_iterator& operator--() {
    iter_end = false;
    if (!begin) {
      --position;
      if (chunk->right) {
        if (end_right) {
          --index_r;
          r_minus_bounds();
        }
        this->ptr_ = &chunk->elements[index_r--];
        r_minus_bounds();
      } else {
        if (end_left) {
          ++index_l;
          l_minus_bounds();
        }
        this->ptr_ = &chunk->elements[index_l++];
        l_minus_bounds();
      }
    }
    end_right = false;
    end_left = false;
    end = false;
    return *this;
  }

  Deque_iterator operator--(int) {
    Deque_iterator<ValueType> temp(*this);
    --*this;
    return temp;
  }

  Deque_iterator operator+(const difference_type& value) const {
    Deque_iterator<ValueType> temp(*this);
    for (int i = 0; i < value; ++i) {
      ++temp;
    }
    return temp;
  }

  Deque_iterator& operator+=(const difference_type& value) {
    for (int i = 0; i < value; ++i) {
      ++*this;
    }
    return *this;
  }

  Deque_iterator operator-(const difference_type& value) const {
    Deque_iterator<ValueType> temp(*this);
    int counter = 0;
    while (!temp.iter_end) {
      ++temp;
      counter++;
    }
    for (int i = 0; i < counter + value - 1; ++i) {
      --temp;
    }
    return temp;
  }

  Deque_iterator& operator-=(const difference_type& value) {
    int counter = 0;
    while (!this->iter_end) {
      ++*this;
      counter++;
    }
    for (int i = 0; i < counter + value - 1; ++i) {
      --*this;
    }
    return *this;
  }

  difference_type operator-(const Deque_iterator& other) const {
    return this->position - other.position;
  }

  reference operator[](const difference_type& value) {
    Deque_iterator<ValueType> temp(*this);
    if (value >= this->size_ || value < 0) {
      return *temp.ptr_;
    }
    if (value > temp.position) {
      while (temp.position != value) {
        ++temp;
      }
    } else {
      while (temp.position != value) {
        --temp;
      }
    }
    return *temp.ptr_;
  }

  friend bool operator<(const Deque_iterator<ValueType>& first,
                        const Deque_iterator<ValueType>& second) {
    if (first - second < 0) {
      return true;
    }
    return false;
  }
  friend bool operator<=(const Deque_iterator<ValueType>& first,
                         const Deque_iterator<ValueType>& second) {
    if (first - second <= 0) {
      return true;
    }
    return false;
  }
  friend bool operator>(const Deque_iterator<ValueType>& first,
                        const Deque_iterator<ValueType>& second) {
    return !(first <= second);
  }
  friend bool operator>=(const Deque_iterator<ValueType>& first,
                         const Deque_iterator<ValueType>& second) {
    if (first - second >= 0) {
      return true;
    }
    return false;
  }

  pointer ptr_;

 private:
  size_t position = 0;
  Chunk<value_type>* tail;
  Chunk<value_type>* head;
  int index_r = 0;
  int index_l = 0;
  Chunk<int>* chunk;
  bool end = false;
  bool begin = false;
  bool end_right = false;
  bool end_left = false;
  bool begin_right = false;
  bool begin_left = false;
  bool iter_end = false;
  size_t size_;

  void r_minus_bounds() {
    if (index_r < 0) {
      if (chunk->prev == nullptr) {
        tail = chunk;
        index_r = 0;
        begin = true;
        begin_right = true;
        begin_left = false;
      } else {
        chunk = chunk->prev;
        index_r = chunk->count - 1;
      }
    }
  }

  void l_minus_bounds() {
    if (index_l == chunk->count) {
      if (chunk->prev == nullptr) {
        tail = chunk;
        index_l = chunk->count - 1;
        index_r = 0;
        begin = true;
        begin_left = true;
        begin_right = false;
      } else {
        chunk = chunk->prev;
        index_l = 0;
      }
    }
  }

  void r_plus_bounds() {
    if (index_r == chunk->count) {
      if (chunk->next == nullptr) {
        head = chunk;
        --index_r;
        end_right = true;
        end_left = false;
        index_l = 0;
        end = true;
      } else {
        index_r = 0;
        chunk = chunk->next;
      }
    }
  }

  void l_plus_bounds() {
    if (index_l < 0) {
      if (chunk->next == nullptr) {
        head = chunk;
        end_left = true;
        end_right = false;
        index_l = 0;
        end = true;
      } else {
        chunk = chunk->next;
        index_l = chunk->count - 1;
      }
    }
  }
};

template <typename ValueType>
class Deque_const_iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = ValueType;
  using difference_type = std::ptrdiff_t;
  using pointer = const ValueType*;
  using reference = const ValueType&;

  Deque_const_iterator() = default;

  Deque_const_iterator(Chunk<value_type>* position, int size = 0) noexcept {
    if (position == nullptr) {
      chunk = nullptr;
      end = true;
      iter_end = true;
      ptr_ = nullptr;
      this->position = size;
      return;
    } else {
      if (position->prev == nullptr) {
        tail = position;
      }
    }
    size_ = size;
    chunk = position;
    index_l = chunk->count - 1, index_r = 0;
    if (chunk->right) {
      this->ptr_ = &chunk->elements[index_r++];
      if (index_r == chunk->count) {
        if (chunk->next == nullptr) {
          head = chunk;
        }
        index_r = 0;
        chunk = chunk->next;
      }
    } else {
      this->ptr_ = &chunk->elements[index_l--];
      if (index_l < 0) {
        if (chunk->next == nullptr) {
          head = chunk;
        }
        chunk = chunk->next;
        index_l = chunk->count - 1;
      }
    }
  }

  Deque_const_iterator(const Deque_const_iterator& other) noexcept {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
  }

  Deque_const_iterator& operator=(const Deque_const_iterator& other) {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
    return *this;
  }

  ~Deque_const_iterator() = default;

  friend void swap(Deque_const_iterator<ValueType>& first,
                   Deque_const_iterator<ValueType>& second) {
    Deque_const_iterator<ValueType> temp(first);
    first = second;
    second = first;
  }

  friend bool operator==(const Deque_const_iterator<ValueType>& first,
                         const Deque_const_iterator<ValueType>& second) {
    if ((first.iter_end && second.iter_end) || first.ptr_ == second.ptr_) {
      return true;
    }
    return false;
  }

  friend bool operator!=(const Deque_const_iterator<ValueType>& first,
                         const Deque_const_iterator<ValueType>& second) {
    return !(first == second);
  }

  reference operator*() const {
    reference ref = *ptr_;
    return ref;
  }

  pointer operator->() const;

  Deque_const_iterator& operator++() {
    if (!end) {
      ++position;
      if (chunk->right == 1) {
        if (begin_right) {
          --index_r;
          r_plus_bounds();
        }
        this->ptr_ = &chunk->elements[index_r++];
        r_plus_bounds();
      } else {
        if (begin_left) {
          --index_l;
          if (index_l < 0) {
            if (chunk->next == nullptr) {
              head = chunk;
              end_left = true;
              end_right = false;
              index_l = 0;
              end = true;
            } else {
              chunk = chunk->next;
              index_l = chunk->count - 1;
            }
          }
        }
        this->ptr_ = &chunk->elements[index_l--];
        l_plus_bounds();
      }
    } else {
      iter_end = true;
    }
    begin_right = false;
    begin_left = false;
    begin = false;
    return *this;
  }

  Deque_const_iterator operator++(int) {
    Deque_const_iterator<ValueType> temp(*this);
    ++*this;
    return temp;
  }

  Deque_const_iterator& operator--() {
    iter_end = false;
    if (!begin) {
      --position;
      if (chunk->right) {
        if (end_right) {
          --index_r;
          r_minus_bounds();
        }
        this->ptr_ = &chunk->elements[index_r--];
        r_minus_bounds();
      } else {
        if (end_left) {
          ++index_l;
          l_minus_bounds();
        }
        this->ptr_ = &chunk->elements[index_l++];
        l_minus_bounds();
      }
    }
    end_right = false;
    end_left = false;
    end = false;
    return *this;
  }

  Deque_const_iterator operator--(int) {
    Deque_const_iterator<ValueType> temp(*this);
    --*this;
    return temp;
  }

  Deque_const_iterator operator+(const difference_type& value) const {
    Deque_const_iterator<ValueType> temp(*this);
    for (int i = 0; i < value; ++i) {
      ++temp;
    }
    return temp;
  }

  Deque_const_iterator& operator+=(const difference_type& value) {
    for (int i = 0; i < value; ++i) {
      ++*this;
    }
    return *this;
  }

  Deque_const_iterator operator-(const difference_type& value) const {
    Deque_const_iterator<ValueType> temp(*this);
    int counter = 0;
    while (!temp.iter_end) {
      ++temp;
      counter++;
    }
    for (int i = 0; i < counter + value - 1; ++i) {
      --temp;
    }
    return temp;
  }

  Deque_const_iterator& operator-=(const difference_type& value) {
    int counter = 0;
    while (!this->iter_end) {
      ++*this;
      counter++;
    }
    for (int i = 0; i < counter + value - 1; ++i) {
      --*this;
    }
    return *this;
  }

  difference_type operator-(const Deque_const_iterator& other) const {
    return this->position - other.position;
  }

  reference operator[](const difference_type& value) {
    Deque_const_iterator<ValueType> temp(*this);
    if (value >= this->size_ || value < 0) {
      return *temp.ptr_;
    }
    if (value > temp.position) {
      while (temp.position != value) {
        ++temp;
      }
    } else {
      while (temp.position != value) {
        --temp;
      }
    }
    return *temp.ptr_;
  }

  friend bool operator<(const Deque_const_iterator<ValueType>& first,
                        const Deque_const_iterator<ValueType>& second) {
    if (first - second < 0) {
      return true;
    }
    return false;
  }
  friend bool operator<=(const Deque_const_iterator<ValueType>& first,
                         const Deque_const_iterator<ValueType>& second) {
    if (first - second <= 0) {
      return true;
    }
    return false;
  }
  friend bool operator>(const Deque_const_iterator<ValueType>& first,
                        const Deque_const_iterator<ValueType>& second) {
    return !(first <= second);
  }
  friend bool operator>=(const Deque_const_iterator<ValueType>& first,
                         const Deque_const_iterator<ValueType>& second) {
    if (first - second >= 0) {
      return true;
    }
    return false;
  }

  pointer ptr_;

 private:
  size_t position = 0;
  Chunk<value_type>* tail;
  Chunk<value_type>* head;
  int index_r = 0;
  int index_l = 0;
  Chunk<int>* chunk;
  bool end = false;
  bool begin = false;
  bool end_right = false;
  bool end_left = false;
  bool begin_right = false;
  bool begin_left = false;
  bool iter_end = false;
  size_t size_;

  void r_minus_bounds() {
    if (index_r < 0) {
      if (chunk->prev == nullptr) {
        tail = chunk;
        index_r = 0;
        begin = true;
        begin_right = true;
        begin_left = false;
      } else {
        chunk = chunk->prev;
        index_r = chunk->count - 1;
      }
    }
  }

  void l_minus_bounds() {
    if (index_l == chunk->count) {
      if (chunk->prev == nullptr) {
        tail = chunk;
        index_l = chunk->count - 1;
        index_r = 0;
        begin = true;
        begin_left = true;
        begin_right = false;
      } else {
        chunk = chunk->prev;
        index_l = 0;
      }
    }
  }

  void r_plus_bounds() {
    if (index_r == chunk->count) {
      if (chunk->next == nullptr) {
        head = chunk;
        --index_r;
        end_right = true;
        end_left = false;
        index_l = 0;
        end = true;
      } else {
        index_r = 0;
        chunk = chunk->next;
      }
    }
  }

  void l_plus_bounds() {
    if (index_l < 0) {
      if (chunk->next == nullptr) {
        head = chunk;
        end_left = true;
        end_right = false;
        index_l = 0;
        end = true;
      } else {
        chunk = chunk->next;
        index_l = chunk->count - 1;
      }
    }
  }
};

template <class Iter>
class Deque_reverse_iterator {
 public:
  using iterator_type = Iter;
  using iterator_category =
      typename std::iterator_traits<Iter>::iterator_category;
  using value_type = typename std::iterator_traits<Iter>::value_type;
  using difference_type = typename std::iterator_traits<Iter>::difference_type;
  using pointer = typename std::iterator_traits<Iter>::pointer;
  using reference = typename std::iterator_traits<Iter>::reference;

  constexpr Deque_reverse_iterator(Chunk<value_type>* position, int size = 0) {
    if (position == nullptr) {
      chunk = nullptr;
      end = true;
      iter_end = true;
      ptr_ = nullptr;
      this->position = size;
      return;
    } else {
      if (position->prev == nullptr) {
        tail = position;
      }
    }
    size_ = size;
    chunk = position;
    index_l = 0, index_r = chunk->count - 1;
    if (chunk->right) {
      this->ptr_ = &chunk->elements[index_r--];
      if (index_r == chunk->count) {
        if (chunk->next == nullptr) {
          head = chunk;
        }
        index_r = 0;
        chunk = chunk->next;
      }
    } else {
      this->ptr_ = &chunk->elements[index_l++];
      if (index_l < 0) {
        if (chunk->next == nullptr) {
          head = chunk;
        }
        chunk = chunk->next;
        index_l = chunk->count - 1;
      }
    }
  }

  constexpr explicit Deque_reverse_iterator(iterator_type other) {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
  }

  template <class U>
  constexpr Deque_reverse_iterator(const Deque_reverse_iterator<U>& other) {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
  }

  template <class U>
  Deque_reverse_iterator& operator=(const Deque_reverse_iterator<U>& other) {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
    return *this;
  }

  iterator_type base() const;

  reference operator*() const {
    reference ref = *ptr_;
    return ref;
  }

  pointer operator->() const;

  reference operator[](difference_type value) {
    Deque_reverse_iterator<value_type> temp(*this);
    if (value >= this->size_ || value < 0) {
      return *temp.ptr_;
    }
    if (value > temp.position) {
      while (temp.position != value) {
        --temp;
      }
    } else {
      while (temp.position != value) {
        ++temp;
      }
    }
    return *temp.ptr_;
  }

  Deque_reverse_iterator& operator++() {
    iter_end = false;
    if (!begin) {
      --position;
      if (chunk->right) {
        if (end_right) {
          --index_r;
          r_minus_bounds();
        }
        this->ptr_ = &chunk->elements[index_r--];
        r_minus_bounds();
      } else {
        if (end_left) {
          ++index_l;
          l_minus_bounds();
        }
        this->ptr_ = &chunk->elements[index_l++];
        l_minus_bounds();
      }
    }
    end_right = false;
    end_left = false;
    end = false;
    return *this;
  }
  Deque_reverse_iterator operator++(int) {
    Deque_reverse_iterator<value_type> temp(*this);
    --*this;
    return temp;
  }

  Deque_reverse_iterator& operator--() {
    if (!end) {
      ++position;
      if (chunk->right == 1) {
        if (begin_right) {
          --index_r;
          r_plus_bounds();
        }
        this->ptr_ = &chunk->elements[index_r++];
        r_plus_bounds();
      } else {
        if (begin_left) {
          --index_l;
          if (index_l < 0) {
            if (chunk->next == nullptr) {
              head = chunk;
              end_left = true;
              end_right = false;
              index_l = 0;
              end = true;
            } else {
              chunk = chunk->next;
              index_l = chunk->count - 1;
            }
          }
        }
        this->ptr_ = &chunk->elements[index_l--];
        l_plus_bounds();
      }
    } else {
      iter_end = true;
    }
    begin_right = false;
    begin_left = false;
    begin = false;
    return *this;
  }
  Deque_reverse_iterator operator--(int) {
    Deque_reverse_iterator<value_type> temp(*this);
    ++*this;
    return temp;
  }

  Deque_reverse_iterator operator+(difference_type value) const {
    Deque_reverse_iterator<value_type> temp(*this);
    int counter = 0;
    while (!temp.iter_end) {
      ++temp;
      counter++;
    }
    for (int i = 0; i < counter + value - 1; ++i) {
      --temp;
    }
    return temp;
  }

  Deque_reverse_iterator& operator+=(difference_type value) {
    int counter = 0;
    while (!this->iter_end) {
      ++*this;
      counter++;
    }
    for (int i = 0; i < counter + value - 1; ++i) {
      --*this;
    }
    return *this;
  }

  Deque_reverse_iterator operator-(difference_type value) const {
    Deque_reverse_iterator<value_type> temp(*this);
    for (int i = 0; i < value; ++i) {
      ++temp;
    }
    return temp;
  }
  Deque_reverse_iterator& operator-=(difference_type value) {
    for (int i = 0; i < value; ++i) {
      ++*this;
    }
    return *this;
  }

  template <class Iterator1, class Iterator2>
  friend bool operator==(const Deque_reverse_iterator<Iterator1>& lhs,
                         const Deque_reverse_iterator<Iterator2>& rhs) {
    if ((lhs.iter_end && rhs.iter_end) || lhs.ptr_ == rhs.ptr_) {
      return true;
    }
    return false;
  }

  template <class Iterator1, class Iterator2>
  friend bool operator!=(const Deque_reverse_iterator<Iterator1>& lhs,
                         const Deque_reverse_iterator<Iterator2>& rhs) {
    return !(lhs == rhs);
  }

  template <class Iterator1, class Iterator2>
  friend bool operator>(const Deque_reverse_iterator<Iterator1>& lhs,
                        const Deque_reverse_iterator<Iterator2>& rhs) {
    return !(lhs <= rhs);
  }

  template <class Iterator1, class Iterator2>
  friend bool operator<(const Deque_reverse_iterator<Iterator1>& lhs,
                        const Deque_reverse_iterator<Iterator2>& rhs) {
    if (lhs - rhs < 0) {
      return true;
    }
    return false;
  }

  template <class Iterator1, class Iterator2>
  friend bool operator<=(const Deque_reverse_iterator<Iterator1>& lhs,
                         const Deque_reverse_iterator<Iterator2>& rhs) {
    if (lhs - rhs <= 0) {
      return true;
    }
    return false;
  }

  template <class Iterator1, class Iterator2>
  friend bool operator>=(const Deque_reverse_iterator<Iterator1>& lhs,
                         const Deque_reverse_iterator<Iterator2>& rhs) {
    if (lhs - rhs >= 0) {
      return true;
    }
    return false;
  }

 private:
  size_t position = 0;
  pointer ptr_;
  Chunk<value_type>* tail;
  Chunk<value_type>* head;
  int index_r = 0;
  int index_l = 0;
  Chunk<int>* chunk;
  bool end = false;
  bool begin = false;
  bool end_right = false;
  bool end_left = false;
  bool begin_right = false;
  bool begin_left = false;
  bool iter_end = false;
  size_t size_;

  void r_minus_bounds() {
    if (index_r < 0) {
      if (chunk->prev == nullptr) {
        tail = chunk;
        index_r = 0;
        begin = true;
        begin_right = true;
        begin_left = false;
      } else {
        chunk = chunk->prev;
        index_r = chunk->count - 1;
      }
    }
  }

  void l_minus_bounds() {
    if (index_l == chunk->count) {
      if (chunk->prev == nullptr) {
        tail = chunk;
        index_l = chunk->count - 1;
        index_r = 0;
        begin = true;
        begin_left = true;
        begin_right = false;
      } else {
        chunk = chunk->prev;
        index_l = 0;
      }
    }
  }

  void r_plus_bounds() {
    if (index_r == chunk->count) {
      if (chunk->next == nullptr) {
        head = chunk;
        --index_r;
        end_right = true;
        end_left = false;
        index_l = 0;
        end = true;
      } else {
        index_r = 0;
        chunk = chunk->next;
      }
    }
  }

  void l_plus_bounds() {
    if (index_l < 0) {
      if (chunk->next == nullptr) {
        head = chunk;
        end_left = true;
        end_right = false;
        index_l = 0;
        end = true;
      } else {
        chunk = chunk->next;
        index_l = chunk->count - 1;
      }
    }
  }
};

template <class Iter>
class Deque_const_reverse_iterator {
 public:
  using iterator_type = Iter;
  using iterator_category =
      typename std::iterator_traits<Iter>::iterator_category;
  using value_type = typename std::iterator_traits<Iter>::value_type;
  using difference_type = typename std::iterator_traits<Iter>::difference_type;
  using pointer = const typename std::iterator_traits<Iter>::pointer;
  using reference = const typename std::iterator_traits<Iter>::reference;

  constexpr Deque_const_reverse_iterator(Chunk<value_type>* position,
                                         int size = 0) {
    if (position == nullptr) {
      chunk = nullptr;
      end = true;
      iter_end = true;
      ptr_ = nullptr;
      this->position = size;
      return;
    } else {
      if (position->prev == nullptr) {
        tail = position;
      }
    }
    size_ = size;
    chunk = position;
    index_l = 0, index_r = chunk->count - 1;
    if (chunk->right) {
      this->ptr_ = &chunk->elements[index_r--];
      if (index_r == chunk->count) {
        if (chunk->next == nullptr) {
          head = chunk;
        }
        index_r = 0;
        chunk = chunk->next;
      }
    } else {
      this->ptr_ = &chunk->elements[index_l++];
      if (index_l < 0) {
        if (chunk->next == nullptr) {
          head = chunk;
        }
        chunk = chunk->next;
        index_l = chunk->count - 1;
      }
    }
  }

  constexpr explicit Deque_const_reverse_iterator(iterator_type other) {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
  }

  template <class U>
  constexpr Deque_const_reverse_iterator(
      const Deque_const_reverse_iterator<U>& other) {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
  }

  template <class U>
  Deque_const_reverse_iterator& operator=(
      const Deque_const_reverse_iterator<U>& other) {
    this->ptr_ = other.ptr_;
    this->tail = other.tail;
    this->chunk = other.chunk;
    this->index_l = other.index_l;
    this->index_r = other.index_r;
    this->head = other.head;
    this->end = other.end;
    this->begin = other.begin;
    this->begin_right = other.begin_right;
    this->begin_left = other.begin_left;
    this->end_left = other.end_left;
    this->end_right = other.end_right;
    this->iter_end = other.iter_end;
    return *this;
  }

  iterator_type base() const;

  reference operator*() const {
    reference ref = *ptr_;
    return ref;
  }

  pointer operator->() const;

  reference operator[](difference_type value) {
    Deque_const_reverse_iterator<value_type> temp(*this);
    if (value >= this->size_ || value < 0) {
      return *temp.ptr_;
    }
    if (value > temp.position) {
      while (temp.position != value) {
        --temp;
      }
    } else {
      while (temp.position != value) {
        ++temp;
      }
    }
    return *temp.ptr_;
  }

  Deque_const_reverse_iterator& operator++() {
    iter_end = false;
    if (!begin) {
      --position;
      if (chunk->right) {
        if (end_right) {
          --index_r;
          r_minus_bounds();
        }
        this->ptr_ = &chunk->elements[index_r--];
        r_minus_bounds();
      } else {
        if (end_left) {
          ++index_l;
          l_minus_bounds();
        }
        this->ptr_ = &chunk->elements[index_l++];
        l_minus_bounds();
      }
    }
    end_right = false;
    end_left = false;
    end = false;
    return *this;
  }
  Deque_const_reverse_iterator operator++(int) {
    Deque_const_reverse_iterator<value_type> temp(*this);
    --*this;
    return temp;
  }

  Deque_const_reverse_iterator& operator--() {
    if (!end) {
      ++position;
      if (chunk->right == 1) {
        if (begin_right) {
          --index_r;
          r_plus_bounds();
        }
        this->ptr_ = &chunk->elements[index_r++];
        r_plus_bounds();
      } else {
        if (begin_left) {
          --index_l;
          if (index_l < 0) {
            if (chunk->next == nullptr) {
              head = chunk;
              end_left = true;
              end_right = false;
              index_l = 0;
              end = true;
            } else {
              chunk = chunk->next;
              index_l = chunk->count - 1;
            }
          }
        }
        this->ptr_ = &chunk->elements[index_l--];
        l_plus_bounds();
      }
    } else {
      iter_end = true;
    }
    begin_right = false;
    begin_left = false;
    begin = false;
    return *this;
  }
  Deque_const_reverse_iterator operator--(int) {
    Deque_const_reverse_iterator<value_type> temp(*this);
    ++*this;
    return temp;
  }

  Deque_const_reverse_iterator operator+(difference_type value) const {
    Deque_const_reverse_iterator<value_type> temp(*this);
    int counter = 0;
    while (!temp.iter_end) {
      ++temp;
      counter++;
    }
    for (int i = 0; i < counter + value - 1; ++i) {
      --temp;
    }
    return temp;
  }

  Deque_const_reverse_iterator& operator+=(difference_type value) {
    int counter = 0;
    while (!this->iter_end) {
      ++*this;
      counter++;
    }
    for (int i = 0; i < counter + value - 1; ++i) {
      --*this;
    }
    return *this;
  }

  Deque_const_reverse_iterator operator-(difference_type value) const {
    Deque_const_reverse_iterator<value_type> temp(*this);
    for (int i = 0; i < value; ++i) {
      ++temp;
    }
    return temp;
  }
  Deque_const_reverse_iterator& operator-=(difference_type value) {
    for (int i = 0; i < value; ++i) {
      ++*this;
    }
    return *this;
  }

  template <class Iterator1, class Iterator2>
  friend bool operator==(const Deque_const_reverse_iterator<Iterator1>& lhs,
                         const Deque_const_reverse_iterator<Iterator2>& rhs) {
    if ((lhs.iter_end && rhs.iter_end) || lhs.ptr_ == rhs.ptr_) {
      return true;
    }
    return false;
  }

  template <class Iterator1, class Iterator2>
  friend bool operator!=(const Deque_const_reverse_iterator<Iterator1>& lhs,
                         const Deque_const_reverse_iterator<Iterator2>& rhs) {
    return !(lhs == rhs);
  }

  template <class Iterator1, class Iterator2>
  friend bool operator>(const Deque_const_reverse_iterator<Iterator1>& lhs,
                        const Deque_const_reverse_iterator<Iterator2>& rhs) {
    return !(lhs <= rhs);
  }

  template <class Iterator1, class Iterator2>
  friend bool operator<(const Deque_const_reverse_iterator<Iterator1>& lhs,
                        const Deque_const_reverse_iterator<Iterator2>& rhs) {
    if (lhs - rhs < 0) {
      return true;
    }
    return false;
  }

  template <class Iterator1, class Iterator2>
  friend bool operator<=(const Deque_const_reverse_iterator<Iterator1>& lhs,
                         const Deque_const_reverse_iterator<Iterator2>& rhs) {
    if (lhs - rhs <= 0) {
      return true;
    }
    return false;
  }

  template <class Iterator1, class Iterator2>
  friend bool operator>=(const Deque_const_reverse_iterator<Iterator1>& lhs,
                         const Deque_const_reverse_iterator<Iterator2>& rhs) {
    if (lhs - rhs >= 0) {
      return true;
    }
    return false;
  }

 private:
  size_t position = 0;
  pointer ptr_;
  Chunk<value_type>* tail;
  Chunk<value_type>* head;
  int index_r = 0;
  int index_l = 0;
  Chunk<int>* chunk;
  bool end = false;
  bool begin = false;
  bool end_right = false;
  bool end_left = false;
  bool begin_right = false;
  bool begin_left = false;
  bool iter_end = false;
  size_t size_;

  void r_minus_bounds() {
    if (index_r < 0) {
      if (chunk->prev == nullptr) {
        tail = chunk;
        index_r = 0;
        begin = true;
        begin_right = true;
        begin_left = false;
      } else {
        chunk = chunk->prev;
        index_r = chunk->count - 1;
      }
    }
  }

  void l_minus_bounds() {
    if (index_l == chunk->count) {
      if (chunk->prev == nullptr) {
        tail = chunk;
        index_l = chunk->count - 1;
        index_r = 0;
        begin = true;
        begin_left = true;
        begin_right = false;
      } else {
        chunk = chunk->prev;
        index_l = 0;
      }
    }
  }

  void r_plus_bounds() {
    if (index_r == chunk->count) {
      if (chunk->next == nullptr) {
        head = chunk;
        --index_r;
        end_right = true;
        end_left = false;
        index_l = 0;
        end = true;
      } else {
        index_r = 0;
        chunk = chunk->next;
      }
    }
  }

  void l_plus_bounds() {
    if (index_l < 0) {
      if (chunk->next == nullptr) {
        head = chunk;
        end_left = true;
        end_right = false;
        index_l = 0;
        end = true;
      } else {
        chunk = chunk->next;
        index_l = chunk->count - 1;
      }
    }
  }
};

template <class Iter>
Deque_reverse_iterator<Iter> make_reverse_iterator(Iter i);

template <typename T, typename Allocator = Allocator<T>>
class Deque {
 public:
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;
  using iterator = Deque_iterator<value_type>;
  using const_iterator = Deque_const_iterator<value_type>;
  using reverse_iterator = Deque_reverse_iterator<iterator>;
  using const_reverse_iterator = Deque_reverse_iterator<const_iterator>;

  /// @brief Default constructor. Constructs an empty container with a
  /// default-constructed allocator.
  Deque() : allocator() { createRightChunk(); }

  /// @brief Constructs an empty container with the given allocator
  /// @param alloc allocator to use for all memory allocations of this container
  explicit Deque(const Allocator& alloc) {
    this->allocator = alloc;
    createRightChunk();
  }

  /// @brief Constructs the container with count copies of elements with value
  /// and with the given allocator
  /// @param count the size of the container
  /// @param value the value to initialize elements of the container with
  /// @param alloc allocator to use for all memory allocations of this container
  Deque(size_type count, const T& value, const Allocator& alloc = Allocator()) {
    this->allocator = alloc;
    createRightChunk();
    for (int i = 0; i < count; ++i) {
      push_back(value);
    }
  }

  /// @brief Constructs the container with count default-inserted instances of
  /// T. No copies are made.
  /// @param count the size of the container
  /// @param alloc allocator to use for all memory allocations of this container
  explicit Deque(size_type count, const Allocator& alloc = Allocator()) {
    this->allocator = alloc;
    createRightChunk();
    for (int i = 0; i < count; ++i) {
      push_back(0);
    }
  }

  /// @brief Copy constructor. Constructs the container with the copy of the
  /// contents of other.
  /// @param other another container to be used as source to initialize the
  /// elements of the container with
  Deque(const Deque& other) {
    this->left_chunks_ = other.left_chunks_;
    this->right_chunks_ = other.right_chunks_;
    this->tail = other.tail;
    this->head = other.head;
    this->allocator = other.allocator;
    this->number_of_elements_ = other.number_of_elements_;
  }

  /// @brief Constructs the container with the copy of the contents of other,
  /// using alloc as the allocator.
  /// @param other another container to be used as source to initialize the
  /// elements of the container with
  /// @param alloc allocator to use for all memory allocations of this container
  Deque(const Deque& other, const Allocator& alloc) {
    this->left_chunks_ = other.left_chunks_;
    this->right_chunks_ = other.right_chunks_;
    this->tail = other.tail;
    this->head = other.head;
    this->allocator = alloc;
    this->number_of_elements_ = other.number_of_elements_;
  }

  /**
   * @brief Move constructor.
   *
   * Constructs the container with the contents of other using move semantics.
   * Allocator is obtained by move-construction from the allocator belonging to
   * other.
   *
   * @param other another container to be used as source to initialize the
   * elements of the container with
   */
  Deque(Deque&& other) {
    if (this != &other) {
      delete this->left_chunks_;
      delete this->right_chunks_;
      this->tail = other.tail;
      this->head = other.head;
      this->allocator = other.allocator;
      this->number_of_elements_ = other.number_of_elements_;
      delete other.left_chunks_;
      delete other.right_chunks_;
      other.tail = nullptr;
      other.head = nullptr;
      other.number_of_elements_ = 0;
    }
    return *this;
  }

  /**
   * @brief Allocator-extended move constructor.
   * Using alloc as the allocator for the new container, moving the contents
   * from other; if alloc != other.get_allocator(), this results in an
   * element-wise move.
   *
   * @param other another container to be used as source to initialize the
   * elements of the container with
   * @param alloc allocator to use for all memory allocations of this container
   */
  Deque(Deque&& other, const Allocator& alloc) {
    delete this->left_chunks_;
    delete this->right_chunks_;
    this->tail = other.tail;
    this->head = other.head;
    this->allocator = alloc;
    this->number_of_elements_ = other.number_of_elements_;
    delete other.left_chunks_;
    delete other.right_chunks_;
    other.tail = nullptr;
    other.head = nullptr;
    other.number_of_elements_ = 0;
  }

  /// @brief Destructs the deque.
  ~Deque() {
    for (int i = 0; static_cast<size_t>(i) < right_chunks_.size(); ++i)
      delete right_chunks_[i];
    for (int i = 0; static_cast<size_t>(i) < left_chunks_.size(); ++i)
      delete left_chunks_[i];
  }

  /// @brief Copy assignment operator. Replaces the contents with a copy of the
  /// contents of other.
  /// @param other another container to use as data source
  /// @return *this
  Deque& operator=(const Deque& other) {
    this->left_chunks_ = other.left_chunks_;
    this->right_chunks_ = other.right_chunks_;
    this->tail = other.tail;
    this->head = other.head;
    this->number_of_elements_ = other.number_of_elements_;
    this->allocator = other.allocator;
    return *this;
  }

  /**
   * Move assignment operator.
   *
   * Replaces the contents with those of other using move semantics
   * (i.e. the data in other is moved from other into this container).
   * other is in a valid but unspecified state afterwards.
   *
   * @param other another container to use as data source
   * @return *this
   */
  Deque& operator=(Deque&& other) {
    if (this != &other) {
      delete this->left_chunks_;
      delete this->right_chunks_;
      this->tail = other.tail;
      this->head = other.head;
      this->allocator = other.allocator;
      this->number_of_elements_ = other.number_of_elements_;
      delete other.left_chunks_;
      delete other.right_chunks_;
      other.tail = nullptr;
      other.head = nullptr;
      other.number_of_elements_ = 0;
    }
    return *this;
  }

  /// @brief Replaces the contents with count copies of value
  /// @param count
  /// @param value
  void assign(size_type count, const T& value) {
    this->clear();
    for (int i = 0; i < count; ++i) {
      this->push_back(value);
    }
  }

  /// @brief Returns the allocator associated with the container.
  /// @return The associated allocator.
  allocator_type get_allocator() const noexcept { return allocator; }

  /// ELEMENT ACCESS

  /// @brief Returns a reference to the element at specified location pos, with
  /// bounds checking. If pos is not within the range of the container, an
  /// exception of type std::out_of_range is thrown.
  /// @param pos position of the element to return
  /// @return Reference to the requested element.
  /// @throw std::out_of_range
  reference at(size_type pos) {
    int index = 0;
    if (!empty()) {
      if (!left_chunks_.empty()) {
        for (int i = left_chunks_.size() - 1; i >= 0; i--) {
          for (int j = left_chunks_[i]->count - 1; j >= 0; j--) {
            if (index == pos) {
              reference ref = left_chunks_[i]->elements[j];
              return ref;
            }
            ++index;
          }
        }
      }

      if (!right_chunks_.empty()) {
        for (int i = 0; i < right_chunks_.size(); ++i) {
          for (int j = 0; j < right_chunks_[i]->count; ++j) {
            if (index == pos) {
              reference ref = right_chunks_[i]->elements[j];
              return ref;
            }
            ++index;
          }
        }
      }
    }
    std::string message = std::to_string(pos);
    throw std::out_of_range("index " + message + " is out of deque bounds");
  }

  /// @brief Returns a const reference to the element at specified location pos,
  /// with bounds checking. If pos is not within the range of the container, an
  /// exception of type std::out_of_range is thrown.
  /// @param pos position of the element to return
  /// @return Const Reference to the requested element.
  /// @throw std::out_of_range
  const_reference at(size_type pos) const {
    int index = 0;
    if (!empty()) {
      if (!left_chunks_.empty()) {
        for (int i = left_chunks_.size() - 1; i >= 0; i--) {
          for (int j = left_chunks_[i].count - 1; j >= 0; j--) {
            if (index == pos) {
              const_reference ref = left_chunks_[i].elements[j];
              return ref;
            }
            ++index;
          }
        }
      }

      if (!right_chunks_.empty()) {
        for (int i = 0; i < right_chunks_.size(); ++i) {
          for (int j = 0; j < right_chunks_[i].count; ++j) {
            if (index == pos) {
              const_reference ref = right_chunks_[i].elements[j];
              return ref;
            }
            ++index;
          }
        }
      }
    }
    std::string message = std::to_string(pos);
    throw std::out_of_range("index " + message + " is out of deque bounds");
  }

  /// @brief Returns a reference to the element at specified location pos. No
  /// bounds checking is performed.
  /// @param pos position of the element to return
  /// @return Reference to the requested element.
  reference operator[](size_type pos) {
    int index = 0;
    if (!empty()) {
      if (!left_chunks_.empty()) {
        for (int i = left_chunks_.size() - 1; i >= 0; i--) {
          for (int j = left_chunks_[i]->count - 1; j >= 0; j--) {
            if (index == pos) {
              reference ref = left_chunks_[i]->elements[j];
              return ref;
            }
            ++index;
          }
        }
      }

      if (!right_chunks_.empty()) {
        for (int i = 0; i < right_chunks_.size(); ++i) {
          for (int j = 0; j < right_chunks_[i]->count; ++j) {
            if (index == pos) {
              reference ref = right_chunks_[i]->elements[j];
              return ref;
            }
            ++index;
          }
        }
      }
    }
    reference ref = right_chunks_[0]->elements[0];
    return ref;
  }

  /// @brief Returns a const reference to the element at specified location pos.
  /// No bounds checking is performed.
  /// @param pos position of the element to return
  /// @return Const Reference to the requested element.
  const_reference operator[](size_type pos) const {
    int index = 0;
    if (!empty()) {
      if (!left_chunks_.empty()) {
        for (int i = left_chunks_.size() - 1; i >= 0; i--) {
          for (int j = left_chunks_[i].count - 1; j >= 0; j--) {
            if (index == pos) {
              const_reference ref = left_chunks_[i].elements[j];
              return ref;
            }
            ++index;
          }
        }
      }

      if (!right_chunks_.empty()) {
        for (int i = 0; i < right_chunks_.size(); ++i) {
          for (int j = 0; j < right_chunks_[i].count; ++j) {
            if (index == pos) {
              const_reference ref = right_chunks_[i].elements[j];
              return ref;
            }
            ++index;
          }
        }
      }
    }
  }

  /// @brief Returns a reference to the first element in the container.
  /// Calling front on an empty container is undefined.
  /// @return Reference to the first element
  reference front() {
    if (!tail->right) {
      reference ref = tail->elements[tail->count - 1];
      return ref;
    }
    reference ref = tail->elements[0];
    return ref;
  }

  /// @brief Returns a const reference to the first element in the container.
  /// Calling front on an empty container is undefined.
  /// @return Const reference to the first element
  const_reference front() const {
    if (!tail->right) {
      const_reference ref = tail->elements[tail->count - 1];
      return ref;
    }
    const_reference ref = tail->elements[0];
    return ref;
  }

  /// @brief Returns a reference to the last element in the container.
  /// Calling back on an empty container causes undefined behavior.
  /// @return Reference to the last element.
  reference back() {
    reference ref = head->elements[head->count - 1];
    return ref;
  }

  /// @brief Returns a const reference to the last element in the container.
  /// Calling back on an empty container causes undefined behavior.
  /// @return Const Reference to the last element.
  const_reference back() const {
    const_reference ref = head->elements[head->count - 1];
    return ref;
  }

  /// ITERATORS

  /// @brief Returns an iterator to the first element of the deque.
  /// If the deque is empty, the returned iterator will be equal to end().
  /// @return Iterator to the first element.
  iterator begin() noexcept {
    iterator dq(tail);
    return dq;
  }

  /// @brief Returns an iterator to the first element of the deque.
  /// If the deque is empty, the returned iterator will be equal to end().
  /// @return Iterator to the first element.
  const_iterator begin() const noexcept {
    const_iterator dq(tail);
    return dq;
  }

  /// @brief Same to begin()
  const_iterator cbegin() const noexcept {
    const_iterator dq(tail);
    return dq;
  }

  /// @brief Returns an iterator to the element following the last element of
  /// the deque. This element acts as a placeholder; attempting to access it
  /// results in undefined behavior.
  /// @return Iterator to the element following the last element.
  iterator end() noexcept {
    iterator dq(head->next, this->size());
    return dq;
  }

  /// @brief Returns an constant iterator to the element following the last
  /// element of the deque. This element acts as a placeholder; attempting to
  /// access it results in undefined behavior.
  /// @return Constant Iterator to the element following the last element.
  const_iterator end() const noexcept {
    const_iterator dq(head->next, this->size());
    return dq;
  }

  /// @brief Same to end()
  const_iterator cend() const noexcept {
    const_iterator dq(head->next, this->size());
    return dq;
  }

  /// @brief Returns a reverse iterator to the first element of the reversed
  /// deque. It corresponds to the last element of the non-reversed deque. If
  /// the deque is empty, the returned iterator is equal to rend().
  /// @return Reverse iterator to the first element.
  reverse_iterator rbegin() noexcept {
    reverse_iterator dq(head, this->size());
    return dq;
  }

  /// @brief Returns a const reverse iterator to the first element of the
  /// reversed deque. It corresponds to the last element of the non-reversed
  /// deque. If the deque is empty, the returned iterator is equal to rend().
  /// @return Const Reverse iterator to the first element.
  const_reverse_iterator rbegin() const noexcept {
    const_reverse_iterator dq(head, this->size());
    return dq;
  }

  /// @brief Same to rbegin()
  const_reverse_iterator crbegin() const noexcept {
    const_reverse_iterator dq(head, this->size());
    return dq;
  }

  /// @brief Returns a reverse iterator to the element following the last
  /// element of the reversed deque. It corresponds to the element preceding the
  /// first element of the non-reversed deque. This element acts as a
  /// placeholder, attempting to access it results in undefined behavior.
  /// @return Reverse iterator to the element following the last element.
  reverse_iterator rend() noexcept {
    reverse_iterator dq(tail->prev, this->size());
    return dq;
  }

  /// @brief Returns a const reverse iterator to the element following the last
  /// element of the reversed deque. It corresponds to the element preceding the
  /// first element of the non-reversed deque. This element acts as a
  /// placeholder, attempting to access it results in undefined behavior.
  /// @return Const Reverse iterator to the element following the last element.
  const_reverse_iterator rend() const noexcept {
    const_reverse_iterator dq(tail->prev, this->size());
    return dq;
  }

  /// @brief Same to rend()
  const_reverse_iterator crend() const noexcept {
    const_reverse_iterator dq(tail->prev, this->size());
    return dq;
  }

  /// CAPACITY

  /// @brief Checks if the container has no elements
  /// @return true if the container is empty, false otherwise
  bool empty() const noexcept { return !number_of_elements_; }

  /// @brief Returns the number of elements in the container
  /// @return The number of elements in the container.
  size_type size() const noexcept { return number_of_elements_; }

  /// @brief Erases all elements from the container.
  /// nvalidates any references, pointers, or iterators referring to contained
  /// elements. Any past-the-end iterators are also invalidated.
  void clear() noexcept {
    int size = this->size();
    for (int i = 0; i < size; ++i) {
      if (!this->right_chunks_.empty()) {
        this->pop_back();
      }
      if (!this->left_chunks_.empty()) {
        this->pop_front();
      }
    }
  }

  /// @brief Inserts value before pos.
  /// @param pos iterator before which the content will be inserted.
  /// @param value element value to insert
  /// @return Iterator pointing to the inserted value.
  iterator insert(const_iterator pos, const T& value) {
    this[pos] = value;
    iterator it;
  }

  /// @brief Appends the given element value to the end of the container.
  /// The new element is initialized as a copy of value.
  /// @param value the value of the element to append
  void push_back(const T& value) { emplace_back(value); }

  /// @brief Appends the given element value to the end of the container.
  /// Value is moved into the new element.
  /// @param value the value of the element to append
  void push_back(T&& value) { emplace_back(std::move(value)); }

  template <class A>
  pointer emplace_back(A value) {
    if (right_chunks_.back()->count == right_chunks_.back()->size) {
      createRightChunk();
    }
    right_chunks_.back()->elements[right_chunks_.back()->count] = value;
    pointer ref = &right_chunks_.back()->elements[right_chunks_.back()->count];
    right_chunks_.back()->count++;
    ++number_of_elements_;
    return ref;
  }

  /// @brief Appends a new element to the end of the container.
  /// @param ...args arguments to forward to the constructor of the element
  /// @return A reference to the inserted element.
  template <class... Args>
  reference emplace_back(Args&&... args) {
    pointer ref;
    for (auto v : {args...}) {
      ref = emplace_back(v);
    }
    reference result = *ref;
    return result;
  }

  /// @brief Removes the last element of the container.
  void pop_back() {
    if (head->count) {
      head->count--;
      --number_of_elements_;
    }
    if (!head->count && right_chunks_.size() > 1) {
      deleteRightChunk();
      head = right_chunks_.back();
    }
  }

  /// @brief Prepends the given element value to the beginning of the container.
  /// @param value the value of the element to prepend
  void push_front(const T& value) { emplace_front(value); }

  /// @brief Prepends the given element value to the beginning of the container.
  /// @param value moved value of the element to prepend
  void push_front(T&& value) { emplace_front(std::move(value)); }

  template <class A>
  pointer emplace_front(A value) {
    if (left_chunks_.empty() ||
        left_chunks_.back()->count == left_chunks_.back()->size) {
      createLeftChunk();
    }
    left_chunks_.back()->elements[left_chunks_.back()->count] = value;
    pointer ref = &left_chunks_.back()->elements[left_chunks_.back()->count];
    left_chunks_.back()->count++;
    ++number_of_elements_;
    return ref;
  }
  /// @brief Inserts a new element to the beginning of the container.
  /// @param ...args arguments to forward to the constructor of the element
  /// @return A reference to the inserted element.
  template <class... Args>
  reference emplace_front(Args&&... args) {
    pointer ref;
    for (auto v : {args...}) {
      ref = emplace_front(v);
    }
    reference result = *ref;
    return result;
  }

  /// @brief Removes the first element of the container.
  void pop_front() {
    if (tail != nullptr) {
      if (tail->count) {
        tail->count--;
        --number_of_elements_;
      }
      if (!tail->count) {
        deleteLeftChunk();
        if (!left_chunks_.empty()) {
          tail = left_chunks_.back();
        } else {
          tail = nullptr;
        }
      }
    }
  }

  /// @brief Resizes the container to contain count elements.
  /// If the current size is greater than count, the container is reduced to its
  /// first count elements. If the current size is less than count, additional
  /// default-inserted elements are appended
  /// @param count new size of the container
  void resize(size_type count) {
    size_t size = this->size();
    if (count < size) {
      for (int i = 0; i < size - count; ++i) {
        this->pop_back();
      }
      size = this->size();
      if (count < size) {
        for (int i = 0; i < size - count; ++i) {
          this->pop_front();
        }
      }
    }
  }

  /// @brief Resizes the container to contain count elements.
  /// If the current size is greater than count, the container is reduced to its
  /// first count elements. If the current size is less than count, additional
  /// copies of value are appended.
  /// @param count new size of the container
  /// @param value the value to initialize the new elements with
  void resize(size_type count, const value_type& value) {
    this->clear();
    for (int i = 0; i < count; ++i) {
      this->push_back(value);
    }
  }

  /// @brief Checks if the contents of lhs and rhs are equal
  /// @param lhs,rhs deques whose contents to compare
  template <class U, class Alloc>
  friend bool operator==(const Deque<U, Alloc>& lhs,
                         const Deque<U, Alloc>& rhs) {
    if (lhs.size() != rhs.size()) return false;
    for (int i = 0; i < lhs.right_chunks_.size(); ++i) {
      for (int j = 0; j < lhs.right_chunks_[i]->count; ++j) {
        if (lhs.right_chunks_[i]->elements[j] !=
            rhs.right_chunks_[i]->elements[j]) {
          return false;
        }
      }
    }
    for (int i = 0; i < lhs.left_chunks_.size(); ++i) {
      for (int j = 0; j < lhs.left_chunks_[i]->count; ++j) {
        if (lhs.left_chunks_[i]->elements[j] !=
            rhs.left_chunks_[i]->elements[j]) {
          return false;
        }
      }
    }
    return true;
  }

  /// @brief Checks if the contents of lhs and rhs are not equal
  /// @param lhs,rhs deques whose contents to compare
  template <class U, class Alloc>
  friend bool operator!=(const Deque<U, Alloc>& lhs,
                         const Deque<U, Alloc>& rhs) {
    return !(lhs == rhs);
  }

  /// @brief Compares the contents of lhs and rhs lexicographically.
  /// @param lhs,rhs deques whose contents to compare
  template <class U, class Alloc>
  friend bool operator>(const Deque<U, Alloc>& lhs,
                        const Deque<U, Alloc>& rhs) {
    return !(lhs <= rhs);
  }

  /// @brief Compares the contents of lhs and rhs lexicographically.
  /// @param lhs,rhs deques whose contents to compare
  template <class U, class Alloc>
  friend bool operator<(const Deque<U, Alloc>& lhs,
                        const Deque<U, Alloc>& rhs) {
    if (lhs == rhs) {
      return false;
    }
    if (lhs.size() != rhs.size()) {
      if (lhs.size() > rhs.size()) {
        return false;
      } else {
        return true;
      }
    }
    for (int i = 0; i < lhs.right_chunks_.size(); ++i) {
      for (int j = 0; j < lhs.right_chunks_[i]->count; ++j) {
        if (lhs.right_chunks_[i]->elements[j] >
            rhs.right_chunks_[i]->elements[j]) {
          return false;
        }
      }
    }
    for (int i = 0; i < lhs.left_chunks_.size(); ++i) {
      for (int j = 0; j < lhs.left_chunks_[i]->count; ++j) {
        if (lhs.left_chunks_[i]->elements[j] >
            rhs.left_chunks_[i]->elements[j]) {
          return false;
        }
      }
    }
    return true;
  }

  /// @brief Compares the contents of lhs and rhs lexicographically.
  /// @param lhs,rhs deques whose contents to compare
  template <class U, class Alloc>
  friend bool operator>=(const Deque<U, Alloc>& lhs,
                         const Deque<U, Alloc>& rhs) {
    return !(lhs < rhs);
  }

  /// @brief Compares the contents of lhs and rhs lexicographically.
  /// @param lhs,rhs deques whose contents to compare
  template <class U, class Alloc>
  friend bool operator<=(const Deque<U, Alloc>& lhs,
                         const Deque<U, Alloc>& rhs) {
    return (lhs < rhs || lhs == rhs);
  }

 private:
  allocator_type allocator;
  std::vector<Chunk<value_type>*> left_chunks_;
  std::vector<Chunk<value_type>*> right_chunks_;
  size_t number_of_elements_ = 0;
  Chunk<value_type>* tail = nullptr;
  Chunk<value_type>* head = nullptr;

  void createRightChunk() {
    auto* new_chunk = new Chunk<value_type>(&allocator, RIGHT);

    if (right_chunks_.empty()) {
      new_chunk->prev = nullptr;
      new_chunk->next = nullptr;
      if (!left_chunks_.empty()) {
        new_chunk->prev = left_chunks_.front();
        left_chunks_.front()->next = new_chunk;
      }
    } else {
      new_chunk->prev = right_chunks_.back();
      right_chunks_.back()->next = new_chunk;
      new_chunk->next = nullptr;
    }

    right_chunks_.push_back(new_chunk);

    head = right_chunks_.back();
    if (left_chunks_.empty()) {
      tail = right_chunks_.front();
    }
  }

  void deleteRightChunk() {
    delete right_chunks_.back();
    right_chunks_.pop_back();
  }

  void createLeftChunk() {
    auto* new_chunk = new Chunk<value_type>(&allocator, LEFT);

    if (left_chunks_.empty()) {
      new_chunk->next = nullptr;
      new_chunk->prev = nullptr;
      if (!right_chunks_.empty()) {
        new_chunk->next = right_chunks_.front();
        right_chunks_.front()->prev = new_chunk;
      }
    } else {
      new_chunk->next = left_chunks_.front();
      left_chunks_.front()->prev = new_chunk;
      new_chunk->prev = nullptr;
    }

    left_chunks_.push_back(new_chunk);

    tail = left_chunks_.back();
    if (right_chunks_.empty()) {
      head = left_chunks_.front();
    }
  }

  void deleteLeftChunk() {
    delete left_chunks_.back();
    left_chunks_.pop_back();
  }
};
}  // namespace fefu_laboratory_two
