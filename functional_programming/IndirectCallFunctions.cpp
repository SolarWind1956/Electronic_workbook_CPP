/*
	IndirectCallFunctions.cpp
*/
#include "IndirectCallFunctions.hpp"

auto main() -> int 
{
	cerr << "-------------- I'm " <<  __PRETTY_FUNCTION__ << '\n';
	
	cout << " I am Ch_16_1_6_Efficiency_and_flexibility.cpp" << '\n';
	/*
		------------ Привязка функции удаления во время выполнения ------------
	*/
	//the_first_step();
	//the_second_step();
	//the_third_step();
	//the_fourth_step();
	//the_fifth_step();
	//the_sixth_step();
	//the_seventh_step();
	//test_noexcept_operator_2();
	universal_indirect_call_test();
	
	return 0;
}

