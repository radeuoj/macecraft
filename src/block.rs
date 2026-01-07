#[derive(Debug, Copy, Clone)]
pub enum Block {
    Air,
    Dirt,
}

impl From<u8> for Block {
    fn from(value: u8) -> Self {
        use Block::*;

        match value {
            0 => Air,
            1 => Dirt,
            _ => unreachable!(),
        }
    }
}