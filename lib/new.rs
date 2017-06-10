use error::{YabsError, YabsErrorKind};

use std::fs;
use std::io::Write;
use std::path::{Path, PathBuf};

pub fn new_project(name: &String, lib: bool) -> Result<(), YabsError> {
    let dir = Path::new(name);
    if dir.exists() {
        bail!(YabsErrorKind::DirExists(dir.to_path_buf()));
    }
    fs::create_dir_all(dir)?;
    fs::create_dir_all(dir.join("src"))?;
    match lib {
        true => create_bin_files(dir.to_path_buf(), name)?,
        false => (),
    };
    Ok(create_bin_files(dir.to_path_buf(), name)?)
}

pub fn create_bin_files(path: PathBuf, name: &String) -> Result<(), YabsError> {
    fs::File::create(path.join(format!("{}.toml", name)))
        .and_then(|mut file| {
            Ok(file.write_all(format!("[project]\n\
                name = \"{0}\"\n\
                version = \"0.0.1\"\n\
                target = [\"{0}\"]\n\
                file-extensions = [\"cpp\"]\n\
                compiler = \"g++\"\n\
                compiler-flags = []\n\
                src = [\n\
                \t\"src/main.cpp\"\n\
                ]\n\
                libraries = []\n\
                library-directories = []\n\
                include = []\n\
                linker-flags = []\n\
                ignore = []\n\
                before-script = []\n\
                after-script = []\n\n\
                [[bin]]\n\
                name = \"{0}\"\n\
                path = \"./src/main.cpp\"
                ",
                                      name)
                                  .as_bytes())?)
        })?;
    fs::File::create(path.join("src/main.cpp"))
        .and_then(|mut src| {
                      Ok(src.write_all(b"#include <iostream>\n\n\
            int main() {\n\
            \treturn 0;\n\
            }")?)
                  })?;
    Ok(())
}
