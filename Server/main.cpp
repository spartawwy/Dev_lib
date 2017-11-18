#include "TLib/tool/tsystem_exe_frame.h"

#include "TLib/core/tsystem_core_paths.h"

#include "server_app.h"

#include "MyServer.h"

class TheFrame : public TSystem::ExecutableFrame
{
    int main(int argc, char* argv[]) override
    {
        TSystem::utility::ProjectTag(PROJECT_TAG_);

        //MyServer app("wzerelay", "0.1");
        MyServer app(argv[1], "0.1");
        app.Initiate();
		
		PrintAppInfo(app);
         
        app.RollState(); 
        app.WaitShutdown();

		//system("pause");

        return 0;
    }
};

int main(int argc, char* argv[])
{
    TheFrame   frame;
    frame.LaunchServer(argc, argv);

    return 0;
}