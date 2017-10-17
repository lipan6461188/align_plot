
#include <QApplication>
#include "shape.h"
#include "align.h"
#include "align_plot.h"
#include <string_split.h>
#include <QDebug>

#define VERSION "1.0.0"
#define DATE "2017-10-17"
#define AUTHOR "Li Pan"

using namespace std;

const vector<string> color_base = { "#4C72B0", "#55A868", "#C44E52", "#8172B2", "#CCB974", "#64B5CD" };

struct Param
{
    plot_param* plot_p;
    
    /* align_file/shape_file/out_pdf_file */
    vector<string> file_list;
};


void print_usage()
{
    cout << "\nalign_plot: a programe to plot icSHAPE score of mutiple aligned species...\n";
    cout << "==========================================================================\n";
    cout << "align_plot [options] align_file shape_file out_pdf_file" << "\n\n";
    cout << "options:\n";
    cout << "\t --baseColor <string>: A:#4C72B0/T:#55A868/C:#C44E52/G:#8172B2/-:#64B5CD\n";
    cout << "\t --shapeColor <string>: KU501215.1:#55a868/AY632535.2:#c44e52\n\n";
    cout << "\t \e[1mVersion\e[0m: " << VERSION << "\n";
    cout << "\t \e[1mVersion\e[0m: " << DATE << "\n";
    cout << "\t \e[1mAUTHOR\e[0m: " << AUTHOR << "\n";
    cout << endl;
}

void parse_params(int argc, char *argv[], Param &param)
{
    // default
    param.plot_p->base_pens['A'] = QPen(QColor("#4C72B0"));
    param.plot_p->base_pens['T'] = QPen(QColor("#55A868"));
    param.plot_p->base_pens['C'] = QPen(QColor("#C44E52"));
    param.plot_p->base_pens['G'] = QPen(QColor("#8172B2"));
    param.plot_p->base_pens['-'] = QPen(QColor("#64B5CD"));
    
    for(size_t idx=1; idx<argc; idx++)
    {
        string cur_param( argv[idx] );
        if( cur_param.at(0) == '-' and cur_param.at(1) == '-' )
        {
            if( idx+1 == argc )
            {
                cerr << "Param Error: " << cur_param << endl;
                print_usage();
                throw runtime_error("Param Error");
            }
            string next_param( argv[idx+1] );
            if( cur_param == "--baseColor" )
            {
                vector<string> items = split(next_param, '/');
                for(string pair_string: items)
                {
                    vector<string> pair = split(pair_string, ':');
                    param.plot_p->base_pens[ pair.at(0).at(0) ] = QPen(QColor( pair.at(1).c_str() ));
                }
            }else if( cur_param == "--shapeColor" )
            {
                vector<string> items = split(next_param, '/');
                for(string pair_string: items)
                {
                    vector<string> pair = split(pair_string, ':');
                    param.plot_p->species_block_brush[ pair.at(0) ] = QColor( pair.at(1).c_str() );
                }
            }else{
                cerr << "Param Error: " << cur_param << endl;
            }
            idx++;
        }else{
            param.file_list.push_back( cur_param );
        }
    }
    
    if( param.file_list.size() != 3 )
    {
        cerr << "Param Error" << endl;
        print_usage();
        throw runtime_error("Param Error");
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    if( argc < 4 )
    {
        print_usage();
        return -1;
    }
    
    Param param;
    param.plot_p = new plot_param;
    shared_ptr<plot_param> ptr_param( param.plot_p );
    parse_params(argc, argv, param);
    
    Multi_align ma( param.file_list.at(0) );
    const ShapeSet shape( param.file_list.at(1) );
    
    param.plot_p->species_number = ma.size();
    param.plot_p->total_len = ma.get_align( ma.keys().at(0) ).align_length;
    
    vector<string> species_names = ma.keys();
    int idx = 0;
    for( string name: species_names )
    {
        if( param.plot_p->species_block_brush.find( name ) == param.plot_p->species_block_brush.end() )
            param.plot_p->species_block_brush[ name ] = QColor(color_base.at(idx%color_base.size()).c_str());
        idx++;
    }
    param.plot_p->species_block_brush[ "default" ] = QColor("#b2b2b2");
    

    clog << "Start to calculate plot params..." << endl;
    calculate_plot_params( ptr_param );

    
    clog << "Start to plot..." << endl;
    align_plot( ptr_param,
               param.file_list.at(2),
               ma,
               shape);

    return 0;
}
