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


#ifndef SWARM_EXCEPTION_H_INCLUDED
#define SWARM_EXCEPTION_H_INCLUDED


#include <stdexcept>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>



namespace swarm {


class Exception: public std::exception
	/// This is the base class for all exceptions defined
	/// in the SWARM class library.
{
public:
	Exception(const std::string& msg, int code = 0);
		/// Creates an exception.

	Exception(const std::string& msg, const std::string& arg, int code = 0);
		/// Creates an exception.

	Exception(const std::string& msg, const Exception& nested, int code = 0);
		/// Creates an exception and stores a clone
		/// of the nested exception.

	Exception(const Exception& exc);
		/// Copy constructor.

  Exception(const std::exception& exc);
		/// Copy constructor.

  Exception(const boost::system::system_error& exc);
		/// Copy constructor.
		
	~Exception() throw();
		/// Destroys the exception and deletes the nested exception.

	Exception& operator = (const Exception& exc);
		/// Assignment operator.

  Exception& operator = (const std::exception& exc);
    /// Assignment operator.

  Exception& operator = (const boost::system::system_error& exc);

	virtual const char* name() const throw();
		/// Returns a static string describing the exception.
		
	virtual const char* className() const throw();
		/// Returns the name of the exception class.
		
	virtual const char* what() const throw();
		/// Returns a static string describing the exception.
		///
		/// Same as name(), but for compatibility with std::exception.
		
	const Exception* nested() const;
		/// Returns a pointer to the nested exception, or
		/// null if no nested exception exists.
			
	const std::string& message() const;
		/// Returns the message text.
			
	int code() const;
		/// Returns the exception code if defined.
		
	std::string displayText() const;
		/// Returns a string consisting of the
		/// message name and the message text.

	virtual Exception* clone() const;
		/// Creates an exact copy of the exception.
		///
		/// The copy can later be thrown again by
		/// invoking rethrow() on it.
		
	virtual void rethrow() const;
		/// (Re)Throws the exception.
		///
		/// This is useful for temporarily storing a
		/// copy of an exception (see clone()), then
		/// throwing it again.

protected:
	Exception(int code = 0);
		/// Standard constructor.
		
private:
	std::string _msg;
	Exception*  _pNested;
	int			_code;
};


//
// inlines
//
inline const Exception* Exception::nested() const
{
	return _pNested;
}


inline const std::string& Exception::message() const
{
	return _msg;
}


inline int Exception::code() const
{
	return _code;
}


//
// Macros for quickly declaring and implementing exception classes.
// Unfortunately, we cannot use a template here because character
// pointers (which we need for specifying the exception name)
// are not allowed as template arguments.
//
#define SWARM_DECLARE_EXCEPTION(CLS, BASE) \
	class CLS: public BASE														\
	{																				\
	public:																			\
		CLS(int code = 0);															\
		CLS(const std::string& msg, int code = 0);									\
		CLS(const std::string& msg, const std::string& arg, int code = 0);			\
		CLS(const std::string& msg, const swarm::Exception& exc, int code = 0);		\
		CLS(const CLS& exc);														\
		~CLS() throw();																\
		CLS& operator = (const CLS& exc);											\
		const char* name() const throw();											\
		const char* className() const throw();										\
		swarm::Exception* clone() const;												\
		void rethrow() const;														\
	};


#define SWARM_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)	\
	CLS::CLS(int code): BASE(code)		\
	{					\
	}					\
	CLS::CLS(const std::string& msg, int code): BASE(msg, code)	\
	{					\
	}					\
	CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(msg, arg, code)		\
	{					\
	}					\
	CLS::CLS(const std::string& msg, const swarm::Exception& exc, int code): BASE(msg, exc, code)	\
	{					\
	}					\
	CLS::CLS(const CLS& exc): BASE(exc)	\
	{					\
	}					\
	CLS::~CLS() throw()			\
	{					\
	}					\
	CLS& CLS::operator = (const CLS& exc)			\
	{					\
		BASE::operator = (exc);	\
		return *this;	\
	}	\
	const char* CLS::name() const throw()	\
	{				\
		return NAME;	\
	}		\
	const char* CLS::className() const throw() \
	{																								\
		return typeid(*this).name();	\
	}				\
	swarm::Exception* CLS::clone() const \
	{																								\
		return new CLS(*this); 	\
	}			\
	void CLS::rethrow() const \
	{			\
		throw *this;	\
	}


#define SWARM_CREATE_INLINE_EXCEPTION(CLS, BASE, NAME) \
	class CLS: public BASE	{ \
	public: \
		CLS(int code = 0); \
		CLS(const std::string& msg, int code = 0); \
		CLS(const std::string& msg, const std::string& arg, int code = 0); \
		CLS(const std::string& msg, const swarm::Exception& exc, int code = 0); \
		CLS(const CLS& exc); \
		~CLS() throw();																\
		CLS& operator = (const CLS& exc); \
		const char* name() const throw(); \
		const char* className() const throw();	\
		swarm::Exception* clone() const;	\
		void rethrow() const; \
	}; \
        inline CLS::CLS(int code): BASE(code)		\
	{		                    \
	}			            \
	inline CLS::CLS(const std::string& msg, int code): BASE(msg, code) \
	{			\
	}			\
	inline CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(msg, arg, code)		\
	{ \
	}  \
	inline CLS::CLS(const std::string& msg, const swarm::Exception& exc, int code): BASE(msg, exc, code)	\
	{	\
	}       \
	inline CLS::CLS(const CLS& exc): BASE(exc)	\
	{						\
	}						\
	inline CLS::~CLS() throw()			\
	{						\
	}						\
	inline CLS& CLS::operator = (const CLS& exc)	\
	{						\
		BASE::operator = (exc);			\
		return *this;				\
	}						\
	inline const char* CLS::name() const throw()	\
	{						\
		return NAME;				\
	}						\
	inline const char* CLS::className() const throw()		\
	{								\
		return typeid(*this).name();                            \
	}								\
	inline swarm::Exception* CLS::clone() const			\
	{								\
		return new CLS(*this);					\
	}								\
	inline void CLS::rethrow() const				\
	{								\
		throw *this;						\
	}


//
// Standard exception classes
//
SWARM_DECLARE_EXCEPTION(LogicException, Exception)
SWARM_DECLARE_EXCEPTION(AssertionViolationException, LogicException)
SWARM_DECLARE_EXCEPTION(NullPointerException, LogicException)
SWARM_DECLARE_EXCEPTION(BugcheckException, LogicException)
SWARM_DECLARE_EXCEPTION(InvalidArgumentException, LogicException)
SWARM_DECLARE_EXCEPTION(NotImplementedException, LogicException)
SWARM_DECLARE_EXCEPTION(RangeException, LogicException)
SWARM_DECLARE_EXCEPTION(IllegalStateException, LogicException)
SWARM_DECLARE_EXCEPTION(InvalidAccessException, LogicException)
SWARM_DECLARE_EXCEPTION(SignalException, LogicException)
SWARM_DECLARE_EXCEPTION(UnhandledException, LogicException)

SWARM_DECLARE_EXCEPTION(RuntimeException, Exception)
SWARM_DECLARE_EXCEPTION(NotFoundException, RuntimeException)
SWARM_DECLARE_EXCEPTION(ExistsException, RuntimeException)
SWARM_DECLARE_EXCEPTION(TimeoutException, RuntimeException)
SWARM_DECLARE_EXCEPTION(SystemException, RuntimeException)
SWARM_DECLARE_EXCEPTION(RegularExpressionException, RuntimeException)
SWARM_DECLARE_EXCEPTION(LibraryLoadException, RuntimeException)
SWARM_DECLARE_EXCEPTION(LibraryAlreadyLoadedException, RuntimeException)
SWARM_DECLARE_EXCEPTION(NoThreadAvailableException, RuntimeException)
SWARM_DECLARE_EXCEPTION(PropertyNotSupportedException, RuntimeException)
SWARM_DECLARE_EXCEPTION(PoolOverflowException, RuntimeException)
SWARM_DECLARE_EXCEPTION(NoPermissionException, RuntimeException)
SWARM_DECLARE_EXCEPTION(OutOfMemoryException, RuntimeException)
SWARM_DECLARE_EXCEPTION(DataException, RuntimeException)

SWARM_DECLARE_EXCEPTION(DataFormatException, DataException)
SWARM_DECLARE_EXCEPTION(SyntaxException, DataException)
SWARM_DECLARE_EXCEPTION(CircularReferenceException, DataException)
SWARM_DECLARE_EXCEPTION(PathSyntaxException, SyntaxException)
SWARM_DECLARE_EXCEPTION(IOException, RuntimeException)
SWARM_DECLARE_EXCEPTION(FileException, IOException)
SWARM_DECLARE_EXCEPTION(FileExistsException, FileException)
SWARM_DECLARE_EXCEPTION(FileNotFoundException, FileException)
SWARM_DECLARE_EXCEPTION(PathNotFoundException, FileException)
SWARM_DECLARE_EXCEPTION(FileReadOnlyException, FileException)
SWARM_DECLARE_EXCEPTION(FileAccessDeniedException, FileException)
SWARM_DECLARE_EXCEPTION(CreateFileException, FileException)
SWARM_DECLARE_EXCEPTION(OpenFileException, FileException)
SWARM_DECLARE_EXCEPTION(WriteFileException, FileException)
SWARM_DECLARE_EXCEPTION(ReadFileException, FileException)
SWARM_DECLARE_EXCEPTION(UnknownURISchemeException, RuntimeException)

SWARM_DECLARE_EXCEPTION(ApplicationException, Exception)
SWARM_DECLARE_EXCEPTION(BadCastException, RuntimeException)

SWARM_DECLARE_EXCEPTION(NetException, IOException)

} // namespace swarm


#endif //SWARM_EXCEPTION_H_INCLUDED

