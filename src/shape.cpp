
#include <algorithm>
#include "shape.h"

istream &operator >>(istream &IN, Shape &shape)
{
    string this_line, this_word;
    if(getline(IN, this_line))
    {
        istringstream input_stream(this_line);
        input_stream >> shape.transID >> shape.length >> shape.rpkm;
        while(input_stream >> this_word)
        {
            if(this_word == "NULL")
                shape.shape_score.push_back(NULL_Score);
            else
                shape.shape_score.push_back( stof(this_word) );
        }
        if(shape.shape_score.size() != shape.length or shape.length == 0)
        {
            cerr << shape.shape_score.size() << "\t" << shape.length << endl;
            cerr << "Bad Shape Line: " << this_line << "\n" << "\t";
            int i = 0;
            for_each(shape.shape_score.cbegin(), shape.shape_score.cend(), [&i](float score){ cout << i++ << ": " << score << "\n"; });
            cout << endl;
            shape.transID = "";
        }
    }else{
        // do nothing
    }
    return IN;
}


void ShapeSet::read_in_shape()
{
    ifstream IN(shape_file_name, ifstream::in);
    if(IN)
    {
        while(1)
        {
            shared_ptr<Shape> new_shape(new Shape);
            IN >> *new_shape;
            if(*new_shape)
            {
                transID_list.push_back(new_shape->transID);
                shape_list[new_shape->transID] = new_shape;
                ++capacity;
            }else if(IN.fail()){
                break;
            }
        }
        sort(transID_list.begin(), transID_list.end());
    }else{
        cerr << "Bad Input File: " << shape_file_name << endl;
    }
    IN.close();
}

const Shape& ShapeSet::get_shape(const string &transID) const
{
    try{
        return *shape_list.at(transID);
    }catch(out_of_range error){
        return *shared_ptr<Shape>(new Shape);
    }
}
