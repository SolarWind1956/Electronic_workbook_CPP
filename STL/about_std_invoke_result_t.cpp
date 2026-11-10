/*
	about_std_invoke_result_t.cpp
*/
#include <iostream>
using std::cout; using std::endl; using std::cerr;  using std::boolalpha;

#include <thread>
#include <list>
#include <chrono>
using namespace std::chrono_literals;
#include <iterator> 		// 	для std::next

#include "../include/type_names_tester.hpp"
using namespace chat_gpt_seh;
/*
	===================================================================
	ИИ	-	Google Chrome Chat AI
	===================================================================	
*/
/*
	
*/
/*
	*****************************************************************************************************
	*
	*	std::invoke_result_t
	*
	*****************************************************************************************************
	std::invoke_result_t - 	это вспомогательный шаблон-псевдоним 
						(alias template), представленный в C++17, 
						который позволяет получить `тип возвращаемого значения` 
						`вызываемого объекта` 
						(функции, лямбды, функтора, указателя на метод или члена класса) 
						на этапе компиляции с `заданными `типами `аргументов`.
						
	Он определен в заголовочном файле <type_traits> 
	и является эквивалентом записи 
*/
#if 0	
typename std::invoke_result<Callable, Args...>::type

//	📋 Синтаксис

template <class Callable, class... Args>
using invoke_result_t = typename std::invoke_result<Callable, Args...>::type;

#endif	
/*
	• 	Callable: 		тип объекта, который вы хотите вызвать 
					(например, указатель на функцию, класс с operator(), указатель на метод).
					
	• 	Args...: 		список типов аргументов, с которыми будет производиться гипотетический вызов.

	
	💻 Пример использования

	std::invoke_result_t идеально подходит 
	для создания `универсальных функций-оберток` 
	(например, `логирования` или `асинхронного выполнения`), 
	где `возвращаемый тип` 
	зависит от `переданного колбэка`.
*/
// 	Функция-обертка, которая вызывает callable-объект и возвращает его результат
template <	typename 	FuncType
		, 	typename... 	Args
		>
auto run_and_log(FuncType&& functor, Args&&... args)-> std::invoke_result_t<FuncType, Args...>  {
    cout << "[Log] Calling function...\n";
    // 	Рекомендуется использовать std::invoke для универсального вызова любых типов
    return 	std::invoke	(	std::forward<FuncType>(functor)
						, 	std::forward<Args>(args)...
						);
}

auto sum(int a_param, int b_param) -> int  
{ 
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	return a_param + b_param; }

struct Multiplier {
    auto operator()(double a_param, double b_param) const  -> double 
	{ 
		cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
		return a_param * b_param; 
	}
};
void first_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	// 	1. 	Обычная функция (тип возвращаемого значения: int)
    [[maybe_unused]] auto res1 = run_and_log(sum, 10, 20);
    static_assert(std::is_same_v<decltype(res1), int>);

    // 	2.	 Лямбда-выражение (тип возвращаемого значения: void)
    run_and_log([] -> auto  { cout << "Hello from lambda!\n"; });

    // 	3. 	Функтор / Структура (тип возвращаемого значения: double)
    Multiplier multiplier;
    [[maybe_unused]] auto res2 = run_and_log(multiplier, 2.5, 4.0);
    static_assert(std::is_same_v<decltype(res2), double>);
}

void second_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
}
void third_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
}
/*
	****************************************************************************************************************
	int main()
	****************************************************************************************************************
*/
auto main() -> int {
		cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';

	first_example();
	//second_example();
	//third_example();
		
	//system("pause");
	
	return 0;
}	

