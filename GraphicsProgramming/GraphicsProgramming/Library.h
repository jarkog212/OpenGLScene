#pragma once
#include "structs.h"
#include "Material.h"

//class that load all assets once and gives references to their memory locations
class Library
{
	TEX_wrap_type wrapping[2] = { TEX_wrap_type::Repeat,TEX_wrap_type::Repeat };  //array created for texture initialisation, specifies wrapping of texture in both axes

	// a map of textures, all statically created and loaded, uses strings as keys for easier handling. Texture struct constructor allows for application, wrapping and filtering to be set for each texture
	unordered_map<string, Texture> textures = {
		{"Wood",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Bilinear,"gfx/floor texture edit.png")},
		{"Floor",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Bilinear,"gfx/floor.png")},
		{"CircleImposter",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Bilinear,"gfx/imposter.png")},
		{"SquareImposter",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Bilinear,"gfx/square.png")},
		{"SquareImposterA",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Bilinear,"gfx/squareA.png")},
		{"SquareImposterB",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Bilinear,"gfx/squareB.png")},
		{"Plant",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Bilinear,"gfx/plantText.png")},
		{"Jacket",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/jacketT.png")},
		{"Screen",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/screen.png")},
		{"ScreenA",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/screenA.png")},
		{"BoxBrand1",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/brand texture1.png")},
		{"BoxBrand2",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/brand texture2.png")},
		{"SignBrand1",Texture(TEX_application_type::Decal,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/sign brand 1.png")},
		{"SignBrand2",Texture(TEX_application_type::Decal,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/sign brand 2.png")},
		{"SignBrand3",Texture(TEX_application_type::Decal,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/sign brand 3.png")},
		{"SignBrand4",Texture(TEX_application_type::Decal,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/sign brand 4.png")},
		{"ArticleBrand1",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/Tshirt1.png")},
		{"ArticleBrand2",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/Jacket2.png")},
		{"ArticleBrand3",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/Hoodie3.png")},
		{"ArticleBrand4",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/Tshirt4.png")},
		{"Fabric",Texture(TEX_application_type::Modulate,wrapping,TEX_filtering_type::Trilinear_Bi_MM_Point,"gfx/fabric.png")}
	};
	
	// a map of materials, all statically created, uses strings as keys for easier handling. Material class constructor allows for name, soublesidedness, 
	// diffuse colour, ambient colour, emissive colour, specular colour and shininess to be set
	unordered_map<string, Material> materials = {
		{"BlueEmissive",Material("BlueEmissive",true,{0.184,0.4,1,1},{0.184,0.4,1,1},{0.184,0.4,1,1},DefinedColours::black(),0)},
		{"SignBrand",Material("SignBrand",false,DefinedColours::white()*0.3,DefinedColours::white()*0.1,{0,0,0,0},DefinedColours::white(),120)},
		{"Metal",Material("Metal",false,DefinedColours::white()*0.4,DefinedColours::white()*0.3,{0,0,0,0},DefinedColours::white()*0.5,85)},
		{"WhiteShine",Material("WhiteShine",false,DefinedColours::white()*0.7,DefinedColours::white()*0.5,{0,0,0,0},DefinedColours::white(),128)},
		{"Wood",Material("Wood",false,{0.25,0.12,0.1,1},{0.25,0.12,0.1,1},{0,0,0,0},{0.8, 0.65, 0.5, 1},75)},
		{"SignBrand",Material("SignBrand",false,DefinedColours::white() * 0.3,DefinedColours::white() * 0.1,{0,0,0,0},DefinedColours::white(),120)},
		{"ReflectiveFloor",Material("ReflectiveFloor",false,{0.1,0.1,0.1,0.8},{0.3,0.3,0.3,0.8},{0,0,0,0},DefinedColours::white(),80)},
		{"GreyShinyWall",Material("GreyShinyWall",false,{0.111*3,0.111*3,0.111*3,1},{0.111*3,0.111*3,0.111*3,1},{0,0,0,0},DefinedColours::white(),128)},
		{"LED",Material("LED",false,DefinedColours::white(),DefinedColours::white(),DefinedColours::white(),DefinedColours::white(),0)},
		{"WhiteWall",Material("WhiteWall",false,DefinedColours::white()*0.6,DefinedColours::white()*0.5,{0,0,0,0},DefinedColours::white()*0.6,40)},
		{"Cloth0",Material("Cloth0",false,{0.8,0.4,0.45,1},{0.8,0.4,0.45,1},{0,0,0,0},DefinedColours::black(),0)},
		{"Cloth1",Material("Cloth1",false,{0.55,0.5,0.8,1},{0.55,0.5,0.8,1},{0,0,0,0},DefinedColours::black(),0)},
		{"Cloth2",Material("Cloth2",false,{0.8,0.81,0.5,1},{0.8,0.81,0.5,1},{0,0,0,0},DefinedColours::black(),0)},
		{"Cloth3",Material("Cloth3",false,{0.7,0.85,0.8,1},{0.7,0.85,0.8,1},{0,0,0,0},DefinedColours::black(),0)},
		{"Cloth4",Material("Cloth4",false,{0.7,0.2,0.8,1},{0.7,0.2,0.8,1},{0,0,0,0},DefinedColours::white()*0.2,40)},
		{"Cloth5",Material("Cloth5",false,{0.85,0.7,0.4,1},{0.85,0.7,0.4,1},{0,0,0,0},DefinedColours::white()*0.2,40)},
		{"Cloth6",Material("Cloth6",false,{0.45,0.4,0.75,1},{0.45,0.4,0.75,1},{0,0,0,0},DefinedColours::white()*0.2,40)},
		{"Cloth7",Material("Cloth7",false,{0.9,0.9,0.9,1},{0.9,0.9,0.9,1},{0,0,0,0},DefinedColours::black(),0)},
		{"Cloth8",Material("Cloth8",false,{0.6,0.6,0.6,1},{0.6,0.6,0.6,1},{0,0,0,0},DefinedColours::black(),0)},
		{"Cloth9",Material("Cloth9",false,{0.4,0.4,0.4,1},{0.4,0.4,0.4,1},{0,0,0,0},DefinedColours::black(),0)},
		{"Screen",Material("ScreenA",false,{1,1,1,0.6},{1,1,1,0.6},{1,1,1,1},DefinedColours::black(),0)},
		{"Floor",Material("Floor",false,{0.2,0.2,0.2,0.9},{0.2,0.2,0.2,0.9},{0,0,0,1},DefinedColours::white(),128)}
	};

public:
	Library();
	Texture* getTexture(std::string name) { return &textures[name]; }    //uses key to return a refference to the texture 
	Material* getMaterial(std::string name) {return &materials[name]; }  //uses key to return a refference to the material 
};
