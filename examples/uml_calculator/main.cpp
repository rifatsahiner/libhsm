#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <string>
#include <cstring>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>

#include "state_machine.h"

using namespace std;
using namespace Hsm;

enum CustomSignals : SignalId {
    DIGIT_0_SIG = 4,
    DIGIT_1_9_SIG,
    POINT_SIG,
    OPER_SIG,
    EQUALS_SIG,
    CE_SIG,
    C_SIG,
    OFF_SIG
};

struct DigitEvent : public Event {
    char keyId;
    DigitEvent(char id, SignalId sig) : Event(sig), keyId(id) {}
};

struct OperatorEvent : public Event {
    char op;
    OperatorEvent(char o) : Event(OPER_SIG), op(o) {}
};

class CalculatorStateMachine : public StateMachine {
public:
    CalculatorStateMachine() = default;

    const string& display() const { return _display; }

private:
    string _display = "0";
    double _operand1 = 0.0;
    char _pendingOp = 0;

    void clearDisplay() {
        _display = "0";
    }

    void insert(char c) {
        if (_display == "0" || _display == "-0") {
            _display = (_display == "-0") ? "-" : "";
        }
        _display += c;
    }

    void negate() {
        _display = "-0";
    }

    bool compute() {
        double op2;
        try {
            op2 = stod(_display);
        } catch (...) {
            _display = "Error";
            return true;
        }
        double result;
        bool err = false;

        if (_pendingOp == 0) {
            result = op2;
        } else {
            switch (_pendingOp) {
                case '+': result = _operand1 + op2; break;
                case '-': result = _operand1 - op2; break;
                case '*': result = _operand1 * op2; break;
                case '/':
                    if (op2 == 0.0) {
                        err = true;
                        break;
                    }
                    result = _operand1 / op2;
                    break;
                default: err = true;
            }
        }

        if (err || isnan(result) || isinf(result) || fabs(result) > 9.999e15) {
            _display = "Error";
            return true;
        }

        _operand1 = result;
        ostringstream oss;
        oss << fixed << setprecision(10) << result;
        string s = oss.str();
        // trim trailing zeros
        size_t dot = s.find('.');
        if (dot != string::npos) {
            size_t end = s.find_last_not_of('0');
            if (end != string::npos && end > dot) end++;
            else end = dot;
            s = s.substr(0, end);
            if (s.back() == '.') s.pop_back();
        }
        _display = s.empty() ? "0" : s;
        return false;
    }

    HandleResult on(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                return _handled();
            case EXIT_SIG:
                return _handled();
            case INIT_SIG:
                return _trans(&CalculatorStateMachine::ready);
            case C_SIG:
                return _trans(&CalculatorStateMachine::begin);
            case OFF_SIG:
                return _handled();
            case CE_SIG:
                clearDisplay();
                return _trans(&CalculatorStateMachine::zero1);
            default:
                return _super(&CalculatorStateMachine::top);
        }
    }

    HandleResult ready(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
            case EXIT_SIG:
                return _handled();
            case INIT_SIG:
                return _trans(&CalculatorStateMachine::begin);
            default:
                return _super(&CalculatorStateMachine::on);
        }
    }

    HandleResult begin(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                clearDisplay();
                return _handled();
            case DIGIT_0_SIG:
                return _trans(&CalculatorStateMachine::zero1);
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                clearDisplay();
                insert(e.keyId);
                return _trans(&CalculatorStateMachine::int1);
            }
            case POINT_SIG: {
                insert('0');
                insert('.');
                return _trans(&CalculatorStateMachine::frac1);
            }
            case OPER_SIG: {
                const auto& e = event_cast<OperatorEvent>(event);
                _operand1 = stod(_display);
                _pendingOp = e.op;
                if (e.op == '-') {
                    return _trans(&CalculatorStateMachine::negated1);
                }
                clearDisplay();
                return _trans(&CalculatorStateMachine::opEntered);
            }
            default:
                return _super(&CalculatorStateMachine::ready);
        }
    }

    HandleResult result(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                return _handled();
            case DIGIT_0_SIG:
                return _trans(&CalculatorStateMachine::zero1);
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                clearDisplay();
                insert(e.keyId);
                return _trans(&CalculatorStateMachine::int1);
            }
            case POINT_SIG: {
                clearDisplay();
                insert('0');
                insert('.');
                return _trans(&CalculatorStateMachine::frac1);
            }
            case OPER_SIG: {
                const auto& e = event_cast<OperatorEvent>(event);
                _operand1 = stod(_display);
                _pendingOp = e.op;
                if (e.op == '-') {
                    return _trans(&CalculatorStateMachine::negated1);
                }
                clearDisplay();
                return _trans(&CalculatorStateMachine::opEntered);
            }
            default:
                return _super(&CalculatorStateMachine::ready);
        }
    }

    HandleResult negated1(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                negate();
                return _handled();
            case DIGIT_0_SIG:
                return _trans(&CalculatorStateMachine::zero1);
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _trans(&CalculatorStateMachine::int1);
            }
            case POINT_SIG: {
                insert('.');
                return _trans(&CalculatorStateMachine::frac1);
            }
            default:
                return _super(&CalculatorStateMachine::on);
        }
    }

    HandleResult operand1(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
            case EXIT_SIG:
                return _handled();
            case CE_SIG:
                clearDisplay();
                return _trans(&CalculatorStateMachine::zero1);
            case OPER_SIG: {
                const auto& e = event_cast<OperatorEvent>(event);
                _operand1 = stod(_display);
                _pendingOp = e.op;
                clearDisplay();
                return _trans(&CalculatorStateMachine::opEntered);
            }
            default:
                return _super(&CalculatorStateMachine::on);
        }
    }

    HandleResult zero1(const Event& event) {
        switch (event.signal) {
            case DIGIT_0_SIG:
                return _handled();
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _trans(&CalculatorStateMachine::int1);
            }
            case POINT_SIG: {
                clearDisplay();
                insert('0');
                insert('.');
                return _trans(&CalculatorStateMachine::frac1);
            }
            default:
                return _super(&CalculatorStateMachine::operand1);
        }
    }

    HandleResult int1(const Event& event) {
        switch (event.signal) {
            case DIGIT_0_SIG:
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _handled();
            }
            case POINT_SIG: {
                insert('.');
                return _trans(&CalculatorStateMachine::frac1);
            }
            default:
                return _super(&CalculatorStateMachine::operand1);
        }
    }

    HandleResult frac1(const Event& event) {
        switch (event.signal) {
            case DIGIT_0_SIG:
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _handled();
            }
            case POINT_SIG:
                return _handled();
            default:
                return _super(&CalculatorStateMachine::operand1);
        }
    }

    HandleResult opEntered(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                clearDisplay();
                return _handled();
            case DIGIT_0_SIG:
                return _trans(&CalculatorStateMachine::zero2);
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _trans(&CalculatorStateMachine::int2);
            }
            case POINT_SIG: {
                insert('0');
                insert('.');
                return _trans(&CalculatorStateMachine::frac2);
            }
            case OPER_SIG: {
                const auto& e = event_cast<OperatorEvent>(event);
                _pendingOp = e.op;
                if (e.op == '-') {
                    return _trans(&CalculatorStateMachine::negated2);
                }
                clearDisplay();
                return _trans(&CalculatorStateMachine::opEntered);
            }
            default:
                return _super(&CalculatorStateMachine::on);
        }
    }

    HandleResult negated2(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                negate();
                return _handled();
            case DIGIT_0_SIG:
                return _trans(&CalculatorStateMachine::zero2);
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _trans(&CalculatorStateMachine::int2);
            }
            case POINT_SIG: {
                insert('.');
                return _trans(&CalculatorStateMachine::frac2);
            }
            default:
                return _super(&CalculatorStateMachine::on);
        }
    }

    HandleResult operand2(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
            case EXIT_SIG:
                return _handled();
            case CE_SIG:
                clearDisplay();
                return _trans(&CalculatorStateMachine::zero2);
            case EQUALS_SIG: {
                if (compute()) {
                    return _trans(&CalculatorStateMachine::error);
                }
                return _trans(&CalculatorStateMachine::result);
            }
            default:
                return _super(&CalculatorStateMachine::on);
        }
    }

    HandleResult zero2(const Event& event) {
        switch (event.signal) {
            case DIGIT_0_SIG:
                return _handled();
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _trans(&CalculatorStateMachine::int2);
            }
            case POINT_SIG: {
                insert('0');
                insert('.');
                return _trans(&CalculatorStateMachine::frac2);
            }
            default:
                return _super(&CalculatorStateMachine::operand2);
        }
    }

    HandleResult int2(const Event& event) {
        switch (event.signal) {
            case DIGIT_0_SIG:
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _handled();
            }
            case POINT_SIG: {
                insert('.');
                return _trans(&CalculatorStateMachine::frac2);
            }
            default:
                return _super(&CalculatorStateMachine::operand2);
        }
    }

    HandleResult frac2(const Event& event) {
        switch (event.signal) {
            case DIGIT_0_SIG:
            case DIGIT_1_9_SIG: {
                const auto& e = event_cast<DigitEvent>(event);
                insert(e.keyId);
                return _handled();
            }
            case POINT_SIG:
                return _handled();
            default:
                return _super(&CalculatorStateMachine::operand2);
        }
    }

    HandleResult error(const Event& event) {
        switch (event.signal) {
            case ENTRY_SIG:
                _display = "Error";
                return _handled();
            case C_SIG:
                return _trans(&CalculatorStateMachine::begin);
            case CE_SIG:
                clearDisplay();
                return _trans(&CalculatorStateMachine::zero1);
            case OPER_SIG: {
                const auto& e = event_cast<OperatorEvent>(event);
                _operand1 = 0.0;
                _pendingOp = e.op;
                clearDisplay();
                if (e.op == '-') {
                    return _trans(&CalculatorStateMachine::negated1);
                }
                return _trans(&CalculatorStateMachine::opEntered);
            }
            default:
                return _super(&CalculatorStateMachine::on);
        }
    }

    HandleResult _topInitialTrans(void) override {
        return _trans(&CalculatorStateMachine::on);
    }
};

CalculatorStateMachine* g_calc = nullptr;
Fl_Output* g_display = nullptr;

void button_cb(Fl_Widget*, void* data) {
    const char* cmd = static_cast<const char*>(data);
    if (!cmd || !g_calc) return;

    if (strcmp(cmd, "OFF") == 0) {
        exit(0);
    } else if (strcmp(cmd, "C") == 0) {
        g_calc->dispatch(Event(C_SIG));
    } else if (strcmp(cmd, "CE") == 0) {
        g_calc->dispatch(Event(CE_SIG));
    } else if (strcmp(cmd, "=") == 0) {
        g_calc->dispatch(Event(EQUALS_SIG));
    } else if (strcmp(cmd, ".") == 0) {
        g_calc->dispatch(Event(POINT_SIG));
    } else if (strlen(cmd) == 1 && isdigit(static_cast<unsigned char>(cmd[0]))) {
        char key = cmd[0];
        SignalId sig = (key == '0') ? DIGIT_0_SIG : DIGIT_1_9_SIG;
        DigitEvent e(key, sig);
        g_calc->dispatch(e);
    } else if (strlen(cmd) == 1 && (cmd[0] == '+' || cmd[0] == '-' || cmd[0] == '*' || cmd[0] == '/')) {
        OperatorEvent e(cmd[0]);
        g_calc->dispatch(e);
    }

    g_display->value(g_calc->display().c_str());
}

int main() {
    CalculatorStateMachine calc;
    g_calc = &calc;
    calc.init();

    Fl_Window* win = new Fl_Window(290, 420, "HSM UML Calculator (Figure 2.18)");
    win->color(FL_WHITE);

    g_display = new Fl_Output(15, 15, 260, 45);
    g_display->value(calc.display().c_str());
    g_display->textsize(28);
    g_display->textcolor(FL_BLACK);
    g_display->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
    g_display->box(FL_DOWN_BOX);

    // Row 1: C CE OFF /
    (new Fl_Button(15, 75, 60, 55, "C"))->callback(button_cb, (void*)"C");
    (new Fl_Button(85, 75, 60, 55, "CE"))->callback(button_cb, (void*)"CE");
    (new Fl_Button(155, 75, 60, 55, "OFF"))->callback(button_cb, (void*)"OFF");
    (new Fl_Button(225, 75, 60, 55, "/"))->callback(button_cb, (void*)"/");

    // Row 2: 7 8 9 *
    (new Fl_Button(15, 140, 60, 55, "7"))->callback(button_cb, (void*)"7");
    (new Fl_Button(85, 140, 60, 55, "8"))->callback(button_cb, (void*)"8");
    (new Fl_Button(155, 140, 60, 55, "9"))->callback(button_cb, (void*)"9");
    (new Fl_Button(225, 140, 60, 55, "*"))->callback(button_cb, (void*)"*");

    // Row 3: 4 5 6 -
    (new Fl_Button(15, 205, 60, 55, "4"))->callback(button_cb, (void*)"4");
    (new Fl_Button(85, 205, 60, 55, "5"))->callback(button_cb, (void*)"5");
    (new Fl_Button(155, 205, 60, 55, "6"))->callback(button_cb, (void*)"6");
    (new Fl_Button(225, 205, 60, 55, "-"))->callback(button_cb, (void*)"-");

    // Row 4: 1 2 3 +
    (new Fl_Button(15, 270, 60, 55, "1"))->callback(button_cb, (void*)"1");
    (new Fl_Button(85, 270, 60, 55, "2"))->callback(button_cb, (void*)"2");
    (new Fl_Button(155, 270, 60, 55, "3"))->callback(button_cb, (void*)"3");
    (new Fl_Button(225, 270, 60, 55, "+"))->callback(button_cb, (void*)"+");

    // Row 5: 0 . =
    (new Fl_Button(15, 335, 130, 55, "0"))->callback(button_cb, (void*)"0");
    (new Fl_Button(155, 335, 60, 55, "."))->callback(button_cb, (void*)".");
    (new Fl_Button(225, 335, 60, 55, "="))->callback(button_cb, (void*)"=");

    win->resizable(g_display);
    win->show();

    return Fl::run();
}
