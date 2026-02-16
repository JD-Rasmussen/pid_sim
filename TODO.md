- Project TODO list (structured)
- Phase 0 – Project setup
- [x] Pick stack: C++ + Qt Widgets (or another GUI framework).
- [x] Create a new project and verify:
- [x] A window opens.
- [x] A timer ticks (e.g., prints to console every 100 ms).
- Phase 1 – Core PID block
- Goal: A reusable PID “function block” with no GUI.

- [ ] Define a PIDController class:
- [x] Members: Kp, Ki, Kd, integral, prev_error, maybe output_min/max.
- [x] Constructor to set gains and limits.
- [x] reset() method (zero integral, prev_error).
- [x] update(setpoint, measurement, dt) method that returns control output.
- [x] Implement:
- [x] P term: Kp * error.
- [x] I term with anti‑windup (clamp integral or conditional integration).
- [x] D term, probably on measurement or error, with basic filtering if you like.
- [x] implement gTest
- [x] test that P is correct
- [x] test that I is correct
- [x] test that D is correct
- [x] test limits

- Goal: process model

- [ ] Create a ProcessModel interface / base object:
- [ ] it should return a PV take
- [ ] host sub models that simulate behaivoure
- [ ] selection of sub models
- [ ] water tank level, with variable tank size and varying water consumption


- [ ] second sub module
- [ ] steel plate being heated
- [ ] variable heating element size
- [ ] variable steel plate size and heat spread
- [ ] variable insulation level of the plate, or some other way to simulate disturbances in the process













- Goal: Same logic as console, but user‑friendly.

- [ ] GUI inputs:
- [x] Fields/spin boxes for Kp, Ki, Kd, setpoint.
- [x] Dropdown to select process model (heating vs tank, etc.).
- [ ] Start/Stop/Reset buttons.
- [ ] Simulation control:
- [x] Use a QTimer to call stepSimulation() at fixed dt.
- [ ] Each step:
- [ ] Call PID.update(SP, PV, dt).
- [ ] Call ProcessModel.update(output, dt).
- [ ] Store PV and output (for display).
- [ ] Basic 2D visualization:
- [ ] For tank: draw rectangle + fill height proportional to level.
- [ ] For heater: draw a block whose color depends on temperature.
- [ ] Trigger repaint on each timer tick.

- Goal: Richer control structures once basics work.

- 4.1 Cascade control
- [ ] Design structure:
- [ ] Primary PID: controls “outer” variable (e.g., tank level).
- [ ] Secondary PID: controls “inner” variable (e.g., flow/valve position or temperature).
- [ ] Implement a CascadeController:
- [ ] Primary output becomes secondary setpoint.
- [ ] Secondary output drives the process.
- [ ] Add GUI switches:
- [ ] Mode: single loop vs cascade.
- [ ] Show both loops’ values (PV/SP for each).
- 4.2 Feedforward
- [ ] Add an optional feedforward term:
- [ ] e.g. u = PID_output + FF * measured_disturbance.
- [ ] In simulation, create a disturbance input (e.g., step in outflow, change in ambient temperature).
- [ ] GUI options:
- [ ] Enable/disable feedforward.
- [ ] Adjust FF gain.
- [ ] Trigger disturbance events and visualize effect.

- 5.1 Input filtering
- [ ] Implement a simple filter block:
- [ ] Moving average, or
- [ ] First‑order low‑pass: y += alpha * (x - y).
- [ ] Allow:
- [ ] Raw PV → optional filter → PID.
- [ ] Tunable filter strength (alpha or window size).
- 5.2 Nasty process behavior
- [ ] Add noise to PV:

- [ ] Gaussian or uniform random component.
- [ ] Add dead time (transport delay) to some models if desired.

- [ ] Possibly add non‑linearities:

- [ ] Saturations (valve max/min).
- [ ] Different gains in different regions.
- [ ] GUI toggles:

- [ ] Enable/disable noise.
- [ ] Enable/disable delay.
- [ ] Adjust noise amplitude.
- Phase 6 – Polish and packaging
- [ ] Clean up code into logical modules/files.
- [ ] Add basic labels, units, tooltips in the UI.
- [ ] Preconfigure a few example scenarios with presets.
- [ ] Build release .exe:
- [ ] Verify it runs on a “clean” machine if you can test that.
