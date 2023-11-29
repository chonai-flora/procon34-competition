#include "Fetch.hpp"

namespace procon34 {
	namespace Fetch {
		std::pair<HTTPResponse, JSON> Get(const URL& serverURL, const String& token, const FilePath& saveFileName) {
			const HashTable<String, String> headers{
				{ U"Content-Type", U"application/json" },
				{ U"procon-token", token },
			};
			const String saveFilePath{ U"log/" + saveFileName };

			const auto& response{ SimpleHTTP::Get(serverURL, headers, saveFilePath) };
			return { response, response.isOK() ? JSON::Load(saveFilePath) : JSON() };
		}

		std::pair<HTTPResponse, JSON> Post(const URL& serverURL, const String& token, const int32 turn, const Array<Action>& actions, const FilePath& saveFileName) {
			const HashTable<String, String> headers{
				{ U"Content-Type", U"application/json" },
				{ U"procon-token", token },
			};
			const String saveFilePath{ U"log/" + saveFileName };

			// 提出する `actions` は 0-indexed なのでスライスする
			const std::string data = JSON{
				{ U"turn", turn },
				{ U"actions", actions.slice(1).map([](Action action) {
					return JSON{
						{ U"type", Cast<int32>(action.type) },
						{ U"dir", action.dir },
					};
				})}
			}.formatUTF8();

			const auto& response{ SimpleHTTP::Post(serverURL, headers, data.data(), data.size(), saveFilePath) };
			return { response, response.isOK() ? JSON::Load(saveFilePath) : JSON() };
		}
	}
}
