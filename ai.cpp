/*
* Copyright 2015.215B工作室
* All rights reserved.
*
* 文件名称：ai.cpp
* 摘	要：LOTA
*
* 当前版本：1.1
* 作	者: chendaxixi/陈少滨
* 完成日期：2015年4月14日
*
* 取代版本：1.0
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
	PData(const PMap* m = NULL, const PPlayerInfo* i = NULL):map(m), info(i), state(-1), 
		myHeros(), block(), enemys(), target(), enemysInit(), blocksAdd(0), revivingTIME(), enemysRating(), symbols(), towers(), enemyDying(0),
		towerLeft_Enemy(2){
		for(int i = 0;i < 5;i++){
			already[i] = false;
			levelUP[i] = 0;
			enemyExist[i] = false;
			firstAppearing[i] = true;
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
	int state;
	vector<PUnit> myHeros;
	vector<PUnit> enemys;
	vector<PUnit> enemysInit;
	vector<PUnit> target;
	vector<PUnit> towers;
	vector<Pos> block;
	vector<Pos> symbols;
	vector<int> enemysRating;
	vector<int> revivingTIME;
	Pos spring;
	bool already[5];
	int towerLeft_Enemy;
	int blocksAdd;
	int levelUP[5];
	int enemyDying;
	bool enemyExist[5];
	bool firstAppearing[5];
};

//声明函数指针类型
typedef bool (*Update)(vector<PUnit>&);
typedef bool (*Rule)(vector<PUnit>&);
typedef void (*Action)(vector<PUnit>&, PCommand*);

//常量宏定义
#define UpdateCount 7
#define StateCount 7
#define RuleCount 4

//Update_State声明
bool Update_0(vector<PUnit>&);
bool Update_1(vector<PUnit>&);
bool Update_2(vector<PUnit>&);
bool Update_3(vector<PUnit>&);
bool Update_4(vector<PUnit>&);
bool Update_5(vector<PUnit>&);
bool Update_6(vector<PUnit>&);
//Rule声明
bool Rule_0_0(vector<PUnit>&);
bool Rule_1_0(vector<PUnit>&);
bool Rule_2_0(vector<PUnit>&);
bool Rule_2_1(vector<PUnit>&);
bool Rule_2_2(vector<PUnit>&);
bool Rule_2_3(vector<PUnit>&);
bool Rule_3_0(vector<PUnit>&);
bool Rule_3_1(vector<PUnit>&);
bool Rule_3_2(vector<PUnit>&);
bool Rule_4_0(vector<PUnit>&);
bool Rule_5_0(vector<PUnit>&);
bool Rule_6_0(vector<PUnit>&);
//Action声明
void Action_0_0(vector<PUnit>&, PCommand*);
void Action_1_0(vector<PUnit>&, PCommand*);
void Action_2_0(vector<PUnit>&, PCommand*);
void Action_2_1(vector<PUnit>&, PCommand*);
void Action_2_2(vector<PUnit>&, PCommand*);
void Action_2_3(vector<PUnit>&, PCommand*);
void Action_3_0(vector<PUnit>&, PCommand*);
void Action_3_1(vector<PUnit>&, PCommand*);
void Action_3_2(vector<PUnit>&, PCommand*);
void Action_4_0(vector<PUnit>&, PCommand*);
void Action_5_0(vector<PUnit>&, PCommand*);
void Action_6_0(vector<PUnit>&, PCommand*);
//其他函数
void Interpert_Input(PCommand* cmd);
void Init();
void ChooseHero(PCommand* cmd);
void UpdateData(PCommand* cmd);
int rankHero(PUnit& hero);
void UpdateState(vector<PUnit>& heros);
void Rule_Action(vector<PUnit>& heros, PCommand*);
int Evaluation(vector<PUnit>& heros, PUnit& enemy);
void UpdateEnemys();
PUnit ChooseEnemy(vector<PUnit>& heros);
PSkill ChooseSkill(PUnit& hero, PUnit& enemy);
void MoveBack(PUnit& hero, PUnit& enemy, PCommand* cmd);
void UpdateUnit(PUnit& unit);
int rankEnemy(PUnit& enemy);
const PArg* Hp(PUnit& hero);

//全局变量
Update updates[UpdateCount] = {Update_0, Update_1, Update_2, Update_3, Update_4, Update_5, Update_6};
int updates_State[UpdateCount] = {0, 1, 2, 3, 4, 5, 6};
int updateRank[UpdateCount] = {0,1,6,5,4,3,2};
Rule rules[StateCount][RuleCount] = {
	{Rule_0_0},
	{Rule_1_0},
	{Rule_2_0, Rule_2_1, Rule_2_2, Rule_2_3},
	{Rule_3_0, Rule_3_1, Rule_3_2},
	{Rule_4_0},
	{Rule_5_0},
	{Rule_6_0}
	};
Action actions[StateCount][RuleCount] = {
	{Action_0_0},
	{Action_1_0},
	{Action_2_0, Action_2_1, Action_2_2, Action_2_3},
	{Action_3_0, Action_3_1, Action_3_2},
	{Action_4_0},
	{Action_5_0},
	{Action_6_0}
	};
int ruleCount[StateCount] = {1, 1, 4, 3, 1, 1, 1};
int ruleRank[StateCount][RuleCount] ={
	{0},
	{0},
	{3,0,1,2},
	{0,1,2},
	{0},
	{0},
	{0}
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
	UpdateData(cmd);
	Rule_Action(data->myHeros, cmd);
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

	for(int i = 0;i < 5;i++){
		data->already[i] = false;
	}

	if(data->info->round > 0){
		UpdateEnemys();
		data->target.clear();
		data->target.push_back(ChooseEnemy(data->myHeros));
		data->blocksAdd = 0;
		for(int i = 0;i < data->enemys.size();i++){
	//		if(data->enemys[i].max_hp > 0){
			output << "hpSize:" << Hp(data->enemys[i])->val.size() << endl;
			if(Hp(data->enemys[i])->val[1] > 0){
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
	}

	output << "revivingTIMEsize:" << data->revivingTIME.size() << " " << data->enemys.size() << endl;
	for(int i = 0;i < data->enemys.size();i++)
		if(data->revivingTIME[i] > 0){
			data->revivingTIME[i]--;
		}
	
	UpdateState(data->myHeros);
}
//更新State
void UpdateState(vector<PUnit>& heros){
	if((heros.size() == 0) && (data->info->round > 0)){
		data->state = -1;
		return;
	}
	for(int i = 0;i < UpdateCount;i++)
		if(updates[updateRank[i]](heros)){
			data->state = updates_State[updateRank[i]];
			output << "Updata:" << i << " state:" << data->state << endl;
			break;
		}
}
//Rule-Action规则对应
void Rule_Action(vector<PUnit>& heros, PCommand* cmd){
	if(data->state == -1) return;
	for(int i = 0;i < ruleCount[data->state];i++)
		if(rules[data->state][ruleRank[data->state][i]](heros)){
			output << "Rule:" << data->state << "_" << i << endl;
			actions[data->state][ruleRank[data->state][i]](heros, cmd);
			break;
		}
}

//状态转变：开局状态
bool Update_0(vector<PUnit>& heros){
	if(data->info->round == 0)
		return true;
	return false;
}
//状态转变：技能升级状态
bool Update_1(vector<PUnit>& heros){
	if(heros.size() == 0) return false;
	for(int i = 0;i < heros.size();i++){
		output << "ExpSize:" << heros[i]["Exp"]->val.size() << " id:" << heros[i].id << endl;
		if(heros[i]["Exp"]->val[3] > 0)
			return true;
	}
	return false;
}
//状态转变：打野状态
bool Update_2(vector<PUnit>& heros){
	if(heros.size() == 0) return false;
	return true;
}
//状态转变：战斗状态
bool Update_3(vector<PUnit>& heros){
	if(heros.size() == 0) return false;
	//if((data->target.size() > 0) && (data->target[0].max_hp > 0))
	if((data->target.size() > 0) && (Hp(data->target[0])->val[1] > 0)){
		output << "U3:ValSize:" << Hp(data->target[0])->val.size() << endl;
		return true;
	}
	return false;
}
//状态转变：推塔状态
bool Update_4(vector<PUnit>& heros){
	if(heros.size() == 0) return false;
	if(data->state == 3)
		return false;
	int level = 0;
	for(int i = 0;i < heros.size();i++)
		level += heros[i].level;
	level /= heros.size();
	int hp = 0;
	for(int i = 0;i < heros.size();i++)
	//	hp += heros[i]["Hp"]->val[0];
		hp += Hp(heros[i])->val[0];
	if((level >= 2) && (hp >= 125*heros.size()))
		return true;
	return false;
}
//状态转变：目标死亡状态
bool Update_5(vector<PUnit>& heros){
	if(heros.size() == 0) return false;
	if(data->target.size() == 0)
		return false;
//	if((data->target[0].max_hp > 0) && (data->target[0].hp <= 0))
	output << "U5:ValSize:" << Hp(data->target[0])->val.size() << endl;
	if((Hp(data->target[0])->val[1] > 0) && (Hp(data->target[0])->val[0] <= 0))
		return true;
	if(data->target[0].typeId == 3){
		bool flag = false;
		for(int i = 0;i < heros.size();i++){
			if((dis2(data->target[0].pos, heros[i].pos) <= heros[i].view) && !heros[i].findBuff("Reviving")){
				flag = true;
				break;
			}
		}
	//	if(flag && data->target[0].max_hp == 0)
		if(flag && Hp(data->target[0])->val[1] == 0)
			return true;
	}
	return false;
}
//状态转变：守塔状态
bool Update_6(vector<PUnit>& heros){
	output << "U6: " << data->towerLeft_Enemy << " " <<  data->towers.size() << endl;
	if(heros.size() == 0) return false;
	if(data->towerLeft_Enemy > 1) return false;
	if(data->towers.size() < 2) return false;
	bool flag = false;
	for(int i = 0;i < 2;i++){
		if(data->towers[i].max_hp == 0)
			flag = true;
	}
	return flag;
}

//开局状态：round==0
bool Rule_0_0(vector<PUnit>& heros){
	return true;
}
//Action:初始化一些数据；选取英雄
void Action_0_0(vector<PUnit>& heros, PCommand* cmd){
	Init();
	ChooseHero(cmd);
}

//技能升级状态：
bool Rule_1_0(vector<PUnit>& heros){
	return true;
}
//Action:技能升级，其他英雄继续
void Action_1_0(vector<PUnit>& heros, PCommand* cmd){
	int skill_0[] = {12,12,12,8,8,8,10,10,10,19};
	int skill_1[] = {12,8,8,8,12,12,10,10,10,19};
	int skill_2[] = {12,10,10,10,8,12,12,8,8,19};
	int* skill[] = {skill_0, skill_0, skill_1, skill_1, skill_2};
	vector<PUnit> herosLeft;
	for(int i = 0;i < heros.size();i++)
		if(heros[i]["Exp"]->val[3] > 0){
			Operation op;
			op.id = heros[i].id;
			op.typeId = skill[i][data->levelUP[i]];
			data->levelUP[i] += 1;
			cmd->cmds.push_back(op);
			data->already[rankHero(heros[i])] = true;
		}
		else
			herosLeft.push_back(heros[i]);
	data->state = -1;
	UpdateState(herosLeft);
	Rule_Action(herosLeft, cmd);
}

//寻怪状态：集体寻怪
bool Rule_2_0(vector<PUnit>& heros){
	if(data->target.size() == 2)
		return false;
	return true;
}
//Action:
void Action_2_0(vector<PUnit>& heros, PCommand* cmd){
	output << "Action_2_0:Begin\n";
	if(data->target.size() == 0){
		data->target.push_back(ChooseEnemy(heros));
	}
	output << "Target:" << data->target[0].id << endl;
	for(int i = 0;i < heros.size();i++)
		if(!data->already[rankHero(heros[i])]){
			Operation op;
			op.id = heros[i].id;
			op.type = "Move";
		/*	if((data->state == 4) && (data->towerLeft_Enemy == 1)){
				output << "Symbols:" << data->symbols.size() << endl;
				vector<Pos> symbols;
				symbols.push_back(heros[i].pos);
				for(int j = 0;j < data->symbols.size();j++)
					symbols.push_back(data->symbols[j]);
				symbols.push_back(Pos(data->target[0].pos.x-1,data->target[0].pos.y));
				findShortestPathWithSymbols(*data->map, symbols, data->block, op.targets);
			}
			else*/
				findShortestPath(*data->map, heros[i].pos, data->target[0].pos, data->block, op.targets);
			output << "PathSize:" << op.targets.size() << " id:"<<  heros[i].id << endl;
			if(op.targets.size() == 1){
				data->enemysRating[rankEnemy(data->target[0])] = 1000;
			}
			cmd->cmds.push_back(op);
			data->already[rankHero(heros[i])] = true;
		}
}

//寻怪状态：2-3靠拢寻怪
bool Rule_2_1(vector<PUnit>& heros){
	return false;
}
//Action
void Action_2_1(vector<PUnit>& heros, PCommand* cmd){
}

//寻怪状态：2-3分散寻怪
bool Rule_2_2(vector<PUnit>& heros){
	return false;
}
//Action
void Action_2_2(vector<PUnit>& heros, PCommand* cmd){
}

//寻怪状态：开局刷大龙
bool Rule_2_3(vector<PUnit>& heros){
	return true;
}
//Action
void Action_2_3(vector<PUnit>& heros, PCommand* cmd){
	output << "RatingSize:" << data->enemysRating.size() << " need:" << 28+data->info->camp << endl;
	data->enemysRating[28+data->info->camp] = 0;
	Action_2_0(heros, cmd);
}

//战斗状态：野怪
bool Rule_3_0(vector<PUnit>& heros){
	return (data->target[0].camp == 2);
}
//Action
void Action_3_0(vector<PUnit>& heros, PCommand* cmd){
	output << "Action_3_0:Begin: HeroSize:" << heros.size() << " Target:" << data->target.size() << ":" << data->target[0].id << endl;
	for(int i = 0;i < heros.size();i++)
		if(!data->already[rankHero(heros[i])]){
			if(dis2(data->target[0].pos, heros[i].pos) <= heros[i].range){
				output << "Attack: id:" << heros[i].id << endl;
				PSkill skill = ChooseSkill(heros[i], data->target[0]);
				if(skill.cd == 0){
					Operation op;
					op.id = heros[i].id;
					op.typeId = skill.typeId;
					if(skill.needTarget())
						op.targets.push_back(data->target[0].pos);
					cmd->cmds.push_back(op);
					data->already[rankHero(heros[i])] = true;
				}
				else{
					MoveBack(heros[i], data->target[0],cmd);
				}
			}
			else{
				Operation op;
				op.id = heros[i].id;
				op.type = "Move";
			/*	if((data->state == 4) && (data->towerLeft_Enemy == 1)){
					output << "Symbols:" << data->symbols.size() << endl;
					vector<Pos> symbols;
					symbols.push_back(heros[i].pos);
					for(int j = 0;j < data->symbols.size();j++)
						symbols.push_back(data->symbols[j]);
					symbols.push_back(Pos(data->target[0].pos.x-1,data->target[0].pos.y));
					findShortestPathWithSymbols(*data->map, symbols, data->block, op.targets);
				}
				else */
					findShortestPath(*data->map, heros[i].pos, data->target[0].pos, data->block, op.targets);
				output << "Move: id" << heros[i].id << " Size:" << op.targets.size() << endl;
				cmd->cmds.push_back(op);
				data->already[rankHero(heros[i])] = true;
			}
		}
}

//战斗状态：英雄
bool Rule_3_1(vector<PUnit>& heros){
	if(data->target.size() == 0) return false;
	return data->target[0].isHero();
}
//Action
void Action_3_1(vector<PUnit>& heros, PCommand* cmd){
	Action_3_0(heros, cmd);
}

//战斗状态：防御塔
bool Rule_3_2(vector<PUnit>& heros){
	if(data->target.size() == 0) return false;
	return data->target[0].typeId == 3;
}
//Action
void Action_3_2(vector<PUnit>& heros, PCommand* cmd){
	Action_3_0(heros, cmd);
}

//推塔状态：
bool Rule_4_0(vector<PUnit>& heros){
	return true;
}
//Action
void Action_4_0(vector<PUnit>& heros, PCommand* cmd){
	output << "Action_4_0:Begin:Tower" << data->towerLeft_Enemy << endl;;
	data->target.clear();
	if(data->towerLeft_Enemy == 2){
		data->target.push_back(data->enemys[1]);
		data->enemysRating[1] = 0;
	}
	else{
		data->target.push_back(data->enemys[0]);
		data->enemysRating[0] = 0;
	}
	UpdateUnit(data->target[0]);
	output << "Target:" << data->target.size() << ":" <<  data->target[0].id << endl;
	//if(data->target[0].max_hp == 0){
	output << "ValSize:" << Hp(data->target[0])->val.size() << endl;
	int dying = 0, hp = 0, num = 0;
	for(int i = 0;i < heros.size();i++){
		if(heros[i].findBuff("Reviving")){
			dying++;
		}
		else{
			hp += heros[i].hp;
			num += 1;
		}
	}
	if((data->towerLeft_Enemy == 1) && ((num < 4) || (hp / num < 100))){
		for(int i = 0;i < heros.size();i++){
			Operation op;
			op.id = heros[i].id;
			op.type = "Move";
			findShortestPath(*data->map, heros[i].pos, data->spring, data->block, op.targets);
			cmd->cmds.push_back(op);
		}
		return;
	}
	for(int i = 0;i < heros.size();i++){
		if(!heros[i].findBuff("Reviving") && (heros[i].hp < 70) && (heros[i].mp < 70)){
			Operation op;
			op.id = heros[i].id;
			op.type = "Move";
			op.targets.clear();
			findShortestPath(*data->map, heros[i].pos, data->spring, data->block, op.targets);
			cmd->cmds.push_back(op);
			data->already[rankHero(heros[i])] = true;
		}
	}
	PUnit tmp = data->target[0];
	for(int i = 0;i < 5;i++){
		const PUnit& unit = data->enemys[3+i];
		if(dis2(unit.pos, data->target[0].pos) < 144){
			if(unit.hp <= tmp.hp){
				tmp = unit;
			}
		}
	}
	data->target[0] = tmp;
	if(Hp(data->target[0])->val[1] == 0){
		Action_2_0(heros, cmd);
	}
	else{
		Action_3_0(heros, cmd);
	}
}

//目标死亡状态：
bool Rule_5_0(vector<PUnit>& heros){
	return true;
}
//Action
void Action_5_0(vector<PUnit>& heros, PCommand* cmd){
	output << "Action_5_0:Begin:Target:" << data->target.size() << ":" << data->target[0].id << endl;
	int index = rankEnemy(data->target[0]);
	int typeId = data->target[0].typeId;
	output << "Index:" << index << " Size:" << data->revivingTIME.size() << endl;
	if(typeId < 3){
		data->revivingTIME[index] = 5*(data->target[0].level+1);
	}
	else if(typeId < 5){
		data->revivingTIME[index] = 2000;
		if(typeId == 3){
			data->towerLeft_Enemy -= 1;
		}
	}
	else if(typeId < 8){
		data->revivingTIME[index] = 20;
	}
	else if(typeId < 10){
		data->revivingTIME[index] = 25;
	}
	else if(typeId < 11){
		data->revivingTIME[index] = 60;
		data->enemysRating[index] = 1;
	}
	else{
		data->revivingTIME[index] = 80;
	}
	data->target.clear();
	data->target.push_back(ChooseEnemy(heros));
	UpdateState(heros);
	Rule_Action(heros, cmd);
}

//守塔状态：
bool Rule_6_0(vector<PUnit>& heros){
	return true;
}
void Action_6_0(vector<PUnit>& heros, PCommand* cmd){
	int eTmp = 0;
	for(int i = 0;i < 5;i++){
		if(data->enemys[3+i].max_hp == 0){
			eTmp++;
		}
	}
	int exist = 0;
	for(int i = 0;i < heros.size();i++){
		if(!heros[i].findBuff("Reviving")){
			exist += 1;
		}
	}
	output << "Action6: Dying:" << data->enemyDying << " Exist:" << exist << endl;
	if((data->info->round > 950) || ((exist > 2) && (eTmp < 2))){
		data->target.clear();
		if(data->towerLeft_Enemy == 2){
			data->target.push_back(data->enemys[1]);
			data->enemysRating[1] = 0;
		}
		else{
			data->target.push_back(data->enemys[0]);
			data->enemysRating[0] = 0;
		}
		UpdateUnit(data->target[0]);
		if(Hp(data->target[0])->val[1] == 0){
			Action_2_0(heros, cmd);
		}
		else{
			Action_3_0(heros, cmd);
		}
		return;
	}
	PUnit tower = data->towers[0];
	if(tower.max_hp == 0){
		tower = data->towers[1];
	}
	bool flag = false;
	bool enemys[5];
	for(int i = 0;i < 5;i++){
		enemys[i] = false;
	}
	for(int i = 0;i < heros.size();i++){
		for(int j = 0;j < 5;j++){
			if(dis2(data->enemys[3+j].pos, heros[i].pos) < heros[i].range){
				flag = true;
				enemys[j] = true;
			}
		}
		Operation op;
		op.id = heros[i].id;
		op.type = "Move";
		findShortestPath(*data->map, heros[i].pos, tower.pos, data->block, op.targets);
		cmd->cmds.push_back(op);
		if(!heros[i].findBuff("Reviving") && (heros[i].hp < 70) && (heros[i].mp < 70)){
			op.targets.clear();
			findShortestPath(*data->map, heros[i].pos, data->spring, data->block, op.targets);
			cmd->cmds.push_back(op);
			data->already[rankHero(heros[i])] = true;
		}
	}
	if(flag){
		PUnit tmp = data->enemys[3];
		int hp = 1000;
		for(int i = 0;i < 5;i++){
			if(enemys[i]){
				if(data->enemys[3+i].hp < hp){
					tmp = data->enemys[3+i];
					hp = data->enemys[3+i].hp;
				}
			}
		}
		data->target.clear();
		data->target.push_back(tmp);
		Action_3_0(heros, cmd);
	}
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
		data->enemysRating.push_back(1);
		data->revivingTIME.push_back(0);
		PArg arg(2);
		if(i < 2)
			arg.val.push_back(1000);
		else if(i == 2)
			arg.val.push_back(2000);
		else if(i == 30)
			arg.val.push_back(1200);
		else 
			arg.val.push_back(0);
		arg.val.push_back(0);
		data->enemysInit[i].args.push_back(arg);
	}
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
}
//五弓手
void ChooseHero(PCommand* cmd){
	for(int i = 0;i < data->info->units.size();i++){
		const PUnit& unit = data->info->units[i];
		if(unit.camp == data->info->camp && unit.typeId == 5){
			Operation op;
			op.id = unit.id;
			op.type = "ChooseArcher";
			cmd->cmds.push_back(op);
		}
	}
}
//英雄下标
int rankHero(PUnit& hero){
	output << "RankHero: " << data->myHeros.size() << " id:" << hero.id << endl;
	for(int i = 0;i < 5;i++)
		if(data->myHeros[i].id == hero.id){
			output << " Result:" << i << endl;
			return i;
		}
}
//敌人下标
int rankEnemy(PUnit& enemy){
	output << "RankEnemy: " << data->enemys.size() << " id:" << enemy.id << endl;
	for(int i = 0;i < data->enemys.size();i++)
		if(data->enemys[i].id == enemy.id){
			output << " Result:" << i << endl;
			return i;
		}
}
//更新Enemys
void UpdateEnemys(){
	data->enemys.clear();
	output << "UpdateEnemys: Size:" << data->enemysInit.size() << endl;
	for(int i = 0;i < data->enemysInit.size();i++){
		const PUnit* unit = data->info->findUnitById(data->enemysInit[i].id);
		if(unit != NULL){
			data->enemys.push_back(*unit);
		}
		else
			data->enemys.push_back(data->enemysInit[i]);
	}
	output << " resultSize:" << data->enemys.size() << endl;
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
		hp += Hp(heros[i])->val[0];
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
	output << "Evaluation:ValSize:" << Hp(enemy)->val.size() << endl;
	if(Hp(enemy)->val[1] >= 1000)
		result = 1000;
	result += arrive * 10;
	result *= data->enemysRating[rankEnemy(enemy)];
	return result;
}
//选取敌人
PUnit ChooseEnemy(vector<PUnit>& heros){
	int min = 1000;
	int tmp = 0;
	output << "ChooseEnemy: Size:" << data->enemys.size() << endl;
	PUnit result = data->enemys[0];
	for(int i = 0;i < data->enemys.size();i++){
		tmp = Evaluation(heros, data->enemys[i]);
		if(tmp < min){
			min = tmp;
			result = data->enemys[i];
		}
	}
	return result;
}
//风筝走位
void MoveBack(PUnit& hero, PUnit& enemy, PCommand* cmd){
	Operation op;
	op.id = hero.id;
	op.type = "Move";
	vector<PUnit> enemys;
	for(int i = 0;i < data->enemys.size();i++){
		if(dis2(hero.pos, data->enemys[i].pos) < data->enemys[i].range){
			enemys.push_back(data->enemys[i]);
		}
	}
	Pos result = hero.pos;
	int num = enemys.size();
	for(int i = -5;i < 5;i++)
		for(int j = -5;j < 5;j++){
			Pos pos(hero.pos.x+i,hero.pos.y+j);
			if((dis2(pos, hero.pos) < hero.speed)&&(dis2(pos, enemy.pos) < hero.range)&&(dis2(pos, enemy.pos) > enemy.range)){
				int tmp = 0;
				for(int k = 0;k < enemys.size();k++){
					if(dis2(pos, enemys[k].pos) <= enemys[k].range)
						tmp++;
				}
				if(tmp < num){
					result = pos;
					num = tmp;
				}
				else if(tmp == num){
					if(dis2(pos, enemy.pos) > dis2(result, enemy.pos)){
						result = pos;
					}
				}
/*				if((dis2(pos, enemy.pos) > enemy.range) && (dis2(pos, enemy.pos) > dis2(result, enemy.pos))){
					result = pos;
				}
				if((result == hero.pos) && (dis2(pos, enemy.pos) > dis2(result, enemy.pos))){
					result == pos;
				}	*/
			}
		}
	findShortestPath(*data->map, hero.pos, result, data->block, op.targets);
	cmd->cmds.push_back(op);
	data->already[rankHero(hero)] = true;
}

PSkill ChooseSkill(PUnit& hero, PUnit& enemy){
	output << "ChooseSkill: " << hero.skills.size() << endl;
	if((enemy.typeId == 10) || (enemy.typeId == 3)){
		bool flag = false;
		for(int i = 0;i < hero.buffs.size();i++)
			if(hero.buffs[i].typeId == 5){
				flag = true;
				break;
			}
		if(!flag && hero.canUseSkill(11)){
			for(int i = 0;i < hero.skills.size();i++)
				if(hero.skills[i].typeId == 11)
					return hero.skills[i];
		}
		if(flag){
			for(int i = 0;i < hero.skills.size();i++)
				if(hero.skills[i].typeId == 0)
					return hero.skills[i];
		}
		for(int i = 0;i < hero.skills.size();i++)
			if(hero.skills[i].typeId != 0)
				if(hero.canUseSkill(hero.skills[i].typeId))
					return hero.skills[i];
		for(int i = 0;i < hero.skills.size();i++)
			if(hero.skills[i].typeId == 0)
				return hero.skills[i];
	}
	if(enemy.typeId < 3){
		if(hero.canUseSkill(7)){
			for(int i = 0;i < hero.skills.size();i++)
				if(hero.skills[i].typeId == 7)
					return hero.skills[i];
		}
		for(int i = 0;i < hero.skills.size();i++)
			if(hero.skills[i].typeId != 0)
				if(hero.canUseSkill(hero.skills[i].typeId))
					return hero.skills[i];
		for(int i = 0;i < hero.skills.size();i++)
			if(hero.skills[i].typeId == 0)
				return hero.skills[i];
	}
	for(int i = 0;i < hero.skills.size();i++)
		if(hero.skills[i].typeId == 0)
			return hero.skills[i];
}

void UpdateUnit(PUnit& unit){
	const PUnit* tmp = data->info->findUnitById(unit.id);
	if(tmp != NULL)
		unit = *tmp;
}

const PArg* Hp(PUnit& hero){
	const PArg* result = hero["Hp"];
	if(result == NULL){
		result = &hero.args[0];
	}
	return result;
}
