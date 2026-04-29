//
// Created by novakdominikviktor on 2026. 02. 28..
//

#include "app.h"

int main(int argc, char* args[]) {
    (void)argc; (void)args;

    App app;
    if (!app_init(&app)) return 1;

    while (app.running) {
        app_handle_events(&app);
        app_render(&app);
    }

    app_quit(&app);
    return 0;
}