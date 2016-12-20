// Generate documentation for this project
#[derive(Debug,Default,Serialize, Deserialize,Clone,PartialEq)]
pub struct DocDesc {
    doc: Option<Vec<String>>,
}

impl DocDesc {
    pub fn gen_make(&self) -> String {
        let mut doc_str = String::from("doc:\n");
        if let Some(doc) = self.doc.clone() {
            for line in doc {
                doc_str.push_str(&format!("\t{}\n", &line));
            }
        }
        return doc_str;
    }
}
