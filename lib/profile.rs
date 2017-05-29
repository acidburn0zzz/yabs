extern crate serde;

use desc::project::*;
use desc::install::*;
use desc::doc::*;

// Profile has descriptions that describe build instructions (proj_desc),
// install instructions (inst_desc), and documentation instructions (doc_desc).
#[derive(Debug, Serialize, Deserialize, Default, Clone, PartialEq)]
pub struct Profile {
    pub proj_desc: ProjectDesc,
}
