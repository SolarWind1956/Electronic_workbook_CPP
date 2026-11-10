/*
	hand_over_hand_locking.cpp
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
	*	🛡️ Thread Safety Analysis (Аннотации потокобезопасности)
	*
	*****************************************************************************************************
	
	Если вы используете Clang, в нем есть мощный механизм статического анализа потокобезопасности. 
	
	Вы можете прямо в коде «подсказать» анализатору, 
	какой мьютекс какую переменную защищает. 
	
	Если вы забудете захватить замок или сделаете std::move не туда, 
	компилятор выдаст ошибку еще на этапе сборки.
	
	Для этого используются специальные атрибуты (макросы):
*/
// В реальных проектах эти макросы обычно определены в базовых хедерах
#define GUARDED_BY(x) __attribute__((implemented_by_attributes_or_clang)) 
// (Для Clang: __attribute__((guarded_by(x))))

/*
	**************************************************************************************************************
	`Эстафетная блокировка с перекрытием` 
	(часто называется `hand-over-hand locking` или `lock coupling`) - это `паттерн синхронизации` 
	для обхода `связанных структур данных` 
	(например, односвязных/двухсвязных списков или Б-деревьев), 
	при котором поток захватывает `замок следующего узла` до того, 
	как отпустит замок текущего.
	
	Этот подход позволяет нескольким потокам 
	`одновременно` работать с `разными частями` одной структуры данных, 
	обеспечивая гораздо большую параллельность (concurrency), 
	чем блокировка всей структуры целиком.
	**************************************************************************************************************
	
	⚡️ Как это работает (Аналогия)

	Представьте, что вы лезете по канату или лестнице: 
	вы не отпускаете правую руку, 
	пока левая прочно не ухватится за следующую перекладину. 
	
	Если вы отпустите обе руки сразу - вы упадете 
	(в программировании это приведет к `состоянию гонки` / `race condition`, 
	когда другой поток успеет вклиниться и удалить следующий узел).
*/
/*
	**************************************************************************************************************
	
	💻 Реализация на C++ (Пример с `односвязным списком`)

	Ниже представлен 
	каноничный пример реализации безопасного обхода односвязного списка 
	с использованием std::unique_lock и std::mutex.
*/
struct Node 
{
    int 						data;
    std::mutex 				mtx; 	// 	`Каждый узел` имеет `свой собственный мьютекс`
    std::shared_ptr<Node> 	next;

    explicit	Node(int value) : data(value), next(nullptr) {}
};

class HandOverHandList 
{
 private:
    std::shared_ptr<Node> 	head;
    std::mutex 				head_mtx; 	// 	Мьютекс для защиты указателя на голову списка

 public:
    HandOverHandList() : head(nullptr) {}

    // 	1. 	Добавление в начало списка 
    void insert_front(int value) 
	{
        std::unique_lock<std::mutex> 	lock(head_mtx);
		
        auto 						new_node = std::make_shared<Node>(value);
		
        new_node->next 	= head;
		
        head 			= new_node;	// NOLINT(performance-use-std-move)
    }
	// 	2. 	ВСТАВКА В СЕРЕДИНУ: ищет target и вставляет value строго ПОСЛЕ него
    auto insert_after(int target, int value) -> bool 
	{
		std::unique_lock<std::mutex> 	current_lock(head_mtx);
		
		if (!head) {
			return false;
		}
		// 	Инициализируем первый узел
		auto 						current = head;
		
		std::unique_lock<std::mutex> 	next_lock(current->mtx);
		
		current_lock.unlock(); 				// 	Отпускаем голову
		
		current_lock = std::move(next_lock); 	// 	Теперь `current_lock` держит первый узел

		while (current) 
		{
			if (current->data == target) 
			{
				auto new_node 	= std::make_shared<Node>(value);
				
				new_node->next 	= current->next;
				
				current->next 	= new_node;	// NOLINT(performance-use-std-move)
				
				return true;
			}

			// 	Переходим к следующему узлу
			auto next_node = current->next;
			
			if (!next_node) {
				break; 			// 	Конец списка, дальше идти некуда
			}

			// 	Локальный замок для следующего узла.
			// 	Он создается ТОЛЬКО если узел существует.
			std::unique_lock<std::mutex> 	temp_lock(next_node->mtx);

			// Продвигаем указатель
			current = next_node;

			// 	Передаем владение: 
			// 	Старый замок в current_lock уничтожается (освобождая старый узел),
			// 	а новый замок из temp_lock занимает его место.
			current_lock = std::move(temp_lock); 
		}
		return false;
	}


    // 	3.	Поиск элемента с использованием эстафетной блокировки
	/*
		Внутри find() мы `обязаны` использовать std::unique_lock 
		по двум причинам:
		
		• 	Ручное освобождение: 
			
			Нам нужно вызывать current_lock.unlock() вручную до того, 
			как деструктор выйдет из области видимости. 
			
			std::lock_guard или std::scoped_lock так не умеют - они освобождают мьютекс 
			строго при уничтожении объекта.
			
		• 	Передача владения (std::move): 
			
			Нам необходимо передавать захваченный мьютекс из одной переменной (next_lock) 
			в другую (current_lock). 
			
			std::lock_guard и std::scoped_lock не перемещаемые (move-assignable) тип, 
			а std::unique_lock - перемещаемый.
	*/
    auto find(int target) -> bool 
	{
		std::unique_lock<std::mutex> 	current_lock(head_mtx);
		
		if (!head) {
			return false;
		}
		
		auto 						current = head;
		
		std::unique_lock<std::mutex> 	next_lock(current->mtx);
		
		current_lock.unlock(); 				// 	Отпускаем голову списка
		current_lock = std::move(next_lock); 	// 	Теперь первый узел под защитой current_lock

		while (current) {
			if (current->data == target) {
				return true; // Элемент найден, замок автоматически откроется при выходе
			}

			auto next_node = current->next;
			if (!next_node) {
				break; 					// 	узлов больше нет, выходим из цикла!
			}

			// 	Захватываем следующий узел в свежий локальный замок
			std::unique_lock<std::mutex> temp_lock(next_node->mtx);

			current = next_node;
			current_lock = std::move(temp_lock); 	// Шаг эстафеты: старый мьютекс освобождается
		}

		return false; 	// 	Дошли до конца и ничего не нашли
	}
	//	
	// 	4.	Безопасное удаление элемента по значению
    auto erase(int target) -> bool  
	{
        std::unique_lock<std::mutex> 	prev_lock(head_mtx);
		
        if (!head) {
			return false;
		}
        // 	Отдельный сценарий: удаление первого узла
        if (head->data == target) {
            head = head->next;
            return true;
        }

        // 	Подготовка к эстафете для удаления из глубины
        auto 						prev = head;
		
        std::unique_lock<std::mutex> 	current_lock(prev->mtx);
		
        prev_lock.unlock(); 
		
        prev_lock = std::move(current_lock); 	// 	prev_lock теперь держит узел 'prev'

        auto 						current = prev->next;

        while (current) 
		{
            // 	Захватываем узел, который проверяем на удаление
            std::unique_lock<std::mutex> temp_lock(current->mtx);

            if (current->data == target) {
                // 	И prev, и current гарантированно заблокированы
                prev->next = current->next;
                
				return true;
            }

            auto 	next_node = current->next;
            if (!next_node) {
                break; // Конец списка, удалять больше нечего
            }

            // 	Продвигаем эстафету вперед
            prev 		= current;
            current 		= next_node;
            prev_lock 	= std::move(temp_lock);
        }
        return false;
    }
};

void first_example() 
{
	cerr << "------ I'm " <<  __PRETTY_FUNCTION__ << " in thread " 
        << std::this_thread::get_id() << '\n';
	
	HandOverHandList 		list;
	
    list.insert_front(10);
    list.insert_front(20);
    list.insert_front(30);

    if (list.find(20)) {
        cout << "Элемент найден!" << '\n';
    } else {
        cout << "Элемент не найден." << '\n';
    }	
}

/*
	*****************************************************************************************************
	
	⚖️ Плюсы и минусы подхода


	Преимущества									
	
	Высокий параллелизм: 		Потоки могут «идти по цепочке» друг за другом.	
	Локальность блокировок: 		Блокируются только те узлы, с которыми идет работа.	
	Защита от удаления: 			Ни один поток не может удалить узел перед идущим потоком.	
	
	Недостатки
	
	Оверхед на блокировки: 		Захват/освобождение мьютекса на каждом шаге стоит дорого.
	Угроза Deadlock: 				Если один поток идет от начала к концу, а другой от конца к началу 
								(в двусвязном списке), они заблокируют друг друга.
	Сложность кода: 				Легко ошибиться с порядком деструкции/освобождения unique_lock.								
	
	🛠️ На что обратить внимание в C++

	1. 	Используйте std::unique_lock вместо std::lock_guard: 
		
		Вам нужна гибкость, чтобы явно вызывать .unlock() 
		и передавать владение замком через std::move.
	
	2. 	Строгий порядок (Направление): 
	
		Потоки должны обходить структуру данных `строго в одном направлении` 
		(например, только слева направо), иначе взаимная блокировка (Deadlock) неизбежна.
		
	3. 	Smart Pointers: 
	
		Использование std::shared_ptr гарантирует, что узел не удалится физически из памяти, 
		пока поток держит на него локальную ссылку, 
		даже если другой поток вырезал его из списка.

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

