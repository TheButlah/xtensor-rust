use paste::paste;

#[cxx::bridge(namespace = "xtensor_rust::bridge")]
mod ffi {

    // Unfortunately we could not automate this boilerplate with a macro
    extern "Rust" {
        type RsTensorU8;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[u8];
        fn as_slice_mut(&mut self) -> &mut [u8];
        unsafe fn rsU8_copy_from_ptr(shape: &[usize], ptr: *const u8) -> Box<RsTensorU8>;
    }

    extern "Rust" {
        type RsTensorI8;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[i8];
        fn as_slice_mut(&mut self) -> &mut [i8];
        unsafe fn rsI8_copy_from_ptr(shape: &[usize], ptr: *const i8) -> Box<RsTensorI8>;

    }

    extern "Rust" {
        type RsTensorU16;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[u16];
        fn as_slice_mut(&mut self) -> &mut [u16];
        unsafe fn rsU16_copy_from_ptr(shape: &[usize], ptr: *const u16) -> Box<RsTensorU16>;
    }

    extern "Rust" {
        type RsTensorI16;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[i16];
        fn as_slice_mut(&mut self) -> &mut [i16];
        unsafe fn rsI16_copy_from_ptr(shape: &[usize], ptr: *const i16) -> Box<RsTensorI16>;
    }

    extern "Rust" {
        type RsTensorU32;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[u32];
        fn as_slice_mut(&mut self) -> &mut [u32];
        unsafe fn rsU32_copy_from_ptr(shape: &[usize], ptr: *const u32) -> Box<RsTensorU32>;
    }

    extern "Rust" {
        type RsTensorI32;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[i32];
        fn as_slice_mut(&mut self) -> &mut [i32];
        unsafe fn rsI32_copy_from_ptr(shape: &[usize], ptr: *const i32) -> Box<RsTensorI32>;
    }

    extern "Rust" {
        type RsTensorU64;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[u64];
        fn as_slice_mut(&mut self) -> &mut [u64];
        unsafe fn rsU64_copy_from_ptr(shape: &[usize], ptr: *const u64) -> Box<RsTensorU64>;
    }

    extern "Rust" {
        type RsTensorI64;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[i64];
        fn as_slice_mut(&mut self) -> &mut [i64];
        unsafe fn rsI64_copy_from_ptr(shape: &[usize], ptr: *const i64) -> Box<RsTensorI64>;
    }

    extern "Rust" {
        type RsTensorF32;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[f32];
        fn as_slice_mut(&mut self) -> &mut [f32];
        unsafe fn rsF32_copy_from_ptr(shape: &[usize], ptr: *const f32) -> Box<RsTensorF32>;
    }

    extern "Rust" {
        type RsTensorF64;
        fn shape(&self) -> &[usize];
        fn as_slice(&self) -> &[f64];
        fn as_slice_mut(&mut self) -> &mut [f64];
        unsafe fn rsF64_copy_from_ptr(shape: &[usize], ptr: *const f64) -> Box<RsTensorF64>;
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

                pub unsafe fn copy_from_ptr(shape: &[usize], ptr: *const $typ) -> Self {
                    Self(RsTensor::copy_from_ptr(shape, ptr))
                }

            }
            // Implement conversion traits to/from inner type
            impl From<RsTensor<$typ>> for [<RsTensor $typ:upper>] {
                fn from(tensor: RsTensor<$typ>) -> Self { Self(tensor) }
            }
            impl From<[<RsTensor $typ:upper>]> for RsTensor<$typ> {
                fn from(wrapped: [<RsTensor $typ:upper>]) -> Self { wrapped.0 }
            }

            // Had to make this wrapped in a box due to opaque type across FFI boundary
            // Could not make an associated function, due to lack of support in CXX
            #[allow(non_snake_case)]
            unsafe fn [<rs $typ:upper _copy_from_ptr>](shape: &[usize], ptr: *const $typ) -> Box<[<RsTensor $typ:upper>]> {
                Box::new([<RsTensor $typ:upper>]::copy_from_ptr(shape, ptr))
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

    pub fn shape(&self) -> &[usize] {
        self.0.shape()
    }

    pub fn as_slice(&self) -> &[T] {
        self.0.as_slice().unwrap()
    }

    pub fn as_slice_mut(&mut self) -> &mut [T] {
        self.0.as_slice_mut().unwrap()
    }

    pub unsafe fn copy_from_ptr(shape: &[usize], ptr: *const T) -> Self {
        let view = ndarray::ArrayViewD::from_shape_ptr(shape, ptr);
        let arr = view.to_owned();
        Self(arr)
    }
}
impl<T> From<RsTensor<T>> for ndarray::ArrayD<T> {
    fn from(tensor: RsTensor<T>) -> Self {
        tensor.0
    }
}
