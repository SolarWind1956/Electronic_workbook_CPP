//	std_invoke_result.cpp

#include	<iostream>
using std::cout; using std::endl; using std::boolalpha; using std::cerr;

#include <type_traits>
#include <utility> 

#include "../include/type_names_tester.hpp"
using namespace chat_gpt_seh;

/*
	ИИ	================================================================
	Шаблон класса `std::result_of` (определенный в заголовочном файле <type_traits>) 
	использовался в C++ для определения 
	`типа "возвращаемого значения" вызываемого объекта` 
	(функции, лямбды, функтора) 
	с заданными `на этапе компиляции` 
	`типами аргументов`.
	
	Однако в современном C++ его статус изменился: 
	он был объявлен `устаревшим` (deprecated) в C++17 и полностью удален в C++20. 
	
	Вместо устаревшего и удаленного из C++20 `std::result_of` 
	теперь используется `std::invoke_result`.
	
	Главная проблема `std::result_of` заключалась в его синтаксисе, 
	который «маскировался» под `тип функции` Функция(Аргументы...), 
	что приводило к потере `const` квалификаторов, 
	некорректной работе с массивами 
	и абстрактными классами. 

	`std::invoke_result` принимает `вызываемый объект` и `типы аргументов` 
	как `раздельные параметры шаблона`, 
	что устраняет эти баги.	
	
	Сравнение: std::result_of vs std::invoke_result
-----------------------------------------------------------------------------------------------------------------------------------------------------------------
Характеристика		std::result_of (C++11)					std::invoke_result (C++17)
-----------------------------------------------------------------------------------------------------------------------------------------------------------------
Статус в C++20		Удален ❌							Рекомендуется к использованию *️⃣

Синтаксис 
получения типа		typename std::result_of<F(Args...)>::type	typename std::invoke_result<F, Args...>::type
					аргументы в круглых скобках			круглых скобок нет сосвсем

Удобный alias 		
(с суффиксом _t)		std::result_of_t<F(Args...)>				std::invoke_result_t<F, Args...>
-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	Примеры использования
*/
/*
	
	1. Старый подход с std::result_of (C++11 / C++14)

	Этот код будет работать только если вы компилируете проект со стандартом ниже C++20:
*/
#if 0	
int sum(int a, double b) {
    return a + static_cast<int>(b);
}
void test_1()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	// 	Передаем сигнатуру как тип функции: Callable(Arg1, Arg2)
    using MethodResult = typename std::result_of<decltype(&sum)(int, double)>::type;
    
    // 	В C++14 появился более короткий вариант:
    // 	using MethodResult = std::result_of_t<decltype(&sum)(int, double)>;

    cout << std::is_same_v<MethodResult, int> << endl; 		// 	Выведет 1 (true)
}
#endif	
/*
	2. Современный подход с std::invoke_result (C++17 / C++20 / C++23)

	Для актуальных стандартов 
	разделяйте тип функции и типы аргументов через запятую:
*/
struct Functor {
    auto operator()(int x_par) const -> double { return x_par * 2.5; }
};

void test_2()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	    // Сила синтаксиса: типы передаются раздельно через запятую
    using 	ReturnType = std::invoke_result_t<Functor, int>;

    cout << std::is_same_v<ReturnType, double> << '\n'; 	// 	Выведет 1 (true)	
	
	cout << "using ReturnType = std::invoke_result_t<Functor, int>;  Тип ReturnType == " 
		<< type_names_tester<ReturnType>() << '\n';
}
/*
	3. 	Альтернатива через decltype

	Если вам нужно узнать тип возвращаемого значения 
	в контексте конкретного выражения (например, внутри шаблона), 
	зачастую проще и надежнее использовать оператор 
	`decltype` совместно с `std::declval`:
*/
// 	Функция-обёртка (автовызов)
// 	Она принимает любой вызываемый объект (функцию, лямбду) и любые аргументы.
// 	Благодаря -> decltype(...) компилятор сам смотрит, какой тип получится при вызове function(args...).
template <typename F, typename... Args>
auto auto_call(F some_callable, Args&&... args) -> decltype(some_callable(std::forward<Args>(args)...)) 
{
    cout << "[Лог] Вызываем функцию через обёртку..." << '\n';
    
    // 	std::forward нужен для идеальной передачи аргументов (perfect forwarding)
    return some_callable(std::forward<Args>(args)...);
}
// 	1. Обычная функция, возвращающая int
auto add(int a_par, int b_par) -> int {
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
    return a_par + b_par;
}

// 	2. Функция, возвращающая std::string
auto greet(const std::string& name) -> std::string {
    return "Привет, " + name + "!";
}

void test_3()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	// 	Вызываем add(5, 10). 
	//	auto_call автоматически поймет, что вернется int
    int 			sum 			= auto_call(add, 5, 10);
    cout << "Результат add: " << sum << " (тип: int)\n\n";

    // 	Вызываем greet("Алексей"). 
	//	auto_call автоматически поймет, что вернется std::string
    std::string 	message 		= auto_call(greet, static_cast<const char*>("Алексей"));
    cout << "Результат greet: " << message << " (тип: std::string)\n\n";

    // 	3. Можно передать даже лямбду "на лету"
    auto			lambda_res 	= auto_call([](double x_par) -> double  { return x_par * 2.5; }, 4.0);
    std::cout << "Результат лямбды: " << lambda_res << " (тип: double)\n";
}
/*
	4.	Цель - написать этот же автовызов (функцию-обёртку), 
		но принципиально использовать именно std::invoke_result_t 
		(как это часто делают в современном промышленном коде для строгости), 
		то синтаксис будет выглядеть следующим образом.
		
		Вот тот самый пример, переписанный строго через std::invoke_result_t:	
*/
// 	Начиная с C++17 мы можем явно указать тип возвращаемого значения через invoke_result_t.
// 	Мы передаем туда тип самой функции (F) и типы всех её аргументов (Args...)
template <typename F, typename... Args>
auto auto_call_with_invoke(F some_callable, Args&&... args) -> std::invoke_result_t<F, Args...> 
{
    cout << "[Лог] Вызов через std::invoke_result_t..." << std::endl;
    
    // 	std::invoke автоматически корректно вызовет и обычную функцию, и лямбду, и метод класса
    return 	std::invoke(some_callable, std::forward<Args>(args)...);
}
/*
	Синтаксис std::invoke_result_t<F, Args...> 
	работает в связке со стандартной функцией std::invoke(). 
	
	Это самый мощный и универсальный инструмент в современном C++. 
	
	Ему всё равно, что вы передали: 
	-	обычную функцию, 
	-	лямбду, 
	-	функтор 
	или 
	-	указатель на метод структуры - 
	он определит тип возвращаемого значения и выполнит вызов абсолютно для всего.
*/
int multiply(int a, int b) {
    return a * b;
}
void test_4()
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	// 	Компилятор выведет тип std::invoke_result_t<decltype(multiply), int, int>, то есть int
    int result = auto_call_with_invoke(multiply, 6, 7);
    
    cout << "Результат: " << result << '\n';
}
auto main() -> int  
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	//test_1();
	//test_2();
	//test_3();
	test_4();
}	