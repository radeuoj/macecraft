use crate::player::Player;

pub struct AABB {
    pub position: glam::Vec3,
    pub size: glam::Vec3,
}

impl AABB {
    pub fn from_block(position: glam::IVec3) -> Self {
        Self {
            position: position.as_vec3(),
            size: glam::Vec3::ONE,
        }
    }

    pub fn from_player(position: glam::Vec3) -> Self {
        Self {
            position: position - glam::vec3(Player::WIDTH / 2.0, 0.0, Player::WIDTH / 2.0),
            size: glam::vec3(Player::WIDTH, Player::HEIGHT, Player::WIDTH),
        }
    }

    pub fn collision(a: &AABB, b: &AABB) -> bool {
        a.position.x < b.position.x + b.size.x && a.position.x + a.size.x > b.position.x &&
        a.position.y < b.position.y + b.size.y && a.position.y + a.size.y > b.position.y &&
        a.position.z < b.position.z + b.size.z && a.position.z + a.size.z > b.position.z
    }
}