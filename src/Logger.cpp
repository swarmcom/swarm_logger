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


#include <boost/thread.hpp>
#include "Poco/AutoPtr.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Message.h"
#include "Poco/Logger.h"
#include "Poco/Timestamp.h"
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>

#include "swarm/Logger.h"

namespace swarm
{
 
  static const std::string LOGGER_DEFAULT_NAME = "SwarmLogger";
  static const std::string LOGGER_DEFAULT_FORMAT = "%h-%M-%S.%i: %t"; 
  static const Logger::Priority LOGGER_DEFAULT_PRIORITY = Logger::PRIO_INFORMATION;
  static const unsigned int LOGGER_DEFAULT_PURGE_COUNT = 0; /// Disable log rotatepoco
  static unsigned int DEFAULT_VERIFY_TTL = 5; /// TTL in seconds for verification to kick in

  Logger* Logger::_pLoggerInstance = 0;
  
  Logger* Logger::instance()
  {
    if (!Logger::_pLoggerInstance)
    {
      Logger::_pLoggerInstance = new Logger(LOGGER_DEFAULT_NAME);
    }
    return Logger::_pLoggerInstance;
  }

  void Logger::releaseInstance()
  {
    delete Logger::_pLoggerInstance;
    Logger::_pLoggerInstance = 0;
  }
   
  Logger::Logger(const std::string& name) :
    _name(name),
    _instanceCount(0),
    _lastVerifyTime(0)
  {
    std::ostringstream strm;
    strm << _name << "-" << _instanceCount;
    _internalName = strm.str();
  }

  Logger::~Logger()
  {
    close();
  }


  bool Logger::open(
    const std::string& path 
  )
  {
    return open(path, LOGGER_DEFAULT_PRIORITY);
  }
  
  bool Logger::open(
    const std::string& path,
    Priority priority
  )
  {
    return open(path, priority, LOGGER_DEFAULT_FORMAT);
  }

  bool Logger::open(
    const std::string& path,  
    Priority priority,
    const std::string& format 
  )
  {
    return open(path, priority, format, LOGGER_DEFAULT_PURGE_COUNT);
  }

  bool Logger::open(
    const std::string& path,  
    Priority priority,
    const std::string& format, 
    unsigned int purgeCount 
  )
  {
    try
    {
      _path = path;
      _priority = priority;
      _format = format;
      _purgeCount = purgeCount;

      bool enableLogRotate = LOGGER_DEFAULT_PURGE_COUNT > 0;
      std::string strPurgeCount = boost::lexical_cast<std::string>(purgeCount);
      Poco::AutoPtr<Poco::FileChannel> fileChannel(new Poco::FileChannel(path));

      if (enableLogRotate)
      {
        fileChannel->setProperty("rotation", "daily");
        fileChannel->setProperty("archive", "timestamp");
        fileChannel->setProperty("compress", "true");
        fileChannel->setProperty("purgeCount", strPurgeCount);
      }

      Poco::AutoPtr<Poco::Formatter> formatter(new Poco::PatternFormatter(format.c_str()));
      Poco::AutoPtr<Poco::Channel> formattingChannel(new Poco::FormattingChannel(formatter, fileChannel));
      Poco::Logger::create(_internalName, formattingChannel, priority);
      
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        std::ostringstream log;
        log << "Logger::open(" << _internalName << ") path: " << _path;
        pLogger->notice(log.str());
      }
      else
      {
        return false;
      }
    }
    catch(const std::exception& e)
    {
      std::cerr << "Logger::open(" << _internalName << ") << exception: " << e.what() << std::endl;
      close();
      return false;
    }
    catch(...)
    {
      std::cerr << "Logger::open unknown exception" << std::endl;
      close();
      return false;
    }
    
    return true;
  }

  void Logger::close()
  {
    Poco::Logger::destroy(_internalName);
    _path = "";
  }
 
  void Logger::setPriority(Logger::Priority priority)
  {
    _priority = priority;
    Poco::Logger* pLogger = Poco::Logger::has(_internalName);
    if (pLogger)
    {
      pLogger->setLevel(priority);
    }
  }
  
  bool Logger::willLog(Priority priority) const
  {
    return priority <= _priority;
  }

  void Logger::fatal(const std::string& log)
  {
    if (willLog(PRIO_FATAL) && verifyLogFile())
    {
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        pLogger->fatal(log);
      }
    }
  }

  void Logger::critical(const std::string& log)
  {
    if (willLog(PRIO_CRITICAL) && verifyLogFile())
    {
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        pLogger->critical(log);
      }
    }
  }

  void Logger::error(const std::string& log)
  {
    if (willLog(PRIO_ERROR) && verifyLogFile())
    {
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        pLogger->error(log);
      }
    }
  }

  void Logger::warning(const std::string& log)
  {
    if (willLog(PRIO_WARNING) && verifyLogFile())
    {
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        pLogger->warning(log);
      }
    }
  }

  void Logger::notice(const std::string& log)
  {
    if (willLog(PRIO_NOTICE) && verifyLogFile())
    {
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        pLogger->notice(log);
      }
    }
  }

  void Logger::information(const std::string& log)
  {
    if (willLog(PRIO_INFORMATION) && verifyLogFile())
    {
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        pLogger->information(log);
      }
    }
  }

  void Logger::debug(const std::string& log)
  {
    if (willLog(PRIO_DEBUG) && verifyLogFile())
    {
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        pLogger->debug(log);
      }
    }
  }

  void Logger::trace(const std::string& log)
  {
    if (willLog(PRIO_TRACE) && verifyLogFile())
    {
      Poco::Logger* pLogger = Poco::Logger::has(_internalName);
      if (pLogger)
      {
        pLogger->trace(log);
      }
    }
  }
  
  bool Logger::verifyLogFile()
  {
    Poco::Timestamp timeStamp;
    std::time_t now = timeStamp.epochTime();
    
    if (now - _lastVerifyTime < DEFAULT_VERIFY_TTL)
    {
      return true; // TTL for this file has not yet expired
    }
    
    _lastVerifyTime = now;
    
    if (!_path.empty() && !boost::filesystem::exists(_path))
    {
      //
      // Make sure we preserve the _path because close() will empty it out.
      //
      std::string oldPath = _path;    
      close();
      
      ++_instanceCount;
      std::ostringstream strm;
      strm << _name << "-" << _instanceCount;
      _internalName = strm.str();
      
      return open(oldPath, _priority, _format, _purgeCount); 
    }

    return !_path.empty();
  }

} /// swarm


