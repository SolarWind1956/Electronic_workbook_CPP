/*
	std_shared_ptr_swap.cpp
*/
#include <iostream>
using std::cout; using std::endl; using std::cerr;  using std::boolalpha;

#include <thread>
#include <chrono>
using namespace std::chrono_literals;
#include <vector>
#include <queue>
#include <algorithm>
#include <mutex>
#include <memory>

#include "../include/type_names_tester.hpp"
using namespace chat_gpt_seh;
/*
	*****************************************************************************************************
	*
	*	std::shared_ptr::swap()
	*
	*****************************************************************************************************
	std::shared_ptr<T>::swap - это встроенный метод класса std::shared_ptr, 
	который обменивает содержимое двух умных указателей местами.
	
	Этот метод просто меняет местами 
	`указатели на управляемые объекты` и `счетчики ссылок`, 
	`не изменяя` при этом `сами счетчики`. 
	
	Операция выполняется очень быстро (за константное время O(1)) и 
	гарантированно не генерирует исключений (noexcept).

	🛠 Способы вызова (Синтаксис)

	Существует два равноправных способа обменять значения:
	
	
*/

/*
*/
/*
	**************************************************************************************************************
*/

void first_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	
	std::shared_ptr<int> ptr1 = std::make_shared<int>(10);
	
	std::shared_ptr<int> ptr2 = std::make_shared<int>(20);

	cout << *ptr1 << "   " << *ptr2 << '\n';
	
	// 	Способ 1: Метод класса
	ptr1.swap(ptr2);
	cout << *ptr1 << "   " << *ptr2 << '\n';
	
	// Способ 2: Нечленная функция-шаблон (рекомендуется для универсального кода)
	std::swap(ptr1, ptr2); 
	cout << *ptr1 << "   " << *ptr2 << '\n';
}

/*
	*****************************************************************************************************
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
*/

void fourth_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';

}
/*
*/
void fifth_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';

}
/*
*/
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
	//fifth_example();

	//system("pause");
	
	return 0;
}	

