# Testing Plan
We want to both:
- have comprehensive unit testing, off arduino
- have comprehensive testing, on arduino
- have a way to simulate the BCU live (since parts are delayed).

Two of these require us to 'simulate' the arduino's interface on the BCU. Currently this code is in bcu_simulation.cpp, but will be split into fake_arduino.h and bcu_simulation.cpp

For testing, below is a list of what needs to be implemented. Not sure if we should use something like GoogleTest or if we'd rather just throw together something quick like BICA has. We'll need to do our own thing on the arduino anyways.

List of things to test:
- Collision Avoidance
- - Takeover and stop (Unit and Sim)
- - Stuttering and other issues (Sim)
- - Individual axes blocked (Unit and Sim)

- Safety
- -
