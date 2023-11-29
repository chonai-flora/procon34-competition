#include "AssetLoad.hpp"

namespace procon34 {
	namespace AssetLoad {
		void Fonts() {
			for (auto& size : Range(5, 50, 5)) {
				FontAsset::Register(Format(size), size);
				FontAsset::Register(Format(size) + U" bold", size, Typeface::Bold);
			}
			FontAsset::Register(U"icon", FontMethod::MSDF, 50, Typeface::Icon_MaterialDesign);
		}

		void Textures() {
			TextureAsset::Register(U"pond", Resource(U"assets/pond.png"));
			TextureAsset::Register(U"castle", Resource(U"assets/castle.png"));
			TextureAsset::Register(U"procon-logo", Resource(U"assets/procon-logo.svg"));
		}
	}
}
