#include "Library.h"

//sets additional settings for materials 
Library::Library()
{
	materials["LED"].setEmissionMultiplier(20);         //sets emission multiplier
	materials["ScreenA"].setEmissionMultiplier(20);     //sets emission multiplier
}
