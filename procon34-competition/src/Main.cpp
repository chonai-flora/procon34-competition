#include "procon34.hpp"
#include "GUI.hpp"
#include "Fetch.hpp"
#include "Match.hpp"
#include "Board.hpp"
#include "AssetLoad.hpp"

void Main() {
	Window::SetTitle(U"決戦！n乗谷城");
	Window::Resize(procon34::WindowSize);
	Scene::SetBackground(Palette::Snow);
	Console.open();

	procon34::AssetLoad::Fonts();
	procon34::AssetLoad::Textures();

	procon34::Match match;
	Optional<procon34::GUI> gui;

	AsyncTask<int> task;
	while (true) {
		if (System::Update()) {
			if (gui.has_value()) {
				if (gui->isReady()) {
					if (gui->matchEnds()) {
						gui->stop();
					}
					else {
						gui->update();
					}

					gui->draw();
				}
				else {
					match.wait();
					gui->fetchBoard();
				}
			}
			else {
				match.wait();
				if (match.isReady()) {
					if (match.launchServer()) {
						match.updateJson();

						const char* command = match.getServerCommand();
						task = Async(std::system, command);
						System::Sleep(0.001s);
					}

					match.fetch();
					gui.emplace(procon34::GUI(match));
				}
			}
		}
		else {
			if (match.launchServer()) {
				const char* command = "taskkill /im n-jo-server.exe /f";
				task = Async(std::system, command);
			}
			break;
		}
	}

	if (task.isValid()) {
		task.wait();
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
