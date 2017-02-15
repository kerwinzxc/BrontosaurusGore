#pragma once

#define BASIC_FUNC_PTR_SIZE sizeof(int*)
#define self (*this)

#include <functional>

template<bool IS_BIG, int FUNCTION_SIZE, typename RETURN_TYPE, typename... ARGS>
struct SFunctionImpl
{
	using ImplType = RETURN_TYPE(*)(ARGS...);

	SFunctionImpl() { memset(this, 0, sizeof(*this)); }
	~SFunctionImpl() {}

	ImplType Get() { return reinterpret_cast<ImplType>(&myImpl[0]); }
	void Set(const ImplType& aFuncPtr) { Get() = aFuncPtr; }
	
	char myImpl[FUNCTION_SIZE];
};

template<int FUNCTION_SIZE, typename RETURN_TYPE, typename... ARGS>
struct SFunctionImpl<true, FUNCTION_SIZE, RETURN_TYPE, ARGS...>
{
	using ImplType = RETURN_TYPE(*)(ARGS...);

	SFunctionImpl() { myImpl = new char[FUNCTION_SIZE]; memset(myImpl, 0, FUNCTION_SIZE); }
	~SFunctionImpl() { delete[] myImpl; }

	ImplType Get() { return static_cast<ImplType>(myImpl); }
	void Set(const ImplType& aFuncPtr) { memcpy(myImpl, aFuncPtr, FUNCTION_SIZE); }

	char* myImpl;
};

template<typename ReturnType, typename... Arguments>
class CFunction
{
public:
	CFunction();
	CFunction(const CFunction& aCopy);
	template<typename LAMBDA> CFunction(LAMBDA aLambda);
	~CFunction();

	inline ReturnType operator()(Arguments... aArguments);

	inline CFunction& operator=(const CFunction& aCopy);
	template<typename LAMBDA> inline CFunction& operator=(LAMBDA aLambda);

	inline bool operator==(const CFunction& aOther) const;
	inline bool operator==(const std::nullptr_t aPointer) const;

private:
	SFunctionImpl<false, BASIC_FUNC_PTR_SIZE, ReturnType, Arguments...> myImpl;
};

template<typename ReturnType, typename... Arguments>
CFunction<ReturnType, Arguments...>::CFunction()
{
}

template<typename ReturnType, typename ...Arguments>
CFunction<ReturnType, Arguments...>::CFunction(const CFunction& aCopy)
{
	self = aCopy;
}

template<typename ReturnType, typename ...Arguments>
template<typename LAMBDA>
inline CFunction<ReturnType, Arguments...>::CFunction(LAMBDA aLambda)
{
	self = aLambda;
}

template<typename ReturnType, typename ...Arguments>
CFunction<ReturnType, Arguments...>::~CFunction()
{
}

template<typename ReturnType, typename ...Arguments>
inline ReturnType CFunction<ReturnType, Arguments...>::operator()(Arguments... aArguments)
{
	return myImpl.Get()(aArguments...);
}

template<typename ReturnType, typename ...Arguments>
inline CFunction<ReturnType, Arguments...>& CFunction<ReturnType, Arguments...>::operator=(const CFunction& aCopy)
{
	myImpl = aCopy.myImpl;
	return self;
}

template<typename ReturnType, typename ...Arguments>
template<typename LAMBDA>
inline CFunction<ReturnType, Arguments...>& CFunction<ReturnType, Arguments...>::operator=(LAMBDA aLambda)
{
	myImpl = SFunctionImpl<(sizeof(LAMBDA) > 8), sizeof(LAMBDA), ReturnType, Args...>();
	return self;
}

template<typename ReturnType, typename ...Arguments>
inline bool CFunction<ReturnType, Arguments...>::operator==(const CFunction& aOther) const
{
	return myImpl.myImpl == aOther.myImpl.myImpl;
}

template<typename ReturnType, typename ...Arguments>
inline bool CFunction<ReturnType, Arguments...>::operator==(const std::nullptr_t aPointer) const
{
	return myImpl.myImpl == aPointer;
}

#undef self