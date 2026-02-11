class Tuple {
public:
    Tuple(float x, float y, float z, float w);
    ~Tuple() = default;
    float x;
    float y;
    float z;
    float w;
};

Tuple Point(float x, float y, float z);
Tuple Vector(float x, float y, float z);

bool operator==(const Tuple lhs, const Tuple rhs);
/*Tuple operator+(const Tuple lhs, const Tuple rhs);
Tuple operator-(const Tuple lhs, const Tuple rhs);
Tuple operator-(const Tuple neg);
Tuple operator*(const Tuple t, float scalar);
Tuple operator/(const Tuple t, float div);*/