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

#include "swarm/Exception.h"


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
    typedef std::vector<std::string> Keys;
    
    Application();
    
    ~Application();
    
    void addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool repeatable, const OptionCallback& callback);
      /// Creates an option with the given properties.

    void addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool repeatable, bool required, const OptionCallback& callback);
      /// Creates an option with the given properties.

    void addOption(const std::string& fullName, const std::string& shortName, const std::string& description, bool repeatable, bool required, const std::string& argName, bool argRequired, const OptionCallback& callback);
      /// Creates an option with the given properties.
        
    int run(const MainCallback& callback, int argc, char** argv);
      /// Run the application
    
    void loadConfiguration(const std::string& path);
		/// Loads configuration information from the file specified by
		/// the given path. The file type is determined by the file
		/// extension. The following extensions are supported:
		///   - .properties - properties file
		///   - .ini        - initialization file
		///   - .xml        - XML file
		///
		/// Extensions are not case sensitive.
		///
		/// The configuration will be added to the application's 
		/// LayeredConfiguration.
    
    void setInitCallback(const InitCallback& callback);
      /// Set a callback to be called when application initializes
    
    void setUninitCallback(const InitCallback& callback);
      /// Set a callback to be called when application uninitialize
    
    void setReinitCallback(const InitCallback& callback);
      /// Set a callback to be called for application reinitialize.
      /// In Unix systems, this callback is called when a HUP signal is 
      /// intercepted.  If not set, a HUP signal will terminate the application
    
    void setTerminateCallback(const InitCallback& callback);
      /// Set a callback to be called for application termination signal
    
    void formatHelp(const std::string& usage, const std::string& header, std::ostream& strm);
      /// Format the registered options and write it to the output stream
    
    bool hasProperty(const std::string& key) const;
		/// Returns true iff the property with the given key exists.
	
    std::string getString(const std::string& key) const;
		/// Returns the string value of the property with the given name.
		/// Throws a NotFoundException if the key does not exist.
		/// If the value contains references to other properties (${<property>}), these
		/// are expanded.
		
    std::string getString(const std::string& key, const std::string& defaultValue) const;
		/// If a property with the given key exists, returns the property's string value,
		/// otherwise returns the given default value.
		/// If the value contains references to other properties (${<property>}), these
		/// are expanded.

    std::string getRawString(const std::string& key) const;
		/// Returns the raw string value of the property with the given name.
		/// Throws a NotFoundException if the key does not exist.
		/// References to other properties are not expanded.
		
    std::string getRawString(const std::string& key, const std::string& defaultValue) const;
		/// If a property with the given key exists, returns the property's raw string value,
		/// otherwise returns the given default value.
		/// References to other properties are not expanded.
		
    int getInt(const std::string& key) const;
		/// Returns the int value of the property with the g iven name.
		/// Throws a NotFoundException if the key does not exist.
		/// Throws a SyntaxException if the property can not be converted
		/// to an int.
		/// Numbers starting with 0x are treated as hexadecimal.
		/// If the value contains references to other properties (${<property>}), these
		/// are expanded.
		
    int getInt(const std::string& key, int defaultValue) const;
		/// If a property with the given key exists, returns the property's int value,
		/// otherwise returns the given default value.
		/// Throws a SyntaxException if the property can not be converted
		/// to an int.
		/// Numbers starting with 0x are treated as hexadecimal.
		/// If the value contains references to other properties (${<property>}), these
		/// are expanded.

    double getDouble(const std::string& key) const;
		/// Returns the double value of the property with the given name.
		/// Throws a NotFoundException if the key does not exist.
		/// Throws a SyntaxException if the property can not be converted
		/// to a double.
		/// If the value contains references to other properties (${<property>}), these
		/// are expanded.
		
    double getDouble(const std::string& key, double defaultValue) const;
		/// If a property with the given key exists, returns the property's double value,
		/// otherwise returns the given default value.
		/// Throws a SyntaxException if the property can not be converted
		/// to an double.
		/// If the value contains references to other properties (${<property>}), these
		/// are expanded.

    bool getBool(const std::string& key) const;
		/// Returns the boolean value of the property with the given name.
		/// Throws a NotFoundException if the key does not exist.
		/// Throws a SyntaxException if the property can not be converted
		/// to a boolean.
		/// If the value contains references to other properties (${<property>}), these
		/// are expanded.
		
    bool getBool(const std::string& key, bool defaultValue) const;
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
		
    void setString(const std::string& key, const std::string& value);
		/// Sets the property with the given key to the given value.
		/// An already existing value for the key is overwritten.
		
    void setInt(const std::string& key, int value);
		/// Sets the property with the given key to the given value.
		/// An already existing value for the key is overwritten.

    void setDouble(const std::string& key, double value);
		/// Sets the property with the given key to the given value.
		/// An already existing value for the key is overwritten.

    void setBool(const std::string& key, bool value);
		/// Sets the property with the given key to the given value.
		/// An already existing value for the key is overwritten.
		
    void keys(Keys& range) const;
		/// Returns in range the names of all keys at root level.

    void keys(const std::string& key, Keys& range) const;
		/// Returns in range the names of all subkeys under the given key.
		/// If an empty key is passed, all root level keys are returned.
    
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
    InitCallback _terminateCallback;
    
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
  
  inline void Application::setTerminateCallback(const InitCallback& callback)
  {
    _terminateCallback = callback;
  }


} // swarm


#endif	// SWARM_APPLICATION_H_INCLUDED

