#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

// Enum for security tool states
enum class ToolState { IDLE, RUNNING, PAUSED, STOPPED };

// Enum for security alerts
enum class AlertLevel { LOW, MEDIUM, HIGH };

// Security tool class
class SecurityTool {
private:
    string toolName;
    ToolState state;
    mutex mtx;
    vector<string> logs;

public:
    SecurityTool(string name) : toolName(name), state(ToolState::IDLE) {}

    void start() {
        lock_guard<mutex> lock(mtx);
        state = ToolState::RUNNING;
        cout << "Tool started: " << toolName << endl;
    }

    void pause() {
        lock_guard<mutex> lock(mtx);
        state = ToolState::PAUSED;
        cout << "Tool paused: " << toolName << endl;
    }

    void stop() {
        lock_guard<mutex> lock(mtx);
        state = ToolState::STOPPED;
        cout << "Tool stopped: " << toolName << endl;
    }

    void logAlert(AlertLevel level, string message) {
        lock_guard<mutex> lock(mtx);
        logs.emplace_back(to_string(static_cast<int>(level)) + ": " + message);
        cout << "Log: " << logs.back() << endl;
    }

    void printLogs() {
        lock_guard<mutex> lock(mtx);
        for (const auto& log : logs) {
            cout << log << endl;
        }
    }
};

// Automated security tool controller class
class SecurityController {
private:
    vector<SecurityTool> tools;

public:
    void addTool(SecurityTool tool) {
        tools.emplace_back(tool);
    }

    void startAllTools() {
        for (auto& tool : tools) {
            tool.start();
        }
    }

    void pauseAllTools() {
        for (auto& tool : tools) {
            tool.pause();
        }
    }

    void stopAllTools() {
        for (auto& tool : tools) {
            tool.stop();
        }
    }

    void monitorTools() {
        while (true) {
            for (auto& tool : tools) {
                if (tool.logs.size() > 0) {
                    tool.printLogs();
                }
                this_thread::sleep_for(chrono::seconds(5));
            }
        }
    }
};

int main() {
    SecurityController controller;

    SecurityTool tool1("Firewall");
    SecurityTool tool2("Intrusion Detection");
    SecurityTool tool3("Virus Scanner");

    controller.addTool(tool1);
    controller.addTool(tool2);
    controller.addTool(tool3);

    controller.startAllTools();

    thread monitorThread(&SecurityController::monitorTools, &controller);
    monitorThread.detach();

    this_thread::sleep_for(chrono::seconds(10));

    tool1.logAlert(AlertLevel::HIGH, "Suspicious activity detected");
    tool2.logAlert(AlertLevel::MEDIUM, "Potential threat identified");

    controller.pauseAllTools();

    this_thread::sleep_for(chrono::seconds(5));

    controller.stopAllTools();

    return 0;
}