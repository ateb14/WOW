//
//  main.cpp
//  monster world
//
//  Created by 汪逸阳 on 2021/8/11.
//
#include <iostream>
#include <string>
#include<cstdio>
#include<vector>

using namespace std;

enum WarriorID{
    DRAGON = 0, NINJA, ICEMAN, LION, WOLF,
};
enum WeaponID{
    SWORD = 0, BOMB, ARROW,
};
const int WarriorTypeNum = 5;
const int MaxWarriorNum = 10001;
const int ColorNum = 2;

class base;
class warrior;
class dragon;
class ninja;
class iceman;
class lion;
class wolf;
class weapon;
class sword;
class bomb;
class arrow;

class base{
    int StrengthNum;
    int color;//0红 1蓝
    int TotalWarriorNum;
    bool isstop;//true生产结束，false生产进行中
    int CurCreatingOrder;//当前生产轮次
    int WarriorNum[WarriorTypeNum];//不同种类武士的数目
    warrior *pwarrior[MaxWarriorNum];
public:
    friend class warrior;
    friend class weapon;
    static int creatingorder[ColorNum][WarriorTypeNum];//生成武士的顺序
    base (int StrengthNum_, int color_);
    ~base();
    void creatwarrior(int hour);
    void broadcast_stopcreating(int hour);
    string getColor();
    bool getIsstop(){return isstop;}
    int getStrengthNum(){return StrengthNum;}
};

class weapon{
    int id;//武器种类id;
    warrior *pwarrior;
public:
    friend class warrior;
    friend class dragon;
    friend class ninja;
    friend class iceman;
    friend class lion;
    friend class wolf;
    weapon(int id_, warrior* pwarrior_);
    static vector<string> WeaponName;
};

class warrior{
    int id;//武士种类id
    int number;//第几个武士
    int strength;
    base *pbase;
public:
    friend class weapon;
    static int Strengths[WarriorTypeNum];//不同种类武士的生命力
    static vector<string> WarriorName;//id与武士名的对应关系
    void broadcast_original(int hour);
    virtual void broadcast_create(int hour){};
    warrior(int id_, int number_, base *pbase_);
    virtual ~warrior(){};
};

class dragon:public warrior{
    double morale;
    weapon *DragonWeapon;
public:
    virtual void broadcast_create(int hour);
    dragon(int id_, int number_, base *pbase_);
    ~dragon(){delete DragonWeapon;}
};

class ninja:public warrior{
    weapon *NinjaWeapon_1;
    weapon *NinjaWeapon_2;
public:
    virtual void broadcast_create(int hour);
    ninja(int id_, int number_, base *pbase_);
    ~ninja(){delete NinjaWeapon_1;delete NinjaWeapon_2;}
};

class iceman:public warrior{
    weapon *IcemanWeapon;
public:
    virtual void broadcast_create(int hour);
    iceman(int id_, int number_, base *pbase_);
    ~iceman(){delete IcemanWeapon;}
};

class lion:public warrior{
    int loyalty;
public:
    virtual void broadcast_create(int hour);
    lion(int id_, int number_, base *pbase_);
};

class wolf:public warrior{
public:
    virtual void broadcast_create(int hour);
    wolf(int id_, int number_, base *pbase_);
};

//weapon
weapon::weapon(int id_, warrior *pwarrior_):id(id_), pwarrior(pwarrior_){}

//warrior
warrior::warrior(int id_, int number_, base *pbase_):id(id_), number(number_), pbase(pbase_), strength(Strengths[id_]){}
void warrior::broadcast_original(int hour){
    printf("%03d ",hour);
    cout<<pbase->getColor()<<" "<<WarriorName[id]<<" "<<number<<" born with strength "<<strength<<","<<pbase->WarriorNum[id]<<" "<<WarriorName[id]<<" in "<<pbase->getColor()<<" headquarter"<<endl;
    //000 red iceman 1 born with strength 5,1 iceman in red headquarter
}
dragon::dragon(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_){
    DragonWeapon = new weapon (number_%3, this);
    int basepower = pbase_->getStrengthNum();
    morale = basepower*1.0/Strengths[DRAGON];
}
ninja::ninja(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_){
    NinjaWeapon_1 = new weapon(number_%3, this);
    NinjaWeapon_2 = new weapon((number_+1)%3, this);
}
iceman::iceman(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_) {
    IcemanWeapon = new weapon(number_%3, this);
}
lion::lion(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_){
    loyalty = pbase_->getStrengthNum();
}
wolf::wolf(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_){}
void dragon::broadcast_create(int hour){
    warrior::broadcast_original(hour);
    cout<<"It has a "<<DragonWeapon->WeaponName[DragonWeapon->id]<<",and it's morale is ";
    printf("%.2lf", morale);
    cout<<endl;
}
void ninja::broadcast_create(int hour){
    warrior::broadcast_original(hour);
    cout<<"It has a "<<NinjaWeapon_1->WeaponName[NinjaWeapon_1->id]<<" and a "<< NinjaWeapon_2->WeaponName[NinjaWeapon_2->id]<<endl;
}
void iceman::broadcast_create(int hour){
    warrior::broadcast_original(hour);
    cout<<"It has a "<<IcemanWeapon->WeaponName[IcemanWeapon->id]<<endl;
}
void lion::broadcast_create(int hour){
    warrior::broadcast_original(hour);
    cout<<"It's loyalty is "<<loyalty<<endl;
}
void wolf::broadcast_create(int hour){
    warrior::broadcast_original(hour);
}

warrior *GetNewWarrior(int id_, int number_, base *pbase_){
    if(id_== DRAGON){
        return new dragon(id_, number_, pbase_);
    }
    else if(id_==NINJA){
        return new ninja(id_, number_, pbase_);
    }
    else if(id_==ICEMAN){
        return new iceman(id_, number_, pbase_);
    }
    else if(id_==LION){
        return new lion(id_, number_, pbase_);
    }
    else if(id_==WOLF){
        return new wolf(id_, number_, pbase_);
    }
    return nullptr;
}
//base
base::base (int StrengthNum_, int color_):StrengthNum(StrengthNum_), color(color_){
    isstop = false;
    CurCreatingOrder = 0;
    TotalWarriorNum = 0;
    for(int i=0; i<WarriorTypeNum; i++){
        WarriorNum[i] = 0;
    }
}
base::~base(){
    for(int i=1;i<=TotalWarriorNum;i++){
        delete pwarrior[i];
    }
}
void base::broadcast_stopcreating(int hour){
    printf("%03d ",hour);
    cout<<getColor()<<" headquarter stops making warriors"<<endl;
    //004 blue headquarter stops making warriors
}
string base::getColor(){
    if(color == 0){
        return "red";
    }
    else {
        return "blue";
    }
}
void base::creatwarrior(int hour){
    if(isstop == true) return;
    int counter = 0;
    while(1){
        int curid = creatingorder[color][CurCreatingOrder];
        if(counter >= WarriorTypeNum){
            isstop = true;
            broadcast_stopcreating(hour);
            break;
        }
        if (warrior::Strengths[curid]<=StrengthNum){
            StrengthNum-=warrior::Strengths[curid];
            TotalWarriorNum++;
            WarriorNum[curid]++;
            warrior *newwarrior = GetNewWarrior(curid, TotalWarriorNum, this);
            pwarrior[TotalWarriorNum] = newwarrior;
            newwarrior->broadcast_create(hour);//多态
            CurCreatingOrder = (CurCreatingOrder+1)%WarriorTypeNum;
            break;
        }
        else{
            CurCreatingOrder = (CurCreatingOrder+1)%WarriorTypeNum;
            counter++;
        }
    }
}

int base::creatingorder[2][WarriorTypeNum]={{ICEMAN, LION, WOLF, NINJA, DRAGON},{LION, DRAGON, NINJA, ICEMAN, WOLF}};
vector<string> warrior::WarriorName = {"dragon", "ninja", "iceman", "lion", "wolf"};
vector<string> weapon::WeaponName = {"sword", "bomb", "arrow"};
int warrior::Strengths[WarriorTypeNum];

int main() {
    int CaseNum;
    cin >> CaseNum;
    for(int n=1; n<=CaseNum; ++n){
        cout << "Case:" << n << "\n";
        int Hour = 0;//Time initialization
        int LifeNum;
        cin >> LifeNum;
        for(int i=0; i<WarriorTypeNum; i++){
            cin >> warrior::Strengths[i];
        }//Number of strength and strength of warrior initialization
        base RedBase(LifeNum, 0),BlueBase(LifeNum, 1);
        //Hours passing by
        while(1){
            RedBase.creatwarrior(Hour);
            BlueBase.creatwarrior(Hour);
            if(RedBase.getIsstop()&&BlueBase.getIsstop()) break;//红蓝基地均停止生产
            Hour += 1;
        }
    }
    return 0;
}
