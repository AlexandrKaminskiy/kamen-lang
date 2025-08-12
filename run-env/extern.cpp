#include <iostream>
#include <cairo.h>
#include <cairo-svg.h>
#include <list>
#include <algorithm>
#include <string.h>

typedef enum ShapeType {
    BEZIER,
    RECTANGLE,
    OVAL,
    CIRCLE,
    LINE,
    POLYLINE,
    TEXT
} ShapeType;

typedef struct Shape {
    ShapeType type;
    bool has_color;
    double r;
    double g;
    double b;
    double rotate;
    double radius;

    const char* text;
    double font_scale;
    const char* family;

    double x0;
    double y0;
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;
} Shape;

typedef struct Context {
    cairo_surface_t* surface;
    cairo_t* cr;
    std::list<Shape *> shapes;
    double width;
    double height;
} Context;



Shape* copy_shape(const Shape* src) {
    if (!src) return nullptr;

    Shape* dst = new Shape();

    dst->type = src->type;
    dst->has_color = src->has_color;
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->rotate = src->rotate;
    dst->radius = src->radius;

    dst->text = src->text ? strdup(src->text) : nullptr;
    dst->family = src->family ? strdup(src->family) : nullptr;
    dst->font_scale = src->font_scale;
    dst->x0 = src->x0;
    dst->y0 = src->y0;
    dst->x1 = src->x1;
    dst->y1 = src->y1;
    dst->x2 = src->x2;
    dst->y2 = src->y2;
    dst->x3 = src->x3;
    dst->y3 = src->y3;

    return dst;
}

void draw_circle(Context* ctx, Shape* shape) {
    cairo_arc(ctx->cr, 0, 0, shape->radius, 0, 2 * 3.14159265);
    cairo_fill(ctx->cr);
}

void draw_rectangle(Context* ctx, Shape* shape) {
    double width = shape->x1 - shape->x0;
    double height = shape->y1 - shape->y0;
    cairo_rectangle(ctx->cr, 0, 0, width, height);
    cairo_fill(ctx->cr);
}

void draw_line(Context* ctx, Shape* shape) {
    cairo_set_line_width(ctx->cr, 2.0);
    cairo_move_to(ctx->cr, 0, 0);
    cairo_line_to(ctx->cr, shape->x1 - shape->x0, shape->y1 - shape->y0);
    cairo_stroke(ctx->cr);
}

void rotate(Context* ctx, Shape* shape) {
    cairo_save(ctx->cr);
    cairo_translate(ctx->cr, shape->x0, shape->y0);
    cairo_rotate(ctx->cr, shape->rotate);
}

void rotate_revert(Context* ctx) {
    cairo_restore(ctx->cr);
}

void draw_bezier(Context* ctx, Shape* shape) {
    cairo_move_to(ctx->cr, 0, 0);
    const double x0 = shape->x0;
    const double y0 = shape->y0;
    cairo_curve_to(ctx->cr, shape->x1 - x0, shape->y1 - y0, shape->x2 - x0, shape->y2 - y0, shape->x3 - x0, shape->y3 - y0);
    cairo_stroke(ctx->cr);
}

void draw_text(Context* ctx, Shape* shape) {
    cairo_select_font_face(ctx->cr, strdup(shape->family), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(ctx->cr, shape->font_scale);
    cairo_move_to(ctx->cr, 0, 0);
    cairo_show_text(ctx->cr, strdup(shape->text));
}

extern "C" void Render(Context* ctx) {
    for (auto shape: ctx->shapes) {
        cairo_set_source_rgb(ctx->cr, shape->r, shape->g, shape->b);
        rotate(ctx, shape);
        switch (shape->type) {
            case CIRCLE: draw_circle(ctx, shape); break;
            case RECTANGLE: draw_rectangle(ctx, shape); break;
            case LINE: draw_line(ctx, shape); break;
            case TEXT: draw_text(ctx, shape); break;
            case BEZIER: draw_bezier(ctx, shape); break;
            default: break;
        }
        rotate_revert(ctx);
    }
    cairo_destroy(ctx->cr);
    cairo_surface_destroy(ctx->surface);
}


extern "C" Context* CreateContext(const char* filename, int width, int height) {
    Context* context = new Context();
    context->surface = cairo_svg_surface_create(strdup(filename), width, height);
    context->cr = cairo_create(context->surface);
    context->width = width;
    context->height = height;
    cairo_set_source_rgb(context->cr, 1, 1, 1);
    cairo_paint(context->cr);
    cairo_set_source_rgb(context->cr, 0, 0, 0);
    return context;
}

extern "C" Shape *DrawBezier(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3) {
    Shape *bezier = new Shape();
    bezier->type = BEZIER;
    bezier->x0 = x0;
    bezier->y0 = y0;
    bezier->x1 = x1;
    bezier->y1 = y1;
    bezier->x2 = x2;
    bezier->y2 = y2;
    bezier->x3 = x3;
    bezier->y3 = y3;
    return bezier;
}

extern "C" Shape *DrawText(const char *text, double x, double y, double font_scale, const char *family) {
    Shape *text_shape = new Shape();
    text_shape->type = TEXT;
    text_shape->x0 = x;
    text_shape->y0 = y;
    text_shape->font_scale = font_scale;
    text_shape->family = strdup(family);
    text_shape->text = strdup(text);
    return text_shape;
}

extern "C" Shape * DrawCircle(double x, double y, int radius) {
    Shape * circle = new Shape();
    circle->type = CIRCLE;
    circle->radius = radius;
    circle->has_color = false;
    circle->x0 = x;
    circle->y0 = y;
    return circle;
}

extern "C" Shape * DrawRect(double x0, double y0, double x1, double y1) {
    Shape * rect = new Shape();
    rect->type = RECTANGLE;
    rect->has_color = false;
    rect->y0 = y0;
    rect->x0 = x0;
    rect->y1 = y1;
    rect->x1 = x1;
    return rect;
}

extern "C" Shape * DrawLine(double x0, double y0, double x1, double y1) {
    Shape * rect = new Shape();
    rect->type = LINE;
    rect->has_color = false;
    rect->y0 = y0;
    rect->x0 = x0;
    rect->y1 = y1;
    rect->x1 = x1;
    return rect;
}

extern "C" void Fill(Shape * shape, int r, int g, int b) {
    shape->has_color = true;
    shape->r = r / 255.0;
    shape->g = g / 255.0;
    shape->b = b / 255.0;
}

extern "C" void Rotate(Shape * shape, double rads) {
    shape->has_color = true;
    shape->rotate = rads;
}

extern "C" Shape * Reflect(Shape * shape, double x, double y) {
    auto shape_copy = copy_shape(shape);
    shape_copy->x0 = 2 * x - shape_copy->x0;
    shape_copy->y0 = 2 * x - shape_copy->y0;
    shape_copy->x1 = 2 * x - shape_copy->x1;
    shape_copy->y1 = 2 * x - shape_copy->y1;
    shape_copy->x2 = 2 * x - shape_copy->x2;
    shape_copy->y2 = 2 * x - shape_copy->y2;
    shape_copy->x3 = 2 * x - shape_copy->x3;
    shape_copy->y3 = 2 * x - shape_copy->y3;

    return shape_copy;
}

extern "C" void AddToContext(Context* ctx, Shape * shape) {
    ctx->shapes.push_back(shape);
}

extern "C" void RemoveFromContext(Context* ctx, Shape * shape) {
    auto it = std::find(ctx->shapes.begin(), ctx->shapes.end(), shape);
    if (it != ctx->shapes.end()) {
        ctx->shapes.erase(it);
    }
}

