#pragma once
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace rust {
inline namespace cxxbridge1 {
// #include "rust/cxx.h"

#ifndef CXXBRIDGE1_PANIC
#define CXXBRIDGE1_PANIC
template <typename Exception>
void panic [[noreturn]] (const char *msg);
#endif // CXXBRIDGE1_PANIC

namespace {
template <typename T>
class impl;
} // namespace

template <typename T>
::std::size_t size_of();
template <typename T>
::std::size_t align_of();

#ifndef CXXBRIDGE1_RUST_SLICE
#define CXXBRIDGE1_RUST_SLICE
namespace detail {
template <bool>
struct copy_assignable_if {};

template <>
struct copy_assignable_if<false> {
  copy_assignable_if() noexcept = default;
  copy_assignable_if(const copy_assignable_if &) noexcept = default;
  copy_assignable_if &operator=(const copy_assignable_if &) &noexcept = delete;
  copy_assignable_if &operator=(copy_assignable_if &&) &noexcept = default;
};
} // namespace detail

template <typename T>
class Slice final
    : private detail::copy_assignable_if<std::is_const<T>::value> {
public:
  using value_type = T;

  Slice() noexcept;
  Slice(T *, std::size_t count) noexcept;

  Slice &operator=(const Slice<T> &) &noexcept = default;
  Slice &operator=(Slice<T> &&) &noexcept = default;

  T *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  T &operator[](std::size_t n) const noexcept;
  T &at(std::size_t n) const;
  T &front() const noexcept;
  T &back() const noexcept;

  Slice(const Slice<T> &) noexcept = default;
  ~Slice() noexcept = default;

  class iterator;
  iterator begin() const noexcept;
  iterator end() const noexcept;

  void swap(Slice &) noexcept;

private:
  class uninit;
  Slice(uninit) noexcept;
  friend impl<Slice>;
  friend void sliceInit(void *, const void *, std::size_t) noexcept;
  friend void *slicePtr(const void *) noexcept;
  friend std::size_t sliceLen(const void *) noexcept;

  std::array<std::uintptr_t, 2> repr;
};

template <typename T>
class Slice<T>::iterator final {
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = typename std::add_pointer<T>::type;
  using reference = typename std::add_lvalue_reference<T>::type;

  reference operator*() const noexcept;
  pointer operator->() const noexcept;
  reference operator[](difference_type) const noexcept;

  iterator &operator++() noexcept;
  iterator operator++(int) noexcept;
  iterator &operator--() noexcept;
  iterator operator--(int) noexcept;

  iterator &operator+=(difference_type) noexcept;
  iterator &operator-=(difference_type) noexcept;
  iterator operator+(difference_type) const noexcept;
  iterator operator-(difference_type) const noexcept;
  difference_type operator-(const iterator &) const noexcept;

  bool operator==(const iterator &) const noexcept;
  bool operator!=(const iterator &) const noexcept;
  bool operator<(const iterator &) const noexcept;
  bool operator<=(const iterator &) const noexcept;
  bool operator>(const iterator &) const noexcept;
  bool operator>=(const iterator &) const noexcept;

private:
  friend class Slice;
  void *pos;
  std::size_t stride;
};

template <typename T>
Slice<T>::Slice() noexcept {
  sliceInit(this, reinterpret_cast<void *>(align_of<T>()), 0);
}

template <typename T>
Slice<T>::Slice(T *s, std::size_t count) noexcept {
  assert(s != nullptr || count == 0);
  sliceInit(this,
            s == nullptr && count == 0
                ? reinterpret_cast<void *>(align_of<T>())
                : const_cast<typename std::remove_const<T>::type *>(s),
            count);
}

template <typename T>
T *Slice<T>::data() const noexcept {
  return reinterpret_cast<T *>(slicePtr(this));
}

template <typename T>
std::size_t Slice<T>::size() const noexcept {
  return sliceLen(this);
}

template <typename T>
std::size_t Slice<T>::length() const noexcept {
  return this->size();
}

template <typename T>
bool Slice<T>::empty() const noexcept {
  return this->size() == 0;
}

template <typename T>
T &Slice<T>::operator[](std::size_t n) const noexcept {
  assert(n < this->size());
  auto pos = static_cast<char *>(slicePtr(this)) + size_of<T>() * n;
  return *reinterpret_cast<T *>(pos);
}

template <typename T>
T &Slice<T>::at(std::size_t n) const {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Slice index out of range");
  }
  return (*this)[n];
}

template <typename T>
T &Slice<T>::front() const noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
T &Slice<T>::back() const noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
typename Slice<T>::iterator::reference
Slice<T>::iterator::operator*() const noexcept {
  return *static_cast<T *>(this->pos);
}

template <typename T>
typename Slice<T>::iterator::pointer
Slice<T>::iterator::operator->() const noexcept {
  return static_cast<T *>(this->pos);
}

template <typename T>
typename Slice<T>::iterator::reference Slice<T>::iterator::operator[](
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto pos = static_cast<char *>(this->pos) + this->stride * n;
  return *reinterpret_cast<T *>(pos);
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator++() noexcept {
  this->pos = static_cast<char *>(this->pos) + this->stride;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator++(int) noexcept {
  auto ret = iterator(*this);
  this->pos = static_cast<char *>(this->pos) + this->stride;
  return ret;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator--() noexcept {
  this->pos = static_cast<char *>(this->pos) - this->stride;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator--(int) noexcept {
  auto ret = iterator(*this);
  this->pos = static_cast<char *>(this->pos) - this->stride;
  return ret;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator+=(
    typename Slice<T>::iterator::difference_type n) noexcept {
  this->pos = static_cast<char *>(this->pos) + this->stride * n;
  return *this;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator-=(
    typename Slice<T>::iterator::difference_type n) noexcept {
  this->pos = static_cast<char *>(this->pos) - this->stride * n;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator+(
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ret = iterator(*this);
  ret.pos = static_cast<char *>(this->pos) + this->stride * n;
  return ret;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator-(
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ret = iterator(*this);
  ret.pos = static_cast<char *>(this->pos) - this->stride * n;
  return ret;
}

template <typename T>
typename Slice<T>::iterator::difference_type
Slice<T>::iterator::operator-(const iterator &other) const noexcept {
  auto diff = std::distance(static_cast<char *>(other.pos),
                            static_cast<char *>(this->pos));
  return diff / this->stride;
}

template <typename T>
bool Slice<T>::iterator::operator==(const iterator &other) const noexcept {
  return this->pos == other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator!=(const iterator &other) const noexcept {
  return this->pos != other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator<(const iterator &other) const noexcept {
  return this->pos < other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator<=(const iterator &other) const noexcept {
  return this->pos <= other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator>(const iterator &other) const noexcept {
  return this->pos > other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator>=(const iterator &other) const noexcept {
  return this->pos >= other.pos;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::begin() const noexcept {
  iterator it;
  it.pos = slicePtr(this);
  it.stride = size_of<T>();
  return it;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::end() const noexcept {
  iterator it = this->begin();
  it.pos = static_cast<char *>(it.pos) + it.stride * this->size();
  return it;
}

template <typename T>
void Slice<T>::swap(Slice &rhs) noexcept {
  std::swap(*this, rhs);
}
#endif // CXXBRIDGE1_RUST_SLICE

#ifndef CXXBRIDGE1_RUST_BOX
#define CXXBRIDGE1_RUST_BOX
template <typename T>
class Box final {
public:
  using element_type = T;
  using const_pointer =
      typename std::add_pointer<typename std::add_const<T>::type>::type;
  using pointer = typename std::add_pointer<T>::type;

  Box() = delete;
  Box(Box &&) noexcept;
  ~Box() noexcept;

  explicit Box(const T &);
  explicit Box(T &&);

  Box &operator=(Box &&) &noexcept;

  const T *operator->() const noexcept;
  const T &operator*() const noexcept;
  T *operator->() noexcept;
  T &operator*() noexcept;

  template <typename... Fields>
  static Box in_place(Fields &&...);

  void swap(Box &) noexcept;

  static Box from_raw(T *) noexcept;

  T *into_raw() noexcept;

  /* Deprecated */ using value_type = element_type;

private:
  class uninit;
  class allocation;
  Box(uninit) noexcept;
  void drop() noexcept;

  friend void swap(Box &lhs, Box &rhs) noexcept { lhs.swap(rhs); }

  T *ptr;
};

template <typename T>
class Box<T>::uninit {};

template <typename T>
class Box<T>::allocation {
  static T *alloc() noexcept;
  static void dealloc(T *) noexcept;

public:
  allocation() noexcept : ptr(alloc()) {}
  ~allocation() noexcept {
    if (this->ptr) {
      dealloc(this->ptr);
    }
  }
  T *ptr;
};

template <typename T>
Box<T>::Box(Box &&other) noexcept : ptr(other.ptr) {
  other.ptr = nullptr;
}

template <typename T>
Box<T>::Box(const T &val) {
  allocation alloc;
  ::new (alloc.ptr) T(val);
  this->ptr = alloc.ptr;
  alloc.ptr = nullptr;
}

template <typename T>
Box<T>::Box(T &&val) {
  allocation alloc;
  ::new (alloc.ptr) T(std::move(val));
  this->ptr = alloc.ptr;
  alloc.ptr = nullptr;
}

template <typename T>
Box<T>::~Box() noexcept {
  if (this->ptr) {
    this->drop();
  }
}

template <typename T>
Box<T> &Box<T>::operator=(Box &&other) &noexcept {
  if (this->ptr) {
    this->drop();
  }
  this->ptr = other.ptr;
  other.ptr = nullptr;
  return *this;
}

template <typename T>
const T *Box<T>::operator->() const noexcept {
  return this->ptr;
}

template <typename T>
const T &Box<T>::operator*() const noexcept {
  return *this->ptr;
}

template <typename T>
T *Box<T>::operator->() noexcept {
  return this->ptr;
}

template <typename T>
T &Box<T>::operator*() noexcept {
  return *this->ptr;
}

template <typename T>
template <typename... Fields>
Box<T> Box<T>::in_place(Fields &&...fields) {
  allocation alloc;
  auto ptr = alloc.ptr;
  ::new (ptr) T{std::forward<Fields>(fields)...};
  alloc.ptr = nullptr;
  return from_raw(ptr);
}

template <typename T>
void Box<T>::swap(Box &rhs) noexcept {
  using std::swap;
  swap(this->ptr, rhs.ptr);
}

template <typename T>
Box<T> Box<T>::from_raw(T *raw) noexcept {
  Box box = uninit{};
  box.ptr = raw;
  return box;
}

template <typename T>
T *Box<T>::into_raw() noexcept {
  T *raw = this->ptr;
  this->ptr = nullptr;
  return raw;
}

template <typename T>
Box<T>::Box(uninit) noexcept {}
#endif // CXXBRIDGE1_RUST_BOX

#ifndef CXXBRIDGE1_RUST_OPAQUE
#define CXXBRIDGE1_RUST_OPAQUE
class Opaque {
public:
  Opaque() = delete;
  Opaque(const Opaque &) = delete;
  ~Opaque() = delete;
};
#endif // CXXBRIDGE1_RUST_OPAQUE

#ifndef CXXBRIDGE1_IS_COMPLETE
#define CXXBRIDGE1_IS_COMPLETE
namespace detail {
namespace {
template <typename T, typename = std::size_t>
struct is_complete : std::false_type {};
template <typename T>
struct is_complete<T, decltype(sizeof(T))> : std::true_type {};
} // namespace
} // namespace detail
#endif // CXXBRIDGE1_IS_COMPLETE

#ifndef CXXBRIDGE1_LAYOUT
#define CXXBRIDGE1_LAYOUT
class layout {
  template <typename T>
  friend std::size_t size_of();
  template <typename T>
  friend std::size_t align_of();
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return T::layout::size();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return sizeof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      size_of() {
    return do_size_of<T>();
  }
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return T::layout::align();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return alignof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      align_of() {
    return do_align_of<T>();
  }
};

template <typename T>
std::size_t size_of() {
  return layout::size_of<T>();
}

template <typename T>
std::size_t align_of() {
  return layout::align_of<T>();
}
#endif // CXXBRIDGE1_LAYOUT
} // namespace cxxbridge1
} // namespace rust

namespace xtensor_rust {
  namespace bridge {
    struct RsTensorU8;
    struct RsTensorI8;
    struct RsTensorU16;
    struct RsTensorI16;
    struct RsTensorU32;
    struct RsTensorI32;
    struct RsTensorU64;
    struct RsTensorI64;
    struct RsTensorF32;
    struct RsTensorF64;
  }
}

namespace xtensor_rust {
namespace bridge {
#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU8
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU8
struct RsTensorU8 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const ::std::uint8_t> as_slice() const noexcept;
  ::rust::Slice<::std::uint8_t> as_slice_mut() noexcept;
  ~RsTensorU8() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU8

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI8
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI8
struct RsTensorI8 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const ::std::int8_t> as_slice() const noexcept;
  ::rust::Slice<::std::int8_t> as_slice_mut() noexcept;
  ~RsTensorI8() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI8

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU16
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU16
struct RsTensorU16 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const ::std::uint16_t> as_slice() const noexcept;
  ::rust::Slice<::std::uint16_t> as_slice_mut() noexcept;
  ~RsTensorU16() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU16

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI16
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI16
struct RsTensorI16 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const ::std::int16_t> as_slice() const noexcept;
  ::rust::Slice<::std::int16_t> as_slice_mut() noexcept;
  ~RsTensorI16() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI16

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU32
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU32
struct RsTensorU32 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const ::std::uint32_t> as_slice() const noexcept;
  ::rust::Slice<::std::uint32_t> as_slice_mut() noexcept;
  ~RsTensorU32() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU32

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI32
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI32
struct RsTensorI32 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const ::std::int32_t> as_slice() const noexcept;
  ::rust::Slice<::std::int32_t> as_slice_mut() noexcept;
  ~RsTensorI32() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI32

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU64
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU64
struct RsTensorU64 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const ::std::uint64_t> as_slice() const noexcept;
  ::rust::Slice<::std::uint64_t> as_slice_mut() noexcept;
  ~RsTensorU64() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorU64

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI64
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI64
struct RsTensorI64 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const ::std::int64_t> as_slice() const noexcept;
  ::rust::Slice<::std::int64_t> as_slice_mut() noexcept;
  ~RsTensorI64() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorI64

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorF32
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorF32
struct RsTensorF32 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const float> as_slice() const noexcept;
  ::rust::Slice<float> as_slice_mut() noexcept;
  ~RsTensorF32() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorF32

#ifndef CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorF64
#define CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorF64
struct RsTensorF64 final : public ::rust::Opaque {
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const double> as_slice() const noexcept;
  ::rust::Slice<double> as_slice_mut() noexcept;
  ~RsTensorF64() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_xtensor_rust$bridge$RsTensorF64

::rust::Box<::xtensor_rust::bridge::RsTensorU8> rsU8_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint8_t *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorI8> rsI8_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int8_t *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorU16> rsU16_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint16_t *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorI16> rsI16_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int16_t *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorU32> rsU32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint32_t *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorI32> rsI32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int32_t *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorU64> rsU64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint64_t *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorI64> rsI64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int64_t *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorF32> rsF32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const float *ptr) noexcept;

::rust::Box<::xtensor_rust::bridge::RsTensorF64> rsF64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const double *ptr) noexcept;
} // namespace bridge
} // namespace xtensor_rust
