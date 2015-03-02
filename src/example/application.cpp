#include "swarm/Application.h"


class MyApplication
{
public:
  MyApplication(swarm::Application& application) : _application(application)
  {
    _application.setInitCallback(boost::bind(&MyApplication::initialize, this));
    _application.setUninitCallback(boost::bind(&MyApplication::uninitialize, this));
    _application.setReinitCallback(boost::bind(&MyApplication::reinitialize, this));
  }
  
  int MyMain(const std::vector<std::string>&)
  {
    return 0;
  }
  
  void initialize()
  {
    //
    // Configuration has been loaded at this point.
    // We can safely access configuration parameters from the _application
    // to determine proper initialization setup
    //
  }
  
  void uninitialize()
  {
  }
  
  void reinitialize()
  {
  }
  
  swarm::Application& _application;
};

int main(int argc, char** argv)
{
  swarm::Application swarmApp;
  MyApplication myApp(swarmApp);
  return swarmApp.run(boost::bind(&MyApplication::MyMain, &myApp, _1), argc, argv);
}
  
