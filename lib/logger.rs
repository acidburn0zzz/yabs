extern crate log;
extern crate ansi_term;

use log::{LogRecord, LogLevel, LogMetadata, LogLevelFilter};
use ansi_term::Colour;

use error::YabsError;

pub struct Logger;

impl Logger {
    pub fn init() -> Result<(), YabsError> {
        Ok(log::set_logger(|max_log_level| {
            max_log_level.set(LogLevelFilter::Info);
            Box::new(Logger)
        })?)
    }
}

impl log::Log for Logger {
    fn enabled(&self, metadata: &LogMetadata) -> bool {
        metadata.level() <= LogLevel::Info
    }

    fn log(&self, record: &LogRecord) {
        if self.enabled(record.metadata()) {
            match record.level() {
                LogLevel::Error => {
                    println!("{}: {}", Colour::Red.bold().paint("error"), record.args());
                },
                LogLevel::Warn => {

                },
                LogLevel::Info => {
                    println!("{}: {}", Colour::Green.bold().paint("yabs"), record.args());
                },
                LogLevel::Debug => {

                },
                LogLevel::Trace => {

                },
            };
        }
    }
}
