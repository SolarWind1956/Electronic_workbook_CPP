/*
	about_std_partial_sum.cpp
*/
#include <iostream>
using std::cout; using std::endl; using std::cerr;  using std::boolalpha;

#include <thread>
#include <list>
#include <chrono>
using namespace std::chrono_literals;
#include <iterator> 		// 	для std::next
#include <vector>
#include <numeric>

#include "../include/type_names_tester.hpp"
using namespace chat_gpt_seh;
/*
	===================================================================
	ИИ	-	Google Chrome Chat AI
	===================================================================	
*/
/*
	*****************************************************************************************************
	*
	*	std::partial_sum
	*
	*****************************************************************************************************
	
	Функция std::partial_sum (определенная в заголовке <numeric>) 
	вычисляет `частичные суммы` (`префиксные суммы`) элементов 
	в заданном диапазоне 
	и записывает результаты в другой диапазон.
	
	Проще говоря, она считает «сумму с накоплением»: 
	первый элемент результата равен первому элементу исходного массива, 
	второй - сумме первого и второго, 
	третий - сумме первых трех 
	и так далее. 
	
	Также `вместо сложения` 
	можно передать любую другую `бинарную операцию` 
	(например, умножение для получения факториалов).

	
	📋 Синтаксис

	У функции есть две основные перегрузки (доступные на cppreference.com):	
*/
#if 0	
// 	1. 	С использованием дефолтного сложения (operator+)
template	<class InputIt, class OutputIt>
auto partial_sum	( 	InputIt 		first
				, 	InputIt 		last
				, 	OutputIt 	d_first 
				) 	-> OutputIt
				;

// 	2. 	С кастомной бинарной операцией
template	<class InputIt, class OutputIt, class BinaryOperation>
auto partial_sum	( 	InputIt 			first
				, 	InputIt 			last
				, 	OutputIt 		d_first
				, 	BinaryOperation 	op 
				) 	-> OutputIt 
				;
#endif	
/*
	• 	first, last 	- 	итераторы, определяющие исходный диапазон элементов.
	
	• 	d_first 		- 	итератор начала целевого диапазона, 
						куда будут записываться результаты. 
						
						Важно: 	целевой контейнер должен иметь достаточный размер, 
						либо `итератор` должен быть `вставляющим` 
						(например, std::back_inserter). 
						
						Допускается запись «на месте» в тот же контейнер.
						
	• 	op 			- 	бинарная функция (`функтор` или `лямбда`), 
						принимающая `текущую накопленную сумму` 
						и `следующий элемент`.
	--------------------------------------------------------------------------------------------------------------------------------------------------------
	
	Примеры кода

*/
void first_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	
	std::vector<int> src = {1, 2, 3, 4, 5};

	for (int num : src) {
		cout << num << " "; 
	}
	cout << '\n';
	
    std::vector<int> dst(src.size()); 		// 	Создаем вектор нужного размера

    // 	Вычисляем префиксные суммы
    std::partial_sum(src.begin(), src.end(), dst.begin());

    // 	Вывод: 1 3 6 10 15
    for (int num : dst) {
		cout << num << " "; 
	}
	cout << '\n';
}
/*
	----------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void second_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
}		
/*
*/
void third_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
}
/*
	===================================================================
*/
void fourth_example() 
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
	//fourth_example();
	
	//system("pause");
	
	return 0;
}	

