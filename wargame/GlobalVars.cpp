
#include "stdafx.h"
#include "Globals.h"
using namespace TInfo;
TInfo::TroopList TInfo::FullTroopList({ SoldierType, CatapultType, WallType });
using namespace Attach;
AttachList Attach::FullAttachList({ Torch, BA, Horse, Armor, CataHorse });
BuildList FullBuildList{ SFarm, LFarm, CataFact, TrainCenter, BA_Factory, TorchFactory, Armory, Stable };
BuildList AttachBuilds{ BA_Factory, TorchFactory, Armory, Stable };
BuildList IncomeBuilds{ SFarm, LFarm };
BuildList TrainBuilds{ CataFact, TrainCenter };

Array2d<int> PlayerOcc;
Array2d<Domination> PlayerDom;
