local ObjectManager = require("managers.object.object_manager")

Battleground = ScreenPlay:new {
	zoneName = "",
	zoneArea = {},
	factions = {},
}

function Battleground:getFactionCreaturesInArea()
	local creatures = getInRangeCreatureObjects(self.zoneName, self.zoneArea);
	local factionCreatures = {}
	for k, faction in pairs(self.factions) do
		factionCreatures[faction] = {
			players = {},
			npcs = {}
		}
	end

	for k, pCreature in pairs(creatures) do
		for k, faction in pairs(self.factions) do
			-- Ignore non-PvP faction creatures such as recruiters
			if (CreatureObject(pCreature):getFaction() == hashCode(faction) and TangibleObject(pCreature):getPvpStatusBitmask() ~= NONE) then
				if (SceneObject(pCreature):isPlayerCreature()) then
					table.insert(factionCreatures[faction].players, pCreature)
				else
					table.insert(factionCreatures[faction].npcs, pCreature)
				end
			end
		end
	end

	return factionCreatures
end

function Battleground:debug(pPlayer)
	local factionCreatures = self:getFactionCreaturesInArea()
	for k, faction in pairs(self.factions) do
		CreatureObject(pPlayer):sendSystemMessage("There are " .. #factionCreatures[faction].npcs .. " " .. faction .. " npcs and " .. #factionCreatures[faction].players .. " players in the area.")
	end
end
