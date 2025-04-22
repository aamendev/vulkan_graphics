#include "window.h"
#include "application.h"
#include <cstdlib>
#include <ctime>
int main()
{
    srand(time(NULL));
    Lina::App app;
    app.run();
    return 0;
}
