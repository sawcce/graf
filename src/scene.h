#pragma once

#include "sokol_app.h"

#define i_key sapp_event *
#define i_type AppEventQueue
#include "stc/cqueue.h"

void scene_setup();
void scene_draw();
void scene_destroy();
void event(sapp_event *event);