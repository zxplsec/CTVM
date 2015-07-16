#include <iostream>
#include "ctvm.h"
#include "ctvm_util.h"

int main(int argc, char **argv){
/* Program: ctvm-recover <sinogram-image> <tilt-angles> <recovered-output> 
-----------
*/
    using namespace std;

    /* Test Inputs */
    if(argc != 4){
        cout<<"Usage: ctvm-recover <sinogram-image> <tilt-angles> <recovered-output>"<<endl;
        return 0;
    }

    /* Get Filenames */
    char* SinogramFile = argv[1];
    char* TiltAngleFile = argv[2];
    char* RecoveredOutput = argv[3];

    /* Debugging */
    cout<<"SinogramFile: "<<SinogramFile<<endl;
    cout<<"TiltAngleFile: "<<TiltAngleFile<<endl;
    cout<<"RecoveredOutput: "<<RecoveredOutput<<endl;
    cout<<endl;
    /* Load Tilt Anlges */
    cout<<"Loading Tilt Angles."<<endl;
    BoostDoubleVector TiltAngles = ReadTiltAngles(TiltAngleFile);
    cout<<TiltAngles<<endl;

return 0;
}