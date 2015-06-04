#include "MaxipixCamera.h"
#include "MaxipixReconstruction.h"



using namespace lima;
using namespace lima::Maxipix;
using namespace std;

DEB_GLOBAL(DebModTest);

int main()
{
DEB_GLOBAL_FUNCT();

  string p= "/users/blissadm/local/maxipix/tpxatl25";
  string u = "tpxatl25";
  Camera* cam;
  cam = new Camera(0,p,u, true);
  return(1);
}
