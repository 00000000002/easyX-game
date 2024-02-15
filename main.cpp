#include <graphics.h>
#include <string>
#include <vector>
int idx_current_anim = 0;
const int PLAYER_ANIM_NUM = 6;
IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];



const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")
inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}
class Animation {
public:
	Animation(LPCTSTR path, int num, int interval) {
		interval_ms = interval;
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++) {
			_stprintf_s(path_file, path, i);
			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}
	~Animation() {
		for (size_t i = 0; i < frame_list.size(); i++) {
			delete frame_list[i];
		}

	}
	void play(int x, int y, int delta) {
		timer += delta;
		if (timer >= interval_ms) {
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, frame_list[idx_frame]);

	}



private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;
};

class Bullet {
public:
	POINT position = { 0,0 };
public:
	Bullet() = default;
	~Bullet() = default;
	void Draw() const {
		setlinecolor(RGB(255, 155, 50));
		setlinecolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}
private:
	const int RADIUS = 10;
};

class Player {
public:
	Player() {
		loadimage(&img_shadow, _T("img/shadow_player/png"));
		anim_left=new Animation(_T("img/player_left_%d.png"), 6, 45);
		anim_right=new Animation(_T("img/player_right_%d.png"), 6, 45);
	}
	~Player() {
		delete anim_left;
		delete anim_right;
	}
	void ProcessEvent(const ExMessage& msg) {
		switch(msg.message)
		{case WM_KEYDOWN:
			switch (msg.vkcode) {
			case VK_UP:
				is_moveup = true;
				//player_pos.y -= PLAYER_SPEED;
				break;
			case VK_DOWN:
				is_movedown = true;
				//player_pos.y += PLAYER_SPEED;
				break;
			case VK_LEFT:
				is_moveleft = true;
				//player_pos.x -= PLAYER_SPEED;
				break;
			case VK_RIGHT:
				is_moveright = true;
				//player_pos.x += PLAYER_SPEED;
				break;
			}
			break;
		
		case WM_KEYUP: {
			switch (msg.vkcode) {
			case VK_UP:
				is_moveup = false;
				//player_pos.y -= PLAYER_SPEED;
				break;
			case VK_DOWN:
				is_movedown = false;
				//player_pos.y += PLAYER_SPEED;
				break;
			case VK_LEFT:
				is_moveleft = false;
				//player_pos.x -= PLAYER_SPEED;
				break;
			case VK_RIGHT:
				is_moveright = false;
				//player_pos.x += PLAYER_SPEED;
				break;
			}
			break;
		}
	}
	}
	void Move() {
		int dir_x = is_moveright - is_moveleft;
		int dir_y = is_movedown - is_moveup;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double normalize_x = dir_x / len_dir;
			double normalize_y = dir_y / len_dir;
			player_pos.x += (int)(PLAYER_SPEED * normalize_x);
			player_pos.y += (int)(PLAYER_SPEED * normalize_y);
		}
		if (player_pos.x < 0)player_pos.x = 0;
		if (player_pos.y < 0)player_pos.y = 0;
		if (player_pos.x + PLAYER_WIDTH > WINDOW_WIDTH)player_pos.x = WINDOW_WIDTH - PLAYER_WIDTH;
		if (player_pos.y + PLAYER_HEIGHT > WINDOW_HEIGHT)player_pos.y = WINDOW_HEIGHT - PLAYER_HEIGHT;
	}
	void Draw(int delta) {
		int pos_shadow_x = player_pos.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = player_pos.y + PLAYER_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		static bool facing_left = false;
		int dir_x = is_moveright - is_moveleft;
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

		if (facing_left)
			anim_left->play(player_pos.x, player_pos.y, delta);
		else
			anim_right->play(player_pos.x, player_pos.y, delta);
	}
	const POINT& GetPosition() const {
		return player_pos;
	}
private:
	const float PLAYER_SPEED = 5;
	const int PLAYER_WIDTH = 80;
	const int PLAYER_HEIGHT = 80;
	const int SHADOW_WIDTH = 32;
private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT player_pos = { 500,500 };
	bool is_moveup = false;
	bool is_movedown = false;
	bool is_moveleft = false;
	bool is_moveright = false;

};




void LoadAnimation() {
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++) {
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());

	}
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++) {
		std::wstring path = L"img/player_right_"+ std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
	}
}






class Enemy {
public:
	Enemy() {
		loadimage(&img_shadow, _T("img/shadow_player/png"));
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);
		enum class SpawnEdge
		{
			up = 0,
			Down,
			Left,
			Right

		};
		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge) {
		case SpawnEdge::up:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -FRAME_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		default:
			break;
		}

	}
	bool CheckBulletCollision(const Bullet& bullet) {
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}
	bool CheckPlayerCollision(const Player& player) {
		POINT check_point = { position.x + FRAME_WIDTH / 2,position.y + FRAME_HEIGHT / 2 };
		return false;
	}
	void Move(const Player& player) {
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double normalize_x = dir_x / len_dir;
			double normalize_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalize_x);
			position.y += (int)(SPEED * normalize_y);
		}
	}
	void draw(int delta) {
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + FRAME_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		static bool facing_left = false;


		if (facing_left)
			anim_left->play(position.x, position.y, delta);
		else
			anim_right->play(position.x, position.y, delta);
	}
	//const POINT& GetPosition() const {
	//	return position;
	//}

	~Enemy() {
		delete anim_left;
		delete anim_right;
	}
	void Hurt() {
		alive = false;
	}
	bool CheckAlive() {
		return alive;

	}
private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;
private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0,0 };
	bool facing_left = false;
	bool alive = true;

};


void DrawPlayer(int delta,int dir_x) {

	
}

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list) {
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy());
	}
}


void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player) {
	const double PADSP = 0.0045;
	const double TANSP = 0.0055;
	double radian_interval = 2 * 3.14 / bullet_list.size();
	POINT player_position = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * PADSP);
	for (size_t i = 0; i < bullet_list.size(); i++) {
		double radian = GetTickCount() * TANSP + radian_interval * i;
	
		bullet_list[i].position.x = player_position.x + 40 + (int)(radius * sin(radian));
		bullet_list[i].position.y = player_position.y + 40 + int(radius * cos(radian));

	}

}

void DrawPlayerScore(int score) {
	static TCHAR text[64];
	_stprintf_s(text, _T("dddd:%d"), score);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}


int main() {

	initgraph(1280, 720);
	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/bgm.mp3 alias hit"), NULL, 0, NULL);
	mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	bool runing = true;
	int score = 0;
	Player player;
	ExMessage msg;
	IMAGE img_background;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet>bullet_list(3);
	loadimage(&img_background, _T("img/background.png"));
	//loadimage(&img_shadow, _T("img/shadow_player.png"));

	BeginBatchDraw();
	while (runing)
	{
		
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg)) {
			player.ProcessEvent(msg);
		}
		player.Move();
		UpdateBullets(bullet_list, player);
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
			enemy->Move(player);
		for (Enemy* enemy : enemy_list) {
			if (enemy->CheckPlayerCollision(player)) {
				MessageBox(GetHWnd(), _T("..."), _T("go"), MB_OK);
				runing = false;
				break;
			}
		}
		for (Enemy* enemy : enemy_list) {
			for (const Bullet& bullet : bullet_list) {
				if (enemy->CheckBulletCollision(bullet)) {
					mciSendString(_T("play hit repeat from 0"), NULL, 0, NULL);
					enemy->Hurt();
					score++;
				}
			}
		}
		for (size_t i = 0; i < enemy_list.size(); i++) {
			Enemy* enemy = enemy_list[i];
			if (!enemy->CheckAlive()) {
				std::swap(enemy_list[i], enemy_list.back());
				enemy_list.pop_back();
				delete enemy;
			}
		}
		//	LoadAnimation();
		//	
		//if (is_movedown) player_pos.y += PLAYER_SPEED;
		//if (is_moveup)player_pos.y -= PLAYER_SPEED;
		//if (is_moveleft)player_pos.x -= PLAYER_SPEED;
		//if (is_moveright)player_pos.x += PLAYER_SPEED;


		//static int counter = 0;
		//if (++counter % 5 == 0) {
		//	idx_current_anim++;
		//}
		//idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM;

		cleardevice();
		putimage(0, 0, &img_background);
		player.Draw(1000 / 144);
		for (Enemy* enmey : enemy_list)
			enmey->draw(1000 / 144);
		for (const Bullet& bullet : bullet_list)
			bullet.Draw();
		DrawPlayerScore(score);
		/*DrawPlayer(1000/144, is_moveright-is_moveleft);*/
		/*putimage_alpha(player_pos.x, player_pos.y, &img_player_left[idx_current_anim]);*/
		FlushBatchDraw();
		DWORD  end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144) {
			Sleep(1000 / 144 - delta_time);
		}

	}
	EndBatchDraw();
	return 0;
}
