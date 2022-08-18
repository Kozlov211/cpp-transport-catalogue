#include "svg.h"

using namespace std;

namespace Svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    RenderObject(context);

    context.out << std::endl;
}


Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\""sv;
    out << " r=\""sv << radius_ << "\" "sv;
    RenderAttrs(out);
    out << "/>"sv;
}

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    for (size_t i = 0; i < points_.size(); ++i) {
        if (i == points_.size() - 1) {
            out << points_[i];
        } else {
            out << points_[i] << " ";
        }
    }
    out << "\" ";
    RenderAttrs(out);
    out << "/>"sv;
}

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t font_size) {
    size_ = font_size;
    return *this;
}

Text& Text::SetFontFamily(string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(string data) {
    data_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text ";
    RenderAttrs(out);
    out << " x=\"" << pos_.x << "\" y=\"" << pos_.y << "\"";
    out << " dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\"";
    out << " font-size=\"" << size_ << "\"";
    if (!font_family_.empty()){
        out << " font-family=\"" << font_family_ << "\"";
    }
    if (!font_weight_.empty())   {
        out << " font-weight=\"" << font_weight_ << "\"";
    }
    out << ">" << data_ << "</text>"sv;
}

void Document::AddPtr(unique_ptr<Object>&& obj)  {
    documents_.push_back(move(obj));

}

void Document::Render(ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << endl;
    for (auto& document : documents_) {
        RenderContext ctx(out);
        document->Render(ctx);
    }
    out << "</svg>" << endl;
}

ostream& operator<< (ostream& out, Svg::Point point) {
    out << point.x << ","sv << point.y;
    return out;
}

ostream& operator<< (ostream& out, Svg::StrokeLineCap line_cap) {
    if (line_cap == Svg::StrokeLineCap::BUTT) {
        out << "butt"sv;
    } else if (line_cap == Svg::StrokeLineCap::ROUND) {
        out << "round"sv;
    } else  {
        out << "square"sv;
    }
    return out;
}

ostream& operator<< (ostream& out, Svg::StrokeLineJoin line_join) {
    if (line_join == Svg::StrokeLineJoin::ARCS) {
       out << "arcs"sv;
    } else if (line_join == Svg::StrokeLineJoin::BEVEL) {
        out << "bevel"sv;
    } else if (line_join == Svg::StrokeLineJoin::MITER) {
        out << "miter"sv;
    } else if (line_join == Svg::StrokeLineJoin::MITER_CLIP) {
        out << "miter-clip"sv;
    } else {
        out << "round"sv;
    }
    return out;
}

ostream& operator<< (ostream& out, Svg::Color color) {
    visit([&out](auto value) {
        PrintRoots(out, value);
    }, color);
    return out;
}


void PrintRoots(std::ostream& out, std::monostate) {
    out << "none"sv;
}

void PrintRoots(std::ostream& out, std::string str) {
    out << str;
}

void PrintRoots(std::ostream& out, Svg::Rgb rgb) {
    out << "rgb(" << static_cast<uint>(rgb.red) << "," << static_cast<uint>(rgb.green) << "," << static_cast<uint>(rgb.blue) << ")";
}

void PrintRoots(std::ostream& out, Svg::Rgba rgba) {
    out << "rgba(" << static_cast<uint>(rgba.red) << "," << static_cast<uint>(rgba.green) << "," << static_cast<uint>(rgba.blue) << "," << rgba.opacity << ")";
}

}  // namespace svg


