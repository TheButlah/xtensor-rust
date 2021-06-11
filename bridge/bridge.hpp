#pragma once

#include "bridge/generated/xtensor_rust.rs.hpp"

namespace xtensor_rust::bridge {

using ::rust::Slice, ::rust::Box;

template <typename T> class RsTensor {
public:
  RsTensor(::rust::Slice<const ::std::size_t> shape, const T *ptr);
  ::rust::Slice<const ::std::size_t> shape() const noexcept;
  ::rust::Slice<const T> as_slice() const noexcept;
  ::rust::Slice<T> as_slice_mut() noexcept;
};

} // namespace xtensor_rust::bridge

// Create specializations for all supported dtypes
#include "bridge/bridge.tpl.hpp"
