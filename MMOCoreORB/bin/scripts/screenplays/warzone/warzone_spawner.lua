WarZoneSpawner = ScreenPlay:new {
	numberOfActs = 1,

	screenplayName = "WarZoneSpawner"
}

registerScreenPlay("WarZoneSpawner", true)

function WarZoneSpawner:start()
	if (isZoneEnabled("corellia")) then
		self:spawnMobiles()
		--self:spawnSceneObjects()
	end
end

function WarZoneSpawner:spawnSceneObjects()

	--Guild Hall regular
	spawnSceneObject("corellia", "object/static/structure/corellia/shared_corl_imprv_wall_4x32_s02.iff", -218.89, 28.64,-4049.52, 0, math.rad(-150) )

end
