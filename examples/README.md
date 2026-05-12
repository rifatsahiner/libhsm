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

### UML Calculator Example (`uml_calculator/`)

A graphical calculator example that implements the calculator UML state machine (Figure 2.18) with a complete GUI interface:

- **Hierarchical State Machine**: Implements multiple levels of state nesting for calculator operations
- **Event-Driven Architecture**: Handles digit input, operators, decimal points, and special functions through events
- **Error Handling**: Manages division by zero, overflow, and invalid operations with dedicated error states
- **GUI Interface**: Built with FLTK library providing an interactive calculator interface
- **State Persistence**: Maintains calculator state across operations with proper operand handling
- **Input Validation**: Ensures proper number formatting and operation sequencing

#### Running the Example

```bash
./examples/uml_calculator/uml_calculator
```

#### Interface Features

The calculator provides a standard calculator interface with:

- **Digit Buttons**: 0-9 for number input
- **Operator Buttons**: +, -, *, / for arithmetic operations
- **Decimal Point**: . for floating-point numbers
- **Equals**: = to compute results
- **Clear Functions**: C (clear all), CE (clear entry)
- **Display**: Shows current number or result
- **Error Display**: Shows "Error" for invalid operations

#### State Machine Features

The example demonstrates:
1. Initial state handling and number entry
2. Operator precedence and operand storage
3. Decimal number input and formatting
4. Error recovery mechanisms
5. State transitions for different input contexts

The example starts with:
1. Display shows `0`
2. Ready for number input
3. No pending operations

### UML Hypothetical Example (`uml_hypothetical/`)

An interactive example that implements the hypothetical UML state machine that has all possible transitions shown in the provided attachment:

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
