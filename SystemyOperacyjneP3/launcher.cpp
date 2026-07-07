#include <unistd.h>
#include <iostream>

int main()
{
    for(int i=0;i<5;i++)
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            execl("./local", "local", nullptr);
        }
    }
    return 0;
}
