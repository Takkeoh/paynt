#include <gtk/gtk.h>
#include <cairo.h>

#ifndef TOOLS_H_
#define TOOLS_H_

extern GtkWidget *window;
extern GtkWidget *drawing_area1;
extern GtkWidget *drawing_area2;
extern GtkWidget *drawing_area3;
extern GtkWidget *preview;
extern GtkWidget *layer1;
extern GtkWidget *layer2;
extern GtkWidget *layer3;
extern GtkWidget *Clear;
extern GtkWidget *brush_line;
extern GtkWidget *brush_square;
extern GtkWidget *brush_circle;
extern GtkWidget *eraser;
extern GtkWidget *Black;
extern GtkBuilder *builder;
extern GtkWidget *redScale;
extern GtkWidget *greenScale;
extern GtkWidget *blueScale;
extern GtkWidget *op_scale1;
extern GtkWidget *size;
extern GtkWidget *preview;
extern gdouble opacity;

extern struct Point *start;
extern struct Point *end;
extern struct Layer *current;
extern struct Layer *layers;
extern int current_layer;
extern cairo_surface_t *surfpreview;


gboolean on_clear(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean button_press_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean motion_notify_event_cb (GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean set_color_black(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean on_line(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean on_square(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean on_circle(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean set_eraser(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean rScale_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean gScale_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean bScale_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean size_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean draw_cb (GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean draw_cbprev (GtkWidget *widget, cairo_t *cr, gpointer data);
void on_save(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
void clear_surface (struct Layer layer);
void clear_surfpreview (void);

#endif
