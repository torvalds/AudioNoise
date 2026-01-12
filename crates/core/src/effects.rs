use crate::math::clip;

pub trait Effect: Send {
    fn process(&mut self, input: f32) -> f32;
}

#[derive(Debug, Clone, Copy)]
pub struct Gain {
    gain: f32,
}

impl Gain {
    pub fn new(gain: f32) -> Self {
        Self { gain }
    }

    pub fn gain(&self) -> f32 {
        self.gain
    }

    pub fn set_gain(&mut self, gain: f32) {
        self.gain = gain;
    }
}

impl Effect for Gain {
    fn process(&mut self, input: f32) -> f32 {
        input * self.gain
    }
}

#[derive(Debug, Clone, Copy)]
pub struct Clip {
    min: f32,
    max: f32,
}

impl Clip {
    pub fn new(min: f32, max: f32) -> Self {
        Self { min, max }
    }

    pub fn set_limits(&mut self, min: f32, max: f32) {
        self.min = min;
        self.max = max;
    }
}

impl Effect for Clip {
    fn process(&mut self, input: f32) -> f32 {
        clip(input, self.min, self.max)
    }
}

pub struct Chain {
    effects: Vec<Box<dyn Effect>>,
}

impl Chain {
    pub fn new() -> Self {
        Self {
            effects: Vec::new(),
        }
    }

    pub fn push<E: Effect + 'static>(&mut self, effect: E) {
        self.effects.push(Box::new(effect));
    }

    pub fn clear(&mut self) {
        self.effects.clear();
    }

    pub fn is_empty(&self) -> bool {
        self.effects.is_empty()
    }

    pub fn len(&self) -> usize {
        self.effects.len()
    }

    pub fn process(&mut self, mut input: f32) -> f32 {
        for effect in &mut self.effects {
            input = effect.process(input);
        }
        input
    }

    pub fn process_buffer(&mut self, buffer: &mut [f32]) {
        for sample in buffer {
            *sample = self.process(*sample);
        }
    }
}

impl Default for Chain {
    fn default() -> Self {
        Self::new()
    }
}
