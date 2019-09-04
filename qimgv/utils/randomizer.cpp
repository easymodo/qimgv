#include "randomizer.h"

Randomizer::Randomizer() {
    setCount(0);
}

Randomizer::Randomizer(int _count) : currentIndex(0) {
    setCount(_count);
}

void Randomizer::setCount(int _count) {
    vec.resize(_count);
    fill();
}

void Randomizer::shuffle() {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::shuffle(vec.begin(), vec.end(), rng);
}

void Randomizer::setCurrent(int _current) {
    currentIndex = indexOf(_current);
}

// this assumes our array contains shuffled int values [0 ... count]
// which it does in our case
int Randomizer::indexOf(int item) {
    int index = -1;
    if(item >= vec.size())
        return index;
    std::vector<int>::iterator it;
    int i = 0;
    for (it = vec.begin(); it != vec.end(); ++it, ++i) {
        if(vec[i] == item) {
            index = i;
            break;
        }
    }
    return index;
}

void Randomizer::fill() {
    std::vector<int>::iterator it;
    int i = 0;
    for (it = vec.begin(); it != vec.end(); ++it, ++i) {
        *it = i;
    }
}

void Randomizer::print() {
    qDebug() << "---vector---";
    std::vector<int>::iterator it;
    for (it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << std::endl;
    }
    qDebug() << "----end----";
}

int Randomizer::next() {
    // re-shuffle when needed
    // because vector gets rearranged this will break prev()
    while(currentIndex == vec.size() - 1) {
        int currentItem = vec[currentIndex];
        shuffle();
        setCurrent(currentItem);
    }
    currentIndex++;
    return vec[currentIndex];
}

int Randomizer::prev() {
    while(currentIndex == 0) {
        int currentItem = vec[currentIndex];
        shuffle();
        setCurrent(currentItem);
    }
    currentIndex--;
    return vec[currentIndex];
}
