#ifndef RBX_CAPI_MRI_OOP_H
#define RBX_CAPI_MRI_OOP_H

#ifndef native_int
  typedef intptr_t native_int;
#endif

#ifndef native_uint
  typedef uintptr_t native_uint;
#endif

/*
 *  In MRI, VALUE represents an object.
 *
 *  In Rubinius, this is a Handle.
 */
#define VALUE intptr_t

/*
 *  In MRI, ID represents an interned string, i.e. a Symbol.
 *
 *  In Rubinius, this is a raw Symbol.
 */
#define ID    intptr_t

/*
 * The C-API uses handles to refer to all Ruby objects rather than raw
 * pointers to objects. There are four categories of objects:
 *
 *   1. References (e.g. instances of Array, String, Hash, etc.)
 *   2. Fixnums
 *   3. Symbols
 *   4. "Booleans" (i.e. Qfalse, Qtrue, Qnil, Qundef)
 *
 * The handles are tagged to distinguish the categories of objects.
 *
 * The tagging scheme allows passing Symbols and Fixnums straight through
 * without changing their values. This ensures that if a C extension
 * assigns a global ID with the value of a Symbol, any subsequent handle
 * requested for that symbol (e.g. from rb_intern()) will have the same
 * value. Passing Fixnums through means that all the Fixnum conversions
 * do not have to be reimplemented for the C-API.
 *
 * The tags break down as follows (@see machine/oop.hpp for more details):
 *
 *   00 0 0000   Qfalse
 *   xx x xxx1   Fixnum
 *   xx x x110   Symbol
 *
 *   01 0 0010   Qtrue
 *   10 0 0010   Qnil
 *   11 0 0010   Qundef
 *
 * In other words, any bit pattern ending in 1 is a Fixnum, ending in 110
 * is a Symbol, ending in 0010 is a C-API "boolean" and Qfalse is 0.
 *
 * NOTE: The Qfalse value is defined to be 0 because it is 0 in MRI and
 * extensions written for MRI have (absolutely wrongly, infuriatingly,
 * but-what-can-you-do-now) relied on that assumption in boolean contexts.
 * Rather than fighting a myriad subtle bugs, we just go along with it.
 */

/* The false object. */
#define Qfalse ((VALUE)0x00)

/* The true object. */
#define Qtrue  ((VALUE)0x22)

/* The nil object. */
#define Qnil   ((VALUE)0x42)

/* The undef object. NEVER EXPOSE THIS TO USER CODE. EVER. */
#define Qundef ((VALUE)0x62)

#define FALSE_P(v)          ((VALUE)(v) == Qfalse)
#define TRUE_P(v)           ((VALUE)(v) == Qtrue)
#define NIL_P(v)            ((VALUE)(v) == Qnil)
#define UNDEF_P(v)          ((VALUE)(v) == Qundef)


#undef TAG_REF
#undef TAG_REF_MASK

#define TAG_REF             0x0
#define TAG_REF_MASK        0x3

#undef TAG_FIXNUM
#undef TAG_FIXNUM_SHIFT
#undef TAG_FIXNUM_MASK

#define TAG_FIXNUM          0x1
#define TAG_FIXNUM_SHIFT    0x1
#define TAG_FIXNUM_MASK     0x1

#undef TAG_SYMBOL
#undef TAG_SYMBOL_MASK

#define TAG_SYMBOL          0x6
#define TAG_SYMBOL_MASK     0x7

#define REFERENCE_P(v)      ((v) && (((VALUE)(v) & TAG_REF_MASK) == TAG_REF))
#define IMMEDIATE_P(x)      (!REFERENCE_P(x))

#define FIXNUM_P(v)         (((VALUE)(v) & TAG_FIXNUM_MASK) == TAG_FIXNUM)
#define SYMBOL_P(v)         (((VALUE)(v) & TAG_SYMBOL_MASK) == TAG_SYMBOL)

#define CAPI_TAG_FIXNUM(v)  ((VALUE)(((VALUE)(v) << TAG_FIXNUM_SHIFT) | TAG_FIXNUM))

#define FIXNUM_WIDTH        ((8 * sizeof(native_int)) - TAG_FIXNUM_SHIFT - 1)
#define FIXNUM_MAX          (((native_int)1 << FIXNUM_WIDTH) - 1)
#define FIXNUM_MIN          (-(FIXNUM_MAX))

#endif
