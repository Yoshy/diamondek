// (C) 2011 PowerBIT Studio
/*! 
	\class Diamondek::Singleton
    \brief Singleton class

    Simple implementation of a singleton pattern.
*/

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

namespace Diamondek {

template <typename T> class Singleton {
public:
	static T* getSingleton(void) { if (_singleton == NULL) _singleton = new T(); return _singleton; };
	//static T& getSingleton(void) { assert(_singleton); return *_singleton; };
protected:
	Singleton(){ assert(_singleton == NULL); };
private:
	Singleton(T&) { assert(false); };
	T& operator= (const T&) { };

	static T* _singleton;
};

template <typename T>  T* Singleton<T>::_singleton = NULL;

}; // namespace Vox2D

#endif // _SINGLETON_H_
