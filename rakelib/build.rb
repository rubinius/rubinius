def llvm_configure
  case Rubinius::BUILD_CONFIG[:llvm]
  when :svn, :prebuilt
    "vm/external_libs/llvm/Release/bin/llvm-config"
  when :config
    Rubinius::BUILD_CONFIG[:llvm_configure]
  else
    raise "Tried to use LLVM unconfigure!"
  end
end

def build_perl
  Rubinius::BUILD_CONFIG[:build_perl]
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

  @llvm_flags += `#{build_perl} #{llvm_configure} --cflags`.split(/\s+/)
  @llvm_flags.delete_if { |e| e.index("-O") == 0 }
  @llvm_flags
end

def llvm_link_flags
  return "" unless LLVM_ENABLE

  `#{build_perl} #{llvm_configure} --ldflags`.strip
end

def llvm_lib_files
  return [] unless LLVM_ENABLE

  files = `#{build_perl} #{llvm_configure} --libfiles`.split(/\s+/)
  files.select { |f| File.file? f }
end

def llvm_version
  `#{build_perl} #{llvm_configure} --version`.strip
end

def host_triple
  Rubinius::BUILD_CONFIG[:host]
end

def gcc_major_version
  `#{$CC} -dumpversion`.strip.split(".")[0,2].join(".")
end

def llvm_config_flags
  "--build=#{host_triple} --host=#{host_triple} " \
  "--enable-optimized --enable-targets=host-only"
end
