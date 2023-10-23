#define SOKOL_IMPL

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_log.h"
#include "sokol_glue.h"

#include "scene.h"

sg_pass_action pass_action;

void init(void)
{
    printf("Setup\n");

    sg_setup(&(sg_desc){
        .context = sapp_sgcontext(),
        .logger.func = slog_func,
    });
    pass_action = (sg_pass_action){
        .colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {1.0f, 0.0f, 0.0f, 1.0f}}};

    scene_setup();
}

void frame(void)
{
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    scene_draw();
    sg_end_pass();
    sg_commit();
}

void cleanup(void)
{
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char *argv[])
{
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 400,
        .height = 300,
        .window_title = "DJI",
        .logger.func = slog_func,
        .win32_console_attach = true,
    };
}