/*
** $Id: lapi.c,v 1.97 2000/09/12 13:47:46 roberto Exp $
** Lua API
** See Copyright Notice in lua.h
*/


#include <string.h>

#include "lua.h"

#include "lapi.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"


const char lua_ident[] = "$Lua: " LUA_VERSION " " LUA_COPYRIGHT " $\n"
                               "$Authors: " LUA_AUTHORS " $";



#define Index(L,i)	((i) >= 0 ? (L->Cbase+((i)-1)) : (L->top+(i)))

#define api_incr_top(L)	incr_top




TObject *luaA_index (lua_State *L, int index) {
  return Index(L, index);
}


void luaA_pushobject (lua_State *L, const TObject *o) {
  *L->top = *o;
  incr_top;
}

int lua_stackspace (lua_State *L) {
  return (L->stack_last - L->top);
}



/*
** basic stack manipulation
*/


int lua_gettop (lua_State *L) {
  return (L->top - L->Cbase);
}


void lua_settop (lua_State *L, int index) {
  if (index >= 0)
    luaD_adjusttop(L, L->Cbase, index);
  else
    L->top = L->top+index+1;  /* index is negative */
}


void lua_remove (lua_State *L, int index) {
  StkId p = Index(L, index);
  while (++p < L->top) *(p-1) = *p;
  L->top--;
}


void lua_insert (lua_State *L, int index) {
  TObject temp = *(L->top-1);
  StkId p = Index(L, index);
  StkId q;
  for (q = L->top-1; q>p; q--)
    *q = *(q-1);
  *p = temp;
}


void lua_pushvalue (lua_State *L, int index) {
  *L->top = *Index(L, index);
  api_incr_top(L);
}



/*
** access functions (stack -> C)
*/


#define btest(L,i,value,default)	{ \
  StkId o; \
  if ((i) >= 0) { \
    o = L->Cbase+((i)-1); \
    if (o >= L->top) return (default); \
  } \
  else o = L->top+(i); \
  return (value); }


#define access(L,i,test,default,value)	{ \
  StkId o; \
  if ((i) >= 0) { \
    o = L->Cbase+((i)-1); \
    if (o >= L->top) return (default); \
  } \
  else o = L->top+(i); \
  return ((test) ? (value) : (default)); }


const char *lua_type (lua_State *L, int index) {
  btest(L, index, luaO_typename(o), "NO VALUE");
}

int lua_iscfunction (lua_State *L, int index) {
  btest(L, index, (ttype(o) == TAG_CCLOSURE), 0);
}

int lua_isnumber (lua_State *L, int index) {
  btest(L, index, (tonumber(Index(L, index)) == 0), 0);
}

int lua_tag (lua_State *L, int index) {
  btest(L, index, 
   ((ttype(o) == TAG_USERDATA) ? tsvalue(o)->u.d.tag :
                                 luaT_effectivetag(L, o)), -1);
}

int lua_equal (lua_State *L, int index1, int index2) {
  StkId o1 = Index(L, index1);
  StkId o2 = Index(L, index2);
  if (o1 >= L->top || o2 >= L->top) return 0;  /* index out-of-range */
  else return luaO_equalObj(o1, o2);
}

int lua_lessthan (lua_State *L, int index1, int index2) {
  StkId o1 = Index(L, index1);
  StkId o2 = Index(L, index2);
  if (o1 >= L->top || o2 >= L->top) return 0;  /* index out-of-range */
  else return luaV_lessthan(L, o1, o2, L->top);
}



double lua_tonumber (lua_State *L, int index) {
  access(L, index, (tonumber(o) == 0), 0.0, nvalue(o));
}

const char *lua_tostring (lua_State *L, int index) {
  luaC_checkGC(L);  /* `tostring' may create a new string */
  access(L, index, (tostring(L, o) == 0), NULL, svalue(o));
}

size_t lua_strlen (lua_State *L, int index) {
  access(L, index, (tostring(L, o) == 0), 0, tsvalue(o)->u.s.len);
}

lua_CFunction lua_tocfunction (lua_State *L, int index) {
  access(L, index, (ttype(o) == TAG_CCLOSURE), NULL, clvalue(o)->f.c);
}

void *lua_touserdata (lua_State *L, int index) {
  access(L, index, (ttype(o) == TAG_USERDATA), NULL, tsvalue(o)->u.d.value);
}

const void *lua_topointer (lua_State *L, int index) {
  StkId o = Index(L, index);
  switch (ttype(o)) {
    case TAG_TABLE: 
      return hvalue(o);
    case TAG_CCLOSURE: case TAG_LCLOSURE:
      return clvalue(o);
    default: return NULL;
  }
}



/*
** push functions (C -> stack)
*/


void lua_pushnil (lua_State *L) {
  ttype(L->top) = TAG_NIL;
  api_incr_top(L);
}


void lua_pushnumber (lua_State *L, double n) {
  ttype(L->top) = TAG_NUMBER;
  nvalue(L->top) = n;
  api_incr_top(L);
}


void lua_pushlstring (lua_State *L, const char *s, size_t len) {
  luaC_checkGC(L);
  tsvalue(L->top) = luaS_newlstr(L, s, len);
  ttype(L->top) = TAG_STRING;
  api_incr_top(L);
}


void lua_pushstring (lua_State *L, const char *s) {
  if (s == NULL)
    lua_pushnil(L);
  else
    lua_pushlstring(L, s, strlen(s));
}


void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n) {
  luaC_checkGC(L);
  luaV_Cclosure(L, fn, n);
}


void lua_pushusertag (lua_State *L, void *u, int tag) {  /* ORDER LUA_T */
  luaC_checkGC(L);
  if (tag != LUA_ANYTAG && tag != TAG_USERDATA && tag < NUM_TAGS)
    luaO_verror(L, "invalid tag for a userdata (%d)", tag);
  tsvalue(L->top) = luaS_createudata(L, u, tag);
  ttype(L->top) = TAG_USERDATA;
  api_incr_top(L);
}



/*
** get functions (Lua -> stack)
*/


void lua_getglobal (lua_State *L, const char *name) {
  StkId top = L->top;
  *top = *luaV_getglobal(L, luaS_new(L, name));
  L->top = top;
  api_incr_top(L);
}


void lua_gettable (lua_State *L, int tableindex) {
  StkId t = Index(L, tableindex);
  StkId top = L->top;
  *(top-1) = *luaV_gettable(L, t);
  L->top = top;  /* tag method may change top */
}


void lua_rawget (lua_State *L, int tableindex) {
  StkId t = Index(L, tableindex);
  LUA_ASSERT(ttype(t) == TAG_TABLE, "table expected");
  *(L->top - 1) = *luaH_get(L, hvalue(t), L->top - 1);
}


void lua_rawgeti (lua_State *L, int index, int n) {
  StkId o = Index(L, index);
  LUA_ASSERT(ttype(o) == TAG_TABLE, "table expected");
  *L->top = *luaH_getnum(hvalue(o), n);
  api_incr_top(L);
}


void lua_getglobals (lua_State *L) {
  hvalue(L->top) = L->gt;
  ttype(L->top) = TAG_TABLE;
  api_incr_top(L);
}


int lua_getref (lua_State *L, int ref) {
  if (ref == LUA_REFNIL)
    ttype(L->top) = TAG_NIL;
  else if (0 <= ref && ref < L->refSize &&
          (L->refArray[ref].st == LOCK || L->refArray[ref].st == HOLD))
    *L->top = L->refArray[ref].o;
  else
    return 0;
  api_incr_top(L);
  return 1;
}


void lua_newtable (lua_State *L) {
  luaC_checkGC(L);
  hvalue(L->top) = luaH_new(L, 0);
  ttype(L->top) = TAG_TABLE;
  api_incr_top(L);
}



/*
** set functions (stack -> Lua)
*/


void lua_setglobal (lua_State *L, const char *name) {
  StkId top = L->top;
  luaV_setglobal(L, luaS_new(L, name));
  L->top = top-1;  /* remove element from the top */
}


void lua_settable (lua_State *L, int tableindex) {
  StkId t = Index(L, tableindex);
  StkId top = L->top;
  luaV_settable(L, t, top-2);
  L->top = top-2;  /* pop index and value */
}


void lua_rawset (lua_State *L, int tableindex) {
  StkId t = Index(L, tableindex);
  LUA_ASSERT(ttype(t) == TAG_TABLE, "table expected");
  *luaH_set(L, hvalue(t), L->top-2) = *(L->top-1);
  L->top -= 2;
}


void lua_rawseti (lua_State *L, int index, int n) {
  StkId o = Index(L, index);
  LUA_ASSERT(ttype(o) == TAG_TABLE, "table expected");
  *luaH_setint(L, hvalue(o), n) = *(L->top-1);
  L->top--;
}


void lua_setglobals (lua_State *L) {
  StkId newtable = --L->top;
  LUA_ASSERT(ttype(newtable) == TAG_TABLE, "table expected");
  L->gt = hvalue(newtable);
}


int lua_ref (lua_State *L,  int lock) {
  int ref;
  if (ttype(L->top-1) == TAG_NIL)
    ref = LUA_REFNIL;
  else {
    if (L->refFree != NONEXT) {  /* is there a free place? */
      ref = L->refFree;
      L->refFree = L->refArray[ref].st;
    }
    else {  /* no more free places */
      luaM_growvector(L, L->refArray, L->refSize, 1, struct Ref,
                      "reference table overflow", MAX_INT);
      ref = L->refSize++;
    }
    L->refArray[ref].o = *(L->top-1);
    L->refArray[ref].st = lock ? LOCK : HOLD;
  }
  L->top--;
  return ref;
}


/*
** "do" functions (run Lua code)
** (most of them are in ldo.c)
*/

void lua_rawcall (lua_State *L, int nargs, int nresults) {
  luaD_call(L, L->top-(nargs+1), nresults);
}


/*
** miscellaneous functions
*/

void lua_settag (lua_State *L, int tag) {
  luaT_realtag(L, tag);
  switch (ttype(L->top-1)) {
    case TAG_TABLE:
      hvalue(L->top-1)->htag = tag;
      break;
    case TAG_USERDATA:
      tsvalue(L->top-1)->u.d.tag = tag;
      break;
    default:
      luaO_verror(L, "cannot change the tag of a %.20s",
                  luaO_typename(L->top-1));
  }
  L->top--;
}


void lua_unref (lua_State *L, int ref) {
  if (ref >= 0) {
    LUA_ASSERT(ref < L->refSize && L->refArray[ref].st < 0, "invalid ref");
    L->refArray[ref].st = L->refFree;
    L->refFree = ref;
  }
}


int lua_next (lua_State *L, int tableindex) {
  StkId t = Index(L, tableindex);
  Node *n;
  LUA_ASSERT(ttype(t) == TAG_TABLE, "table expected");
  n = luaH_next(L, hvalue(t), Index(L, -1));
  if (n) {
    *(L->top-1) = *key(n);
    *L->top = *val(n);
    api_incr_top(L);
    return 1;
  }
  else {  /* no more elements */
    L->top -= 1;  /* remove key */
    return 0;
  }
}


int lua_getn (lua_State *L, int index) {
  Hash *h = hvalue(Index(L, index));
  const TObject *value = luaH_getstr(h, luaS_new(L, "n"));  /* value = h.n */
  if (ttype(value) == TAG_NUMBER)
    return (int)nvalue(value);
  else {
    Number max = 0;
    int i = h->size;
    Node *n = h->node;
    while (i--) {
      if (ttype(key(n)) == TAG_NUMBER &&
          ttype(val(n)) != TAG_NIL &&
          nvalue(key(n)) > max)
        max = nvalue(key(n));
      n++;
    }
    return (int)max;
  }
}


void lua_concat (lua_State *L, int n) {
  StkId top = L->top;
  luaV_strconc(L, n, top);
  L->top = top-(n-1);
}

