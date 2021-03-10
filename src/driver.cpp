#include"kernel.h"
#include"kersem.h"
#include"thread.h"

int userMain(int argc, char** argv);

class UserThread:public Thread
{
    public:
    int _argc;
    char** _argv;
    int _ret;
    UserThread(int argc, char** argv):Thread(), _argc(argc), _argv(argv){}
    void run()
    {
        _ret=userMain(_argc, _argv);
    }
};

int main(int argc, char** argv)
{
    running=&driverPCB;
    UserThread userThread(argc, argv);
    int r;
    inic();

    userThread.start();
    userThread.waitToComplete();
    r=userThread._ret;

    restore();

    cout<<"userMain finished with code: "<<r<<endl;

    return 0;
}
