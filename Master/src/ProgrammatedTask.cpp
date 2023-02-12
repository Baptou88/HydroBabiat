#include <ProgrammatedTask.h>

bool ProgrammatedTask::isActive(){
    return _active;
}
String  ProgrammatedTask::getHours(){
    return h < 10 ? "0" + String(h) : String(h);
}
String  ProgrammatedTask::getMinutes(){
    return m < 10 ? "0" + String(m) : String(m);
}
void ProgrammatedTask::execute(){
    Serial.println("Tache executé");
}
ProgrammatedTask::ProgrammatedTask(byte heures,byte minutes, String Name)
{
    name=Name;
    h = heures;
    m = minutes;
}

ProgrammatedTask::~ProgrammatedTask()
{
}

void ProgrammatedTask::activate(){
    _active = true;
}
void ProgrammatedTask::deactivate(){
    _active = false;
}