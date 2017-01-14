extern crate serde;

// Descibe how to install this project
#[derive(Debug,Default,Serialize, Deserialize,Clone,PartialEq)]
pub struct InstallDesc {
    prefix: Option<String>,
}
