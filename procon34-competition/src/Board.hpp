#pragma once
#include "procon34.hpp"

namespace procon34 {
	class Board {
	public:
		Board();
		Board(JSON boardJson);
		int32 turn() const;
		int32 width() const;
		int32 height() const;
		Structure structureAt(const int32 x, const int32 y) const;
		Mason masonAt(const int32 x, const int32 y) const;
		Wall wallAt(const int32 x, const int32 y) const;
		Territory territoryAt(const int32 x, const int32 y) const;
		Point getMasonPoint(const Mason mason) const;
		Grid<bool> getLegalMoves(const ActionType type, const Point& point) const;

	private:
		int32 m_turn;
		int32 m_width;
		int32 m_height;
		Array<Array<Structure>> m_structures;
		Array<Array<Mason>> m_masons;
		Array<Array<Wall>> m_walls;
		Array<Array<Territory>> m_territories;
	};
}
