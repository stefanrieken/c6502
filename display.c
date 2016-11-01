#include <gtk/gtk.h>

#include "memory.h"

#define WIDTH 32
#define PIXEL_SIZE 5
#define SIZE WIDTH * PIXEL_SIZE

cairo_pattern_t * colors[16];

GtkWidget * window;

double c(unsigned char color)
{
	return color / 255.0;
}

void setup_colors()
{
	colors[ 0] = cairo_pattern_create_rgb(c(0),c(0),c(0));
	colors[ 1] = cairo_pattern_create_rgb(c(0xFF), c(0xFF), c(0xFF));
	colors[ 2] = cairo_pattern_create_rgb(c(0x88),c(0),c(0));
	colors[ 3] = cairo_pattern_create_rgb(c(0xAA),c(0xFF),c(0xEE));
	colors[ 4] = cairo_pattern_create_rgb(c(0xCC), c(0x44), c(0xCC));
	colors[ 5] = cairo_pattern_create_rgb(c(0), c(0xCC), c(0x55));
	colors[ 6] = cairo_pattern_create_rgb(c(0),c(0),c(0xAA));
	colors[ 7] = cairo_pattern_create_rgb(c(0xEE), c(0XEE), c(0x77));
	colors[ 8] = cairo_pattern_create_rgb(c(0xDD), c(0x88), c(0x55));
	colors[ 9] = cairo_pattern_create_rgb(c(0x66),c(0x44),c(0));
	colors[10] = cairo_pattern_create_rgb(c(0xFF), c(0x77), c(0x77));
	colors[11] = cairo_pattern_create_rgb(c(0x33),c(0x33),c(0x33));
	colors[12] = cairo_pattern_create_rgb(c(0x77),c(0x77),c(0x77));
	colors[13] = cairo_pattern_create_rgb(c(0xaa),c(0xFF),c(0x66));
	colors[14] = cairo_pattern_create_rgb(c(0x00), c(0x88), c(0xFF));
	colors[15] = cairo_pattern_create_rgb(c(0xBB), c(0xBB), c(0xBB));
}

gboolean draw(GtkWidget * widget, cairo_t * cr, gpointer data)
{
	cairo_set_line_width (cr, 0.1);

	for (int i=0x0000; i<0x0400; i++) {
		int x = i % 32;
		int y = i / 32;
		int color = mem_get(0x0200 + i);
		cairo_set_source (cr, colors[color % 16]);
		cairo_rectangle (cr, x*PIXEL_SIZE, y*PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE);
		cairo_fill (cr);
	}

	return FALSE;
}

static void activate(GtkApplication * app, gpointer user_data)
{
	//GtkWidget * window;
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Window");
	gtk_window_set_default_size(GTK_WINDOW(window), SIZE,SIZE);

	GtkWidget * drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing_area, SIZE,SIZE);
	g_signal_connect(drawing_area, "draw", G_CALLBACK(draw), NULL);
	gtk_container_add(GTK_CONTAINER(window), drawing_area);

	gtk_widget_show_all(window);
}

int display_init (char * app_name)
{
	GtkApplication * app;
	int status;

	int argc = 1;
	char * argv[1] = {app_name};

	setup_colors();

	app = gtk_application_new("nl.mad.c6502", G_APPLICATION_HANDLES_OPEN);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
}

static gboolean in_gtk_thread(gpointer user_data)
{
	gtk_widget_queue_draw(window);
	return G_SOURCE_REMOVE;
}

void display_redraw()
{
	gdk_threads_add_idle(in_gtk_thread, NULL);
}

