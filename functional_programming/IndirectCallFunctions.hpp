#pragma once
/*
	IndirectCallFunctions.hpp
*/
#include <vector> 

#include <string>

#include <iostream>
using std::cout; using std::endl; using std::boolalpha; using std::cerr;

#include <functional>
using namespace std::placeholders;

#include <future>

#include "../include/type_names_tester.hpp"
using namespace chat_gpt_seh;

/*
	https://goldenandy.blogspot.com/2021/09/func-ptr-in-c.html
	Косвенное обращение к функции в языке Си
	
	При помощи указателя можно получить доступ к содержимому переменной. 
	
	Это просто.

	Аналогичный способ существует и для обращения к функции. И он тоже не сложный.

	Очевидно, что реализация этого способа требует определения указателя на функцию. 

	Делается это достаточно просто, путем объявления переменной соответствующего типа:
*/
#if 0	
int (*ptr_to_func)(int, int);			// 	Объявление указателя (собственно указателя) 
									//	на функцию int(int, int)

typedef int (*type_ptr_to_func)(int, int);	// NOLINT(modernize-use-using)
										//	здесь есть тонкость: 
										//	предыдущая строка объявляет 
										//	ТИП указателя на функцию,
										//	возвращающую `int`
										//	и принимающую два параметра типа `int`
type_ptr_to_func		function_ptr;
#endif	
/*
	Этот пример объявляет переменную `ptr_to_func` -
	`переменную` типа "указатель на функцию", 
	`возвращающую результат` типа `int` и имеющую 2 `параметра` типа `int`. 
	
	Сделанная запись 
	type_ptr_to_func		function_ptr;
	– не что иное, как `объявление переменной` `function_ptr` с новым типом `type_ptr_to_func`.
*/	
/*
	Обращение к `функции по указателю` еще проще, 
	чем обращение к `переменной по указателю` - 
	в данном случае операция разыменования указателя не используется:

	i = ptr_to_func(3, 12);

	Но следует помнить, что в данном случае 
	`переменную-указатель` нужно `инициализировать`. 
	
	Иначе обращение к ней приведет к непредсказуемым последствиям.
	
	Присвоение переменной адреса существующей функции очень простое.

	Есть функция
*/
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
int (*ptr_to_func)(int, int);			// 	Объявление указателя (собственно указателя) 
									//	на функцию int(int, int)

auto my_function(int x_param, int y_param) -> int {
	cout << "I am int my_function(int x_param, int y_param) и два параметра x == " 
		<< x_param << " и y_param == " << y_param << '\n';
	return x_param + y_param;
}
/*
		int (*ptr_to_func)(int, int);		// Внимание!!! Без `typedef`
		
Её можно присвоить переменной ptr_to_func:

		ptr_to_func = my_function;

С этого момента обращение к `ptr_to_func` будет по факту обращением к `my_function`.

Адрес функции можно передавать как параметр какой то процедуры точно так же, 
как и другие переменные.

Помним, что у нас выше объявлен тип `t_func`.
*/
void	the_first_step()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';

	cout 	<< "Типы указателей на функцию: ptr_to_func и my_function - " 
			<< typeid(ptr_to_func).name() << " и " << typeid(my_function).name() 
			<< '\n';
	// PFiiiE и FiiiE - вроде как разные, но, видимо, допускается неявное преобразование при присваивании		
	// Обе конструкции работают
	ptr_to_func		= &my_function;
	ptr_to_func		= my_function;
	
	cout 	<< "Типы указателей на функцию: ptr_to_func и my_function - " 
			<< typeid(ptr_to_func).name() << " и " << typeid(my_function).name() 
			<< '\n';
	// После присваивания типы не изменились - видимо, есть нюансы в этом деле
	
	auto 		int_var 	= ptr_to_func(3, 12);
	cout << "auto int_var = ptr_to_func(3, 12); == " << int_var << '\n';
}

// 	Пусть есть две функции:

auto my_sum(int x_param, int y_param) -> int {
  return x_param + y_param;
}
auto my_mul(int x_param, int y_param) -> int {
  return x_param * y_param;
}

/* 
	Можно определить тип указателя на функцию при помощи `typedef`, 
	после чего определять указатели на функции станет легче:
	тип указателя на функцию с 2 аргументами int и результатом int
*/	
typedef 	int (*type_ptr_to_func)(int, int);  			// NOLINT(modernize-use-using)
													// переменная - ТИП указателя на функцию
//	Или:
using  		type_ptr_to_func = int (*)(int, int);  	// переменная - ТИП указателя на функцию
//	Эта запись:	int (*ptr_to_func)(int, int); 				- была бы НЕэквивалентная запись - 
//												выше тип указателя, а здесь сам указатель

//	Делаем процедуру-агрегатор:
auto agr_func(int x_param, int y_param, type_ptr_to_func  fun) -> int 
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	cout 	<< "Тип указателя на функцию: type_ptr_to_func  fun - "  << typeid(fun).name()  << '\n';
	cout 	<< "Тип указателя на функцию: type_ptr_to_func - "  << typeid(type_ptr_to_func).name()  << '\n';
	cout 	<< "fun(x_param, y_param) - "  << fun(x_param, y_param) << '\n';
	return fun(x_param, y_param);
}
/*
	Параметр `fun` с типом `t_func` - не что иное, как `обычный` указатель на функцию. 
	
	А вызов fun(x, y) - не что иное, как вызов функции, 
	адрес которой нам передан в параметре `fun`.

	В основной программе делаем так:
*/
void	the_second_step()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';

	cout 	<< "Тип указателя на функцию: type_ptr_to_func - "  
		<< typeid(type_ptr_to_func).name()  << '\n';
	// PFiiiE 

	auto i_var = agr_func(3, 4, my_sum);
	auto j_var = agr_func(3, 4, my_mul);
	
	cout << "auto i_var = agr_func(3, 4, my_sum); == " << i_var << '\n';
	cout << "auto j_var = agr_func(3, 4, my_mul); == " << j_var << '\n';
}
/*
	Итак, разница:
	
	int 	my_function(int x_param, int y_param)
	{	
		// ...
		
		return int{}; 
	}
	
	typedef 			int (*type_ptr_to_func)(int, int);  	// переменная - ТИП указателя на функцию
	//	Или:
	using 				type_ptr_to_func = int (*)(int, int);  // переменная - ТИП указателя на функцию
	
	type_ptr_to_func		function_ptr;						// Объявление указателя на функцию
	function_ptr			= my_function;
	auto 				= function_ptr(3, 12);
	
	и 
	
	int 	(*ptr_to_func)(int, int);	// Объявление указателя (собственно указателя) на функцию int(int, int)
	ptr_to_func		= my_function;
	auto	ptr_to_func(42, 84);
	
*/
void	the_third_step()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	auto my_function_sum	=	[](int x_param, int y_param) -> int {	
		return x_param + y_param; 
	};
	// 1.
	typedef 			int (*type_ptr_to_func)(int, int);  	// NOLINT(modernize-use-using)
														// 	Объявляется переменная - 
														//	ТИП указателя на функцию
	//	Или:
	using 			type_ptr_to_func = int (*)(int, int);
	
	// 	Объявление собственно указателя на функцию int(int, int)
	type_ptr_to_func		function_ptr{my_function_sum};	
	
	// 	Определение (инициализация) указателя на функцию
	function_ptr		= my_function_sum;				

	// 	Вызов функции через указатель
	auto result_1 		= function_ptr(33, 12);			
	
	// 	На строчку больше, но такая конструкция используется 
	//	при передаче другой функции такого параметра,
	// 	когда необходимо указывать в объявлении фунции тип аргумента (параметра)
	
	// 2.
	// 	Объявление собственно указателя на функцию int(int, int)
	int 			(*ptr_to_func)(int, int)  = nullptr;	//	Я и не знал про такую инициализацию
	
	// 	Определение (инициализация) указателя на функцию
	ptr_to_func	= my_function_sum;		
	
	// Вызов функции через указатель	
	auto	result_2	= ptr_to_func(33, 12);				
	
	cout << "result_1 == " <<  result_1 << ",  result_2 == " << result_2 << '\n';
}	
/*
	https://radioprog.ru/post/1196
	
	Оператор косвенного обращения: *

	Indirection Operator: *
*/
void	the_fourth_step()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	auto my_function_sum	=	[](int x_param, int y_param) -> int {	
		return x_param + y_param; 
	};
	// 	1.
	// 	Объявляется переменная - ТИП указателя на функцию 
	typedef 			int (*type_ptr_to_func)(int, int);  // NOLINT(modernize-use-using)
	//	Или:
	using 			type_ptr_to_func = int (*)(int, int);  
	
	// 	Объявление с инициализацией собственно указателя на функцию int(int, int) - 
	//	еще на строчку короче
	type_ptr_to_func		function_ptr{ my_function_sum };						
	
	// 	Вызов функции через указатель
	auto result_1 		= function_ptr(33, 12);			
	
	// 	На строчку больше, но такая конструкция используется 
	//	при передаче другой функции такого параметра,
	// 	когда необходимо указывать в объявлении фунции тип аргумента (параметра)
	
	// 	2.
	// 	Объявление с инициализацией собственно указателя на функцию int(int, int) - 
	//	еще на строчку короче
	int 		(*ptr_to_func)(int, int) { my_function_sum };

	// 	Вызов функции через указатель	
	auto		result_2	= ptr_to_func(33, 12);				
	
	cout << "result_1 == " <<  result_1 << ",  result_2 == " << result_2 << '\n';
}
/*

*/
void	the_fifth_step()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	auto my_function_sum	=	[](int x_param, int y_param) -> int {	
		return x_param + y_param; 
	};
	{
		std::function<int(int, int)> 		function_ptr{ my_function_sum };
	
		auto result_1 		= function_ptr(33, 12);			// Вызов функции через указатель
		cout << "result_1 == " <<  result_1 << '\n';
	}
	
	{
		std::function<int(int, int)> 	function_ptr	{ 
													[](int x_param, int y_param) -> int 
													{ 
														return x_param + y_param; 
													} 
												};
	
		auto result_1 		= function_ptr(33, 12);			// Вызов функции через указатель
		cout << "result_1 == " <<  result_1 << '\n';
		cout << "function_ptr(42, 12) == " <<  function_ptr(42, 12) << '\n';
		
	}
	
	{
		// С лямбда можно вообще напрямую
		auto result_1{ my_function_sum };						
		cout << "result_1(33, 12) == " <<  result_1(33, 12) << '\n';
	}
}
class SumWithCallingFunction
{
public:
	auto operator()(int op_1, int op_2) ->  int 
	{ 
		return op_1 + op_2; 
	}
};

void	the_sixth_step()
{
	cout << "I am void	the_sixth_step()" << '\n';
	cout << "SumWithCallingFunction()(84, 42); == " <<  SumWithCallingFunction()(84, 42) << '\n';
}	
void	the_seventh_step()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	auto f_bind = std::bind(my_function, _1, _2);	// NOLINT(modernize-avoid-bind)
	
	cout << "f_bind(84, 42); == " <<  f_bind(84, 42) << '\n';
}	

/*
	Итак, следующие варианты использования косвенного обращения к функциями:
	
	1. 
*/
#if 0	
	int (*ptr_to_func)(int, int) { my_function_sum } ;
	auto result = ptr_to_func(11, 21);
#endif	
/*	
	2.
*/
#if 0	
	typedef 			int (*type_ptr_to_func)(int, int);  	
	//	Или:
	using 				type_ptr_to_func = int (*)(int, int);  	
	type_ptr_to_func		function_ptr { my_function_sum };						
	auto result = function_ptr(33, 12);			
#endif	
/*	
	3.
*/
#if 0	
	auto my_function_sum	=	[](int x, int y) -> int { return x + y; };
	auto function_ptr { my_function_sum };
	auto result = function_ptr(33, 12);			
#endif	
/*	
	4.
*/
#if 0	
	std::function<int(int, int)> 	function_ptr{ [](int x, int y) -> int { return x + y; } };
	auto result = function_ptr(33, 12);			
#endif	
/*	
	5.
*/
#if 0	
	std::function<int(int, int)> 	function_ptr{ my_function };
	auto result = function_ptr(33, 12);			
#endif	
/*	
	6.
*/
#if 0	
	class SumWithCallingFunction { public: int operator()(int op_1, int op_2) { return op_1 + op_2; } };
	auto result = SumWithCallingFunction()(84, 42); 
#endif	
/*
	7. 
*/
#if 0	
	auto f_bind = std::bind(my_function, _1, _2);
	auto result = f_bind(84, 42);
#endif	
//	*********************************************************************************************************
//	Косвенный вызов функции с параметром как параметр другой функции
typedef 		void (*voif_f_par_int)(int);	// NOLINT(modernize-use-using)
using 		voif_f_par_int	=	void (*)(int);	

// 	Это  фрагмент из книги Липпмана и главы об исключениях
/*
	В более общем виде выражение noexcept(е_func) возвращает значение `true`, 
	если у всех вызванных е_func() функций нет спецификаций передачи,
	и сама е_func() не содержит операторов `throw`. 
	
	В противном случае выражение noexcept(е_func) возвращает значение `false`.
	
	Оператор `noexcept` можно использовать 
	для формирования `спецификатора исключения` следующим образом:
*/	
#if 0	
	void func() noexcept( noexcept( g() ) ); 	// 	func() имеет тот же спецификатор исключения, 
											//	что и g()
#endif		
/*	
	Если функция g() обещает не передавать исключений, то func() также не будет.
	
	Если g() не имеет спецификатора исключения или имеет спецификатор, 
	позволяющий передачу исключений,
	то функция func() также может передавать их.
*/	
// не будет передавать исключений
void 	g_1814(int par) 	noexcept(true)	
{ 
	cout << "void g_1814(int) noexcept(true); par == " << par <<'\n'; 
}; 		
// может передавать исключения
void 	h_1814(int par) 	noexcept(false)	{ 
	cout << "void h_1814(int) noexcept(true); par == " << par <<'\n'; 
}; 		

//	`Указатель на функцию`, возвращающую `void` и принимающую параметр `int`
typedef 	void (*void_f_par_int)(int);			// NOLINT(modernize-use-using)
using 	void_f_par_int	=	void (*)(int);	

//	void 	func_1814(void_f_par_int 	function_ptr,  int par)	
// 	Так тоже катит, только, возможно, менее наглядно, но... на строчку меньше конструкция
void 	func_1814(void(*function_ptr)(int), int par)	
{
	function_ptr(par);
};

void 	test_noexcept_operator_2()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	// noexcept() == true, если вызов функции не может передать исключение, и false в противном случае
	cout << "func_1814(g_1814, 42); " << std::boolalpha << noexcept(noexcept(g_1814(42)))	<< '\n';
	func_1814(g_1814, 42); 
	
	cout << "func_1814(h_1814, 42); " << std::boolalpha << noexcept(noexcept(h_1814(42)))	<< '\n';
	func_1814(h_1814, 42); 
}

/*
	*****************************************************************************************************
	*	Универсальный косвенный вызов
	*
	*****************************************************************************************************
	std::invoke, std::invoke_r 	- 	это шаблонная функция, 
								появившаяся в C++17 (и расширенная в C++20/C++23), 
								которая позволяет единообразно вызывать 
								любые «вызываемые» (Callable) объекты 
								с переданными аргументами.
	Основные возможности

	• 	Обычные функции и лямбды: 
	
		вызываются напрямую, как если бы вы написали f(args...).
	
	• 	Функциональные объекты (функторы): 
		
		экземпляры классов с перегруженным оператором operator().
		
	• 	Указатели на методы классов: 
		
		автоматически разыменовывает указатель на объект или ссылку на объект и вызывает метод.
		
	• 	Указатели на члены-данные: 
	
		позволяет получить доступ к переменной члена класса по указателю на неё.
*/
#if 0	
template<typename FuncType, typename... Args>
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
auto universal_indirect_call	(	FuncType&& 	functor
							, 	Args&&... 	args
							) 	-> std::invoke_result_t	<	FuncType
														, 	Args...
														>
{
	using result_type = std::invoke_result_t<FuncType, Args...>;
	if (std::is_same_v<std::invoke_result_t<FuncType, Args...>, void>) {
		cout << "  Тип result_type == void" << '\n';
	} else // NOLINT(readability-else-after-return)
	{
		cout << "  Тип result_type == void ????" << '\n';
	}
	cout << "  Тип result_type == " << type_names_tester<result_type>() << '\n';
	
	return 	std::invoke	(	std::forward<FuncType>(functor)
						, 	std::forward<Args>(args)...
						);
	#if 0	
	// 	Лямбда захватывает `функцию` и `аргументы` по `универсальной ссылке` (perfect forwarding)
	auto task = 
		[	func 			= std::forward<FuncType>(functor)
		, 	...captured_args 	= std::forward<Args>(args)
		]() mutable -> auto 			// NOLINT(readability-redundant-lambda-parameter-list)
		{
			if (std::is_same_v<std::invoke_result_t<FuncType, Args...>, void>) {
				std::invoke(func, std::forward<decltype(captured_args)>(captured_args)...);
			} 
			else // NOLINT(readability-else-after-return)
			{	
				return std::invoke(func, std::forward<decltype(captured_args)>(captured_args)...);
			}	
		};
	if (std::is_same_v<std::invoke_result_t<FuncType, Args...>, void>) {
		return;
	} 
	else // NOLINT(readability-else-after-return)
	{	
		return task;
	}	
	#endif	
}
#endif	
// 	Функция-обертка, которая вызывает callable-объект и возвращает его результат
template <	typename 	FuncType
		, 	typename... 	Args
		>
auto universal_indirect_call	(	FuncType&& 	functor
							, 	Args&&... 	args
							)	-> std::invoke_result_t	<	FuncType
														, 	Args...
														>  {
    cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
    // 	Рекомендуется использовать std::invoke для универсального вызова любых типов
    return 	std::invoke	(	std::forward<FuncType>(functor)
						, 	std::forward<Args>(args)...
						);
}

auto sum(int a_param, int b_param) -> int  
{ 
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	return a_param + b_param; 
}

struct Multiplier {
    auto operator()(double a_param, double b_param) const  -> double 
	{ 
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
		
		return a_param * b_param; 
	}
};
void universal_indirect_call_test() 
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	// 	1. 	Обычная функция (тип возвращаемого значения: int)
    [[maybe_unused]] auto res1 = universal_indirect_call(sum, 10, 20);
    
    // 	2.	 Лямбда-выражение (тип возвращаемого значения: void)
    universal_indirect_call([] -> auto  { cout << "Hello from lambda!\n"; });

    // 	3. 	Функтор / Структура (тип возвращаемого значения: double)
    Multiplier multiplier;
    [[maybe_unused]] auto res2 = universal_indirect_call(multiplier, 2.5, 4.0);
}
