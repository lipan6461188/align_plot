#ifndef SHAPE_H
#define SHAPE_H

#include "include_head.h"


struct Shape
{
    string transID;
    uLONG length = 0;
    double rpkm = 0.0;
    vector<float> shape_score;

    friend istream &operator >>(istream &, Shape &);
    operator bool(){ return transID.empty() ? false : true; }
};

class ShapeSet
{
public:
    ShapeSet(const string &shape_file_name):shape_file_name(shape_file_name){ read_in_shape(); }
    uINT size() const { return capacity; }
    const vector<string>& keys() const { return transID_list; }
    const Shape& get_shape(const string &transID) const;
    bool has(const string &transID){ return shape_list.find(transID) != shape_list.end(); }

private:
    uINT capacity = 0;
    string shape_file_name;

    unordered_map<string, shared_ptr<Shape>> shape_list;
    vector<string> transID_list;

    void read_in_shape();
};


#endif // SHAPE_H
