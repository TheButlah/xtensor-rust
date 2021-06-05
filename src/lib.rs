mod primitives;

use ffi::DType;
use primitives::TensorPrimitive;

#[cxx::bridge]
mod ffi {

    pub enum DType {
        Int8,
        UInt8,
        Float32,
        Float64,
    }
}

pub struct TensorView<'a> {
    shape: &'a [usize],
    dtype: DType,
    bytes: *const u8,
}
impl TensorView<'_> {
    pub const fn shape(&self) -> &[usize] {
        self.shape
    }

    pub const fn dtype(&self) -> DType {
        self.dtype
    }

    pub const fn bytes(&self) -> *const u8 {
        self.bytes
    }
}

pub struct TensorViewMut<'a> {
    shape: &'a [usize],
    dtype: DType,
    bytes: *mut u8,
}
impl TensorViewMut<'_> {
    pub const fn shape(&self) -> &[usize] {
        self.shape
    }

    pub const fn dtype(&self) -> DType {
        self.dtype
    }

    pub fn bytes(&mut self) -> *mut u8 {
        self.bytes
    }
}

pub struct RsTensor<T> {
    inner: ndarray::ArrayD<T>,
}
impl<T: TensorPrimitive> RsTensor<T> {
    pub fn view(&self) -> TensorView<'_> {
        TensorView {
            shape: self.inner.shape(),
            dtype: T::DTYPE,
            bytes: self.inner.as_ptr().cast(),
        }
    }

    pub fn view_mut(&mut self) -> TensorViewMut<'_> {
        let bytes = self.inner.as_mut_ptr().cast();
        TensorViewMut {
            // TODO: Is it safe to effectively reborrow as immutable?
            shape: self.inner.shape(),
            dtype: T::DTYPE,
            bytes,
        }
    }
}
