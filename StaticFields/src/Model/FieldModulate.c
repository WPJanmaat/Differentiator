#include "Parameters.h"
#include "Vector.h"
#include "Field.h"
#include "FieldModulate.h"
#include "Interpolate.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//returns the field strength at the given coordinates.
Vector ModulateField(Field ACField, Field DCField, Vector coordinates, int timestep, Parameters params) {
    Vector output;
    Vector ACForce = interpTriLin(ACField, coordinates);
    Vector DCForce = interpTriLin(DCField, coordinates);
    ACForce = scalarMult(ACForce, params.ACV*sin((timestep*params.dt+params.startTime)*params.freq*2*pi));
    DCForce = scalarMult(DCForce, params.DCV);
    output = vecSum(ACForce, DCForce);
    return output;
}

