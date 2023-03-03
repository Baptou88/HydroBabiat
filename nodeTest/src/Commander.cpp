#include <LinkedList.h>

enum CommandStatus{
    Finiite,
    EnCours,
    Echec
};

class Command
{
private:
    /* data */
public:
    Command(const char Name,const char param);
    ~Command();

    CommandStatus execute();
};

Command::Command(const char Name,const char param)
{
}

Command::~Command()
{
}

CommandStatus Command::execute()
{
    return CommandStatus();
}

class Commander
{
private:
    LinkedList<Command*>Commands = LinkedList<Command*>();
public:
    Commander(/* args */);
    ~Commander();
    void loop();
    void addCommand(Command* cmd);
};

Commander::Commander(/* args */)
{
    
}

Commander::~Commander()
{
}

void Commander::addCommand(Command* cmd)
{
    Commands.add(cmd);
}
