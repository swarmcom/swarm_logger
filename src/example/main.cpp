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

#include <boost/filesystem.hpp>
#include <iostream>

#include "swarm/Logger.h"


int main(int argc, char** argv) 
{
  boost::filesystem::path path("/home/joegen/Desktop/sample.log");
  
  swarm::Logger::instance()->open(path.string(), swarm::Logger::PRIO_TRACE);
  
  SWARM_LOG_FATAL("This is a sample FATAL log");
  SWARM_LOG_CRITICAL("This is a sample CRITICAL log");
  SWARM_LOG_ERROR("This is a sample ERROR log");
  SWARM_LOG_WARNING("This is a sample WARNING log");
  SWARM_LOG_NOTICE("This is a sample NOTICE log");
  SWARM_LOG_INFO("This is a sample INFO log");
  SWARM_LOG_DEBUG("This is a sample DEBUG log");
  SWARM_LOG_TRACE("This is a sample TRACE log");
  
  return 0;
}

