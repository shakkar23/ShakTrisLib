#pragma once

#include "./../ShaktrisConstants.hpp"
#include <array>
#include <algorithm>

struct Botris {
	static constexpr std::array<int, 10> combo_table = { 0, 0, 1, 1, 1, 2, 2, 3, 3, 4 };
	static constexpr std::array<int, 5> attack_table = { 0, 0, 1, 2, 4 };
	static constexpr std::array<int, 5> all_spin_bonus = { 0, 2, 4, 6, 0 };
	static constexpr int pc_bonus = 10;
	static constexpr int b2b_bonus = 1;

	std::size_t points(int linesCleared, spinType spin, bool pc, auto& combo, auto& b2b) {
		std::size_t garbage = 0;


		if (linesCleared) {
			auto maintainsB2B = false;
			combo++;
			if (4 == linesCleared || spin != spinType::null)
				maintainsB2B = true;

			if (maintainsB2B)
				b2b = true;
			else
				b2b = false;
		}
		else {
			combo = 0;
		}

		if (linesCleared) {
			garbage += attack_table[linesCleared];
			if(spin == spinType::normal)
				garbage += all_spin_bonus[linesCleared];

			// combo
			garbage += combo_table[std::min(combo_table.size() - 1, (size_t)combo)];

			if (pc)
				garbage += pc_bonus;
			if (b2b)
				garbage += b2b_bonus;
		}


		return garbage;
	}
};