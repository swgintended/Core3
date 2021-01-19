local ObjectManager = require("managers.object.object_manager")

AnchorheadBattleground = Battleground:new {
	screenplayName = "AnchorheadBattleground",

	zoneName = "tatooine",
	zoneArea = {
		{40, -5348, 350}
	},
	factions = {
		"imperial",
		"rebel"
	},
}
