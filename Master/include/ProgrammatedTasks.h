#include <Arduino.h>
#include <ProgrammatedTask.h>
#include <LList.h>
#include <SPIFFS.h>
#include <NTPClient.h>

#if !defined(__PROGRAMMATEDTASKS_H__)
#define __PROGRAMMATEDTASKS_H__



class ProgrammatedTasksClass
{
private:
    const char* ProgrammatedTaskFile = "/Programmated";
    unsigned long previouscheck = 0;
    NTPClient* NtpCli;
public:
    LList<ProgrammatedTask*> *ListTasks =  new LList<ProgrammatedTask*>();
    ProgrammatedTasksClass(/* args */);
    ~ProgrammatedTasksClass();

    bool begin(NTPClient *ntpCli);

    bool initTask();
    void addTask(ProgrammatedTask* task);
    bool saveTask();
    ProgrammatedTask* getTask(int taskNumber);
    void loop();

    static String templateProcessor(const String var);
};




extern ProgrammatedTasksClass ProgTasks;

#endif // __PROGRAMMATEDTASKS_H__