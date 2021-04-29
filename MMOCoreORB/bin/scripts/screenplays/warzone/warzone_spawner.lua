WarZoneSpawner = ScreenPlay:new {
	numberOfActs = 1,

	screenplayName = "WarZoneSpawner"
}

registerScreenPlay("WarZoneSpawner", true)

function WarZoneSpawner:start()
	if (isZoneEnabled("corellia")) then
		--self:spawnMobiles()
		self:spawnSceneObjects()
	end
end

function WarZoneSpawner:spawnSceneObjects()

	--Guild Hall regular
	--spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s02.iff", -218.9, 28.6,-4049.5, 0, 0)

	--Parimeter
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s01.iff", -307.2, 28,-4146.5, 9995397, 0)
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s01.iff", -429.3, 28,-4147, 9995396, 0)
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s01.iff", -518.2, 28,-4217.8, 9995395, math.rad(135) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s01.iff", -563.4, 28,-4239.9, 9995394, math.rad(135) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s01.iff", -591.5, 28,-4330.6, 9995393, math.rad(90) )

	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -591.6, 28,-4480.56, 9995392, math.rad(90) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -591.6, 28,-4506.78, 9995391, math.rad(90) )

	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s01.iff", -535.9, 28,-4626.4, 9995390, math.rad(0) )

	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s01.iff", -523.57, 28,-4642.17, 9995389, math.rad(90) )

	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s02.iff", -524.57, 28.6,-4666.34, 9995388, math.rad(90) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s02.iff", -524.3, 28.6,-4679.34, 9995387, math.rad(90) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s02.iff", -503.84, 28.0,-4699, 9995386, math.rad(0) )

	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -301.5, 28,-4783.65, 9995385, math.rad(0) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -218.99, 28,-4794.91, 9995384, math.rad(90) )

	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -120.9, 28,-4814.69, 9995383, math.rad(0) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -90.84, 28,-4833.5, 9995382, math.rad(0) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -26.62, 28,-4838.64, 9995381, math.rad(0) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -1.37, 28,-4847.86, 9995380, math.rad(0) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", 24.55, 28,-4836.67, 9995379, math.rad(350) )

	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s04.iff", 163.37, 28,-4700.05, 9995378, math.rad(315) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s04.iff", 126.03, 28,-4542.33, 9995372, math.rad(270) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", 141.94, 28,-4536.77, 9995373, math.rad(315) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", 109.26, 28,-4467.42, 9995374, math.rad(180) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x32_s01.iff", 93.69, 28,-4402.79, 9995375, math.rad(270) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", 20.53, 28,-4242.06, 9995376, math.rad(180) )
	spawnSceneObject("corellia", "object/static/structure/corellia/corl_imprv_wall_4x16_s01.iff", -116.13, 28,-4206.98, 9995377, math.rad(180) )
end
