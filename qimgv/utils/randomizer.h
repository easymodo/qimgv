#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include <QDebug>
#include <QString>

class Randomizer {
public:
    Randomizer();
    Randomizer(int _count);

    void setCount(int _count);
    int next();
    int prev();

    void shuffle();
    void print();
    void setCurrent(int _current);
private:
    int currentIndex;
    std::vector<int> vec;
    void fill();
    int indexOf(int n);
};
