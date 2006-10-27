#define CMETHOD_f_INSTANCE_VARIABLES 0
#define cmethod_get_instance_variables(obj) NTH_FIELD(obj, CMETHOD_f_INSTANCE_VARIABLES)
#define cmethod_set_instance_variables(obj, val) SET_FIELD(obj, CMETHOD_f_INSTANCE_VARIABLES, val)
#define CMETHOD_f_PRIMITIVE 1
#define cmethod_get_primitive(obj) NTH_FIELD(obj, CMETHOD_f_PRIMITIVE)
#define cmethod_set_primitive(obj, val) SET_FIELD(obj, CMETHOD_f_PRIMITIVE, val)
#define CMETHOD_f_REQUIRED 2
#define cmethod_get_required(obj) NTH_FIELD(obj, CMETHOD_f_REQUIRED)
#define cmethod_set_required(obj, val) SET_FIELD(obj, CMETHOD_f_REQUIRED, val)
#define CMETHOD_f_BYTECODES 3
#define cmethod_get_bytecodes(obj) NTH_FIELD(obj, CMETHOD_f_BYTECODES)
#define cmethod_set_bytecodes(obj, val) SET_FIELD(obj, CMETHOD_f_BYTECODES, val)
#define CMETHOD_f_LOCALS 4
#define cmethod_get_locals(obj) NTH_FIELD(obj, CMETHOD_f_LOCALS)
#define cmethod_set_locals(obj, val) SET_FIELD(obj, CMETHOD_f_LOCALS, val)
#define CMETHOD_f_LITERALS 5
#define cmethod_get_literals(obj) NTH_FIELD(obj, CMETHOD_f_LITERALS)
#define cmethod_set_literals(obj, val) SET_FIELD(obj, CMETHOD_f_LITERALS, val)
#define CMETHOD_f_ARGUMENTS 6
#define cmethod_get_arguments(obj) NTH_FIELD(obj, CMETHOD_f_ARGUMENTS)
#define cmethod_set_arguments(obj, val) SET_FIELD(obj, CMETHOD_f_ARGUMENTS, val)
#define CMETHOD_f_SCOPE 7
#define cmethod_get_scope(obj) NTH_FIELD(obj, CMETHOD_f_SCOPE)
#define cmethod_set_scope(obj, val) SET_FIELD(obj, CMETHOD_f_SCOPE, val)
#define CMETHOD_f_EXCEPTIONS 8
#define cmethod_get_exceptions(obj) NTH_FIELD(obj, CMETHOD_f_EXCEPTIONS)
#define cmethod_set_exceptions(obj, val) SET_FIELD(obj, CMETHOD_f_EXCEPTIONS, val)
#define CMETHOD_f_LINES 9
#define cmethod_get_lines(obj) NTH_FIELD(obj, CMETHOD_f_LINES)
#define cmethod_set_lines(obj, val) SET_FIELD(obj, CMETHOD_f_LINES, val)
#define CMETHOD_f_FILE 10
#define cmethod_get_file(obj) NTH_FIELD(obj, CMETHOD_f_FILE)
#define cmethod_set_file(obj, val) SET_FIELD(obj, CMETHOD_f_FILE, val)
#define CMETHOD_f_NAME 11
#define cmethod_get_name(obj) NTH_FIELD(obj, CMETHOD_f_NAME)
#define cmethod_set_name(obj, val) SET_FIELD(obj, CMETHOD_f_NAME, val)
#define cmethod_allocate(st) cmethod_allocate_with_extra(st, 0)
#define CMETHOD_FIELDS 12
OBJECT cmethod_allocate_with_extra(STATE, int extra);
#define tuple_allocate(st) tuple_allocate_with_extra(st, 0)
#define TUPLE_FIELDS 0
OBJECT tuple_allocate_with_extra(STATE, int extra);
#define ARRAY_f_TOTAL 0
#define array_get_total(obj) NTH_FIELD(obj, ARRAY_f_TOTAL)
#define array_set_total(obj, val) SET_FIELD(obj, ARRAY_f_TOTAL, val)
#define ARRAY_f_TUPLE 1
#define array_get_tuple(obj) NTH_FIELD(obj, ARRAY_f_TUPLE)
#define array_set_tuple(obj, val) SET_FIELD(obj, ARRAY_f_TUPLE, val)
#define array_allocate(st) array_allocate_with_extra(st, 0)
#define ARRAY_FIELDS 2
OBJECT array_allocate_with_extra(STATE, int extra);
#define blank_allocate(st) blank_allocate_with_extra(st, 0)
#define BLANK_FIELDS 0
OBJECT blank_allocate_with_extra(STATE, int extra);
#define symbol_allocate(st) symbol_allocate_with_extra(st, 0)
#define SYMBOL_FIELDS 0
OBJECT symbol_allocate_with_extra(STATE, int extra);
#define METHTBL_f_INSTANCE_VARIABLES 0
#define methtbl_get_instance_variables(obj) NTH_FIELD(obj, METHTBL_f_INSTANCE_VARIABLES)
#define methtbl_set_instance_variables(obj, val) SET_FIELD(obj, METHTBL_f_INSTANCE_VARIABLES, val)
#define METHTBL_f_KEYS 1
#define methtbl_get_keys(obj) NTH_FIELD(obj, METHTBL_f_KEYS)
#define methtbl_set_keys(obj, val) SET_FIELD(obj, METHTBL_f_KEYS, val)
#define METHTBL_f_VALUES 2
#define methtbl_get_values(obj) NTH_FIELD(obj, METHTBL_f_VALUES)
#define methtbl_set_values(obj, val) SET_FIELD(obj, METHTBL_f_VALUES, val)
#define METHTBL_f_BINS 3
#define methtbl_get_bins(obj) NTH_FIELD(obj, METHTBL_f_BINS)
#define methtbl_set_bins(obj, val) SET_FIELD(obj, METHTBL_f_BINS, val)
#define METHTBL_f_ENTRIES 4
#define methtbl_get_entries(obj) NTH_FIELD(obj, METHTBL_f_ENTRIES)
#define methtbl_set_entries(obj, val) SET_FIELD(obj, METHTBL_f_ENTRIES, val)
#define METHTBL_f_DEFAULT 5
#define methtbl_get_default(obj) NTH_FIELD(obj, METHTBL_f_DEFAULT)
#define methtbl_set_default(obj, val) SET_FIELD(obj, METHTBL_f_DEFAULT, val)
#define methtbl_allocate(st) methtbl_allocate_with_extra(st, 0)
#define METHTBL_FIELDS 6
OBJECT methtbl_allocate_with_extra(STATE, int extra);
#define bytearray_allocate(st) bytearray_allocate_with_extra(st, 0)
#define BYTEARRAY_FIELDS 0
OBJECT bytearray_allocate_with_extra(STATE, int extra);
#define METACLASS_f_INSTANCE_VARIABLES 0
#define metaclass_get_instance_variables(obj) NTH_FIELD(obj, METACLASS_f_INSTANCE_VARIABLES)
#define metaclass_set_instance_variables(obj, val) SET_FIELD(obj, METACLASS_f_INSTANCE_VARIABLES, val)
#define METACLASS_f_METHODS 1
#define metaclass_get_methods(obj) NTH_FIELD(obj, METACLASS_f_METHODS)
#define metaclass_set_methods(obj, val) SET_FIELD(obj, METACLASS_f_METHODS, val)
#define METACLASS_f_METHOD_CACHE 2
#define metaclass_get_method_cache(obj) NTH_FIELD(obj, METACLASS_f_METHOD_CACHE)
#define metaclass_set_method_cache(obj, val) SET_FIELD(obj, METACLASS_f_METHOD_CACHE, val)
#define METACLASS_f_NAME 3
#define metaclass_get_name(obj) NTH_FIELD(obj, METACLASS_f_NAME)
#define metaclass_set_name(obj, val) SET_FIELD(obj, METACLASS_f_NAME, val)
#define METACLASS_f_CONSTANTS 4
#define metaclass_get_constants(obj) NTH_FIELD(obj, METACLASS_f_CONSTANTS)
#define metaclass_set_constants(obj, val) SET_FIELD(obj, METACLASS_f_CONSTANTS, val)
#define METACLASS_f_SUPERCLASS 5
#define metaclass_get_superclass(obj) NTH_FIELD(obj, METACLASS_f_SUPERCLASS)
#define metaclass_set_superclass(obj, val) SET_FIELD(obj, METACLASS_f_SUPERCLASS, val)
#define METACLASS_f_INSTANCE_FIELDS 6
#define metaclass_get_instance_fields(obj) NTH_FIELD(obj, METACLASS_f_INSTANCE_FIELDS)
#define metaclass_set_instance_fields(obj, val) SET_FIELD(obj, METACLASS_f_INSTANCE_FIELDS, val)
#define METACLASS_f_INSTANCE_FLAGS 7
#define metaclass_get_instance_flags(obj) NTH_FIELD(obj, METACLASS_f_INSTANCE_FLAGS)
#define metaclass_set_instance_flags(obj, val) SET_FIELD(obj, METACLASS_f_INSTANCE_FLAGS, val)
#define METACLASS_f_ATTACHED_INSTANCE 8
#define metaclass_get_attached_instance(obj) NTH_FIELD(obj, METACLASS_f_ATTACHED_INSTANCE)
#define metaclass_set_attached_instance(obj, val) SET_FIELD(obj, METACLASS_f_ATTACHED_INSTANCE, val)
#define metaclass_allocate(st) metaclass_allocate_with_extra(st, 0)
#define METACLASS_FIELDS 9
OBJECT metaclass_allocate_with_extra(STATE, int extra);
#define METHCTX_f_INSTANCE_VARIABLES 0
#define methctx_get_instance_variables(obj) NTH_FIELD(obj, METHCTX_f_INSTANCE_VARIABLES)
#define methctx_set_instance_variables(obj, val) SET_FIELD(obj, METHCTX_f_INSTANCE_VARIABLES, val)
#define METHCTX_f_SENDER 1
#define methctx_get_sender(obj) NTH_FIELD(obj, METHCTX_f_SENDER)
#define methctx_set_sender(obj, val) SET_FIELD(obj, METHCTX_f_SENDER, val)
#define METHCTX_f_IP 2
#define methctx_get_ip(obj) NTH_FIELD(obj, METHCTX_f_IP)
#define methctx_set_ip(obj, val) SET_FIELD(obj, METHCTX_f_IP, val)
#define METHCTX_f_SP 3
#define methctx_get_sp(obj) NTH_FIELD(obj, METHCTX_f_SP)
#define methctx_set_sp(obj, val) SET_FIELD(obj, METHCTX_f_SP, val)
#define METHCTX_f_BLOCK 4
#define methctx_get_block(obj) NTH_FIELD(obj, METHCTX_f_BLOCK)
#define methctx_set_block(obj, val) SET_FIELD(obj, METHCTX_f_BLOCK, val)
#define METHCTX_f_RAISEABLE 5
#define methctx_get_raiseable(obj) NTH_FIELD(obj, METHCTX_f_RAISEABLE)
#define methctx_set_raiseable(obj, val) SET_FIELD(obj, METHCTX_f_RAISEABLE, val)
#define METHCTX_f_METHOD 6
#define methctx_get_method(obj) NTH_FIELD(obj, METHCTX_f_METHOD)
#define methctx_set_method(obj, val) SET_FIELD(obj, METHCTX_f_METHOD, val)
#define METHCTX_f_BYTECODES 7
#define methctx_get_bytecodes(obj) NTH_FIELD(obj, METHCTX_f_BYTECODES)
#define methctx_set_bytecodes(obj, val) SET_FIELD(obj, METHCTX_f_BYTECODES, val)
#define METHCTX_f_LITERALS 8
#define methctx_get_literals(obj) NTH_FIELD(obj, METHCTX_f_LITERALS)
#define methctx_set_literals(obj, val) SET_FIELD(obj, METHCTX_f_LITERALS, val)
#define METHCTX_f_RECEIVER 9
#define methctx_get_receiver(obj) NTH_FIELD(obj, METHCTX_f_RECEIVER)
#define methctx_set_receiver(obj, val) SET_FIELD(obj, METHCTX_f_RECEIVER, val)
#define METHCTX_f_LOCALS 10
#define methctx_get_locals(obj) NTH_FIELD(obj, METHCTX_f_LOCALS)
#define methctx_set_locals(obj, val) SET_FIELD(obj, METHCTX_f_LOCALS, val)
#define METHCTX_f_ARGCOUNT 11
#define methctx_get_argcount(obj) NTH_FIELD(obj, METHCTX_f_ARGCOUNT)
#define methctx_set_argcount(obj, val) SET_FIELD(obj, METHCTX_f_ARGCOUNT, val)
#define METHCTX_f_NAME 12
#define methctx_get_name(obj) NTH_FIELD(obj, METHCTX_f_NAME)
#define methctx_set_name(obj, val) SET_FIELD(obj, METHCTX_f_NAME, val)
#define methctx_allocate(st) methctx_allocate_with_extra(st, 0)
#define METHCTX_FIELDS 13
OBJECT methctx_allocate_with_extra(STATE, int extra);
#define CLASS_f_INSTANCE_VARIABLES 0
#define class_get_instance_variables(obj) NTH_FIELD(obj, CLASS_f_INSTANCE_VARIABLES)
#define class_set_instance_variables(obj, val) SET_FIELD(obj, CLASS_f_INSTANCE_VARIABLES, val)
#define CLASS_f_METHODS 1
#define class_get_methods(obj) NTH_FIELD(obj, CLASS_f_METHODS)
#define class_set_methods(obj, val) SET_FIELD(obj, CLASS_f_METHODS, val)
#define CLASS_f_METHOD_CACHE 2
#define class_get_method_cache(obj) NTH_FIELD(obj, CLASS_f_METHOD_CACHE)
#define class_set_method_cache(obj, val) SET_FIELD(obj, CLASS_f_METHOD_CACHE, val)
#define CLASS_f_NAME 3
#define class_get_name(obj) NTH_FIELD(obj, CLASS_f_NAME)
#define class_set_name(obj, val) SET_FIELD(obj, CLASS_f_NAME, val)
#define CLASS_f_CONSTANTS 4
#define class_get_constants(obj) NTH_FIELD(obj, CLASS_f_CONSTANTS)
#define class_set_constants(obj, val) SET_FIELD(obj, CLASS_f_CONSTANTS, val)
#define CLASS_f_SUPERCLASS 5
#define class_get_superclass(obj) NTH_FIELD(obj, CLASS_f_SUPERCLASS)
#define class_set_superclass(obj, val) SET_FIELD(obj, CLASS_f_SUPERCLASS, val)
#define CLASS_f_INSTANCE_FIELDS 6
#define class_get_instance_fields(obj) NTH_FIELD(obj, CLASS_f_INSTANCE_FIELDS)
#define class_set_instance_fields(obj, val) SET_FIELD(obj, CLASS_f_INSTANCE_FIELDS, val)
#define CLASS_f_INSTANCE_FLAGS 7
#define class_get_instance_flags(obj) NTH_FIELD(obj, CLASS_f_INSTANCE_FLAGS)
#define class_set_instance_flags(obj, val) SET_FIELD(obj, CLASS_f_INSTANCE_FLAGS, val)
#define class_allocate(st) class_allocate_with_extra(st, 0)
#define CLASS_FIELDS 8
OBJECT class_allocate_with_extra(STATE, int extra);
#define IO_f_DESCRIPTOR 0
#define io_get_descriptor(obj) NTH_FIELD(obj, IO_f_DESCRIPTOR)
#define io_set_descriptor(obj, val) SET_FIELD(obj, IO_f_DESCRIPTOR, val)
#define io_allocate(st) io_allocate_with_extra(st, 0)
#define IO_FIELDS 1
OBJECT io_allocate_with_extra(STATE, int extra);
#define HASH_f_INSTANCE_VARIABLES 0
#define hash_get_instance_variables(obj) NTH_FIELD(obj, HASH_f_INSTANCE_VARIABLES)
#define hash_set_instance_variables(obj, val) SET_FIELD(obj, HASH_f_INSTANCE_VARIABLES, val)
#define HASH_f_KEYS 1
#define hash_get_keys(obj) NTH_FIELD(obj, HASH_f_KEYS)
#define hash_set_keys(obj, val) SET_FIELD(obj, HASH_f_KEYS, val)
#define HASH_f_VALUES 2
#define hash_get_values(obj) NTH_FIELD(obj, HASH_f_VALUES)
#define hash_set_values(obj, val) SET_FIELD(obj, HASH_f_VALUES, val)
#define HASH_f_BINS 3
#define hash_get_bins(obj) NTH_FIELD(obj, HASH_f_BINS)
#define hash_set_bins(obj, val) SET_FIELD(obj, HASH_f_BINS, val)
#define HASH_f_ENTRIES 4
#define hash_get_entries(obj) NTH_FIELD(obj, HASH_f_ENTRIES)
#define hash_set_entries(obj, val) SET_FIELD(obj, HASH_f_ENTRIES, val)
#define HASH_f_DEFAULT 5
#define hash_get_default(obj) NTH_FIELD(obj, HASH_f_DEFAULT)
#define hash_set_default(obj, val) SET_FIELD(obj, HASH_f_DEFAULT, val)
#define hash_allocate(st) hash_allocate_with_extra(st, 0)
#define HASH_FIELDS 6
OBJECT hash_allocate_with_extra(STATE, int extra);
#define STRING_f_BYTES 0
#define string_get_bytes(obj) NTH_FIELD(obj, STRING_f_BYTES)
#define string_set_bytes(obj, val) SET_FIELD(obj, STRING_f_BYTES, val)
#define STRING_f_CHARACTERS 1
#define string_get_characters(obj) NTH_FIELD(obj, STRING_f_CHARACTERS)
#define string_set_characters(obj, val) SET_FIELD(obj, STRING_f_CHARACTERS, val)
#define STRING_f_ENCODING 2
#define string_get_encoding(obj) NTH_FIELD(obj, STRING_f_ENCODING)
#define string_set_encoding(obj, val) SET_FIELD(obj, STRING_f_ENCODING, val)
#define STRING_f_DATA 3
#define string_get_data(obj) NTH_FIELD(obj, STRING_f_DATA)
#define string_set_data(obj, val) SET_FIELD(obj, STRING_f_DATA, val)
#define string_allocate(st) string_allocate_with_extra(st, 0)
#define STRING_FIELDS 4
OBJECT string_allocate_with_extra(STATE, int extra);
#define SYMTBL_f_INSTANCE_VARIABLES 0
#define symtbl_get_instance_variables(obj) NTH_FIELD(obj, SYMTBL_f_INSTANCE_VARIABLES)
#define symtbl_set_instance_variables(obj, val) SET_FIELD(obj, SYMTBL_f_INSTANCE_VARIABLES, val)
#define SYMTBL_f_SYMBOLS 1
#define symtbl_get_symbols(obj) NTH_FIELD(obj, SYMTBL_f_SYMBOLS)
#define symtbl_set_symbols(obj, val) SET_FIELD(obj, SYMTBL_f_SYMBOLS, val)
#define SYMTBL_f_STRINGS 2
#define symtbl_get_strings(obj) NTH_FIELD(obj, SYMTBL_f_STRINGS)
#define symtbl_set_strings(obj, val) SET_FIELD(obj, SYMTBL_f_STRINGS, val)
#define symtbl_allocate(st) symtbl_allocate_with_extra(st, 0)
#define SYMTBL_FIELDS 3
OBJECT symtbl_allocate_with_extra(STATE, int extra);
#define BLOKCTX_f_INSTANCE_VARIABLES 0
#define blokctx_get_instance_variables(obj) NTH_FIELD(obj, BLOKCTX_f_INSTANCE_VARIABLES)
#define blokctx_set_instance_variables(obj, val) SET_FIELD(obj, BLOKCTX_f_INSTANCE_VARIABLES, val)
#define BLOKCTX_f_SENDER 1
#define blokctx_get_sender(obj) NTH_FIELD(obj, BLOKCTX_f_SENDER)
#define blokctx_set_sender(obj, val) SET_FIELD(obj, BLOKCTX_f_SENDER, val)
#define BLOKCTX_f_IP 2
#define blokctx_get_ip(obj) NTH_FIELD(obj, BLOKCTX_f_IP)
#define blokctx_set_ip(obj, val) SET_FIELD(obj, BLOKCTX_f_IP, val)
#define BLOKCTX_f_SP 3
#define blokctx_get_sp(obj) NTH_FIELD(obj, BLOKCTX_f_SP)
#define blokctx_set_sp(obj, val) SET_FIELD(obj, BLOKCTX_f_SP, val)
#define BLOKCTX_f_BLOCK 4
#define blokctx_get_block(obj) NTH_FIELD(obj, BLOKCTX_f_BLOCK)
#define blokctx_set_block(obj, val) SET_FIELD(obj, BLOKCTX_f_BLOCK, val)
#define BLOKCTX_f_RAISEABLE 5
#define blokctx_get_raiseable(obj) NTH_FIELD(obj, BLOKCTX_f_RAISEABLE)
#define blokctx_set_raiseable(obj, val) SET_FIELD(obj, BLOKCTX_f_RAISEABLE, val)
#define BLOKCTX_f_HOME 6
#define blokctx_get_home(obj) NTH_FIELD(obj, BLOKCTX_f_HOME)
#define blokctx_set_home(obj, val) SET_FIELD(obj, BLOKCTX_f_HOME, val)
#define BLOKCTX_f_LAST_OP 7
#define blokctx_get_last_op(obj) NTH_FIELD(obj, BLOKCTX_f_LAST_OP)
#define blokctx_set_last_op(obj, val) SET_FIELD(obj, BLOKCTX_f_LAST_OP, val)
#define BLOKCTX_f_START_OP 8
#define blokctx_get_start_op(obj) NTH_FIELD(obj, BLOKCTX_f_START_OP)
#define blokctx_set_start_op(obj, val) SET_FIELD(obj, BLOKCTX_f_START_OP, val)
#define blokctx_allocate(st) blokctx_allocate_with_extra(st, 0)
#define BLOKCTX_FIELDS 9
OBJECT blokctx_allocate_with_extra(STATE, int extra);
#define MODULE_f_INSTANCE_VARIABLES 0
#define module_get_instance_variables(obj) NTH_FIELD(obj, MODULE_f_INSTANCE_VARIABLES)
#define module_set_instance_variables(obj, val) SET_FIELD(obj, MODULE_f_INSTANCE_VARIABLES, val)
#define MODULE_f_METHODS 1
#define module_get_methods(obj) NTH_FIELD(obj, MODULE_f_METHODS)
#define module_set_methods(obj, val) SET_FIELD(obj, MODULE_f_METHODS, val)
#define MODULE_f_METHOD_CACHE 2
#define module_get_method_cache(obj) NTH_FIELD(obj, MODULE_f_METHOD_CACHE)
#define module_set_method_cache(obj, val) SET_FIELD(obj, MODULE_f_METHOD_CACHE, val)
#define MODULE_f_NAME 3
#define module_get_name(obj) NTH_FIELD(obj, MODULE_f_NAME)
#define module_set_name(obj, val) SET_FIELD(obj, MODULE_f_NAME, val)
#define MODULE_f_CONSTANTS 4
#define module_get_constants(obj) NTH_FIELD(obj, MODULE_f_CONSTANTS)
#define module_set_constants(obj, val) SET_FIELD(obj, MODULE_f_CONSTANTS, val)
#define module_allocate(st) module_allocate_with_extra(st, 0)
#define MODULE_FIELDS 5
OBJECT module_allocate_with_extra(STATE, int extra);
#define OBJECT_f_INSTANCE_VARIABLES 0
#define object_get_instance_variables(obj) NTH_FIELD(obj, OBJECT_f_INSTANCE_VARIABLES)
#define object_set_instance_variables(obj, val) SET_FIELD(obj, OBJECT_f_INSTANCE_VARIABLES, val)
#define object_allocate(st) object_allocate_with_extra(st, 0)
#define OBJECT_FIELDS 1
OBJECT object_allocate_with_extra(STATE, int extra);
OBJECT _cmethod_basic_class(STATE, OBJECT sup);
OBJECT _cmethod_class(STATE, OBJECT sup);
OBJECT _tuple_basic_class(STATE, OBJECT sup);
OBJECT _tuple_class(STATE, OBJECT sup);
OBJECT _array_basic_class(STATE, OBJECT sup);
OBJECT _array_class(STATE, OBJECT sup);
OBJECT _blank_basic_class(STATE, OBJECT sup);
OBJECT _blank_class(STATE, OBJECT sup);
OBJECT _symbol_basic_class(STATE, OBJECT sup);
OBJECT _symbol_class(STATE, OBJECT sup);
OBJECT _methtbl_basic_class(STATE, OBJECT sup);
OBJECT _methtbl_class(STATE, OBJECT sup);
OBJECT _bytearray_basic_class(STATE, OBJECT sup);
OBJECT _bytearray_class(STATE, OBJECT sup);
OBJECT _metaclass_basic_class(STATE, OBJECT sup);
OBJECT _metaclass_class(STATE, OBJECT sup);
OBJECT _methctx_basic_class(STATE, OBJECT sup);
OBJECT _methctx_class(STATE, OBJECT sup);
OBJECT _class_basic_class(STATE, OBJECT sup);
OBJECT _class_class(STATE, OBJECT sup);
OBJECT _io_basic_class(STATE, OBJECT sup);
OBJECT _io_class(STATE, OBJECT sup);
OBJECT _hash_basic_class(STATE, OBJECT sup);
OBJECT _hash_class(STATE, OBJECT sup);
OBJECT _string_basic_class(STATE, OBJECT sup);
OBJECT _string_class(STATE, OBJECT sup);
OBJECT _symtbl_basic_class(STATE, OBJECT sup);
OBJECT _symtbl_class(STATE, OBJECT sup);
OBJECT _blokctx_basic_class(STATE, OBJECT sup);
OBJECT _blokctx_class(STATE, OBJECT sup);
OBJECT _module_basic_class(STATE, OBJECT sup);
OBJECT _module_class(STATE, OBJECT sup);
OBJECT _object_basic_class(STATE, OBJECT sup);
OBJECT _object_class(STATE, OBJECT sup);
