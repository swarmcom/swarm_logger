#include "Poco/Util/ServerApplication.h"
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/AbstractConfiguration.h>
#include "Poco/TaskManager.h"
#include <Poco/AutoPtr.h>
#include <iostream>
#include "swarm/Application.h"


namespace swarm
{
  using Poco::Util::ServerApplication;
  using Poco::Util::Option;
  using Poco::Util::OptionSet;
  using Poco::Util::OptionCallback;
  using Poco::Util::HelpFormatter;
  using Poco::Task;
  using Poco::TaskManager;

  class Worker: public Task
  {
  public:
    Worker(const Application::MainCallback& main, const std::vector<std::string>& args) : 
      Task("swarm::Daemon::Worker"),
      _main(main),
      _args(args)
    {
    }

    void runTask()
    {
      if (_main)
        _main(_args);
    }
    
    const Application::MainCallback _main;
    const std::vector<std::string> _args;
  };


  class Daemon: public ServerApplication
  {
  public:
    Daemon(swarm::Application& application) : 
      _application(application)
    {
    }

    ~Daemon()
    {
    }

  protected:
    void initialize(Application& self)
    {
      loadConfiguration(); // load default configuration files, if present
      ServerApplication::initialize(self);
      
      if (_application._initCallback)
        _application._initCallback();
    }
    
    void reinitialize(Application& self)
    {
      ServerApplication::reinitialize(self);
      if (_application._reinitCallback)
        _application._reinitCallback();
    }

    void uninitialize()
    {
      if (_application._uninitCallback)
        _application._uninitCallback();
      ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options)
    {
      ServerApplication::defineOptions(options);

      for (swarm::Application::OptionList::iterator iter = _application.options().begin(); iter != _application.options().end(); iter++)
      {
        Option item(iter->_fullName, iter->_shortName, iter->_description);
        
        item
          .required(iter->_required)
          .repeatable(iter->_repeatable)
          .argument(iter->_argName, iter->_argRequired);
        
        if (iter->_hasCallback)
          item.callback(OptionCallback<swarm::Application>(&_application, &swarm::Application::handleConfig));
        
        options.addOption(item);
      }
    }
    
    

    int main(const std::vector<std::string>& args)
    {

      if (!_application.stopProcessing())
      {
        TaskManager tm;
        tm.start(new Worker(_application.mainCallback(), args));
        waitForTerminationRequest();
        tm.cancelAll();
        tm.joinAll();
      }

      return Daemon::EXIT_OK;
    }

  private:
    swarm::Application& _application;
  };
  
  static Daemon* _pDaemon = 0;
  
  Application::Application()
  {
  }
  
  Application::~Application()
  {
    delete _pDaemon;
    _pDaemon = 0;
  }
  
  void Application::addOption(const std::string& fullName, const std::string& shortName, const OptionCallback& callback)
  {
    Option option;
    option._fullName = fullName;
    option._shortName = shortName;
    _options.push_back(option);
    
    if (callback)
    {
      option._hasCallback = true;
      _optionCallbacks[fullName] = callback;
    }
  }
  
  void Application::handleConfig(const std::string& name, const std::string& value)
  {
    OptionCallbackMap::iterator iter = _optionCallbacks.find(name);
    if (iter != _optionCallbacks.end())
      iter->second(name, value, _stopProcessing);
  }

  void Application::addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool required, const OptionCallback& callback)
  {
    Option option;
    option._fullName = fullName;
    option._shortName = shortName;
    option._description = description;
    option._required = required;
    _options.push_back(option);
    
    if (callback)
    {
      option._hasCallback = true;
      _optionCallbacks[fullName] = callback;
    }
  }

  void Application::addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool required, const std::string& argName, bool argRequired, const OptionCallback& callback)
  {
    Option option;
    option._fullName = fullName;
    option._shortName = shortName;
    option._description = description;
    option._required = required;
    option._argName = argName;
    option._argRequired = argRequired;
    
    if (callback)
    {
      option._hasCallback = true;
      _optionCallbacks[fullName] = callback;
    }
  }
  
  int Application::run(const MainCallback& callback, int argc, char** argv)
  {
    _mainCallback = callback;
    assert(!_pDaemon);
    _pDaemon = new Daemon(*this);
    return _pDaemon->run(argc, argv);
  }
      /// Run the application


} // swarm
