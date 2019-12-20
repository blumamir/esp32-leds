#include <discrete_float_func/i_discrete_float_func.h>

#include <discrete_float_func/const_discrete_float_func.h>
#include <discrete_float_func/step_target_discrete_float_func.h>
#include <discrete_float_func/step_diff_discrete_float_func.h>

IDiscreteFloatFunc *DiscreteFloatFuncFactory(const JsonObject &func_params, float total) {

    const char *func_type = func_params["t"];
    if(func_type == NULL) {
        return NULL;
    }

    IDiscreteFloatFunc *func = NULL;
    if(strcmp(func_type, "const") == 0) {
        func = new ConstDiscreteFloatFunc();
    } else if(strcmp(func_type, "step_target") == 0) {
        func = new StepTargetDiscreteFloatFunc();
    } else if(strcmp(func_type, "step_diff") == 0) {
        func = new StepDiffDiscreteFloatFunc();
    } else if(strcmp(func_type, "segment") == 0) {
        // func = new SegmentFloatFunc();
    }

    if(func != NULL) {
        func->InitFromJson(func_params, total);
    }

    return func;
}


