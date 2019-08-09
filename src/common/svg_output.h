#pragma once
#ifndef _SvgOutput_H
#define _SvgOutput_H

#include <vector>
#include <string>
#include <fstream>

//=============================================================================
class SvgColour {
public:
  SvgColour();
  
  void set(int colour, double opacity);

  const std::string& string(const std::string& type);
  
private:
  int m_red;
  int m_green;
  int m_blue;
  double m_opacity;
  
  std::string m_str;
};

enum SvgArrow {
  SVG_ARROW_NONE = 0,
  SVG_ARROW_END
};

//=============================================================================
class SvgOutput { 
public:
  struct Point2 {
    Point2() : x(0), y(0) {}
    Point2(double x_, double y_) : x(x_), y(y_) {}
    double x;
    double y;
  };

  // Constructor.
  SvgOutput(const std::string& filename);

  SvgOutput(const SvgOutput&) = delete;
  SvgOutput& operator=(const SvgOutput&) = delete;

  void open_layer(const std::string& layer_name);
  void open_group(const std::string& group_name);
  void close_level();
  
  void set_stroke(
    int colour,
    double opacity,
    double width = 1,
    SvgArrow arrow = SVG_ARROW_NONE
  );

  void set_fill(
    int colour,
    double opacity
  );
  
  void write_line(
    const std::string& id,
    Point2 start,
    Point2 end
  );

  void write_path(
    const std::string& id,
    std::vector<Point2> points
  );
  
  void write_connector(
    const std::string& id,
    const std::string& from,
    const std::string& to
  );

  void write_circle(
    const std::string& id,
    Point2 centre,
    double radius
  );

  void set_scale(
    double scale,
    bool flip_in_x_axis = false
  );

  // if abs_clip<=0 then no clipping will be applied
  void set_clip(double abs_clip);

  // Destructor.
  ~SvgOutput();

private:
  // friends

  // functions
  void clip_point(Point2& point);
  void prepare(Point2& point);
  
  // variables
  std::ofstream m_file_stream;
  double m_scale;
  double m_flip;
  double m_clip_max;
  int m_layer;
  int m_level;
  
  SvgColour m_stroke;
  double m_stroke_width;
  SvgArrow m_stroke_arrows;
  
  SvgColour m_fill;

};

#endif
