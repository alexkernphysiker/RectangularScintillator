#ifndef iFOirRgb
#define iFOirRgb
#include <exception>
#include <string>
class RectScinException:public std::exception{
public:
    RectScinException(std::string wh);
	virtual ~RectScinException()throw(){}
	virtual const char* what() const throw();
private:
	std::string m_msg;
};
#endif