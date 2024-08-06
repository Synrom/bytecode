#ifndef LOCATION_H
#define LOCATION_H

#include <cstddef>

class Location {
public:
    Location() : row(0), column(0) {}
    Location(size_t row, size_t column) : row(row), column(column) {}
    size_t row, column;
};

class Range {
public:
    Range(size_t lineno, size_t column, size_t idx) : start(lineno, column), end(lineno, column), startidx(idx), endidx(idx) {}
    /* Both ends are inclusive! */
    Location start, end;
    /* start and end idx in original string */
    size_t startidx, endidx;
};

#endif