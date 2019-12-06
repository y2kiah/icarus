/* ScriptManager_LuaJIT.h
Author: Jeff Kiah
Orig.Date: 06/08/2012
*/
#pragma once

#include "lua.hpp"
#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

class ScriptManager {
	private:
		lua_State	*m_state;

	public:
		// Methods
		/*---------------------------------------------------------------------
			Executes a string of Lua. scriptStr must be null-terminated
		---------------------------------------------------------------------*/
		inline bool doString(const string &scriptStr);
		/*---------------------------------------------------------------------
			Loads and executes Lua code from a file
		---------------------------------------------------------------------*/
		inline bool doFile(const string &filename);
		
		void update();
		
		bool init(const string &filename);
		
		void deinit();

		explicit ScriptManager() {}
		~ScriptManager() {}
};

#include "Impl/ScriptManager_LuaJIT.inl"