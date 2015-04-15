#GoZ-Metaseq

ZBrush <-> MetasequoiaをGoZでやりとりできるようにするメタセコ４向けプラグイン。GoBを移植したもの

SPECIAL THANKS! [Shunton (Original Blender Ver.)](http://www.zbrushcentral.com/member.php?287794-Stunton)



ダウンロード:

[1.0.4 Installer](https://github.com/devil-tamachan/goz-metaseq/releases)

インストール方法:

 - exeを実行してインストールしてください。
 - ZBrushを起動してMetaseq.exeの場所を設定します
 - 画面上部のメニュー"Preferences" -> GoZ -> "Path to Metaseq"でMetaseq.exeの場所を設定してください。
  * 64ビット版メタセコを使う場合は"C:\Program Files\tetraface\Metasequoia4\Metaseq.exe"
  * 32ビット版メタセコ+64ビットOSは"C:\Program Files (x86)\tetraface\Metasequoia4\Metaseq.exe"
  * 32ビット版メタセコ+32ビットOSは"C:\Program Files\tetraface\Metasequoia4\Metaseq.exe"

使用上の注意:

 - インポートは手動です。ZBrushで「GoZ」、もしくは「All」ボタンを押して出力した後、メタセコイアに戻り、「入力」ボタンを押してください。
 - マテリアルはエクスポートされません
 - 日本語は文字化けします。ZBrushの仕様なのでどうしようもないです

Tips:

 - 部品が分離した場合、SubtoolのAppendでまとめて表示できます。

Changelog:

 - 1.0.4 インポート/エクスポート時に軸がおかしいのを修正
 - 1.0.3 インポート、機能追加、バグ修正
  * 選択したものだけ出力、各種フリーズ (Special Thanks! まじかる☆しげぽん)
  * 多角形を含んでいるとZbrushがクラッシュするバグ修正（ポリグループの出力がおかしい）
 - 1.0.2 改行忘れ修正 (オブジェクトが複数だと読み込めないバグ) (Special Thanks! まじかる☆しげぽん)
 - 1.0.1 32ビット版が再コンパイルできてなくて、すぐウィンドウが消える感じになっていたのでちゃんと再コンパイル
 - 1.0.0 Exportのみ実装 (Metasequoia -> ZBrush)