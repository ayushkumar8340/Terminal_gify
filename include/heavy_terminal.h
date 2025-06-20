#ifndef HEAVY_TERMINAL_H
#define HEAVY_TERMINAL_H

#include <gtk/gtk.h>
#include <vte/vte.h>
#include <string>

#define DEFAULT_H 700
#define DEFAULT_W 1000

#define TERMINAL_NAME "HeavyTerminal"
#define DEFAULT_GIF_PATH "/.config/heavy_terminal/default_gif.gif"
#define DEFAULT_WIN_NAME "HeavyNotebook"

namespace HeavyTerminal
{
        bool setup(int argc,char** argv);
        void start();
  
        GtkWidget *gif_image_ = NULL;
        const std::string str_path = std::string(g_get_home_dir()) + DEFAULT_GIF_PATH;
        const char *default_gif_path_ = str_path.c_str();
        void applyStyling();
        void updateGIFPath(const char* filepath);
        void addNewTerminalTab(GtkButton* button, gpointer user_data);
        void onSettingButtonClicked(GtkButton* button, gpointer user_data);
        void onTerminalExit(VteTerminal *terminal, int status, gpointer notebook);
        void onTabCloseButtonClicked(GtkButton *button, gpointer user_data);
        void closeTerminalTab(GtkWidget *terminal_widget, GtkNotebook *notebook);
        gboolean onTerminalKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
};

#endif
