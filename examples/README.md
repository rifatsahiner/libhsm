# HSM Library Examples

This directory contains examples demonstrating the usage of the Hierarchical State Machine (HSM) library.

## Building Examples

To build the examples, enable the `HSM_BUILD_EXAMPLES` option when configuring with CMake:

```bash
cd build
cmake -DHSM_BUILD_EXAMPLES=ON ..
make
```

## Available Examples

### Basic Usage (`basic_usage/`)

A comprehensive example demonstrating a traffic light state machine that shows:

- **Custom State Machine Class**: Inherits from `Hsm::StateMachine`
- **Multiple States**: Red, Yellow, Green, and Maintenance states
- **Custom Signals**: Timer events, maintenance requests, etc.
- **Extended Events**: Events with additional data (TimerEvent with duration)
- **Signal-Only Events**: Simple events without extra data
- **State Transitions**: Using `_trans()` to change states
- **Entry/Exit Actions**: Automatic execution during state changes
- **Event Dispatching**: Processing events and observing state changes

#### Running the Example

```bash
./examples/basic_usage/hsm_basic_example
```

The example simulates a traffic light system that:
1. Starts in RED state
2. Transitions through GREEN → YELLOW → RED cycle based on timer events
3. Can enter MAINTENANCE mode from any state
4. Ignores timer events while in maintenance
5. Returns to RED state when maintenance is complete

### UML Oven Example (`uml_oven/`)

An interactive example that implements the oven/toaster UML state machine from the provided drawing:

- **Composite State**: `heating` owns the `toasting` and `baking` substates
- **Sibling State**: `door_open` sits outside `heating`
- **Entry/Exit Actions**: Prints `heater_on()`, `heater_off()`, `arm_time_event()`, `set_temperature()`, and lamp actions
- **Interactive Commands**: Change mode, temperature, toast color, and door position from the terminal
- **Configurable Open-Door Behavior**: Mode, temperature, and toast color can all be changed while the door is open
- **Mode-Aware Door Close**: Closing the door re-enters `heating` and follows its initial transition into the currently selected mode

#### Running the Example

```bash
./examples/uml_oven/hsm_uml_oven_example
```

#### Supported Commands

```text
mode toast
mode bake
temp <celsius>
color light|medium|dark
door open
door close
status
help
quit
```

The example starts with:
1. Door closed
2. Temperature set to `180 C`
3. Toast color set to `medium`
4. Initial transition path `heating -> toasting`

### UML Hypothetical Example (`uml_hypothetical/`)

An interactive example that implements the hypothetical UML state machine shown in the provided attachment:

- **Deep Hierarchy**: Demonstrates `s`, `s1`, `s11`, `s2`, `s21`, and `s211`
- **All Transition Topologies**: Exercises self, sibling, ancestor, descendant, and inherited transitions
- **Guarded Behavior**: Uses the extended state variable `foo` to enable or disable guarded transitions
- **Transition Tracing**: Prints every entry, exit, guard result, nested initial transition, and transition action
- **Interactive Commands**: Lets you inject UML events `A` through `I` directly from the terminal
- **Terminal State**: Includes a dedicated example-only `terminated` state to stand in for the UML final pseudostate

#### Running the Example

```bash
./examples/uml_hypothetical/hsm_uml_hypothetical_example
```

#### Supported Commands

```text
a..i
event <a-i>
terminate
status
help
quit
```

The example starts with:
1. `foo = 0`
2. Initial transition path `top -> s2 -> s21 -> s211`
3. Full trace output for every later event

## API Overview

The HSM library provides:

- `Hsm::StateMachine`: Abstract base class for state machines
- `Hsm::Event`: Base class for events (can be extended with data)
- State handlers: Member functions returning `HandleResult`
- Transition methods: `_trans(target)`, `_super(parent)`
- Event dispatching: `init()`, `dispatch(event)`, `isIn(state)`

See the example source code for detailed usage patterns.
