
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/AbstractConfiguration.h>
#include "Poco/TaskManager.h"
#include <Poco/AutoPtr.h>
#include <iostream>
#include "swarm/Application.h"

#if defined(POCO_OS_FAMILY_UNIX) 
#include <signal.h>
#endif


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
      if (_application.stopProcessing())
        return;
      
      loadConfiguration(); // load default configuration files, if present
      ServerApplication::initialize(self);
      
      if (_application._initCallback)
        _application._initCallback();
    }
    
    void reinitialize(Application& self)
    {
      if (_application.stopProcessing())
        return;
      
      ServerApplication::reinitialize(self);
      if (_application._reinitCallback)
        _application._reinitCallback();
    }

    void uninitialize()
    {
      if (_application.stopProcessing())
        return;
      
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
    
    //
    // Unix specific code
    //
#if defined(POCO_OS_FAMILY_UNIX) 
    int waitForTerminationRequest()
    {
      sigset_t sset;
      sigemptyset(&sset);
      sigaddset(&sset, SIGINT);
      sigaddset(&sset, SIGQUIT);
      sigaddset(&sset, SIGTERM);
      sigaddset(&sset, SIGHUP);
      sigprocmask(SIG_BLOCK, &sset, NULL);
      int sig;
      sigwait(&sset, &sig);
      return sig;
    }
#endif
    
    int main(const std::vector<std::string>& args)
    {

      if (!_application.stopProcessing())
      {
        TaskManager tm;
        tm.start(new Worker(_application.mainCallback(), args));
        
#if defined(POCO_OS_FAMILY_UNIX)        
        while (SIGHUP == waitForTerminationRequest())
        {
          if (!_application._reinitCallback)
            break;
          
          _application._reinitCallback();
        }
#else
        waitForTerminationRequest();
#endif
        if (_application._terminateCallback)
        {
          _application._terminateCallback();
        }
        
        tm.cancelAll();
        tm.joinAll();
      }

      return Daemon::EXIT_OK;
    }

  private:
    swarm::Application& _application;
  };
  
  static Daemon* _pDaemon = 0;
  
  Application::Application() :
    _stopProcessing(false)
  {
    assert(!_pDaemon);
    _pDaemon = new Daemon(*this);
  }
  
  Application::~Application()
  {
    delete _pDaemon;
    _pDaemon = 0;
  }
  
  void Application::addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool repeatable, const OptionCallback& callback)
  {
    Option option;
    option._fullName = fullName;
    option._shortName = shortName;
    option._description = description;
    option._repeatable = repeatable;
    if (callback)
    {
      option._hasCallback = true;
      _optionCallbacks[fullName] = callback;
    }
    
    _options.push_back(option);
  }

  void Application::addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool repeatable, bool required, const OptionCallback& callback)
  {
    Option option;
    option._fullName = fullName;
    option._shortName = shortName;
    option._description = description;
    option._repeatable = repeatable;
    option._required = required;
    if (callback)
    {
      option._hasCallback = true;
      _optionCallbacks[fullName] = callback;
    }
    
    _options.push_back(option);
  }

  void Application::addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool repeatable, bool required, const std::string& argName, bool argRequired, const OptionCallback& callback)
  {
    Option option;
    option._fullName = fullName;
    option._shortName = shortName;
    option._description = description;
    option._repeatable = repeatable;
    option._required = required;
    option._argName = argName;
    option._argRequired = argRequired;
    
    if (callback)
    {
      option._hasCallback = true;
      _optionCallbacks[fullName] = callback;
    }
    
    _options.push_back(option);
  }
  
  void Application::handleConfig(const std::string& name, const std::string& value)
  {
    OptionCallbackMap::iterator iter = _optionCallbacks.find(name);
    if (iter != _optionCallbacks.end())
      iter->second(name, value, _stopProcessing);
  }
  
  int Application::run(const MainCallback& callback, int argc, char** argv)
  {
    _mainCallback = callback;
     return _pDaemon->run(argc, argv);
  }
  
  void Application::loadConfiguration(const std::string& path)
  {
    _pDaemon->loadConfiguration(path);
  }
  
  
  void Application::formatHelp(const std::string& usage, const std::string& header, std::ostream& strm)
  {
    HelpFormatter helpFormatter(_pDaemon->options());
		helpFormatter.setCommand(_pDaemon->commandName());
		helpFormatter.setUsage(usage);
		helpFormatter.setHeader(header);
		helpFormatter.format(strm);
  }
  
  bool Application::hasProperty(const std::string& key) const
  {
    return _pDaemon->config().hasProperty(key);
  }

  std::string Application::getString(const std::string& key) const
  /// Returns the string value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
  {
    try
    {
      return _pDaemon->config().getString(key);
    }
    catch(...)
    {
      throw swarm::NotFoundException();
    }
  }

  std::string Application::getString(const std::string& key, const std::string& defaultValue) const
  /// If a property with the given key exists, returns the property's string value,
  /// otherwise returns the given default value.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
  {
    return _pDaemon->config().getString(key, defaultValue);
  }

  std::string Application::getRawString(const std::string& key) const
  /// Returns the raw string value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// References to other properties are not expanded.
  {
    try
    {
      return _pDaemon->config().getRawString(key);
    }
    catch(...)
    {
      throw swarm::NotFoundException();
    }
  }

  std::string Application::getRawString(const std::string& key, const std::string& defaultValue) const
  /// If a property with the given key exists, returns the property's raw string value,
  /// otherwise returns the given default value.
  /// References to other properties are not expanded.
  {
    return _pDaemon->config().getRawString(key, defaultValue);
  }

  int Application::getInt(const std::string& key) const
  /// Returns the int value of the property with the g iven name.
  /// Throws a NotFoundException if the key does not exist.
  /// Throws a SyntaxException if the property can not be converted
  /// to an int.
  /// Numbers starting with 0x are treated as hexadecimal.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
  {
    try
    {
      return _pDaemon->config().getInt(key);
    }
    catch(const Poco::NotFoundException&)
    {
      throw swarm::NotFoundException();
    }
    catch(...)
    {
      throw swarm::SyntaxException();
    }
  }

  int Application::getInt(const std::string& key, int defaultValue) const
  /// If a property with the given key exists, returns the property's int value,
  /// otherwise returns the given default value.
  /// Throws a SyntaxException if the property can not be converted
  /// to an int.
  /// Numbers starting with 0x are treated as hexadecimal.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
  {
    try
    {
      return _pDaemon->config().getInt(key, defaultValue);
    }
    catch(...)
    {
      throw swarm::SyntaxException();
    }
  }

  double Application::getDouble(const std::string& key) const
  /// Returns the double value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// Throws a SyntaxException if the property can not be converted
  /// to a double.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
  {
    try
    {
      return _pDaemon->config().getDouble(key);
    }
    catch(const Poco::NotFoundException&)
    {
      throw swarm::NotFoundException();
    }
    catch(...)
    {
      throw swarm::SyntaxException();
    }
  }

  double Application::getDouble(const std::string& key, double defaultValue) const
  /// If a property with the given key exists, returns the property's double value,
  /// otherwise returns the given default value.
  /// Throws a SyntaxException if the property can not be converted
  /// to an double.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
  {
    try
    {
      return _pDaemon->config().getDouble(key, defaultValue);
    }
    catch(...)
    {
      throw swarm::SyntaxException();
    }
  }

  bool Application::getBool(const std::string& key) const
  /// Returns the boolean value of the property with the given name.
  /// Throws a NotFoundException if the key does not exist.
  /// Throws a SyntaxException if the property can not be converted
  /// to a boolean.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
  {
    try
    {
      return _pDaemon->config().getBool(key);
    }
    catch(const Poco::NotFoundException&)
    {
      throw swarm::NotFoundException();
    }
    catch(...)
    {
      throw swarm::SyntaxException();
    }
  }

  bool Application::getBool(const std::string& key, bool defaultValue) const
  /// If a property with the given key exists, returns the property's boolean value,
  /// otherwise returns the given default value.
  /// Throws a SyntaxException if the property can not be converted
  /// to a boolean.
  /// The following string values can be converted into a boolean:
  ///   - numerical values: non zero becomes true, zero becomes false
  ///   - strings: true, yes, on become true, false, no, off become false
  /// Case does not matter.
  /// If the value contains references to other properties (${<property>}), these
  /// are expanded.
  {
    try
    {
      return _pDaemon->config().getBool(key, defaultValue);
    }
    catch(...)
    {
      throw swarm::SyntaxException();
    }
  }

  void Application::setString(const std::string& key, const std::string& value)
  /// Sets the property with the given key to the given value.
  /// An already existing value for the key is overwritten.
  {
    _pDaemon->config().setString(key, value);
  }

  void Application::setInt(const std::string& key, int value)
  /// Sets the property with the given key to the given value.
  /// An already existing value for the key is overwritten.
  {
    _pDaemon->config().setInt(key, value);
  }

  void Application::setDouble(const std::string& key, double value)
  /// Sets the property with the given key to the given value.
  /// An already existing value for the key is overwritten.
  {
    _pDaemon->config().setDouble(key, value);
  }

  void Application::setBool(const std::string& key, bool value)
  /// Sets the property with the given key to the given value.
  /// An already existing value for the key is overwritten.
  {
    _pDaemon->config().setBool(key, value);
  }

  void Application::keys(Keys& range) const
  /// Returns in range the names of all keys at root level.
  {
    _pDaemon->config().keys(range);
  }

  void Application::keys(const std::string& key, Keys& range) const
  /// Returns in range the names of all subkeys under the given key.
  /// If an empty key is passed, all root level keys are returned.
  {
    _pDaemon->config().keys(key, range);
  }



} // swarm
