find_package(Ruby)
find_program(BISON bison)

if(NOT BISON)
  message(SEND_ERROR "Bison not found")
endif(NOT BISON)

CHECK_FUNCTION_EXISTS(strlcat HAVE_STRLCAT)
CHECK_FUNCTION_EXISTS(strlcpy HAVE_STRLCPY)

set(lib_src
  ar.c archive.c array.c baker.c bignum.c bootstrap.c bytearray.c class.c
  compress.c config_hash.c cpu.c cpu_cache.c cpu_event.c cpu_instructions.c
  cpu_marshal.c cpu_primitives.c cpu_sample.c cpu_task.c environment.c
  ffi_util.c float.c grammar_runtime.c hash.c heap.c io.c list.c
  log.c machine.c marksweep.c metaclass.c methctx.c methtbl.c module.c
  numeric.c object.c object_memory.c regexp.c rubinius.c selector.c sendsite.c
  sha1.c state.c string.c symbol.c tuple.c var_table.c)

set(subtend_src
  subtend/ffi_amd64.c subtend/ffi.c subtend/ffi_x86.c subtend/handle.c
  subtend/library.c subtend/nmc.c subtend/nmethod.c subtend/PortableUContext.c
  subtend/ruby.c subtend/setup.c subtend/PortableUContext_asm.S)


if(NOT HAVE_STRLCAT)
  set(strl_src ${strl_src} strlcat.c)
endif(NOT HAVE_STRLCAT)

if(NOT HAVE_STRLCPY)
  set(strl_src ${strl_src} strlcpy.c)
endif(NOT HAVE_STRLCPY)


#### generate instruction_names.c, instruction_names.h, instructions.gen,
####          instruction_dt.gen, instruction_funcs.gen

set(instructions ${CMAKE_CURRENT_SOURCE_DIR}/instructions.rb)
set(inames_c     ${CMAKE_CURRENT_BINARY_DIR}/instruction_names.c)
set(inames_h     ${CMAKE_CURRENT_BINARY_DIR}/instruction_names.h)
set(i_gen        ${CMAKE_CURRENT_BINARY_DIR}/instructions.gen)
set(idt_gen      ${CMAKE_CURRENT_BINARY_DIR}/instruction_dt.gen)
set(ifunc_gen    ${CMAKE_CURRENT_BINARY_DIR}/instruction_funcs.gen)

add_custom_command(
  OUTPUT ${inames_c} ${inames_h} ${i_gen} ${idt_gen} ${ifunc_gen}
  COMMAND ${RUBY_EXECUTABLE} ${instructions} > ${i_gen}
  DEPENDS ${instructions}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


#### generate system_primitives.gen

set(primitives   ${CMAKE_CURRENT_SOURCE_DIR}/primitives.rb)
set(p_sysgen     ${CMAKE_CURRENT_BINARY_DIR}/system_primitives.gen)
set(p_implgen    ${CMAKE_CURRENT_BINARY_DIR}/primitive_implementation.gen)
set(p_indexes_h  ${CMAKE_CURRENT_BINARY_DIR}/primitive_indexes.h)
set(p_util_h     ${CMAKE_CURRENT_BINARY_DIR}/primitive_util.h)

add_custom_command(
  OUTPUT ${p_sysgen} ${p_implgen} ${p_indexes_h} ${p_util_h}
  COMMAND ${RUBY_EXECUTABLE} ${primitives} > ${p_sysgen}
  DEPENDS ${primitives}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


#### generate auto.h and auto.c

set(genheader ${CMAKE_CURRENT_SOURCE_DIR}/genheader.rb)
set(auto_h    ${CMAKE_CURRENT_BINARY_DIR}/auto.h)
set(auto_c    ${CMAKE_CURRENT_BINARY_DIR}/auto.c)

add_custom_command(
  OUTPUT ${auto_c} ${auto_h}
  COMMAND ${RUBY_EXECUTABLE} ${genheader} > ${auto_c}
  DEPENDS ${genheader} 
          ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/compiler/system_hints.rb
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

### generate node_types.c and node_types.h
set(ntypes   ${CMAKE_CURRENT_SOURCE_DIR}/node_types.rb)
set(ntypes_h ${CMAKE_CURRENT_BINARY_DIR}/node_types.h)
set(ntypes_c ${CMAKE_CURRENT_BINARY_DIR}/node_types.c)

add_custom_command(
  OUTPUT ${ntypes_h} ${ntypes_c}
  COMMAND ${RUBY_EXECUTABLE} ${ntypes}
  DEPENDS ${ntypes}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

### generate grammar.c
set(grammar_y ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y)
set(grammar_c ${CMAKE_CURRENT_BINARY_DIR}/grammar.c)

add_custom_command(
  OUTPUT ${grammar_c} 
  COMMAND ${BISON} -o ${grammar_c} ${grammar_y}
  DEPENDS ${grammar_y})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../..)

foreach(d libbstring libcchash libev libmpa libmquark libtommath libzip/lib 
    lightning onig)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external_libs/${d})
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/../external_libs/${d})
endforeach(d)

add_library(rubinius SHARED 
  ${lib_src} 
  ${subtend_src} 
  ${inames_c} ${inames_h} ${i_gen} ${idt_gen} ${ifunc_gen}

  ${p_sysgen} ${p_implgen} ${p_indexes_h} ${p_util_h}
  ${auto_c} ${auto_h}
  ${ntypes_c} ${ntypes_h}
  ${grammar_c})

target_link_libraries(rubinius 
  bstring cchash ev ltdl ptr_array mquark tommath zip onig)

#grammar.o:
#        $(COMP) $(CFLAGS) -c grammar.c


#subtend/ffi.o: subtend/ffi.c
#        $(COMP) $(CFLAGS) $(OPTIMIZATIONS) -Wno-unused-variable -Wno-unused-value -c $< -o $@

#grammar.c: grammar.y
#        bison -o grammar.c grammar.y
