use crate::math::i32_to_f32;

#[derive(Clone, Copy, Debug, Default)]
pub struct MinMax {
    pub min: f32,
    pub max: f32,
    pub has_data: bool,
}

pub fn range_min_max_i32(samples: &[i32], start: usize, end: usize) -> Option<(f32, f32)> {
    let end = end.min(samples.len());
    if start >= end {
        return None;
    }

    let mut min_y = f32::INFINITY;
    let mut max_y = f32::NEG_INFINITY;

    for &sample in &samples[start..end] {
        let value = i32_to_f32(sample);
        min_y = min_y.min(value);
        max_y = max_y.max(value);
    }

    if !min_y.is_finite() || !max_y.is_finite() {
        return None;
    }

    Some((min_y, max_y))
}

pub fn bucket_min_max_i32(
    samples: &[i32],
    start: usize,
    end: usize,
    buckets: usize,
) -> Vec<MinMax> {
    if buckets == 0 {
        return Vec::new();
    }

    let end = end.min(samples.len());
    if start >= end {
        return vec![MinMax::default(); buckets];
    }

    let len = end - start;
    let mut out = Vec::with_capacity(buckets);

    for bucket in 0..buckets {
        let bucket_start = start + (len * bucket) / buckets;
        let bucket_end = start + (len * (bucket + 1)) / buckets;

        if bucket_start >= bucket_end {
            out.push(MinMax::default());
            continue;
        }

        let mut min_y = f32::INFINITY;
        let mut max_y = f32::NEG_INFINITY;

        for &sample in &samples[bucket_start..bucket_end] {
            let value = i32_to_f32(sample);
            min_y = min_y.min(value);
            max_y = max_y.max(value);
        }

        if !min_y.is_finite() || !max_y.is_finite() {
            out.push(MinMax::default());
        } else {
            out.push(MinMax {
                min: min_y,
                max: max_y,
                has_data: true,
            });
        }
    }

    out
}
