#include "heavy_terminal.h"

using namespace std;

int main(int argc,char** argv)
{
    HeavyTerminal::setup(argc,argv);
    HeavyTerminal::start();

    return 1;
}