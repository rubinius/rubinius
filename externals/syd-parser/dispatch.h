static st_table *_id_tbl;
METH(block);
METH(defined);
METH(colon2);
METH(match2);
METH(match3);
METH(begin);
METH(opt_n);
METH(not);
METH(if);
METH(case);
METH(when);
METH(while);
METH(until);
METH(block_pass);
METH(retry);
METH(false);
METH(nil);
METH(self);
METH(true);
METH(zarray);
METH(zsuper);
METH(redo);
METH(break);
METH(next);
METH(return);
METH(super);
METH(method);
METH(splat);
METH(to_ary);
METH(svalue);
METH(dsym);
METH(evstr);
METH(scope);
METH(yield);
METH(rescue);
METH(resbody);
METH(ensure);
METH(dot2);
METH(dot3);
METH(flip2);
METH(flip3);
METH(argscat);
METH(argspush);
METH(op_asgn_and);
METH(op_asgn_or);
METH(and);
METH(or);
METH(call);
METH(fcall);
METH(vcall);
METH(op_asgn1);
METH(op_asgn2);
METH(masgn);
METH(lasgn);
METH(iasgn);
METH(dasgn);
METH(dasgn_curr);
METH(cvasgn);
METH(cvdecl);
METH(gasgn);
METH(cdecl);
METH(alias);
METH(valias);
METH(hash);
METH(array);
METH(dstr);
METH(dxstr);
METH(dregx);
METH(defs);
METH(defn);
METH(class);
METH(module);
METH(sclass);
METH(args);
METH(lvar);
METH(dvar);
METH(ivar);
METH(cvar);
METH(gvar);
METH(const);
METH(attrset);
METH(str);
METH(xstr);
METH(match);
METH(lit);
METH(newline);
METH(nth_ref);
METH(back_ref);
METH(block_arg);
METH(attrasgn);
METH(iter);
METH(for);
static inline NODE *_dispatch(ID curr, VALUE ary) {
  int idx = -1;
  if(!st_lookup(_id_tbl, (st_data_t)curr, &idx)) { printf("Unable to find value '%s'!\n", rb_id2name(curr)); rb_p(ary); abort(); }
  switch(idx) {
  case 0:
     return CALL(block, ary);
  case 1:
     return CALL(defined, ary);
  case 2:
     return CALL(colon2, ary);
  case 3:
     return CALL(match2, ary);
  case 4:
     return CALL(match3, ary);
  case 5:
     return CALL(begin, ary);
  case 6:
     return CALL(opt_n, ary);
  case 7:
     return CALL(not, ary);
  case 8:
     return CALL(if, ary);
  case 9:
     return CALL(case, ary);
  case 10:
     return CALL(when, ary);
  case 11:
     return CALL(while, ary);
  case 12:
     return CALL(until, ary);
  case 13:
     return CALL(block_pass, ary);
  case 14:
     return CALL(retry, ary);
  case 15:
     return CALL(false, ary);
  case 16:
     return CALL(nil, ary);
  case 17:
     return CALL(self, ary);
  case 18:
     return CALL(true, ary);
  case 19:
     return CALL(zarray, ary);
  case 20:
     return CALL(zsuper, ary);
  case 21:
     return CALL(redo, ary);
  case 22:
     return CALL(break, ary);
  case 23:
     return CALL(next, ary);
  case 24:
     return CALL(return, ary);
  case 25:
     return CALL(super, ary);
  case 26:
     return CALL(method, ary);
  case 27:
     return CALL(splat, ary);
  case 28:
     return CALL(to_ary, ary);
  case 29:
     return CALL(svalue, ary);
  case 30:
     return CALL(dsym, ary);
  case 31:
     return CALL(evstr, ary);
  case 32:
     return CALL(scope, ary);
  case 33:
     return CALL(yield, ary);
  case 34:
     return CALL(rescue, ary);
  case 35:
     return CALL(resbody, ary);
  case 36:
     return CALL(ensure, ary);
  case 37:
     return CALL(dot2, ary);
  case 38:
     return CALL(dot3, ary);
  case 39:
     return CALL(flip2, ary);
  case 40:
     return CALL(flip3, ary);
  case 41:
     return CALL(argscat, ary);
  case 42:
     return CALL(argspush, ary);
  case 43:
     return CALL(op_asgn_and, ary);
  case 44:
     return CALL(op_asgn_or, ary);
  case 45:
     return CALL(and, ary);
  case 46:
     return CALL(or, ary);
  case 47:
     return CALL(call, ary);
  case 48:
     return CALL(fcall, ary);
  case 49:
     return CALL(vcall, ary);
  case 50:
     return CALL(op_asgn1, ary);
  case 51:
     return CALL(op_asgn2, ary);
  case 52:
     return CALL(masgn, ary);
  case 53:
     return CALL(lasgn, ary);
  case 54:
     return CALL(iasgn, ary);
  case 55:
     return CALL(dasgn, ary);
  case 56:
     return CALL(dasgn_curr, ary);
  case 57:
     return CALL(cvasgn, ary);
  case 58:
     return CALL(cvdecl, ary);
  case 59:
     return CALL(gasgn, ary);
  case 60:
     return CALL(cdecl, ary);
  case 61:
     return CALL(alias, ary);
  case 62:
     return CALL(valias, ary);
  case 63:
     return CALL(hash, ary);
  case 64:
     return CALL(array, ary);
  case 65:
     return CALL(dstr, ary);
  case 66:
     return CALL(dxstr, ary);
  case 67:
     return CALL(dregx, ary);
  case 68:
     return CALL(defs, ary);
  case 69:
     return CALL(defn, ary);
  case 70:
     return CALL(class, ary);
  case 71:
     return CALL(module, ary);
  case 72:
     return CALL(sclass, ary);
  case 73:
     return CALL(args, ary);
  case 74:
     return CALL(lvar, ary);
  case 75:
     return CALL(dvar, ary);
  case 76:
     return CALL(ivar, ary);
  case 77:
     return CALL(cvar, ary);
  case 78:
     return CALL(gvar, ary);
  case 79:
     return CALL(const, ary);
  case 80:
     return CALL(attrset, ary);
  case 81:
     return CALL(str, ary);
  case 82:
     return CALL(xstr, ary);
  case 83:
     return CALL(match, ary);
  case 84:
     return CALL(lit, ary);
  case 85:
     return CALL(newline, ary);
  case 86:
     return CALL(nth_ref, ary);
  case 87:
     return CALL(back_ref, ary);
  case 88:
     return CALL(block_arg, ary);
  case 89:
     return CALL(attrasgn, ary);
  case 90:
     return CALL(iter, ary);
  case 91:
     return CALL(for, ary);
  default:
      printf("Unknown node type %d, %d, %s!\n", idx, curr, rb_id2name(curr));
      return NULL;
   }
}
static _init_dispatch() {
   _id_tbl = st_init_numtable_with_size(92);
   st_insert(_id_tbl, (st_data_t)rb_intern("block"), (st_data_t)0);
   st_insert(_id_tbl, (st_data_t)rb_intern("defined"), (st_data_t)1);
   st_insert(_id_tbl, (st_data_t)rb_intern("colon2"), (st_data_t)2);
   st_insert(_id_tbl, (st_data_t)rb_intern("match2"), (st_data_t)3);
   st_insert(_id_tbl, (st_data_t)rb_intern("match3"), (st_data_t)4);
   st_insert(_id_tbl, (st_data_t)rb_intern("begin"), (st_data_t)5);
   st_insert(_id_tbl, (st_data_t)rb_intern("opt_n"), (st_data_t)6);
   st_insert(_id_tbl, (st_data_t)rb_intern("not"), (st_data_t)7);
   st_insert(_id_tbl, (st_data_t)rb_intern("if"), (st_data_t)8);
   st_insert(_id_tbl, (st_data_t)rb_intern("case"), (st_data_t)9);
   st_insert(_id_tbl, (st_data_t)rb_intern("when"), (st_data_t)10);
   st_insert(_id_tbl, (st_data_t)rb_intern("while"), (st_data_t)11);
   st_insert(_id_tbl, (st_data_t)rb_intern("until"), (st_data_t)12);
   st_insert(_id_tbl, (st_data_t)rb_intern("block_pass"), (st_data_t)13);
   st_insert(_id_tbl, (st_data_t)rb_intern("retry"), (st_data_t)14);
   st_insert(_id_tbl, (st_data_t)rb_intern("false"), (st_data_t)15);
   st_insert(_id_tbl, (st_data_t)rb_intern("nil"), (st_data_t)16);
   st_insert(_id_tbl, (st_data_t)rb_intern("self"), (st_data_t)17);
   st_insert(_id_tbl, (st_data_t)rb_intern("true"), (st_data_t)18);
   st_insert(_id_tbl, (st_data_t)rb_intern("zarray"), (st_data_t)19);
   st_insert(_id_tbl, (st_data_t)rb_intern("zsuper"), (st_data_t)20);
   st_insert(_id_tbl, (st_data_t)rb_intern("redo"), (st_data_t)21);
   st_insert(_id_tbl, (st_data_t)rb_intern("break"), (st_data_t)22);
   st_insert(_id_tbl, (st_data_t)rb_intern("next"), (st_data_t)23);
   st_insert(_id_tbl, (st_data_t)rb_intern("return"), (st_data_t)24);
   st_insert(_id_tbl, (st_data_t)rb_intern("super"), (st_data_t)25);
   st_insert(_id_tbl, (st_data_t)rb_intern("method"), (st_data_t)26);
   st_insert(_id_tbl, (st_data_t)rb_intern("splat"), (st_data_t)27);
   st_insert(_id_tbl, (st_data_t)rb_intern("to_ary"), (st_data_t)28);
   st_insert(_id_tbl, (st_data_t)rb_intern("svalue"), (st_data_t)29);
   st_insert(_id_tbl, (st_data_t)rb_intern("dsym"), (st_data_t)30);
   st_insert(_id_tbl, (st_data_t)rb_intern("evstr"), (st_data_t)31);
   st_insert(_id_tbl, (st_data_t)rb_intern("scope"), (st_data_t)32);
   st_insert(_id_tbl, (st_data_t)rb_intern("yield"), (st_data_t)33);
   st_insert(_id_tbl, (st_data_t)rb_intern("rescue"), (st_data_t)34);
   st_insert(_id_tbl, (st_data_t)rb_intern("resbody"), (st_data_t)35);
   st_insert(_id_tbl, (st_data_t)rb_intern("ensure"), (st_data_t)36);
   st_insert(_id_tbl, (st_data_t)rb_intern("dot2"), (st_data_t)37);
   st_insert(_id_tbl, (st_data_t)rb_intern("dot3"), (st_data_t)38);
   st_insert(_id_tbl, (st_data_t)rb_intern("flip2"), (st_data_t)39);
   st_insert(_id_tbl, (st_data_t)rb_intern("flip3"), (st_data_t)40);
   st_insert(_id_tbl, (st_data_t)rb_intern("argscat"), (st_data_t)41);
   st_insert(_id_tbl, (st_data_t)rb_intern("argspush"), (st_data_t)42);
   st_insert(_id_tbl, (st_data_t)rb_intern("op_asgn_and"), (st_data_t)43);
   st_insert(_id_tbl, (st_data_t)rb_intern("op_asgn_or"), (st_data_t)44);
   st_insert(_id_tbl, (st_data_t)rb_intern("and"), (st_data_t)45);
   st_insert(_id_tbl, (st_data_t)rb_intern("or"), (st_data_t)46);
   st_insert(_id_tbl, (st_data_t)rb_intern("call"), (st_data_t)47);
   st_insert(_id_tbl, (st_data_t)rb_intern("fcall"), (st_data_t)48);
   st_insert(_id_tbl, (st_data_t)rb_intern("vcall"), (st_data_t)49);
   st_insert(_id_tbl, (st_data_t)rb_intern("op_asgn1"), (st_data_t)50);
   st_insert(_id_tbl, (st_data_t)rb_intern("op_asgn2"), (st_data_t)51);
   st_insert(_id_tbl, (st_data_t)rb_intern("masgn"), (st_data_t)52);
   st_insert(_id_tbl, (st_data_t)rb_intern("lasgn"), (st_data_t)53);
   st_insert(_id_tbl, (st_data_t)rb_intern("iasgn"), (st_data_t)54);
   st_insert(_id_tbl, (st_data_t)rb_intern("dasgn"), (st_data_t)55);
   st_insert(_id_tbl, (st_data_t)rb_intern("dasgn_curr"), (st_data_t)56);
   st_insert(_id_tbl, (st_data_t)rb_intern("cvasgn"), (st_data_t)57);
   st_insert(_id_tbl, (st_data_t)rb_intern("cvdecl"), (st_data_t)58);
   st_insert(_id_tbl, (st_data_t)rb_intern("gasgn"), (st_data_t)59);
   st_insert(_id_tbl, (st_data_t)rb_intern("cdecl"), (st_data_t)60);
   st_insert(_id_tbl, (st_data_t)rb_intern("alias"), (st_data_t)61);
   st_insert(_id_tbl, (st_data_t)rb_intern("valias"), (st_data_t)62);
   st_insert(_id_tbl, (st_data_t)rb_intern("hash"), (st_data_t)63);
   st_insert(_id_tbl, (st_data_t)rb_intern("array"), (st_data_t)64);
   st_insert(_id_tbl, (st_data_t)rb_intern("dstr"), (st_data_t)65);
   st_insert(_id_tbl, (st_data_t)rb_intern("dxstr"), (st_data_t)66);
   st_insert(_id_tbl, (st_data_t)rb_intern("dregx"), (st_data_t)67);
   st_insert(_id_tbl, (st_data_t)rb_intern("defs"), (st_data_t)68);
   st_insert(_id_tbl, (st_data_t)rb_intern("defn"), (st_data_t)69);
   st_insert(_id_tbl, (st_data_t)rb_intern("class"), (st_data_t)70);
   st_insert(_id_tbl, (st_data_t)rb_intern("module"), (st_data_t)71);
   st_insert(_id_tbl, (st_data_t)rb_intern("sclass"), (st_data_t)72);
   st_insert(_id_tbl, (st_data_t)rb_intern("args"), (st_data_t)73);
   st_insert(_id_tbl, (st_data_t)rb_intern("lvar"), (st_data_t)74);
   st_insert(_id_tbl, (st_data_t)rb_intern("dvar"), (st_data_t)75);
   st_insert(_id_tbl, (st_data_t)rb_intern("ivar"), (st_data_t)76);
   st_insert(_id_tbl, (st_data_t)rb_intern("cvar"), (st_data_t)77);
   st_insert(_id_tbl, (st_data_t)rb_intern("gvar"), (st_data_t)78);
   st_insert(_id_tbl, (st_data_t)rb_intern("const"), (st_data_t)79);
   st_insert(_id_tbl, (st_data_t)rb_intern("attrset"), (st_data_t)80);
   st_insert(_id_tbl, (st_data_t)rb_intern("str"), (st_data_t)81);
   st_insert(_id_tbl, (st_data_t)rb_intern("xstr"), (st_data_t)82);
   st_insert(_id_tbl, (st_data_t)rb_intern("match"), (st_data_t)83);
   st_insert(_id_tbl, (st_data_t)rb_intern("lit"), (st_data_t)84);
   st_insert(_id_tbl, (st_data_t)rb_intern("newline"), (st_data_t)85);
   st_insert(_id_tbl, (st_data_t)rb_intern("nth_ref"), (st_data_t)86);
   st_insert(_id_tbl, (st_data_t)rb_intern("back_ref"), (st_data_t)87);
   st_insert(_id_tbl, (st_data_t)rb_intern("block_arg"), (st_data_t)88);
   st_insert(_id_tbl, (st_data_t)rb_intern("attrasgn"), (st_data_t)89);
   st_insert(_id_tbl, (st_data_t)rb_intern("iter"), (st_data_t)90);
   st_insert(_id_tbl, (st_data_t)rb_intern("for"), (st_data_t)91);
}
