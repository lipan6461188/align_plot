#ifndef ALIGN_PLOT_H
#define ALIGN_PLOT_H

#include <QPdfWriter>
#include <QPainter>
#include <QString>
#include "include_head.h"
#include "align.h"
#include "shape.h"

double pt2mm(double pt)
{
    return pt / 47.0;
}

struct plot_param
{
    double margin = 500;             /* Margin size */
    double base_block_dist = 20;    /* dist between base and block */
    double block_dist = 30;         /* dist between base and block */
    double line_dist = 50;          /* dist between two  lines of dist */
    double block_height = 60;       /*  */

    uLONG coor_label_dist = 20;

    double block_width = 20;
    uINT blocks_in_line = 200;

    /* color parameters */
    unordered_map<char, QPen> base_pens;
    unordered_map<string, QBrush> species_block_brush;

    bool draw_coor = true;
    bool draw_base = true;

    /* set by user */
    uINT species_number = 0;
    uLONG total_len = 0;

    uLONG total_lines = 0;
    double line_height = 0;

    double page_height = 0.0;
    double page_width = 0.0;
    vector<double> y_start_lst;

};

void calculate_plot_params(shared_ptr<plot_param> param)
{
    uINT total_lines = ceil((double)param->total_len / param->blocks_in_line);
    param->total_lines = total_lines;
    double line_height = param->species_number*(param->base_block_dist+param->block_height+param->block_dist);
    param->line_height = line_height;

    param->page_height = 2*param->margin + param->total_lines * (line_height + param->line_dist) - param->line_dist;
    param->page_width = 2*param->margin + param->block_width * param->blocks_in_line;

    //cout << "page_height:" << param->page_height << "\npage_width:" << param->page_width << endl;

    param->y_start_lst.clear();
    for(uINT idx=0; idx < param->total_lines; idx++)
    {
        param->y_start_lst.push_back( param->margin + idx*(line_height+param->line_dist) );
    }
}

void align_plot(shared_ptr<plot_param> param, const string &save_file_name, const Multi_align &multi_align, const ShapeSet &shape_set)
{
    QPdfWriter pdf_writer(QString::fromStdString(save_file_name));
    pdf_writer.setResolution(1200);
    pdf_writer.setPageSize( QPageSize( QSizeF( pt2mm(param->page_width), pt2mm(param->page_height) ), QPageSize::Millimeter, "", QPageSize::ExactMatch ) );
    pdf_writer.setPageMargins(QMarginsF(0,0,0,0));

    //clog << "Start to plot " << save_file_name << endl;

    QPainter painter( &pdf_writer );
    painter.setRenderHint( QPainter::Antialiasing );

    double width = painter.device()->width();
    double height = painter.device()->height();

    //cout << width << "\t" << param->page_width << "\n" << height << "\t" << param->page_height << endl;

    QFont base_font("HelveticaNeueLTStd*", 1, 3);
    QFont species_name_font("HelveticaNeueLTStd*", 2, 3);
  //  painter.setFont(base_font);

    uLONG align_len = multi_align.get_align( multi_align.keys().at(0) ).align_length;

    bool first = 1; unordered_map<uINT, uLONG> base_map_idex;
    for(uINT line_idx = 0; line_idx < param->total_lines; line_idx++)
    {
        double line_base = param->margin + line_idx * (param->line_height + param->line_dist);

        for(uINT species_idx = 0; species_idx < param->species_number; species_idx++)
        {
            double species_base = line_base + species_idx * (param->base_block_dist + param->block_height + param->block_dist);
            string species_name = multi_align.keys().at(species_idx);

            /* draw species name */
            double species_name_x = param->margin - 800;
            double species_name_y = species_base + param->base_block_dist;
            QRect species_name_rect(species_name_x, species_name_y, 775, param->block_height);
            QPen tmp_pen( param->species_block_brush.at(species_name).color() );
            painter.setPen( tmp_pen );
            painter.setFont(species_name_font);
            painter.drawText(species_name_rect, Qt::AlignVCenter | Qt::AlignRight, QString::fromStdString(species_name));
            painter.setFont(base_font);

            for(uINT base_idx=0; base_idx<param->blocks_in_line; base_idx++)
            {
                uINT total_base_idx = base_idx+line_idx*param->blocks_in_line;
                if(total_base_idx == align_len) break;
                char base = multi_align.get_align( species_name ).align_seq.at( total_base_idx );

                //cout << base;
                double base_x = param->margin + param->block_width/2 + base_idx * param->block_width;
                double base_y = species_base;



                if(param->draw_base){
                    painter.setPen(param->base_pens.at(base));
                    QString char_to_draw = QString(QChar(base));
                    painter.drawText(QRect(base_x,base_y,param->block_width, param->base_block_dist), Qt::AlignCenter, char_to_draw);
                }


                // plot block
                double score_x = param->margin + param->block_width/2 + base_idx * param->block_width;
                double score_y_bottom = species_base + param->base_block_dist + param->block_height;
                if(base != '-')
                {
                    base_map_idex[species_idx]++;


                    /* plot base index */
                    if(param->draw_coor and base_map_idex[species_idx] % param->coor_label_dist == 0)
                    {
                        QPen black_pen(QColor("black"));
                        painter.setPen(black_pen);
                        painter.drawText(QRect(base_x-400+param->block_width/2, base_y-param->base_block_dist, 800, param->base_block_dist), Qt::AlignVCenter | Qt::AlignHCenter, QString::number(base_map_idex[species_idx]));
                    }


                    double score = 0.0;
                    try{
                       score  = shape_set.get_shape(species_name).shape_score.at( base_map_idex[species_idx] -1 );
                    }catch(out_of_range e){
                        cout << "Error: " << score << "\t" << base_map_idex[species_idx] -1 << endl;
                    }

                    QBrush &current_brush = (score == -1) ? param->species_block_brush.at("default") : param->species_block_brush.at(species_name);

                    if(score == NULL_Score)
                    {
                       // painter.setBrush(brown_brush);

                        painter.fillRect(QRect(score_x, score_y_bottom-param->block_height, param->block_width, param->block_height), current_brush);
                    }else{
                       // painter.setBrush(blue_brush);
                        painter.fillRect(QRectF(score_x, score_y_bottom-param->block_height*score, param->block_width, param->block_height*score), current_brush);
                    }
                }else{
                    painter.fillRect(QRect(score_x, score_y_bottom-param->block_height, param->block_width, param->block_height), param->species_block_brush.at("default"));

                }
            }
            first = false;
        }

    }
}


#endif // ALIGN_PLOT_H

















