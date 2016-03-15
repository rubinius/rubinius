#ifndef RBX_VM_MISSING_MATH_H
#define RBX_VM_MISSING_MATH_H

/* On OS X 10.5, GCC 4.0.1 tr1/unordered_map requires <cmath>. That
 * header undef's isinf and isnan. So, unless we remove our dependence
 * on unordered_map for SymbolTable, there's no way to avoid <cmath>
 * even when we don't include it ourselves. Later versions of GCC fix
 * this issue.
 */

#ifndef isinf
#define isinf   std::isinf
#endif

#ifndef isnan
#define isnan   std::isnan
#endif

#ifndef signbit
#define signbit std::signbit
#endif

#endif
