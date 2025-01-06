#include <ProgrammatedTask.h>
#include <ArduinoJson.h>

bool ProgrammatedTask::isActive(){
    return _active;
}
String  ProgrammatedTask::getHours(){
    return h < 10 ? "0" + String(h) : String(h);
}
String  ProgrammatedTask::getMinutes(){
    return m < 10 ? "0" + String(m) : String(m);
}
String ProgrammatedTask::getTime()
{
    return String((h < 10 ? "0" + String(h) : String(h)) + ":" +( m < 10 ? "0" + String(m) : String(m)));
}
JsonDocument ProgrammatedTask::toJson()
{
    JsonDocument doc;

    doc["name"] = this->name;
    doc["execOnce"] = this->execOnce;
    doc["deepSleep"] = this->deepsleep;
    doc["targetVanne"] = this->targetVanne;
    doc["active"] = this->_active;
    doc["time"] = (String)this->h + ":" + (String)this->m;
    
    //doc.shrinkToFit();

    return doc;
    
    
}
void ProgrammatedTask::execute()
{
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