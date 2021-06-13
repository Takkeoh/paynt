#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "tools.h"
#include <math.h>

struct Point
{
  gdouble x;
  gdouble y;
  double r, g, b;
};

struct Layer
{
  GtkWidget *drawing_area;
  cairo_surface_t *surface;
};

int if_eraser = 0;

int if_line = 0;

int if_square = 0;

int if_circle = 0;

int tab[] = {0,0,0,0};

double line_size = 2.0;

void clear_surface (struct Layer layer)
{
  cairo_t *cr;

  cr = cairo_create (layer.surface);

  cairo_set_source_rgba (cr, 1, 1, 1, 0);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);

  cairo_destroy (cr);
}

void clear_surfpreview (void)
{
  cairo_t *cr;

  cr = cairo_create (surfpreview);

  cairo_set_source_rgb (cr, 0,0,0);
  cairo_paint (cr);

  cairo_destroy (cr);
}

gboolean on_clear(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  clear_surface(layers[current_layer]);
  gtk_widget_queue_draw (layers[current_layer].drawing_area);

  return TRUE;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
gboolean draw_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  cairo_set_source_surface (cr, layers[3].surface, 0, 0);
  cairo_paint(cr);
  
  cairo_set_source_surface (cr, layers[0].surface, 0, 0);
  cairo_paint(cr);

  cairo_set_source_surface (cr, layers[1].surface, 0, 0);
  cairo_paint(cr);

  cairo_set_source_surface (cr, layers[2].surface, 0, 0);
  cairo_paint(cr);

  return FALSE;
  
  /* cairo_set_source_surface (cr, layers[current_layer].surface, 0, 0);

     cairo_paint(cr);
   
     return FALSE;*/
}

gboolean draw_cbprev (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  cairo_set_source_surface (cr, surfpreview, 0, 0);
  cairo_paint (cr);

  return FALSE;
}

/* Draw a rectangle on the surface at the given position */
void draw_brush (GtkWidget *widget, struct Point *p1, struct Point *p2, cairo_surface_t *surf)
{
  
  cairo_t *cr;

  /* Paint to the surface, where we store our state */
  cr = cairo_create (surf);
  
  //gtk_widget_set_opacity(layers[1].drawing_area, 0.2);
  
  gdouble x = p1->x;
  gdouble y = p1->y;
  if(if_eraser)
    {
      cairo_set_source_rgba(cr, p1->r, p1->g, p1->b, 0);
      cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    }
  else
    {
      cairo_set_source_rgba(cr, p1->r, p1->g, p1->b, opacity);
    }
  cairo_rectangle (cr, x, y, line_size, line_size);
  cairo_fill (cr);
  cairo_destroy(cr);
  p2->x = x;
  p2->y = y;

  /* Now invalidate the affected region of the drawing area. */
  gtk_widget_queue_draw_area (widget, x, y, line_size, line_size);
}

void draw_line(GtkWidget *widget, struct Point *p1, struct Point *p2, cairo_surface_t *surf)
{
  
  cairo_t *cr;

  /* Paint to the surface, where we store our state */
  cr = cairo_create (surf);

  gdouble x = p1->x;
  gdouble y = p1->y;

  if(if_eraser)
    {
      cairo_set_source_rgba(cr, p1->r, p1->g, p1->b, 0);
      cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    }
  else
    {
      cairo_set_source_rgba(cr, p1->r, p1->g, p1->b, opacity);
    }
  
  //printf("draw line mother fucker\n");
  cairo_set_line_width(cr, line_size);
  cairo_move_to(cr, p2->x, p2->y);
  cairo_line_to(cr, x, y);
  cairo_stroke(cr);

  p2->x = x;
  p2->y = y;
  
  gtk_widget_queue_draw (widget);
}

/* Handle button press events by either drawing a rectangle
 * or clearing the surface, depending on which button was pressed.
 * The ::button-press signal handler receives a GdkEventButton
 * struct which contains this information.
 */
gboolean button_press_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  //printf("current_layer is: %i\n",current_layer);
  
  /* paranoia check, in case we haven't gotten a configure event */
  if (layers[current_layer].surface == NULL)
    return FALSE;
  
  if (event->button == GDK_BUTTON_PRIMARY)
    {
      //printf("Press\n");
      start->x = event->x;
      start->y = event->y;
      
      if (if_line == 1)
	{
	  if (tab[0] == 0)
	    {
	      tab[0] = start->x;
	      tab[1] = start->y;
	    }
	  else if (tab[2] == 0)
	    {
	      tab[2] = start->x;
	      tab[3] = start->y;
	      start->x = tab[0];
	      start->y = tab[1];
	      end->x = tab[2];
	      end->y = tab[3];
	      draw_line (widget, start, end, layers[current_layer].surface);
	      tab[0] = 0;
	      tab[1] = 0;
	      tab[2] = 0;
	      tab[3] = 0;
	    }
	}

      if (if_square == 1)
	{
	  if (tab[0] == 0)
	    {
	      tab[0] = start->x;
	      tab[1] = start->y;
	    }
	  else if (tab[2] == 0)
	    {
	      tab[2] = start->x;
	      tab[3] = start->y;
	      
	      //join x,y to x',y
	      start->x = tab[0];
	      start->y = tab[1];
	      end->x = tab[2];
	      end->y = tab[1];
	      draw_line (widget, start, end, layers[current_layer].surface);

	      //join x',y to x', y'
	      start->x = tab[2];
	      start->y = tab[1];
	      end->x = tab[2];
	      end->y = tab[3];
	      draw_line (widget, start, end, layers[current_layer].surface);

	      //join x,y to x,y'
	      start->x = tab[0];
	      start->y = tab[1];
	      end->x = tab[0];
	      end->y = tab[3];
	      draw_line (widget, start, end, layers[current_layer].surface);

	      //join x,y' to x', y'
	      start->x = tab[0];
	      start->y = tab[3];
	      end->x = tab[2];
	      end->y = tab[3];
	      draw_line (widget, start, end, layers[current_layer].surface);
	      
	      tab[0] = 0;
	      tab[1] = 0;
	      tab[2] = 0;
	      tab[3] = 0;
	    }
	}

      else if (if_circle == 1)
	{
	  cairo_t *cr;
	  /* Paint to the surface, where we store our state */
	  cr = cairo_create (layers[current_layer].surface);

	  cairo_set_line_width(cr, line_size);
	  cairo_set_source_rgba(cr, start->r, start->g, start->b, opacity);
	  cairo_translate(cr, start->x, start->y);
	  cairo_arc(cr, 0, 0, 100, -M_PI, M_PI);
	  cairo_stroke_preserve(cr);
	  gtk_widget_queue_draw (widget);
	}
	    
      else
	draw_brush (widget, start, end, layers[current_layer].surface);
    }
  else if (event->button == GDK_BUTTON_SECONDARY)
    {
      clear_surface(layers[current_layer]);
      gtk_widget_queue_draw (widget);
    }
  //printf("button press event mother fucker\n");
  
  /* We've handled the event, stop processing */
  return TRUE;
}

/* Handle motion events by continuing to draw if button 1 is
 * still held down. The ::motion-notify signal handler receives
 * a GdkEventMotion struct which contains this information.
 */
gboolean motion_notify_event_cb (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  
  /* paranoia check, in case we haven't gotten a configure event */
  if (layers[current_layer].surface == NULL)
    return FALSE;

  if (event->state & GDK_BUTTON1_MASK)
    {
      //printf("Motion Event\n");
      start->x = event->x;
      start->y = event->y;
      if ((if_line == 0)&&(if_square == 0)&&(if_circle == 0))
	draw_line (widget, start, end, layers[current_layer].surface);
      
    }

  //printf("motion notify event mother fucker\n");
  /* We've handled it, stop processing */
  return TRUE;
}

gboolean set_color_black(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if_square = 0;
  if_eraser = 0;
  if_line = 0;
  if_circle = 0;
  
  start->r = gtk_range_get_value(GTK_RANGE(redScale))/255;
  start->g = gtk_range_get_value(GTK_RANGE(greenScale))/255;
  start->b = gtk_range_get_value(GTK_RANGE(blueScale))/255;

  cairo_t *cr;

  cr = cairo_create (surfpreview);

  cairo_set_source_rgb (cr, start->r, start->g, start->b);
  cairo_paint (cr);

  gtk_widget_queue_draw (preview);
  

  return TRUE;
}

gboolean on_circle(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if_line = 0;
  if_eraser = 0;
  if_square = 0;
  if_circle = 1;
  
  return TRUE;
}

gboolean on_line(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if_line = 1;
  if_eraser = 0;
  if_square = 0;
  if_circle = 0;
  
  return TRUE;
}

gboolean on_square(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if_square = 1;
  if_line = 0;
  if_eraser = 0;
  if_circle = 0;
  
  return TRUE;
}

gboolean set_eraser(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if_eraser = 1;
  if_line = 0;
  if_square = 0;
  if_circle = 0;
  
  start->r = 255;
  start->g = 255;
  start->b = 255;
  

  return TRUE;
}

void on_save(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{

  //Create the background image
  cairo_surface_t *img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 840, 610);

  //Create the cairo context
  cairo_t *cr = cairo_create(img);

  //Initialize the image to black transparent
  cairo_set_source_rgba(cr, 1,1,1, 1);
  cairo_paint(cr);

  //Paint one image
  cairo_set_source_surface(cr, layers[0].surface, 0, 0);
  cairo_paint(cr);

  //Paint the other image
  cairo_set_source_surface(cr, layers[1].surface, 0, 0);
  cairo_paint(cr);

  cairo_set_source_surface(cr, layers[2].surface, 0, 0);
  cairo_paint(cr);

  //Destroy the cairo context and/or flush the destination image
  cairo_surface_flush(img);
  cairo_destroy(cr);

  //And write the results into a new file
  cairo_surface_write_to_png(img, "payn't_image.png");

  //Be tidy and collect your trash
  cairo_surface_destroy(img);
}

void draw_preview(void)
{

  double _red = gtk_range_get_value(GTK_RANGE(redScale))/255;
  double _green = gtk_range_get_value(GTK_RANGE(greenScale))/255;
  double _blue = gtk_range_get_value(GTK_RANGE(blueScale))/255;
  
  cairo_t *cr;

  cr = cairo_create (surfpreview);

  cairo_set_source_rgb (cr, _red, _green, _blue);
  cairo_paint (cr);

  gtk_widget_queue_draw (preview);
}


gboolean rScale_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if (if_eraser == 0)
    {
      GtkRange *scale = GTK_RANGE(widget);
      gdouble pos = gtk_range_get_value(scale);
      start->r = pos/255;
    }
  draw_preview();
  
  return TRUE;
}

gboolean gScale_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if (if_eraser == 0)
    {
      GtkRange *scale = GTK_RANGE(widget);
      gdouble pos = gtk_range_get_value(scale);
      start->g = pos/255;

      
    }

  draw_preview();
  
  return TRUE;
}

gboolean bScale_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if (if_eraser == 0)
    {
      GtkRange *scale = GTK_RANGE(widget);
      gdouble pos = gtk_range_get_value(scale);
      start->b = pos/255;

    }

  draw_preview();
  
  return TRUE;
}


gboolean size_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  GtkRange *scale = GTK_RANGE(widget);
  gdouble pos = gtk_range_get_value(scale);
  line_size = pos/5;

  return TRUE;
}
