use std::io;
use std::time::Duration;

use crossterm::{
    event::{self, Event, KeyCode},
    execute,
    terminal::{EnterAlternateScreen, LeaveAlternateScreen, disable_raw_mode, enable_raw_mode},
};
use ratatui::{
    Terminal,
    backend::CrosstermBackend,
    prelude::Backend,
    widgets::{Block, Borders, Paragraph},
};

fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync + 'static>> {
    enable_raw_mode()?;
    let mut stdout = io::stdout();
    execute!(stdout, EnterAlternateScreen)?;

    let backend = CrosstermBackend::new(stdout);
    let mut terminal = Terminal::new(backend)?;

    let res = run(&mut terminal);

    disable_raw_mode()?;
    execute!(terminal.backend_mut(), LeaveAlternateScreen)?;
    terminal
        .show_cursor()
        .map_err(|e| io::Error::new(io::ErrorKind::Other, e.to_string()))?;

    res
}

fn run<B: Backend>(
    terminal: &mut Terminal<B>,
) -> Result<(), Box<dyn std::error::Error + Send + Sync + 'static>> {
    loop {
        terminal
            .draw(|frame| {
                let size = frame.area();
                let ui = Paragraph::new("Press q to quit.").block(
                    Block::default()
                        .title("AudioNoise TUI (ratatui)")
                        .borders(Borders::ALL),
                );
                frame.render_widget(ui, size);
            })
            .map_err(|e| io::Error::new(io::ErrorKind::Other, e.to_string()))?;

        if event::poll(Duration::from_millis(50))? {
            if let Event::Key(key) = event::read()? {
                if matches!(key.code, KeyCode::Char('q')) {
                    break;
                }
            }
        }
    }

    Ok(())
}
