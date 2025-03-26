use std::fs;
use std::io::{self, BufRead, BufReader, Error};
use std::path::Path;

/// Struct to store file metadata and content
#[derive(Debug)]
struct FileReader {
    path: String,
    filename: String,
    size_bytes: u64,
    content: Vec<String>,
}

impl FileReader {
    /// Create a new FileReader instance with an empty content
    fn new(file_path: &str) -> Self {
        FileReader {
            path: file_path.to_string(),
            filename: Path::new(file_path)
                .file_name()
                .unwrap_or_default()
                .to_str()
                .unwrap_or_default()
                .to_string(),
            size_bytes: 0,
            content: Vec::new(),
        }
    }

    /// Read the file and populate the struct's metadata and content
    fn read_file(&mut self) -> Result<(), Error> {
        // Get file metadata
        let metadata = fs::metadata(&self.path)?;
        self.size_bytes = metadata.len();

        // Read file content line by line
        let file = fs::File::open(&self.path)?;
        let reader = BufReader::new(file);
        
        // Clear any existing content
        self.content.clear();

        // Read lines into the vector
        for line in reader.lines() {
            self.content.push(line?);
        }

        Ok(())
    }

    /// Print file metadata and content
    fn print_file_info(&self) {
        println!("File Information:");
        println!("Path: {}", self.path);
        println!("Filename: {}", self.filename);
        println!("Size: {} bytes", self.size_bytes);
        println!("Number of Lines: {}", self.content.len());
        
        println!("\nFile Contents:");
        for (index, line) in self.content.iter().enumerate() {
            println!("{}: {}", index + 1, line);
        }
    }
}

fn main() -> io::Result<()> {
    // Example usage
    let file_path = "src/main.c";
    
    // Create a new FileReader instance
    let mut file_reader = FileReader::new(file_path);
    
    // Read the file
    match file_reader.read_file() {
        Ok(_) => {
            // Print file information
            file_reader.print_file_info();
            Ok(())
        }
        Err(e) => {
            eprintln!("Error reading file: {}", e);
            Err(e)
        }
    }
}

