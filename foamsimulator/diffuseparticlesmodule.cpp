// VisualSPHysics
// Copyright (C) 2020 Orlando Garcia-Feal orlando@uvigo.es

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <Python.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "SimulationParams.h"
#include "DiffuseCalculator.h"

/*
 * This is a simple Python module to run the foam simulation.
 */

extern "C"
{

  static PyObject * diffuseparticles_run(PyObject *self, PyObject *args){
    const char * dataPath, * filePrefix, * outputPath, * outputPreffix, * exclusionZoneFile;
    SimulationParams sp;
    PyObject *touts_list;
    int touts_length;
    TimeOut *touts;


    if(!PyArg_ParseTuple(args, "sssssiiippppdddddddddddddddddddddO",
			 &dataPath,
			 &filePrefix,
			 &outputPath,
			 &outputPreffix,
			 &exclusionZoneFile,
			 &sp.nstart,
			 &sp.nend,
			 &sp.nzeros,
			 &sp.text_files,
			 &sp.vtk_files,
			 &sp.vtk_diffuse_data,
			 &sp.vtk_fluid_data,
			 &sp.h, &sp.mass,// &sp.TIMESTEP,
			 &sp.MINX, &sp.MINY, &sp.MINZ, &sp.MAXX, &sp.MAXY, &sp.MAXZ,
			 &sp.MINTA, &sp.MAXTA, &sp.MINWC, &sp.MAXWC,
			 &sp.MINK, &sp.MAXK, &sp.KTA, &sp.KWC,
			 &sp.SPRAY, &sp.BUBBLES, &sp.LIFEFIME, &sp.KB, &sp.KD,
       &touts_list
			 )){
      return NULL;
    }

    
    sp.dataPath = dataPath;
    sp.filePrefix = filePrefix;
    sp.outputPath = outputPath;
    sp.outputPreffix = outputPreffix;
    sp.exclusionZoneFile = exclusionZoneFile;

    touts_length = PyObject_Length(touts_list);
    if (touts_length < 0)
      return NULL;

    touts = (TimeOut *) malloc(sizeof(TimeOut) * touts_length);
    if (touts == NULL)
      return NULL;

    for (int i = 0; i < touts_length; i++) {
        PyObject *tout_obj;
        tout_obj = PyList_GetItem(touts_list, i);

        PyObject *nstep;
        PyObject *tout;

        nstep = PyObject_GetAttrString(tout_obj, "nstep");
        tout = PyObject_GetAttrString(tout_obj, "tout");

        if (!PyLong_Check(nstep))
            return NULL;
        if (!PyFloat_Check(tout))
            return NULL;

        touts[i] = {.nstep=(int)PyLong_AsLong(nstep), .tout=PyFloat_AsDouble(tout)};
    }

    sp.timesteps = touts;
    sp.ntimesteps = touts_length;
      
    DiffuseCalculator dc(sp);
    dc.runSimulation();
    
    free(touts);
    return Py_True;
  }

  static PyMethodDef DiffuseParticlesMethods[] = {
    {"run", diffuseparticles_run, METH_VARARGS, "Run Diffuse Particles Simulation"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
  };

  static struct PyModuleDef diffuseparticlesmodule = {
    PyModuleDef_HEAD_INIT,
    "diffuseparticles", /* Name of the module */
    NULL, /* Module of documentation */
    -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    DiffuseParticlesMethods
  };

  PyMODINIT_FUNC PyInit_diffuseparticles(void) {
    return PyModule_Create(&diffuseparticlesmodule);
  }
  
}
