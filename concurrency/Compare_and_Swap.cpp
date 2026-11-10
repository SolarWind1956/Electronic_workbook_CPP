/*
	Compare_and_Swap.cpp
*/
#include <iostream>
using std::cout; using std::endl; using std::cerr;  using std::boolalpha;

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include <type_traits>
#include <vector>
#include <queue>
#include <ranges>
#include <algorithm>
#include <utility>	//	std::forward
#include <atomic>

#include "../include/type_names_tester.hpp"
using namespace chat_gpt_seh;
/*
	**********************************************************************************************************************
	compare_exchange (или Compare-and-Swap / CAS) - 
	это фундаментальная атомарная операция 
	в многопоточном программировании. 
	
	Она позволяет безопасно обновлять значение переменной из нескольких потоков 
	без использования тяжелых блокировок (мьютексов).

	В современных языках программирования (например, в C++11 и новее) 
	эта операция представлена в двух основных вариантах: 
	
	compare_exchange_strong 
	
	и 
	
	compare_exchange_weak.
		
	Как работает механизм `compare_exchange`

	Операция принимает `три` аргумента:
	
	1. 	`expected` 	- 	значение, которое, как ожидает поток, 
						сейчас находится в переменной.
	
	2. 	`desired` 	- 	новое значение, 
						которое поток хочет записать.
						
	3. 	`Атомарная переменная`, над которой проводится операция.
	
	Алгоритм действий:
	
	• 	Успех: 
		
		Если `текущее значение переменной` "равно" `expected`, 
		оно атомарно `заменяется` на `desired`, 
		а `функция `возвращает `true`.
		
	• 	Неудача: 
	
		Если `значение переменной` "не равно" `expected` 
		(его успел изменить другой поток), 
		перезаписи не происходит. 
		
		Вместо этого в `саму переменную `expected` 
		`записывается` 
		`актуальное текущее значение` из памяти, 
		а `функция `возвращает `false`.
	*************************************************************************************************************
	
	Главные отличия

	--------------------------------------------------------------------------------------------------------------------------------------------------------
	Критерий						compare_exchange_strong		compare_exchange_weak
	--------------------------------------------------------------------------------------------------------------------------------------------------------
	Ложные сбои (Spurious failures)		Невозможны. 				Возможны.
									Возвращает `false` только 		Может вернуть `false`, 
									если значения 				даже если текущее значение
									действительно не равны.	   	равно ожидаемому.
									
	Использование в цикле			Подходит для 				Идеален для циклов
									простых условий, 			while (!atomic.compare_exchange_weak(...))
									но в цикле может быть 
									менее эффективным 
									на некоторых процессорах.	 
									
	Производительность				Чуть медленнее 				Быстрее на ARM / PowerPC, 
									на архитектурах 				так как компилируется 
									ARM / PowerPC 				в простые инструкции LL/SC.
									из-за жестких 
									аппаратных гарантий.	
									
	
	Сложные типы (паддинг)			Может работать  				Сходится быстрее 
									некорректно,					при наличии битов заполнения 
									если в структуре 				или нескольких представлений 
									есть биты заполнения 		(например, NaN).
									(padding).	
									
	--------------------------------------------------------------------------------------------------------------------------------------------------------
	
	Алгоритм работы (Общий для обоих)

	1. 	Функция принимает два основных аргумента: 
		`expected` (что мы ожидаем увидеть) 
		и 
		`desired` (на что хотим заменить).
		
	2. 	Происходит `атомарное сравнение` `значения объекта` с `expected`.

	3. 	Если `значения равны`: 
		`объект` перезаписывается значением `desired`, 
		функция возвращает `true`.

	4. 	Если `значения НЕ равны`: 
		в переменную `expected` записывается 
		`актуальное текущее значение объекта`, 
		а 
		функция возвращает `false`.

	
	Почему существуют две версии?

	Разница обусловлена архитектурой процессоров:
	
	• 	`x86` (Intel/AMD) имеют `встроенную аппаратную` инструкцию `CMPXCHG`, 
		которая гарантирует выполнение операции без `ложных сбоев`. 
		
		На этой архитектуре обе функции работают практически одинаково.
	
	• 	`ARM` и `PowerPC` используют пару инструкций: 
		Load-Link / Store-Conditional (LL/SC). 
		
		`Между `чтением` и `записью` 
		может произойти `переключение контекста` или `прерывание`, 
		из-за чего `процессор `сбросит `статус отслеживания`, 
		и запись Store-Conditional завершится ошибкой, 
		даже если данные в памяти никто не менял.
	
	`compare_exchange_weak` позволяет процессору 
	сообщить об этом сбое программе. 
	
	Если `операция` и так `обернута в цикл while`, 
	`повторная итерация` мгновенно `исправит ситуацию`, 
	избежав при этом дорогостоящей внутренней проверки, 
	которую `compare_exchange_strong` 
	вынужден делать на `ARM`.

	Что выбирать?

	• 	Используйте `strong`, 
		если вам `нужно выполнить `проверку `строго один раз` (без цикла) 
		и `логика программы` зависит 
		от `точного результата сравнения`.
	
	• 	Используйте `weak`, 
		если вы реализуете `бесблокировочную структуру данных` (lock-free алгоритм), 
		где `обновление переменной` 
		в любом случае 
		происходит в цикле do-while.
		
	
	Параметры success и failure

	У функций `compare_exchange` есть `расширенные сигнатуры`, 
	которые позволяют управлять тем, 
	как процессор и компилятор оптимизируют 
	(переставляют местами) 
	инструкции вокруг атомарной операции. 
	
	Вы можете задать два порядка памяти:

	1. 	`success` - модель `консистентности памяти`, 
		если операция сравнения прошла успешно и запись состоялась.
	
	2. 	`failure` - модель памяти, если сравнение провалилось 
		(записи не было, произошло только чтение).
*/
#if 0	
// 	Пример расширенной сигнатуры
atomic_var.compare_exchange_weak	(	expected
								, 	desired
								, 	std::memory_order_release 		// 	если успешно
								,	std::memory_order_relaxed  	// 	если провал
                                );
#endif	
/*
	Зачем это нужно?

	`По умолчанию` используется `самый строгий` и `безопасный` порядок - 
	std::memory_order_seq_cst (последовательная согласованность). 
	
	Он гарантирует, что все потоки видят изменения в строго одинаковом порядке, 
	но это самый медленный режим.
	
	`Тонкая` настройка позволяет `ускорить код`:
	
	• 	Если запись успешна (success), 
		часто используют `memory_order_release`, 
		чтобы "опубликовать" изменения для других потоков.
		
	• 	Если произошел провал (failure), 
		мы просто считываем актуальное значение, 
		поэтому часто достаточно `memory_order_relaxed`
		(самый быстрый режим, без синхронизации окружающего кода) 
		или `memory_order_acquire`.
*/
/*
	*************************************************************************************************************
*/
void first_example() 
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	bool 				expected				=	false;			//	`Ожидаемое` значение 
	bool 				desired				=	true;			//	`Желаемое` значение
	bool 				result				=	true;			//	`Результат` операции
	std::atomic<bool> 	atomic_bool_var		=	false; 			// 	`Атомарная переменная` 
	/*
		if 	(atomic_bool_var == expected) {
			atomic_bool_var 	= 	desired;
			result			=	true;
		}
		else
		{
			expected			=	atomic_bool_var;
			result			=	false;
		}
		
	*/
	cout << std::boolalpha;
	cout << "std::atomic<bool> atomic_bool_var " << atomic_bool_var << '\n';
	cout << "expected " << expected << ", desired " << desired << '\n';
	cout << "result = atomic_bool_var.compare_exchange_strong(expected, desired); " << result << '\n';
	
	cout << '\n';
	
	result	=	atomic_bool_var.compare_exchange_strong(expected, desired);
	
	cout << "std::atomic<bool> atomic_bool_var " << atomic_bool_var << '\n';
	cout << "expected " << expected << ", desired " << desired << '\n';
	cout << "result = atomic_bool_var.compare_exchange_strong(expected, desired); " << result << '\n';
	
}

/*
	*****************************************************************************************************
	
	Реализация Spinlock на `compare_exchange`

	Обычно для такого спинлока используется std::atomic<bool>. 
	
	Значение `false` означает, что `блокировка свободна`, а `true` - что она `захвачена`.
*/
class CASSpinlock_mutex {
  private:
    std::atomic<bool> 	lock_state{false}; 	// 	`false` = свободно, `true` = занято

  public:
    void lock() {
		cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
        
		bool 	expected	=	false;			//	`Ожидаемое` значение 
		bool 	desired	=	true;			//	`Желаемое` значение
		
        // 	Пытаемся заменить `false` на `true`.
        // 	Если `lock_state` == `true`, сравнение провалится, `expected` обновится на `true`,
        // 	поэтому внутри цикла мы сбрасываем `expected` обратно в `false`.
        while 	(!lock_state.compare_exchange_weak	(	expected
												, 	desired
												,	std::memory_order_acquire
												,	std::memory_order_relaxed
												)
				) {
            expected = false; 
            
            // 	Архитектурная оптимизация для процессоров (особенно x86):
            // 	Ожидаем в "микро-паузе", чтобы не перегружать шину памяти процессора
            #if defined(__x86_64__) || defined(_M_X64)
            asm volatile("pause" ::: "memory");
            #endif
        }
    }

    void unlock() {
		cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
        // 	Освобождаем блокировку
        lock_state.store	(	false
						, 	std::memory_order_release
						);
    }
};

struct shared_resource	{
	static CASSpinlock_mutex	spinlock_mtx;
	static std::vector<int> 	int_vector;
	static std::atomic<bool>	stop_flg;
};	
std::vector<int> 		shared_resource::int_vector{};
std::atomic<bool>		shared_resource::stop_flg{false};	//	Атомарная переменная как флаг останова
CASSpinlock_mutex		shared_resource::spinlock_mtx{};

void shared_int_vector_reader(){
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	
	while (!shared_resource::stop_flg.load(std::memory_order_relaxed)){
		
		shared_resource::spinlock_mtx.lock();
		{		
			if (shared_resource::int_vector.empty()) {
			
				shared_resource::spinlock_mtx.unlock(); // НЕ ЗАБЫВАЕМ освободить перед continue!
				
				std::this_thread::sleep_for(200ms);
				
				continue;
			}
			
			auto iter = shared_resource::int_vector.end();
			iter--;
			cout << "Выводим на печать последнее значение и удаляем его из вектора " << *iter << '\n';
			
			shared_resource::int_vector.pop_back();
		}	
		shared_resource::spinlock_mtx.unlock();
		
		std::this_thread::sleep_for(200ms);
	}	
}
void shared_int_vector_writer(){
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	int count = 0;		
	while (!shared_resource::stop_flg.load(std::memory_order_relaxed)){
		
		shared_resource::spinlock_mtx.lock();
		{
			shared_resource::int_vector.push_back(count++);
		}
		shared_resource::spinlock_mtx.unlock();
		
		std::this_thread::sleep_for(200ms);
	}	
	
}
/*
	Для `stop_flg` используется порядок `std::memory_order_relaxed`. 
	
	Поскольку этот флаг служит исключительно `триггером` 
	для остановки и вокруг него нет операций, 
	требующих строгой синхронизации данных, 
	«расслабленный» порядок идеален - 
	он `не создает лишних барьеров памяти` и 
	работает максимально быстро.
*/
void second_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	shared_resource::stop_flg.store(false, std::memory_order_relaxed);
	
	std::thread 	writer_thrd(shared_int_vector_writer);
	std::thread 	reader_thrd(shared_int_vector_reader);
	
	std::this_thread::sleep_for(2000ms);
	
	// 	Атомарно сигнализируем о завершении
    shared_resource::stop_flg.store(true, std::memory_order_relaxed); 
	
	writer_thrd.join();
	reader_thrd.join();
}

/*
*/
void third_example() 
{
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
	
}
/*
*/

void fourth_example() 
{
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
}
/*
*/
void fifth_example() 
{
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
}
/*
*/
/*
	****************************************************************************************************************
	int main()
	****************************************************************************************************************
*/
auto main() -> int {
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
		
	//first_example();
	second_example();
	//third_example();
	//fourth_example();
	//fifth_example();
	//system("pause");
	
	return 0;
}	

