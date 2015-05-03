/*
* Copyright 2015.215B工作室
* All rights reserved.
*
* 文件名称：ai.cpp
* 摘	要：LOTA
*
* 当前版本：1.2
* 作	者: chendaxixi/陈少滨
* 完成日期：2015年4月30日
*
* 取代版本：1.1
* 原作者  ：chendaxixi/陈少滨
* 完成日期：2015年4月14日
*/

#include "sdk.h"
#include <ctime>
#include <cstdlib>
#include <fstream>
using namespace std;

//定义游戏数据类
class PData{
public:
	PData(const PMap* m = NULL, const PPlayerInfo* i = NULL):map(m), info(i), state(), 
		myHeros(), block(), enemys(), target(), enemysInit(), blocksAdd(0), revivingTIME(), enemysRating(), symbols(), towers(),
		towerLeft_Enemy(2), HideTIME(), flag(false), towerAttacked(false){
		for(int i = 0;i < 5;i++){
			already[i] = false;
			levelUP[i] = 0;
			state.push_back(-1);
		}
	}
public:
	void SetData(const PMap* m, const PPlayerInfo* i){
		map = m;
		info = i;
	}
public:
	const PMap* map;
	const PPlayerInfo* info;
	vector<int> state;
	vector<PUnit> myHeros;
	vector<PUnit> enemys;
	vector<PUnit> enemysSeen;
	vector<PUnit> enemysInit;
	vector<Pos> endPos;
	vector<PUnit> target;
	vector<PUnit> towers;
	vector<Pos> block;
	vector<Pos> symbols;
	vector<int> enemysRating;
	vector<int> revivingTIME;
	vector<int> HideTIME;
	Pos spring;
	bool already[5];
	int towerLeft_Enemy;
	int blocksAdd;
	int levelUP[5];
	bool flag;
	bool towerAttacked;
};

//声明函数指针类型
typedef bool (*Update)(PUnit&);
typedef bool (*Rule)(PUnit&);
typedef void (*Action)(PUnit&, PCommand*);

//常量宏定义
#define UpdateCount 12
#define StateCount 12
#define RuleCount 5

//Update_State声明
bool Update_0(PUnit&);
bool Update_1(PUnit&);
bool Update_2(PUnit&);
bool Update_3(PUnit&);
bool Update_4(PUnit&);
bool Update_5(PUnit&);
bool Update_6(PUnit&);
bool Update_7(PUnit&);
bool Update_8(PUnit&);
bool Update_9(PUnit&);
bool Update_10(PUnit&);
bool Update_11(PUnit&);
//Rule声明
bool Rule_0_0(PUnit&);
bool Rule_0_1(PUnit&);
bool Rule_0_2(PUnit&);
bool Rule_0_3(PUnit&);
bool Rule_1_0(PUnit&);
bool Rule_1_1(PUnit&);
bool Rule_1_2(PUnit&);
bool Rule_1_3(PUnit&);
bool Rule_1_4(PUnit&);
bool Rule_2_0(PUnit&);
bool Rule_2_1(PUnit&);
bool Rule_2_2(PUnit&);
bool Rule_3_0(PUnit&);
bool Rule_4_0(PUnit&);
bool Rule_5_0(PUnit&);
bool Rule_6_0(PUnit&);
bool Rule_7_0(PUnit&);
bool Rule_7_1(PUnit&);
bool Rule_7_2(PUnit&);
bool Rule_8_0(PUnit&);
bool Rule_9_0(PUnit&);
bool Rule_10_0(PUnit&);
bool Rule_10_1(PUnit&);
bool Rule_11_0(PUnit&);
//Action声明
void Action_0_0(PUnit&, PCommand*);
void Action_0_1(PUnit&, PCommand*);
void Action_0_2(PUnit&, PCommand*);
void Action_0_3(PUnit&, PCommand*);
void Action_1_0(PUnit&, PCommand*);
void Action_1_1(PUnit&, PCommand*);
void Action_1_2(PUnit&, PCommand*);
void Action_1_3(PUnit&, PCommand*);
void Action_1_4(PUnit&, PCommand*);
void Action_2_0(PUnit&, PCommand*);
void Action_2_1(PUnit&, PCommand*);
void Action_2_2(PUnit&, PCommand*);
void Action_3_0(PUnit&, PCommand*);
void Action_4_0(PUnit&, PCommand*);
void Action_5_0(PUnit&, PCommand*);
void Action_6_0(PUnit&, PCommand*);
void Action_7_0(PUnit&, PCommand*);
void Action_7_1(PUnit&, PCommand*);
void Action_7_2(PUnit&, PCommand*);
void Action_8_0(PUnit&, PCommand*);
void Action_9_0(PUnit&, PCommand*);
void Action_10_0(PUnit&, PCommand*);
void Action_10_1(PUnit&, PCommand*);
void Action_11_0(PUnit&, PCommand*);
//其他函数
void Interpert_Input(PCommand* cmd);
void Init();
void ChooseHero(PCommand* cmd);
void UpdateData(PCommand* cmd);
int rankHero(PUnit& hero);
int rankEnemy(PUnit& enemy);
void UpdateState(PUnit& hero);
void Rule_Action(PUnit& hero, PCommand* cmd);
int Evaluation(vector<PUnit>& heros, vector<PUnit>& enemys, int round);
int Evaluation(vector<PUnit>& heros, PUnit& enemy);
void UpdateEnemys();
PUnit ChooseEnemy(PUnit& hero);
int ArriveTime(PUnit& hero, Pos pos, int speed);
void UpdateUnit(PUnit& unit);
bool isTargetDead(PUnit& hero);
bool isSeen(PUnit& enemy);
void FindEnemysAble(PUnit& hero, vector<PUnit>& enemys);
Pos FindHeroAtkPos(PUnit& hero, PUnit& enemy);
Pos FindHeroWaitPos(PUnit& hero, PUnit& enemy);
void UpdateTarget(PUnit& hero);
bool LevelUp(PUnit& hero, PCommand* cmd);
int FindDirection(PUnit& hero, PUnit& enemy);
bool canUseSkill(PUnit& hero, int typeId);
bool BeginAttack(vector<PUnit>& heros, PUnit& enemy);
void UpdateReviving();

//全局变量
Update updates[UpdateCount] = {Update_0, Update_1, Update_2, Update_3, Update_4, Update_5, Update_6, Update_7, Update_8, Update_9, Update_10, Update_11};
int updates_State[UpdateCount] = {0,1,2,3,4,5,6,7,8,9,10,11};
int updateRank[UpdateCount] = {11,10,9,8,7,6,5,2,4,3,1,0};
Rule rules[StateCount][RuleCount] = {
	{Rule_0_0, Rule_0_1, Rule_0_2, Rule_0_3},
	{Rule_1_0, Rule_1_1, Rule_1_2, Rule_1_3, Rule_1_4},
	{Rule_2_0, Rule_2_1, Rule_2_2},
	{Rule_3_0},
	{Rule_4_0},
	{Rule_5_0},
	{Rule_6_0},
	{Rule_7_0, Rule_7_1, Rule_7_2},
	{Rule_8_0},
	{Rule_9_0},
	{Rule_10_0, Rule_10_1},
	{Rule_11_0}
	};
Action actions[StateCount][RuleCount] = {
	{Action_0_0, Action_0_1, Action_0_2, Action_0_3},
	{Action_1_0, Action_1_1, Action_1_2, Action_1_3, Action_1_4},
	{Action_2_0, Action_2_1, Action_2_2},
	{Action_3_0},
	{Action_4_0},
	{Action_5_0},
	{Action_6_0},
	{Action_7_0, Action_7_1, Action_7_2},
	{Action_8_0},
	{Action_9_0},
	{Action_10_0, Action_10_1},
	{Action_11_0}
	};
int ruleCount[StateCount] = {4,5,3,1,1,1,1,3,1,1,2,1};
int ruleRank[StateCount][RuleCount] ={
	{0,1,2,3},
	{0,1,2,3,4},
	{0,1,2},
	{0},
	{0},
	{0},
	{0},
	{0,1,2},
	{0},
	{0},
	{0,1},
	{0}
	};
PData* data = new PData();
ofstream output("result.txt");

void player_ai(const PMap &map, const PPlayerInfo &info, PCommand &cmd)
{
	time_t t1 = clock();
	data->SetData(&map, &info);
	output << "round:" << data->info->round << endl;
	Interpert_Input(&cmd);
	time_t t2 = clock();
	output << "\nTime:" << (double)(t2-t1)/CLOCKS_PER_SEC * 1000 << endl;
	if((double)(t2-t1)/CLOCKS_PER_SEC * 1000 > 100){
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	}
}

void Interpert_Input(PCommand* cmd){
	cmd->cmds.clear();
	if(data->info->round == 0){
		Init();
		ChooseHero(cmd);
		return;
	}
	UpdateData(cmd);
	for(int i = 0;i < data->myHeros.size();i++){
		Rule_Action(data->myHeros[i], cmd);
	}
	output << "blocks:" << data->block.size() << " " << data->blocksAdd << endl;
	for(int i = 0;i < data->blocksAdd;i++)
		data->block.pop_back();
	for(int i = 0;i < cmd->cmds.size();i++){
		output << "!!!cmd: id:" << cmd->cmds[i].id << " typeId:" << cmd->cmds[i].typeId << endl;
	}
}
//更新data
void UpdateData(PCommand* cmd){
	data->myHeros.clear();
	output << "units:" << endl;
	for(int i = 0;i < data->info->units.size();i++){
		const PUnit& unit = data->info->units[i];
		output << "id:" << unit.id << endl;
		if(unit.camp == data->info->camp && unit.isHero()){
			data->myHeros.push_back(unit);
			if(unit.findBuff("Reviving")){
				cout << data->info->round << " Yes\n";
			}
		}
	}

	for(int i = 0;i < data->myHeros.size();i++){
		data->already[i] = false;
	}

	UpdateEnemys();
	UpdateReviving();
	data->blocksAdd = 0;
	for(int i = 0;i < data->enemys.size();i++){
//		if(data->enemys[i].max_hp > 0){
		if(isSeen(data->enemys[i])){
			data->block.push_back(data->enemys[i].pos);
			data->blocksAdd++;
		}
	}
	for(int i = 0;i < data->myHeros.size();i++){
		data->block.push_back(data->myHeros[i].pos);
		data->blocksAdd++;
	}
		
	data->towers.clear();
	if(data->info->camp == 0){
		for(int i = 0;i < 2;i++){
			data->towers.push_back(PUnit(3, 10+i, 0, 0, Player0_tower_pos[i].x, Player0_tower_pos[i].y));
			data->towers[i].max_hp = 0;
			UpdateUnit(data->towers[i]);
			if(data->towers[i].max_hp > 0 && !data->towerAttacked){
				for(int j = 3;j < 8;j++)
					if(data->towers[i]["LastHit"]->val.size() > 0){
						data->towerAttacked = true;
						break;
					}
			}
		}
	}
	else{
		for(int i = 0;i < 2;i++){
			data->towers.push_back(PUnit(3, 12+i, 1, 1, Player1_tower_pos[i].x, Player1_tower_pos[i].y));
			data->towers[i].max_hp = 0;
			UpdateUnit(data->towers[i]);
			if(data->towers[i].max_hp > 0 && !data->towerAttacked){
				for(int j = 3;j < 8;j++)
					if(data->towers[i]["LastHit"]->val.size() > 0){
						data->towerAttacked = true;
						break;
					}
			}
		}
	}

//	if(data->info->round >= 310)
//		data->towerAttacked = true;
	
	for(int i = 0;i < data->myHeros.size();i++){
		if(data->myHeros[i].findBuff("Reviving")){
			data->state[i] = 0;
		}
		UpdateUnit(data->target[i]);
		UpdateTarget(data->myHeros[i]);
		if(data->HideTIME[i] > 0){
			data->HideTIME[i]--;
		}
	}
	for(int i = 0;i < data->myHeros.size();i++){
		if(!data->myHeros[i].findBuff("Reviving"))
			UpdateState(data->myHeros[i]);
	}
}
//更新State
void UpdateState(PUnit& hero){
	for(int i = 0;i < UpdateCount;i++)
		if(updates[updateRank[i]](hero)){
			data->state[rankHero(hero)] = updates_State[updateRank[i]];
			output << "Updata:" << i << " state:" << data->state[rankHero(hero)] << endl;
			break;
		}
}
//Rule-Action规则对应
void Rule_Action(PUnit& hero, PCommand* cmd){
	if(hero.findBuff("Reviving")){
		return;
	}
	int state = data->state[rankHero(hero)];
	for(int i = 0;i < ruleCount[state];i++)
		if(rules[state][ruleRank[state][i]](hero)){
			output << "Rule:" << state << "_" << i << endl;
			actions[state][ruleRank[state][i]](hero, cmd);
			break;
		}
}

//状态：奔走状态
bool Update_0(PUnit& hero){
	return true;
}
//状态：视野看到目标状态
bool Update_1(PUnit& hero){
	int index = rankHero(hero);
	if(data->target[index].id == -1){
		return false;
	}
	if(dis2(data->target[index].pos, hero.pos) > hero.view){
		return false;
	}
	return true;
}
//状态：遭遇对方英雄状态
bool Update_2(PUnit& hero){
	if(hero.findBuff("Hided")){
		return false;
	}
	for(int i = 3;i < 8;i++){
		if(isSeen(data->enemys[i]) && (dis2(data->enemys[i].pos, hero.pos) <= hero.view)){
			return true;
		}
	}
	return false;
}
//状态：战斗状态
bool Update_3(PUnit& hero){
	int index = rankHero(hero);
	if(data->target[index].id == -1){
		return false;
	}
	if(isSeen(data->target[index]) && dis2(data->target[index].pos, hero.pos) <= hero.range){
		return true;
	}
	return false;
}
//状态：推塔行进状态
bool Update_4(PUnit& hero){
	int index = rankHero(hero);
	if(data->towerAttacked) return true;
	for(int i = 0;i < data->myHeros.size();i++){
		if(data->state[i] >= 4 && data->state[i] <= 6)
			return true;
	}
	bool flag = true;
	for(int i = 0;i < data->myHeros.size();i++){
		if(data->myHeros[i].level < 2 || data->myHeros[i].hp < 50){
			flag = false;
			break;
		}
	}
	return flag;
}
//状态：推塔巡逻状态
bool Update_5(PUnit& hero){
	int index = rankHero(hero);
	if(data->state[index] == 5){
		return true;
	}
	if(data->target[rankHero(hero)].typeId != 3) 
		return false;
//	if(data->state[index] == 4 && dis2(data->target[index].pos, hero.pos) <= hero.view && dis2(data->target[index].pos, hero.pos) > data->target[index].range){
//		return true;
//	}
	int dx[] = {-15,-16,-17,-18,-19};
	int dy[] = {-2,-3,-4,-5,-6};
	int xx[] = {-10,-9,-8,-7,-6};
	int yy[] = {10,11,12,13,13};
	int rate = 1;
	if(data->info->camp == 1){
		rate = -1;
	}
	Pos pos;
	if(data->enemysRating[1] != 2000){
		pos.x = data->target[rankHero(hero)].pos.x + dx[rankHero(hero)] * rate;
		pos.y = data->target[rankHero(hero)].pos.y + dy[rankHero(hero)] * rate;
	}
	else{
		pos.x = data->enemys[0].pos.x + xx[rankHero(hero)] * rate;
		pos.y = data->enemys[0].pos.y + yy[rankHero(hero)] * rate;
	}
	if(data->state[index] == 4 && ArriveTime(hero, pos, hero.speed) == 0){
		return true;
	}
	return false;
}
//状态：推塔战斗状态
bool Update_6(PUnit& hero){
	if(data->state[rankHero(hero)] == 6){
		return true;
	}
	int num = 0;
	for(int i = 0;i < data->myHeros.size();i++){
		if(data->state[i] != 5 && data->state[i] != 6){
			num++;
		}
	}
	if(num <= 1 && (data->state[rankHero(hero)] == 5)){
		return true;
	}
	return false;
}
//状态：目标死亡状态
bool Update_7(PUnit& hero){
	return isTargetDead(hero);	
}
//状态：对方塔边清敌状态
bool Update_8(PUnit& hero){
	return false;
}
//状态：守塔状态
bool Update_9(PUnit& hero){
	return false;
}
//状态：濒死状态
bool Update_10(PUnit& hero){
	vector<PUnit> enemys;
	FindEnemysAble(hero, enemys);
	int hunt = 0;
	for(int i = 0;i < enemys.size();i++){
		hunt += enemys[i].atk - hero.def;
	}
	if(hunt > hero.hp){
		return true;
	}
	return false;
}
//状态：TMP
bool Update_11(PUnit& hero){
	return data->flag;
}
	

//寻怪奔走状态：刺客一次隐身可至
bool Rule_0_0(PUnit& hero){
	if(data->target[rankHero(hero)].typeId == 10) return false;
	if(hero.typeId != 2 || hero.findBuff(0)){
		return false;
	}
	if(!canUseSkill(hero, 13)){
		return false;
	}
	int round = ArriveTime(hero, FindHeroAtkPos(hero, data->target[rankHero(hero)]), 
		hero.speed);
	if(round < 5){
		return true;
	}
	return false;
}
//寻怪奔走状态：刺客二次隐身可至
bool Rule_0_1(PUnit& hero){
	return false;
	if(hero.typeId != 2 || hero.findBuff(0)){
		return false;
	}
	if(!canUseSkill(hero, 13)){
		return false;
	}
	if(hero.mp < 60){
		return false;
	}
	int round = ArriveTime(hero, FindHeroAtkPos(hero, data->target[rankHero(hero)]), hero.speed);
	return false;
}
//寻怪奔走状态：刺客Blink可至
bool Rule_0_2(PUnit& hero){
	if(hero.typeId != 2){
		return false;
	}
	if(!canUseSkill(hero, 17)){
		return false;
	}
	Pos pos = FindHeroAtkPos(hero, data->target[rankHero(hero)]);
	if(abs(data->map->height[hero.pos.x][hero.pos.y] - data->map->height[pos.x][pos.y]) >= 2
		&& dis2(hero.pos, pos) <= 81){
		return true;
	}
	return false;
}
//寻怪奔走状态：
bool Rule_0_3(PUnit& hero){
	return true;
}

//视野看到目标状态:刺客准备隐身
bool Rule_1_0(PUnit& hero){
	if(hero.typeId != 2){
		return false;
	}
	if(hero.findBuff("Hided")){
		return false;
	}
	if(!canUseSkill(hero, 13)){
		return false;
	}
	return true;
}
//视野看到目标状态:刺客已隐身，准备联合进攻
bool Rule_1_1(PUnit& hero){
	if(hero.typeId != 2 || !hero.findBuff(0)){
		return false;
	}
	int num = 0;
	int index = rankHero(hero);
	for(int i = 0;i < data->myHeros.size();i++){
		if(!data->myHeros[i].findBuff("Reviving"))
		if(data->target[i].id == data->target[index].id){
			int round;
			if(data->myHeros[i].findBuff("Hided")){
				round = ArriveTime(data->myHeros[i], FindHeroAtkPos(data->myHeros[i], data->target[i]), 
					data->myHeros[i].speed);
			}
			else{
				round = ArriveTime(data->myHeros[i], FindHeroAtkPos(data->myHeros[i], data->target[i]),
					data->myHeros[i].speed);
				if(data->myHeros[i].mp + round * 2 >= 50){
					round += 1;
				}
			}
			if(data->HideTIME[index] >= round+1){
				num++;
			}
		}
	}
	if(num >= 2){
		return true;
	}
	return false;
}
//视野看到目标状态:刺客隐身即将结束，直接进攻
bool Rule_1_2(PUnit& hero){
	return false;
	if(hero.typeId != 2 && !hero.findBuff(0)){
		return false;
	}
	if(data->HideTIME[rankHero(hero)] == 2){
		return true;
	}
	return false;
}
//视野看到目标状态:刺客准备联合进攻
bool Rule_1_3(PUnit& hero){
	if(hero.typeId != 2){
		return false;
	}
	int num = 0;
	for(int i = 0;i < data->myHeros.size();i++){
		if(!data->myHeros[i].findBuff("Reviving"))
		if(data->target[i].id == data->target[rankHero(hero)].id){
			if(ArriveTime(data->myHeros[i], FindHeroAtkPos(data->myHeros[i], data->target[i]),
				data->myHeros[i].speed) <= 4){
					num++;
			}
		}
	}
	if(num >= 2){
		return true;
	}
	return false;
}
//视野看到目标状态:直接进攻
bool Rule_1_4(PUnit& hero){
	return true;
}

//遭遇对方英雄状态：能干掉
bool Rule_2_0(PUnit& hero){
	return false;
}
//遭遇对方英雄状态：能Blink
bool Rule_2_1(PUnit& hero){
	if(hero.typeId == 2 && canUseSkill(hero, 17)){
		return true;
	}
	return false;
}
//遭遇对方英雄状态：跑
bool Rule_2_2(PUnit& hero){
	return true;
}

//战斗状态：战斗
bool Rule_3_0(PUnit& hero){
	return true;
}

//推塔行进状态：
bool Rule_4_0(PUnit& hero){
	return true;
}

//推塔巡逻状态：
bool Rule_5_0(PUnit& hero){
	return true;
}

//推塔战斗状态:
bool Rule_6_0(PUnit& hero){
	return true;
}

//目标死亡状态:塔
bool Rule_7_0(PUnit& hero){
	if(data->target[rankHero(hero)].typeId == 3){
		return true;
	}
	return false;
}
//目标死亡状态:野怪
bool Rule_7_1(PUnit& hero){
	if(data->target[rankHero(hero)].typeId >= 6){
		return true;
	}
	return false;
}
//目标死亡状态:英雄
bool Rule_7_2(PUnit& hero){
	if(data->target[rankHero(hero)].typeId <= 2){
		return true;
	}
	return false;
}

//对方塔边清敌状态:
bool Rule_8_0(PUnit& hero){
	return true;
}

//守塔状态:
bool Rule_9_0(PUnit& hero){
	return true;
}

//濒死状态:能Blink
bool Rule_10_0(PUnit& hero){
	if(canUseSkill(hero, 17)){
		return true;
	}
	return false;
}

//濒死状态:
bool Rule_10_1(PUnit& hero){
	return true;
}

//
bool Rule_11_0(PUnit& hero){
	return true;
}

//Action_0_0：
void Action_0_0(PUnit& hero, PCommand* cmd){
	if(LevelUp(hero, cmd)) return;
	Operation op;
	op.id = hero.id;
	op.typeId = 13;
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
	data->HideTIME[rankHero(hero)] = 9;
}
//Action_0_1:
void Action_0_1(PUnit& hero, PCommand* cmd){
	Action_0_0(hero, cmd);
}
//Action_0_2:
void Action_0_2(PUnit& hero, PCommand* cmd){
	if(LevelUp(hero, cmd)) return;
	Operation op;
	op.id = hero.id;
	op.typeId = 17;
	op.targets.push_back(FindHeroWaitPos(hero, data->target[rankHero(hero)]));
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
}
//Action_0_3:
void Action_0_3(PUnit& hero, PCommand* cmd){
	if(data->already[rankHero(hero)]) return;
	if(LevelUp(hero, cmd)) return;
	Operation op;
	op.id = hero.id;
	findShortestPath(*data->map, hero.pos, FindHeroWaitPos(hero, data->target[rankHero(hero)]), 
		data->block, op.targets);
	if(op.targets.size() == 1){
		if(canUseSkill(hero, 17)){
			for(int i = -9;i <=9;i++)
				for(int j = -9;j <= 9;j++){
					if(i*i+j*j <= 81){
						Pos pos(hero.pos.x + i, hero.pos.y + j);
						if(pos.x >=0 && pos.y >= 0 && data->map->height[pos.x][pos.y] <= 2){
							op.typeId = 17;
							op.targets.push_back(pos);
							cmd->cmds.push_back(op);
							data->already[rankHero(hero)] = true;
							return;
						}
					}
				}
		}
	}
	op.type = "Move";
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
	Pos tmp = FindHeroWaitPos(hero, data->target[rankHero(hero)]);
	output << "Action_0_3:(" << tmp.x << "," << tmp.y << ")\n";
}

//Action_1_0:
void Action_1_0(PUnit& hero, PCommand* cmd){
	if(LevelUp(hero, cmd)) return;
	Operation op;
	op.id = hero.id;
	op.typeId = 13;
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
	data->HideTIME[rankHero(hero)] = 9;
}
//Action_1_1:
void Action_1_1(PUnit& hero, PCommand* cmd){
	if(LevelUp(hero, cmd)) return;
	vector<PUnit> heros;
	for(int i = 0;i < data->myHeros.size();i++){
		if(data->target[i].id == data->target[rankHero(hero)].id){
			int round = ArriveTime(data->myHeros[i], 
				FindHeroWaitPos(data->myHeros[i], data->target[i]), data->myHeros[i].speed);
			if(round == 0){
				heros.push_back(data->myHeros[i]);
			}
		}
	}
	if(BeginAttack(heros, data->target[rankHero(hero)])){
		Operation op;
		op.id = hero.id;
		op.type = "Move";
		findShortestPath(*data->map, hero.pos, FindHeroAtkPos(hero, data->target[rankHero(hero)]),
			data->block, op.targets);
		cmd->cmds.push_back(op);
		data->already[rankHero(hero)] = true;
	}
	else{
		Action_0_3(hero, cmd);
	}
}
//Action_1_2:
void Action_1_2(PUnit& hero, PCommand* cmd){
	if(LevelUp(hero, cmd)) return;
	Operation op;
	op.id = hero.id;
	op.type = "Move";
	findShortestPath(*data->map, hero.pos, FindHeroAtkPos(hero, data->target[rankHero(hero)]),
		data->block, op.targets);
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
}
//Action_1_3:
void Action_1_3(PUnit& hero, PCommand* cmd){
	Action_1_1(hero, cmd);
}
//Action_1_4:
void Action_1_4(PUnit& hero, PCommand* cmd){
	Action_1_2(hero, cmd);
}

//Action_2_0:
void Action_2_0(PUnit& hero, PCommand* cmd){
}
//Action_2_1:
void Action_2_1(PUnit& hero, PCommand* cmd){
	Operation op;
	op.id = hero.id;
	op.typeId = 17;
	for(int i = -9;i <= 9;i++){
		for(int j = -9;j <= 9;j++){
			if(i*i + j*j <= 81){
				if(data->map->height[hero.pos.x + i][hero.pos.y + j] - 
					data->map->height[hero.pos.x][hero.pos.y] >= 2){
					op.targets.push_back(Pos(hero.pos.x+i,hero.pos.y+j));
					cmd->cmds.push_back(op);
					data->already[rankHero(hero)] = true;
					return;
				}
			}
		}
	}
	int max = 0;
	Pos pos;
	for(int i = -9;i <= 9;i++){
		for(int j = -9;j <= 9;j++){
			if(i*i+j*j <= 81){
				int tmp = 0;
				Pos posTmp(hero.pos.x+i, hero.pos.y+j);
				for(int k = 0;k <= data->enemysSeen.size();k++)
					if(dis2(data->enemysSeen[k].pos, hero.pos) <= hero.view){
						tmp += dis2(data->enemysSeen[k].pos, posTmp);
					}
				if(tmp > max){
					max = tmp;
					pos = posTmp;
				}
			}
		}
	}
	op.targets.push_back(pos);
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
}
//Action_2_2:
void Action_2_2(PUnit& hero, PCommand* cmd){
	Operation op;
	op.id = hero.id;
	if(hero.findBuff(0)){
		op.type = "Move";
		findShortestPath(*data->map, hero.pos, data->spring, data->block, op.targets);
		cmd->cmds.push_back(op);
		data->already[rankHero(hero)] = true;
		return;
	}
	if(canUseSkill(hero, 13)){
		op.typeId = 13;
		cmd->cmds.push_back(op);
		data->already[rankHero(hero)] = true;
		data->HideTIME[rankHero(hero)] = 9;
		return;
	}
	op.type = "Move";
	findShortestPath(*data->map, hero.pos, data->spring, data->block, op.targets);
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
	return;
}

//Action_3_0:
void Action_3_0(PUnit& hero, PCommand* cmd){
	Operation op;
	op.id = hero.id;
	op.type = "Attack";
	op.targets.push_back(data->target[rankHero(hero)].pos);
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
}

//Action_4_0:
void Action_4_0(PUnit& hero, PCommand* cmd){
	if(LevelUp(hero, cmd)) return;
	if(data->enemysRating[1] != 2000){
		data->enemysRating[1] = 0;
	}
	else{
		data->enemysRating[0] = 0;
	}
	UpdateTarget(hero);
	if(data->enemysRating[1] == 2000){
		int num = 0;
		for(int i = 0;i < data->myHeros.size();i++){
			if(data->myHeros[i].findBuff("Reviving")){
				num++;
			}
		}
		if(num > 0){
			if(canUseSkill(hero, 13)){
				Operation op;
				op.id = hero.id;
				op.typeId = 13;
				cmd->cmds.push_back(op);
				return;
			}
			Operation op;
			op.id = hero.id;
			op.type = "Move";
			findShortestPath(*data->map, hero.pos, data->spring, data->block, op.targets);
			cmd->cmds.push_back(op);
			return;
		}
	}
//	Action_0_3(hero, cmd);
	Operation op;
	op.id = hero.id;
	int dx[] = {-15,-16,-17,-18,-19};
	int dy[] = {-2,-3,-4,-5,-6};
	int xx[] = {-10,-9,-8,-7,-6};
	int yy[] = {10,11,12,13,13};
	int rate = 1;
	if(data->info->camp == 1){
		rate = -1;
	}
	Pos pos;
	if(data->enemysRating[1] != 2000){
		pos.x = data->target[rankHero(hero)].pos.x + dx[rankHero(hero)] * rate;
		pos.y = data->target[rankHero(hero)].pos.y + dy[rankHero(hero)] * rate;
	}
	else{
		if(dis2(hero.pos, data->spring) <= 4900){
			pos.x = data->enemys[1].pos.x + dx[rankHero(hero)] * rate;
			pos.y = data->enemys[1].pos.y + dy[rankHero(hero)] * rate;
		}
		else{
			pos.x = data->enemys[0].pos.x + xx[rankHero(hero)] * rate;
			pos.y = data->enemys[0].pos.y + yy[rankHero(hero)] * rate;
		}
	}
	findShortestPath(*data->map, hero.pos, pos, 
		data->block, op.targets);
	op.type = "Move";
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
}

//Action_5_0:
void Action_5_0(PUnit& hero, PCommand* cmd){
	//Action_1_1(hero, cmd);
}

//Action_6_0:
void Action_6_0(PUnit& hero, PCommand* cmd){
	output << " id:" << hero.id << " dis:" << dis2(hero.pos, data->target[rankHero(hero)].pos) << " range:" << data->target[rankHero(hero)].range << endl;
	if(data->enemysRating[1] != 2000 && !data->towerAttacked)
		return;
	if(dis2(hero.pos, data->target[rankHero(hero)].pos) > data->target[rankHero(hero)].range){
		if(!hero.findBuff("Hided")){
			output << " buff:no hided\n"; 
			for(int i = 0;i < data->myHeros.size();i++)
				if(data->target[i].id == data->target[rankHero(hero)].id && data->state[i] == 6 
					&& !canUseSkill(data->myHeros[i], 13) && !data->myHeros[i].findBuff("Hided"))
					return;
			output << "\t then hided\n";
			Operation op;
			op.id = hero.id;
			op.typeId = 13;
			cmd->cmds.push_back(op);
			data->already[rankHero(hero)] = true;
			return;
		}
		else{
			output << " buff:hided\n"; 
			for(int i = 0;i < data->myHeros.size();i++)
				if(data->target[i].id == data->target[rankHero(hero)].id && data->state[i] == 6 && !data->myHeros[i].findBuff("Hided"))
					return;
			output << "\t then move\n";
			Operation op;
			op.id = hero.id;
			op.type = "Move";
			findShortestPath(*data->map, hero.pos, FindHeroAtkPos(hero, data->target[rankHero(hero)]), data->block, op.targets);
			cmd->cmds.push_back(op);
		}
		return;
	}
	if(dis2(hero.pos, data->target[rankHero(hero)].pos) > hero.range){
		Operation op;
		op.id = hero.id;
		op.type = "Move";
		findShortestPath(*data->map, hero.pos, FindHeroAtkPos(hero, data->target[rankHero(hero)]), data->block, op.targets);
		cmd->cmds.push_back(op);
		return;
	}
	Action_3_0(hero, cmd);
}

//Action_7_0:
void Action_7_0(PUnit& hero, PCommand* cmd){
	data->enemysRating[rankEnemy(data->target[rankHero(hero)])] = 2000;
	data->target[rankHero(hero)] = PUnit(0,-1,-1,-1,-1,-1);
	data->state[rankHero(hero)] = 0;
	if(data->target[rankHero(hero)].id == data->enemys[1].id){
		int num = 0;
		for(int i = 0;i < data->myHeros.size();i++){
			if(data->myHeros[i].findBuff("Reviving") || data->myHeros[i].hp < 100){
				num++;
			}
		}
		if(num >= 2){
			if(canUseSkill(hero, 17)){
				Operation op;
				op.id = hero.id;
				Pos pos = hero.pos;
				for(int i = -9;i <= 9;i++){
					for(int j = -9;j <= 9;j++){
						if(i*i+j*j <= 81){
							Pos tmp(hero.pos.x+i, hero.pos.y+j);
							if(data->map->height[tmp.x][tmp.y] == 0 && dis2(tmp, data->spring) < dis2(pos, data->spring)){
								pos = tmp;
							}
						}
					}
				}
				op.typeId = 17;
				op.targets.push_back(pos);
				cmd->cmds.push_back(op);
				return;
			}
			if(canUseSkill(hero, 13)){
				Operation op;
				op.id = hero.id;
				op.typeId = 13;
				cmd->cmds.push_back(op);
				return;
			}
			Operation op;
			op.id = hero.id;
			op.type = "Move";
			findShortestPath(*data->map, hero.pos, data->spring, data->block, op.targets);
			cmd->cmds.push_back(op);
			return;
		}
	}
	UpdateUnit(data->target[rankHero(hero)]);
	UpdateState(hero);
	UpdateTarget(hero);
	Rule_Action(hero, cmd);
}
//Action_7_1
void Action_7_1(PUnit& hero, PCommand* cmd){
	bool tmp = true;
	for(int i = 0;i < 2;i++)
		if(data->towers[i].max_hp == 0)
			tmp = false;
	if(data->enemysRating[1] != 2000 && tmp)
		data->flag = true;
	for(int i = 0;i < data->myHeros.size();i++)
		if(data->myHeros[i].level < 2)
			data->flag = false;
	int index = rankEnemy(data->target[rankHero(hero)]);
	int typeId = data->target[rankHero(hero)].typeId;
	if(typeId < 8){
		data->revivingTIME[index] = 20;
	}
	else if(typeId < 10){
		data->revivingTIME[index] = 25;
	}
	else if(typeId < 11){
		data->revivingTIME[index] = 60;
	}
	else{
		data->revivingTIME[index] = 80;
	}
	data->target[rankHero(hero)] = PUnit(0,-1,-1,-1,-1,-1);
	UpdateUnit(data->target[rankHero(hero)]);
	UpdateState(hero);
	UpdateTarget(hero);
	Rule_Action(hero, cmd);
}
//Action_7_2:
void Action_7_2(PUnit& hero, PCommand* cmd){
	data->revivingTIME[rankEnemy(data->target[rankHero(hero)])] = 5 * 
		(data->target[rankHero(hero)].level + 1);
	data->target[rankHero(hero)] = PUnit(0,-1,-1,-1,-1,-1);
	UpdateUnit(data->target[rankHero(hero)]);
	UpdateState(hero);
	UpdateTarget(hero);
	Rule_Action(hero, cmd);
}

//Action_8_0
void Action_8_0(PUnit& hero, PCommand* cmd){
}

//Action_9_0
void Action_9_0(PUnit& hero, PCommand* cmd){
}

//Action_10_0:
void Action_10_0(PUnit& hero, PCommand* cmd){
	Action_2_1(hero, cmd);
}
//Action_10_1:
void Action_10_1(PUnit& hero, PCommand* cmd){
	Action_3_0(hero, cmd);
}
//Action_11_0:
void Action_11_0(PUnit& hero, PCommand* cmd){
	if(canUseSkill(hero, 17)){
		for(int i = 3;i < 8;i++)
			if(data->enemys[i].max_hp > 0 && dis2(data->enemys[i].pos, hero.pos) <= data->enemys[i].range &&
				(!hero.findBuff("Hided") || dis2(hero.pos, data->enemys[1].pos) < data->enemys[1].range)){
				Pos pos = hero.pos;
				for(int j = -9;j < 9;j++)
					for(int k = -9;k < 9;k++)
						if(j*j+k*k <= 81){
							Pos tmp(hero.pos.x+j, hero.pos.y+k);
							if(dis2(tmp, data->spring) < dis2(pos, data->spring) && 
								data->map->height[tmp.x][tmp.y] <= 2)
								pos = tmp;
						}
				Operation op;
				op.id = hero.id;
				op.typeId = 17;
				op.targets.push_back(pos);
				cmd->cmds.push_back(op);
				data->already[rankHero(hero)] = true;
				return;
			}
	}
	if(canUseSkill(hero, 13)){
		Operation op;
		op.id = hero.id;
		op.typeId = 13;
		cmd->cmds.push_back(op);
		data->already[rankHero(hero)] = true;
		return;
	}
	bool flag = true;
	for(int i = 0;i < data->myHeros.size();i++)
		if(!(dis2(data->myHeros[i].pos, data->spring) < 3000 && data->myHeros[i].hp > 200)){
			flag = false;
			break;
		}
	if(flag){
		data->flag = false;
		UpdateState(hero);
		Rule_Action(hero, cmd);
		return;
	}
	Operation op;
	op.id = hero.id;
	op.type = "Move";
	findShortestPath(*data->map, hero.pos, data->spring, data->block, op.targets);
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
}

int forbidden[] = {24,34,25,27,35,28};

//初始化
void Init(){
	data->block.clear();
	data->block.push_back(Player0_spring_pos[0]);
	for(int i = 0;i < 2;i++){
		data->block.push_back(Player0_tower_pos[i]);
		data->block.push_back(Player1_tower_pos[i]);
	}

	if(data->info->camp == 0){
		for(int i = 0;i < 2;i++){
			data->enemysInit.push_back(PUnit(3, 12+i, 1, 1, Player1_tower_pos[i].x, Player1_tower_pos[i].y));
		}
		data->enemysInit.push_back(PUnit(4, 15, 1, 1, Player1_spring_pos[0].x, Player1_spring_pos[0].y));
		for(int i = 0;i < 5;i++)
			data->enemysInit.push_back(PUnit(1, 44+i, 1, 1, Player1_hero_initial_pos[i].x, Player1_hero_initial_pos[i].y));
	}
	else{
		for(int i = 0;i < 2;i++){
			data->enemysInit.push_back(PUnit(3, 10+i, 0, 0, Player0_tower_pos[i].x, Player0_tower_pos[i].y));
		}
		data->enemysInit.push_back(PUnit(4, 14, 0, 0, Player0_spring_pos[0].x, Player0_spring_pos[0].y));
		for(int i = 0;i < 5;i++)
			data->enemysInit.push_back(PUnit(1, 39+i, 0, 0, Player0_hero_initial_pos[i].x, Player0_hero_initial_pos[i].y));
	}
	for(int i = 0;i < 23;i++){
		if(i < 8){
			data->enemysInit.push_back(PUnit(6, 16+i, 2, 2, Dog_pos[i].x, Dog_pos[i].y));
		}
		else if(i < 14){
			data->enemysInit.push_back(PUnit(7, 16+i, 2, 2, Bow_pos[i-8].x, Bow_pos[i-8].y));
		}
		else if(i < 18){
			data->enemysInit.push_back(PUnit(8, 16+i, 2, 2, Wolf_pos[i-14].x, Wolf_pos[i-14].y));
		}
		else if(i < 20){
			data->enemysInit.push_back(PUnit(9, 16+i, 2, 2, StoneMan_pos[i-18].x, StoneMan_pos[i-18].y));
		}
		else if(i < 22){
			data->enemysInit.push_back(PUnit(10, 16+i, 2, 2, Dragon_pos[i-20].x, Dragon_pos[i-20].y));
		}
		else{
			data->enemysInit.push_back(PUnit(11, 16+i, 2, 2, Roshan_pos[0].x, Roshan_pos[0].y));
		}
	}
	for(int i = 0;i < data->enemysInit.size();i++){
		data->enemysRating.push_back(1000);
		data->revivingTIME.push_back(0);
		data->enemysInit[i].max_hp = 0;
		data->enemysInit[i].hp = 0;
		if(data->enemysInit[i].typeId == 3)
			data->enemysInit[i].range = 100;
	}
	data->enemysRating[8+data->info->camp * 4] = 1;
	data->enemysRating[9+data->info->camp * 4] = 1;
	data->enemysRating[28+data->info->camp] = 0;
	output << "Init: Size:" << data->enemysInit.size() << " " << data->enemysRating.size() << " " << data->revivingTIME.size() << endl;
//	data->enemysInit[0].hp = 1000;
//	data->enemysInit[1].hp = 1000;
//	data->enemysInit[2].hp = 2000;
//	data->enemysInit[30].hp = 1200;
	UpdateEnemys();
	for(int i = 0;i < 6;i++){
		PUnit enemy(0,forbidden[i],0,0,0,0);
		data->enemysRating[rankEnemy(enemy)] = 1000;
		output << "Forbidden:" << rankEnemy(enemy) << " Size:" << data->enemysRating.size() << endl;
	}

	data->symbols.push_back(Pos(22,68));
	if(data->info->camp == 0){
		data->spring = Player0_spring_pos[0];
	}
	else{
		data->spring = Player1_spring_pos[0];
	}

	for(int i = 0;i < 5;i++){
		data->target.push_back(PUnit(0,-1,-1,-1,-1,-1));
		cout << data->target[i].id << endl;
	}

	for(int i = 0;i < 5;i++){
		data->HideTIME.push_back(0);
	}
}
//五弓手
void ChooseHero(PCommand* cmd){
	for(int i = 0;i < data->info->units.size();i++){
		const PUnit& unit = data->info->units[i];
		if(unit.camp == data->info->camp && unit.typeId == 5){
			Operation op;
			op.id = unit.id;
			op.type = "ChooseAssassin";
			cmd->cmds.push_back(op);
		}
	}
}
//英雄下标
int rankHero(PUnit& hero){
	for(int i = 0;i < 5;i++)
		if(data->myHeros[i].id == hero.id){
			return i;
		}
	return 0;
}
//敌人下标
int rankEnemy(PUnit& enemy){
	for(int i = 0;i < data->enemys.size();i++)
		if(data->enemys[i].id == enemy.id){
			return i;
		}
	return 0;
}
//更新Enemys
void UpdateEnemys(){
	data->enemys.clear();
	data->enemysSeen.clear();
	output << "UpdateEnemys: Size:" << data->enemysInit.size() << endl;
	for(int i = 0;i < data->enemysInit.size();i++){
		const PUnit* unit = data->info->findUnitById(data->enemysInit[i].id);
		if(unit != NULL){
			data->enemys.push_back(*unit);
			data->enemysSeen.push_back(*unit);
		}
		else
			data->enemys.push_back(data->enemysInit[i]);
	}
}
//评估函数
int Evaluation(vector<PUnit>& heros, PUnit& enemy){
	int result = 1000;
	int size = heros.size();
	if(size == 0)
		return result;
	Pos pos(0,0);
	for(int i = 0;i < heros.size();i++){
		int flag = 0;
		pos = heros[i].pos;
		for(int j = 0;j < heros.size();j++){
			if(dis(pos, heros[j].pos) > 10)
				flag++;
		}
		if(flag < heros.size()/2)
			break;
	}
	int level = 0, speed = 0, hp = 0;
	for(int i = 0;i < size;i++){
		level += heros[i].level;
		speed += heros[i].speed;
		hp += heros[i].hp;
	}
	level /= size;
	speed /= size;
	hp /= size;
	int arrive = dis(pos, enemy.pos) / sqrt(speed);
	if(data->revivingTIME[rankEnemy(enemy)] > arrive)
		return result;
	if(enemy.isHero()){
		if(enemy.level > level + 3)
			return result;
		if(enemy.typeId == 0)
			result = 75;
		else if(enemy.typeId == 1)
			result = 75;
		else if(enemy.typeId == 2)
			result = 75;
	}
	else{
		if(enemy.typeId < 8)
			result = 150;
		else if(enemy.typeId == 8)
			result = 140;
		else if(enemy.typeId == 10)
			result = 120;
	}
	//if(enemy.max_hp >= 1000)
	if(enemy.typeId == 3 || enemy.typeId == 4 || enemy.typeId == 11)
		result = 1000;
	result += arrive * 10;
	result *= data->enemysRating[rankEnemy(enemy)];
	for(int i = 0;i < rankHero(heros[0]);i++){
		if(data->target[i].id == enemy.id){
			result *= 0.5;
		}
	}
	if(result < 1000){
		output << "Evulation: id:" << enemy.id << " Rating:" << data->enemysRating[rankEnemy(enemy)] << " result:" << result << endl;
	}
	if(data->target[rankHero(heros[0])].id == enemy.id && enemy.hp > 0 && !enemy.findBuff("Reviving")){
		return 0;
	}
	return result;
}
//选取敌人
PUnit ChooseEnemy(PUnit& hero){
	int min = 1000;
	int tmp = 0;
	output << "ChooseEnemy: Size:" << data->enemys.size() << endl;
	PUnit result = data->enemys[0];
	vector<PUnit> heros;
	heros.push_back(hero);
	for(int i = 0;i < data->enemys.size();i++){
		tmp = Evaluation(heros, data->enemys[i]);
		if(tmp < min){
			min = tmp;
			result = data->enemys[i];
		}
	}
	if(min == 1000){
		return data->enemys[28+data->info->camp];
	}
	return result;
}

void UpdateUnit(PUnit& unit){
	const PUnit* tmp = data->info->findUnitById(unit.id);
	if(tmp != NULL)
		unit = *tmp;
}

bool isTargetDead(PUnit& hero){
	int index = rankHero(hero);
	if((data->target[index].max_hp > 0) && (data->target[index].hp <= 0)){
		return true;
	}
	if(data->target[index].typeId == 3){
		bool flag = false;
		for(int i = 0;i < data->myHeros.size();i++){
			if((dis2(data->target[index].pos, data->myHeros[i].pos) <= data->myHeros[i].view) 
				&& (data->map->height[data->target[index].pos.x][data->target[index].pos.y] - data->map->height[hero.pos.x][hero.pos.y] < 2)
				&& !data->myHeros[i].findBuff("Reviving")){
				flag = true;
				break;
			}
		}
	//	if(flag && data->target[0].max_hp == 0)
		if(flag && data->target[index].max_hp == 0)
			return true;
	}
	return false;
}

bool isSeen(PUnit& enemy){
	if(enemy.max_hp > 0)
		return true;
	return false;
}

bool LevelUp(PUnit& hero, PCommand* cmd){
	if(hero["Exp"]->val[3] > 0){
		int skill[] = {14,14,14,18,18,18,19,19,19,19};
		Operation op;
		op.id = hero.id;
		op.typeId = skill[data->levelUP[rankHero(hero)]];
		data->levelUP[rankHero(hero)] += 1;
		cmd->cmds.push_back(op);
		data->already[rankHero(hero)] = true;
		return true;
	}
	return false;
}

void UpdateTarget(PUnit& hero){
	data->target[rankHero(hero)] = ChooseEnemy(hero);	
	output << "Target:" << hero.id << "vs" << data->target[rankHero(hero)].id << endl;
}

int ArriveTime(PUnit& hero, Pos pos, int speed){
	int dx = hero.pos.x - pos.x;
	int dy = hero.pos.y - pos.y;
	if(abs(dx) <= 1 && abs(dy) <= 1){
		return 0;
	}
	int distance = sqrt(dx*dx+dy*dy);
	return distance / sqrt(speed);
}

void FindEnemysAble(PUnit& hero, vector<PUnit>& enemys){
	enemys.clear();
	for(int i = 0;i < data->enemysSeen.size();i++){
		if(dis2(data->enemysSeen[i].pos, hero.pos) < data->enemysSeen[i].range && !data->enemysSeen[i].findBuff("Reviving")){
			enemys.push_back(data->enemysSeen[i]);
		}
	}
}

Pos FindHeroAtkPos(PUnit& hero, PUnit& enemy){
	int direction = FindDirection(hero, enemy);
	if(direction == 0){
		int dx[] = {-1,1,2,1,2};
		int dy[] = {2,2,1,0,-1};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else if(direction == 1){
		int dx[] = {-1,1,-2,-1,-2};
		int dy[] = {2,2,1,0,-1};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else if(direction == 2){
		int dx[] = {-2,-2,0,-1,1};
		int dy[] = {1,-1,-1,-2,-2};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else{
		int dx[] = {2,0,2,-1,1};
		int dy[] = {1,-1,-1,-2,-2};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	return enemy.pos;
}

Pos FindHeroWaitPos(PUnit& hero, PUnit& enemy){
	int direction = FindDirection(hero, enemy);
	if(direction == 0){
		int dx[] = {-1,1,3,4,4};
		int dy[] = {4,4,4,1,-1};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else if(direction == 1){
		int dx[] = {-1,1,-3,-4,-4};
		int dy[] = {4,4,4,1,-1};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else if(direction == 2){
		int dx[] = {-4,-4,-2,-1,1};
		int dy[] = {1,-1,-3,-4,-4};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else{
		int dx[] = {4,2,4,-1,1};
		int dy[] = {1,-3,-1,-4,-4};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	return enemy.pos;
}

int FindDirection(PUnit& hero, PUnit& enemy){
	Pos pos;
	for(int i = 0;i < data->myHeros.size();i++){
		pos.x += data->myHeros[i].pos.x;
		pos.y += data->myHeros[i].pos.y;
	}
	pos.x /= data->myHeros.size();
	pos.y /= data->myHeros.size();
	if(enemy.pos.x < pos.x && enemy.pos.y < pos.y)
		return 0;
	else if(enemy.pos.x >= pos.x && enemy.pos.y < pos.y)
		return 1;
	else if(enemy.pos.x >= pos.x && enemy.pos.y >= pos.y)
		return 2;
	else
		return 3;
	return 0;
}

bool canUseSkill(PUnit& hero, int typeId) //判断单位目前是否可以使用某种技能，_typeId为技能类型编号
{
	const PSkill* skill = hero.findSkill(typeId);
	if(skill->level < 0){
		return false;
	}
	if(skill->cd == 0 && hero.mp >= skill->mp){
		return true;
	}
	return false;
}

bool BeginAttack(vector<PUnit>& heros, PUnit& enemy){
	if(heros.size() == 0) return false;
	if(enemy.typeId == 10){
		if(heros[0].level == 0 && heros.size() < 5)
			return false;
		if(heros[0].level < 3 && heros.size() < 4)
			return false;
		return true;
	}
	if(enemy.typeId >= 6 && enemy.typeId != 10){
		if(heros.size() > 1)
			return true;
		return false;
	}
	return false;
}

void UpdateReviving(){
	for(int i = 0;i < data->enemys.size();i++){
		if(isSeen(data->enemys[i]) && !data->enemys[i].findBuff("Reviving")){
			data->revivingTIME[i] = 0;
		}
		if(data->revivingTIME[i] > 0){
			data->revivingTIME[i]--;
		}
	}
}