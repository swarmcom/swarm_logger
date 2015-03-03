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


#include <typeinfo>

#include "swarm/Exception.h"


namespace swarm {

Exception::Exception(int code): _pNested(0), _code(code)
{
}


Exception::Exception(const std::string& msg, int code): _msg(msg), _pNested(0), _code(code)
{
}


Exception::Exception(const std::string& msg, const std::string& arg, int code): _msg(msg), _pNested(0), _code(code)
{
	if (!arg.empty())
	{
		_msg.append(": ");
		_msg.append(arg);
	}
}


Exception::Exception(const std::string& msg, const Exception& nested, int code): _msg(msg), _pNested(nested.clone()), _code(code)
{
}


Exception::Exception(const Exception& exc):
	std::exception(exc),
	_msg(exc._msg),
	_code(exc._code)
{
	_pNested = exc._pNested ? exc._pNested->clone() : 0;
}

Exception::Exception(const std::exception& exc):
  _pNested(0),
  _code(0)
{
	_msg = exc.what();
}

Exception::Exception(const boost::system::system_error& exc) :
  _pNested(0),
  _code(0)
{
  _msg = exc.what();
  _code = exc.code().value();
}
	
Exception::~Exception() throw()
{
	delete _pNested;
}


Exception& Exception::operator = (const Exception& exc)
{
	if (&exc != this)
	{
		delete _pNested;
		_msg     = exc._msg;
		_pNested = exc._pNested ? exc._pNested->clone() : 0;
		_code    = exc._code;
	}
	return *this;
}


Exception& Exception::operator = (const std::exception& exc)
{
  if (&exc != this)
	{
		delete _pNested;
    _pNested = 0;
    _code = 0;
    _msg = exc.what();
	}
	return *this;
}

Exception& Exception::operator = (const boost::system::system_error& exc)
{
  delete _pNested;
  _pNested = 0;
  _code = exc.code().value();
  _msg = exc.what();
	return *this;
}

const char* Exception::name() const throw()
{
	return "Exception";
}


const char* Exception::className() const throw()
{
	return typeid(*this).name();
}

	
const char* Exception::what() const throw()
{
  if(_msg.empty())
    return name();
  return _msg.c_str();
}

	
std::string Exception::displayText() const
{
	std::string txt = name();
	if (!_msg.empty())
	{
		txt.append(": ");
		txt.append(_msg);
	}
	return txt;
}


Exception* Exception::clone() const
{
	return new Exception(*this);
}


void Exception::rethrow() const
{
	throw *this;
}


SWARM_IMPLEMENT_EXCEPTION(LogicException, Exception, "Logic exception")
SWARM_IMPLEMENT_EXCEPTION(AssertionViolationException, LogicException, "Assertion violation")
SWARM_IMPLEMENT_EXCEPTION(NullPointerException, LogicException, "Null pointer")
SWARM_IMPLEMENT_EXCEPTION(BugcheckException, LogicException, "Bugcheck")
SWARM_IMPLEMENT_EXCEPTION(InvalidArgumentException, LogicException, "Invalid argument")
SWARM_IMPLEMENT_EXCEPTION(NotImplementedException, LogicException, "Not implemented")
SWARM_IMPLEMENT_EXCEPTION(RangeException, LogicException, "Out of range")
SWARM_IMPLEMENT_EXCEPTION(IllegalStateException, LogicException, "Illegal state")
SWARM_IMPLEMENT_EXCEPTION(InvalidAccessException, LogicException, "Invalid access")
SWARM_IMPLEMENT_EXCEPTION(SignalException, LogicException, "Signal received")
SWARM_IMPLEMENT_EXCEPTION(UnhandledException, LogicException, "Unhandled exception")

SWARM_IMPLEMENT_EXCEPTION(RuntimeException, Exception, "Runtime exception")
SWARM_IMPLEMENT_EXCEPTION(NotFoundException, RuntimeException, "Not found")
SWARM_IMPLEMENT_EXCEPTION(ExistsException, RuntimeException, "Exists")
SWARM_IMPLEMENT_EXCEPTION(TimeoutException, RuntimeException, "Timeout")
SWARM_IMPLEMENT_EXCEPTION(SystemException, RuntimeException, "System exception")
SWARM_IMPLEMENT_EXCEPTION(RegularExpressionException, RuntimeException, "Error in regular expression")
SWARM_IMPLEMENT_EXCEPTION(LibraryLoadException, RuntimeException, "Cannot load library")
SWARM_IMPLEMENT_EXCEPTION(LibraryAlreadyLoadedException, RuntimeException, "Library already loaded")
SWARM_IMPLEMENT_EXCEPTION(NoThreadAvailableException, RuntimeException, "No thread available")
SWARM_IMPLEMENT_EXCEPTION(PropertyNotSupportedException, RuntimeException, "Property not supported")
SWARM_IMPLEMENT_EXCEPTION(PoolOverflowException, RuntimeException, "Pool overflow")
SWARM_IMPLEMENT_EXCEPTION(NoPermissionException, RuntimeException, "No permission")
SWARM_IMPLEMENT_EXCEPTION(OutOfMemoryException, RuntimeException, "Out of memory")
SWARM_IMPLEMENT_EXCEPTION(DataException, RuntimeException, "Data error")

SWARM_IMPLEMENT_EXCEPTION(DataFormatException, DataException, "Bad data format")
SWARM_IMPLEMENT_EXCEPTION(SyntaxException, DataException, "Syntax error")
SWARM_IMPLEMENT_EXCEPTION(CircularReferenceException, DataException, "Circular reference")
SWARM_IMPLEMENT_EXCEPTION(PathSyntaxException, SyntaxException, "Bad path syntax")
SWARM_IMPLEMENT_EXCEPTION(IOException, RuntimeException, "I/O error")
SWARM_IMPLEMENT_EXCEPTION(FileException, IOException, "File access error")
SWARM_IMPLEMENT_EXCEPTION(FileExistsException, FileException, "File exists")
SWARM_IMPLEMENT_EXCEPTION(FileNotFoundException, FileException, "File not found")
SWARM_IMPLEMENT_EXCEPTION(PathNotFoundException, FileException, "Path not found")
SWARM_IMPLEMENT_EXCEPTION(FileReadOnlyException, FileException, "File is read-only")
SWARM_IMPLEMENT_EXCEPTION(FileAccessDeniedException, FileException, "Access to file denied")
SWARM_IMPLEMENT_EXCEPTION(CreateFileException, FileException, "Cannot create file")
SWARM_IMPLEMENT_EXCEPTION(OpenFileException, FileException, "Cannot open file")
SWARM_IMPLEMENT_EXCEPTION(WriteFileException, FileException, "Cannot write file")
SWARM_IMPLEMENT_EXCEPTION(ReadFileException, FileException, "Cannot read file")
SWARM_IMPLEMENT_EXCEPTION(UnknownURISchemeException, RuntimeException, "Unknown URI scheme")

SWARM_IMPLEMENT_EXCEPTION(NetException, IOException, "NET Exception");

SWARM_IMPLEMENT_EXCEPTION(ApplicationException, Exception, "Application exception")
SWARM_IMPLEMENT_EXCEPTION(BadCastException, RuntimeException, "Bad cast exception")

} // SWARM

