#include <iostream>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

const int frame_rate = 50;
const int INF = 1e9;

struct Fire
{
    float x;
    float y;
};

struct Potion
{
    float x;
    float y;
};

class Object {
public:

    string move(float x, float y) {
        return "move " + this->get_name() + " " + to_string(x) + " " + to_string(y);
    }

    string shoot(float x, float y) {
        return "shoot " + this->get_name() + " " + to_string(x) + " " + to_string(y);
    }

    // index: 技能id
    // args: 技能参数
    virtual string skill(int index, vector<string> args) = 0;
    
    // 获取角色名称
    virtual string get_name() = 0;
    void set_pos(float x, float y) {
        pos.first = x;
        pos.second = y;
    }

    // 更新位置
    virtual void set_pos(pair<float, float> pos) {
        this->pos = pos;
    }

    // 获取位置
    virtual pair<float, float> get_pos() {
        return pos;
    }
    
    // 更新hp
    void set_hp(float hp) {
        this->hp = hp;
    }

    // 获取hp
    float get_hp() {
        return hp;
    }
    // 设置阵营
    void set_type(int type) {
        this->type = type;
    }

    // 获取阵营
    int get_type() {
        return type;
    }

public:
    pair<float, float> pos;
    float hp;
    int type; // 0: A队物体, 1: B队物体
};

class Gunner: public Object {
public:
    Gunner() {
        hp = 50;
        speed = 5;
        range = 20;
        attack = 5;
        defense = 4;
        _CD = {20 * frame_rate, 30 * frame_rate};
        _effect = {5 * frame_rate, 5 * frame_rate};
        CD = {0, 0};
        effect = {0, 0};
    }
    virtual string get_name() {
        return "Gunner";
    }

    // index = 1 加速
    // index = 2 射速
    virtual string skill(int index, vector<string> args) {
        string skill_str = "skill " + this->get_name();
        switch (index) {
            case 1:
                skill_str += " 1";
                break;
            case 2:
                skill_str += " 2";
                break;
            default:
                break;
        }
        return skill_str;
    }
public:
    // 移动速度
    float speed;
    // 射程
    float range;
    // 攻击力
    float attack;
    // 防御力
    float defense;
    // 技能 CD
    vector<int> _CD;
    // 技能现在 CD
    vector<int> CD;
    // 技能效果现在持续时间
    vector<int> effect;
    // 技能效果持续时间
    // 地雷设置成无限大
    vector<int> _effect;
};

class Hurler: public Object {
public:
    Hurler() {
        hp = 50;
        speed = 5;
        range = 20;
        attack = 8;
        defense = 6;
        _CD = {25 * frame_rate, 0};
        _effect = {3 * frame_rate, INF};
        CD = {0, 0};
        effect = {0, 0};
    }
    virtual string get_name() {
        return "Hurler";
    }

    // index = 1 燃烧
    // index = 2 隐形
    virtual string skill(int index, vector<string> args) {
        string skill_str = "skill " + this->get_name();
        switch (index) {
            case 1:
                skill_str += " 1";
                break;
            case 2:
                skill_str += " 2";
                break;
            default:
                break;
        }
        return skill_str;
    }
public:
    // 移动速度
    float speed;
    // 射程
    float range;
    // 攻击力
    float attack;
    // 防御力
    float defense;
    // 技能 CD
    vector<int> _CD;
    // 技能现在 CD
    vector<int> CD;
    // 技能效果现在持续时间
    vector<int> effect;
    // 技能效果持续时间
    // 地雷设置成无限大
    vector<int> _effect;
};

class Medic: public Object {
public:
    Medic() {
        hp = 50;
        speed = 5;
        range = 20;
        attack = 5;
        defense = 6;
        _CD = {25 * frame_rate, 60 * frame_rate};
        _effect = {3 * frame_rate, INF};
        CD = {0, 0};
        effect = {0, 0};
    }
    virtual string get_name() {
        return "Medic";
    }

    // index = 1 治疗
    // index = 2 盾
    virtual string skill(int index, vector<string> args) {
        string skill_str = "skill " + this->get_name();
        switch (index) {
            case 1:
                skill_str += " 1";
                break;
            case 2:
                skill_str += " 2";
                break;
            default:
                break;
        }
        return skill_str;
    }
public:
    // 移动速度
    float speed;
    // 射程
    float range;
    // 攻击力
    float attack;
    // 防御力
    float defense;
    // 技能 CD
    vector<int> _CD;
    // 技能现在 CD
    vector<int> CD;
    // 技能效果现在持续时间
    vector<int> effect;
    // 技能效果持续时间
    // 地雷设置成无限大
    vector<int> _effect;
};

vector<vector<int>> game_map; //游戏地图
// pair<float,float> GunnerA_Pos; //GunnerA位置
// pair<float,float> HurlerA_Pos; //HurlerA位置
// pair<float,float> MedicA_Pos; //MedicA位置
// pair<float,float> GunnerB_Pos; //GunnerB位置
// pair<float,float> HurlerB_Pos; //HurlerB位置
// pair<float,float> MedicB_Pos; //MedicB位置
// float GunnerA_HP; //GunnerA血量
// float HurlerA_HP; //HurlerA血量
// float MedicA_HP;  //MedicA血量
// float GunnerB_HP; //GunnerB血量
// float HurlerB_HP; //HurlerB血量
// float MedicB_HP;  //MedicB血量

Gunner GunnerA, GunnerB;
Hurler HurlerA, HurlerB;
Medic MedicA, MedicB;


bool has_coin; //金币存在信息
pair<float,float> coin_Pos; //金币位置信息
int scoreA; //A阵营得分
int scoreB; //B阵营得分
vector<Fire> fire_pos; //场上火焰的位置
vector<Potion> potion_pos; //场上药水的位置

int frame_count; //帧数信息

void init_input()
{
    string init_str;
    cin >> init_str;

    int n,m;
    cin >> n >> m;
    for (int i = 0; i < n; i++)
    {
        vector<int> temp(m,0);
        for (int j = 0; j < m; j++)
        {
            cin >> temp[j];
        }
        game_map.push_back(temp);
    }
    
    cin >> init_str;
}

void soldier_input()
{
    string pos;
    cin >> pos;
    int count;
    cin >> count;
    for (int i = 0; i < count; i++)
    {
        string team;
        string type;
        float hp;
        float x;
        float y;
        cin >> pos >> team >> pos >> type >> pos >> hp >> pos >> x >> pos >> y;
        if (team == "A")
        {
            if (type == "GunnerA")
            {
                GunnerA.set_hp(hp);
                GunnerA.set_pos(x, y);
            }else if (type == "HurlerA")
            {
                HurlerA.set_hp(hp);
                HurlerA.set_pos(x, y);
            }else if (type == "MedicA")
            {
                MedicA.set_hp(hp);
                MedicA.set_pos(x, y);
            }
        }else if (team == "B")
        {
            if (type == "GunnerB")
            {
                GunnerB.set_hp(hp);
                GunnerB.set_pos(x, y);
            }else if (type == "HurlerB")
            {
                HurlerB.set_hp(hp);
                HurlerB.set_pos(x, y);
            }else if (type == "MedicB")
            {
                MedicB.set_hp(hp);
                MedicB.set_pos(x, y);
            }
        }
    }
    cin >> pos;
}

void fire_input()
{
    fire_pos.clear();
    string pos;
    cin >> pos;
    int count;
    cin >> count;
    
    for (int i =0; i < count; i++)
    {
        Fire fire;
        float x;
        float y;
        cin >> pos >> x >> pos >> y;
        fire.x = x;
        fire.y = y;
        fire_pos.push_back(fire);
    }

    cin >> pos;
}

void score_input()
{
    string pos;
    cin >> pos;

    int A;
    int B;
    cin >> pos >> A >> pos >> B;
    scoreA = A;
    scoreB = B;

    cin >> pos;
}

void coin_input()
{
    string pos;
    cin >> pos;
    
    string temp_has_coin;
    cin >> temp_has_coin;
    
    if (temp_has_coin == "true")
    {
        float x;
        float y;
        cin >> pos >> x >> pos >> y;
        coin_Pos.first = x;
        coin_Pos.second = y;
        has_coin = true;
    }else if (temp_has_coin == "false")
    {
        has_coin = false;
        cout << "Dont have coin!!!" << endl;
    }
    
    cin >> pos;
}

void potion_input()
{
    potion_pos.clear();
    string pos;
    cin >> pos;

    int count;
    cin >> count;
    for (int i = 0; i < count; i++)
    {
        Potion potion;
        float x;
        float y;
        cin >> pos >> x >> pos >> y;
        potion.x = x;
        potion.y = y;
        potion_pos.push_back(potion);
    }

    cin >> pos;
}

void per_frame_input()
{
    string frame;
    cin >> frame >> frame_count;

    soldier_input();
    fire_input();
    score_input();
    coin_input();
    potion_input();

    cin >> frame >> frame_count;
}

void output_command() //请不要删除End一行，每行输出记得加上换行符
{
    //示例代码
    // if (frame_count < 1000)
    // {
    //     cout << "move gunner 5 16.5" << endl;
    // }else if (frame_count < 2000)
    // {
    //     cout << "move gunner 15 16.5" << endl;
    // }else
    // {
    //     cout << "shoot gunner " << GunnerA_Pos.first + 1 << " " << GunnerA_Pos.second << endl;
    // }
    // cout << "skill hurler 1 10 10" << endl;
    // cout << "skill hurler 2" << endl;
    // cout << "skill gunner 1" << endl;
    // cout << "skill gunner 2" << endl;
    // cout << "skill medic 1 3.5 3.5" << endl;
    // cout << "skill medic 2 gunner" << endl;
    //不要删除这一行
    cout << "End" << endl;
}

int main()
{
    //初始化地图
    init_input();

    while(true)
    {
        per_frame_input();
        output_command();
    } 
    return 0;
}