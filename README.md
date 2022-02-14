# CAD-Application

A simple Command Line CAD clone made using C++ and the olcPixelGameEngine library. 
The purpose of this project was to learn low level language and it's features. 
The project is a part of a Youtube tutorial aimed to explain Polymorphysm.
I used it to learn more about the programming language C++ and some of it's features. 

![CAD_ToolBox_Off](https://user-images.githubusercontent.com/99598013/153961692-aff1ff30-cf25-438c-8157-920c4ec786e9.png)


I did some modifications from the orginal code: 

  - Adding measure mode
  - Adding a Toolbox that the user can toggle on and off 
  - Adding displays for the current mode and current shape

### Language used
  - C++

### Libraries used
  - olcPixelGameEngine
  - iostream
  - std library 

### What was explored in the project? 
  - Structures
  - Polymorphism
  - Pointers 
  - Reference and Value types

### Summary: 
This simple CAD application on Windows allows the user to draw,edit,erase and measure different types of 2D shappes.
  There are 3 modes that the user can switch from:
    - Draw mode (button D)
    - Edit mode (button E)
    - Measure mode (button M)
    
In Draw mode the user can choose from 6 different 2D shapes:
  - Line (button L)
  - Box  (button B)
  - Triangle  (button T)
  - Circle  (button C)
  - Spline/Curve  (button S)
  - Axis Line (button A)
 
 It also allows the user to navigate through the 2D word (panning) and to zoom in and out 
  - Panning (Hold Z and drag with the mouse)
  - Zoom (Mouse Scroll Wheel)
  - Toggle ToolBox On and Off (button Shift)


![CAD](https://user-images.githubusercontent.com/99598013/153961845-511cbe2a-1a9f-4860-bbbf-3db9e84729c4.png)


### Code Summary
The project was created with the help of a drawing library - olcPixelGameEngine. 
The different shapes were made with the use of a Object Orientated feature - Polymorphysm.

There is one parent structure - sShape, and all of the different shapes derive from it. 
This makes it possible to add all of the shapes into a single collection (list or a vector) and manipulate them easily. 
In order to accomplish this, the main class 'sShape' is created as a pointer and when a specific shape is being created, the 'new' keyword is used.
This way all of the different shapes are grouped in one collection of type 'sShape'. 



  Copyright 2018-2019 OneLoneCoder.com

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
