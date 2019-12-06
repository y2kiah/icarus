/* ScriptManager_LuaJIT.cpp
Author: Jeff Kiah
Orig.Date: 06/08/2012
*/
#include "Script/ScriptManager_LuaJIT.h"
#include "Utility/Debug.h"

#if defined(_DEBUG)
#pragma comment ( lib, "lua51_d.lib" )
#else
#pragma comment ( lib, "lua51.lib" )
#endif

__declspec(dllexport) void debug_printf(const char *str) {
	debugPrintf(str);
}

void ScriptManager::update()
{
}

bool ScriptManager::init(const string &filename)
{
	m_state = luaL_newstate();

	luaL_openlibs(m_state); /* Load Lua libraries */

	lua_newtable(m_state);
	lua_setglobal(m_state, "engine");

	/* Load the file containing the script we are going to run */
	if (!doFile(filename)) {
		return false;
	}

	return true;
}

void ScriptManager::deinit()
{
	lua_close(m_state);
}