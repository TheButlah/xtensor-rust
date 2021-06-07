mod primitives;

use ffi::DType;
use paste::paste;

#[cxx::bridge(namespace = "xtensor_rust::bridge")]
mod ffi {

    pub enum DType {
        Int8,
        UInt8,
        Float32,
        Float64,
    }

    extern "Rust" {}

    // Unfortunately we could not automate this boilerplate with a macro
    extern "Rust" {
        type RsTensorU8;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[u8];
        fn as_slice_mut(&mut self) -> &mut [u8];
    }

    extern "Rust" {
        type RsTensorI8;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[i8];
        fn as_slice_mut(&mut self) -> &mut [i8];
    }

    extern "Rust" {
        type RsTensorU16;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[u16];
        fn as_slice_mut(&mut self) -> &mut [u16];
    }

    extern "Rust" {
        type RsTensorI16;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[i16];
        fn as_slice_mut(&mut self) -> &mut [i16];
    }

    extern "Rust" {
        type RsTensorU32;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[u32];
        fn as_slice_mut(&mut self) -> &mut [u32];
    }

    extern "Rust" {
        type RsTensorI32;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[i32];
        fn as_slice_mut(&mut self) -> &mut [i32];
    }

    extern "Rust" {
        type RsTensorU64;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[u64];
        fn as_slice_mut(&mut self) -> &mut [u64];
    }

    extern "Rust" {
        type RsTensorI64;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[i64];
        fn as_slice_mut(&mut self) -> &mut [i64];
    }

    extern "Rust" {
        type RsTensorF32;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[f32];
        fn as_slice_mut(&mut self) -> &mut [f32];
    }

    extern "Rust" {
        type RsTensorF64;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[f64];
        fn as_slice_mut(&mut self) -> &mut [f64];
    }
}

/// Handles creating newtypes for RsTensor<T> for all the primitive T's
macro_rules! newtypes {
    ($typ:ty) => {
        paste! {
            struct [<RsTensor $typ:upper>](RsTensor<$typ>);
            // Project the wrapped methods to the inner type
            impl [<RsTensor $typ:upper>] {
                pub fn shape(&self) -> &[usize] {
                    self.0.shape()
                }
                pub fn as_slice(&self) -> &[$typ] {
                    self.0.as_slice()
                }

                pub fn as_slice_mut(&mut self) -> &mut [$typ] {
                    self.0.as_slice_mut()
                }
            }
            // Implement conversion traits to/from inner type
            impl From<RsTensor<$typ>> for [<RsTensor $typ:upper>] {
                fn from(tensor: RsTensor<$typ>) -> Self { Self(tensor) }
            }
            impl From<[<RsTensor $typ:upper>]> for RsTensor<$typ> {
                fn from(wrapped: [<RsTensor $typ:upper>]) -> Self { wrapped.0 }
            }
        }
    };
    ($typ:ty, $($tail:ty),+) => {
        newtypes!($typ);
        newtypes!($($tail),+);
    }
}
newtypes!(u8, i8, u16, i16, u32, i32, u64, i64, f32, f64);

pub struct RsTensor<T>(ndarray::ArrayD<T>);
impl<T: Copy> RsTensor<T> {
    /// Returns Err(()) if the array was not a standard layout. See `[ndarray::ArrayBase::is_standard_layout()]`
    pub fn new(arr: ndarray::ArrayD<T>) -> Result<Self, ()> {
        if arr.is_standard_layout() {
            Ok(Self(arr))
        } else {
            Err(())
        }
    }

    pub unsafe fn copy_from_ptr(shape: &[usize], ptr: *const T) -> Self {
        let view = ndarray::ArrayViewD::from_shape_ptr(shape, ptr);
        let arr = view.to_owned();
        Self(arr)
    }

    pub fn shape(&self) -> &[usize] {
        self.0.shape()
    }

    pub fn as_slice(&self) -> &[T] {
        self.0.as_slice().unwrap()
    }

    pub fn as_slice_mut(&mut self) -> &mut [T] {
        self.0.as_slice_mut().unwrap()
    }
}
impl<T> From<RsTensor<T>> for ndarray::ArrayD<T> {
    fn from(tensor: RsTensor<T>) -> Self {
        tensor.0
    }
}
