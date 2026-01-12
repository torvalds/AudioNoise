use numpy::PyReadonlyArray1;
use pyo3::prelude::*;

use crate::math;
use crate::waveform;

#[pyfunction]
fn dbfs(value: f32) -> f32 {
    math::dbfs(value)
}

#[pyfunction]
fn rms(samples: Vec<f32>) -> f32 {
    math::rms(&samples)
}

#[pyfunction]
fn peak(samples: Vec<f32>) -> f32 {
    math::peak(&samples)
}

#[pyfunction]
fn windowed_rms(samples: Vec<f32>, window: usize) -> Vec<f32> {
    math::windowed_rms(&samples, window)
}

#[pyfunction]
fn windowed_peak(samples: Vec<f32>, window: usize) -> Vec<f32> {
    math::windowed_peak(&samples, window)
}

#[pyfunction]
fn autoscale_symmetric(min_y: f32, max_y: f32) -> (f32, f32) {
    math::autoscale_symmetric(min_y, max_y)
}

#[pyfunction]
fn bucket_min_max_i32(samples: Vec<i32>, buckets: usize) -> Vec<(f32, f32, bool)> {
    waveform::bucket_min_max_i32(&samples, 0, samples.len(), buckets)
        .into_iter()
        .map(|bucket| (bucket.min, bucket.max, bucket.has_data))
        .collect()
}

#[pyfunction]
fn bucket_min_max_i32_np(
    samples: PyReadonlyArray1<i32>,
    start: usize,
    end: usize,
    buckets: usize,
) -> PyResult<Vec<(f32, f32, bool)>> {
    let slice = samples.as_slice()?;
    Ok(waveform::bucket_min_max_i32(slice, start, end, buckets)
        .into_iter()
        .map(|bucket| (bucket.min, bucket.max, bucket.has_data))
        .collect())
}

/// A Python module implemented in Rust. The name of this module must match
/// the `lib.name` setting in the `Cargo.toml`.
#[pymodule]
fn _core(m: &Bound<'_, PyModule>) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(dbfs, m)?)?;
    m.add_function(wrap_pyfunction!(rms, m)?)?;
    m.add_function(wrap_pyfunction!(peak, m)?)?;
    m.add_function(wrap_pyfunction!(windowed_rms, m)?)?;
    m.add_function(wrap_pyfunction!(windowed_peak, m)?)?;
    m.add_function(wrap_pyfunction!(autoscale_symmetric, m)?)?;
    m.add_function(wrap_pyfunction!(bucket_min_max_i32, m)?)?;
    m.add_function(wrap_pyfunction!(bucket_min_max_i32_np, m)?)?;
    Ok(())
}
