//
// Copyright (c) eZuce, Inc.
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare 
// derivative works of the Software, all subject to the 
// "GNU Lesser General Public License (LGPL)".
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef SWARM_APPLICATION_H_INCLUDED
#define	SWARM_APPLICATION_H_INCLUDED


#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <Poco/Util/OptionSet.h>


namespace swarm
{
  
  
  class Application : boost::noncopyable
  {
  public:
       
    enum Type
    {
      DAEMON,
      COMMANDLINE
    };
    
    struct Option
    {
      std::string _shortName;
      std::string _fullName;
      std::string _description;
      std::string _argName;
      bool _required;
      bool _repeatable;
      bool _argRequired;
      bool _hasCallback;

      Option() :
        _shortName(),
        _fullName(),
        _description(),
        _argName(),
        _required(false),
        _argRequired(false),
        _hasCallback(false)
      {
      }
    };
  
    typedef boost::function<void(const std::string&, const std::string&, bool&)> OptionCallback;
    typedef std::map<std::string, OptionCallback> OptionCallbackMap;
    typedef std::vector<Option> OptionList;
    typedef boost::function<int(const std::vector<std::string>& args)> MainCallback;
    typedef boost::function<void()> InitCallback;
    
    Application();
    
    ~Application();
    
    void addOption(const std::string& fullName, const std::string& shortName, const OptionCallback& callback = OptionCallback());
      /// Creates an option with the given properties.

    void addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool required, const OptionCallback& callback = OptionCallback());
      /// Creates an option with the given properties.

    void addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool required, const std::string& argName, bool argRequired, const OptionCallback& callback = OptionCallback());
      /// Creates an option with the given properties.
        
    int run(const MainCallback& callback, int argc, char** argv);
      /// Run the application
    
    void setInitCallback(const InitCallback& callback);
      /// Set a callback to be called when application initializes
    
    void setUninitCallback(const InitCallback& callback);
      /// Set a callback to be called when application uninitialize
    
    void setReinitCallback(const InitCallback& callback);
      /// Set a callback to be called for application reinitialize
    
  protected:
    friend class Daemon;
    
    OptionCallbackMap& optionCallbacks();
    
    OptionList& options();
    
    MainCallback& mainCallback();
    
    void handleConfig(const std::string& name, const std::string& value);
    
    bool stopProcessing() const;
    
    bool& stopProcessing();
    
  protected:
    InitCallback _initCallback;
    InitCallback _uninitCallback;
    InitCallback _reinitCallback;
    
  private:
    OptionCallbackMap _optionCallbacks;
    OptionList _options;
    MainCallback _mainCallback;
    bool _stopProcessing;
  };
  
  //
  // Inlines
  //
  
  inline Application::OptionCallbackMap& Application::optionCallbacks()
  {
    return _optionCallbacks;
  }
  
  inline Application::OptionList& Application::options()
  {
    return _options;
  }
  
  inline Application::MainCallback& Application::mainCallback()
  {
    return _mainCallback;
  }
  
  inline bool Application::stopProcessing() const
  {
    return _stopProcessing;
  }
    
  inline bool& Application::stopProcessing()
  {
    return _stopProcessing;
  }
  
  inline void Application::setInitCallback(const InitCallback& callback)
  {
    _initCallback = callback;
  }

  inline void Application::setUninitCallback(const InitCallback& callback)
  {
    _uninitCallback = callback;
  }

  inline void Application::setReinitCallback(const InitCallback& callback)
  {
    _reinitCallback = callback;
  }

} // swarm


#endif	// SWARM_APPLICATION_H_INCLUDED

