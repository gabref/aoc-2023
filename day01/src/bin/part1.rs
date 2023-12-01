use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

fn main() {
    let filename = "input/big.txt";
    let mut sum: u32 = 0;
    if let Ok(lines) = read_lines(filename) {
        // Consumes the iterator, returns an (Optional) String
        for line_r in lines {
            if let Ok(line) = line_r {
                let mut res = (line
                    .as_bytes()
                    .get(line.find(|c: char| c.is_ascii_digit()).unwrap_or(0))
                    .unwrap_or(&48)
                    - 48)
                    * 10;
                res += line
                    .as_bytes()
                    .get(line.rfind(|c: char| c.is_ascii_digit()).unwrap_or(0))
                    .unwrap_or(&48)
                    - 48;
                sum += res as u32;
                // println!("{} - {}", line, res);
            }
        }
        println!("sum: {sum}");
    } else {
        println!("could not read the file {filename}");
    }
}

// The output is wrapped in a Result to allow matching on errors
// Returns an Iterator to the Reader of the lines of the file.
fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where
    P: AsRef<Path>,
{
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

