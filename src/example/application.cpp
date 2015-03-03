#include <iostream>
#include <sys/select.h>
#include "swarm/Application.h"


class MyApplication
{
public:
  MyApplication(swarm::Application& application) : 
    _application(application),
    _terminate(false)
  {
    _application.setInitCallback(boost::bind(&MyApplication::initialize, this));
    _application.setUninitCallback(boost::bind(&MyApplication::uninitialize, this));
    _application.setReinitCallback(boost::bind(&MyApplication::reinitialize, this));
    _application.setTerminateCallback(boost::bind(&MyApplication::terminate, this));
    
    //
    // Add the options we need
    //

    _application.addOption(
      "help", "h", "Display help information", 
      false,  // repeatable 
      false,  // required
      boost::bind(&MyApplication::handleHelp, this, _1, _2, _3));
    
    _application.addOption(
      "define", "D", "define a configuration property", 
      true,  // repeatable 
      false,  // required
      "key=value", //argName
      true, // argRequired
      boost::bind(&MyApplication::handleDefine, this, _1, _2, _3));
    
    _application.addOption(
      "config-file", "f", "load configuration data from a file", 
      true,  // repeatable 
      false,  // required
      "file-name", //argName
      true, // argRequired
      boost::bind(&MyApplication::handleConfig, this, _1, _2, _3));
    
  }
  
  void initialize()
  {
    //
    // Configuration has been loaded at this point.
    // We can safely access configuration parameters from the _application
    // to determine proper initialization setup
    //
    std::cout << "swarm::application::init" << std::endl;
  }
  
  void uninitialize()
  {
    std::cout << "swarm::application::uninit" << std::endl;
    
  }
  
  void reinitialize()
  {
    std::cout << "swarm::application::reinit" << std::endl;
  }
  
  void terminate()
  {
    _terminate = true;
    std::cout << "swarm::application::terminate" << std::endl;
  }
  
  void handleHelp(const std::string& name, const std::string& value, bool& stopProcessing)
  {
    (void)name;
    (void)value;
    _application.formatHelp("OPTIONS", "Sample Server Application", std::cout);
    stopProcessing = true; // tell the subsystem to stop processing anything after this
  }
  
  void handleConfig(const std::string& name, const std::string& value, bool& stopProcessing)
  {
    (void)name;
    (void)stopProcessing;
    
    _application.loadConfiguration(value);
  }
  
  void handleDefine(const std::string& name, const std::string& value, bool& stopProcessing)
  {
    (void)name;
    (void)stopProcessing;
    
    std::string prop_name;
		std::string prop_value;
		std::string::size_type pos = value.find('=');
		if (pos != std::string::npos)
		{
			prop_name.assign(value, 0, pos);
			prop_value.assign(value, pos + 1, value.length() - pos);
		}
		else 
    {
      prop_name = value;
    }
    
		_application.setString(prop_name, prop_value);
  }
  
  void thread_sleep( unsigned long milliseconds )
  {
    timeval sTimeout = { (long int)(milliseconds / 1000), (long int)(( milliseconds % 1000 ) * 1000) };
    select( 0, 0, 0, 0, &sTimeout );
  }
  
  void printProperties(const std::string& base)
	{
		swarm::Application::Keys keys;
		_application.keys(base, keys);
		if (keys.empty())
		{
			if (_application.hasProperty(base))
			{
				std::string msg;
				msg.append(base);
				msg.append(" = ");
				msg.append(_application.getString(base));
				std::cout << msg << std::endl;
			}
		}
		else
		{
			for (swarm::Application::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				std::string fullKey = base;
				if (!fullKey.empty()) fullKey += '.';
				fullKey.append(*it);
				printProperties(fullKey);
			}
		}
	}
  
  int MyMain(const std::vector<std::string>& args)
  {
    for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
      std::cout << *it;
    }
    
    printProperties("");
    
    while(!_terminate)
    {
      thread_sleep(1000);
    }
    
    std::cout << "swarm::application::main exiting..." << std::endl;
    return 0;
  }
  
  swarm::Application& _application;
  bool _terminate;
};

int main(int argc, char** argv)
{
  swarm::Application swarmApp;
  MyApplication myApp(swarmApp);
  return swarmApp.run(boost::bind(&MyApplication::MyMain, &myApp, _1), argc, argv);
}
  
