/* ScriptManager_LuaJIT.h
Author: Jeff Kiah
Orig.Date: 06/08/2012
*/
#pragma once

#include "Script/ScriptManager_LuaJIT.h"
#include "Utility/Debug.h"

/*---------------------------------------------------------------------
	Executes a string of Lua. scriptStr must be null-terminated
---------------------------------------------------------------------*/
inline bool ScriptManager::doString(const string &scriptStr)
{
	return (!luaL_dostring(m_state, scriptStr.c_str()));
}
/*---------------------------------------------------------------------
	Loads and executes Lua code from a file
---------------------------------------------------------------------*/
inline bool ScriptManager::doFile(const string &filename)
{
	int e = luaL_dofile(m_state, filename.c_str());
	if (e) { debugPrintf("ScriptManager: couldn't load file: %s\n", lua_tostring(m_state, -1)); }
	return (!e);
}
