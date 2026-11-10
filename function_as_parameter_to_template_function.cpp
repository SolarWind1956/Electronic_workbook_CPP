//	function_as_parameter_to_template_function.cpp
/*
	**********************************************************************************************************
	Передача функции как параметр в шаблонную функцию
	
	ИИ Gemini	========================================================
	
	В C++ передать функцию как параметр в шаблонную функцию 
	можно четырьмя основными способами. 
	
	Выбор зависит от того, нужна ли максимальная производительность, 
	поддержка лямбда-выражений или универсальность.
	
	Ниже приведены все актуальные способы с примерами.	
*/
#include	<iostream>
using std::cout; using std::endl; using std::boolalpha; using std::cerr;

/*
	ИИ Gemini	========================================================
	
	1. 	Передача через обычный параметр шаблона (Рекомендуемый способ)
	
		Этот способ самый гибкий и эффективный. 
		
		Компилятор видит вызываемую функцию «напрямую» 
		и часто делает инлайнинг (встраивание кода), 
		что убирает накладные расходы на вызов.
*/
// 	`Шаблонная функция`, принимающая любой `вызываемый объект` (Callable)
template <typename Func>
void execute(Func func, int value) {
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
    func(value); 	// 	Вызов переданной функции
}

// 	Обычная функция для теста
void printSquare(int x_param) {
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
    cout << "Квадрат: " << x_param * x_param << "\n";
}

void the_first_way(){
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	// 	1. 	Передаем обычную функцию
    execute	(	printSquare
			, 	5
			);

    // 	2. 	Передаем лямбда-выражение
    execute	(	[](int x_param) -> void { cout << "Куб: " << x_param * x_param * x_param << "\n"; }
			, 	3
			);
	
}

/*
	2. 	Использование классических указателей на функцию

		Этот способ жестко ограничивает тип передаваемой функции. 
		
		Он работает быстрее всего при компиляции, 
		но не поддерживает лямбда-выражения, 
		которые захватывают переменные (замыкания).
*/
// 	Шаблон принимает типы, но параметр строго ограничен указателем на функцию: void(*)(int)
template <typename T>
void runWithNumber(void (*funcPtr)(T), T value) {
    funcPtr(value);
}

void showNumber(int x_param) {
    cout << "Число: " << x_param << "\n";
}

void the_second_way(){
    runWithNumber(showNumber, 42);
}
/*
	3. 	Использование универсального std::function (C++11)
	
	std::function - это контейнер для любого 	`вызываемого объекта`. 
	
	Способ удобен, если вам нужно явно ограничить сигнатуру функции, 
	но он имеет накладные расходы (динамическая память и виртуальные вызовы), 
	поэтому работает медленнее, чем первый вариант.
*/
#include <functional> 	// 	Обязательно для std::function

// 	Шаблон принимает тип, но функция строго определена через std::function
template <typename T>
void process(const std::function<void(T)>& func, T value) {
    func(value);
}

void the_third_way(){
    auto lambda = [](int x_param) -> void { cout << "Результат: " << x_param + 10 << "\n"; };
    
    process<int>(lambda, 5);
}	
/*
	4. 	Использование Концептов (C++20)
	
		Если вы используете современный C++20, 
		лучше всего явно ограничить шаблон с помощью концепта std::invocable. 
		
		Это защитит от непонятных ошибок компиляции, 
		если в функцию передадут что-то не то.
*/	
#include <concepts>

// 	Шаблон примет только то, что можно вызвать с параметром `int`
template <typename Func>
	requires std::invocable<Func, int>
void modernExecute(Func func, int value) {
    func(value);
}

void the_forth_way(){
    modernExecute([](int x_param) -> void { cout << x_param * 2 << "\n"; }, 10);
    
    // modernExecute(42, 10); // Ошибка компиляции (42 нельзя вызвать как функцию)
}

auto main() -> int  
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	the_first_way();
	
	the_third_way();
	
	return	0;
}	