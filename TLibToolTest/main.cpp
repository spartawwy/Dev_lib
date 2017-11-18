#include "TLib/tool/tsystem_exe_frame.h"

#include <tuple>

#include "app.h"
#include "terminal_app.h"

#include <TLib/core/tsystem_core_paths.h>


class TheFrame
	: public TSystem::ExecutableFrame
{
public:
	int main(int argc, char* argv[]) override
	{
		TSystem::utility::ProjectTag("WZF");
        int rc = 0;
       /* std::tuple<int, int> tuple_temp;
        std::get<0>(tuple_temp) = 11;
        std::get<1>(tuple_temp) = 100;

        int val_temp = std::get<0>(tuple_temp);
        val_temp = val_temp;*/
#if 0
		App app;
		app.Start();
		PrintAppInfo(app);

		//rc = app.ProcessCmd(argc, argv);
		//app.Shutdown();
#else
        TerminalApp app;
        app.Initiate();
#endif
          
		app.WaitShutdown();
        
		system("pause");
		
		return rc;
	}
};

int main(int argc, char* argv[])
{
	TheFrame frame;
	return frame.LaunchServer(argc, argv);
}