

#include "svg_output.h"

using namespace std;

//=============================================================================
SvgOutput::SvgOutput(
  const std::string& filename
)
  : m_file_stream(filename.c_str(), std::ios::out),
    m_scale(1),
    m_flip(1),
    m_layer(2),
    m_level(0),
    m_stroke_width(2),
    m_stroke_arrows(SVG_ARROW_NONE),
    m_clip_max(-1)
{
  m_fill.set(0x000000, 0.0);

  const char* header =
  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
  "<!-- Created with Inkscape (http://www.inkscape.org/) -->\n"
  "\n"
  "<svg\n"
  "   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
  "   xmlns:cc=\"http://creativecommons.org/ns#\"\n"
  "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
  "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
  "   xmlns=\"http://www.w3.org/2000/svg\"\n"
  "   xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"\n"
  "   xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n"
  "   version=\"1.1\"\n"
  "   width=\"744.09448\"\n"
  "   height=\"1052.3622\"\n"
  "   id=\"svg2\"\n"
  "   inkscape:version=\"0.48.1 \"\n"
  "   sodipodi:docname=\"D:\\data\\utest_vd001.svg\">\n"
  "  <sodipodi:namedview\n"
  "     pagecolor=\"#ffffff\"\n"
  "     bordercolor=\"#666666\"\n"
  "     borderopacity=\"1\"\n"
  "     objecttolerance=\"10\"\n"
  "     gridtolerance=\"10\"\n"
  "     guidetolerance=\"10\"\n"
  "     inkscape:pageopacity=\"0\"\n"
  "     inkscape:pageshadow=\"2\"\n"
  "     inkscape:window-width=\"640\"\n"
  "     inkscape:window-height=\"480\"\n"
  "     id=\"namedview74\"\n"
  "     showgrid=\"false\"\n"
  "     inkscape:zoom=\"0.24801347\"\n"
  "     inkscape:cx=\"-91.637245\"\n"
  "     inkscape:cy=\"526.18109\"\n"
  "     inkscape:window-x=\"0\"\n"
  "     inkscape:window-y=\"0\"\n"
  "     inkscape:window-maximized=\"0\"\n"
  "     inkscape:current-layer=\"layer1\" />\n"
  "  <defs\n"
  "     id=\"defs4\">\n"
  "    <marker\n"
  "       inkscape:stockid=\"Arrow1Mend\"\n"
  "       orient=\"auto\"\n"
  "       refY=\"0.0\"\n"
  "       refX=\"0.0\"\n"
  "       id=\"Arrow1Mend\"\n"
  "       style=\"overflow:visible;\">\n"
  "      <path\n"
  "         id=\"path3830\"\n"
  "         d=\"M 0.0,0.0 L 5.0,-5.0 L -12.5,0.0 L 5.0,5.0 L 0.0,0.0 z \"\n"
  "         style=\"fill-rule:evenodd;stroke:#000000;stroke-width:1.0pt;"
    "stroke-opacity:0.7;fill-opacity:0.0;marker-start:none;\"\n"
  "         transform=\"scale(0.4) rotate(180) translate(10,0)\" />\n"
  "    </marker>\n"
  "  </defs>\n"
  "  <metadata\n"
  "     id=\"metadata7\">\n"
  "    <rdf:RDF>\n"
  "      <cc:Work\n"
  "         rdf:about=\"\">\n"
  "        <dc:format>image/svg+xml</dc:format>\n"
  "        <dc:type\n"
  "           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n"
  "        <dc:title></dc:title>\n"
  "      </cc:Work>\n"
  "    </rdf:RDF>\n"
  "  </metadata>\n";
  
  m_file_stream << header; //<< showpoint
}

//=============================================================================
void SvgOutput::open_layer(const std::string& layer_name)
{
  while (m_level > 0) {
    close_level();
  }
  
  m_file_stream << "  <g\n    inkscape:groupmode=\"layer\"\n"
                   "    id=\"layer" << m_layer << "\"\n"
                   "    inkscape:label=\"" << layer_name << "\">\n";
          
  m_layer++;
  m_level++;
}

//=============================================================================
void SvgOutput::open_group(const std::string& group_name)
{
  m_file_stream << "  <g\n     id=\"" << group_name << "\">\n";
  m_level++;
}

//=============================================================================
void SvgOutput::close_level()
{
  m_file_stream << "  </g>\n";
  m_level--;
}

//=============================================================================
SvgColour::SvgColour()
  : m_red(0),
    m_green(0),
    m_blue(0),
    m_opacity(1)
{
}

//=============================================================================
const std::string& SvgColour::string(const std::string& type)
{
  m_str = type + ":rgb(" + to_string(m_red) + "," + to_string(m_green) + "," + to_string(m_blue) + ");"
        + type + "-opacity:" + to_string(m_opacity); //opacity_stream.str();

  return m_str;
}

//=============================================================================
void SvgColour::set(
  int colour,
  double opacity
)
{
  m_red = (colour & 0xFF0000) >> 16;
  m_green = (colour & 0x00FF00) >> 8;
  m_blue = (colour & 0x0000FF);

  m_opacity = opacity;
}

//=============================================================================
void SvgOutput::set_stroke(
  int colour,
  double opacity,
  double width,
  SvgArrow arrow
)
{
  m_stroke.set(colour, opacity);
  
  m_stroke_width = width;
  
  m_stroke_arrows = arrow;
}

//=============================================================================
void SvgOutput::set_fill(
  int colour,
  double opacity
)
{
  m_fill.set(colour, opacity);
}

//=============================================================================
void SvgOutput::write_line(
  const std::string& id,
  Point2 start,
  Point2 end
)
{
  std::string arrow = "";
  if (m_stroke_arrows == SVG_ARROW_END) {
    arrow = "marker-end:url(#Arrow1Mend)";
  }

  start.x=(start.x*m_scale); start.y=(start.y*m_scale*m_flip);
  end.x=(end.x*m_scale); end.y=(end.y*m_scale*m_flip);

  clip_point(start);
  clip_point(end);
  
  m_file_stream
    << "    <line id=\"" << id.c_str()
    << "\" x1=\"" << start.x << "\" y1=\"" << start.y
    << "\" x2=\"" << end.x   << "\" y2=\"" << end.y << "\"\n"
    << "      style=\"" << m_stroke.string("stroke").c_str()
    << ";stroke-width:" << m_stroke_width
    << ";\n        " << arrow.c_str() << ";\"/>\n";
}

//=============================================================================
void SvgOutput::write_path(
  const std::string& id,
  std::vector<Point2> points
)
{
  std::string arrow = "";
  if (m_stroke_arrows == SVG_ARROW_END) {
    arrow = "marker-end:url(#Arrow1Mend)";
  }

  m_file_stream
    << "    <path id=\"" << id.c_str() << "\""
    << "          d=\"M ";
  for (Point2 point : points) {
    prepare(point);
    m_file_stream << point.x << "," << point.y << " ";
  }

  m_file_stream
    << "\"\n"
    << "          style=\"" << m_stroke.string("stroke").c_str()
    << ";stroke-width:" << m_stroke_width
    << ";" << m_fill.string("fill").c_str()
    << ";" << arrow.c_str() << ";\"/>\n";
}

//=============================================================================
void SvgOutput::write_connector(
  const std::string& id,
  const std::string& from,
  const std::string& to
)
{
  std::string arrow = "";
  if (m_stroke_arrows == SVG_ARROW_END) {
    arrow = "marker-end:url(#Arrow1Mend)";
  }
  
  m_file_stream << "    <path"
                << "      style=\"" << m_stroke.string("stroke").c_str()
                << ";stroke-width:" << m_stroke_width
                << ";" << arrow.c_str() <<";\"\n"
                   "      d=\"M 0,0 0,0\"\n"
                   "      id=\"" << id.c_str() << "\"\n"
                   "      inkscape:connector-type=\"polyline\"\n"
                   "      inkscape:connector-curvature=\"0\"\n"
                   "      inkscape:connection-start=\"#"<<from.c_str()<<"\"\n"
                   "      inkscape:connection-start-point=\"d4\"\n"
                   "      inkscape:connection-end=\"#" << to.c_str() << "\"\n"
                   "      inkscape:connection-end-point=\"d4\" />\n";
          
}

//=============================================================================
void SvgOutput::write_circle(
  const std::string& id,
  Point2 centre,
  double radius
)
{
  centre.x = centre.x * m_scale; 
  centre.y = centre.y * m_scale * m_flip;
  radius *= m_scale;
  
  clip_point(centre);
  if (m_clip_max>0 &&
      (radius < -m_clip_max || m_clip_max < radius)) {
    radius = m_clip_max;
  }
  
  m_file_stream << "    <circle id=\"" << id.c_str()
                << "\" cx=\"" << centre.x << "\" cy=\"" << centre.y
                << "\" r=\"" << radius << "\"\n"
                   "      style=\"" << m_stroke.string("stroke").c_str()
                << ";stroke-width:" << m_stroke_width
                << ";\n        " << m_fill.string("fill").c_str() << ";\"/>\n";
}

//=============================================================================
void SvgOutput::set_scale(
  double scale,
  bool flip
)
{
  m_scale = scale;
  m_flip = flip ? -1 : 1;
}

//=============================================================================
void SvgOutput::set_clip(
  double abs_clip
)
{
  m_clip_max = abs_clip;
}

//=============================================================================
void SvgOutput::clip_point(
  Point2& point
)
{
  if (m_clip_max > 0) {
    if (point.x > m_clip_max) {
      point.x = m_clip_max;
    } else if (point.x < -m_clip_max) {
      point.x = -m_clip_max;
    }
    if (point.y > m_clip_max) {
      point.y = m_clip_max;
    } else if (point.y < -m_clip_max) {
      point.y = -m_clip_max;
    }
  }
}
  
//=============================================================================
void SvgOutput::prepare(Point2& point)
{
  point.x = (point.x*m_scale);
  point.y = (point.y*m_scale*m_flip);

  clip_point(point);
}

//=============================================================================
SvgOutput::~SvgOutput()
{
  while (m_level > 0) {
    close_level();
  }
  m_file_stream << "</svg>\n";
  m_file_stream.close();
}

