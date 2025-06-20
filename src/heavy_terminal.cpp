#include "heavy_terminal.h"



bool HeavyTerminal::setup(int argc,char** argv)
{
    gtk_init(&argc, &argv);
    applyStyling();
    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(main_window), DEFAULT_W, DEFAULT_H);
    gtk_window_set_title(GTK_WINDOW(main_window), TERMINAL_NAME);
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), TERMINAL_NAME);
    gtk_window_set_titlebar(GTK_WINDOW(main_window), header_bar);

    GtkWidget *new_tab_button = gtk_button_new_from_icon_name("list-add-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), new_tab_button);

    GtkWidget *settings_button = gtk_button_new_from_icon_name("emblem-system-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), settings_button);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    gif_image_ = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), gif_image_, FALSE, FALSE, 0);
    updateGIFPath(default_gif_path_);  

    GtkWidget *notebook = gtk_notebook_new();
    gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    g_signal_connect(new_tab_button, "clicked", G_CALLBACK(addNewTerminalTab), notebook);
    g_signal_connect(settings_button, "clicked", G_CALLBACK(onSettingButtonClicked), main_window);

    addNewTerminalTab(NULL, notebook);
    gtk_widget_show_all(main_window);

    return true;
}

gboolean HeavyTerminal::onTerminalKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer user_data) 
{
    if (!GTK_IS_WIDGET(widget)) return FALSE;

    // Detect Ctrl+Shift+C for copy
    if ((event->state & GDK_CONTROL_MASK) && (event->state & GDK_SHIFT_MASK) && event->keyval == GDK_KEY_C) {
        vte_terminal_copy_clipboard(VTE_TERMINAL(widget));
        return TRUE;
    }

    // Detect Ctrl+Shift+V for paste
    if ((event->state & GDK_CONTROL_MASK) && (event->state & GDK_SHIFT_MASK) && event->keyval == GDK_KEY_V) {
        vte_terminal_paste_clipboard(VTE_TERMINAL(widget));
        return TRUE;
    }

    return FALSE; // Propagate other keys
}

void HeavyTerminal::updateGIFPath(const char* filepath)
{
    GError *error = nullptr;
    GdkPixbufAnimation *animation = gdk_pixbuf_animation_new_from_file(filepath, &error);
    if (!animation) {
        g_warning("Failed to load selected GIF: %s. Loading default GIF.", error->message);
        g_error_free(error);
        animation = gdk_pixbuf_animation_new_from_file(default_gif_path_, NULL);
    }

    gtk_image_set_from_animation(GTK_IMAGE(gif_image_), animation);
}

void HeavyTerminal::onSettingButtonClicked(GtkButton* button, gpointer user_data)
{
    GtkWidget *parent_window = GTK_WIDGET(user_data);

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Choose a GIF",
        GTK_WINDOW(parent_window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.gif");
    gtk_file_filter_set_name(filter, "GIF images");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        updateGIFPath(filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void HeavyTerminal::addNewTerminalTab(GtkButton* button, gpointer user_data)
{
    GtkNotebook *notebook = GTK_NOTEBOOK(user_data);
    GtkWidget *terminal = vte_terminal_new();

    const GdkRGBA bg_color = {0.117, 0.117, 0.117, 1.0};
    const GdkRGBA fg_color = {0.862, 0.862, 0.862, 1.0};
    vte_terminal_set_colors(VTE_TERMINAL(terminal), &fg_color, &bg_color, nullptr, 0);

    const char *argv[] = {"/bin/bash", nullptr};
    vte_terminal_spawn_async(
        VTE_TERMINAL(terminal), VTE_PTY_DEFAULT,
        nullptr, (char **)argv, nullptr, G_SPAWN_DEFAULT,
        nullptr, nullptr, nullptr, -1,
        nullptr, nullptr, nullptr);

    g_signal_connect(terminal, "child-exited", G_CALLBACK(onTerminalExit), notebook);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(onTerminalKeyPress), nullptr);

    GtkWidget *tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *tab_title = gtk_label_new(TERMINAL_NAME);
    GtkWidget *tab_close_button = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);
    gtk_widget_set_name(tab_close_button, "tab-close-button");

    gtk_box_pack_start(GTK_BOX(tab_label_box), tab_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tab_label_box), tab_close_button, FALSE, FALSE, 0);

    GObject *close_data = G_OBJECT(g_object_new(G_TYPE_OBJECT, nullptr));
    g_object_set_data(close_data, TERMINAL_NAME, terminal);
    g_object_set_data(close_data, DEFAULT_WIN_NAME, notebook);
    g_signal_connect_object(tab_close_button, "clicked", G_CALLBACK(onTabCloseButtonClicked), close_data, G_CONNECT_SWAPPED);

    gtk_notebook_append_page(notebook, terminal, tab_label_box);
    gtk_widget_show_all(GTK_WIDGET(notebook));
    gtk_notebook_set_current_page(notebook, -1);
    gtk_widget_grab_focus(terminal);
}

void HeavyTerminal::onTabCloseButtonClicked(GtkButton *button, gpointer user_data)
{
    GObject *data = G_OBJECT(user_data);
    GtkWidget *terminal_widget = GTK_WIDGET(g_object_get_data(data, TERMINAL_NAME));
    GtkNotebook *notebook = GTK_NOTEBOOK(g_object_get_data(data, DEFAULT_WIN_NAME));
    closeTerminalTab(terminal_widget, notebook);
}

void HeavyTerminal::closeTerminalTab(GtkWidget *terminal_widget, GtkNotebook *notebook)
{
    int page_num = gtk_notebook_page_num(notebook, terminal_widget);
    if (page_num != -1) {
        if (gtk_notebook_get_n_pages(notebook) <= 1) {
            gtk_main_quit();
        } else {
            gtk_notebook_remove_page(notebook, page_num);
        }
    }
}



void HeavyTerminal::onTerminalExit(VteTerminal *terminal, int status, gpointer notebook)
{
    GObject* data_obj = G_OBJECT(g_object_new(G_TYPE_OBJECT, nullptr));
    g_object_set_data(data_obj, TERMINAL_NAME, terminal);
    g_object_set_data(data_obj, DEFAULT_WIN_NAME, notebook);

    g_idle_add([](gpointer data) -> gboolean {
        GObject *obj = G_OBJECT(data);
        GtkWidget *term_widget = GTK_WIDGET(g_object_get_data(obj, TERMINAL_NAME));
        GtkNotebook *notebook_ptr = GTK_NOTEBOOK(g_object_get_data(obj, DEFAULT_WIN_NAME));

        if (GTK_IS_WIDGET(term_widget)) {
            closeTerminalTab(term_widget, notebook_ptr);
        }
        g_object_unref(obj);
        return G_SOURCE_REMOVE;
    }, data_obj);
}

void HeavyTerminal::start()
{
    gtk_main();
}

void HeavyTerminal::applyStyling()
{
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css =
        "window, terminal { background-color: #1E1E1E; }"
        "headerbar { background-color: #2D2D2D; }"
        "headerbar .title { color: #DCDCDC; font-weight: bold; }"
        "headerbar button { background: none; border: none; border-radius: 4px; }"
        "headerbar button:hover { background-color: rgba(255,255,255,0.1); }"
        "notebook > header { background-color: #2D2D2D; }"
        "notebook tab { background-color: #404040; color: #AAAAAA; padding: 4px 10px; }"
        "notebook tab:checked { background-color: #1E1E1E; color: #FFFFFF; }"
        "#tab-close-button { padding: 2px; margin-left: 6px; }";
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
}

