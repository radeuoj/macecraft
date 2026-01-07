pub enum Block {
    Air,
    Dirt,
}

impl TryFrom<u8> for Block {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(Block::Air),
            1 => Ok(Block::Dirt),
            _ => Err(()),
        }
    }
}