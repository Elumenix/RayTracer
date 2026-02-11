#pragma once

struct Environment {
    Environment(Tuple gravityVec, Tuple windVec) : gravity(gravityVec), wind(windVec) {};
    Tuple gravity;
    Tuple wind;
};