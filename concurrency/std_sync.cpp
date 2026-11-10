/*
	std_sync.cpp
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
#include <type_traits>
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
	*	std::sync
	*
	*****************************************************************************************************
	
	🚀 Что такое std::async?

	std::async 	- 	это `функция-фабрика`, которая объединяет:
					-	создание задачи, 
					-	запуск потока (если нужно) 
					и 
					-	получение std::future в одну-единственную строчку.
					
	Больше не нужно создавать std::thread, 
	не нужно вручную делать std::move(task) 
	и, что самое приятное, 
	не нужно вызывать .join(). 
	
	`Деструктор фьючерса`, возвращенного из std::async, 
	заблокирует поток 
	и 
	дождется завершения задачи автоматически 
	(если фьючерс не будет перемещен).
	
	Функция принимает так называемую `политику запуска` (launch policy):
	
		• 	std::launch::async 		- 	задача `гарантированно` запускается в `новом`, 
									`асинхронном` потоке.
		
		• 	std::launch::deferred 	- 	ленивое выполнение. 
									
									Функция запустится `синхронно` в `текущем` потоке, 
									но `только тогда`❗❗❗💥💥💥, 
									когда вы вызовете .get() или .wait().
									
		• 	По умолчанию (std::launch::async | std::launch::deferred) 	- 
									среда выполнения (рантайм) `сама решает`, 
									`создавать поток` 
									или 
									`выполнить на месте`, 
									исходя из загруженности системы.

	Пример использования std::async:	
	--------------------------------------------------------------------------------------------------------------------------------------------------------
*/
auto super_heavy_work(double param) -> double {
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
	double	result 	= 	std::numbers::pi * param;
	
	cout << "[Фоновый поток] Работа завершена!\n";
    // 	Исключение для проверки (раскомментируйте, чтобы проверить проброс)
	if (param > std::numeric_limits<double>::max())	{
		throw std::runtime_error("Что-то пошло не так!");
	} else	// NOLINT(readability-else-after-return)
	{
		return	result;
	}	
}

void first_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	
	cout << "[Главный поток] Запускаем задачу через std::async...\n";

    // 	1. 	Запуск в ОДНУ строчку. 
	//		Поток создается сам, аргументы передаются прямо туда.
    // 		Используем явную политику std::launch::async для гарантированного фонового потока.
    //std::future<double> 	future_result = std::async(std::launch::async, super_heavy_work, 2.0);
	//std::future<double> 	future_result = std::async(std::launch::deferred, super_heavy_work, 2.0);
	std::future<double> 	future_result = std::async	(	std::launch::async | std::launch::deferred
												, 	super_heavy_work
												, 	2.0
												);
	
	
    cout << "[Главный поток] Задача крутится в фоне, а мы можем делать что-то еще.\n";
    std::this_thread::sleep_for(500ms);

    try {
        // 	2. 	Точно так же забираем результат или ловим исключение
        double 	result 	= 	future_result.get();
        cout << "[Главный поток] Результат из async: " << result << '\n';
    } catch (const std::exception& excptn) {
        cout << "[Главный поток] Перехвачено исключение: " << excptn.what() << "\n";
    }
}    
/*
	===================================================================
	ИИ
	
	Метод 
	std::future::wait_for() 	- 	это инструмент, который позволяет 
							главному потоку не засыпать наглухо в ожидании результата, 
							а проверить готовность задачи в течение определенного времени.
							
	Вместо того чтобы блокировать программу, как это делает .get(), 
	метод wait_for() возвращает управление через заданный таймаут 
	и сообщает `статус задачи` с помощью перечисления std::future_status.

	🚦 	Три статуса std::future_status

	Метод возвращает одно из трех значений:
	
	1. 	std::future_status::ready 	- 	задача успешно завершена. 
		
									Можно мгновенно и без блокировки вызвать .get(), 
									чтобы забрать результат.
	
	2. 	std::future_status::timeout 	- 	заданное время вышло, 
									но задача всё еще выполняется в фоне.
									
	3. 	std::future_status::deferred 	- 	задача была запущена с политикой 
									std::launch::deferred 
									и вообще не начнет выполняться, 
									пока вы `принудительно` 
									не вызовете .wait() или .get().

	💻 	Дополнение к конспекту: 	fifth_example()

		Вот классический паттерн «анимации загрузки» 
		(индикатора прогресса), 
		пока фоновый поток выполняет тяжелую работу. 
		
	
	⚠️ 	Важные нюансы при работе с wait_for()

	1. 	Многократность использования: 	В отличие от .get(), 
										который разрушает общее состояние 
										и может быть вызван только один раз, 
										метод wait_for() 
										можно вызывать сколько угодно раз в цикле, 
										пока он не вернет std::future_status::ready.
										
	2. 	Связка с std::promise: 				Этот механизм точно так же работает и для фьючерсов, 
										полученных из std::promise или std::packaged_task. 
										
										wait_for() ждет именно момента, 
										когда в разделяемом состоянии появится отметка «готово» 
										(будь то `return` функции или вызов .set_value()).
										
	3. 	Проблема ложных пробуждений: 	Системные таймеры операционных систем 
										имеют погрешность, 
										поэтому wait_for(400ms) 
										может проснуться чуть раньше или чуть позже 
										(на единицы миллисекунд), 
										что для прикладного кода обычно не критично.
	ИИ
	===================================================================	
*/ 
void second_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	 // 	1. 	Запускаем задачу c super_heavy_work() асинхронно (она спит 2 секунды по вашему коду)
    std::future<double> 	future_result = std::async	(	std::launch::async
												, 	super_heavy_work
												, 	2.0
												);

    cout << "[Главный поток] Ожидаем результат с таймаутами...\n";

    // 	2. 	Цикл ожидания, пока статус не станет std::future_status::ready
    while (true) {
        // Проверяем готовность каждые 400 миллисекунд
        std::future_status 	status 	= 	future_result.wait_for(400ms);

        if (status == std::future_status::ready) {
            cout << "\n[Главный поток] Ура! Задача готова.\n";
            break; 	// 	Выходим из цикла ожидания
        } 
		// NOLINTNEXTLINE(readability-else-after-return)
        else if (status == std::future_status::timeout) {
            // 	Задача еще считается, выводим точку прогресса в текущей строке
            cout << "." << std::flush; 
        } 
        else if (status == std::future_status::deferred) {
            cout << "\n[Главный поток] Задача была запущена с политикой std::launch::deferred, отложена, вычислений в фоне нет!\n";
            break;
        }
    }

    // 	3. 	Безопасно забираем результат (он гарантированно готов, блокировки не будет)
    try {
        double result = future_result.get();
        cout << "[Главный поток] Финальный результат: " << result << '\n';
    } catch (const std::exception& excptn) {
        cout << "\n[Главный поток] Перехвачено исключение: " << excptn.what() << "\n";
    }	
}
/*
	----------------------------------------------------------------------------------------------------------------------------------------------------------
*/
struct X_Test {
	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	void	foo(int iPar1, const std::string & strPar2) {
		cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
			<< std::this_thread::get_id() 
			<< "; iPar1 == " << iPar1 << "; strPar2 == " << strPar2 << '\n';
	};
	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	auto	bar(const std::string & strng) -> std::string		{ return strng + "Hello!"; };
	
	int		int_var{};
	
	double	dbl_var{};
};
// 	Структура - функтор
struct Y_Test	{
	auto operator()(double par) -> double { return 2 * par; };
};

auto free_baz(X_Test& par) -> X_Test& {
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	par.int_var = 4242;
	
	return 	par;
}

class move_only {
 public:
	// Конструктор
	move_only() {
		cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	};
	~move_only() = default;

	move_only(move_only&&	other) noexcept = default;
	move_only(const move_only & other) = delete;
	
	auto operator=(move_only&& other[[maybe_unused]])  noexcept  -> move_only&  { return *this; };
	auto operator=(const move_only & other) -> move_only& = delete;
	
	// Функтор - перегружен оператор вызова
	void operator()() {
		cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	};
};
void third_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
		
	X_Test		x_test;
	// 	Вызов ptr->foo(42, "hello"), где рtr - это &x_test
	(&x_test)->foo(42, "Hello, Solar Wind! Я и не думал, что так можно...");
	/*
		Если первым аргументом является указатель на компонентную функцию, 
		второй аргумент предоставляет объект, 
		к которому применяется компонентная функция 
		(либо напрямую, либо через указатель, либо заключенным в оболочку std::ref), 
		а остальные аргументы передаются компонентной функции 
		в качестве ее простых аргументов. 
	*/
	auto		futures_1	= std::async	(	&X_Test::foo
									,	&x_test
									,	542
									,	"Hello, Solar Wind! It's Normal"
									);
	// 	Вызов tmpx.bar("goodbye"), где tmpx - это копия x_test
	auto		futures_2	= std::async	(	&X_Test::bar
									, 	x_test
									, 	"Goodbye, Baby!"
									);

	// 	Вызов временного объекта tmp(), где `tmp` создается из std::move(move_only())
	// 	Так как конструктор копирования объявлен удаленным, то был вызван конструктор перемещения
	move_only	tmp_obj;
	auto		futures_3	= std::async(std::move(tmp_obj));
	/*
		• 	Проблема: 		Выражение (move_only()) создает временный объект (prvalue). 
			
							В C++11/14 для передачи его в std::async требовалось перемещение 
							(вызывался move-конструктор). 
						
							Начиная с C++17, благодаря правилу Guaranteed Copy Elision, 	
							этот объект конструируется сразу «по месту» внутри std::async. 
						
							Однако сигнатура std::async всё равно требует, 
							чтобы внутренние типы были сдвигаемыми.
						
		• 	Как исправить: 	Чтобы явно показать намерение переместить `move-only` объект, 
							правильно писать: std::async(std::move(tmp_obj)) 
							или передавать его через `лямбду`.
	*/
	
	// Запуск в новом потоке асинхронно
	auto		futures_4	= std::async	(	std::launch::async
									, 	Y_Test()
									, 	1.2
									);	

	// 	Запуск в wait() или get() - а здесь откладвается
	// 	до того момента, когда будет вызвана функция-член wait() или get() объекта - будущего;
	// 	а здесь копия объекта не создается, так как передается ссылка на него в обертке std::ref()
	auto		futures_5	= std::async	(	std::launch::deferred
									, 	free_baz			//	X_Test&		free_baz(X_Test& par)
									, 	std::ref(x_test)
									);
	// !!! А ТАК НЕ КАТИТ!!! auto	futures_5 = std::async(std::launch::deferred, free_baz, ((x_test)));
	
	// 	Вызов отложенной функции по усмотрению реализации
	auto		futures_6	= std::async	(	std::launch::deferred | std::launch::async
									,	free_baz				//	X_Test&		free_baz(X_Test& par)
									, 	std::ref(x_test)
									);
	// Вызов отложенной функции по усмотрению реализации
	auto		futures_7	= std::async	(	free_baz				//	X_Test&		free_baz(X_Test& par)
									,	std::ref(x_test)
									);

	Y_Test	y_test;
	// 	Вызов y_test(3.141)
	auto		futures_8	= std::async((Y_Test()), std::numbers::pi);

	// 	Вызов y_test(2.718)
	auto		futures_9	= std::async(std::ref(y_test), std::numbers::e);

	//	X_Test&		free_baz(X_Test& par)
	auto		futures_10	= std::async(free_baz, std::ref(x_test));

	auto		X_Test_obj_1	= futures_4.get();
	cout << "auto X_Test_obj_1  = futures_4.get(); == " << X_Test_obj_1 << '\n';

	// 	Вызов отложенной параметром std::launch::deferred функции - дополнительная гибкость - 
	// 	иначе придется ее вызывать методом get() и потом ждать, пока она отработает
	futures_5.wait();
	// 	Вызов auto 	X_Test_obj_2 = futures_5.get(); не приведет к желаемому результату, 
	// 	так как `auto` отбрасывает ссылку
	// 	Поэтому явно укажем тип возвращаемой величины
	// 	X_Test&		X_Test_obj_2		= 	futures_5.get();
	// 	А можно и так:
	// 	auto		X_Test_obj_2 = (futures_5.get());		// 	Компенсируем отброшенную ссылку
	// 	А можно и так:
	decltype(auto)	X_Test_obj_2 = futures_5.get();
	cout << "X_Test&  X_Test_obj_2 = futures_5.get(); == " << X_Test_obj_2.int_var << '\n';

	X_Test_obj_2.int_var = 42;
	cout << "После присваивания X_Test::int_var == " << X_Test_obj_2.int_var << '\n';
}
/*
	===================================================================
*/
void void_thread_function(double dbl_param)
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << "; dbl_param == " << dbl_param << '\n';
		
	if (dbl_param > std::numeric_limits<double>::max())	{
		throw std::runtime_error("Что-то пошло не так!");
	} else	// NOLINT(readability-else-after-return)
	{
		return;
	}			
}
void fourth_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';

	cout << "[Главный поток] Запускаем задачу через std::async...\n";

    // 	1. 	Запуск в ОДНУ строчку. 
	//		Поток создается сам, аргументы передаются прямо туда.
    // 		Используем явную политику std::launch::async для гарантированного фонового потока.
    std::future<void> 	future_result = std::async	(	std::launch::async | std::launch::deferred
												, 	void_thread_function
												, 	2.0
												);
	
    cout << "[Главный поток] Задача крутится в фоне, а мы можем делать что-то еще.\n";
    std::this_thread::sleep_for(500ms);

    try {
			future_result.get();		//	повторное выбрасывание исключения, возникшее в функции потока
	} catch (const std::exception& excptn) {
        cout << "[Главный поток] Перехвачено исключение: " << excptn.what() << "\n";
    }
}
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
	//third_example();
	fourth_example();
	
	//system("pause");
	
	return 0;
}	

