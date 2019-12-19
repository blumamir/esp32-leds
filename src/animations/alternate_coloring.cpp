#include <animations/alternate_coloring.h>

AlternateColoringAnimation::~AlternateColoringAnimation() {

}

void AlternateColoringAnimation::InitFromJson(const JsonObject &animation_params) {

    JsonVariant hsv1 = animation_params["hsv1"];
    JsonVariant hsv2 = animation_params["hsv2"];

    color1 = HSV(hsv1.as<JsonArray>());
    color2 = HSV(hsv2.as<JsonArray>());

    numberOfPixels = animation_params["numPix"];
}

void AlternateColoringAnimation::Render(float rel_time, int cycle_index) {

    int i=0;
    for(std::vector<HSV *>::const_iterator it = pixels->begin(); it != pixels->end(); ++it) {

        HSV *pixel = *it;

        bool pixelState = (i % (2 * numberOfPixels)) < numberOfPixels;
        if(pixelState) {
            // TODO: this can be done better
            pixel->hue = color1.hue;
            pixel->sat = color1.sat;
            pixel->val = color1.val;
        }
        else {
            pixel->hue = color2.hue;
            pixel->sat = color2.sat;
            pixel->val = color2.val;
        }

        i++;
    }
}
