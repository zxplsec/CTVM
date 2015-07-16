#include <iostream>
#include "ctvm.h"
#include "ctvm_util.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <Magick++.h>

int main(int argc, char **argv)
{
    /* Test CTVM.dylib Link */
    std::cout<<"Testing libctvm Link."<<std::endl;
    BoostDoubleMatrix DummySinogram(0,0);
    BoostDoubleVector DummyAngles(0);
    BoostDoubleMatrix DummyReconstruction = tval3_reconstruction(DummySinogram,DummyAngles);
    std::cout<<"    "<<"Passed."<<std::endl;
    
    /* Test CTVM_util.dylib Link */
    std::cout<<"Allocating tiny(3x3) random matrix..."<<std::endl;
    BoostDoubleMatrix RandomMatrix = CreateRandomMatrix(3,3);
    std::cout <<"    "<<RandomMatrix << std::endl;
    std::cout<<"    "<<"Passed."<<std::endl;

    std::cout<<"Rasterized Version..."<<std::endl;
    std::cout<<"    "<<MatrixToVector(RandomMatrix)<<std::endl;
    std::cout<<"    "<<"Passed."<<std::endl;

    std::cout<<"Back to Matrix..."<<std::endl;
    std::cout<<"    "<<VectorToMatrix(MatrixToVector(RandomMatrix),3,3)<<std::endl;
    std::cout<<"    "<<"Passed."<<std::endl;

    std::cout<<"Allocating large(1000x1000) random matrix..."<<std::endl;
    BoostDoubleMatrix RandomMatrixLarge = CreateRandomMatrix(1000,1000);
    std::cout<<"    "<<"Passed."<<std::endl;

    /* A Hardcoded Image Test */

    /* Test ImageMagick */
    std::cout<<"Testing ImageMagick Link."<<std::endl;
    using namespace Magick;
    InitializeMagick("");
    Image someImage;
    someImage.read("test/data/peppers.jpg");
    std::cout<<"    "<<"Passed."<<std::endl;

    /* Test CTVM Image Load */
    std::cout<<"Testing CTVM Image Load."<<std::endl;
    BoostDoubleMatrix ImageMatrix = LoadImage("test/data/peppers.jpg");
    std::cout<<"    "<<"Passed."<<std::endl;
}