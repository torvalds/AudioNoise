use std::io;
use std::path::PathBuf;
use std::time::Duration;

use _core::{math, raw::RawAudioFile, waveform};
use clap::Parser;
use crossterm::{
    event::{self, Event, KeyCode, KeyEventKind},
    execute,
    terminal::{EnterAlternateScreen, LeaveAlternateScreen, disable_raw_mode, enable_raw_mode},
};
use ratatui::prelude::Frame;
use ratatui::{
    Terminal,
    backend::CrosstermBackend,
    buffer::Buffer,
    layout::{Constraint, Direction, Layout, Rect},
    style::{Color, Modifier, Style},
    text::{Line, Span, Text},
    widgets::{Block, Borders, Paragraph, Widget},
};

// --- Constants ---
const INITIAL_WINDOW_SEC: f64 = 2.0;
const DEFAULT_MAX_WIDTH_SEC: f64 = 2.0;
const DEFAULT_MIN_ZOOM_SAMPLES: usize = 100;

#[derive(Parser, Debug)]
#[command(
    name = "audionoise-tui",
    about = "Terminal waveform viewer for int32 raw audio"
)]
struct Args {
    #[arg(value_name = "FILE", required = true)]
    files: Vec<PathBuf>,
    #[arg(long, default_value_t = 48000)]
    rate: u32,
    #[arg(long, default_value_t = DEFAULT_MIN_ZOOM_SAMPLES)]
    min_zoom_samples: usize,
    #[arg(long, default_value_t = DEFAULT_MAX_WIDTH_SEC)]
    max_width_sec: f64,
}

struct App {
    files: Vec<RawAudioFile>,
    styles: Vec<Style>,
    rate: u32,
    min_width_sec: f64,
    max_width_sec: f64,
    duration_sec: f64,
    start_time: f64,
    window_width: f64,
}

impl App {
    fn new(args: Args) -> io::Result<Self> {
        if args.rate == 0 {
            return Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                "sample rate must be > 0",
            ));
        }

        let mut files = Vec::new();
        let mut max_samples = 0usize;

        // Load files
        for path in &args.files {
            match RawAudioFile::open(path) {
                Ok(file) => {
                    max_samples = max_samples.max(file.len_samples());
                    files.push(file);
                }
                Err(err) => {
                    eprintln!("Skipping {}: {}", path.display(), err);
                }
            }
        }

        if files.is_empty() {
            return Err(io::Error::new(
                io::ErrorKind::NotFound,
                "no readable audio files",
            ));
        }

        // Derived view limits
        let duration_sec = max_samples as f64 / args.rate as f64;
        let max_width_sec = args.max_width_sec.min(duration_sec.max(0.0));
        let min_width_sec = (args.min_zoom_samples as f64 / args.rate as f64).min(max_width_sec);

        let window_width = if max_width_sec == 0.0 {
            0.0
        } else {
            INITIAL_WINDOW_SEC.clamp(min_width_sec, max_width_sec)
        };

        let styles = build_styles(files.len());

        Ok(Self {
            files,
            styles,
            rate: args.rate,
            min_width_sec,
            max_width_sec,
            duration_sec,
            start_time: 0.0,
            window_width,
        })
    }

    fn view_samples(&self) -> (usize, usize) {
        let start = (self.start_time * self.rate as f64).floor().max(0.0) as usize;
        let end = ((self.start_time + self.window_width) * self.rate as f64).ceil() as usize;
        (start, end)
    }

    fn max_start_time(&self) -> f64 {
        (self.duration_sec - self.window_width).max(0.0)
    }

    fn pan_fraction(&mut self, fraction: f64) {
        let delta = self.window_width * fraction;
        self.start_time = (self.start_time + delta).clamp(0.0, self.max_start_time());
    }

    fn zoom(&mut self, factor: f64) {
        let center = self.start_time + self.window_width / 2.0;
        let min_width = self.min_width_sec.min(self.max_width_sec);
        let max_width = self.max_width_sec.max(min_width);
        let new_width = (self.window_width * factor).clamp(min_width, max_width);
        let max_start = (self.duration_sec - new_width).max(0.0);
        let new_start = (center - new_width / 2.0).clamp(0.0, max_start);

        self.window_width = new_width;
        self.start_time = new_start;
    }

    fn jump_start(&mut self) {
        self.start_time = 0.0;
    }

    fn jump_end(&mut self) {
        self.start_time = self.max_start_time();
    }

    fn y_range(&self) -> (f32, f32) {
        // Scan the visible window to auto-scale the Y axis.
        let (start, end) = self.view_samples();
        let mut min_y = f32::INFINITY;
        let mut max_y = f32::NEG_INFINITY;

        for file in &self.files {
            if let Some((local_min, local_max)) =
                waveform::range_min_max_i32(file.samples(), start, end)
            {
                min_y = min_y.min(local_min);
                max_y = max_y.max(local_max);
            }
        }

        math::autoscale_symmetric(min_y, max_y)
    }
}

struct Waveform<'a> {
    files: &'a [RawAudioFile],
    styles: &'a [Style],
    rate: u32,
    start_time: f64,
    window_width: f64,
    y_min: f32,
    y_max: f32,
}

impl<'a> Waveform<'a> {
    fn new(app: &'a App, y_min: f32, y_max: f32) -> Self {
        Self {
            files: &app.files,
            styles: &app.styles,
            rate: app.rate,
            start_time: app.start_time,
            window_width: app.window_width,
            y_min,
            y_max,
        }
    }
}

impl Widget for Waveform<'_> {
    fn render(self, area: Rect, buf: &mut Buffer) {
        if area.width == 0 || area.height == 0 {
            return;
        }

        let cols = area.width as usize;
        let rows = area.height as usize;

        // Clear the drawing area.
        for y in 0..rows {
            for x in 0..cols {
                if let Some(cell) = buf.cell_mut((area.x + x as u16, area.y + y as u16)) {
                    cell.set_char(' ');
                }
            }
        }

        // Convert the visible time window into sample indices.
        let start_sample = (self.start_time * self.rate as f64).floor().max(0.0) as usize;
        let end_sample = ((self.start_time + self.window_width) * self.rate as f64).ceil() as usize;

        // Draw a horizontal zero line when it intersects the view.
        if self.y_min < 0.0 && self.y_max > 0.0 {
            let zero_row = y_to_row(0.0, self.y_min, self.y_max, rows);
            for col in 0..cols {
                if let Some(cell) = buf.cell_mut((area.x + col as u16, area.y + zero_row as u16)) {
                    cell.set_char('-');
                    cell.set_style(Style::default().fg(Color::DarkGray));
                }
            }
        }

        // Per column, collapse samples to min/max for a stable waveform view.
        for (idx, file) in self.files.iter().enumerate() {
            let style = self.styles.get(idx).copied().unwrap_or_default();
            let samples = file.samples();

            if start_sample >= samples.len() {
                continue;
            }

            let file_end = end_sample.min(samples.len());
            let buckets = waveform::bucket_min_max_i32(samples, start_sample, file_end, cols);

            for (col, bucket) in buckets.into_iter().enumerate() {
                if !bucket.has_data {
                    continue;
                }

                let top = y_to_row(bucket.max, self.y_min, self.y_max, rows);
                let bottom = y_to_row(bucket.min, self.y_min, self.y_max, rows);
                let (top, bottom) = if top <= bottom {
                    (top, bottom)
                } else {
                    (bottom, top)
                };
                let x = area.x + col as u16;

                for row in top..=bottom {
                    if let Some(cell) = buf.cell_mut((x, area.y + row as u16)) {
                        cell.set_char('|');
                        cell.set_style(style);
                    }
                }
            }
        }
    }
}

fn y_to_row(y: f32, y_min: f32, y_max: f32, rows: usize) -> usize {
    if rows == 0 || y_max <= y_min {
        return 0;
    }
    let t = ((y - y_min) / (y_max - y_min)).clamp(0.0, 1.0);
    let row = ((1.0 - t) * (rows as f32 - 1.0)).round() as usize;
    row.min(rows - 1)
}

fn build_styles(count: usize) -> Vec<Style> {
    let palette = [
        Color::Cyan,
        Color::Yellow,
        Color::Green,
        Color::Magenta,
        Color::Red,
        Color::Blue,
    ];
    (0..count)
        .map(|idx| Style::default().fg(palette[idx % palette.len()]))
        .collect()
}

fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync + 'static>> {
    let args = Args::parse();
    let app = App::new(args)?;

    enable_raw_mode()?;
    let mut stdout = io::stdout();
    execute!(stdout, EnterAlternateScreen)?;

    let backend = CrosstermBackend::new(stdout);
    let mut terminal = Terminal::new(backend)?;

    let res = run(&mut terminal, app);

    disable_raw_mode()?;
    execute!(terminal.backend_mut(), LeaveAlternateScreen)?;
    terminal
        .show_cursor()
        .map_err(|e| io::Error::new(io::ErrorKind::Other, e.to_string()))?;

    res
}

fn run<B: ratatui::prelude::Backend>(
    terminal: &mut Terminal<B>,
    mut app: App,
) -> Result<(), Box<dyn std::error::Error + Send + Sync + 'static>> {
    loop {
        terminal
            .draw(|frame| ui(frame, &app))
            .map_err(|e| io::Error::new(io::ErrorKind::Other, e.to_string()))?;

        // Poll for input with a short timeout to keep the UI responsive.
        if event::poll(Duration::from_millis(50))? {
            if let Event::Key(key) = event::read()? {
                if key.kind != KeyEventKind::Press {
                    continue;
                }
                match key.code {
                    KeyCode::Char('q') => break,                                   // Quit
                    KeyCode::Left | KeyCode::Char('h') => app.pan_fraction(-0.25), // Pan left
                    KeyCode::Right | KeyCode::Char('l') => app.pan_fraction(0.25), // Pan right
                    KeyCode::PageDown | KeyCode::Char('z') => app.zoom(0.5), // Zoom in (0.5x width)
                    KeyCode::PageUp | KeyCode::Char('x') => app.zoom(2.0),   // Zoom out (2x width)
                    KeyCode::Char('+') => app.zoom(0.8),                     // Fine zoom in
                    KeyCode::Char('-') => app.zoom(1.25),                    // Fine zoom out
                    KeyCode::Home | KeyCode::Char('g') => app.jump_start(),  // Jump to start
                    KeyCode::End | KeyCode::Char('G') => app.jump_end(),     // Jump to end
                    _ => {}
                }
            }
        }
    }

    Ok(())
}

fn ui(frame: &mut Frame, app: &App) {
    // Layout: status (1 row), waveform (flex), footer (2 rows).
    let layout = Layout::default()
        .direction(Direction::Vertical)
        .constraints([
            Constraint::Length(1),
            Constraint::Min(5),
            Constraint::Length(2),
        ])
        .split(frame.area());

    render_status(frame, layout[0], app);
    render_waveform(frame, layout[1], app);
    render_footer(frame, layout[2], app);
}

fn render_status(frame: &mut Frame, area: Rect, app: &App) {
    let mut spans = Vec::new();
    spans.push(Span::styled(
        "AudioNoise TUI",
        Style::default().add_modifier(Modifier::BOLD),
    ));
    spans.push(Span::raw("  "));
    spans.push(Span::raw(format!("{} Hz", app.rate)));
    spans.push(Span::raw("  "));
    spans.push(Span::raw(format!(
        "{:.2}-{:.2}s / {:.2}s",
        app.start_time,
        app.start_time + app.window_width,
        app.duration_sec
    )));
    spans.push(Span::raw("  "));
    spans.push(Span::raw("files: "));

    for (idx, file) in app.files.iter().enumerate() {
        if idx > 0 {
            spans.push(Span::raw(", "));
        }
        let style = app.styles.get(idx).copied().unwrap_or_default();
        spans.push(Span::styled(file.name(), style));
    }

    let line = Line::from(spans);
    let paragraph = Paragraph::new(line);
    frame.render_widget(paragraph, area);
}

fn render_waveform(frame: &mut Frame, area: Rect, app: &App) {
    let block = Block::default().title("Waveform").borders(Borders::ALL);
    let inner = block.inner(area);
    frame.render_widget(block, area);

    let (y_min, y_max) = app.y_range();
    frame.render_widget(Waveform::new(app, y_min, y_max), inner);
}

fn render_footer(frame: &mut Frame, area: Rect, app: &App) {
    let slider = slider_line(
        area.width as usize,
        app.start_time,
        app.window_width,
        app.duration_sec,
    );
    // Footer: slider line and help text.
    let help = "q quit | <-/h pan | ->/l pan | PgUp/z zoom in | PgDn/x zoom out | +/- fine zoom | g/G start/end";
    let text = Text::from(vec![Line::from(slider), Line::from(help)]);
    let paragraph = Paragraph::new(text);
    frame.render_widget(paragraph, area);
}

fn slider_line(width: usize, start: f64, window: f64, duration: f64) -> String {
    if width == 0 {
        return String::new();
    }
    if duration <= 0.0 {
        return "no data".to_string();
    }

    // Render an ASCII slider showing the visible window within the full duration.
    let mut chars = vec!['-'; width];
    let end = (start + window).min(duration);
    let max_idx = (width - 1) as f64;
    let start_idx = ((start / duration) * max_idx).round() as usize;
    let end_idx = ((end / duration) * max_idx).round() as usize;

    let (left, right) = if start_idx <= end_idx {
        (start_idx, end_idx)
    } else {
        (end_idx, start_idx)
    };

    for idx in left..=right {
        chars[idx] = '=';
    }
    chars[left] = '|';
    chars[right] = '|';

    chars.into_iter().collect()
}
