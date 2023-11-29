#include "procon34.hpp"

namespace procon34 {
	namespace Fetch {
		std::pair<HTTPResponse, JSON> Get(const URL& serverURL, const String& token, const FilePath& saveFileName);
		std::pair<HTTPResponse, JSON> Post(const URL& serverURL, const String& token, const int32 turn, const Array<Action>& actions, const FilePath& saveFileName);
	}
}
