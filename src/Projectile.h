#pragma once

struct Projectile {
    Projectile(Tuple pos, Tuple vel) : position(pos), velocity(vel){};
    Tuple position;
    Tuple velocity;
};