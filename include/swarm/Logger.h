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

#ifndef SWARM_LOGGER_H_INCLUDED
#define	SWARM_LOGGER_H_INCLUDED


#include <ctime>
#include <string>
#include <sstream>
#include <boost/noncopyable.hpp>


namespace swarm
{
  class Logger : public boost::noncopyable
  {
  public:
    
    enum Priority
    {
      PRIO_FATAL = 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
      PRIO_CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
      PRIO_ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
      PRIO_WARNING,     /// A warning. An operation completed with an unexpected result.
      PRIO_NOTICE,      /// A notice, which is an information with just a higher priority.
      PRIO_INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
      PRIO_DEBUG,       /// A debugging message.
      PRIO_TRACE        /// A tracing message. This is the lowest priority.
    };

    Logger(const std::string& name);
    ///
    /// Creates a new logger
    ///
    
    ~Logger();
    ///
    /// Destroy the logger.  Close channel if open
    ///
    
    
    
    bool open(
      const std::string& path // path for the log file
    );
    ///
    /// Open the log file specified by path
    ///
    
    bool open(
      const std::string& path,  // path for the log file
      Priority priority // Log priority level
    );
    ///
    /// Open the log file specified by path
    ///
    
    bool open(
      const std::string& path,  // path for the log file
      Priority priority, // Log priority level
      const std::string& format // format for the log headers
    );
    ///
    /// Open the log file specified by path
    ///
    
    bool open(
      const std::string& path,  // path for the log file 
      Priority priority, // Log priority level
      const std::string& format, // format for the log headers
      unsigned int purgeCount // number of files to maintain during log rotation
    );
    ///
    /// Open the log file specified by path
    ///
       
    void close();
    ///
    /// Close the logging channel
    ///
    
    const std::string& getName() const;
    ///
    /// Returns the logger name specified in constructor
    ///
       
    const std::string& getLogFormat()const;
    ///
    /// Returns the log format specified in open.  Default: %h-%M-%S.%i: %t
    ///
    
    const std::string& getPath() const;
    ///
    /// Returns the path of the log file specified in open()
    ///
    
    unsigned int getPurgeCount() const;
    ///
    /// Returns the purge count specified in open())
    ///
    
    Priority getPriority() const;
    ///
    /// Returns the current priority level
    ///
    
    void setPriority(Priority priority);
    ///
    /// Set the priority level of the logger
    ///
    
    void fatal(const std::string& log);
    ///
    /// Log a message in fatal level 
    ///
    
    void critical(const std::string& log);
    ///
    /// Log a message in critical level 
    ///
    
    void error(const std::string& log);
    ///
    /// Log a message in error level 
    ///
    
    void warning(const std::string& log);
    ///
    /// Log a message in warning level 
    ///
    
    void notice(const std::string& log);
    ///
    /// Log a message in notice level 
    ///
    
    void information(const std::string& log);
    ///
    /// Log a message in info level 
    ///
    
    void debug(const std::string& log);
    ///
    /// Log a message in debug level 
    ///
    
    bool willLog(Priority priority) const;
    ///
    /// Return true if priority is >= _priority
    ///
    
    void trace(const std::string& log);
    ///
    /// Log a message in trace level 
    ///
    
    static Logger* instance();
    ///
    /// Returns the default logger instance.
    ///
    
    static void releaseInstance();
    ///
    /// Delete the default logger instance
    ///

    bool verifyLogFile();
    ///
    /// Ensure that the log file exists.
    /// if not, reopen it
    ///
  private:
    static Logger* _pLoggerInstance; /// Pointer to the default logger instance
    std::string _name; /// The logger name 
    std::string _format; /// Log format string
    std::string _path; /// Path of the log file
    bool _enableCompression; /// Flag to enable compression during rotation
    unsigned int _purgeCount; /// Number of log files to be maintained after last rotation
    Priority _priority; /// The log priority level
    unsigned int _instanceCount;  /// USed to reconstruct a new name for the logger
    std::string _internalName;  /// The internal name of this logger
    std::time_t _lastVerifyTime;
    bool _enableVerfication;
  };
  
  //
  // Inlines
  //
  
  inline const std::string& Logger::getName() const
  {
    return _name;
  }

  inline const std::string& Logger::getLogFormat() const
  {
    return _format;
  }

  inline const std::string& Logger::getPath() const
  {
    return _path;
  }

  inline unsigned int Logger::getPurgeCount() const
  {
    return _purgeCount;
  }
  
  inline Logger::Priority Logger::getPriority() const
  {
    return _priority;
  }
  
} // swarm

//
// Convenience Macros
//

#define SWARM_LOG_FATAL(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  swarm::Logger::instance()->fatal(strm.str()); \
}

#define SWARM_LOG_CRITICAL(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  swarm::Logger::instance()->critical(strm.str()); \
}

#define SWARM_LOG_ERROR(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  swarm::Logger::instance()->error(strm.str()); \
}

#define SWARM_LOG_WARNING(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  swarm::Logger::instance()->warning(strm.str()); \
}

#define SWARM_LOG_NOTICE(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  swarm::Logger::instance()->notice(strm.str()); \
}

#define SWARM_LOG_INFO(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  swarm::Logger::instance()->information(strm.str()); \
}

#define SWARM_LOG_DEBUG(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  swarm::Logger::instance()->debug(strm.str()); \
}

#define SWARM_LOG_TRACE(log) \
{ \
  std::ostringstream strm; \
  strm << log; \
  swarm::Logger::instance()->trace(strm.str()); \
}

#endif	/* LOGGER_H */

