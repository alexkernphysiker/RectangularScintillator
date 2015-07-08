#include "rectscinexception.h"
RectScinException::RectScinException(std::string wh){
	m_msg=wh;
}
const char* RectScinException::what() const throw(){
	return m_msg.c_str();
}

