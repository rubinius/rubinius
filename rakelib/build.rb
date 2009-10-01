def llvm_configure
  case Rubinius::BUILD_CONFIG[:llvm]
  when :svn, :prebuilt
    "vm/external_libs/llvm/Release/bin/llvm-config"
  when :config
    "llvm-config"
  else
    raise "Tried to use LLVM unconfigure!"
  end
end

def llvm_flags
  return [] unless LLVM_ENABLE

  return @llvm_flags if defined?(@llvm_flags)

  case Rubinius::BUILD_CONFIG[:llvm]
  when :svn, :prebuilt
    @llvm_flags = ["-Ivm/external_libs/llvm/include"]
  else
    @llvm_flags = []
  end

  @llvm_flags += `#{llvm_configure} --cflags`.split(/\s+/)
  @llvm_flags.delete_if { |e| e.index("-O") == 0 }
  @llvm_flags
end

def llvm_link_flags
  return "" unless LLVM_ENABLE

  `#{llvm_configure} --ldflags`.strip
end

def llvm_lib_files
  return [] unless LLVM_ENABLE

  files = `#{llvm_configure} --libfiles all`.split(/\s+/)
  files.select { |f| File.file? f }
end

def host_triple
  Rubinius::BUILD_CONFIG[:arch]
end

def llvm_config_flags
  "--build=#{host_triple} --host=#{host_triple} " \
  "--enable-optimized --enable-targets=host-only"
end
