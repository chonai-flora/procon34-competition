#include "GUI.hpp"

namespace procon34 {
	GUI::GUI(Match& match, Optional<Board> board)
		: m_match(match)
		, m_board(board)
		, m_myTurn(m_match.first())
		, m_width(m_match.width())
		, m_height(m_match.height())
		, m_maxTurn(m_match.maxTurn())
		, m_masonCount(m_match.masonCount())
		, m_turnSeconds(m_match.turnSeconds())
		, m_bonus(m_match.bonus())
		, m_mason(1)
		, m_submitted(false)
		, m_actionId(1)
		, m_blockLength(WindowSize.y / Cast<double>(Max(m_width, m_height)))
		, m_margin((WindowSize.x - m_blockLength * m_width) / 2.0, (WindowSize.y - m_blockLength * m_height) / 2.0)
		, m_masonPoint(-1, -1)
		, m_stopwatch()
		, m_scores({ 0, 0 })
		, m_icons({ U"", U"\U000F0F22", U"\U000F0D32", U"\U000F17C2" })
		, m_actionKeys({ U"滞在", U"移動", U"建築", U"解体" })
		, m_actions(m_masonCount + 1, { ActionType::STAY, 0, Point(-1, -1) })
		, m_blocks(m_width, m_height, Rect())
		, m_legalMoves(m_width, m_height, false)
		, m_actionPlans(m_masonCount + 1) {
		for (const auto& y : step(m_height)) {
			for (const auto& x : step(m_width)) {
				m_blocks[y][x] = RectF(x * m_blockLength + m_margin.x, y * m_blockLength + m_margin.y, { m_blockLength }).stretched(-m_blockLength * 0.05);
			}
		}
	}

	bool GUI::isReady() const {
		return m_board.has_value();
	}

	bool GUI::matchEnds() const {
		return (m_board->turn() > m_maxTurn);
	}

	void GUI::drawMason(const int32 x, const int32 y) const {
		const auto& size = m_blockLength * 0.275;
		const auto& mason{ m_board->masonAt(x, y) };
		const auto& color{ TeamColor.at(Cast<Team>(Sign(mason))) };
		const Vec2& center{ (x + 0.5) * m_blockLength + m_margin.x, (y + 0.5) * m_blockLength + m_margin.y };

		Circle{ center, size }
		.drawFrame(2, color);
		FontAsset(Format(Round(size / 5.0) * 5) + U" bold")(U"{:+}"_fmt(mason))
			.draw(Arg::center(center), color);
	}

	void GUI::drawAction(const int32 x, const int32 y) const {
		const Vec2& center{ (x + 0.5) * m_blockLength + m_margin.x, (y + 0.5) * m_blockLength + m_margin.y };

		FontAsset(U"icon")(m_icons[m_actionId])
			.drawAt(m_blockLength / 3, center, ColorF{ 0.5 });
		if (Cast<ActionType>(m_actionId) == ActionType::BUILD) {
			FontAsset(U"icon")(U"\U000F1323")
				.drawAt(m_blockLength / 5.25, center, ColorF{ 0.5 });
		}
	}

	void GUI::drawAction(const int32 actionIndex, const int32 x, const int32 y) const {
		const int32 size = m_blockLength * 0.2;
		const Vec2& center{ (x + 0.5) * m_blockLength + m_margin.x, (y + 0.5) * m_blockLength + m_margin.y };
		const Vec2& movedPoint{ center.movedBy(size * 0.9, -size * 0.9) };
		const Array<String> masonIds{ U"", U"①", U"②", U"③", U"④", U"⑤", U"⑥"};

		const size_t actionId = Cast<size_t>(m_actions.at(actionIndex).type);
		FontAsset(U"icon")(m_icons[actionId])
			.drawAt(m_blockLength / 2, center, ColorF{ 0.1 });
		if (Cast<ActionType>(actionId) == ActionType::BUILD) {
			FontAsset(U"icon")(U"\U000F1323")
				.drawAt(m_blockLength / 3.5, center, ColorF{ 0.1 });
		}

		Circle{ movedPoint, size / 2 }
			.draw(Palette::White);
		FontAsset(U"{} bold"_fmt(Round(size / 6.0) * 5))(masonIds.at(actionIndex))
			.draw(Arg::center(movedPoint), Palette::Black);
	}

	//void GUI::drawAction(const ActionPlan actionPlan, const int32 x, const int32 y) const {
	//	const int32 size = m_blockLength * 0.2;
	//	const Vec2& center{ (x + 0.5) * m_blockLength + m_margin.x, (y + 0.5) * m_blockLength + m_margin.y };
	//	const Vec2& movedPoint{ center.movedBy(size * 0.9, -size * 0.9) };
	//	const Array<String> masonIds{ U"①", U"②", U"③", U"④",U"⑤",U"⑥" };

	//	const size_t actionId = Cast<size_t>(actionPlan.type);
	//	FontAsset(U"icon")(m_icons[actionId])
	//		.drawAt(m_blockLength / 3, center, ColorF{ 0.3 });
	//	if (Cast<ActionType>(actionId) == ActionType::BUILD) {
	//		FontAsset(U"icon")(U"\U000F1323")
	//			.drawAt(m_blockLength / 5.25, center, ColorF{ 0.3 });
	//	}

	//	m_blocks[y][x]
	//		.stretched(-m_blockLength * 0.15)
	//		.drawFrame(3.0, TeamColor.at(Team::ME).withAlpha(128));
	//	Circle{ movedPoint, size / 2 }
	//		.draw(Palette::White);
	//	FontAsset(U"{} bold"_fmt(Round(size / 6.0) * 5))(masonIds.at(Cast<size_t>(actionPlan.mason) - 1))
	//		.draw(Arg::center(movedPoint), Palette::Black);
	//	// TODO: ActionPlanに職人ID
	//	// TODO: 最短経路割り出す
	//	// TODO: 
	//}

	void GUI::stop() {
		fetchBoard();
		m_stopwatch.pause();
	}

	void GUI::fetchBoard() {
		const int32 matchId = m_match.id();
		const auto& [response, boardJson] = Fetch::Get(U"{}/matches/{}"_fmt(m_match.serverURL(), matchId), m_match.token(), U"match{}.json"_fmt(matchId));

		if (response.isOK()) {
			m_board = std::move(boardJson);
			m_submitted = false;

			calculateScores();
			setMason(m_mason);

			if (m_board->turn() > 1) {
				m_myTurn = not m_myTurn;
			}
			else {
				m_stopwatch.start();
				updateActions();
			}

			if (not m_myTurn && not matchEnds()) {
				updateActions();
			}
		}
	}

	void GUI::inputActions() {
		// 職人指定
		for (uint8 id = 1; id <= m_masonCount; id++) {
			if (Input{ InputDeviceType::Keyboard, Cast<uint8>(0x30U + id) }.down()) {
				setMason(id);
			}
		}

		// 方向指定
		for (uint8 id = 1; id <= 9; id++) {
			if (Input{ InputDeviceType::Keyboard, Cast<uint8>(0x60U + id) }.down()) {
				const HashTable<int32, ActionDir> directions{
					{ 7, 1 }, { 8, 2 }, { 9, 3 },
					{ 4, 8 }, { 5, 0 }, { 6, 4 },
					{ 1, 7 }, { 2, 6 }, { 3, 5 },
				};

				const auto& dir = directions.at(Cast<int32>(id));
				const auto& point{ m_board->getMasonPoint(m_mason) + Deltas[Cast<size_t>(dir + 4) % 8] };
				if (dir == 0) {
					m_actions[Abs(m_mason)] = {
						ActionType::STAY,
						dir,
						m_board->getMasonPoint(m_mason)
					};
				}
				else if (m_legalMoves[point]) {
					m_actions[Abs(m_mason)] = {
						Cast<ActionType>(m_actionId),
						dir,
						point
					};
				}
			}
		}

		// 行動指定
		if (KeyUp.down()) {
			m_actionId = Cast<size_t>(Max(Cast<int32>(m_actionId) - 1, 0));
			updateLegalMoves();
		}
		else if (KeyDown.down()) {
			m_actionId = Cast<size_t>(Min(Cast<int32>(m_actionId) + 1, 3));
			updateLegalMoves();
		}

		const Array<Input> keys{ KeyNum5, KeyBackspace, KeyNumSubtract, KeyNumAdd };
		for (const auto& [index, key] : Indexed(keys)) {
			if (key.down()) {
				m_actionId = index;
				updateLegalMoves();
			}
		}

		for (const auto& y : step(m_height)) {
			for (const auto& x : step(m_width)) {
				const auto& mason{ m_board->masonAt(x, y) };

				if (m_blocks[y][x].leftClicked()) {
					if (Cast<Team>(Sign(mason)) == Team::ME) {
						setMason(mason, x, y);
					}
					else if (m_legalMoves[y][x]) {
						const Grid<ActionDir> directions{
							{ 1, 2, 3 },
							{ 8, 0, 4 },
							{ 7, 6, 5 },
						};
						const auto& dir = directions[Point(x + 1, y + 1) - m_masonPoint];
						m_actions[Abs(m_mason)] = {
							Cast<ActionType>(m_actionId),
							dir,
							Point(x, y)
						};
					}
				}
			}
		}

		const Point actionPoint{ 1200, 720 };
		if (SimpleGUI::RadioButtons(m_actionId, m_actionKeys, actionPoint, 200)) {
			updateLegalMoves();
		}
		SimpleGUI::RadioButtonsRegion(m_actionKeys, actionPoint, 200)
			.rounded(5.0)
			.drawFrame(1.0, Palette::Black);
		FontAsset(U"25")(U"職人{:+}"_fmt(m_board->masonAt(m_masonPoint.x, m_masonPoint.y)))
			.drawAt(actionPoint.movedBy(45, -20), Palette::Black);
	}

	void GUI::updateActions() {
		m_actions.assign(m_masonCount + 1, { ActionType::STAY, 0, Point(-1, -1) });

		for (const Mason& mason : Range(1, m_masonCount)) {
			ActionPlan nearest{ ActionType::STAY, 0, Point(-1, -1) };
			double minDistance = m_width + m_height;
			const Point masonPoint{ m_board->getMasonPoint(mason) };
			for (const auto& actionPlan : m_actionPlans[mason]) {
				// とりあえず
				if (actionPlan.type != ActionType::MOVE) continue;

				const double distance = masonPoint.distanceFrom(actionPlan.point);
				if (distance < minDistance) {
					nearest = actionPlan;
					minDistance = distance;
				}
			}

			if (nearest.mason > 0) {
				//m_actions[index] = AutoPlay::BFS(nearest, index + 1, *m_board);
			}
			/* ------ */
			if (m_actions[mason].type == ActionType::STAY) {
				m_actions[mason] = AutoPlay::Power(mason, *m_board);
			}
		}

		updateLegalMoves();
	}

	void GUI::calculateScores() {
		m_scores.assign(2, 0);

		for (const auto& y : step(m_height)) {
			for (const auto& x : step(m_width)) {
				const auto& wall{ m_board->wallAt(x, y) };
				const auto& territory{ m_board->territoryAt(x, y) };
				const auto& structure{ m_board->structureAt(x, y) };

				if (wall != Wall::NEUTRAL) {
					m_scores[Cast<size_t>(wall) - 1] += m_bonus.at(Bonus::WALL);
				}

				if (territory != Territory::NEUTRAL) {
					m_scores[Cast<size_t>(territory) - 1] += m_bonus.at(Bonus::TERRITORY);

					if (structure == Structure::CASTLE) {
						m_scores[Cast<size_t>(territory) - 1] += m_bonus.at(Bonus::CASTLE);
					}
				}
			}
		}
	}

	void GUI::updateLegalMoves() {
		m_legalMoves = std::move(m_board->getLegalMoves(Cast<ActionType>(m_actionId), m_masonPoint));
	}

	void GUI::setMovePlan(const Mason mason, const int32 x, const int32 y) {
		const Point point{ x, y };
		if (m_board->getLegalMoves(ActionType::MOVE, point)) {
			m_actionPlans[mason] << ActionPlan{ ActionType::MOVE, mason, point };
		}
	}

	void GUI::setBuildPlan(const Mason mason, const int32 x, const int32 y) {
		const Point from{ x, y };
		if (m_board->getLegalMoves(ActionType::BUILD, from)) {
			m_actionPlans[mason] << ActionPlan{ ActionType::BUILD, mason, from };

			for (int32 i = 0; i < 8; i += 2) {
				const Point to{ from + Deltas[i] };
				if (InFieldRange(to, m_width, m_height)) {
					if (m_board->getLegalMoves(ActionType::MOVE, from).at(to)) {
						setMovePlan(mason, to.x, to.y);
					}
				}
			}
		}
	}

	void GUI::setMason(const Mason mason, Optional<int32> x, Optional<int32> y) {
		if (x.has_value() && y.has_value()) {
			m_masonPoint.set(*x, *y);
		}
		else {
			m_masonPoint.set(m_board->getMasonPoint(mason));
		}
		m_mason = mason;
		updateLegalMoves();
	}

	void GUI::update() {
		if (m_turnSeconds - m_stopwatch.sF() < 0.001) {
			m_stopwatch.reset();
			m_stopwatch.start();
			fetchBoard();
		}

		if (KeySpace.down()) {
			setBuildPlan(1, 3, 4);
			updateActions();
		}

		if (not m_submitted && m_myTurn && m_turnSeconds - m_stopwatch.sF() <= 0.2) {
			const int32 matchId = m_match.id();
			const auto& [response, _] = Fetch::Post(U"{}/matches/{}"_fmt(m_match.serverURL(), matchId), m_match.token(), m_board->turn(), m_actions, U"match{}_turn{:0>2}.json"_fmt(matchId, m_board->turn()));

			if (response.isOK()) {
				Console << U"ターン{:0>2}: "_fmt(m_board->turn()) << U"提出成功！\n";
				m_submitted = true;
			}
			else {
				Console << U"ターン{:0>2}: "_fmt(m_board->turn()) << U"提出失敗...\n";
			}
		}

		inputActions();
	}

	void GUI::draw() const {
		for (const auto& y : step(m_height)) {
			for (const auto& x : step(m_width)) {
				auto getActionIndex = [&]() {
					for (const auto& [index, action] : Indexed(m_actions)) {
						if (action.point == Point(x, y)) {
							return Cast<int32>(index);
						}
					}

					return -1;
				};
				auto getActionPlan = [&]() {
					for (const auto& index : step(m_masonCount)) {
						for (const auto& actionPlan : m_actionPlans[index]) {
							if (actionPlan.point == Point(x, y)) {
								return actionPlan;
							}
						}
					}

					return ActionPlan{ ActionType::STAY, 0, Point(-1, -1) };
				};

				const int32 actionIndex = getActionIndex();
				const ActionPlan actionPlan{ getActionPlan()};

				const bool masonExists = (m_board->masonAt(x, y) != 0);
				const bool actionExists = (actionIndex >= 0);
				const bool actionPlanExists = (actionPlan.mason > 0);
				const bool someExists = (masonExists || actionExists || actionPlanExists || m_legalMoves[y][x]);

				const auto& center{ m_blocks[y][x].center() };
				const auto& wall{ m_board->wallAt(x, y) };
				const auto& territory{ m_board->territoryAt(x, y) };
				const auto& structure{ m_board->structureAt(x, y) };

				m_blocks[y][x]
					.draw(TerritoryColor.at(territory))
					.drawFrame(
						m_masonPoint == Point(x, y) ? m_blockLength * 0.07 : m_blockLength * 0.05,
						WallColor.at(territory != Territory::NEUTRAL ? Cast<Wall>(territory) : wall)
					);

				switch (structure) {
					case Structure::POND:
						TextureAsset(U"pond")
							.resized(m_blockLength * 0.65)
							.draw(Arg::center(center), ColorF{ 1, someExists ? 0.3 : 1.0 });
						break;
					case Structure::CASTLE:
						TextureAsset(U"castle")
							.resized(m_blockLength * 0.65)
							.draw(Arg::center(center), ColorF{ 1, someExists ? 0.3 : 1.0 });
						break;
					default:
						break;
				}

				if (masonExists) {
					drawMason(x, y);
				}


				if (actionExists) {
					drawAction(actionIndex, x, y);
				}
				else if (actionPlanExists) {
					//drawAction(actionPlan, x, y);
				}
				else if (m_legalMoves[y][x]) {
					drawAction(x, y);
				}
			}
		}

		const auto& centerY = Scene::Center().y;
		const auto& limit{ U"{:.2f}"_fmt(m_turnSeconds - m_stopwatch.sF()) };
		const auto& keyExplanations{ U"{}{}{}{}"_fmt(
			U"↑↓\t\t: 行動指定\n",
			U"1~6\t\t: 職人指定\n",
			U"テンキー\t: 方向指定\n",
			U"スペース\t: オートプレイ"
		) };

		FontAsset(U"20")(keyExplanations)
			.draw(Arg::center(135, 820), Palette::Black);
		FontAsset(U"25")(U"自分のスコア")
			.draw(Arg::center(140, centerY - 100), Palette::Black);
		FontAsset(U"35")(Format(m_scores[0]))
			.draw(Arg::center(140, centerY - 60), Palette::Black);
		FontAsset(U"25")(U"相手のスコア")
			.draw(Arg::center(1300, centerY - 100), Palette::Black);
		FontAsset(U"35")(Format(m_scores[1]))
			.draw(Arg::center(1300, centerY - 60), Palette::Black);
		FontAsset(U"25")(U"残り時間: {:0>5}"_fmt(limit))
			.draw(Arg::center(1300, 30), Palette::Black);
		FontAsset(U"25")(U"ターン: {:>{}}/{}"_fmt(Min(m_board->turn(), m_maxTurn), Format(m_maxTurn + 1).size(), m_maxTurn))
			.draw(Arg::center(140, 30), Palette::Black);
		if (not matchEnds()) {
			FontAsset(U"20")(U"{}のターンです"_fmt(m_myTurn ? U"自分" : U"相手"))
				.draw(Arg::center(140, 60), Palette::Black);
		}
	}
}
