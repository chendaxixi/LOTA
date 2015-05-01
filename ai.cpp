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
		towerLeft_Enemy(2), HideTIME(){
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
};

//声明函数指针类型
typedef bool (*Update)(PUnit&);
typedef bool (*Rule)(PUnit&);
typedef void (*Action)(PUnit&, PCommand*);

//常量宏定义
#define UpdateCount 11
#define StateCount 11
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

//全局变量
Update updates[UpdateCount] = {Update_0, Update_1, Update_2, Update_3, Update_4, Update_5, Update_6, Update_7, Update_8, Update_9, Update_10};
int updates_State[UpdateCount] = {0,1,2,3,4,5,6,7,8,9,10};
int updateRank[UpdateCount] = {10,9,8,7,6,5,4,3,2,1,0};
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
	{Rule_10_0, Rule_10_1}	
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
	{Action_10_0, Action_10_1}	
	};
int ruleCount[StateCount] = {4,5,3,1,1,1,1,3,1,1,2};
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
	{0,1}
	};
PData* data = new PData();
ofstream output("result.txt");

void player_ai(const PMap &map, const PPlayerInfo &info, PCommand &cmd)
{
	data->SetData(&map, &info);
	output << "round:" << data->info->round << endl;
	Interpert_Input(&cmd);
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
		}
	}
	else{
		for(int i = 0;i < 2;i++){
			data->towers.push_back(PUnit(3, 12+i, 1, 1, Player1_tower_pos[i].x, Player1_tower_pos[i].y));
			data->towers[i].max_hp = 0;
			UpdateUnit(data->towers[i]);
		}
	}

	output << "revivingTIMEsize:" << data->revivingTIME.size() << " " << data->enemys.size() << endl;
	for(int i = 0;i < data->enemys.size();i++){
		if(data->revivingTIME[i] > 0){
			data->revivingTIME[i]--;
		}
	}
	
	for(int i = 0;i < data->myHeros.size();i++){
		UpdateUnit(data->target[i]);
		UpdateState(data->myHeros[i]);
		UpdateTarget(data->myHeros[i]);
		if(data->HideTIME[i] > 0){
			data->HideTIME[i]--;
		}
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
}
//状态：推塔行进状态
bool Update_4(PUnit& hero){
	int index = rankHero(hero);
	if(data->state[index] == 4 && data->target[index].typeId == 3 
		&& !(dis2(data->target[index].pos, hero.pos) <= hero.view && dis2(data->target[index].pos, hero.pos) > data->target[index].range)){
			return true;
	}
	bool flag = true;
	for(int i = 0;i < data->myHeros.size();i++){
		if(data->myHeros[i].level < 3 || data->myHeros[i].hp < 105){
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
	if(data->state[index] == 4 && dis2(data->target[index].pos, hero.pos) <= hero.view && dis2(data->target[index].pos, hero.pos) > data->target[index].range){
		return true;
	}
	return false;
}
//状态：推塔战斗状态
bool Update_6(PUnit& hero){
	if(data->state[rankHero(hero)] == 6){
		return true;
	}
	for(int i = 0;i < data->myHeros.size();i++){
		if(data->state[i] != 5 && data->state[i] != 6){
			return false;
		}
	}
	return true;
}
//状态：目标死亡状态
bool Update_7(PUnit& hero){
	return isTargetDead(data->target[rankHero(hero)]);	
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

//寻怪奔走状态：刺客一次隐身可至
bool Rule_0_0(PUnit& hero){
	if(hero.typeId != 2 || hero.findBuff(0)){
		return false;
	}
	output << "!!!!!!!!!!!" << data->info->round << " " << hero.findBuff(0) << endl;
	if(!hero.canUseSkill(13)){
		return false;
	}
	double HideSpeed[] = {1.41, 1.58, 1.73};
	int round = ArriveTime(hero, FindHeroAtkPos(hero, data->target[rankHero(hero)]), 
		hero.speed * HideSpeed[data->levelUP[rankHero(hero)]-1]);
	if(round < 8){
		return true;
	}
	return false;
}
//寻怪奔走状态：刺客二次隐身可至
bool Rule_0_1(PUnit& hero){
	if(hero.typeId != 2 || hero.findBuff(0)){
		return false;
	}
	if(!hero.canUseSkill(13)){
		return false;
	}
	if(hero.mp < 60){
		return false;
	}
	if(data->info->round < 50){
		return true;
	}
	int round = ArriveTime(hero, FindHeroAtkPos(hero, data->target[rankHero(hero)]), hero.speed);
	return false;
}
//寻怪奔走状态：刺客Blink可至
bool Rule_0_2(PUnit& hero){
	if(hero.typeId != 2){
		return false;
	}
	if(!hero.canUseSkill(17)){
		return false;
	}
	Pos pos = FindHeroAtkPos(hero, data->target[rankHero(hero)]);
	if(abs(data->map->height[hero.pos.x][hero.pos.y] - data->map->height[pos.x][pos.y]) >= 2){
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
	if(hero.findBuff(0)){
		return false;
	}
	if(!hero.canUseSkill(13)){
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
		if(data->target[i].id == data->target[index].id){
			int round;
			if(data->myHeros[i].findBuff(0)){
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
}
//视野看到目标状态:刺客隐身即将结束，直接进攻
bool Rule_1_2(PUnit& hero){
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
	if(hero.typeId == 2 && hero.canUseSkill(17)){
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
	if(hero.canUseSkill(17)){
		return true;
	}
	return false;
}

//濒死状态:
bool Rule_10_1(PUnit& hero){
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
	if(LevelUp(hero, cmd)) return;
	Operation op;
	op.id = hero.id;
	op.type = "Move";
	findShortestPath(*data->map, hero.pos, FindHeroWaitPos(hero, data->target[rankHero(hero)]), 
		data->block, op.targets);
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
	bool flag = true;
	for(int i = 0;i < data->myHeros.size();i++){
		if(data->target[i].id == data->target[rankHero(hero)].id){
			int round = ArriveTime(data->myHeros[i], 
				FindHeroWaitPos(data->myHeros[i], data->target[i]), data->myHeros[i].speed);
			if(round != 0){
				flag = false;
				Action_0_3(hero, cmd);
			}
		}
	}
	if(flag){
		Operation op;
		op.id = hero.id;
		op.type = "Move";
		findShortestPath(*data->map, hero.pos, FindHeroAtkPos(hero, data->target[rankHero(hero)]),
			data->block, op.targets);
		cmd->cmds.push_back(op);
		data->already[rankHero(hero)] = true;
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
	if(hero.canUseSkill(13)){
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
	if(data->towerLeft_Enemy == 2){
		data->target[rankHero(hero)] = data->enemys[1];
		data->enemysRating[1] = 0;
	}
	else{
		data->target[rankHero(hero)] = data->enemys[0];
		data->enemysRating[0] = 0;
	}
//	Action_0_3(hero, cmd);
	Operation op;
	op.id = hero.id;
	op.type = "Move";
	int dx[] = {-3,-1,0,1,3};
	int dy[] = {-10,-11,-11,-11,-10};
	Pos pos;
	if(data->towerLeft_Enemy == 2){
		pos.x = data->target[rankHero(hero)].pos.x + dx[rankHero(hero)];
		pos.y = data->target[rankHero(hero)].pos.y + dy[rankHero(hero)];
	}
	else{
		pos.x = data->target[rankHero(hero)].pos.x - dx[rankHero(hero)];
		pos.y = data->target[rankHero(hero)].pos.y - dy[rankHero(hero)];
	}
	findShortestPath(*data->map, hero.pos, pos, 
		data->block, op.targets);
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
}

//Action_5_0:
void Action_5_0(PUnit& hero, PCommand* cmd){
	//Action_1_1(hero, cmd);
}

//Action_6_0:
void Action_6_0(PUnit& hero, PCommand* cmd){
	if(dis2(hero.pos, data->target[rankHero(hero)].pos) > data->target[rankHero(hero)].range){
		if(!hero.findBuff(0) && hero.canUseSkill(13)){
			Operation op;
			op.id = hero.id;
			op.typeId = 13;
			cmd->cmds.push_back(op);
		}
		else{
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
	data->towerLeft_Enemy -= 1;
	data->enemysRating[rankEnemy(data->target[rankHero(hero)])] = 1000;
	data->target[rankHero(hero)] = PUnit(0,-1,-1,-1,-1,-1);
	UpdateUnit(data->target[rankHero(hero)]);
	UpdateState(hero);
	UpdateTarget(hero);
	Rule_Action(hero, cmd);
}
//Action_7_1
void Action_7_1(PUnit& hero, PCommand* cmd){
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
	}
	data->enemysRating[8] = 1;
	data->enemysRating[9] = 1;
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
}
//敌人下标
int rankEnemy(PUnit& enemy){
	for(int i = 0;i < data->enemys.size();i++)
		if(data->enemys[i].id == enemy.id){
			return i;
		}
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
			if((dis2(data->target[index].pos, data->myHeros[i].pos) <= data->myHeros[i].view) && !data->myHeros[i].findBuff("Reviving")){
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
		if(dis2(data->enemysSeen[i].pos, hero.pos) < data->enemysSeen[i].range){
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
}

Pos FindHeroWaitPos(PUnit& hero, PUnit& enemy){
	int direction = FindDirection(hero, enemy);
	if(direction == 0){
		int dx[] = {-1,1,3,3,3};
		int dy[] = {3,3,3,1,-1};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else if(direction == 1){
		int dx[] = {-1,1,-3,-3,-3};
		int dy[] = {3,3,3,1,-1};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else if(direction == 2){
		int dx[] = {-3,-3,-1,-3,1};
		int dy[] = {1,-1,-3,-3,-3};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
	else{
		int dx[] = {3,1,3,-1,3};
		int dy[] = {1,-3,-1,-3,-3};
		int index = rankHero(hero);
		return Pos(enemy.pos.x+dx[index],enemy.pos.y+dy[index]);
	}
}

int FindDirection(PUnit& hero, PUnit& enemy){
	if(enemy.pos.x < hero.pos.x && enemy.pos.y < hero.pos.y)
		return 0;
	else if(enemy.pos.x >= hero.pos.x && enemy.pos.y < hero.pos.y)
		return 1;
	else if(enemy.pos.x >= hero.pos.x && enemy.pos.y >= hero.pos.y)
		return 2;
	else
		return 3;
}