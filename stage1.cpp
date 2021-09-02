//
//  main.cpp
//  monster world
//
//  Created by ateb14 on 2021/7/31.
//
#include <iostream>
#include <string>
#include<cstdio>
#include<unordered_map>

using namespace std;

enum WarriorID{
    DRAGON = 0, NINJA, ICEMAN, LION, WOLF,
};
const int WarriorTypeNum = 5;
const int MaxWarriorNum = 10001;
const int ColorNum = 2;

class base;
class warrior;

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
    static int creatingorder[ColorNum][WarriorTypeNum];//生成武士的顺序
    base (int StrengthNum_, int color_);
    ~base();
    void creatwarrior(int hour);
    void boardcast_stopcreating(int hour);
    string getColor();
    bool getIsstop(){return isstop;}
};

class warrior{
    int id;//武士种类id
    int number;//第几个武士
    base *pbase;
public:
    static int Strengths[WarriorTypeNum];//不同种类武士的生命力
    static unordered_map<int, string> WarriorName;//id与武士名的对应关系
    void boardcast_create(int hour);
    warrior(int id_, int number_, base *pbase_);
};

//warrior
warrior::warrior(int id_, int number_, base *pbase_):id(id_), number(number_), pbase(pbase_){}
void warrior::boardcast_create(int hour){
    printf("%03d ",hour);
    cout<<pbase->getColor()<<" "<<WarriorName[id]<<" "<<number<<" born with strength "<<Strengths[id]<<","<<pbase->WarriorNum[id]<<" "<<WarriorName[id]<<" in "<<pbase->getColor()<<" headquarter"<<endl;
    //000 red iceman 1 born with strength 5,1 iceman in red headquarter
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
void base::boardcast_stopcreating(int hour){
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
            boardcast_stopcreating(hour);
            break;
        }
        if (warrior::Strengths[curid]<=StrengthNum){
            StrengthNum-=warrior::Strengths[curid];
            TotalWarriorNum++;
            WarriorNum[curid]++;
            warrior *newwarrior = new warrior(curid, TotalWarriorNum, this);
            pwarrior[TotalWarriorNum] = newwarrior;
            newwarrior->boardcast_create(hour);
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
unordered_map<int, string> warrior::WarriorName = {{0,"dragon"},{1,"ninja"},{2,"iceman"},{3,"lion"},{4,"wolf"}};
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
