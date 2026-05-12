#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include "state_machine.h"

using namespace std;
using namespace Hsm;

enum CustomSignals : SignalId {
    A_SIG = 4,
    B_SIG,
    C_SIG,
    D_SIG,
    E_SIG,
    F_SIG,
    G_SIG,
    H_SIG,
    I_SIG,
    TERMINATE_SIG
};

class HypotheticalStateMachine : public StateMachine {
public:
    HypotheticalStateMachine()
        : _foo(0) {}

    int fooValue() const {
        return _foo;
    }

    bool isTerminated() {
        return isIn(static_cast<State>(&HypotheticalStateMachine::terminated));
    }

    const char* activeLeafName() {
        if (isTerminated()) {
            return "terminated";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s211))) {
            return "s211";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s11))) {
            return "s11";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s21))) {
            return "s21";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s2))) {
            return "s2";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s1))) {
            return "s1";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s))) {
            return "s";
        }
        return "unknown";
    }

    string activePathString() {
        if (isTerminated()) {
            return "terminated";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s211))) {
            return "s > s2 > s21 > s211";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s11))) {
            return "s > s1 > s11";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s21))) {
            return "s > s2 > s21";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s2))) {
            return "s > s2";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s1))) {
            return "s > s1";
        }
        if (isIn(static_cast<State>(&HypotheticalStateMachine::s))) {
            return "s";
        }
        return "unknown";
    }

private:
    HandleResult s(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                logEntry("s");
                return _handled();

            case EXIT_SIG:
                logExit("s");
                return _handled();

            case INIT_SIG:
                logInit("s", "s1");
                return _trans(&HypotheticalStateMachine::s1);

            case E_SIG:
                logEvent("s", "E");
                return _trans(&HypotheticalStateMachine::s11);

            case I_SIG:
                logEvent("s", "I");
                if (_foo != 0) {
                    logGuard("s", "I", "foo", true);
                    setFoo(0);
                    return _handled();
                }
                logGuard("s", "I", "foo", false);
                return _super(&HypotheticalStateMachine::top);

            case TERMINATE_SIG:
                logEvent("s", "TERMINATE");
                return _trans(&HypotheticalStateMachine::terminated);

            default:
                return _super(&HypotheticalStateMachine::top);
        }
    }

    HandleResult s1(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                logEntry("s1");
                return _handled();

            case EXIT_SIG:
                logExit("s1");
                return _handled();

            case INIT_SIG:
                logInit("s1", "s11");
                return _trans(&HypotheticalStateMachine::s11);

            case A_SIG:
                logEvent("s1", "A");
                return _trans(&HypotheticalStateMachine::s1);

            case B_SIG:
                logEvent("s1", "B");
                return _trans(&HypotheticalStateMachine::s11);

            case C_SIG:
                logEvent("s1", "C");
                return _trans(&HypotheticalStateMachine::s2);

            case D_SIG:
                logEvent("s1", "D");
                if (_foo == 0) {
                    logGuard("s1", "D", "!foo", true);
                    setFoo(1);
                    return _trans(&HypotheticalStateMachine::s);
                }
                logGuard("s1", "D", "!foo", false);
                return _super(&HypotheticalStateMachine::s);

            case F_SIG:
                logEvent("s1", "F");
                return _trans(&HypotheticalStateMachine::s211);

            case I_SIG:
                logEvent("s1", "I");
                return _handled();

            default:
                return _super(&HypotheticalStateMachine::s);
        }
    }

    HandleResult s11(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                logEntry("s11");
                return _handled();

            case EXIT_SIG:
                logExit("s11");
                return _handled();

            case D_SIG:
                logEvent("s11", "D");
                if (_foo != 0) {
                    logGuard("s11", "D", "foo", true);
                    setFoo(0);
                    return _trans(&HypotheticalStateMachine::s1);
                }
                logGuard("s11", "D", "foo", false);
                return _super(&HypotheticalStateMachine::s1);

            case G_SIG:
                logEvent("s11", "G");
                return _trans(&HypotheticalStateMachine::s211);

            case H_SIG:
                logEvent("s11", "H");
                return _trans(&HypotheticalStateMachine::s);

            default:
                return _super(&HypotheticalStateMachine::s1);
        }
    }

    HandleResult s2(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                logEntry("s2");
                return _handled();

            case EXIT_SIG:
                logExit("s2");
                return _handled();

            case INIT_SIG:
                logInit("s2", "s21");
                return _trans(&HypotheticalStateMachine::s21);

            case C_SIG:
                logEvent("s2", "C");
                return _trans(&HypotheticalStateMachine::s1);

            case F_SIG:
                logEvent("s2", "F");
                return _trans(&HypotheticalStateMachine::s11);

            case I_SIG:
                logEvent("s2", "I");
                if (_foo == 0) {
                    logGuard("s2", "I", "!foo", true);
                    setFoo(1);
                    return _handled();
                }
                logGuard("s2", "I", "!foo", false);
                return _super(&HypotheticalStateMachine::s);

            default:
                return _super(&HypotheticalStateMachine::s);
        }
    }

    HandleResult s21(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                logEntry("s21");
                return _handled();

            case EXIT_SIG:
                logExit("s21");
                return _handled();

            case INIT_SIG:
                logInit("s21", "s211");
                return _trans(&HypotheticalStateMachine::s211);

            case A_SIG:
                logEvent("s21", "A");
                return _trans(&HypotheticalStateMachine::s21);

            case B_SIG:
                logEvent("s21", "B");
                return _trans(&HypotheticalStateMachine::s211);

            case G_SIG:
                logEvent("s21", "G");
                return _trans(&HypotheticalStateMachine::s1);

            default:
                return _super(&HypotheticalStateMachine::s2);
        }
    }

    HandleResult s211(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                logEntry("s211");
                return _handled();

            case EXIT_SIG:
                logExit("s211");
                return _handled();

            case D_SIG:
                logEvent("s211", "D");
                return _trans(&HypotheticalStateMachine::s21);

            case H_SIG:
                logEvent("s211", "H");
                return _trans(&HypotheticalStateMachine::s);

            default:
                return _super(&HypotheticalStateMachine::s21);
        }
    }

    HandleResult terminated(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                logEntry("terminated");
                cout << "[SYSTEM] Final pseudostate reached\n";
                return _handled();

            case EXIT_SIG:
                logExit("terminated");
                return _handled();

            default:
                return _super(&HypotheticalStateMachine::top);
        }
    }

    HandleResult _topInitialTrans() override {
        cout << "[INIT] top -> s2\n";
        setFoo(0);
        return _trans(&HypotheticalStateMachine::s2);
    }

    static void logEntry(const char* stateName) {
        cout << "[ENTRY] " << stateName << '\n';
    }

    static void logExit(const char* stateName) {
        cout << "[EXIT] " << stateName << '\n';
    }

    static void logInit(const char* sourceName, const char* targetName) {
        cout << "[INIT] " << sourceName << " -> " << targetName << '\n';
    }

    static void logEvent(const char* stateName, const char* signalName) {
        cout << "[EVENT] " << stateName << '-' << signalName << '\n';
    }

    static void logGuard(
        const char* stateName,
        const char* signalName,
        const char* expression,
        bool result) {
        cout << "[GUARD] " << stateName << '-' << signalName
             << " [" << expression << "] -> "
             << (result ? "true" : "false") << '\n';
    }

    void setFoo(int value) {
        _foo = value;
        cout << "[ACTION] foo = " << _foo << '\n';
    }

    int _foo;
};

static string toLowerCopy(string value) {
    for (char& ch : value) {
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

static const char* signalName(SignalId signal) {
    switch (signal) {
        case A_SIG:
            return "A";
        case B_SIG:
            return "B";
        case C_SIG:
            return "C";
        case D_SIG:
            return "D";
        case E_SIG:
            return "E";
        case F_SIG:
            return "F";
        case G_SIG:
            return "G";
        case H_SIG:
            return "H";
        case I_SIG:
            return "I";
        case TERMINATE_SIG:
            return "TERMINATE";
        default:
            return "?";
    }
}

static bool parseLetterEvent(const string& token, SignalId& signal) {
    if (token.size() != 1) {
        return false;
    }

    switch (tolower(static_cast<unsigned char>(token[0]))) {
        case 'a':
            signal = A_SIG;
            return true;
        case 'b':
            signal = B_SIG;
            return true;
        case 'c':
            signal = C_SIG;
            return true;
        case 'd':
            signal = D_SIG;
            return true;
        case 'e':
            signal = E_SIG;
            return true;
        case 'f':
            signal = F_SIG;
            return true;
        case 'g':
            signal = G_SIG;
            return true;
        case 'h':
            signal = H_SIG;
            return true;
        case 'i':
            signal = I_SIG;
            return true;
        default:
            return false;
    }
}

static void printHelp() {
    cout << "Commands:\n"
         << "  a..i               Dispatch the corresponding UML event\n"
         << "  event <a-i>        Dispatch the corresponding UML event\n"
         << "  terminate          Transition to the terminal state\n"
         << "  status             Show the active state path and foo value\n"
         << "  help               Show this help text\n"
         << "  quit               Exit the example\n";
}

static void printStatus(HypotheticalStateMachine& machine) {
    cout << "[STATUS] path=" << machine.activePathString()
         << ", leaf=" << machine.activeLeafName()
         << ", foo=" << machine.fooValue()
         << ", mode=" << (machine.isTerminated() ? "terminated" : "running")
         << '\n';
}

int main() {
    cout << "==============================================================\n"
         << "            UML HYPOTHETICAL STATE MACHINE EXAMPLE\n"
         << "==============================================================\n"
         << "This example mirrors the attached statechart and prints every\n"
         << "entry, exit, guard, init, and resulting active state.\n\n";

    HypotheticalStateMachine machine;
    machine.init();
    printStatus(machine);
    cout << '\n';
    printHelp();

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
        command = toLowerCopy(command);

        if (command.empty()) {
            continue;
        }

        if (command == "quit") {
            cout << "Exiting hypothetical UML example.\n";
            break;
        }

        if (command == "help") {
            printHelp();
            printStatus(machine);
            continue;
        }

        if (command == "status") {
            printStatus(machine);
            continue;
        }

        SignalId signal = 0;
        bool hasSignal = false;

        if (command == "terminate") {
            signal = TERMINATE_SIG;
            hasSignal = true;
        } else if (command == "event") {
            string token;
            input >> token;
            hasSignal = parseLetterEvent(token, signal);
            if (!hasSignal) {
                cout << "[ERROR] Use `event <a-i>`.\n";
                continue;
            }
        } else if (parseLetterEvent(command, signal)) {
            hasSignal = true;
        }

        if (hasSignal) {
            if (machine.isTerminated()) {
                cout << "[INFO] The machine is already terminated. UML events are ignored.\n";
                printStatus(machine);
                continue;
            }

            cout << "[INPUT] " << signalName(signal) << '\n';
            Event event(signal);
            machine.dispatch(event);
            printStatus(machine);
            continue;
        }

        cout << "[ERROR] Unknown command. Type `help` for supported commands.\n";
    }

    return 0;
}
