#pragma once

struct Projectile {
    Projectile(Point pos, Vector vel) : position(pos), velocity(vel){};
    Point position;
    Vector velocity;
};