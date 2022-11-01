/*  =================== File Information =================
	File Name: main.cpp
	Description:
	Author: Michael Shah

	Purpose: Driver for 3D program to load .ply models 
	Usage:	
	===================================================== */

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/names.h>
#include <FL/gl.h>
#include <FL/glut.h>
#include <FL/glu.h>

#include "MyGLCanvas.h"

using namespace std;

class MyAppWindow;
MyAppWindow *win;

class MyAppWindow : public Fl_Window {
public:
	Fl_Button* wireButton;
	Fl_Button* paintButton;

	Fl_Slider* redSlider;
	Fl_Slider* greenSlider;
	Fl_Slider* blueSlider;

	MyGLCanvas* canvas;

public:
	// APP WINDOW CONSTRUCTOR
	MyAppWindow(int W, int H, const char*L = 0);

	static void idleCB(void* userdata) {
		win->canvas->redraw();
	}

private:
	// Someone changed one of the sliders
	static void toggleCB(Fl_Widget* w, void* userdata) {
		int value = ((Fl_Button*)w)->value();
		printf("value: %d\n", value);
		*((int*)userdata) = value;
	}

	static void sliderCB(Fl_Widget* w, void* userdata) {
		int value = ((Fl_Slider*)w)->value();
		printf("value: %d\n", value);
		*((float*)userdata) = value;
	}

	static void colorCB(Fl_Widget* w, void* userdata) {
		int red = win->redSlider->value();
		int green = win->greenSlider->value();
		int blue = win->blueSlider->value();
		win->canvas->setColor(red, green, blue);
	}
};


MyAppWindow::MyAppWindow(int W, int H, const char*L) : Fl_Window(W, H, L) {
	begin();
	// OpenGL window

	canvas = new MyGLCanvas(10, 10, w() - 160, h() - 20);

	Fl_Pack* pack = new Fl_Pack(w() - 150, 30, 150, h(), "");
	pack->box(FL_DOWN_FRAME);
	pack->type(Fl_Pack::VERTICAL);
	pack->spacing(0);
	pack->begin();


	wireButton = new Fl_Check_Button(0, 0, pack->w() - 20, 20, "Wireframe");
	wireButton->callback(toggleCB, (void*)(&(canvas->wireframe)));
	wireButton->value(canvas->wireframe);


	paintButton = new Fl_Check_Button(0, 0, pack->w() - 20, 20, "Paint");
	paintButton->callback(toggleCB, (void*)(&(canvas->paint)));
	paintButton->value(canvas->paint);

	//slider for controlling the color
	Fl_Box *redTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Red");
	redSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	redSlider->align(FL_ALIGN_TOP);
	redSlider->type(FL_HOR_SLIDER);
	redSlider->bounds(0, 255);
	redSlider->step(1);
	redSlider->value(canvas->red);
	redSlider->callback(colorCB);

	//slider for controlling the color
	Fl_Box *greenTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Green");
	greenSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	greenSlider->align(FL_ALIGN_TOP);
	greenSlider->type(FL_HOR_SLIDER);
	greenSlider->bounds(0, 255);
	greenSlider->step(1);
	greenSlider->value(canvas->green);
	greenSlider->callback(colorCB);

	//slider for controlling the color
	Fl_Box *blueTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Blue");
	blueSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	blueSlider->align(FL_ALIGN_TOP);
	blueSlider->type(FL_HOR_SLIDER);
	blueSlider->bounds(0, 255);
	blueSlider->step(1);
	blueSlider->value(canvas->blue);
	blueSlider->callback(colorCB);

	end();
}


/**************************************** main() ********************/
int main(int argc, char **argv) {
	//win = new MyAppWindow(800, 500, "Paint!");
	win = new MyAppWindow(660, 520, "Paint!");
	win->resizable(win);
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	return(Fl::run());
}