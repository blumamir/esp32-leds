#ifndef __STEP_TARGET_DISCRETE_FLOAT_FUNC_H__
#define __STEP_TARGET_DISCRETE_FLOAT_FUNC_H__

#include "i_discrete_float_func.h"
#include "math.h"

class StepTargetDiscreteFloatFunc : public IDiscreteFloatFunc {

public:

    void InitFromJson(const JsonObject &func_params, float total) {
        this->start = func_params["start"];

        float total_ceil = ceil(total);
        if(total_ceil <= 1)
        {
            this->dx = 0.0;
        }
        else
        {
            float end = func_params["end"];
            this->dx = (end - this->start) / (total_ceil - 1.0);
        }
    }

    float GetValue(int i) {

        return start + i * dx;
    }

private:
    float start = 0.0;
    float dx = 0.0;
};

#endif // __STEP_TARGET_DISCRETE_FLOAT_FUNC_H__