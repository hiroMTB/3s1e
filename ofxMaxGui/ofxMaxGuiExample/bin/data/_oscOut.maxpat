{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 5,
			"minor" : 1,
			"revision" : 9
		}
,
		"rect" : [ 0.0, 0.0, 400.0, 100.0 ],
        "bglocked" : 0,
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "OpenSoundControl",
					"fontsize" : 12.0,
					"presentation_rect" : [ 0.0, 30.0, 113.0, 20.0 ],
					"numinlets" : 1,
					"numoutlets" : 3,
					"patching_rect" : [ 0.0, 30.0, 113.0, 20.0 ],
					"presentation" : 1,
					"id" : "obj-3",
					"fontname" : "Arial",
					"outlettype" : [ "", "", "OSCTimeTag" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"presentation_rect" : [ 120.0, 30.0, 20.0, 20.0 ],
					"numinlets" : 1,
					"numoutlets" : 1,
					"patching_rect" : [ 120.0, 30.0, 20.0, 20.0 ],
					"presentation" : 1,
					"id" : "obj-4",
					"outlettype" : [ "bang" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "r osc_out",
					"fontsize" : 12.0,
					"presentation_rect" : [ 0.0, 0.0, 60.0, 20.0 ],
					"numinlets" : 0,
					"numoutlets" : 1,
					"patching_rect" : [ 0.0, 0.0, 60.0, 20.0 ],
					"presentation" : 1,
					"id" : "obj-122",
					"fontname" : "Arial",
					"outlettype" : [ "" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "udpsend localhost #1",
					"fontsize" : 12.0,
					"presentation_rect" : [ 0.0, 60.0, 124.0, 20.0 ],
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 0.0, 60.0, 124.0, 20.0 ],
					"presentation" : 1,
					"id" : "obj-121",
					"fontname" : "Arial"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-122", 0 ],
					"destination" : [ "obj-4", 0 ],
					"hidden" : 0,
					"midpoints" : [ 9.5, 24.5, 129.5, 24.5 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-122", 0 ],
					"destination" : [ "obj-3", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 0 ],
					"destination" : [ "obj-121", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
 ]
	}

}
