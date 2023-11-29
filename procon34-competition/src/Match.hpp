#pragma once
#include "procon34.hpp"
#include "Board.hpp"
#include "Fetch.hpp"

namespace procon34 {
	class Match {
	public:
		Match(Optional<String> token = none);
		String token() const;
		String serverURL() const;
		bool first() const;
		int32 id() const;
		int32 width() const;
		int32 height() const;
		int32 maxTurn() const;
		int32 masonCount() const;
		int32 turnSeconds() const;
		HashTable<Bonus, int32> bonus() const;

		void wait();
		void fetch();
		void updateJson();
		String getJsonName();
		const char* getServerCommand();
		bool isReady() const;
		bool launchServer() const;

	private:
		Optional<String> m_token;
		TextEditState m_urlState;
		TextEditState m_turnsState;
		ListBoxState m_turnSecListState;
		ListBoxState m_startSecListState;
		ListBoxState m_fieldListState;
		bool m_launchServer;
		bool m_first;
		int32 m_id;
		int32 m_width;
		int32 m_height;
		int32 m_maxTurn;
		int32 m_masonCount;
		int32 m_turnSeconds;
		HashTable<Bonus, int32> m_bonus;
	};
}
