extern crate serde_codegen;

use std::env;
use std::path::Path;

fn main() {
    let out_dir = env::var_os("OUT_DIR").unwrap();

    let project_desc_src = Path::new("lib/desc/project.in.rs");
    let project_desc_dst = Path::new(&out_dir).join("project.rs");

    let doc_desc_src = Path::new("lib/desc/doc.in.rs");
    let doc_desc_dst = Path::new(&out_dir).join("doc.rs");

    let install_desc_src = Path::new("lib/desc/install.in.rs");
    let install_desc_dst = Path::new(&out_dir).join("install.rs");

    let profile_src = Path::new("lib/profile.in.rs");
    let profile_dst = Path::new(&out_dir).join("profile.rs");

    serde_codegen::expand(&project_desc_src, &project_desc_dst).unwrap();
    serde_codegen::expand(&doc_desc_src, &doc_desc_dst).unwrap();
    serde_codegen::expand(&install_desc_src, &install_desc_dst).unwrap();
    serde_codegen::expand(&profile_src, &profile_dst).unwrap();
}
