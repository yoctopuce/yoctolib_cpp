/*********************************************************************
 *
 *  $Id: main.cpp 71694 2026-02-02 08:07:21Z seb $
 *
 *  Doc-Inventory example
 *
 *  You can find more information on our web site:
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include <iostream>

#include "yocto_api.h"

using namespace std;

int main(int argc, const char * argv[])
{
  string      errmsg;


  YRETCODE register_hub = YAPI::RegisterHub("secure://www.yoctopuce.com", errmsg);
  if(register_hub != YAPI::NOT_SUPPORTED) {
    cerr << "SSL supported is not activated: " << errmsg << endl;
    return 0;
  }

  YAPI::FreeAPI();
  return 1;
}
