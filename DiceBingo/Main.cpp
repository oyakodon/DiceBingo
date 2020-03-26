# include <Siv3D.hpp>

// サイコロのテクスチャを作成
Image CreateDiceImage(int32 resolution = 128)
{
	Image image(resolution * 3, resolution * 2, Palette::White);

	const double r = resolution / 100.0;

	// [1]
	Circle(r * 50, r * 50, r * 18).write(image, Palette::Red);

	// [2]
	Circle(r * 122, r * 22, r * 11).write(image, Palette::Black);
	Circle(r * 178, r * 78, r * 11).write(image, Palette::Black);

	// [3]
	Circle(r * 278, r * 22, r * 11).write(image, Palette::Black);
	Circle(r * 250, r * 50, r * 11).write(image, Palette::Black);
	Circle(r * 222, r * 78, r * 11).write(image, Palette::Black);

	// [4]
	Circle(r * 122, r * 122, r * 11).write(image, Palette::Black);
	Circle(r * 122, r * 178, r * 11).write(image, Palette::Black);
	Circle(r * 178, r * 122, r * 11).write(image, Palette::Black);
	Circle(r * 178, r * 178, r * 11).write(image, Palette::Black);

	// [5]
	Circle(r * 22, r * 122, r * 11).write(image, Palette::Black);
	Circle(r * 22, r * 178, r * 11).write(image, Palette::Black);
	Circle(r * 50, r * 150, r * 11).write(image, Palette::Black);
	Circle(r * 78, r * 122, r * 11).write(image, Palette::Black);
	Circle(r * 78, r * 178, r * 11).write(image, Palette::Black);

	// [6]
	Circle(r * 227, r * 122, r * 11).write(image, Palette::Black);
	Circle(r * 227, r * 150, r * 11).write(image, Palette::Black);
	Circle(r * 227, r * 178, r * 11).write(image, Palette::Black);
	Circle(r * 273, r * 122, r * 11).write(image, Palette::Black);
	Circle(r * 273, r * 150, r * 11).write(image, Palette::Black);
	Circle(r * 273, r * 178, r * 11).write(image, Palette::Black);

	return image.fit(resolution, resolution);
}

// 7セグのセグメント
int ssegVertex[][4] =
{
	{ 1, 2, 3, 4 },{ 11, 17, 23, 29 },{ 41, 47, 53, 59 },
{ 61, 62, 63, 64 },{ 36, 42, 48, 54 },{ 6, 12, 18, 24 },
{ 31, 32, 33, 34 }
};

// 10進 -> 7セグのON/OFF
int ssegDecodeTable[][7] =
{
	{ 1, 1, 1, 1, 1, 1, 0 },{ 0, 1, 1, 0, 0, 0, 0 },
{ 1, 1, 0, 1, 1, 0, 1 },{ 1, 1, 1, 1, 0, 0, 1 },
{ 0, 1, 1, 0, 0, 1, 1 },{ 1, 0, 1, 1, 0, 1, 1 },
{ 1, 0, 1, 1, 1, 1, 1 },{ 1, 1, 1, 0, 0, 0, 0 },
{ 1, 1, 1, 1, 1, 1, 1 },{ 1, 1, 1, 1, 0, 1, 1 }
};

// 7セグのサイコロ面をランダムにするためのテーブル
Array<Vec3> randomFaceTable(66);

/// <summary>
/// 7セグ的に並べたサイコロを表示
/// </summary>
/// <param name="n">数字</param>
/// <param name="shift">描画をどれだけずらすか (原点は左上)</param>
/// <param name="mesh">meshDice</param>
/// <param name="tex">textureDice</param>
void drawDiceN(int n, const Vec2& shift, const Mesh& mesh, const Texture& tex)
{
	for (int i = 0; i < 7; i++)
	{
		if (ssegDecodeTable[n][i])
		{
			for (int k = 0; k < 4; k++)
			{
				int v = ssegVertex[i][k];
				mesh.rotated(
					Floor(randomFaceTable[v].x) * Pi / 2.0,
					Ceil(randomFaceTable[v].y) * Pi / 2.0,
					Floor(randomFaceTable[v].z) * Pi / 2.0)
					.translated(
						v % 6 + shift.x,
						-v / 6 + shift.y,
						10)
					.draw(tex);
			}
		}
	}
}

// const int BINGO_NUM = 72;

void Main()
{
	const Font font_M(32, L"Consolas", FontStyle::Regular);
	const Font font_S(16, L"Consolas", FontStyle::Regular);

	Graphics3D::SetAmbientLight(ColorF(0.3));
	const Texture textureDice(CreateDiceImage(256), TextureDesc::For3D);
	const Texture textureYosano(Image(L"yosano.png"), TextureDesc::For3D);
	const Mesh meshDice(MeshData::Box6(1.0));

	// フルスクリーン化
	const Array<Size> resolutions = Graphics::GetFullScreenSize();
	Window::SetFullscreen(true, resolutions[resolutions.size() - 1]);

	bool isNumConfirmed = false;
	bool isReady = false;
	bool modeYosano = false;	// 与謝野モード
	int state = 0;	// 0: ダイスロール, 1: 10の位, 2: 1の位
	int num = 0;	// ビンゴの数字
	int cur = 0;	// カーソル
	Array<int> bingoNumbers;	// ビンゴの数字リスト (使われたらerase)
	Array<int> hitNumbers;		// 使われた数字リスト

	int32 BINGO_NUM = 50;
	bool showDescription = false;
	const INIReader ini(L"settings.ini");
	if (ini)
	{
		BINGO_NUM = ini.getOr<int32>(L"Bingo.num", 50);
		showDescription = ini.getOr<bool>(L"Bingo.desc", false);
	}

	while (System::Update())
	{
		if (!isNumConfirmed)
		{
			font_M(L"Bingo num: ", BINGO_NUM).draw();
			font_S(L"<Up> to increase bingo num.").draw({ 0, 80 });
			font_S(L"<Down> to decrease bingo num.").draw({ 0, 110 });
			font_S(L"<Space> to confirm bingo num").draw({ 0, 140 });

			if (Input::KeyUp.clicked)
			{
				BINGO_NUM = Min(INT32_MAX, BINGO_NUM + 1);
			}
			if (Input::KeyUp.pressed && Input::KeyUp.pressedDuration > 1500)
			{
				if (System::FrameCount() % 6 == 0)
				{
					BINGO_NUM = Min(INT32_MAX, BINGO_NUM + 1);
				}
			}

			if (Input::KeyDown.clicked)
			{
				BINGO_NUM = Max(1, BINGO_NUM - 1);
			}
			if (Input::KeyDown.pressed && Input::KeyDown.pressedDuration > 1500)
			{
				if (System::FrameCount() % 6 == 0)
				{
					BINGO_NUM = Max(1, BINGO_NUM - 1);
				}
			}

			if (Input::KeySpace.clicked)
			{
				isNumConfirmed = true;
				INIWriter writer(L"settings.ini");
				writer.write(L"Bingo", L"num", BINGO_NUM);
				writer.write(L"Bingo", L"desc", showDescription);
			}

			continue;
		}

		if (!isReady)
		{
			font_M(L"Ready.").draw();
			font_S(textureYosano ? L"(yosano loaded.)" : L"(Warning: failed to load \"yosano.png\".)").draw({ 0, 100 });

			int32 y = 200;
			font_S(L"Usage:").draw({ 0, y }); y += 50;
			font_S(L"<Space> to roll dice or select number.").draw({ 0, y }); y += 30;
			font_S(L"<Y> to toggle yosano mode.").draw({ 0, y }); y += 30;
			font_S(L"<[,]> to show previous number. (number history)").draw({ 0, y }); y += 30;
			font_S(L"<Ctrl + R> to reset.").draw({ 0, y });

			if (Input::KeySpace.clicked)
			{
				isReady = true;

				for (int i = 1; i <= BINGO_NUM; i++)
				{
					bingoNumbers.push_back(i);
				}
			}

			continue;
		}

		Graphics3D::FreeCamera();

		if (bingoNumbers.size() == 0)
		{
			font_M(L"End.").draw();
		}
		else
		{
			font_M((BINGO_NUM - bingoNumbers.size() + cur), L" / ", BINGO_NUM).draw();
		}

		if (Input::KeySpace.clicked)
		{
			if (cur == 0)
			{
				if (state == 0)
				{
					// ランダムに数字を選択
					int n = RandomSelect(bingoNumbers.begin(), bingoNumbers.end());

					// 削除
					auto it = std::find(bingoNumbers.begin(), bingoNumbers.end(), n);
					if (it != bingoNumbers.end())
					{
						bingoNumbers.erase(it);
					}

					// hitNumbersに追加
					hitNumbers.push_back(n);

					// 7セグのサイコロ面をランダムに再生成
					for (int i = 0; i < 66; i++)
					{
						randomFaceTable[i] = RandomVec3(3, 3, 3);
					}
				}

				if (bingoNumbers.size() > 0 || state < 2)
				{
					state++;
					state %= 3;
				}
			}
		}

		if (Input::KeyY.clicked)
		{
			// 与謝野モード：トグル
			modeYosano = !modeYosano;
		}

		if (Input::KeyControl.pressed && Input::KeyR.clicked)
		{
			// リセット
			cur = state = num = 0;
			isReady = false;

			bingoNumbers.clear();
			hitNumbers.clear();

			continue;
		}

		num = hitNumbers.size() > 0 ? hitNumbers[hitNumbers.size() + cur - 1] : 0;

		if (state == 2)
		{
			// 過去の記録を見る
			if (Input::KeyLBracket.clicked || (Input::KeyShift.pressed && Input::KeyLBracket.pressed))
			{
				cur = Max(-(int)hitNumbers.size() + 1, cur - 1);
			}

			if (Input::KeyRBracket.clicked || (Input::KeyShift.pressed && Input::KeyRBracket.pressed))
			{
				cur = Min(0, cur + 1);
			}

			// 1の位
			drawDiceN(num % 10, Vec2(2, 5), meshDice, modeYosano ? textureYosano : textureDice);
			// 10の位
			drawDiceN(num / 10, Vec2(-8, 5), meshDice, modeYosano ? textureYosano : textureDice);

			if (showDescription)
			{
				font_S(L"press <Space> to roll dice.").drawCenter({ Window::Center().x, Window::Height() - 100 });
			}
		}
		else if (state == 1)
		{
			// 10の位
			drawDiceN(num / 10, Vec2(-8, 5), meshDice, modeYosano ? textureYosano : textureDice);

			if (showDescription)
			{
				font_S(L"press <Space> to select first digit.").drawCenter({ Window::Center().x, Window::Height() - 100 });
			}
		}
		else
		{
			// クソデカダイスロール
			int fc = System::FrameCount();
			meshDice.scaled(5)
				.rotated(
					fc / 4.0,
					fc / 8.0,
					fc / 12.0)
				.translated(0, 2, 0)
				.draw(modeYosano ? textureYosano : textureDice);

			if (showDescription && bingoNumbers.size() > 0)
			{
				font_S(L"press <Space> to select num.").drawCenter({ Window::Center().x, Window::Height() - 100 });
			}
		}

	}

}
