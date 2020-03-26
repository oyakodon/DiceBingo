# DiceBingo

新入生歓迎会のときのビンゴプログラムです。

一晩で作ったので、ちょっと腑に落ちない実装もありますが、

意外とウケたので公開します。

![Screenshot](assets/diceroll.png)

## ダウンロード

- 下記リンクから適切なバージョンをダウンロードしてください (For Windows only)
  - <https://github.com/oyakodon/DiceBingo/releases>

## 基本的な操作方法

1. 「Bingo num:」画面
    - 矢印キー上下で出る数の最大値を変更
    - スペースキーで確定
2. 「Ready.」画面
    - スペースキーで開始
    - (一応使い方が下にも書いてあります)
3. サイコロが荒ぶるのは仕様です
4. 荒ぶっている状態で、スペースキーで10の位を表示、もう一度押すと1の位を表示
5. もう一度スペースキーを押すと、3. に戻る
6. 左上に「End.」と表示されたら、終わり

## 小ネタ

- 左上には今までに出した数を表示しています
- W, A, S, D, E, X, 矢印キーで自由に3D空間を移動できます
- [ キー, ] キーでこれまでに出た数字を確認することができます(Shift押しながらで早く移動)
- Ctrl + Rで最初からやり直し
- Yキーで？？？モードになり、もう一度押すと通常モードに戻ります
- settings.iniでビンゴ設定を保持してます
  - num: ビンゴの最大の出目 (int)
  - desc: ビンゴ中にキーの説明等を表示するか (true/false)

## 作った人

親子丼 ([github@oyakodon](https://github.com/oyakodon))

Twitter: [@oyakodon2109](https://twitter.com/oyakodon2109)

