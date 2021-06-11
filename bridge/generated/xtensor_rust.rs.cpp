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

template <typename T>
class Slice<T>::uninit {};
template <typename T>
inline Slice<T>::Slice(uninit) noexcept {}

namespace {
namespace repr {
using Fat = ::std::array<::std::uintptr_t, 2>;
} // namespace repr

template <typename T>
class impl<Slice<T>> final {
public:
  static Slice<T> slice(repr::Fat repr) noexcept {
    Slice<T> slice = typename Slice<T>::uninit{};
    slice.repr = repr;
    return slice;
  }
};
} // namespace
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

extern "C" {
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorU8$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorU8$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU8$shape(const ::xtensor_rust::bridge::RsTensorU8 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU8$as_slice(const ::xtensor_rust::bridge::RsTensorU8 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU8$as_slice_mut(::xtensor_rust::bridge::RsTensorU8 &self) noexcept;

::xtensor_rust::bridge::RsTensorU8 *xtensor_rust$bridge$cxxbridge1$rsU8_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint8_t *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorI8$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorI8$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI8$shape(const ::xtensor_rust::bridge::RsTensorI8 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI8$as_slice(const ::xtensor_rust::bridge::RsTensorI8 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI8$as_slice_mut(::xtensor_rust::bridge::RsTensorI8 &self) noexcept;

::xtensor_rust::bridge::RsTensorI8 *xtensor_rust$bridge$cxxbridge1$rsI8_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int8_t *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorU16$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorU16$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU16$shape(const ::xtensor_rust::bridge::RsTensorU16 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU16$as_slice(const ::xtensor_rust::bridge::RsTensorU16 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU16$as_slice_mut(::xtensor_rust::bridge::RsTensorU16 &self) noexcept;

::xtensor_rust::bridge::RsTensorU16 *xtensor_rust$bridge$cxxbridge1$rsU16_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint16_t *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorI16$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorI16$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI16$shape(const ::xtensor_rust::bridge::RsTensorI16 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI16$as_slice(const ::xtensor_rust::bridge::RsTensorI16 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI16$as_slice_mut(::xtensor_rust::bridge::RsTensorI16 &self) noexcept;

::xtensor_rust::bridge::RsTensorI16 *xtensor_rust$bridge$cxxbridge1$rsI16_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int16_t *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorU32$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorU32$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU32$shape(const ::xtensor_rust::bridge::RsTensorU32 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU32$as_slice(const ::xtensor_rust::bridge::RsTensorU32 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU32$as_slice_mut(::xtensor_rust::bridge::RsTensorU32 &self) noexcept;

::xtensor_rust::bridge::RsTensorU32 *xtensor_rust$bridge$cxxbridge1$rsU32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint32_t *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorI32$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorI32$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI32$shape(const ::xtensor_rust::bridge::RsTensorI32 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI32$as_slice(const ::xtensor_rust::bridge::RsTensorI32 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI32$as_slice_mut(::xtensor_rust::bridge::RsTensorI32 &self) noexcept;

::xtensor_rust::bridge::RsTensorI32 *xtensor_rust$bridge$cxxbridge1$rsI32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int32_t *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorU64$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorU64$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU64$shape(const ::xtensor_rust::bridge::RsTensorU64 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU64$as_slice(const ::xtensor_rust::bridge::RsTensorU64 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorU64$as_slice_mut(::xtensor_rust::bridge::RsTensorU64 &self) noexcept;

::xtensor_rust::bridge::RsTensorU64 *xtensor_rust$bridge$cxxbridge1$rsU64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint64_t *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorI64$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorI64$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI64$shape(const ::xtensor_rust::bridge::RsTensorI64 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI64$as_slice(const ::xtensor_rust::bridge::RsTensorI64 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorI64$as_slice_mut(::xtensor_rust::bridge::RsTensorI64 &self) noexcept;

::xtensor_rust::bridge::RsTensorI64 *xtensor_rust$bridge$cxxbridge1$rsI64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int64_t *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorF32$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorF32$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorF32$shape(const ::xtensor_rust::bridge::RsTensorF32 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorF32$as_slice(const ::xtensor_rust::bridge::RsTensorF32 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorF32$as_slice_mut(::xtensor_rust::bridge::RsTensorF32 &self) noexcept;

::xtensor_rust::bridge::RsTensorF32 *xtensor_rust$bridge$cxxbridge1$rsF32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const float *ptr) noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorF64$operator$sizeof() noexcept;
::std::size_t xtensor_rust$bridge$cxxbridge1$RsTensorF64$operator$alignof() noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorF64$shape(const ::xtensor_rust::bridge::RsTensorF64 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorF64$as_slice(const ::xtensor_rust::bridge::RsTensorF64 &self) noexcept;

::rust::repr::Fat xtensor_rust$bridge$cxxbridge1$RsTensorF64$as_slice_mut(::xtensor_rust::bridge::RsTensorF64 &self) noexcept;

::xtensor_rust::bridge::RsTensorF64 *xtensor_rust$bridge$cxxbridge1$rsF64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const double *ptr) noexcept;
} // extern "C"

::std::size_t RsTensorU8::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorU8$operator$sizeof();
}

::std::size_t RsTensorU8::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorU8$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorU8::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU8$shape(*this));
}

::rust::Slice<const ::std::uint8_t> RsTensorU8::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::uint8_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU8$as_slice(*this));
}

::rust::Slice<::std::uint8_t> RsTensorU8::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<::std::uint8_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU8$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorU8> rsU8_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint8_t *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorU8>::from_raw(xtensor_rust$bridge$cxxbridge1$rsU8_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorI8::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorI8$operator$sizeof();
}

::std::size_t RsTensorI8::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorI8$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorI8::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI8$shape(*this));
}

::rust::Slice<const ::std::int8_t> RsTensorI8::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::int8_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI8$as_slice(*this));
}

::rust::Slice<::std::int8_t> RsTensorI8::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<::std::int8_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI8$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorI8> rsI8_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int8_t *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorI8>::from_raw(xtensor_rust$bridge$cxxbridge1$rsI8_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorU16::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorU16$operator$sizeof();
}

::std::size_t RsTensorU16::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorU16$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorU16::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU16$shape(*this));
}

::rust::Slice<const ::std::uint16_t> RsTensorU16::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::uint16_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU16$as_slice(*this));
}

::rust::Slice<::std::uint16_t> RsTensorU16::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<::std::uint16_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU16$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorU16> rsU16_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint16_t *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorU16>::from_raw(xtensor_rust$bridge$cxxbridge1$rsU16_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorI16::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorI16$operator$sizeof();
}

::std::size_t RsTensorI16::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorI16$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorI16::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI16$shape(*this));
}

::rust::Slice<const ::std::int16_t> RsTensorI16::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::int16_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI16$as_slice(*this));
}

::rust::Slice<::std::int16_t> RsTensorI16::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<::std::int16_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI16$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorI16> rsI16_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int16_t *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorI16>::from_raw(xtensor_rust$bridge$cxxbridge1$rsI16_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorU32::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorU32$operator$sizeof();
}

::std::size_t RsTensorU32::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorU32$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorU32::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU32$shape(*this));
}

::rust::Slice<const ::std::uint32_t> RsTensorU32::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::uint32_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU32$as_slice(*this));
}

::rust::Slice<::std::uint32_t> RsTensorU32::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<::std::uint32_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU32$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorU32> rsU32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint32_t *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorU32>::from_raw(xtensor_rust$bridge$cxxbridge1$rsU32_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorI32::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorI32$operator$sizeof();
}

::std::size_t RsTensorI32::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorI32$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorI32::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI32$shape(*this));
}

::rust::Slice<const ::std::int32_t> RsTensorI32::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::int32_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI32$as_slice(*this));
}

::rust::Slice<::std::int32_t> RsTensorI32::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<::std::int32_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI32$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorI32> rsI32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int32_t *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorI32>::from_raw(xtensor_rust$bridge$cxxbridge1$rsI32_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorU64::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorU64$operator$sizeof();
}

::std::size_t RsTensorU64::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorU64$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorU64::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU64$shape(*this));
}

::rust::Slice<const ::std::uint64_t> RsTensorU64::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::uint64_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU64$as_slice(*this));
}

::rust::Slice<::std::uint64_t> RsTensorU64::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<::std::uint64_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorU64$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorU64> rsU64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::uint64_t *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorU64>::from_raw(xtensor_rust$bridge$cxxbridge1$rsU64_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorI64::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorI64$operator$sizeof();
}

::std::size_t RsTensorI64::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorI64$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorI64::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI64$shape(*this));
}

::rust::Slice<const ::std::int64_t> RsTensorI64::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::int64_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI64$as_slice(*this));
}

::rust::Slice<::std::int64_t> RsTensorI64::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<::std::int64_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorI64$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorI64> rsI64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const ::std::int64_t *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorI64>::from_raw(xtensor_rust$bridge$cxxbridge1$rsI64_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorF32::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorF32$operator$sizeof();
}

::std::size_t RsTensorF32::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorF32$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorF32::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorF32$shape(*this));
}

::rust::Slice<const float> RsTensorF32::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const float>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorF32$as_slice(*this));
}

::rust::Slice<float> RsTensorF32::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<float>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorF32$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorF32> rsF32_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const float *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorF32>::from_raw(xtensor_rust$bridge$cxxbridge1$rsF32_copy_from_ptr(shape, ptr));
}

::std::size_t RsTensorF64::layout::size() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorF64$operator$sizeof();
}

::std::size_t RsTensorF64::layout::align() noexcept {
  return xtensor_rust$bridge$cxxbridge1$RsTensorF64$operator$alignof();
}

::rust::Slice<const ::std::size_t> RsTensorF64::shape() const noexcept {
  return ::rust::impl<::rust::Slice<const ::std::size_t>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorF64$shape(*this));
}

::rust::Slice<const double> RsTensorF64::as_slice() const noexcept {
  return ::rust::impl<::rust::Slice<const double>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorF64$as_slice(*this));
}

::rust::Slice<double> RsTensorF64::as_slice_mut() noexcept {
  return ::rust::impl<::rust::Slice<double>>::slice(xtensor_rust$bridge$cxxbridge1$RsTensorF64$as_slice_mut(*this));
}

::rust::Box<::xtensor_rust::bridge::RsTensorF64> rsF64_copy_from_ptr(::rust::Slice<const ::std::size_t> shape, const double *ptr) noexcept {
  return ::rust::Box<::xtensor_rust::bridge::RsTensorF64>::from_raw(xtensor_rust$bridge$cxxbridge1$rsF64_copy_from_ptr(shape, ptr));
}
} // namespace bridge
} // namespace xtensor_rust

extern "C" {
::xtensor_rust::bridge::RsTensorU8 *cxxbridge1$box$xtensor_rust$bridge$RsTensorU8$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorU8$dealloc(::xtensor_rust::bridge::RsTensorU8 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorU8$drop(::rust::Box<::xtensor_rust::bridge::RsTensorU8> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorI8 *cxxbridge1$box$xtensor_rust$bridge$RsTensorI8$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorI8$dealloc(::xtensor_rust::bridge::RsTensorI8 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorI8$drop(::rust::Box<::xtensor_rust::bridge::RsTensorI8> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorU16 *cxxbridge1$box$xtensor_rust$bridge$RsTensorU16$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorU16$dealloc(::xtensor_rust::bridge::RsTensorU16 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorU16$drop(::rust::Box<::xtensor_rust::bridge::RsTensorU16> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorI16 *cxxbridge1$box$xtensor_rust$bridge$RsTensorI16$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorI16$dealloc(::xtensor_rust::bridge::RsTensorI16 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorI16$drop(::rust::Box<::xtensor_rust::bridge::RsTensorI16> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorU32 *cxxbridge1$box$xtensor_rust$bridge$RsTensorU32$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorU32$dealloc(::xtensor_rust::bridge::RsTensorU32 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorU32$drop(::rust::Box<::xtensor_rust::bridge::RsTensorU32> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorI32 *cxxbridge1$box$xtensor_rust$bridge$RsTensorI32$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorI32$dealloc(::xtensor_rust::bridge::RsTensorI32 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorI32$drop(::rust::Box<::xtensor_rust::bridge::RsTensorI32> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorU64 *cxxbridge1$box$xtensor_rust$bridge$RsTensorU64$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorU64$dealloc(::xtensor_rust::bridge::RsTensorU64 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorU64$drop(::rust::Box<::xtensor_rust::bridge::RsTensorU64> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorI64 *cxxbridge1$box$xtensor_rust$bridge$RsTensorI64$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorI64$dealloc(::xtensor_rust::bridge::RsTensorI64 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorI64$drop(::rust::Box<::xtensor_rust::bridge::RsTensorI64> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorF32 *cxxbridge1$box$xtensor_rust$bridge$RsTensorF32$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorF32$dealloc(::xtensor_rust::bridge::RsTensorF32 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorF32$drop(::rust::Box<::xtensor_rust::bridge::RsTensorF32> *ptr) noexcept;

::xtensor_rust::bridge::RsTensorF64 *cxxbridge1$box$xtensor_rust$bridge$RsTensorF64$alloc() noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorF64$dealloc(::xtensor_rust::bridge::RsTensorF64 *) noexcept;
void cxxbridge1$box$xtensor_rust$bridge$RsTensorF64$drop(::rust::Box<::xtensor_rust::bridge::RsTensorF64> *ptr) noexcept;
} // extern "C"

namespace rust {
inline namespace cxxbridge1 {
template <>
::xtensor_rust::bridge::RsTensorU8 *Box<::xtensor_rust::bridge::RsTensorU8>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorU8$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorU8>::allocation::dealloc(::xtensor_rust::bridge::RsTensorU8 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorU8$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorU8>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorU8$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorI8 *Box<::xtensor_rust::bridge::RsTensorI8>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorI8$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorI8>::allocation::dealloc(::xtensor_rust::bridge::RsTensorI8 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorI8$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorI8>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorI8$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorU16 *Box<::xtensor_rust::bridge::RsTensorU16>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorU16$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorU16>::allocation::dealloc(::xtensor_rust::bridge::RsTensorU16 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorU16$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorU16>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorU16$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorI16 *Box<::xtensor_rust::bridge::RsTensorI16>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorI16$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorI16>::allocation::dealloc(::xtensor_rust::bridge::RsTensorI16 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorI16$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorI16>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorI16$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorU32 *Box<::xtensor_rust::bridge::RsTensorU32>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorU32$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorU32>::allocation::dealloc(::xtensor_rust::bridge::RsTensorU32 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorU32$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorU32>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorU32$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorI32 *Box<::xtensor_rust::bridge::RsTensorI32>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorI32$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorI32>::allocation::dealloc(::xtensor_rust::bridge::RsTensorI32 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorI32$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorI32>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorI32$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorU64 *Box<::xtensor_rust::bridge::RsTensorU64>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorU64$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorU64>::allocation::dealloc(::xtensor_rust::bridge::RsTensorU64 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorU64$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorU64>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorU64$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorI64 *Box<::xtensor_rust::bridge::RsTensorI64>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorI64$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorI64>::allocation::dealloc(::xtensor_rust::bridge::RsTensorI64 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorI64$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorI64>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorI64$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorF32 *Box<::xtensor_rust::bridge::RsTensorF32>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorF32$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorF32>::allocation::dealloc(::xtensor_rust::bridge::RsTensorF32 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorF32$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorF32>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorF32$drop(this);
}
template <>
::xtensor_rust::bridge::RsTensorF64 *Box<::xtensor_rust::bridge::RsTensorF64>::allocation::alloc() noexcept {
  return cxxbridge1$box$xtensor_rust$bridge$RsTensorF64$alloc();
}
template <>
void Box<::xtensor_rust::bridge::RsTensorF64>::allocation::dealloc(::xtensor_rust::bridge::RsTensorF64 *ptr) noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorF64$dealloc(ptr);
}
template <>
void Box<::xtensor_rust::bridge::RsTensorF64>::drop() noexcept {
  cxxbridge1$box$xtensor_rust$bridge$RsTensorF64$drop(this);
}
} // namespace cxxbridge1
} // namespace rust
