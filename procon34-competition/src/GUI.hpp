#pragma once
#include "procon34.hpp"
#include "Fetch.hpp"
#include "Match.hpp"
#include "AutoPlay.hpp"

namespace procon34 {
	class GUI {
	public:
		GUI(Match& match, Optional<Board> board = none);
		bool isReady() const;
		bool matchEnds() const;
		void drawMason(const int32 x, const int32 y) const;
		void drawAction(const int32 x, const int32 y) const;
		void drawAction(const int32 actionIndex, const int32 x, const int32 y) const;
		//void drawAction(const ActionPlan actionPlan, const int32 x, const int32 y) const;

		void stop();
		void fetchBoard();
		void inputActions();
		void updateActions();
		void calculateScores();
		void updateLegalMoves();
		void setMovePlan(const Mason mason, const int32 x, const int32 y);
		void setBuildPlan(const Mason mason, const int32 x, const int32 y);
		void setMason(const Mason mason, Optional<int32> x = unspecified, Optional<int32> y = unspecified);

		void update();
		void draw() const;

	private:
		Match m_match;
		Optional<Board> m_board;
		bool m_myTurn;
		int32 m_width;
		int32 m_height;
		int32 m_maxTurn;
		int32 m_masonCount;
		double m_turnSeconds;
		HashTable<Bonus, int32> m_bonus;

		Mason m_mason;
		bool m_submitted;
		size_t m_actionId;
		double m_blockLength;
		Vec2 m_margin;
		Point m_masonPoint;
		Stopwatch m_stopwatch;
		Array<int32> m_scores;
		Array<String> m_icons;
		Array<String> m_actionKeys;
		Array<Action> m_actions;
		Grid<RectF> m_blocks;
		Grid<bool> m_legalMoves;
		Array<Array<ActionPlan>> m_actionPlans;
	};
}
