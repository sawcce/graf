#define SOKOL_IMPL

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "sokol_time.h"

#include "scene.h"
#include "log.h"

sg_pass_action pass_action;
uint64_t current_time;
uint64_t delta_time;
float_t refresh_rate;

char *window_title;

void init(void)
{
    log_info("Initiating setup");
    stm_setup();

    uint64_t time = stm_now();
    log_debug("Set time to: %d", time);
    current_time = time;

    window_title = malloc(50 * sizeof(char));

    sg_setup(&(sg_desc){
        .context = sapp_sgcontext(),
        .logger.func = slog_func,
    });
    pass_action = (sg_pass_action){
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .store_action = SG_STOREACTION_DONTCARE,
            .clear_value = 1.0f,
        },
        .colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {1.0f, 0.0f, 0.0f, 1.0f}}};

    scene_setup();

    log_info("Finished with setup");
}

void frame(void)
{
    delta_time = stm_laptime(&current_time);
    float_t new_refresh_rate = 1.0f / stm_sec(delta_time);

    if ((int64_t)new_refresh_rate != (int64_t)refresh_rate)
    {
        refresh_rate = new_refresh_rate;
        sprintf_s(window_title, 50 * sizeof(window_title), "DJI | FPS: %d", (int64_t)refresh_rate);
        sapp_set_window_title(window_title);
    }

    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    scene_draw();
    sg_end_pass();
    sg_commit();
}

void cleanup(void)
{
    free(window_title);
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