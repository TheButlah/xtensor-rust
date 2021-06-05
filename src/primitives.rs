use crate::DType;

pub trait TensorPrimitive {
    const DTYPE: DType;
}
impl TensorPrimitive for u8 {
    const DTYPE: DType = DType::UInt8;
}
impl TensorPrimitive for i8 {
    const DTYPE: DType = DType::Int8;
}
impl TensorPrimitive for f32 {
    const DTYPE: DType = DType::Float32;
}
impl TensorPrimitive for f64 {
    const DTYPE: DType = DType::Float64;
}
