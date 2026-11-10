/*
	std_promise_std_future.cpp
*/
#include <iostream>
using std::cout; using std::endl; using std::cerr;  using std::boolalpha;

#include <future>
#include <thread>
#include <list>
#include <vector>
#include <chrono>
using namespace std::chrono_literals;
#include <iterator> 		// 	для std::next и для std::advance
#include <numbers>
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
	*	std::promise - std::future
	*
	*****************************************************************************************************
	std::promise 	- 	это «производитель» (отправитель) значения, 
	а std::future 	- 	его «потребитель» (получатель).
	
	std::promise передается в фоновый поток 
	(обычно по rvalue-ссылке через std::move или по обычной ссылке std::ref), 
	и этот поток вызывает метод .set_value(), 
	чтобы передать результат обратно в std::future.
	
	Разница между ними очень простая, если разложить её по ролям:
	
	• 	std::packaged_task 	- 	это `автоматизированный инструмент`. 
	
								Он сам оборачивает целую функцию, 
								сам её вызывает, 
								сам ловит её `return` 
								и автоматически отправляет его во `future`.
								
	• 	std::promise 			- 	это ручной инструмент. 
								
								Ему вообще всё равно, 
								какая функция выполняется, 
								и выполняется ли она вообще. 
								
								Он просто дает вам «канал связи». 
								
								Вы можете сделать вычисления, 
								долго что-то обрабатывать, 
								а потом в любой момент вручную написать 
								promise.set_value(xyz).

	💻 	Пример для закрепления памяти

		Вот типичный паттерн использования std::promise, 
		где мы передаем его в поток:
	--------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void fetch_data_from_network(std::promise<std::string> 	res_promise) 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
    // 	Имитируем долгий сетевой запрос
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::string 	data = "Данные успешно загружены!";
    
    // 	Руками отправляем результат во фьючерс
    res_promise.set_value(data); 
}
void first_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	
	// 	1. 	Создаем промис
    std::promise<std::string> 		my_promise;
    
    // 	2. 	Забираем у него фьючерс
    std::future<std::string> 		my_future 	= 	my_promise.get_future();
    
    // 	3. 	Запускаем поток, передавая промис через std::move (он non-copyable)
    std::thread 					worker(fetch_data_from_network, std::move(my_promise));
    
    cout << "[Главный поток] Ждем данные из сети...\n";
    
    // 	4. 	Блокируемся и ждем, пока в другом потоке вызовется .set_value()
    std::string 					result 		= 	my_future.get();
    
    cout << "[Главный поток] Результат: " << result << "\n";
    
    if (worker.joinable()) {
        worker.join();
    }
}
void second_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	
	// 	1. 	Создаем промис
    std::promise<std::string> 		my_promise;
    
    // 	2. 	Забираем у него фьючерс
    std::future<std::string> 		my_future 	= 	my_promise.get_future();
    
    // 	3. 	Запускаем в главном потоке, передавая промис через std::move (он non-copyable)
    fetch_data_from_network(std::move(my_promise));
    
    cout << "[Главный поток] Ждем данные из сети...\n";
    
    // 	4. 	Блокируемся и ждем, пока в другом потоке вызовется .set_value()
    std::string 					result 		= 	my_future.get();
    
    cout << "[Главный поток] Результат: " << result << "\n";
}
/*
	🧠 	Главная фишка std::promise: 	Отправка исключений вручную

		В случае с std::packaged_task 
		исключения перехватываются автоматически. 
		
		С std::promise вы должны управлять этим сами с помощью 
		
		.set_exception(). 
		
		Это невероятно полезно, если в потоке что-то пошло не так:
*/
void fetch_data_with_error(std::promise<double> res_promise) 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	double	param{std::numbers::pi};
	
    try {
		if (param < std::numeric_limits<double>::max())	{
			// 	... какая-то логика, которая падает ...
			throw std::runtime_error("Сеть недоступна!");
		} else // NOLINT(readability-else-after-return)
		{
			// 	Руками отправляем результат во фьючерс
			res_promise.set_value(param * 2.0); 
		}		
    } catch (...) {
        // 	Передаем текущее исключение во фьючерс
        res_promise.set_exception(std::current_exception());
    }
}
/*
	Если функция `вспомогательного потока` вызовет set_exception(), 
	то `главный поток` на строчке my_future.get() 
	получит это исключение в свой блок try-catch, 
	как будто оно произошло прямо там.
*/
void third_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
		// 	1. 	Создаем промис
    std::promise<double> 		my_promise;
    
    // 	2. 	Забираем у него фьючерс
    std::future<double> 		my_future 	= 	my_promise.get_future();
    
    // 	3. 	Запускаем поток, передавая промис через std::move (он non-copyable)
    std::thread 					worker(fetch_data_with_error, std::move(my_promise));
    
    cout << "[Главный поток] Ждем данные из сети...\n";
    
    // 	4. 	Блокируемся и ждем, пока в другом потоке вызовется .set_value()
	double 					result{};
	try {
        result 		= 	my_future.get();
		/*
			Если функция `вспомогательного потока` вызовет set_exception(), 
			то `главный поток` на строчке my_future.get() 
			получит это исключение в свой блок try-catch, 
			как будто оно произошло прямо там.
		*/
		
    } catch (const std::exception& excptn) {
        cout << "[Главный поток] Перехвачено исключение из потока: " << excptn.what() << "\n";
    }
    
    cout << "[Главный поток] Результат: " << result << "\n";
    
    if (worker.joinable()) {
        worker.join();
    }	
}
/*
	===================================================================
*/	
/*
	****************************************************************************************************************
	int main()
	****************************************************************************************************************
*/
auto main() -> int {
		cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';

	//first_example();
	//second_example();
	third_example();
	//fourth_example();
	
	//system("pause");
	
	return 0;
}	

