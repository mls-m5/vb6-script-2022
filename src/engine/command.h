#pragma once

class Command {
public:
    virtual void run() = 0;
    virtual ~Command() = default;
};

class FunctionCall : public Command {
    void run() override {}
};
