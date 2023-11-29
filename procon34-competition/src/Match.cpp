#include "Match.hpp"

namespace procon34 {
	Match::Match(Optional<String> token)
		: m_token(token)
		, m_urlState()
		, m_turnsState()
		, m_turnSecListState()
		, m_startSecListState()
		, m_fieldListState()
		, m_launchServer(true) {
		m_urlState.text = U"http://localhost:3000";
		m_turnsState.text = U"30";

		for (const auto& sec : Range(3, 15)) {
			m_turnSecListState.items << Format(sec);
		}

		for (const auto& sec : { 2, 5, 10, 15, 20, 30 }) {
			m_startSecListState.items << Format(sec) + U"s";
		}

		const Array<FilePath> paths{ FileSystem::DirectoryContents(U"server\\json") };
		if (paths.size()) {
			m_fieldListState.items << U"ランダム";
			for (const auto& filename : paths) {
				m_fieldListState.items << filename.split(U'/').back().split(U'.').front();
			}
		}

		m_turnSecListState.scroll = 0;
		m_turnSecListState.selectedItemIndex.emplace(0);
		m_startSecListState.scroll = 2;
		m_startSecListState.selectedItemIndex.emplace(2);
		m_fieldListState.selectedItemIndex.emplace(0);
		m_launchServer = Cast<bool>(m_fieldListState.items.size());
	}

	String Match::token() const {
		return m_token.value_or(U"");
	}

	String Match::serverURL() const {
		return m_urlState.text;
	}

	bool Match::first() const {
		return m_first;
	}

	int32 Match::width() const {
		return m_width;
	}

	int32 Match::height() const {
		return m_height;
	}

	int32 Match::id() const {
		return m_id;
	}

	int32 Match::maxTurn() const {
		return m_maxTurn;
	}

	int32 Match::masonCount() const {
		return m_masonCount;
	}

	int32 Match::turnSeconds() const {
		return m_turnSeconds;
	}

	HashTable<Bonus, int32> Match::bonus() const {
		return m_bonus;
	}

	void Match::wait() {
		const auto& center{ Scene::Center() };
		const String dots{ U"{:.>{}}"_fmt(U"", Time::GetMillisec() / 360 % 3 + 1) };

		TextureAsset(U"procon-logo")
			.draw(Arg::center(center.movedBy(0, -200)), Palette::Black);
		FontAsset(U"50")(U"試合開始を待っています{: <3}"_fmt(dots))
			.draw(Arg::center(center), Palette::Black);

		if (isReady()) {
			const Array<String> messages{
				U"試合ID: {}"_fmt(m_id),
				U"先手: {}"_fmt(m_first ? U"自分" : U"相手"),
				U"後手: {}"_fmt(m_first ? U"相手" : U"自分"),
				U"秒数: {}秒"_fmt(m_turnSeconds),
				U"ターン数: {}"_fmt(m_maxTurn),
				U"フィールド: {}x{}"_fmt(m_width, m_height),
			};
			for (const auto& [index, message] : Indexed(messages)) {
				FontAsset(U"20")(message)
					.draw(Arg::center(center.movedBy(0, Cast<int32>(index + 2) * 40)), Palette::Black);
			}
		}
		else {
			FontAsset(U"20")(U"試合用URL").draw(center.movedBy(-300, 150), Palette::Black);
			SimpleGUI::TextBox(m_urlState, center.movedBy(-300, 180), 350);
			FontAsset(U"20")(U"ターン数").draw(center.movedBy(55, 150), Palette::Black);
			SimpleGUI::TextBox(m_turnsState, center.movedBy(55, 180), 145, 36, m_launchServer);
			FontAsset(U"20")(U"秒数").draw(center.movedBy(205, 150), Palette::Black);
			SimpleGUI::ListBox(m_turnSecListState, center.movedBy(205, 180), 95, 36, m_launchServer);
			FontAsset(U"20")(U"フィールド").draw(center.movedBy(-300, 220), Palette::Black);
			SimpleGUI::ListBox(m_fieldListState, center.movedBy(-300, 250), 350, 36, m_launchServer);
			FontAsset(U"20")(U"試合開始まで").draw(center.movedBy(55, 220), Palette::Black);
			SimpleGUI::ListBox(m_startSecListState, center.movedBy(55, 250), 245, 36, m_launchServer);
			SimpleGUI::CheckBox(m_launchServer, U"サーバーを起動する", center.movedBy(-308, 290));

			if (SimpleGUI::Button(U"先手", center.movedBy(-110, 80), 100)) {
				m_token.emplace(U"0");
			}
			if (SimpleGUI::Button(U"後手", center.movedBy(10, 80), 100)) {
				m_token.emplace(U"1");
			}
		}
	}

	void Match::fetch() {
		const auto& [response, matchesJson] = Fetch::Get(U"{}/matches"_fmt(m_urlState.text), *m_token, U"matches.json");
		const auto& matchJson{ matchesJson[U"matches"][0] };
		Console << U"------";
		Console << response.getStatusLine().rtrimmed();
		Console << U"status code: " << FromEnum(response.getStatusCode());
		Console << U"------";
		Console << response.getHeader().rtrimmed();
		Console << U"------\n";
		if (response.isOK()) {
			m_first = matchJson[U"first"].get<bool>();
			m_id = matchJson[U"id"].get<int32>();
			m_width = matchJson[U"board"][U"width"].get<int32>();
			m_height = matchJson[U"board"][U"height"].get<int32>();
			m_maxTurn = matchJson[U"turns"].get<int32>();
			m_masonCount = matchJson[U"board"][U"mason"].get<int32>();
			m_turnSeconds = matchJson[U"turnSeconds"].get<int32>();
			m_bonus = {
				{ Bonus::WALL, matchJson[U"bonus"][U"wall"].get<int32>() },
				{ Bonus::TERRITORY, matchJson[U"bonus"][U"territory"].get<int32>() },
				{ Bonus::CASTLE, matchJson[U"bonus"][U"castle"].get<int32>() },
			};
		}
		else {
			throw Error{ U"サーバーからJSONファイルを取得することができませんでした。" };
		}
	}

	void Match::updateJson() {
		const FilePath filename{ U"server\\json\\{}"_fmt(getJsonName()) };
		JSON json{ JSON::Load(filename) };
		if (not json) {
			throw Error{ U"`{}` の読み込みに失敗しました。"_fmt(filename) };
		}

		const String turnSec{ m_turnSecListState.items.at(m_turnSecListState.selectedItemIndex.value()) };
		json[U"match"][U"turns"] = Parse<int32>(m_turnsState.text);
		json[U"match"][U"turnSeconds"] = Parse<int32>(turnSec);

		// BOMなしJSONの書き出し
		// Visual Studio側の設定で調整できるか検討中
		TextWriter writer{ filename, TextEncoding::UTF8_NO_BOM };
		writer.write(json.format());
	}

	String Match::getJsonName() {
		auto& fieldIndex = m_fieldListState.selectedItemIndex;
		if (fieldIndex.value() == 0) {
			fieldIndex.emplace(Random(1, Cast<int32>(m_fieldListState.items.size()) - 1));
		}
		const String filename{ m_fieldListState.items.at(fieldIndex.value()) };

		return filename + U".json";
	}

	const char* Match::getServerCommand() {
		const String filename{ getJsonName() };
		const String server{ U"n-jo-server.exe" };
		const String startSec{ m_startSecListState.items.at(m_startSecListState.selectedItemIndex.value()) };
		const String port{ m_urlState.text.split(U':').back() };
		const String command{ U"server\\{} -c server\\json\\{} -start {} -l :{}"_fmt(server, filename, startSec, port) };

		return command.narrow().c_str();
	}

	bool Match::isReady() const {
		return m_token.has_value();
	}

	bool Match::launchServer() const {
		return m_launchServer;
	}
}
