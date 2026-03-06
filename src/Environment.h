#pragma once

struct Environment {
    Environment(Vector gravityVec, Vector windVec) : gravity(gravityVec), wind(windVec) {};
    Vector gravity;
    Vector wind;
};