/*
	Fluent_Interface.cpp
	
	**********************************************************************************************************************
	Method_Chaining_Fluent_Interface
	**********************************************************************************************************************
*/
#include <iostream>
using std::cout; using std::endl; using std::cerr;  using std::boolalpha;

#include <type_traits>
#include <vector>
#include <queue>
#include <ranges>
#include <algorithm>
#include <utility>	//	std::forward
#include <functional>

#include "../include/type_names_tester.hpp"
using namespace chat_gpt_seh;
/*
	****************************************************************************************************************
	`Цепочечное выполнение` функций 
	(также известное как `Method Chaining` или `Fluent Interface`) - 
	это подход в программировании, 
	при котором `несколько методов` вызываются `последовательно один за другим` 
	в `одной строке кода`. 
	
	В C++ это `чаще всего` реализуется за счет того, 
	что `каждый метод` класса `возвращает ссылку на сам объект` (*this).
*/
/*
	----------------------------------------------------------------------------------------------------------------------------------------------------------
	1. 	Классический Method Chaining (через return *this)

		Этот паттерн часто применяется в билдерах (Builder), 
		конфигураторах или классах для работы со строками/математикой. 
		
		Чтобы связать вызовы, методы должны возвращать Class&.
*/
class Person {
  private:
    std::string 	m_name;
    int 			m_age = 0;
    std::string 	m_city;

  public:
    // Каждый метод возвращает ссылку на текущий объект
    auto setName(const std::string& name) -> Person&  {
        m_name = name;
        return *this; 
    }

    auto setAge(int age) -> Person& {
        m_age = age;
        return *this;
    }

    auto setCity(const std::string& city) -> Person& {
        m_city = city;
        return *this;
    }

    void print() const {
        cout << m_name << " (" << m_age << " лет) из " << m_city << '\n';
    }
};
/*
	⚠️ 	Подводный камень: 
	
	Избегайте `возврата `по значению` (Person), 
	иначе на каждом шаге будет создаваться `временная копия объекта`. 
	
	Возвращайте `именно ссылку` ( -> Person&).
*/
void first_example() 
{
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';

	Person person;
    
    // 	Вызов методов по цепочке в одну строку
    person.setName("Алексей").setAge(25).setCity("Рига").print();
}
/*
	----------------------------------------------------------------------------------------------------------------------------------------------------------
	
	2. 	Цепочки через оператор `|` (Pipe) в C++20 `Ranges`

	В современном C++20 появился модуль <ranges>, 
	который позволяет элегантно выстраивать цепочки для обработки контейнеров 
	с использованием оператора `|` 
	(по аналогии с `конвейерами` в `Linux` или `Stream API` в других языках).
*/
void second_example() 
{
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
	
	std::vector<int> 	numbers = {1, 2, 3, 4, 5, 6};

    // 	Цепочка: фильтруем четные -> умножаем на 2
    auto result = numbers 
                | 	std::views::filter	([](int n_par) -> auto  { return n_par % 2 == 0; })
                | 	std::views::transform	([](int n_par) -> auto  { return n_par * 2; });

    for (int n_res : result) {
        cout << n_res << " "; // Выведет: 4 8 12
    }
	cout << '\n';
}
/*
	--------------------------------------------------------------------------------------------------------------------------------------------------------
	3. 	Композиция функций (Function Composition)

	Если `нужно передать` 
	`результат одной функции` 
	на `вход другой` (математическая композиция f(g(x))), 
	можно написать `обертку` `compose` 
	или 
	`перегрузить оператор передачи`.
	
	Начиная с C++23 и благодаря шаблонам общего вида (Variadic Templates), 
	разработчики часто собирают подобные конвейеры:
*/
// Простая композиция двух функций: f(g(x))
template <typename F, typename G>
auto compose(F f_func, G g_func) {
    return [=](auto x_par) -> auto { return f_func(g_func(x_par)); };
}
void third_example() 
{
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
	
	auto addOne = [](int x_par) -> auto  { return x_par + 5; };
    auto square = [](int x_par) -> auto  { return x_par * x_par; };

    // Создаем цепочку: сначала возведение в квадрат, потом прибавление единицы
    auto squareThenAddOne = compose(addOne, square);

    cout << squareThenAddOne(5); 	// 	(5 * 5) + 5 = 30
	cout << '\n';
}
/*
	----------------------------------------------------------------------------------------------------------------------------------------------------------
	4.	Цепочечный вызов методов из разных классов
*/
struct A_struct {
	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	void f_func()	{
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	}
};
struct B_struct {
	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	auto g_func() -> A_struct{
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
		return A_struct{};
	}
};
struct C_struct {
	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	auto h_func() -> B_struct{
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
		return B_struct{};
	}
};
void fourth_example() 
{
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
	
	C_struct 	c_obj;

	c_obj.h_func().g_func().f_func();
}
/*
	--------------------------------------------------------------------------------------------------------------------------------------------------------
	5.	Упрощенная модель цепочечных вызовов из книги Энтони Уильямса
*/
template	<	typename PreviousDispatcher
		,	typename Func
		>
struct TemplateDispatcher
{
	PreviousDispatcher* 	prev;		//	Поле внешне не задействовано, но без него все упадет
	Func 				functor;
	
	template<typename OtherFunc>
	auto handle(OtherFunc&& other_func) -> TemplateDispatcher<TemplateDispatcher, OtherFunc>
	{
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
		other_func();
		return TemplateDispatcher	<	TemplateDispatcher
								,	OtherFunc
								>	
								(	this
								,	std::forward<OtherFunc>(other_func)
								);
	}
};
struct dispatcher
{
	template<typename Func>
	auto handle(Func&& functor)-> TemplateDispatcher<dispatcher, Func>
	{
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
		functor();
		return TemplateDispatcher<dispatcher, Func>	(	this
												,	std::forward<Func>(functor)
												);
	}
};
struct receiver
{
	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	auto wait() -> dispatcher 
	{
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
		return dispatcher{};
	}
};
struct functor_1{
	void operator ()(){
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	}
};
struct functor_2{
	void operator ()(){
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	}
};
struct functor_3{
	void operator()(){
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	}
};
struct functor_4{
	void operator()(){
		cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	}
};
void fifth_example() 
{
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
	receiver 		incoming;
	
	incoming.wait()	.handle<functor_1>(functor_1())
					.handle<functor_2>(functor_2())
					.handle<functor_3>(functor_3())
					.handle<functor_4>(functor_4())
					;
}
/*
	Что именно удалось интуитивно поймать и реализовать:

	Наращивание типов (Type Accumulation): 
	
		Каждый вызов .handle() не просто выполняет действие, 
		он порождает новый тип `TemplateDispatcher`, 
		в который в качестве первого параметра (PreviousDispatcher) 
		заворачивается весь предыдущий контекст. 
		
		Компилятор видит эту цепочку как один сложный вложенный тип.

	Сохранение контекста: 
	
		В реальной книге этот механизм нужен для того, 
		чтобы отслеживать, 
ё+		было ли сообщение уже перехвачено. 
		
		Если сообщение поймал первый обработчик (functor_1), 
		то последующие `TemplateDispatcher` на основе типов 
		узнают об этом и пропускают выполнение. 
		
		Вы создали чистейший скелет этого паттерна!
*/
/*
	1. 	«Спинномозговое» ощущение `рекурсии`
	
		Вы абсолютно правы: это `рекурсия`, 
		но происходящая `на этапе компиляции` (Compile-Time Recursion).

		В традиционном программировании 
		рекурсия раскручивается в `рантайме` (функция вызывает функцию). 
		
		Здесь же рекурсия раскручивается в типах данных. 
		
		Каждый новый вызов .handle() 
		порождает новый шаблонный тип, 
		который внутрь себя «заворачивает» 
		`предыдущий тип`. 
		
		Именно поэтому в консольном выводе типы растут вглубь, как матрешка:
		
		TemplateDispatcher<TemplateDispatcher<TemplateDispatcher<...>>>.

	2. 	Роль this
	
		Вы абсолютно правы, `this` здесь - `ключевой связующий элемент`.

		Когда вы вызываете .handle() у текущего диспетчера, 
		метод создает новый объект `TemplateDispatcher` 
		и передает `в его конструктор` 
		`указатель на текущий объект` (this). 
		
		Таким образом, `каждый новый член цепочки` 
		буквально «держит за руку» своего `родителя` (предыдущий диспетчер), 
		выстраивая в памяти `связный список объектов`.

	3. 	Загадка поля `PreviousDispatcher* prev` (почему без него всё падает?)
	
		Это классическая ловушка, в которую попадают многие при изучении шаблонов! 
		
		Кажется: 
		
		«Раз мы поле `prev` нигде не читаем, 
		и методы у него не вызываем, 
		зачем оно нужно?»

		Тут кроются `две` причины (одна `техническая`, вторая `архитектурная`):

		-	`Техническая` (конструктор): 
		
			В конструкторе вы передаете `this` (`указатель` на `предыдущий диспетчер`). 
			
			Компилятор ожидает, что этот аргумент куда-то запишется. 
			
			Если вы удалите поле `prev`, 
			но оставите передачу `this` в конструкторе, 
			компилятор выдаст `ошибку несоответствия аргументов` 
			(вы пытаетесь передать то, чего у класса больше нет в памяти).

		-	`Архитектурная` (сохранение жизни объектов): 
		
			В более полной версии кода (из книги) 
			эти диспетчеры часто передаются по цепочке, 
			и каждый из них может ссылаться на данные 
			или состояние предыдущего. 
			
			Но главное - `тип этого поля` (`PreviousDispatcher`) 
			участвует в определении типа самого класса `TemplateDispatcher`. 
			
			Без этого типа шаблон `TemplateDispatcher<PreviousDispatcher, Func>` 
			просто потерял бы свою первую «половину», 
			и вся рекурсивная цепочка типов рассыпалась бы на этапе компиляции.
*/
/*
	****************************************************************************************************************
	int main()
	****************************************************************************************************************
*/
auto main() -> int {
	cout << "-------------- I'm " <<  __PRETTY_FUNCTION__  << '\n';
		
	//first_example();
	//second_example();
	//third_example();
	//fourth_example();
	fifth_example();
	//system("pause");
	
	return 0;
}	

