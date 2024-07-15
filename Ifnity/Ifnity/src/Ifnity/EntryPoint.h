// ReSharper disable All
#pragma once


extern IFNITY::App* IFNITY::CreateApp();



int main(int argc, char** argv)
{
	IFNITY::Log::init();

	IFNITY_LOG(LogCore, ERROR, "variable{}");
	
	IFNITY::App* source = IFNITY::CreateApp();
	source->run();
	
	delete source;

	return 0;
}