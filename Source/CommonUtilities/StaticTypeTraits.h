#pragma once

namespace CU
{
	template <bool BOOLEAN>
	struct BoolType
	{
		static const bool Result = BOOLEAN;
	};

	struct FalseResult : BoolType<false> {};
	struct TrueResult : BoolType<true> {};

	template <typename LeftType, typename RightType>
	struct IsSame : FalseResult {};

	template <typename LeftType>
	struct IsSame<LeftType, LeftType> : TrueResult {};

	struct Nil
	{
		using ListHead = Nil;
		using ListTail = Nil;
	};

	template<typename LHead, typename LTail = Nil>
	struct List
	{
		using ListHead = LHead;
		using ListTail = LTail;
	};

	template<typename T, typename ListType>
	struct Find
	{
		static const bool Found = IsSame<T, ListType::ListHead>::Result;
		static const bool FoundInTail = Find<T, ListType::ListTail>::Result;
		static const bool Result = Found || FoundInTail;
	};

	template<typename T>
	struct Find<T, Nil> : FalseResult {};

	using IntegerTypes = List<char, List<short, List<long, List<long long>>>>;
	using UnsignedTypes = List<unsigned char, List<unsigned short, List<unsigned long, List<unsigned long long>>>>;
	using FloatTypes = List<float, List<double, List<long double>>>;
	using IntegralTypes = List<IntegerTypes, List<FloatTypes, List<UnsignedTypes>>>;

	template<typename T> struct IsInteger : BoolType<Find<T, IntegerTypes>::Result> {};
	template<typename T> struct IsUnsigned : BoolType<Find<T, UnsignedTypes>::Result> {};
	template<typename T> struct IsFloat : BoolType<Find<T, FloatTypes>::Result> {};
	template<typename T> struct IsIntegral : BoolType<IsInteger<T>::Result || IsUnsigned<T>::Result || IsFloat<T>::Result> {};

	template<typename T> struct IsPointer : FalseResult {};
	template<typename T> struct IsPointer<T*> : TrueResult {};

	template<typename T> struct IsPod : BoolType<IsPointer<T>::Result || IsIntegral<T>::Result> {};

	template<typename ObjectType, typename ListType>
	struct Append
	{
		using newList = List<ObjectType, ListType>;
	};

	template<typename ListType>
	struct Length
	{
		static const long long Result = 1 + Length<ListType::ListTail>::Result;
	};

	template<>
	struct Length<Nil>
	{
		static const long long Result = 0;
	};

	template<bool assumeFalse, typename if_true, typename if_false>
	struct IF
	{
		using ResultType = if_false;
	};

	template<typename if_true, typename if_false>
	struct IF<true, if_true, if_false>
	{
		using ResultType = if_true;
	};

	template<long long VALUE>
	struct LongLong
	{
		static const long long value = VALUE;
	};

	template<typename T, typename ListType>
	struct IndexOf
	{
		static const bool FoundInHead = IsSame<T, ListType::ListHead>::Result;
		static const long long headIndex = IF<FoundInHead, LongLong<1>, LongLong<0>>::ResultType::value;

		static const long long index = headIndex + IndexOf<T, ListType::ListTail>::index;
	};

	template<typename T>
	struct IndexOf<T, Nil>
	{
		static const long long index = 0;
	};

	template<typename T>
	long long GetType(const T& aObject)
	{
		using MyTailType = Nil;
		static const bool HasType = Find<T, MyTailType>::Result;
		using MyTypes = IF<HasType, MyTailType, Append<T, MyTailType>::newList>::ResultType;
		return IndexOf<T, MyTypes>::index;
	}
}
