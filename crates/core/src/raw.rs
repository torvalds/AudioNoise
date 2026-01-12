use std::fs::File;
use std::io;
use std::path::{Path, PathBuf};

use bytemuck::cast_slice;
use memmap2::{Mmap, MmapOptions};

pub const BYTES_PER_SAMPLE: usize = 4;

pub struct RawAudioFile {
    path: PathBuf,
    name: String,
    mmap: Mmap,
    samples: usize,
}

impl RawAudioFile {
    pub fn open(path: &Path) -> io::Result<Self> {
        let file = File::open(path)?;
        let mmap = unsafe { MmapOptions::new().map(&file)? };

        if mmap.len() % BYTES_PER_SAMPLE != 0 {
            return Err(io::Error::new(
                io::ErrorKind::InvalidData,
                format!("file size is not aligned to {BYTES_PER_SAMPLE} bytes"),
            ));
        }

        let samples = mmap.len() / BYTES_PER_SAMPLE;
        if samples == 0 {
            return Err(io::Error::new(
                io::ErrorKind::InvalidData,
                "empty or short file",
            ));
        }

        let name = path
            .file_name()
            .map(|name| name.to_string_lossy().to_string())
            .unwrap_or_else(|| path.display().to_string());

        Ok(Self {
            path: path.to_path_buf(),
            name,
            mmap,
            samples,
        })
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn path(&self) -> &Path {
        &self.path
    }

    pub fn len_samples(&self) -> usize {
        self.samples
    }

    pub fn samples(&self) -> &[i32] {
        let bytes = &self.mmap[..self.samples * BYTES_PER_SAMPLE];
        cast_slice(bytes)
    }

    pub fn duration_sec(&self, rate: u32) -> f64 {
        if rate == 0 {
            return 0.0;
        }
        self.samples as f64 / rate as f64
    }
}
