#pragma once
#include <Siv3D.hpp>

namespace procon34 {
	static const Size WindowSize{ 1440, 900 };

	enum class Team {
		ME = +1,		// 自チーム
		OPPONENT = -1,	// 相手チーム
	};

	enum class Bonus : int32 {
		WALL,		// 壁
		TERRITORY,	// 陣地
		CASTLE,		// 城
	};

	enum class Structure : int32 {
		NEUTRAL,	// 無配置
		POND,		// 池
		CASTLE,		// 城
	};

	using Mason = int32;	// 職人

	enum class Territory : int32 {
		NEUTRAL,	// 無配置
		MINE,		// 自チーム
		OPPONENT,	// 相手チーム
	};

	enum class Wall : int32 {
		NEUTRAL,	// なし
		MINE,		// 自チーム
		OPPONENT,	// 相手チーム
	};

	enum class ActionType : int32 {
		STAY,		// 滞在
		MOVE,		// 移動
		BUILD,		// 建築
		DEMOLISH,	// 解体
	};

	using ActionDir = int32;

	struct Action {
		ActionType type;
		ActionDir dir;
		Point point;
	};

	struct ActionPlan {
		ActionType type;
		Mason mason;
		Point point;
	};

	static const HashTable<Team, Color> TeamColor{
		{ Team::ME, Palette::Blue },
		{ Team::OPPONENT, Palette::Red }
	};

	static const HashTable<Territory, Color> TerritoryColor{
		{ Territory::NEUTRAL, Palette::White },
		{ Territory::MINE, Palette::Lightblue },
		{ Territory::OPPONENT, Palette::Greenyellow }
	};

	static const HashTable<Wall, Color> WallColor{
		{ Wall::NEUTRAL, Palette::Silver },
		{ Wall::MINE, TeamColor.at(Team::ME) },
		{ Wall::OPPONENT, TeamColor.at(Team::OPPONENT) }
	};

	static const Array<Point> Deltas{
		{  1, 0 }, {  1, 1 }, {  0, 1 }, { -1, 1 },
		{ -1, 0 }, { -1,-1 }, {  0,-1 }, {  1,-1 }
	};

	template <typename T, typename U>
	inline T Cast(U value) {
		return static_cast<T>(value);
	}

	inline bool InFieldRange(const Point& point, const int32 width, const int32 height) {
		return (InRange(point.x, 0, width - 1) && InRange(point.y, 0, height - 1));
;	}
}
