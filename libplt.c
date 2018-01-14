#include <stdio.h>
#include <stdlib.h>
#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>
#include <numpy/npy_3kcompat.h> // PyUnicode_FromString
	
// global import objects
PyObject* pyplot;

// global function objects
PyObject *s_python_function_plot;
PyObject *s_python_function_show;
PyObject *s_python_function_legend;

// global formatting options, for now
PyObject *color_red;
PyObject *color_green;
PyObject *color_blue;

PyObject *kwarg_label;
PyObject *kwarg_linestyle;
PyObject *kwarg_marker;

PyObject *kwarg_alpha;
PyObject *kwarg_color;
PyObject *kwarg_lw;
PyObject *kwarg_ms;
PyObject *kwarg_zorder;

PyObject *kwarg_fmt;

void init() {
	printf("Initializing python\n"); fflush(stdout);
	Py_Initialize(); // init python
	_import_array(); // init numpy

	// import matplotlib
	PyObject* pyplotname = PyUnicode_FromString("matplotlib.pyplot");
	pyplot = PyImport_Import(pyplotname);
	Py_XDECREF(pyplotname);

	// get functions
	s_python_function_plot = PyObject_GetAttrString(pyplot, "plot");
	s_python_function_legend = PyObject_GetAttrString(pyplot, "legend");
	s_python_function_show = PyObject_GetAttrString(pyplot, "show");
}

void plot(double *x, double *y, int n, char *fmt, char *label, char *linestyle, char *marker, double *alpha, double *color, double *lw, double *ms, double *zorder) {
	// get PyObjects corresponding to x and y array objects from numpy and set the data 
	// Note: numpy doesn't own the data, so no copying is done
	npy_intp vsize = n;
	PyObject* xarray = PyArray_SimpleNewFromData(1, &vsize, NPY_DOUBLE, (void*)&x[0]);
	PyObject* yarray = PyArray_SimpleNewFromData(1, &vsize, NPY_DOUBLE, (void*)&y[0]);

	// construct plot args
	PyObject* args_plot = PyTuple_New(fmt ? 3 : 2);
	PyTuple_SetItem(args_plot, 0, xarray);
	PyTuple_SetItem(args_plot, 1, yarray);
	if (fmt) {
		kwarg_fmt = PyUnicode_FromString(fmt);
		PyTuple_SetItem(args_plot, 2, kwarg_fmt);
	}

	// construct kwargs
	PyObject* kwargs_plot = PyDict_New();
	if (label) {
		kwarg_label = PyUnicode_FromString(label);
		PyDict_SetItemString(kwargs_plot, "label", kwarg_label);
	}
	if (linestyle) {
		kwarg_linestyle = PyUnicode_FromString(linestyle);
		PyDict_SetItemString(kwargs_plot, "linestyle", kwarg_linestyle);
	}
	if (marker) {
		kwarg_marker = PyUnicode_FromString(marker);
		PyDict_SetItemString(kwargs_plot, "marker", kwarg_marker);
	}

	if (alpha) {
		kwarg_alpha = PyFloat_FromDouble(*alpha);
		PyDict_SetItemString(kwargs_plot, "alpha", kwarg_alpha);
	}
	if (color) {

		color_red   = PyFloat_FromDouble(color[0]);
		color_green = PyFloat_FromDouble(color[1]);
		color_blue  = PyFloat_FromDouble(color[2]);

		kwarg_color = PyTuple_New(3);
		PyTuple_SetItem(kwarg_color, 0, color_red);
		PyTuple_SetItem(kwarg_color, 1, color_green);
		PyTuple_SetItem(kwarg_color, 2, color_blue);

		PyDict_SetItemString(kwargs_plot, "color", kwarg_color);
	}
	if (lw) {
		kwarg_lw = PyFloat_FromDouble(*lw);
		PyDict_SetItemString(kwargs_plot, "lw", kwarg_lw);
	}
	if (ms) {
		kwarg_ms = PyFloat_FromDouble(*ms);
		PyDict_SetItemString(kwargs_plot, "ms", kwarg_ms);
		Py_XDECREF(kwarg_lw);
	}
	if (zorder) {
		kwarg_zorder = PyFloat_FromDouble(*zorder);
		PyDict_SetItemString(kwargs_plot, "zorder", kwarg_zorder);
	}

	// call plot function
	PyObject* ret_plot = PyObject_Call(s_python_function_plot, args_plot, kwargs_plot);

	// cleanup references
	Py_XDECREF(args_plot);
	Py_XDECREF(kwargs_plot);
	Py_XDECREF(ret_plot);

	Py_XDECREF(xarray);
	Py_XDECREF(yarray);

}

void show() {
	// construct show args
	PyObject* args_show = PyTuple_New(0);

	// call show function
	PyObject* ret_show = PyObject_Call(s_python_function_show, args_show, NULL);

	Py_XDECREF(args_show);
	Py_XDECREF(ret_show);
}

void legend() {
	// construct show args
	PyObject* args_show = PyTuple_New(0);

	// call show function
	PyObject* ret_show = PyObject_Call(s_python_function_legend, args_show, NULL);

	Py_XDECREF(args_show);
	Py_XDECREF(ret_show);	
}

void simple_string(char *command) {
	PyRun_SimpleString(command);
}

void finish() {
	Py_XDECREF(kwarg_marker);
	Py_XDECREF(kwarg_lw);
	Py_XDECREF(kwarg_color);

	Py_XDECREF(color_red);
	Py_XDECREF(color_green);
	Py_XDECREF(color_blue);

	Py_XDECREF(s_python_function_plot);
	Py_XDECREF(s_python_function_show);
	
	Py_XDECREF(pyplot);

	Py_Finalize();
}