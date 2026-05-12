#include <iostream>
#include <iomanip>
#include "state_machine.h"

using namespace std;
using namespace Hsm;

// ============================================================================
// CUSTOM EVENT TYPES
// ============================================================================

// Extended event with data: contains timer duration information
struct TimerEvent : public Event {
    uint32_t duration_ms;

    TimerEvent(uint32_t duration = 0)
        : Event(4), duration_ms(duration) {}
};

// Signal-only event: just the signal, no additional data
struct ResetEvent : public Event {
    ResetEvent() : Event(7) {}
};

// ============================================================================
// CUSTOM SIGNALS (starting from 4 as reserved signals are 1-3)
// ============================================================================
enum CustomSignals : SignalId {
    TIMER_SIG = 4,                    // Timer expiration signal
    MAINTENANCE_REQUESTED_SIG = 5,    // Request to enter maintenance mode
    MAINTENANCE_DONE_SIG = 6,         // Maintenance completed signal
    RESET_SIG = 7                     // Reset to initial state
};

// ============================================================================
// TRAFFIC LIGHT STATE MACHINE
// ============================================================================

class TrafficLight : public StateMachine {
public:
    TrafficLight() : _state_duration(0), _transition_count(0) {
        cout << "[SYSTEM] TrafficLight state machine created\n" << endl;
    }

    virtual ~TrafficLight() = default;

    // Get the current duration for diagnostics
    uint32_t getDuration() const { return _state_duration; }
    uint32_t getTransitionCount() const { return _transition_count; }

    // Convenience method to check if in a specific state
    bool isInRed() { return isIn(static_cast<State>(&TrafficLight::red)); }
    bool isInYellow() { return isIn(static_cast<State>(&TrafficLight::yellow)); }
    bool isInGreen() { return isIn(static_cast<State>(&TrafficLight::green)); }
    bool isInMaintenance() { return isIn(static_cast<State>(&TrafficLight::maintenance)); }

private:
    // ========================================================================
    // STATE HANDLERS
    // ========================================================================

    // RED state handler
    // - Turns on the red light for vehicles
    // - Waits for timer to transition to GREEN
    // - Handles maintenance requests
    HandleResult red(const Event& event) {
        switch(event.signal) {
            case ENTRY_SIG: {
                cout << "[RED] Entry Action: Turning ON red light\n";
                _state_duration = 5000;  // 5 seconds
                cout << "[RED] Duration: " << _state_duration << " ms\n";
                return _handled();
            }

            case EXIT_SIG: {
                cout << "[RED] Exit Action: Turning OFF red light\n";
                return _handled();
            }

            case TIMER_SIG: {
                const TimerEvent* te = static_cast<const TimerEvent*>(&event);
                cout << "[RED] Timer event received (duration: " << te->duration_ms
                     << " ms) -> Transitioning to GREEN\n";
                _transition_count++;
                return _trans(&TrafficLight::green);
            }

            case MAINTENANCE_REQUESTED_SIG: {
                cout << "[MAINTENANCE] Maintenance requested -> Transitioning to MAINTENANCE\n";
                _transition_count++;
                return _trans(&TrafficLight::maintenance);
            }

            case RESET_SIG: {
                cout << "[RED] Reset signal received -> Staying in RED\n";
                return _handled();
            }

            default:
                return _super(&TrafficLight::top);
        }
    }

    // YELLOW state handler
    // - Turns on the yellow light (warning)
    // - Waits for timer to transition to RED
    // - Cannot be entered from RED directly (must go through GREEN)
    HandleResult yellow(const Event& event) {
        switch(event.signal) {
            case ENTRY_SIG: {
                cout << "[YELLOW] Entry Action: Turning ON yellow light\n";
                _state_duration = 2000;  // 2 seconds
                cout << "[YELLOW] Duration: " << _state_duration << " ms\n";
                return _handled();
            }

            case EXIT_SIG: {
                cout << "[YELLOW] Exit Action: Turning OFF yellow light\n";
                return _handled();
            }

            case TIMER_SIG: {
                const TimerEvent* te = static_cast<const TimerEvent*>(&event);
                cout << "[YELLOW] Timer event received (duration: " << te->duration_ms
                     << " ms) -> Transitioning to RED\n";
                _transition_count++;
                return _trans(&TrafficLight::red);
            }

            case MAINTENANCE_REQUESTED_SIG: {
                cout << "[MAINTENANCE] Maintenance requested -> Transitioning to MAINTENANCE\n";
                _transition_count++;
                return _trans(&TrafficLight::maintenance);
            }

            default:
                return _super(&TrafficLight::top);
        }
    }

    // GREEN state handler
    // - Turns on the green light for vehicles to proceed
    // - Waits for timer to transition to YELLOW
    // - Handles maintenance requests
    HandleResult green(const Event& event) {
        switch(event.signal) {
            case ENTRY_SIG: {
                cout << "[GREEN] Entry Action: Turning ON green light\n";
                _state_duration = 8000;  // 8 seconds (longest duration)
                cout << "[GREEN] Duration: " << _state_duration << " ms\n";
                return _handled();
            }

            case EXIT_SIG: {
                cout << "[GREEN] Exit Action: Turning OFF green light\n";
                return _handled();
            }

            case TIMER_SIG: {
                const TimerEvent* te = static_cast<const TimerEvent*>(&event);
                cout << "[GREEN] Timer event received (duration: " << te->duration_ms
                     << " ms) -> Transitioning to YELLOW\n";
                _transition_count++;
                return _trans(&TrafficLight::yellow);
            }

            case MAINTENANCE_REQUESTED_SIG: {
                cout << "[MAINTENANCE] Maintenance requested -> Transitioning to MAINTENANCE\n";
                _transition_count++;
                return _trans(&TrafficLight::maintenance);
            }

            default:
                return _super(&TrafficLight::top);
        }
    }

    // MAINTENANCE state handler
    // - Turns on all lights as a warning (or blinks)
    // - Waits for MAINTENANCE_DONE signal
    // - Can be reset from any state
    HandleResult maintenance(const Event& event) {
        switch(event.signal) {
            case ENTRY_SIG: {
                cout << "[MAINTENANCE] Entry Action: Activating maintenance mode\n";
                cout << "[MAINTENANCE] All lights are BLINKING (warning mode)\n";
                _state_duration = 0;  // No time limit in maintenance
                return _handled();
            }

            case EXIT_SIG: {
                cout << "[MAINTENANCE] Exit Action: Deactivating maintenance mode\n";
                return _handled();
            }

            case MAINTENANCE_DONE_SIG: {
                cout << "[MAINTENANCE] Maintenance completed -> Transitioning to RED\n";
                _transition_count++;
                return _trans(&TrafficLight::red);
            }

            case TIMER_SIG: {
                // Ignore timer while in maintenance
                cout << "[MAINTENANCE] Timer event ignored (in maintenance mode)\n";
                return _handled();
            }

            default:
                return _super(&TrafficLight::top);
        }
    }

    // ========================================================================
    // TOP-LEVEL STATE (required pure virtual method)
    // ========================================================================

    // This is the top-level initial transition that every state machine must provide.
    // It defines which state the machine enters first during init().
    virtual HandleResult _topInitialTrans(void) override {
        cout << "[SYSTEM] Top-level initial transition: Starting in RED state\n";
        // Use _trans() to indicate this is a transition to the initial state
        return _trans(&TrafficLight::red);
    }

    // ========================================================================
    // PRIVATE DATA
    // ========================================================================

    uint32_t _state_duration;      // Duration of current state in milliseconds
    uint32_t _transition_count;    // Count of transitions for diagnostics

}; // class TrafficLight

// ============================================================================
// MAIN FUNCTION - DEMONSTRATES STATE MACHINE USAGE
// ============================================================================

int main() {
    cout << "==============================================================\n"
         << "           HIERARCHICAL STATE MACHINE BASIC EXAMPLE\n"
         << "                    Traffic Light System\n"
         << "==============================================================\n\n";

    // ========================================================================
    // 1. CREATE STATE MACHINE INSTANCE
    // ========================================================================
    TrafficLight traffic_light;

    // ========================================================================
    // 2. INITIALIZE THE STATE MACHINE
    // ========================================================================
    cout << "Step 1: Initialize the state machine\n";
    cout << "---------------------------------------------------------------\n";
    traffic_light.init();  // Calls _topInitialTrans() and enters RED state
    cout << "\nState machine is now in RED state and ready for events.\n\n";

    // ========================================================================
    // 3. DISPATCH EVENTS - DEMONSTRATE STATE TRANSITIONS
    // ========================================================================
    cout << "Step 2: Dispatch events to trigger state transitions\n";
    cout << "---------------------------------------------------------------\n\n";

    // Event 1: Timer expires in RED state -> transition to GREEN
    cout << "Event 1: Timer expires (5000 ms)\n";
    cout << "-----------\n";
    TimerEvent timer_event_1(5000);
    traffic_light.dispatch(timer_event_1);
    cout << "Current transitions: " << traffic_light.getTransitionCount() << "\n\n";

    // Verify we're in GREEN
    if (traffic_light.isInGreen()) {
        cout << "[VERIFICATION] Successfully transitioned to GREEN state\n\n";
    }

    // Event 2: Another timer expires in GREEN state -> transition to YELLOW
    cout << "Event 2: Timer expires (8000 ms)\n";
    cout << "-----------\n";
    TimerEvent timer_event_2(8000);
    traffic_light.dispatch(timer_event_2);
    cout << "Current transitions: " << traffic_light.getTransitionCount() << "\n\n";

    // Verify we're in YELLOW
    if (traffic_light.isInYellow()) {
        cout << "[VERIFICATION] Successfully transitioned to YELLOW state\n\n";
    }

    // Event 3: Timer expires in YELLOW state -> transition to RED
    cout << "Event 3: Timer expires (2000 ms)\n";
    cout << "-----------\n";
    TimerEvent timer_event_3(2000);
    traffic_light.dispatch(timer_event_3);
    cout << "Current transitions: " << traffic_light.getTransitionCount() << "\n\n";

    // Verify we're back in RED
    if (traffic_light.isInRed()) {
        cout << "[VERIFICATION] Successfully transitioned back to RED state\n\n";
    }

    // ========================================================================
    // 4. DEMONSTRATE EXTENDED EVENTS WITH DATA
    // ========================================================================
    cout << "Step 3: Dispatch extended event with data\n";
    cout << "---------------------------------------------------------------\n\n";

    cout << "Event 4: Extended timer event with custom duration data (3000 ms)\n";
    cout << "-----------\n";
    TimerEvent extended_timer(3000);
    traffic_light.dispatch(extended_timer);
    cout << "Current transitions: " << traffic_light.getTransitionCount() << "\n\n";

    // ========================================================================
    // 5. DEMONSTRATE SIGNAL-ONLY EVENTS AND STATE-SPECIFIC HANDLING
    // ========================================================================
    cout << "Step 4: Demonstrate signal-only event and maintenance mode\n";
    cout << "---------------------------------------------------------------\n\n";

    cout << "Event 5: Maintenance requested (signal-only event)\n";
    cout << "-----------\n";
    Event maintenance_event(MAINTENANCE_REQUESTED_SIG);
    traffic_light.dispatch(maintenance_event);
    cout << "Current transitions: " << traffic_light.getTransitionCount() << "\n\n";

    // Verify we're in MAINTENANCE
    if (traffic_light.isInMaintenance()) {
        cout << "[VERIFICATION] Successfully transitioned to MAINTENANCE state\n\n";
    }

    // ========================================================================
    // 6. DEMONSTRATE EVENT HANDLING IN MAINTENANCE MODE
    // ========================================================================
    cout << "Step 5: Demonstrate event handling while in maintenance\n";
    cout << "---------------------------------------------------------------\n\n";

    cout << "Event 6: Timer event in maintenance mode (should be ignored)\n";
    cout << "-----------\n";
    TimerEvent timer_in_maintenance(5000);
    traffic_light.dispatch(timer_in_maintenance);
    cout << "Current transitions: " << traffic_light.getTransitionCount() << "\n\n";

    // ========================================================================
    // 7. EXIT MAINTENANCE AND RETURN TO NORMAL OPERATION
    // ========================================================================
    cout << "Step 6: Exit maintenance mode\n";
    cout << "---------------------------------------------------------------\n\n";

    cout << "Event 7: Maintenance completed\n";
    cout << "-----------\n";
    Event maintenance_done(MAINTENANCE_DONE_SIG);
    traffic_light.dispatch(maintenance_done);
    cout << "Current transitions: " << traffic_light.getTransitionCount() << "\n\n";

    // Verify we're back in RED
    if (traffic_light.isInRed()) {
        cout << "[VERIFICATION] Successfully returned to RED state after maintenance\n\n";
    }

    // ========================================================================
    // 8. SUMMARY
    // ========================================================================
    cout << "==============================================================\n"
         << "                         SUMMARY\n"
         << "==============================================================\n\n";

    cout << "Total transitions executed: " << traffic_light.getTransitionCount() << "\n\n";

    cout << "Key concepts demonstrated:\n"
         << "  1. ✓ Custom state machine class inheriting from Hsm::StateMachine\n"
         << "  2. ✓ Multiple states (Red, Yellow, Green, Maintenance)\n"
         << "  3. ✓ Custom signals (TIMER_SIG, MAINTENANCE_REQUESTED_SIG, etc.)\n"
         << "  4. ✓ Extended events with data (TimerEvent with duration_ms)\n"
         << "  5. ✓ Signal-only events (ResetEvent)\n"
         << "  6. ✓ State transitions triggered by events using _trans()\n"
         << "  7. ✓ Proper initialization with _topInitialTrans()\n"
         << "  8. ✓ Event dispatching showing state changes\n"
         << "  9. ✓ Entry/exit actions in states (printed in each state handler)\n"
         << " 10. ✓ Main function that creates, initializes, and dispatches events\n\n";

    cout << "How the state machine works:\n"
         << "  - init() calls _topInitialTrans() to enter RED (initial state)\n"
         << "  - Each state handles events and can transition to other states\n"
         << "  - Entry/exit actions run automatically during transitions\n"
         << "  - _trans() method marks a state transition in the event handler\n"
         << "  - dispatch() processes transitions and executes entry/exit actions\n"
         << "  - isIn() allows checking if currently in a specific state\n\n";

    cout << "==============================================================\n"
         << "State machine successfully terminated.\n"
         << "==============================================================\n";

    return 0;
}