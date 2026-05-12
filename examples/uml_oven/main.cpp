#include <iostream>
#include <sstream>
#include <string>

#include "state_machine.h"

using namespace std;
using namespace Hsm;

enum CustomSignals : SignalId {
    DO_TOASTING_SIG = 4,
    DO_BAKING_SIG,
    DOOR_OPEN_SIG,
    DOOR_CLOSE_SIG,
    SET_TEMPERATURE_SIG,
    SET_TOAST_COLOR_SIG
};

enum class ToastColor {
    Light,
    Medium,
    Dark
};

enum class HeatingMode {
    Toast,
    Bake
};

struct SetTemperatureEvent : public Event {
    int celsius;

    explicit SetTemperatureEvent(int value)
        : Event(SET_TEMPERATURE_SIG), celsius(value) {}
};

struct SetToastColorEvent : public Event {
    ToastColor color;

    explicit SetToastColorEvent(ToastColor value)
        : Event(SET_TOAST_COLOR_SIG), color(value) {}
};

class OvenStateMachine : public StateMachine {
public:
    OvenStateMachine()
        : _temperature(180),
          _toastColor(ToastColor::Medium),
          _selectedMode(HeatingMode::Toast) {}

    int temperature() const { return _temperature; }

    const char* toastColorName() const {
        return toToastColorName(_toastColor);
    }

    const char* activeLeafName() {
        if (isIn(static_cast<State>(&OvenStateMachine::doorOpen))) {
            return "door_open";
        }
        if (isIn(static_cast<State>(&OvenStateMachine::baking))) {
            return "baking";
        }
        return "toasting";
    }

    const char* doorPositionName() {
        return isDoorOpen() ? "open" : "closed";
    }

    const char* selectedModeName() {
        return toHeatingModeName(_selectedMode);
    }

    bool isDoorOpen() {
        return isIn(static_cast<State>(&OvenStateMachine::doorOpen));
    }

private:
    HandleResult heating(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                heaterOn();
                return _handled();

            case EXIT_SIG:
                heaterOff();
                return _handled();

            case INIT_SIG:
                return _selectedMode == HeatingMode::Bake
                    ? _trans(&OvenStateMachine::baking)
                    : _trans(&OvenStateMachine::toasting);

            case DOOR_OPEN_SIG:
                return _trans(&OvenStateMachine::doorOpen);

            default:
                return _super(&OvenStateMachine::top);
        }
    }

    HandleResult toasting(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                armTimeEvent(_toastColor);
                return _handled();

            case EXIT_SIG:
                disarmTimeEvent();
                return _handled();

            case DO_BAKING_SIG:
                _selectedMode = HeatingMode::Bake;
                return _trans(&OvenStateMachine::baking);

            case SET_TOAST_COLOR_SIG: {
                const auto& colorEvent = event_cast<SetToastColorEvent>(event);
                _toastColor = colorEvent.color;
                cout << "[CONFIG] Toast color updated to " << toastColorName() << '\n';
                return _trans(&OvenStateMachine::toasting);
            }

            case SET_TEMPERATURE_SIG: {
                const auto& temperatureEvent = event_cast<SetTemperatureEvent>(event);
                _temperature = temperatureEvent.celsius;
                cout << "[CONFIG] Temperature stored at " << _temperature
                     << " C for baking mode\n";
                return _handled();
            }

            default:
                return _super(&OvenStateMachine::heating);
        }
    }

    HandleResult baking(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                setTemperature(_temperature);
                return _handled();

            case EXIT_SIG:
                setTemperature(0);
                return _handled();

            case DO_TOASTING_SIG:
                _selectedMode = HeatingMode::Toast;
                return _trans(&OvenStateMachine::toasting);

            case SET_TEMPERATURE_SIG: {
                const auto& temperatureEvent = event_cast<SetTemperatureEvent>(event);
                _temperature = temperatureEvent.celsius;
                cout << "[CONFIG] Baking temperature updated to " << _temperature << " C\n";
                return _trans(&OvenStateMachine::baking);
            }

            case SET_TOAST_COLOR_SIG: {
                const auto& colorEvent = event_cast<SetToastColorEvent>(event);
                _toastColor = colorEvent.color;
                cout << "[CONFIG] Toast color stored at " << toastColorName()
                     << " for toasting mode\n";
                return _handled();
            }

            default:
                return _super(&OvenStateMachine::heating);
        }
    }

    HandleResult doorOpen(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                internalLampOn();
                return _handled();

            case EXIT_SIG:
                internalLampOff();
                return _handled();

            case DOOR_CLOSE_SIG:
                return _trans(&OvenStateMachine::heating);

            case DO_TOASTING_SIG:
                _selectedMode = HeatingMode::Toast;
                cout << "[CONFIG] Heating mode set to toast. Will resume toasting when the door closes.\n";
                return _handled();

            case DO_BAKING_SIG:
                _selectedMode = HeatingMode::Bake;
                cout << "[CONFIG] Heating mode set to bake. Will resume baking when the door closes.\n";
                return _handled();

            case SET_TEMPERATURE_SIG: {
                const auto& temperatureEvent = event_cast<SetTemperatureEvent>(event);
                _temperature = temperatureEvent.celsius;
                cout << "[CONFIG] Temperature stored at " << _temperature
                     << " C while the door is open\n";
                return _handled();
            }

            case SET_TOAST_COLOR_SIG: {
                const auto& colorEvent = event_cast<SetToastColorEvent>(event);
                _toastColor = colorEvent.color;
                cout << "[CONFIG] Toast color stored at " << toastColorName()
                     << " while the door is open\n";
                return _handled();
            }

            default:
                return _super(&OvenStateMachine::top);
        }
    }

    HandleResult _topInitialTrans(void) override {
        cout << "[SYSTEM] Initial transition -> heating\n";
        return _trans(&OvenStateMachine::heating);
    }

    static const char* toToastColorName(ToastColor color) {
        switch (color) {
            case ToastColor::Light:
                return "light";
            case ToastColor::Medium:
                return "medium";
            case ToastColor::Dark:
                return "dark";
        }

        return "unknown";
    }

    static const char* toHeatingModeName(HeatingMode mode) {
        switch (mode) {
            case HeatingMode::Toast:
                return "toast";
            case HeatingMode::Bake:
                return "bake";
        }

        return "unknown";
    }

    void heaterOn() const {
        cout << "[ACTION] heater_on()\n";
    }

    void heaterOff() const {
        cout << "[ACTION] heater_off()\n";
    }

    void armTimeEvent(ToastColor color) const {
        cout << "[ACTION] arm_time_event(" << toToastColorName(color) << ")\n";
    }

    void disarmTimeEvent() const {
        cout << "[ACTION] disarm_time_event()\n";
    }

    void setTemperature(int value) const {
        cout << "[ACTION] set_temperature(" << value << ")\n";
    }

    void internalLampOn() const {
        cout << "[ACTION] internal_lamp_on()\n";
    }

    void internalLampOff() const {
        cout << "[ACTION] internal_lamp_off()\n";
    }

    int _temperature;
    ToastColor _toastColor;
    HeatingMode _selectedMode;
};

static ToastColor parseToastColor(const string& value, bool& ok) {
    ok = true;

    if (value == "light") {
        return ToastColor::Light;
    }
    if (value == "medium") {
        return ToastColor::Medium;
    }
    if (value == "dark") {
        return ToastColor::Dark;
    }

    ok = false;
    return ToastColor::Medium;
}

static void printHelp() {
    cout << "Commands:\n"
         << "  mode toast          Switch to toasting mode\n"
         << "  mode bake           Switch to baking mode\n"
         << "  temp <celsius>      Update baking temperature\n"
         << "  color light|medium|dark\n"
         << "                      Update toast color\n"
         << "  door open           Open the door\n"
         << "  door close          Close the door and resume the selected mode\n"
         << "  status              Show current state and settings\n"
         << "  help                Show this help text\n"
         << "  quit                Exit the example\n";
}

static void printStatus(OvenStateMachine& oven) {
    cout << "[STATUS] state=" << oven.activeLeafName()
         << ", door=" << oven.doorPositionName()
         << ", mode=" << oven.selectedModeName()
         << ", temp=" << oven.temperature() << " C"
         << ", color=" << oven.toastColorName() << '\n';
}

int main() {
    cout << "==============================================================\n"
         << "              UML OVEN / TOASTER STATE MACHINE\n"
         << "==============================================================\n"
         << "Defaults: temperature=180 C, toast_color=medium, door=closed\n\n";

    OvenStateMachine oven;
    oven.init();
    printStatus(oven);
    cout << '\n';
    printHelp();
    cout << '\n';

    string line;
    while (true) {
        cout << "\n> ";
        if (!getline(cin, line)) {
            cout << "\nInput closed. Exiting.\n";
            break;
        }

        istringstream input(line);
        string command;
        input >> command;

        if (command.empty()) {
            continue;
        }

        if (command == "quit") {
            cout << "Exiting oven example.\n";
            break;
        }

        if (command == "help") {
            printHelp();
            printStatus(oven);
            continue;
        }

        if (command == "status") {
            printStatus(oven);
            continue;
        }

        if (command == "mode") {
            string mode;
            input >> mode;

            if (mode != "toast" && mode != "bake") {
                cout << "[ERROR] Use `mode toast` or `mode bake`.\n";
                continue;
            }

            if (mode == "toast") {
                if (!oven.isDoorOpen() && string(oven.activeLeafName()) == "toasting") {
                    cout << "[INFO] Already in toasting mode.\n";
                } else {
                    Event event(DO_TOASTING_SIG);
                    oven.dispatch(event);
                }
            } else {
                if (!oven.isDoorOpen() && string(oven.activeLeafName()) == "baking") {
                    cout << "[INFO] Already in baking mode.\n";
                } else {
                    Event event(DO_BAKING_SIG);
                    oven.dispatch(event);
                }
            }

            printStatus(oven);
            continue;
        }

        if (command == "temp") {
            int value = 0;
            if (!(input >> value) || value < 0) {
                cout << "[ERROR] Temperature must be a non-negative integer.\n";
                continue;
            }

            SetTemperatureEvent event(value);
            oven.dispatch(event);
            printStatus(oven);
            continue;
        }

        if (command == "color") {
            string colorValue;
            input >> colorValue;

            bool ok = false;
            ToastColor color = parseToastColor(colorValue, ok);
            if (!ok) {
                cout << "[ERROR] Toast color must be light, medium, or dark.\n";
                continue;
            }

            SetToastColorEvent event(color);
            oven.dispatch(event);
            printStatus(oven);
            continue;
        }

        if (command == "door") {
            string position;
            input >> position;

            if (position == "open") {
                if (oven.isDoorOpen()) {
                    cout << "[INFO] Door is already open.\n";
                } else {
                    Event event(DOOR_OPEN_SIG);
                    oven.dispatch(event);
                }
                printStatus(oven);
                continue;
            }

            if (position == "close") {
                if (!oven.isDoorOpen()) {
                    cout << "[INFO] Door is already closed.\n";
                } else {
                    Event event(DOOR_CLOSE_SIG);
                    oven.dispatch(event);
                }
                printStatus(oven);
                continue;
            }

            cout << "[ERROR] Use `door open` or `door close`.\n";
            continue;
        }

        cout << "[ERROR] Unknown command. Type `help` for supported commands.\n";
    }

    return 0;
}
