#pragma once
#include "procon34.hpp"
#include "Board.hpp"
#include <queue>

namespace procon34 {
	namespace AutoPlay {
		Action Power(const Mason mason, Board& board);
		//Action BFS(const ActionPlan actionPlan, const Mason mason, Board& board);
	}
}
