#ifndef __STEP_DIFF_DISCRETE_FLOAT_FUNC_H__
#define __STEP_DIFF_DISCRETE_FLOAT_FUNC_H__

#include "i_discrete_float_func.h"
#include "math.h"

class StepDiffDiscreteFloatFunc : public IDiscreteFloatFunc {

public:

    void InitFromJson(const JsonObject &func_params, float total) {
        this->start = func_params["start"];
        this->dx = func_params["dx"];
    }

    float GetValue(int i) {

        return start + i * dx;
    }

private:
    float start = 0.0;
    float dx = 0.0;
};

#endif // __STEP_DIFF_DISCRETE_FLOAT_FUNC_H__