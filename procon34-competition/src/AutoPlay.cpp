#include "AutoPlay.hpp"

namespace procon34 {
	namespace AutoPlay {
		Action Power(const Mason mason, Board& board) {
			const auto& width = board.width(), height = board.height();
			const auto& from{ board.getMasonPoint(mason) };
			Array<Action> actions;

			for (int32 i = 0; i < 8; i += 2) {
				const auto& to{ from + Deltas[i] };
				if (InFieldRange(to, width, height)) {
					const ActionType actionType{ board.wallAt(to.x, to.y) == Wall::OPPONENT
						? ActionType::DEMOLISH
						: ActionType::BUILD
					};
					if (board.getLegalMoves(actionType, from).at(to)) {
						actions << Action{
							actionType,
							(i + 3) % 8 + 1,
							to
						};
					}
				}
			}

			if (actions.isEmpty()) {
				int32 count = 0;
				while (true) {
					const auto& index = Random(7);
					const auto& to{ from + Deltas[index] };
					if (InFieldRange(to, width, height) && board.getLegalMoves(ActionType::MOVE, from).at(to)) {
						return Action{
							ActionType::MOVE,
							(index + 4) % 8 + 1,
							to
						};
					}

					if (count++ > 10) {
						return Action{
							ActionType::STAY,
							0,
							from
						};
					}
				}
			}

			return actions.choice();
		}

		//Action BFS(const ActionPlan actionPlan, const Mason mason, Board& board) {
		//	const auto& width = board.width(), height = board.height();
		//	const auto& start{ board.getMasonPoint(mason) };
		//	const auto& goal{ actionPlan.point };

		//	auto search = [&]() {
		//		Grid<int32> distances(width, height, -1);
		//		distances[start] = 1;
		//		std::queue<Point> que;
		//		que.push(start);

		//		while (not que.empty()) {
		//			const auto& from{ que.front() };
		//			que.pop();

		//			for (const auto& delta : Deltas) {
		//				const auto& to{ from + delta };
		//				if (InFieldRange(to, width, height) && board.getLegalMoves(ActionType::MOVE, from).at(to)) {
		//					que.push(to);
		//					distances[to] = distances[from] + 1;
		//				}
		//			}
		//		}

		//		Console << distances;
		//		return distances;
		//	};
		//	auto getNextPoint = [&](Grid<int32> distances) {
		//		Point from{ goal };
		//		int32 count = distances[goal];
		//		Array<Point> steps{ start, goal };

		//		while (count > 1) {
		//			count--;
		//			for (const auto& delta : Deltas) {
		//				Point to{ from + delta };
		//				if (distances[to] == count) {
		//					from = to;
		//					steps << from;
		//					break;
		//				}
		//			}
		//		}

		//		return steps.back();
		//	};

		//	if (actionPlan.type == ActionType::MOVE) {
		//		const auto& result{ search() };
		//		const auto& point{ getNextPoint(result) };

		//		return { ActionType::MOVE, mason, point };
		//	}

		//	return { ActionType::STAY, 0, Point(-1, -1) };
		//}
	}
}
