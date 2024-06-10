# Primordial Particle Life

A 2D particle physics simulation, modelled in an infinite, that aims to displaying emergent properties akin to Conway's Game of Life, using physics in a continuous domain.

Once upon a time, this was my 3rd year final project, now it's actually just my project, my Progress Report and Final Report are attached as part of the documentation. Though now that I'm no longer working in an academic context, they'll soon be outdated - no more arbuitary justifications, I can just have fun with my toy!


## Things You Should Know

### No Delta-Time Coefficient

To keep results deterministic, the time frame of the simulation is dependant on the framerate. It can be standardised by capping the framerate.

### Force Applied to Particle Velocities

![Particle Physics](/docs/particle-motion-law.png)

The equation works with arrays of properties, instead of an array of particle objects. I didn't see how I could feasibly denote a class/object with mathematical notation.

Don't be too intimidated if it seems complicated. Depending on your background, the actual code implementation is most likely far easier to follow!

### Iminent Changes

* Format for settings files will be changed to JSON *very* soon.
* Launcher reopening on exiting a simulation
* Parameters for frame limit and statistical analysis/visualisations
* GUI integration for statistics module
* Help tab to show keybindings and explain parameters
* Thread pool, 
* Issue tracker will actually be used/updated


## Run/Debug the Project

### Core Application

Provided you're also working with vscode, and you have raylib setup with w64devkit. It should be as simple as cloning the repository, opening it in vscode and pressing F5.

I've used a vscode template as a foundation. I have modified it to use C++17 instead of C++11, and I've added an extra profile for the Python statistics module. It's only been tested with Windows 10 and 11. See the link below for the original template.

<p align="center">
    <a href="https://github.com/educ8s/Raylib-CPP-Starter-Template-for-VSCODE-V2/">educ8s's Original Template</a>
</p>

### Statistics Module

As I've mentioned above, for vscode, the tasks.json has a profile for debuggig the statistics module. It was written in Python 3.11.9, though will most likely work in any release from the last five years or so. If you're using any other tooling, note that the current working directory will have to be the statistics directory.


## Interesting Examples

