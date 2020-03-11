#pragma once

#include "Core/std_types.hpp"
#include "Core/time.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>


extern bool wireframe_mode;

/*----------------------------------------------------------------------------------*/

struct framerate {
	Time elapsed;
	u16 frames = 0;
	u16 fps = 0;
};

u16 getFramesPerSecond(Time dt);

/*----------------------------------------------------------------------------------*/

std::vector<std::string> readFile(std::filesystem::path path);
 