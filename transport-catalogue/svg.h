#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <variant>

namespace Svg {

struct Point {
    Point() = default;
    Point(double x, double y) : x(x), y(y) {}

    double x = 0;
    double y = 0;
};

struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<< (std::ostream& out, Svg::Point point);

std::ostream& operator<< (std::ostream& out, Svg::StrokeLineCap line_cap);

std::ostream& operator<< (std::ostream& out, Svg::StrokeLineJoin line_join);

struct Rgb {
    explicit Rgb(uint8_t red_in = 0, uint8_t green_in = 0, uint8_t blue_in = 0) : red(red_in), green(green_in), blue(blue_in) {}

    uint8_t red;
    uint8_t green;
    uint8_t blue;
};


struct Rgba {
    explicit Rgba(uint8_t red_in = 0, uint8_t green_in = 0, uint8_t blue_in = 0, double opacity_in = 1.0)
        : red(red_in), green(green_in), blue(blue_in), opacity (opacity_in) {}

    uint8_t red;
    uint8_t green;
    uint8_t blue;
    double opacity;
};

using Color = std::variant<std::monostate, std::string, Svg::Rgb, Svg::Rgba>;

inline const std::string NoneColor{"none"};

std::ostream& operator<< (std::ostream& out, Color);

void PrintRoots(std::ostream& out, std::monostate);

void PrintRoots(std::ostream& out, std::string str);

void PrintRoots(std::ostream& out, Svg::Rgb rgb);

void PrintRoots(std::ostream& out, Svg::Rgba rgba);


template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        width_ = std::move(width);
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = std::move(line_cap);
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = std::move(line_join);
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;
        if (fill_color_) {
             out << "fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        if (width_) {
            out << " stroke-width=\""sv << *width_ << "\""sv;
        }
        if (line_cap_) {
            out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
        }
        if (line_join_) {
            out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
        }
    }
private:
    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }
};

class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center = {0.0, 0.0});

    Circle& SetRadius(double radius = 1.0);

private:
    Point center_;
    double radius_ = 1.0;

private:
    void RenderObject(const RenderContext& context) const override;
};

class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);

private:
    std::vector<Point> points_;

private:
    void RenderObject(const RenderContext& context) const override;
};

class Text final : public Object, public PathProps<Text> {
public:
    Text& SetPosition(Point pos);

    Text& SetOffset(Point offset);

    Text& SetFontSize(uint32_t font_size);

    Text& SetFontFamily(std::string font_family);

    Text& SetFontWeight(std::string font_weight);

    Text& SetData(std::string data);

private:
    Point pos_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;

private:
    void RenderObject(const RenderContext& context) const override;
};

class ObjectContainer {
public:
    template <typename Obj>
    void Add(Obj obj);

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
};

template <typename Obj>
void ObjectContainer::Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
}

class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;

    virtual ~Drawable() = default;

};

class Document final : public ObjectContainer {
public:
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> documents_;
};

}  // namespace svg
