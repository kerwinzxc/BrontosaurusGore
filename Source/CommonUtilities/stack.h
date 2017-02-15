#pragma once

namespace CU
{
	template<typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray;

	template<typename TYPE, typename SIZE_T = int>
	class Stack
	{
	public:
		Stack();
		
		inline void Push(const TYPE& aValue);
		inline TYPE Pop();
		
		inline TYPE& Top();
		
		inline SIZE_T Size();

		inline void Clear();
	private:
		CU::GrowingArray<TYPE, SIZE_T> myStack;
	
	};
	
	template<typename TYPE, typename SIZE_T>
	Stack<TYPE, SIZE_T>::Stack()
	{
		myStack.Init(10);
	}
	
	template<typename TYPE, typename SIZE_T>
	void Stack<TYPE, SIZE_T>::Push(const TYPE &aValue)
	{
		myStack.Add(aValue);
	}
	
	template<typename TYPE, typename SIZE_T>
	TYPE Stack<TYPE, SIZE_T>::Pop()
	{
		assert(!(myStack.Size() <= 0 && "Stack is empty"));
		TYPE temp = myStack[myStack.Size() - 1];
		myStack.RemoveCyclicAtIndex(myStack.Size() - 1);
		return temp;
	}
	
	template<typename TYPE, typename SIZE_T>
	TYPE& Stack<TYPE, SIZE_T>::Top()
	{
		assert(!(myStack.Size() <= 0 && "Stack is empty"));
		return myStack[myStack.Size() - 1];
	}
	
	template<typename TYPE, typename SIZE_T>
	SIZE_T Stack<TYPE, SIZE_T>::Size()
	{
		return myStack.Size();
	}

	template <typename TYPE, typename SIZE_T>
	void Stack<TYPE, SIZE_T>::Clear()
	{
		myStack.RemoveAll();
	}

}