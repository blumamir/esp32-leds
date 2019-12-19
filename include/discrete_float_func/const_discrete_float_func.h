#ifndef __CONST_DISCRETE_FLOAT_FUNC_H__
#define __CONST_DISCRETE_FLOAT_FUNC_H__

#include "i_discrete_float_func.h"

class ConstDiscreteFloatFunc : public IDiscreteFloatFunc {

public:

    void InitFromJson(const JsonObject &func_params, float total) {
        value_to_return = func_params["val"];
    }

    float GetValue(int i) {
        return value_to_return;
    }

private:
    float value_to_return = 0.0;

};

#endif // __CONST_DISCRETE_FLOAT_FUNC_H__