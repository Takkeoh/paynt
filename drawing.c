#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "tools.h"
#include "err.h"

GtkWidget *window;
GtkWidget *drawing_area0;
GtkWidget *drawing_area1;
GtkWidget *drawing_area2;
GtkWidget *drawing_area3;
GtkWidget *preview;
GtkWidget *layer1;
GtkWidget *layer2;
GtkWidget *layer3;
GtkWidget *save;
GtkWidget *Clear;
GtkWidget *brush_line;
GtkWidget *brush_square;
GtkWidget *brush_circle;
GtkWidget *eraser;
GtkWidget *Black;
GtkBuilder *builder;
GtkWidget *redScale;
GtkWidget *greenScale;
GtkWidget *blueScale;
GtkWidget *op_scale1;
GtkWidget *size;
GtkWidget *cur_layer;
char* filepath;

int one = 1;
int two = 2;
int three = 3;

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

struct Layer *layers = NULL;

gdouble opacity;

struct Layer *current = NULL;
cairo_surface_t *surfpreview = NULL;

int current_layer;

struct Point *start = NULL;
struct Point *end = NULL;

gboolean change_layer1(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
   
    current_layer = 0;
    gtk_label_set_label (GTK_LABEL(cur_layer), "1");

    return TRUE;
}

gboolean change_layer2(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
   
    current_layer = 1;
    gtk_label_set_label (GTK_LABEL(cur_layer), "2");

    return TRUE;
}

gboolean change_layer3(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
   
    current_layer = 2;
    gtk_label_set_label (GTK_LABEL(cur_layer), "3");

    return TRUE;
}

gboolean op_scale1_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  opacity = 1 - (gtk_range_get_value(GTK_RANGE(widget))/100);
  return TRUE;
}


/* Create a new surface of the appropriate size to store our scribbles */
gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  int *nb = (int*)data;
  if (*nb == 1)
    {
      
      if (filepath != NULL)
	{
	  layers[3].surface = cairo_image_surface_create_from_png(filepath);
	}
      else
	{
	  layers[3].surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
								 CAIRO_CONTENT_COLOR,
								 gtk_widget_get_allocated_width (widget),
								 gtk_widget_get_allocated_height (widget));
	  /* Initialize the surface to white */
	  cairo_t *cr;

	  cr = cairo_create (layers[3].surface);

	  cairo_set_source_rgb (cr, 1,1,1);
	  cairo_paint (cr);

	  cairo_destroy (cr);
	}
    }
  if (*nb == 2)
    {
      int width = 2 * gtk_widget_get_allocated_width (widget);
      int height = 2 * gtk_widget_get_allocated_height (widget);
      GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
      guint32 p = 0;
      
      p += 255 << 24;
      p += 255 << 16;
      p += 255 << 8;
      p += 0;
      gdk_pixbuf_fill(pixbuf, p);
      layers[0].surface = gdk_cairo_surface_create_from_pixbuf(pixbuf, 0, gtk_widget_get_window (widget));
      layers[1].surface = gdk_cairo_surface_create_from_pixbuf(pixbuf, 0, gtk_widget_get_window (widget));
      layers[2].surface = gdk_cairo_surface_create_from_pixbuf(pixbuf, 0, gtk_widget_get_window (widget));
    }
    
    
    /* We've handled the configure event, no need for further processing. */
    return TRUE;
}

gboolean configure_event_cbprev (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  if (surfpreview)
    cairo_surface_destroy (surfpreview);

  surfpreview = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));

  /* Initialize the surface to white */
  clear_surfpreview ();

  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}

void close_window (void)
{
  if (layers[0].surface)
    cairo_surface_destroy (layers[0].surface);
     
  gtk_main_quit();
}

int main (int argc,char **argv)
{
  start = malloc(sizeof(struct Point));
  start->r = 0.0;
  start->g = 0.0;
  start->b = 0.0;
  end = malloc(sizeof(struct Point));
  
  layers = malloc(sizeof(struct Layer)*4);

  gtk_init(&argc, &argv);

  builder = gtk_builder_new_from_file ("app_draw.glade");
  
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  gtk_window_set_title (GTK_WINDOW (window), "Payn't");

  g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);

  gtk_builder_connect_signals(builder, NULL);

  opacity = 1;
  
  current_layer = 0;
  drawing_area0    = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area0"));
  drawing_area1    = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area1"));
  drawing_area2    = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area2"));
  drawing_area3    = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area3"));
  layer1 = GTK_WIDGET(gtk_builder_get_object(builder, "layer1"));
  layer2 = GTK_WIDGET(gtk_builder_get_object(builder, "layer2"));
  layer3 = GTK_WIDGET(gtk_builder_get_object(builder, "layer3"));
  save = GTK_WIDGET(gtk_builder_get_object(builder, "save"));
  preview    = GTK_WIDGET(gtk_builder_get_object(builder, "preview"));
  Clear    = GTK_WIDGET(gtk_builder_get_object(builder, "clear"));
  brush_line    = GTK_WIDGET(gtk_builder_get_object(builder, "brush_line"));
  brush_square    = GTK_WIDGET(gtk_builder_get_object(builder, "brush_square"));
  brush_circle    = GTK_WIDGET(gtk_builder_get_object(builder, "brush_circle"));
  eraser = GTK_WIDGET(gtk_builder_get_object(builder, "eraser"));
  Black    = GTK_WIDGET(gtk_builder_get_object(builder, "Black"));
  redScale =  GTK_WIDGET(gtk_builder_get_object(builder, "r_scale"));
  greenScale =  GTK_WIDGET(gtk_builder_get_object(builder, "g_scale"));
  blueScale =  GTK_WIDGET(gtk_builder_get_object(builder, "b_scale"));
  op_scale1 =  GTK_WIDGET(gtk_builder_get_object(builder, "op_scale1"));
  size =  GTK_WIDGET(gtk_builder_get_object(builder, "size"));
  cur_layer = GTK_WIDGET(gtk_builder_get_object(builder, "cur_layer"));

  layers[0].drawing_area = drawing_area1;
  layers[1].drawing_area = drawing_area2;
  layers[2].drawing_area = drawing_area3;
  layers[3].drawing_area = drawing_area0;
  
  gtk_range_set_value(GTK_RANGE(size), 20);

  
  if (argc > 2)
    errx(EXIT_FAILURE, "Too many arguments");
  if (argc == 2)
    filepath = argv[1];
  else
    filepath = NULL;
  
  /* Signals used to handle the backing surface */
  g_signal_connect (drawing_area1, "draw",
                    G_CALLBACK (draw_cb), &two);
  g_signal_connect (drawing_area1,"configure-event",
		    G_CALLBACK (configure_event_cb), &two);
  g_signal_connect (drawing_area2, "draw",
                    G_CALLBACK (draw_cb), &two);
  g_signal_connect (drawing_area2,"configure-event",
		    G_CALLBACK (configure_event_cb), &two);
  g_signal_connect (drawing_area3, "draw",
                    G_CALLBACK (draw_cb), &two);
  g_signal_connect (drawing_area3,"configure-event",
		    G_CALLBACK (configure_event_cb), &two);
  g_signal_connect (drawing_area0, "draw",
                    G_CALLBACK (draw_cb), &one);
  g_signal_connect (drawing_area0,"configure-event",
		    G_CALLBACK (configure_event_cb), &one);


  g_signal_connect (preview, "draw",
                    G_CALLBACK (draw_cbprev), NULL);

  g_signal_connect (preview, "configure-event",
		    G_CALLBACK (configure_event_cbprev), NULL);

  
  /* Event signals */
  g_signal_connect (drawing_area1, "motion-notify-event",
                    G_CALLBACK (motion_notify_event_cb), NULL);
  g_signal_connect (drawing_area1, "button-press-event",
		    G_CALLBACK (button_press_event_cb), NULL);
  g_signal_connect (drawing_area2, "motion-notify-event",
                    G_CALLBACK (motion_notify_event_cb), NULL);
  g_signal_connect (drawing_area2, "button-press-event",
		    G_CALLBACK (button_press_event_cb), NULL);
  g_signal_connect (drawing_area3, "motion-notify-event",
                    G_CALLBACK (motion_notify_event_cb), NULL);
  g_signal_connect (drawing_area3, "button-press-event",
		    G_CALLBACK (button_press_event_cb), NULL);

  

  /* Event signal for layer button */
  g_signal_connect (layer1, "clicked",
    G_CALLBACK (change_layer1), NULL);
  g_signal_connect (layer2, "clicked",
    G_CALLBACK (change_layer2), NULL);
  g_signal_connect (layer3, "clicked",
    G_CALLBACK (change_layer3), NULL);
  g_signal_connect (brush_line, "clicked",
    G_CALLBACK (on_line), NULL);
  g_signal_connect (brush_square, "clicked",
    G_CALLBACK (on_square), NULL);
  g_signal_connect (brush_circle, "clicked",
    G_CALLBACK (on_circle), NULL);
  g_signal_connect (save, "clicked",
    G_CALLBACK (on_save), NULL);

  
  /* Event signal for clear button */
  g_signal_connect (Clear, "clicked",
		    G_CALLBACK (on_clear), NULL);

  /* Event Signals for coloring buttons */
  g_signal_connect (Black, "clicked",
		    G_CALLBACK (set_color_black), NULL);
  g_signal_connect (eraser, "clicked",
		    G_CALLBACK (set_eraser), NULL);


  /* Event signals for coloring sliders */
  g_signal_connect (redScale, "value-changed",
		    G_CALLBACK (rScale_moved), NULL);
  g_signal_connect (greenScale, "value-changed",
		    G_CALLBACK (gScale_moved), NULL);
  g_signal_connect (blueScale, "value-changed",
		    G_CALLBACK (bScale_moved), NULL);
  g_signal_connect (size, "value-changed",
		    G_CALLBACK (size_moved), NULL);

  /* Event signals for opacity */
  g_signal_connect (op_scale1, "value-changed",
		    G_CALLBACK (op_scale1_moved), NULL);

  g_object_unref(builder);
 
  gtk_widget_set_events (drawing_area1, gtk_widget_get_events (drawing_area1)
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_POINTER_MOTION_MASK);
  gtk_widget_set_events (drawing_area2, gtk_widget_get_events (drawing_area2)
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_POINTER_MOTION_MASK);
  gtk_widget_set_events (drawing_area3, gtk_widget_get_events (drawing_area3)
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_POINTER_MOTION_MASK);

  gtk_window_set_keep_above (GTK_WINDOW(window), TRUE);

  gtk_widget_show(window);

  gtk_main();

  free(start);
  free(end);

  return EXIT_SUCCESS;
}
