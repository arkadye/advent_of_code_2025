#pragma once

#include "advent_testcase_setup.h"
#include "advent_test_inputs.h"
#include "advent_headers.h"
#include "advent_solutions.h"

static const verification_test tests[] =
{
	TESTCASE(testcase_one_a, 3),
	TESTCASE(testcase_one_b, 6),
	DAY(one,DAY_01_1_SOLUTION,DAY_01_2_SOLUTION),

	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[0], 11 + 22),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[1], 99),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[2], 1010),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[3], 1188511885),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[4], 222222),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[5], 0),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[6], 446446),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[7], 38593859),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[8], 0),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[9], 0),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO[10], 0),
	TESTCASE_WITH_ARG(testcase_two_a, DAY_TWO_COMBINED, 1227775554),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[0], 11 + 22),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[1], 99 + 111),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[2], 999 + 1010),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[3], 1188511885),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[4], 222222),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[5], 0),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[6], 446446),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[7], 38593859),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[8], 565656),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[9], 824824824),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO[10], 2121212121),
	TESTCASE_WITH_ARG(testcase_two_b, DAY_TWO_COMBINED, 4174379265),
	DAY(two,DAY_02_1_SOLUTION,DAY_02_2_SOLUTION),

	TESTCASE_WITH_ARG(testcase_three_a, DAY_THREE[0],98),
	TESTCASE_WITH_ARG(testcase_three_a, DAY_THREE[1],89),
	TESTCASE_WITH_ARG(testcase_three_a, DAY_THREE[2],78),
	TESTCASE_WITH_ARG(testcase_three_a, DAY_THREE[3],92),
	TESTCASE_WITH_ARG(testcase_three_a, DAY_THREE_COMBINED,357),
	TESTCASE_WITH_ARG(testcase_three_b, DAY_THREE[0],987654321111),
	TESTCASE_WITH_ARG(testcase_three_b, DAY_THREE[1],811111111119),
	TESTCASE_WITH_ARG(testcase_three_b, DAY_THREE[2],434234234278),
	TESTCASE_WITH_ARG(testcase_three_b, DAY_THREE[3],888911112111),
	TESTCASE_WITH_ARG(testcase_three_b, DAY_THREE_COMBINED,3121910778619),
	DAY(three,DAY_03_1_SOLUTION,DAY_03_2_SOLUTION),

	TESTCASE(testcase_four_a, 13),
	TESTCASE(testcase_four_b, 43),
	DAY(four,DAY_04_1_SOLUTION,DAY_04_2_SOLUTION),

	TESTCASE(testcase_five_a, 3),
	TESTCASE(testcase_five_b, 14),
	DAY(five,DAY_05_1_SOLUTION,DAY_05_2_SOLUTION),

	TESTCASE(testcase_six_a,4277556),
	TESTCASE(testcase_six_b,3263827),
	DAY(six,DAY_06_1_SOLUTION,DAY_06_2_SOLUTION),

	TESTCASE(testcase_seven_a,21),
	TESTCASE(testcase_seven_b, 40),
	DAY(seven,DAY_07_1_SOLUTION,DAY_07_2_SOLUTION),

	TESTCASE(testcase_eight_a, 40),
	TESTCASE(testcase_eight_b, 25272),
	DAY(eight,DAY_08_1_SOLUTION,DAY_08_2_SOLUTION),

	TESTCASE(testcase_nine_a, 50),
	TESTCASE(testcase_nine_b, 24),
	DAY(nine,DAY_09_1_SOLUTION,DAY_09_2_SOLUTION),

	TESTCASE_WITH_ARG(testcase_ten_a, DAY_TEN[0], 2),
	TESTCASE_WITH_ARG(testcase_ten_a, DAY_TEN[1], 3),
	TESTCASE_WITH_ARG(testcase_ten_a, DAY_TEN[2], 2),
	TESTCASE_WITH_ARG(testcase_ten_a, DAY_TEN_COMBINED, 7),
	TESTCASE_WITH_ARG(testcase_ten_b, DAY_TEN[0], 10),
	TESTCASE_WITH_ARG(testcase_ten_b, DAY_TEN[1], 12),
	TESTCASE_WITH_ARG(testcase_ten_b, DAY_TEN[2], 11),
	TESTCASE_WITH_ARG(testcase_ten_b, DAY_TEN_COMBINED, 33),
	DAY(ten, DAY_10_1_SOLUTION, DAY_10_2_SOLUTION),
	DAY(eleven, DAY_11_1_SOLUTION, DAY_11_2_SOLUTION),
	DAY(twelve, DAY_12_1_SOLUTION, DAY_12_2_SOLUTION)
};

#undef ARG
#undef TESTCASE
#undef FUNC_NAME
#undef TEST_DECL
#undef DAY
#undef DUMMY
#undef DUMMY_DAY
