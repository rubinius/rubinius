/* The following immediates have different values in
 * the C-API to be consistent with VALUE as an object
 * handle rather than an object reference (pointer).
 *
 * Here we reset the values to the Rubinius values
 * since the tests run in the Rubinius "space" not
 * the C extension "space".
 */

#undef  Qfalse
#undef  Qtrue
#undef  Qnil
#undef  Qundef

#define Qfalse RBX_Qfalse
#define Qtrue  RBX_Qtrue
#define Qnil   RBX_Qnil
#define Qundef RBX_Qundef
