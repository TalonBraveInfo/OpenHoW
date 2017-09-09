
#include "graphics_private.h"

PLLight *plCreateLight(void) {
    PLLight *light = (PLLight*)malloc(sizeof(PLLight));
    if(!light) {
        plGraphicsLog("Failed to create light!\n");
        return NULL;
    }

    memset(light, 0, sizeof(PLLight));

    pl_graphics_state.num_lights++;
    light->colour   = plCreateColour4b(255, 255, 255, 128.f);
    light->type     = PLLIGHT_TYPE_OMNI;
    return light;
}

void plDeleteLight(PLLight *light) {
    if(!light) {
        return;
    }

    pl_graphics_state.num_lights--;
    free(light);
}

void plDrawLight(PLLight *light) {
    if(light->colour.a <= 0) {
        return;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// Utility

void plSetLightPosition(PLLight *light, PLVector3D position) {
    light->position = position;
}

void plSetLightColour(PLLight *light, PLColour colour) {
    light->colour = colour;
}

void plSetLightType(PLLight *light, PLLightType type) {
    light->type = type;
}