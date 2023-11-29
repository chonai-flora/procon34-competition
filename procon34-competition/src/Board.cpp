#include "Board.hpp"

namespace procon34 {
	Board::Board()
		: m_turn(0)
		, m_width(0)
		, m_height(0) {}

	Board::Board(JSON boardJson)
		: m_turn(boardJson[U"turn"].get<int32>() + 1)
		, m_width(boardJson[U"board"][U"width"].get<int32>())
		, m_height(boardJson[U"board"][U"height"].get<int32>()) {
		for (const auto&& [_, line] : boardJson[U"board"][U"structures"]) {
			Array<Structure> tmp;
			for (const auto&& [$, value] : line) {
				tmp << Cast<Structure>(value.get<int32>());
			}
			m_structures << tmp;
		}

		for (const auto&& [_, line] : boardJson[U"board"][U"masons"]) {
			Array<Mason> tmp;
			for (const auto&& [$, value] : line) {
				tmp << value.get<Mason>();
			}
			m_masons << tmp;
		}

		for (const auto&& [_, line] : boardJson[U"board"][U"walls"]) {
			Array<Wall> tmp;
			for (const auto&& [$, value] : line) {
				tmp << Cast<Wall>(value.get<int32>());
			}
			m_walls << tmp;
		}

		for (const auto&& [_, line] : boardJson[U"board"][U"territories"]) {
			Array<Territory> tmp;
			for (const auto&& [$, value] : line) {
				tmp << Cast<Territory>(value.get<int32>());
			}
			m_territories << tmp;
		}
	}

	int32 Board::turn() const {
		return m_turn;
	}

	int32 Board::width() const {
		return m_width;
	}

	int32 Board::height() const {
		return m_height;
	}

	Structure Board::structureAt(const int32 x, const int32 y) const {
		return m_structures.at(y).at(x);
	}

	Mason Board::masonAt(const int32 x, const int32 y) const {
		return m_masons.at(y).at(x);
	}

	Wall Board::wallAt(const int32 x, const int32 y) const {
		return m_walls.at(y).at(x);
	}

	Territory Board::territoryAt(const int32 x, const int32 y) const {
		return m_territories.at(y).at(x);
	}

	Point Board::getMasonPoint(const Mason mason) const {
		for (const auto& [x, y] : step({ m_masons.at(0).size(), m_masons.size() })) {
			if (masonAt(x, y) == mason) {
				return Point(x, y);
			}
		}

		return Point(-1, -1);
	}

	Grid<bool> Board::getLegalMoves(const ActionType type, const Point& point) const {
		Grid<bool> legalMoves(m_width, m_height, false);

		if (type == ActionType::STAY) {
			legalMoves[point] = true;
			return legalMoves;
		}
		const auto& x = point.x, y = point.y;
		for (const auto& [index, delta] : Indexed(Deltas)) {
			if (not InFieldRange(point + delta, m_width, m_height)) continue;

			const auto& p = x + delta.x, q = y + delta.y;
			const auto& structure = structureAt(p, q);
			const auto& mason = masonAt(p, q);
			const auto& territory = territoryAt(p, q);
			const auto& wall = wallAt(p, q);
			if (type == ActionType::MOVE) {
				if (structure != Structure::POND && mason == 0 && wall != Wall::OPPONENT) {
					legalMoves[q][p] = true;
				}
			}
			else if (index % 2 == 0) {
				if (type == ActionType::BUILD) {
					if (structure != Structure::CASTLE &&
						territory == Territory::NEUTRAL &&
						wall == Wall::NEUTRAL &&
						Cast<Team>(Sign(mason)) != Team::OPPONENT) {
						legalMoves[q][p] = true;
					}
				}
				else if (type == ActionType::DEMOLISH) {
					if (wall != Wall::NEUTRAL) {
						legalMoves[q][p] = true;
					}
				}
			}
		}

		return legalMoves;
	}
}
