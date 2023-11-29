# procon34-competition
[全国高等専門学校プログラミングコンテスト第34回](https://www.procon.gr.jp/wp-content/uploads//2023/04/4d890277c1072170185a2709f89342d4.pdf)競技部門のテーマ「決戦！ｎ乗谷城」のプログラムです。

公式の簡易サーバーをそのまま使用しています([SERVER-CREDITS.txt](SERVER-CREDITS.txt))。GUIライブラリに[OpenSiv3D](LICENSE-SIV3D)を使用しています。

## セットアップ
詳細: https://siv3d.github.io/ja-jp/download/windows/
### 1. Visual Studioのセットアップ
[Visual Studioダウンロードページ](https://visualstudio.microsoft.com/ja/downloads/) から「Visual Studio 2022 コミュニティ」のインストーラをダウンロードし実行する。  
インストーラを実行すると、プログラミング言語や開発ツールを選択する画面が出てくる。この中から「C++ によるデスクトップ開発」を選択する。  
そのまま右下の 「インストール」 ボタンを押し、必要なツールのインストールを始める。

### 2. OpenSiv3Dのセットアップ
[OpenSiv3D v0.6.11 Installer for Windows Desktop](https://siv3d.jp/downloads/Siv3D/OpenSiv3D_0.6.11_Installer.exe) をダウンロードして実行する。
実行時に「Windows によって PC が保護されました」と表示された場合は、詳細情報を押して実行を押す。

### 3. 実行
`procon34-competition.sln` を実行する。

## 注意
Releaseビルドで実行してください。