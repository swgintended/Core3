fs_village_area = SharedObjectTemplate:new {
	clientTemplateFileName = "",
	planetMapCategory = "cloningfacility",
	planetMapSubCategory = "",
	autoRegisterWithPlanetMap = 1,
	templateType = CLONINGBUILDING,
	facilityType = CLONER_FS_ONLY,


	spawningPoints = {
		{ x = -8, z = 78.5, y = -46, ow = -0.706572, ox = 0, oz = 0, oy = 0, cellid = 0 }
	},

	zoneComponent = "ZoneComponent",
	objectMenuComponent = "ObjectMenuComponent",
	containerComponent = "ContainerComponent",
	gameObjectType = 33554442
}
	
ObjectTemplates:addTemplate(fs_village_area, "object/fs_village_area.iff")
