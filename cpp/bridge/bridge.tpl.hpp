#pragma once

using ::rust::Slice, ::rust::Box;

namespace xtensor_rust::bridge {

template <> class RsTensor<uint8_t> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const uint8_t *ptr)
      : inner(rsU8_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const uint8_t> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<uint8_t> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorU8> inner;
};

template <> class RsTensor<int8_t> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const int8_t *ptr)
      : inner(rsI8_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const int8_t> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<int8_t> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorI8> inner;
};

template <> class RsTensor<uint16_t> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const uint16_t *ptr)
      : inner(rsU16_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const uint16_t> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<uint16_t> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorU16> inner;
};

template <> class RsTensor<int16_t> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const int16_t *ptr)
      : inner(rsI16_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const int16_t> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<int16_t> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorI16> inner;
};

template <> class RsTensor<uint32_t> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const uint32_t *ptr)
      : inner(rsU32_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const uint32_t> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<uint32_t> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorU32> inner;
};

template <> class RsTensor<int32_t> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const int32_t *ptr)
      : inner(rsI32_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const int32_t> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<int32_t> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorI32> inner;
};

template <> class RsTensor<uint64_t> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const uint64_t *ptr)
      : inner(rsU64_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const uint64_t> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<uint64_t> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorU64> inner;
};

template <> class RsTensor<int64_t> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const int64_t *ptr)
      : inner(rsI64_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const int64_t> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<int64_t> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorI64> inner;
};

template <> class RsTensor<float> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const float *ptr)
      : inner(rsF32_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const float> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<float> as_slice_mut() noexcept { return inner->as_slice_mut(); }

private:
  ::rust::Box<RsTensorF32> inner;
};

template <> class RsTensor<double> {
public:
  RsTensor(::rust::Slice<const size_t> shape, const double *ptr)
      : inner(rsF64_copy_from_ptr(shape, ptr)) {}

  ::rust::Slice<const size_t> shape() const noexcept { return inner->shape(); }
  ::rust::Slice<const double> as_slice() const noexcept {
    return inner->as_slice();
  }
  ::rust::Slice<double> as_slice_mut() noexcept {
    return inner->as_slice_mut();
  }

private:
  ::rust::Box<RsTensorF64> inner;
};

} // namespace xtensor_rust::bridge
