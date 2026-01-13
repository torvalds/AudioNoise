use std::f32;

pub const I32_SCALE: f32 = 1.0 / 2147483648.0;

pub fn i32_to_f32(sample: i32) -> f32 {
    sample as f32 * I32_SCALE
}

pub fn clip(value: f32, min: f32, max: f32) -> f32 {
    value.clamp(min, max)
}

pub fn peak(samples: &[f32]) -> f32 {
    samples.iter().map(|value| value.abs()).fold(0.0, f32::max)
}

pub fn rms(samples: &[f32]) -> f32 {
    if samples.is_empty() {
        return 0.0;
    }

    let sum = samples.iter().map(|value| value * value).sum::<f32>();
    (sum / samples.len() as f32).sqrt()
}

pub fn dbfs(value: f32) -> f32 {
    let value = value.abs();
    if value <= 0.0 {
        return f32::NEG_INFINITY;
    }
    20.0 * value.log10()
}

pub fn normalize_to_peak(samples: &mut [f32], target_peak: f32) -> f32 {
    if samples.is_empty() || target_peak <= 0.0 {
        return 0.0;
    }

    let current_peak = peak(samples);
    if current_peak <= 0.0 {
        return 0.0;
    }

    let gain = target_peak / current_peak;
    for sample in samples {
        *sample *= gain;
    }
    gain
}

pub fn autoscale_symmetric(min_y: f32, max_y: f32) -> (f32, f32) {
    if !min_y.is_finite() || !max_y.is_finite() {
        return (-1.0, 1.0);
    }

    let mut max_val = min_y.abs().max(max_y.abs());
    if max_val < 1e-6 {
        max_val = 0.01;
    } else {
        max_val *= 1.05;
    }

    (-max_val, max_val)
}

pub fn windowed_rms(samples: &[f32], window: usize) -> Vec<f32> {
    if window == 0 {
        return Vec::new();
    }

    samples.chunks(window).map(|chunk| rms(chunk)).collect()
}

pub fn windowed_peak(samples: &[f32], window: usize) -> Vec<f32> {
    if window == 0 {
        return Vec::new();
    }

    samples.chunks(window).map(|chunk| peak(chunk)).collect()
}
