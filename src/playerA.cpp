#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <list>

using namespace std;

const int frame_rate = 50;
const int INF = 1e9;

#include <fstream>
#include <ctime>
#include <sstream>
#include <cstdarg>
// 往文件里打日志 方便debug
#define LOG(filename, format, ...)                                      \
    do                                                                  \
    {                                                                   \
        std::ofstream log_file;                                         \
        log_file.open(filename, std::ios_base::app);                    \
        if (log_file.is_open())                                         \
        {                                                               \
            std::time_t now = std::time(nullptr);                       \
            char buffer[256];                                           \
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",  \
                          std::localtime(&now));                        \
            log_file << buffer << " [Line " << __LINE__ << "]: ";       \
            log_file << FormatString(format, __VA_ARGS__) << std::endl; \
            log_file.close();                                           \
        }                                                               \
        else                                                            \
        {                                                               \
            std::cerr << "Unable to open log file." << std::endl;       \
        }                                                               \
    } while (0)

std::string FormatString(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return std::string(buffer);
}

double get_dis(pair<double, double> p1, pair<double, double> p2)
{
    return sqrt((p1.first - p2.first) * (p1.first - p2.first) + (p1.second - p2.second) * (p1.second - p2.second));
}

struct Fire
{
    double x;
    double y;
};

struct Potion
{
    double x;
    double y;
};

// type = 1 move args = [x, y]
// type = 2 shoot args = [x, y]
// type =3 skill args = [index, args..]
struct Command
{
    int type;
    vector<double> args;
    Command(int type, vector<double> args)
    {
        this->type = type;
        this->args = args;
    }
};

class Object
{
public:
    string move(double x, double y)
    {
        return "move " + this->get_name() + " " + to_string(x) + " " + to_string(y);
    }

    string shoot(double x, double y)
    {
        return "shoot " + this->get_name() + " " + to_string(x) + " " + to_string(y);
    }

    // index: 技能id
    // args: 技能参数
    virtual string skill(int index, vector<string> args) = 0;

    // 获取角色名称
    virtual string get_name() = 0;

    // 更新状态 CD effect
    virtual void update_state()
    {
        for (auto &cd : this->CD)
        {
            cd--;
            if (cd <= 0)
                cd = 0;
        }
        for (auto &eff : this->effect)
        {
            eff--;
            if (eff <= 0)
                eff = 0;
        }
    }

    // 清空任务队列
    virtual void clear_commands()
    {
        commands.clear();
    }

    // 队尾添加任务
    virtual void add_commands(Command cmd)
    {
        commands.push_back(cmd);
    }

    // 队头插入任务
    virtual void insert_commands(Command cmd)
    {
        commands.push_front(cmd);
    }

    virtual string do_command()
    {
        if (this->commands.empty())
        {
            return "";
        }
        auto cmd = this->commands.front();
        switch (cmd.type)
        {
        case 1:
        {
            double dis = get_dis(pair<double, double>(cmd.args[0], cmd.args[1]), this->get_pos());
            LOG("./log.log", "%s moving to: (%lf, %lf)\t now: (%lf, %lf)\t dis: %lf", this->get_name(), cmd.args[0], cmd.args[1], this->pos.first, this->pos.second, dis);
            // move
            if (dis > 0.1)
            {
                return this->move(cmd.args[0], cmd.args[1]);
            }
            else
            {
                this->commands.pop_front();
                return this->do_command();
            }
            break;
        }
        case 2:
        {
            // shoot
            this->commands.pop_front();
            return this->shoot(cmd.args[0], cmd.args[1]);
            break;
        }
        case 3:
        {
            // skill
            this->commands.pop_front();
            vector<string> skill_args;
            for (int i = 1; i < cmd.args.size(); i++)
            {
                skill_args.push_back(to_string(cmd.args[i]));
            }
            return this->skill(cmd.args[0], skill_args);
            break;
        }
        default:
            break;
        }
        return "";
    }

    void set_pos(double x, double y)
    {
        pos.first = x;
        pos.second = y;
    }

    // 更新位置
    virtual void set_pos(pair<double, double> pos)
    {
        this->pos = pos;
    }

    // 获取位置
    virtual pair<double, double> get_pos()
    {
        return pos;
    }

    // 更新hp
    void set_hp(double hp)
    {
        this->hp = hp;
    }

    // 获取hp
    double get_hp()
    {
        return hp;
    }
    // 设置阵营
    void set_type(int type)
    {
        this->type = type;
    }

    // 获取阵营
    int get_type()
    {
        return type;
    }

public:
    pair<double, double> pos;
    double hp;
    int type; // 0: A队物体, 1: B队物体
              // 技能 CD
    vector<int> _CD;
    // 技能现在 CD
    vector<int> CD;
    // 技能效果现在持续时间
    vector<int> effect;
    // 技能效果持续时间
    // 地雷设置成无限大
    vector<int> _effect;

    // 移动速度
    double speed;
    // 射程
    double range;
    // 攻击力
    double attack;
    // 防御力
    double defense;

    list<Command> commands;
};

class Gunner : public Object
{
public:
    Gunner()
    {
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
    virtual string get_name()
    {
        return "Gunner";
    }

    // index = 1 加速
    // index = 2 射速
    virtual string skill(int index, vector<string> args)
    {
        string skill_str = "skill " + this->get_name();
        switch (index)
        {
        case 1:
            skill_str += " 1";
            this->CD[0] = this->_CD[0];
            this->effect[0] = this->_effect[0];
            break;
        case 2:
            this->CD[1] = this->_CD[1];
            this->effect[1] = this->_effect[1];
            skill_str += " 2";
            break;
        default:
            break;
        }
        return skill_str;
    }

    virtual void update_state()
    {
        for (auto &cd : this->CD)
        {
            cd--;
            if (cd <= 0)
                cd = 0;
        }
        for (auto &eff : this->effect)
        {
            eff--;
            if (eff <= 0)
                eff = 0;
        }
    }

public:
};

class Hurler : public Object
{
public:
    Hurler()
    {
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
    virtual string get_name()
    {
        return "Hurler";
    }

    // index = 1 燃烧
    // index = 2 隐形
    virtual string skill(int index, vector<string> args)
    {
        string skill_str = "skill " + this->get_name();
        switch (index)
        {
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

    virtual void update_state()
    {
        for (auto &cd : this->CD)
        {
            cd--;
            if (cd <= 0)
                cd = 0;
        }
        for (auto &eff : this->effect)
        {
            eff--;
            if (eff <= 0)
                eff = 0;
        }
    }

public:
};

class Medic : public Object
{
public:
    Medic()
    {
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
    virtual string get_name()
    {
        return "Medic";
    }

    // index = 1 治疗
    // index = 2 盾
    virtual string skill(int index, vector<string> args)
    {
        string skill_str = "skill " + this->get_name();
        switch (index)
        {
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
};

vector<vector<int>> game_map; // 游戏地图
// pair<double,double> GunnerA_Pos; //GunnerA位置
// pair<double,double> HurlerA_Pos; //HurlerA位置
// pair<double,double> MedicA_Pos; //MedicA位置
// pair<double,double> GunnerB_Pos; //GunnerB位置
// pair<double,double> HurlerB_Pos; //HurlerB位置
// pair<double,double> MedicB_Pos; //MedicB位置
// double GunnerA_HP; //GunnerA血量
// double HurlerA_HP; //HurlerA血量
// double MedicA_HP;  //MedicA血量
// double GunnerB_HP; //GunnerB血量
// double HurlerB_HP; //HurlerB血量
// double MedicB_HP;  //MedicB血量

Gunner GunnerA, GunnerB;
Hurler HurlerA, HurlerB;
Medic MedicA, MedicB;

bool has_coin;                 // 金币存在信息
pair<double, double> coin_Pos; // 金币位置信息
int scoreA;                    // A阵营得分
int scoreB;                    // B阵营得分
vector<Fire> fire_pos;         // 场上火焰的位置
vector<Potion> potion_pos;     // 场上药水的位置

bool pre_has_coin;                 // 金币存在信息
pair<double, double> pre_coin_Pos; // 金币位置信息
vector<Fire> pre_fire_pos;         // 场上火焰的位置
vector<Potion> pre_potion_pos;     // 场上药水的位置

int frame_count; // 帧数信息

void init_input()
{
    string init_str;
    cin >> init_str;

    int n, m;
    cin >> n >> m;
    for (int i = 0; i < n; i++)
    {
        vector<int> temp(m, 0);
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
        double hp;
        double x;
        double y;
        cin >> pos >> team >> pos >> type >> pos >> hp >> pos >> x >> pos >> y;
        if (team == "A")
        {
            if (type == "GunnerA")
            {
                GunnerA.set_hp(hp);
                GunnerA.set_pos(x, y);
            }
            else if (type == "HurlerA")
            {
                HurlerA.set_hp(hp);
                HurlerA.set_pos(x, y);
            }
            else if (type == "MedicA")
            {
                MedicA.set_hp(hp);
                MedicA.set_pos(x, y);
            }
        }
        else if (team == "B")
        {
            if (type == "GunnerB")
            {
                GunnerB.set_hp(hp);
                GunnerB.set_pos(x, y);
            }
            else if (type == "HurlerB")
            {
                HurlerB.set_hp(hp);
                HurlerB.set_pos(x, y);
            }
            else if (type == "MedicB")
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
    pre_fire_pos = fire_pos;
    fire_pos.clear();
    string pos;
    cin >> pos;
    int count;
    cin >> count;

    for (int i = 0; i < count; i++)
    {
        Fire fire;
        double x;
        double y;
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

    pre_has_coin = has_coin;

    string temp_has_coin;
    cin >> temp_has_coin;

    if (temp_has_coin == "true")
    {
        double x;
        double y;
        cin >> pos >> x >> pos >> y;
        coin_Pos.first = x;
        coin_Pos.second = y;
        has_coin = true;
    }
    else if (temp_has_coin == "false")
    {
        has_coin = false;
        cout << "Dont have coin!!!" << endl;
    }

    cin >> pos;
}

void potion_input()
{
    pre_potion_pos = potion_pos;
    potion_pos.clear();
    string pos;
    cin >> pos;

    int count;
    cin >> count;
    for (int i = 0; i < count; i++)
    {
        Potion potion;
        double x;
        double y;
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

void generate_command()
{
    GunnerA.add_commands(Command(3, {1}));
    GunnerA.add_commands(Command(1, {5.5, 16.5}));
    GunnerA.add_commands(Command(1, {16.5, 16.5}));
    GunnerA.add_commands(Command(1, {18.5, 21.5}));

    GunnerA.add_commands(Command(1, {18.5, 23.5}));
    GunnerA.add_commands(Command(1, {16.5, 23.5}));
    GunnerA.add_commands(Command(1, {16.5, 21.5}));
}

void output_command() // 请不要删除End一行，每行输出记得加上换行符
{
    if (frame_count > 300 && pre_has_coin == false && has_coin == true)
    {
        GunnerA.clear_commands();
        GunnerA.insert_commands(Command(1, {coin_Pos.first, coin_Pos.second}));
        GunnerA.add_commands(Command(1, {18.5, 21.5}));
        GunnerA.add_commands(Command(1, {16.5, 21.5}));
    }
    string cmd_str = GunnerA.do_command();
    if (!cmd_str.empty())
    {
        cout << cmd_str << endl;
    }
    // 不要删除这一行
    cout << "End" << endl;
}

int main()
{
    // 初始化地图
    init_input();
    LOG("./log.log", "%s", "start");
    generate_command();
    while (true)
    {
        per_frame_input();
        if (frame_count == 0)
        {
            generate_command();
        }
        GunnerA.update_state();
        HurlerA.update_state();
        MedicA.update_state();
        output_command();
    }
    return 0;
}