#include <ProgrammatedTasks.h>

ProgrammatedTasksClass ProgTasks;

ProgrammatedTasksClass::ProgrammatedTasksClass()
{
    ListTasks =  new  LList<ProgrammatedTask*>();
}

ProgrammatedTasksClass::~ProgrammatedTasksClass()
{
}

bool ProgrammatedTasksClass::begin(NTPClient* ntpCli)
{
    NtpCli = ntpCli;
    return false;
}

bool ProgrammatedTasksClass::initTask()
{
  if (!SPIFFS.exists(ProgrammatedTaskFile))
	{
		return true;
	}
	
	File fichier = SPIFFS.open(ProgrammatedTaskFile,"r");
	while (fichier.available()) {
		String a =fichier.readStringUntil('\n');
		if (a!="")
		{
			
			String name = "";
			byte h = 0;
			byte m = 0;
			bool activ = false;
			int targetVanne = 0;
			double deepsleep = 0;

			while (a.indexOf(";") !=-1)
			{
				String part1 = a.substring(0,a.indexOf("="));
    		String part2 = a.substring(a.indexOf("=")+1 , a.indexOf(";"));
				
				a.remove(0,a.indexOf(";")+1);

				if (part1 == "name")
				{
					name = part2;
				}
				if (part1 == "h")
				{
					h = part2.toInt();
				}
				if (part1 == "m")
				{
					m = part2.toInt();
				}
				if (part1 == "activate")
				{
					activ = part2.toInt();
				}
				if (part1 == "targetVanne")
				{
					targetVanne = part2.toInt();
				}
				
				if (part1 == "deepsleep")
				{
					deepsleep = part2.toDouble();
				}
				
			}
			
			ProgrammatedTask *ajout = new ProgrammatedTask(h,m,name);
			if (activ)
			{
				ajout->activate();
			} else
			{
				ajout->deactivate();
			}
			ajout->deepsleep = deepsleep;
			ajout->targetVanne = targetVanne;
			
			ListTasks->add(ajout);
			
		}
		
		
	}
	fichier.close();

    return true;
}

void ProgrammatedTasksClass::addTask(ProgrammatedTask *task)
{
    ListTasks->add(task);
}

bool ProgrammatedTasksClass::saveTask()
{
  File file = SPIFFS.open("/Programmated","w+");

  for (size_t i = 0; i < ProgTasks.ListTasks->size(); i++)
  {
    ProgrammatedTask *test = ProgTasks.ListTasks->get(i);
    
    file.print("name=" + String(test->name) + ";");
    file.print("h=" + String(test->h) + ";");
    file.print("m=" + String(test->m) + ";");
    file.print("activate=" + String(test->isActive()) + ";");
    file.print("targetVanne=" + String(test->targetVanne) + ";");
    file.println("deepsleep=" + String(test->deepsleep) + ";");
  }
  file.close();
  return true;
}

void ProgrammatedTasksClass::loop()
{
    if (millis() > previouscheck + 60000)
    {
        previouscheck = millis();

        for (size_t i = 0; i < ListTasks->size(); i++)
		{
			ProgrammatedTask *tache = ListTasks->get(i);
			if (tache->isActive())
			{
				if (NtpCli->getHours() == tache->h && NtpCli->getMinutes() == tache->m  )
				{
					Serial.println("exec Tache: " + (String)tache->name);
          //TODO Gerer Execution Tache

          
					// if (tache->deepsleep != 0)
					// {
					// 	for (size_t i = 1; i < allBoard->size()	; i++)
					// 	{
					// 		allBoard->get(i)->msgToSend += "DeepSleep="+ String(tache->deepsleep);
					// 	}
						
					// }
					// if (tache->targetVanne != 0)
					// {
					// 	searchBoardById(TURBINE)->msgToSend += "P=" + String(tache->targetVanne);
					// }
					
					
				}
				
			}
			
		}

    }
    
}

ProgrammatedTask *ProgrammatedTasksClass::getTask(int taskNumber)
{

    
    return ListTasks->get(taskNumber);
    return nullptr;
}

String ProgrammatedTasksClass::templateProcessor(const String var)
{
    String retour = "";

  if (var == "ListeProgram")
  {

    for (size_t i = 0; i < ProgTasks.ListTasks->size(); i++)
    {
      // retour += (String)ProgrammatedTasks->get(i)->name;
      retour += "<div x-data=\"{edit:false, name:'" + (String)ProgTasks.ListTasks->get(i)->name + "'}\" class=\"card border my-2 \" style=\"width: auto\">\n";
      retour += "<form  action =\"/updateprogrammateur\" method=\"post\">\n";
      // retour += "<h3>" + (String)ProgrammatedTasks->get(i)->name + "</h3>";
      retour += "<div class=\"card-header\">\n";
      retour += "<div class=\"form-check form-switch\" >\n";
      retour += "<input class=\"form-check-input\" type=\"checkbox\" name=\"active\" role=\"switch\" id=\"flexSwitchCheckChecked\" " + String("\%ProgrammatedTasks" + String(i) + ":isActive\%") + " onclick=\"update(this)\">";
      // retour += "<label class=\"form-check-label\" for=\"flexSwitchCheckChecked\">Checked switch checkbox input</label>";
      retour += "<button class=\"btn btn-outline-success\" x-show=\"!edit\" @click.prevent=\"edit = ! edit\">Toggle</button>\n";
      retour += "<div x-show=\"!edit\" >\n<h5  class=\"card-title\" x-text=\"name\"></h5>\n</div>\n";
      retour += "<input x-show=\"edit\" @click.outside=\"edit = false\" type=\"text\" name=\"name\" id=\"\" x-model=\"name\">\n";

      retour += "</div>\n";
      retour += "</div>\n";
      retour += "<div class=\"form-group\" hidden>\n";
      retour += "<label for=\"exampleFormControlInput1\">ID</label>\n";
      retour += "<input type=\"number\" class=\"form-control\" name=\"id\" id=\"exampleFormControlInput1\" placeholder=\"id\" value=\"" + (String)i + "\" hidden>\n";
      retour += "</div>\n";
      retour += "<label for=\"appt\">Heure du déclanchement:</label>\n";
      retour += "<input type=\"time\" id=\"appt\" name=\"appt\"  value= \"\%ProgrammatedTasks" + String(i) + ":getHours\%" + ":" + "\%ProgrammatedTasks" + String(i) + ":getMinutes\%" + "\"  required>\n";
      retour += "<label for=\"customRange1\" class=\"form-label\">Example range</label>";
      retour += "<input type=\"range\" value=\"\%ProgrammatedTasks" + String(i) + ":targetVanne\%" + "\" name=\"targetVanne\" class=\"form-range\" id=\"customRange1\">";
      retour += "<label for=\"deepsleep\">DeepSleep (ms) :</label>\n";
      retour += "<input type=\"number\" id=\"appte\" name=\"deepsleep\"  value= \"\%ProgrammatedTasks" + String(i) + ":deepsleep\%" + "\"  required>\n";
      retour += "<button class=\"btn btn-primary\" type=\"submit\">Mettre a jour</button>";
      retour += "<a href=\"/programmateur/?delete=" + (String)i + "\" class=\"btn btn-danger\">";
      retour += "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"16\" height=\"16\" fill=\"white\" class=\"bi bi-trash\" viewBox=\"0 0 16 16\">";
      retour += "<path d=\"M5.5 5.5A.5.5 0 0 1 6 6v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5zm2.5 0a.5.5 0 0 1 .5.5v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5zm3 .5a.5.5 0 0 0-1 0v6a.5.5 0 0 0 1 0V6z\"/>";
      retour += "<path fill-rule=\"evenodd\" d=\"M14.5 3a1 1 0 0 1-1 1H13v9a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V4h-.5a1 1 0 0 1-1-1V2a1 1 0 0 1 1-1H6a1 1 0 0 1 1-1h2a1 1 0 0 1 1 1h3.5a1 1 0 0 1 1 1v1zM4.118 4 4 4.059V13a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.059L11.882 4H4.118zM2.5 3V2h11v1h-11z\"/>";
      retour += "</svg>";
      retour += "</a>";
      retour += "</form>";
      retour += "</div>\n";
    }

    return retour;
  }
  else if (var.startsWith("ProgrammatedTasks"))
  {
    String temp = var;
    temp.replace("ProgrammatedTasks", "");
    int separateur = temp.indexOf(":");
    int num_tache = temp.substring(0, separateur).toInt();
    String methode = temp.substring(separateur + 1, 64);
    if (methode == "getHours")
    {
      return ProgTasks.ListTasks->get(num_tache)->getHours();
    }
    if (methode == "getMinutes")
    {
      return ProgTasks.ListTasks->get(num_tache)->getMinutes();
    }
    if (methode == "isActive")
    {
      return String(ProgTasks.ListTasks->get(num_tache)->isActive() ? "checked" : "");
    }
    if (methode == "targetVanne")
    {
      return String(ProgTasks.ListTasks->get(num_tache)->targetVanne);
    }
    if (methode == "deepsleep")
    {
      return String(ProgTasks.ListTasks->get(num_tache)->deepsleep);
    }

    return "erreur processing ProgrammatedTasks";
  }
  else
  {
    retour += "Erreur processing task";
  }

  return retour;
}
