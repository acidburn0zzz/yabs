#![feature(env,io,core)]

mod gen;
use std::env;
use gen::Gen;

fn main() {
    let mut generator: Gen = Gen {file_list: Vec::new()};
    generator.recur_walk(&env::current_dir().unwrap());
    generator.print_file_list();
}
