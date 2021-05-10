/*
* Copyright (c) 2017-2019 CUJO LLC.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifdef __linux__
#include <linux/module.h>

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

EXPORT_SYMBOL(lua_checkstack);
EXPORT_SYMBOL(lua_xmove);
EXPORT_SYMBOL(lua_atpanic);
EXPORT_SYMBOL(lua_version);
EXPORT_SYMBOL(lua_absindex);
EXPORT_SYMBOL(lua_gettop);
EXPORT_SYMBOL(lua_settop);
EXPORT_SYMBOL(lua_rotate);
EXPORT_SYMBOL(lua_copy);
EXPORT_SYMBOL(lua_pushvalue);
EXPORT_SYMBOL(lua_type);
EXPORT_SYMBOL(lua_typename);
EXPORT_SYMBOL(lua_iscfunction);
EXPORT_SYMBOL(lua_isinteger);
EXPORT_SYMBOL(lua_isnumber);
EXPORT_SYMBOL(lua_isstring);
EXPORT_SYMBOL(lua_isuserdata);
EXPORT_SYMBOL(lua_rawequal);
EXPORT_SYMBOL(lua_arith);
EXPORT_SYMBOL(lua_compare);
EXPORT_SYMBOL(lua_stringtonumber);
EXPORT_SYMBOL(lua_tointegerx);
EXPORT_SYMBOL(lua_toboolean);
EXPORT_SYMBOL(lua_tolstring);
EXPORT_SYMBOL(lua_rawlen);
EXPORT_SYMBOL(lua_tocfunction);
EXPORT_SYMBOL(lua_touserdata);
EXPORT_SYMBOL(lua_tothread);
EXPORT_SYMBOL(lua_topointer);
EXPORT_SYMBOL(lua_pushnil);
EXPORT_SYMBOL(lua_pushinteger);
EXPORT_SYMBOL(lua_pushlstring);
EXPORT_SYMBOL(lua_pushstring);
EXPORT_SYMBOL(lua_pushvfstring);
EXPORT_SYMBOL(lua_pushfstring);
EXPORT_SYMBOL(lua_pushcclosure);
EXPORT_SYMBOL(lua_pushboolean);
EXPORT_SYMBOL(lua_pushlightuserdata);
EXPORT_SYMBOL(lua_pushthread);
EXPORT_SYMBOL(lua_getglobal);
EXPORT_SYMBOL(lua_gettable);
EXPORT_SYMBOL(lua_getfield);
EXPORT_SYMBOL(lua_geti);
EXPORT_SYMBOL(lua_rawget);
EXPORT_SYMBOL(lua_rawgeti);
EXPORT_SYMBOL(lua_rawgetp);
EXPORT_SYMBOL(lua_createtable);
EXPORT_SYMBOL(lua_getmetatable);
EXPORT_SYMBOL(lua_getuservalue);
EXPORT_SYMBOL(lua_setglobal);
EXPORT_SYMBOL(lua_settable);
EXPORT_SYMBOL(lua_setfield);
EXPORT_SYMBOL(lua_seti);
EXPORT_SYMBOL(lua_rawset);
EXPORT_SYMBOL(lua_rawseti);
EXPORT_SYMBOL(lua_rawsetp);
EXPORT_SYMBOL(lua_setmetatable);
EXPORT_SYMBOL(lua_setuservalue);
EXPORT_SYMBOL(lua_callk);
EXPORT_SYMBOL(lua_pcallk);
EXPORT_SYMBOL(lua_load);
EXPORT_SYMBOL(lua_dump);
EXPORT_SYMBOL(lua_status);
EXPORT_SYMBOL(lua_gc);
EXPORT_SYMBOL(lua_error);
EXPORT_SYMBOL(lua_next);
EXPORT_SYMBOL(lua_concat);
EXPORT_SYMBOL(lua_len);
EXPORT_SYMBOL(lua_getallocf);
EXPORT_SYMBOL(lua_setallocf);
EXPORT_SYMBOL(lua_newuserdata);
EXPORT_SYMBOL(lua_getupvalue);
EXPORT_SYMBOL(lua_setupvalue);
EXPORT_SYMBOL(lua_upvalueid);
EXPORT_SYMBOL(lua_upvaluejoin);
EXPORT_SYMBOL(lua_sethook);
EXPORT_SYMBOL(lua_gethook);
EXPORT_SYMBOL(lua_gethookmask);
EXPORT_SYMBOL(lua_gethookcount);
EXPORT_SYMBOL(lua_getstack);
EXPORT_SYMBOL(lua_getlocal);
EXPORT_SYMBOL(lua_setlocal);
EXPORT_SYMBOL(lua_getinfo);
EXPORT_SYMBOL(lua_resume);
EXPORT_SYMBOL(lua_isyieldable);
EXPORT_SYMBOL(lua_yieldk);
EXPORT_SYMBOL(lua_newthread);
EXPORT_SYMBOL(lua_newstate);
EXPORT_SYMBOL(lua_close);
EXPORT_SYMBOL(luaL_traceback);
EXPORT_SYMBOL(luaL_argerror);
EXPORT_SYMBOL(luaL_where);
EXPORT_SYMBOL(luaL_error);
EXPORT_SYMBOL(luaL_newmetatable);
EXPORT_SYMBOL(luaL_setmetatable);
EXPORT_SYMBOL(luaL_testudata);
EXPORT_SYMBOL(luaL_checkudata);
EXPORT_SYMBOL(luaL_checkoption);
EXPORT_SYMBOL(luaL_checkstack);
EXPORT_SYMBOL(luaL_checktype);
EXPORT_SYMBOL(luaL_checkany);
EXPORT_SYMBOL(luaL_checklstring);
EXPORT_SYMBOL(luaL_optlstring);
EXPORT_SYMBOL(luaL_checknumber);
EXPORT_SYMBOL(luaL_optnumber);
EXPORT_SYMBOL(luaL_prepbuffsize);
EXPORT_SYMBOL(luaL_addlstring);
EXPORT_SYMBOL(luaL_addstring);
EXPORT_SYMBOL(luaL_pushresult);
EXPORT_SYMBOL(luaL_pushresultsize);
EXPORT_SYMBOL(luaL_addvalue);
EXPORT_SYMBOL(luaL_buffinit);
EXPORT_SYMBOL(luaL_buffinitsize);
EXPORT_SYMBOL(luaL_ref);
EXPORT_SYMBOL(luaL_unref);
EXPORT_SYMBOL(luaL_loadbufferx);
EXPORT_SYMBOL(luaL_loadstring);
EXPORT_SYMBOL(luaL_getmetafield);
EXPORT_SYMBOL(luaL_callmeta);
EXPORT_SYMBOL(luaL_len);
EXPORT_SYMBOL(luaL_tolstring);
EXPORT_SYMBOL(luaL_setfuncs);
EXPORT_SYMBOL(luaL_getsubtable);
EXPORT_SYMBOL(luaL_requiref);
EXPORT_SYMBOL(luaL_gsub);
EXPORT_SYMBOL(luaL_newstate);
EXPORT_SYMBOL(luaL_checkversion_);
EXPORT_SYMBOL(luaL_openlibs);
EXPORT_SYMBOL(luaopen_base);
EXPORT_SYMBOL(luaopen_package);
EXPORT_SYMBOL(luaopen_coroutine);
EXPORT_SYMBOL(luaopen_debug);
EXPORT_SYMBOL(luaopen_math);
EXPORT_SYMBOL(luaopen_os);
EXPORT_SYMBOL(luaopen_string);
EXPORT_SYMBOL(luaopen_table);
EXPORT_SYMBOL(luaopen_utf8);

int lunatik_dev_init(void);
void lunatik_dev_fini(void);

static int __init modinit(void)
{
        return lunatik_dev_init();
}

static void __exit modexit(void)
{
	lunatik_dev_fini();
}

module_init(modinit);
module_exit(modexit);
MODULE_LICENSE("Dual MIT/GPL");
#endif /* __linux__ */
