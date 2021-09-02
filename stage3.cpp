//
//  main.cpp
//  monster world stage 3
//
//  Created by 汪逸阳 on 2021/8/31.
//
#include <iostream>
#include <string>
#include<cstdio>
#include<vector>
#include<algorithm>

using namespace std;

enum WarriorID{
    DRAGON = 0, NINJA, ICEMAN, LION, WOLF,
};
enum weaponID{
    SWORD = 0, BOMB, ARROW,
};
const int WarriorTypeNum = 5;
const int WeaponTypeNum = 3;
const int MaxWarriorNum = 20;
const int ColorNum = 2;

inline void gettime(int minute){
    printf("%03d:%02d ", minute/60, minute%60);
}

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
class city;

class base{
    int StrengthNum;
    int color;//0 red, 1 blue
    int TotalWarriorNum;//including the death and runners
    int ActualWarriorNum;
    bool isstop;//for prducing
    bool istaken;
    base *enemybase;
    int CurCreatingOrder;
    int WarriorNum[WarriorTypeNum];//the number of different types of warriors
    warrior *pwarrior[MaxWarriorNum];
    warrior *enemytaken;
    warrior *curwarrior;
public:
    friend class warrior;
    friend class weapon;
    friend class city;
    
    static int creatingorder[ColorNum][WarriorTypeNum];//producing order of warriors
    static int CityNum;
    static city *cities;
    static void march(int minute, base *rb, base *bb);
    static void LionRunnerChecker(int minute, base *rb, base *bb);
    static void rob(int minute, base *rb, base *bb);
    static void report(int minute, base *rb, base *bb);
    static void war(int minute, base *rb, base *bb);
    
    base (int StrengthNum_, int color_);
    base(){};
    ~base();
    void creatwarrior(int minute);
    void taken(int minute, warrior *pwarrior);
    void broadcast_taken(int minute);
    void setenemy(base *enemy){enemybase = enemy;}
    void broadcast_strength(int minute);
    
    string getColor();
    bool getIsstop(){return isstop;}
    int getStrengthNum(){return StrengthNum;}
    bool getistaken(){return istaken;}
};

class weapon{
    int weaponid;//weapon type id
    int weaponatk;
    bool isbroken;
public:
    warrior *pwarrior;
    
    static vector<string> WeaponName;
    static weapon * GetNewWeapon(int weaponid_, warrior *pwarrior_);
    
    weapon(int weaponid_, warrior* pwarrior_):weaponid(weaponid_), pwarrior(pwarrior_){isbroken=false;}
    
    virtual void attack(warrior *enemy){};
    
    int getweaponid(){return weaponid;}
    int getweaponatk(){return weaponatk;}
    bool getisbroken(){return isbroken;}
    void broken(){isbroken = true;}
    void setweaponatk(int newone){weaponatk = newone;}
    virtual void resetatk(){};
};

class warrior{
    int id;//warrior type id
    int number;//warrior number
public:
    base *pbase;
    weapon *pweapons[10];
    city *pcity;
    int weaponnum;//including broken ones
    int actualweaponnum;//excluding broken ones
    int strength;
    int postion;
    bool isalive;
    int WeaponStatus[WeaponTypeNum];
    void die();
    
    static int Strengths[WarriorTypeNum];//life strength of different warriors
    static int atks[WarriorTypeNum];// Attacking power of different warriors
    static vector<string> WarriorName;//mapping from id to warrior's name
    static warrior * GetNewWarrior(int id_, int number_, base *pbase_);
    
    warrior(int id_, int number_, base *pbase_);
    ~warrior();
    void broadcast_original(int minute);
    void broadcast_march(int minute);
    void broadcast_march_enemy(int minute);
    void broadcast_win(int minute, warrior *loser);
    virtual void broadcast_create(int minute){};
    void warriormarch(int minute);
    void sortweapon(int mode);
    void mergeweapon();//delete broken ones;
    void reportweapon(int minute);
    void damage(int hurt);
    
    int getid(){return id;}
    int getnumber(){return number;}
};

class dragon:public warrior{
public:
    virtual void broadcast_create(int minute);
    dragon(int id_, int number_, base *pbase_);
    void yell(int minute);
};

class ninja:public warrior{
public:
    virtual void broadcast_create(int minute);
    ninja(int id_, int number_, base *pbase_);
};

class iceman:public warrior{
public:
    virtual void broadcast_create(int minute);
    iceman(int id_, int number_, base *pbase_);
};

class lion:public warrior{
    int loyalty;
public:
    static int LionBetrayPara;
    
    virtual void broadcast_create(int minute);
    lion(int id_, int number_, base *pbase_);
    int getloyalty(){return loyalty;}
    void loyaltyminus(){loyalty-=LionBetrayPara;}
    void run(int minute);
};

class wolf:public warrior{
public:
    virtual void broadcast_create(int minute);
    wolf(int id_, int number_, base *pbase_);
    void rob(warrior *pwarrior,int minute);
};

//weapon
//definition of different types of weapons
class sword:public weapon{
public:
    sword(int weaponid_, warrior* pwarrior_):weapon(weaponid_, pwarrior_){
        setweaponatk(pwarrior->atks[pwarrior->getid()]*2/10);
    }
    virtual void attack(warrior *pwarrior);
    virtual void resetatk(){setweaponatk(pwarrior->atks[pwarrior->getid()]*2/10);}
};
class bomb:public weapon{
public:
    bomb(int weaponid_, warrior* pwarrior_):weapon(weaponid_, pwarrior_){
        setweaponatk(pwarrior->atks[pwarrior->getid()]*4/10);
    }
    virtual void attack(warrior *pwarrior);
    virtual void resetatk(){setweaponatk(pwarrior->atks[pwarrior->getid()]*4/10);}
};
class arrow:public weapon{
    int usingnum;
public:
    arrow(int weaponid_, warrior* pwarrior_):weapon(weaponid_, pwarrior_){
        usingnum = 0;
        setweaponatk(pwarrior->atks[pwarrior->getid()]*3/10);
    }
    virtual void attack(warrior *pwarrior);
    virtual void resetatk(){setweaponatk(pwarrior->atks[pwarrior->getid()]*3/10);}
    int getusingnum(){return usingnum;}
};
//weapon
//getweapon
weapon * weapon::GetNewWeapon(int weaponid_, warrior *pwarrior_){
    if(weaponid_==SWORD){
        return new sword(weaponid_, pwarrior_);
    }
    else if(weaponid_==BOMB){
        return new bomb(weaponid_, pwarrior_);
    }
    else if(weaponid_==ARROW){
        return new arrow(weaponid_, pwarrior_);
    }
    return nullptr;
}

//city
class city{
    int citynumber;
    warrior *predwarrior;
    bool isred;
    warrior *pbluewarrior;
    bool isblue;
public:
    friend class base;
    city(int citynumber_);
    city(){}
    void WarriorIn(warrior *pwarrior_);
    void WarriorOut(string color);
    int fight();//0 red win;-1 draw; 1 blue win.
    void warrob(warrior *winner, warrior *loser);
    void broadcast_draw(int minute);
    
    bool getenemystatus(string color);
    warrior *getenemy(string color);
    int getcitynumber(){return citynumber;}
};

//warrior
//constructor
warrior::warrior(int id_, int number_, base *pbase_):id(id_), number(number_), pbase(pbase_), strength(Strengths[id_]){
    isalive = true;
    pcity = NULL;
    if(pbase_->getColor()=="red"){
        postion = 0;
    }
    else{//blue
        postion = base::CityNum+1;
    }
    for(int i=0;i<WeaponTypeNum;++i){
        WeaponStatus[i] = 0;
    }
}
warrior::~warrior(){
    for(int i=0;i<weaponnum;++i){
        if(pweapons[i]){
            delete pweapons[i];
        }
    }
}
//warrior creating broadcast
void warrior::broadcast_original(int minute){
    gettime(minute);
    cout<<pbase->getColor()<<" "<<WarriorName[id]<<" "<<number<<" born"<<endl;
    //000:00 blue lion 1 born
    //Its loyalty is 24
}
void warrior::broadcast_march(int minute){
    gettime(minute);
    cout<<pbase->getColor()<<" "<<WarriorName[id]<<" "<<number<<" marched to city "<<postion<<" with "<<strength<<" elements and force "<<atks[id]<<endl;
    //000:10 red iceman 1 marched to city 1 with 20 elements and force 30
}
void warrior::broadcast_march_enemy(int minute){
    gettime(minute);
    cout<<pbase->getColor()<<" "<<WarriorName[id]<<" "<<number<<" reached "<<pbase->enemybase->getColor()<<" headquarter with "<<strength<<" elements and force "<<atks[id]<<endl;
    //001:10 red iceman 1 reached blue headquarter with 20 elements and force 30
}
void warrior::broadcast_win(int minute, warrior *loser){
    gettime(minute);
    cout<<pbase->getColor()<<" "<<WarriorName[id]<<" "<<number<<" killed "<<loser->pbase->getColor()<<" "<<WarriorName[loser->id]<<" "<<loser->number<<" in city "<<pcity->getcitynumber()<<" remaining "<<strength<<" elements"<<endl;
    //000:40 red iceman 1 killed blue lion 12 in city 2 remaining 20 elements
}
void warrior::die(){
    pcity->WarriorOut(pbase->getColor());
    --pbase->ActualWarriorNum;
    --pbase->WarriorNum[getid()];
    pbase->pwarrior[getnumber()] = NULL;
}
//warrior marching to the next city or base
void warrior::warriormarch(int minute){
    if(id==ICEMAN){
        int delta = strength/10;
        strength-=delta;
    }
    else if(id==LION){
        lion *plion = dynamic_cast<lion*>(this);
        plion->loyaltyminus();
    }
    if(pbase->getColor()=="red"){
        if(postion<base::CityNum){//still in city
            ++postion;
            base::cities[postion].WarriorIn(this);
            if(postion>=2)
                pcity->WarriorOut("red");
            else
                pbase->curwarrior = NULL;
            pcity = &base::cities[postion];
        }
        else if(postion == base::CityNum){//into base
            ++postion;
            pcity->WarriorOut("red");
            pcity = NULL;
            pbase->enemybase->taken(minute, this);
        }
    }
    else{//blue
        if(postion>1){//still in city
            --postion;
            base::cities[postion].WarriorIn(this);
            if(postion<base::CityNum)
                pcity->WarriorOut("blue");
            else
                pbase->curwarrior = NULL;
            pcity = &base::cities[postion];
        }
        else if(postion == 1){//into base
            --postion;
            pcity->WarriorOut("blue");
            pcity = NULL;
            pbase->enemybase->taken(minute, this);
        }
    }
}
void warrior::sortweapon(int mode = 0){
    struct cmp{
        bool operator ()(weapon *a, weapon *b){
            if(a->getweaponid()<b->getweaponid()){
                return true;
            }
            else if(a->getweaponid()==ARROW&&b->getweaponid()==ARROW){
                arrow *pa = dynamic_cast<arrow *>(a);
                arrow *pb = dynamic_cast<arrow *>(b);
                return pa->getusingnum()>pb->getusingnum();
            }
            return false;
        }
    };
    struct cmp1{
        bool operator ()(weapon *a, weapon *b){
            if(a->getweaponid()<b->getweaponid()){
                return true;
            }
            else if(a->getweaponid()==ARROW&&b->getweaponid()==ARROW){
                arrow *pa = dynamic_cast<arrow *>(a);
                arrow *pb = dynamic_cast<arrow *>(b);
                return pa->getusingnum()<pb->getusingnum();
            }
            return false;
        }
    };
    if(mode==0)//arrow using first
        sort(pweapons, pweapons+weaponnum, cmp());
    else{//arrow not using first
        sort(pweapons, pweapons+weaponnum, cmp1());
    }
}
void warrior::mergeweapon(){
    weapon *tmp[10];
    int cnt = 0;
    for(int i=0;i<weaponnum;++i){
        if(pweapons[i]==NULL) continue;;
        if(!pweapons[i]->getisbroken()){
            tmp[cnt++] = pweapons[i];
            pweapons[i] = NULL;
        }
        else{
            --WeaponStatus[pweapons[i]->getweaponid()];
            delete pweapons[i];
        }
    }
    weaponnum = cnt;
    actualweaponnum = cnt;
    for(int i=0;i<cnt;++i){
        pweapons[i] = tmp[i];
    }
}
void warrior::reportweapon(int minute){
    gettime(minute);
    cout<<pbase->getColor()<<" "<<WarriorName[id]<<" "<<number<<" has "<<WeaponStatus[SWORD]<<" sword "<<WeaponStatus[BOMB]<<" bomb "<<WeaponStatus[ARROW]<<" arrow and "<<strength<<" elements"<<endl;
    //000:55 blue wolf 2 has 2 sword 3 bomb 0 arrow and 7 elements
}
//specific warriors
//constructors
dragon::dragon(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_){
    weaponnum = 1;
    pweapons[0] = weapon::GetNewWeapon(number_%3, this);
    ++WeaponStatus[number_%3];
}
ninja::ninja(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_){
    weaponnum = 2;
    pweapons[0] = weapon::GetNewWeapon(number_%3, this);
    pweapons[1] = weapon::GetNewWeapon((number_+1)%3, this);
    ++WeaponStatus[number_%3];
    ++WeaponStatus[(number_+1)%3];
}
iceman::iceman(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_) {
    weaponnum = 1;
    pweapons[0] = weapon::GetNewWeapon(number_%3, this);
    ++WeaponStatus[number_%3];
}
lion::lion(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_){
    loyalty = pbase_->getStrengthNum();
    weaponnum = 1;
    pweapons[0] = weapon::GetNewWeapon(number_%3, this);
    ++WeaponStatus[number_%3];
}
wolf::wolf(int id_, int number_, base *pbase_):warrior(id_, number_, pbase_){
    weaponnum = 0;
}
//creating broadcast
void dragon::broadcast_create(int minute){
    warrior::broadcast_original(minute);
}
void ninja::broadcast_create(int minute){
    warrior::broadcast_original(minute);
}
void iceman::broadcast_create(int minute){
    warrior::broadcast_original(minute);
}
void lion::broadcast_create(int minute){
    warrior::broadcast_original(minute);
    cout<<"Its loyalty is "<<loyalty<<endl;
}
void wolf::broadcast_create(int minute){
    warrior::broadcast_original(minute);
}
//dragon yell
void dragon::yell(int minute){
    gettime(minute);
    cout<<pbase->getColor()<<" dragon "<<getnumber()<<" yelled in city "<<pcity->getcitynumber()<<endl;
    //005:40 blue dragon 2 yelled in city 6
}
//lion run
void lion::run(int minute){
    isalive = false;
    //broadcast
    gettime(minute);
    cout<<pbase->getColor()<<" "<<"lion "<<getnumber()<<" ran away"<<endl;
    //000:05 blue lion 1 ran away
   //city setting
    if(postion>0&&postion<=base::CityNum){
        pcity->WarriorOut(pbase->getColor());
    }
}
//wolf rob
void wolf::rob(warrior *pwarrior, int minute){
    if(pwarrior->getid()!=WOLF && pwarrior->weaponnum>0 && weaponnum<10){
        pwarrior->sortweapon();//sort
        int robid = pwarrior->pweapons[0]->getweaponid();
        int robcnt = 0;//number of the weapons robbed
        if(robid != ARROW){
            while(robcnt < pwarrior->weaponnum && weaponnum<10 && pwarrior->pweapons[robcnt]->getweaponid()==robid){
                pwarrior->pweapons[robcnt]->pwarrior = this;//changing master
                pwarrior->pweapons[robcnt]->resetatk();//changing atk
                pweapons[weaponnum] = pwarrior->pweapons[robcnt];
                ++WeaponStatus[pweapons[weaponnum]->getweaponid()];
                --pwarrior->WeaponStatus[pweapons[weaponnum]->getweaponid()];
                ++weaponnum;
                ++robcnt;
            }
            for(int i=0;i<pwarrior->weaponnum-robcnt;++i){
                pwarrior->pweapons[i] = pwarrior->pweapons[robcnt+i];
                pwarrior->pweapons[robcnt+i] = NULL;
            }
            pwarrior->weaponnum -= robcnt;
        }
        else if(robid == ARROW){
            int i = pwarrior->weaponnum-1;
            while(i>=0 && weaponnum<10){
                pwarrior->pweapons[i]->pwarrior = this;
                pwarrior->pweapons[i]->resetatk();
                pweapons[weaponnum] = pwarrior->pweapons[i];
                ++WeaponStatus[pweapons[weaponnum]->getweaponid()];
                --pwarrior->WeaponStatus[pweapons[weaponnum]->getweaponid()];
                pwarrior->pweapons[i] = NULL;
                ++weaponnum;
                --pwarrior->weaponnum;
                --i;
                ++robcnt;
            }
        }
        //broadcast
        if(robcnt>0){
            gettime(minute);
            cout<<pbase->getColor()<<" wolf "<<getnumber()<<" took "<<robcnt<<" "<<weapon::WeaponName[robid]<<" from "<<pwarrior->pbase->getColor()<<" "<<WarriorName[pwarrior->getid()]<<" "<<pwarrior->getnumber()<<" in city "<<pcity->getcitynumber()<<endl;
        }
        //000:35 blue wolf 2 took 3 bomb from red dragon 2 in city 4
    }
}
//getwarrior
warrior * warrior::GetNewWarrior(int id_, int number_, base *pbase_){
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
//constructor
base::base (int StrengthNum_, int color_):StrengthNum(StrengthNum_), color(color_){
    isstop = false;
    istaken = false;
    CurCreatingOrder = 0;
    TotalWarriorNum = 0;
    ActualWarriorNum = 0;
    enemytaken = NULL;
    curwarrior = NULL;
    for(int i=0; i<WarriorTypeNum; i++){
        WarriorNum[i] = 0;
    }
}
//destructor
base::~base(){
    for(int i=1;i<=TotalWarriorNum;i++){
        if(pwarrior[i]){
            delete pwarrior[i];
        }
    }
}

string base::getColor(){
    if(color == 0){
        return "red";
    }
    else {
        return "blue";
    }
}

void base::creatwarrior(int minute){
    if(isstop == true) return;
    int curid = creatingorder[color][CurCreatingOrder];
    //produce
    if (warrior::Strengths[curid]<=StrengthNum){
        StrengthNum-=warrior::Strengths[curid];
        ++TotalWarriorNum;
        ++ActualWarriorNum;
        ++WarriorNum[curid];
        warrior *newwarrior = warrior::GetNewWarrior(curid, TotalWarriorNum, this);
        pwarrior[TotalWarriorNum] = newwarrior;
        curwarrior = newwarrior;
        //broadcast
        newwarrior->broadcast_create(minute);
        CurCreatingOrder=(CurCreatingOrder+1)%WarriorTypeNum;
    }
    //stop producing
    else{
        isstop = true;
    }
}
void base::LionRunnerChecker(int minute, base *rb, base *bb){
    //lions run from west to east
    //red base
    if(rb->curwarrior!=NULL&&rb->curwarrior->getid()==LION){
        lion *plion = dynamic_cast<lion*>(rb->curwarrior);
        if(plion==NULL){
            cout<<"plion doesn't point to class lion!!"<<endl;
            return;
        }
        if(plion->getloyalty()<=0){
            plion->run(minute);
            --rb->ActualWarriorNum;
            --rb->WarriorNum[LION];
            rb->curwarrior = NULL;
        }
    }
    //cities
    for(int i=1;i<=CityNum;++i){
        if(cities[i].isred&&cities[i].predwarrior->getid()==LION){
            lion *plion = dynamic_cast<lion*>(cities[i].predwarrior);
            if(plion==NULL){
                cout<<"plion doesn't point to class lion!!"<<endl;
                return;
            }
            if(plion->getloyalty()<=0){
                plion->run(minute);
                --rb->ActualWarriorNum;
                --rb->WarriorNum[LION];
            }
        }
        if(cities[i].isblue&&cities[i].pbluewarrior->getid()==LION){
            lion *plion = dynamic_cast<lion*>(cities[i].pbluewarrior);
            if(plion==NULL){
                cout<<"plion doesn't point to class lion!!"<<endl;
                return;
            }
            if(plion->getloyalty()<=0){
                plion->run(minute);
                --bb->ActualWarriorNum;
                --bb->WarriorNum[LION];
            }
        }
    }
    //blue base
    if(bb->curwarrior!=NULL&&bb->curwarrior->getid()==LION){
        lion *plion = dynamic_cast<lion*>(bb->curwarrior);
        if(plion==NULL){
            cout<<"plion doesn't point to class lion!!"<<endl;
            return;
        }
        if(plion->getloyalty()<=0){
            plion->run(minute);
            --bb->ActualWarriorNum;
            --bb->WarriorNum[LION];
            bb->curwarrior = NULL;
        }
    }
}
void base::march(int minute, base *rb, base *bb){
    //marching
    for(int i=1;i<=rb->TotalWarriorNum;++i){
        if(rb->pwarrior[i]&&rb->pwarrior[i]->isalive){
            rb->pwarrior[i]->warriormarch(minute);
        }
    }
    for(int i=1;i<=bb->TotalWarriorNum;++i){
        if(bb->pwarrior[i]&&bb->pwarrior[i]->isalive){
            bb->pwarrior[i]->warriormarch(minute);
        }
    }
    //broadcast from west to east
    if(rb->getistaken()){//red base
        rb->enemytaken->broadcast_march_enemy(minute);
        rb->broadcast_taken(minute);
    }
    for(int i=1;i<=CityNum;++i){//cities
        if(cities[i].isred){
            cities[i].predwarrior->broadcast_march(minute);
        }
        if(cities[i].isblue){
            cities[i].pbluewarrior->broadcast_march(minute);
        }
    }
    if(bb->getistaken()){//blue base
        bb->enemytaken->broadcast_march_enemy(minute);
        bb->broadcast_taken(minute);
    }
}
void base::rob(int minute,base *rb, base *bb){
    for(int i=1;i<=CityNum;++i){
        if(cities[i].isred&&cities[i].isblue&&cities[i].predwarrior->getid()==WOLF){
            wolf * pwolf = dynamic_cast<wolf*>(cities[i].predwarrior);
            if(pwolf==NULL){
                cout<<"pwolf doesn't point to class wolf!!"<<endl;
                return;
            }
            pwolf->rob(pwolf->pcity->getenemy(pwolf->pbase->getColor()), minute);
        }
        if(cities[i].isblue&&cities[i].isred&&cities[i].pbluewarrior->getid()==WOLF){
            wolf * pwolf = dynamic_cast<wolf*>(cities[i].pbluewarrior);
            if(pwolf==NULL){
                cout<<"pwolf doesn't point to class wolf!!"<<endl;
                return;
            }
            pwolf->rob(pwolf->pcity->getenemy(pwolf->pbase->getColor()), minute);
        }
    }
}
void base::report(int minute, base *rb, base *bb){
    /*if(rb->istaken){
        rb->enemytaken->reportweapon(minute);
    }*/
    for(int i=1;i<=CityNum;++i){
        if(cities[i].isred){
            cities[i].predwarrior->reportweapon(minute);
        }
        if(cities[i].isblue){
            cities[i].pbluewarrior->reportweapon(minute);
        }
    }
    /*if(bb->istaken){
        bb->enemytaken->reportweapon(minute);
    }*/
}
void base::taken(int minute, warrior *pwarrior){
    istaken = true;
    enemytaken = pwarrior;
}
void base::broadcast_taken(int minute){
    gettime(minute);
    cout<<getColor()<<" headquarter was taken"<<endl;
    //003:10 blue headquarter was taken
}
void base::broadcast_strength(int minute){
    gettime(minute);
    cout<<StrengthNum<<" elements in "<<getColor()<<" headquarter"<<endl;
    //000:50 100 elements in red headquarter
}

//city
city::city(int citynumber_):citynumber(citynumber_){
    predwarrior = NULL;
    pbluewarrior = NULL;
    isred = false;
    isblue = false;
}
void city::WarriorIn(warrior *pwarrior_){
    if(pwarrior_->pbase->getColor()=="red"){
        predwarrior = pwarrior_;
        isred = true;
    }
    else{
        pbluewarrior = pwarrior_;
        isblue = true;
    }
}
void city::WarriorOut(string color){
    if(color=="red"){
        predwarrior = NULL;
        isred = false;
    }
    else if(color=="blue"){
        pbluewarrior = NULL;
        isblue = false;
    }
}
bool city::getenemystatus(string color){
    if(color=="red"){
        return isblue;
    }
    else return isred;
}
warrior * city::getenemy(string color){
    if(color=="red"){
        return pbluewarrior;
    }
    else return predwarrior;
}
void city::broadcast_draw(int minute){
    if(predwarrior->isalive&&pbluewarrior->isalive){
        gettime(minute);
        cout<<"both "<<predwarrior->pbase->getColor()<<" "<<warrior::WarriorName[predwarrior->getid()]<<" "<<predwarrior->getnumber()<<" and "<<pbluewarrior->pbase->getColor()<<" "<<warrior::WarriorName[pbluewarrior->getid()]<<" "<<pbluewarrior->getnumber()<<" were alive in city "<<citynumber<<endl;
        //000:40 both red iceman 1 and blue lion 12 were alive in city 2
    }
    else{
        gettime(minute);
        cout<<"both "<<predwarrior->pbase->getColor()<<" "<<warrior::WarriorName[predwarrior->getid()]<<" "<<predwarrior->getnumber()<<" and "<<pbluewarrior->pbase->getColor()<<" "<<warrior::WarriorName[pbluewarrior->getid()]<<" "<<pbluewarrior->getnumber()<<" died in city "<<citynumber<<endl;
        warrior *p1 = predwarrior, *p2 = pbluewarrior;
        predwarrior->die();
        pbluewarrior->die();
        delete p1;
        delete p2;
        //000:40 both red iceman 1 and blue lion 12 died in city 2
    }
}
int city::fight(){
    //sorting weapons
    if(predwarrior->weaponnum>0){
        predwarrior->mergeweapon();
        predwarrior->sortweapon();
    }
    if(pbluewarrior->weaponnum>0){
        pbluewarrior->mergeweapon();
        pbluewarrior->sortweapon();
    }
    //deciding attacking order
    warrior *fwarrior[2];
    if(citynumber%2==1){
        fwarrior[0] = predwarrior;
        fwarrior[1] = pbluewarrior;
    }
    else if(citynumber%2==0){
        fwarrior[0] = pbluewarrior;
        fwarrior[1] = predwarrior;
    }
    //fighting!
    int turn = 0, wpturn[2] = {0, 0};
    predwarrior->actualweaponnum = predwarrior->weaponnum;
    pbluewarrior->actualweaponnum = pbluewarrior->weaponnum;
    int prehealth[2] = {fwarrior[0]->strength, fwarrior[1]->strength};
    int preweaponnum[2] = {fwarrior[0]->weaponnum, fwarrior[1]->weaponnum};
    int nothingchanged = 0;
    while(fwarrior[0]->isalive&&fwarrior[1]->isalive&&(fwarrior[0]->actualweaponnum>0||fwarrior[1]->actualweaponnum>0)){
        if(fwarrior[turn]->actualweaponnum>0){
            fwarrior[turn]->pweapons[wpturn[turn]]->attack(fwarrior[(turn+1)%2]);
            wpturn[turn] = (wpturn[turn]+1)%fwarrior[turn]->weaponnum;
            while(fwarrior[turn]->actualweaponnum>0&&fwarrior[turn]->pweapons[wpturn[turn]]->getisbroken()){
                wpturn[turn] = (wpturn[turn]+1)%fwarrior[turn]->weaponnum;
            }
        }
        turn = (turn+1)%2;
        if(prehealth[0]==fwarrior[0]->strength&&prehealth[1]==fwarrior[1]->strength&&preweaponnum[0]==fwarrior[0]->actualweaponnum&&preweaponnum[1]==fwarrior[1]->actualweaponnum){
            ++nothingchanged;
        }
        else{
            nothingchanged = 0;
            prehealth[0] = fwarrior[0]->strength;
            prehealth[1] = fwarrior[1]->strength;
            preweaponnum[0] = fwarrior[0]->actualweaponnum;
            preweaponnum[1] = fwarrior[1]->actualweaponnum;
        }
        if(nothingchanged>20){
            break;
        }
    }
    //result
    if((predwarrior->isalive&&pbluewarrior->isalive)||(!predwarrior->isalive&&!pbluewarrior->isalive)){
        return -1;//draw
    }
    else if(predwarrior->isalive&&!pbluewarrior->isalive){
        return 0;//red wins
    }
    else {//pbluewarrior->isalive&&!predwarrior->isalive
        return 1;//blue wins
    }
}
void city::warrob(warrior *winner, warrior *loser){
    if(winner->weaponnum>0){
        winner->mergeweapon();
    }
    if(loser->weaponnum>0){
        loser->mergeweapon();
        loser->sortweapon(1);
    }
    int cnt = 0;
    while(winner->weaponnum<10&&loser->weaponnum>0){
        loser->pweapons[cnt]->pwarrior = winner;
        winner->pweapons[winner->weaponnum] = loser->pweapons[cnt];
        winner->pweapons[winner->weaponnum]->resetatk();
        ++winner->WeaponStatus[winner->pweapons[winner->weaponnum]->getweaponid()];
        ++winner->weaponnum;
        --loser->weaponnum;
        ++cnt;
    }
    while(loser->weaponnum>0){
        delete loser->pweapons[cnt++];
        --loser->weaponnum;
    }
    loser->die();
    delete loser;
}
//war time
void base::war(int minute, base *rb, base *bb){
    for(int i=1;i<=CityNum;++i){
        city *pc = &cities[i];
        if(pc->isblue&&pc->isred){
            int ret = pc->fight();
            //red wins
            if(ret==0){
                //broadcast
                pc->predwarrior->broadcast_win(minute, pc->pbluewarrior);
                //rob
                pc->warrob(pc->predwarrior, pc->pbluewarrior);
            }
            //blue wins
            else if(ret==1){
                pc->pbluewarrior->broadcast_win(minute, pc->predwarrior);
                pc->warrob(pc->pbluewarrior, pc->predwarrior);
            }
            else{//draw
                pc->predwarrior->mergeweapon();
                pc->pbluewarrior->mergeweapon();
                pc->broadcast_draw(minute);
            }
            //yell
            if(pc->isred&&pc->predwarrior->getid()==DRAGON&&pc->predwarrior->isalive){
                dragon *pdragon = dynamic_cast<dragon*>(pc->predwarrior);
                pdragon->yell(minute);
            }
            if(pc->isblue&&pc->pbluewarrior->getid()==DRAGON&&pc->pbluewarrior->isalive){
                dragon *pdragon = dynamic_cast<dragon*>(pc->pbluewarrior);
                pdragon->yell(minute);
            }
        }
    }
}
//attack
void sword::attack(warrior *enemy){
    enemy->damage(getweaponatk());
}
void bomb::attack(warrior *enemy){
    enemy->damage(getweaponatk());
    if(pwarrior->getid()!=NINJA){
        pwarrior->damage(getweaponatk()/2);
    }
    broken();
    --pwarrior->actualweaponnum;
}
void arrow::attack(warrior *enemy){
    enemy->damage(getweaponatk());
    ++usingnum;
    if(usingnum>=2){
        broken();
        --pwarrior->actualweaponnum;
    }
}
//damage
void warrior::damage(int hurt){
    strength -= hurt;
    if(strength<=0){
        isalive = false;
    }
}
//world
class world{
    static int minutes[7];
public:
    void execute();
};
void world::execute(){
    int minute = 0, turn = 0;//Time initialization
    int LifeNum, EndTime;
    cin >> LifeNum >> base::CityNum >> lion::LionBetrayPara >> EndTime;
    
    //Number of strength and strength of warrior initialization
    for(int i=0; i<WarriorTypeNum; ++i){
        cin >> warrior::Strengths[i];
    }
    
    //atk initialization
    for(int i=0; i<WarriorTypeNum; ++i){
        cin >> warrior::atks[i];
    }
    
    //bases initialization
    base *RedBase = new base, *BlueBase = new base;
    *RedBase = base(LifeNum, 0);
    *BlueBase = base(LifeNum, 1);
    RedBase->setenemy(BlueBase);
    BlueBase->setenemy(RedBase);
    
    //cities initialization
    base::cities = new city[base::CityNum+1];
    for(int i=1;i<=base::CityNum;++i){
        base::cities[i] = i;
    }
    
    //take turns
    while(minute <= EndTime&&!RedBase->getistaken()&&!BlueBase->getistaken()){
        //producing time
        if(minute%60 == 0){
            RedBase->creatwarrior(minute);
            BlueBase->creatwarrior(minute);
        }
        // lion runnning away
        else if(minute%60 == 5){
            base::LionRunnerChecker(minute, RedBase, BlueBase);
        }
        //marching
        else if(minute%60 == 10){
            base::march(minute, RedBase, BlueBase);
        }
        if(RedBase->getistaken()||BlueBase->getistaken()) break;
        //wolf getting weapons
        else if(minute%60 == 35){
            base::rob(minute, RedBase, BlueBase);
        }
        //fighting
        else if(minute%60 == 40){
            base::war(minute, RedBase, BlueBase);
        }
        //bases reporting strength
        else if(minute%60 == 50){
            RedBase->broadcast_strength(minute);
            BlueBase->broadcast_strength(minute);
        }
        //warriors reporting weapons
        else if(minute%60 == 55){
            base::report(minute, RedBase, BlueBase);
        }
        //time advancing
        minute += minutes[turn%7];
        ++turn;
    }
    delete [] base::cities;
    delete RedBase;
    delete BlueBase;
}

int base::creatingorder[2][WarriorTypeNum]={{ICEMAN, LION, WOLF, NINJA, DRAGON},{LION, DRAGON, NINJA, ICEMAN, WOLF}};
vector<string> warrior::WarriorName = {"dragon", "ninja", "iceman", "lion", "wolf"};
vector<string> weapon::WeaponName = {"sword", "bomb", "arrow"};
int warrior::Strengths[WarriorTypeNum];
int warrior::atks[WarriorTypeNum];
int world::minutes[7] = {5, 5, 25, 5, 10, 5, 5};
int base::CityNum;
city * base::cities;
int lion::LionBetrayPara;

int main() {
    int CaseNum;
    cin >> CaseNum;
    for(int n=1; n<=CaseNum; ++n){
        cout << "Case " << n << ":\n";
        world *NewWorld = new world;
        NewWorld->execute();
    }
    return 0;
}
