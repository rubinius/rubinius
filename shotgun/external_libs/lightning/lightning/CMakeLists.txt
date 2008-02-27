# symlink headers
macro(lightning_symlink src dst)
  set(old ${CMAKE_CURRENT_SOURCE_DIR}/${src}.h)
  set(new ${CMAKE_CURRENT_BINARY_DIR}/${dst}.h)

  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${old} ${new})
endmacro(lightning_symlink)

foreach (h asm fp core)
  lightning_symlink(${cpu}/${h}${suffix} ${h})
endforeach(h)

lightning_symlink(${cpu}/funcs funcs)

if(cpu STREQUAL "i386")
  lightning_symlink(i386/asm-i386 asm-i386)
  lightning_symlink(i386/core-i386 core-i386)
endif(cpu STREQUAL "i386")
