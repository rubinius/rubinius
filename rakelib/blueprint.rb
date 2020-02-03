Daedalus.blueprint do |i|
  i.base_path "#{Rubinius::BUILD_CONFIG[:sourcedir]}"
  i.artifacts_path "#{Rubinius::BUILD_CONFIG[:sourcedir]}/build/artifacts"

  gcc = i.gcc!(Rubinius::BUILD_CONFIG[:cc],
               Rubinius::BUILD_CONFIG[:cxx],
               Rubinius::BUILD_CONFIG[:ldshared],
               Rubinius::BUILD_CONFIG[:ldsharedxx])

  # First define all flags that all code needs to be build with.

  # -fno-omit-frame-pointer is needed to get a backtrace on FreeBSD.
  # It is enabled by default on macOS, on the other hand, not on Linux.
  # To use same build flags across platforms, it is added explicitly.
  gcc.cflags << "-pipe -fPIC -fno-omit-frame-pointer -g"

  # Due to a Clang bug (http://llvm.org/bugs/show_bug.cgi?id=9825),
  # -mno-omit-leaf-frame-pointer is needed for Clang on Linux.
  # On other combinations of platform and compiler, this flag is implicitly
  # assumed from -fno-omit-frame-pointer. To use same build flags across
  # platforms, -mno-omit-leaf-frame-pointer is added explicitly.
  gcc.cflags << "-mno-omit-leaf-frame-pointer" if Rubinius::BUILD_CONFIG[:cc] == "clang"

  # We don't need RTTI information, so disable it. This makes it possible
  # to link Rubinius to an LLVM build with RTTI disabled. We also enable
  # visibility-inlines-hidden for slightly smaller code size and prevents
  # warnings when LLVM is also built with this flag.
  gcc.cxxflags << "-fno-rtti -fvisibility-inlines-hidden"

  gcc.cflags << Rubinius::BUILD_CONFIG[:system_cflags]
  gcc.cflags << Rubinius::BUILD_CONFIG[:user_cflags]
  gcc.cxxflags << Rubinius::BUILD_CONFIG[:system_cxxflags]
  gcc.cxxflags << Rubinius::BUILD_CONFIG[:user_cxxflags]

  if Rubinius::BUILD_CONFIG[:debug_build]
    gcc.cflags << "-O0"
    gcc.mtime_only = true
  else
    gcc.cflags << "-O2"
  end

  if ENV['POKE']
    gcc.mtime_only = true
  end

  Rubinius::BUILD_CONFIG[:defines].each do |flag|
    gcc.cflags << "-D#{flag}"
  end

  gcc.ldflags << "-lm"

  if Rubinius::BUILD_CONFIG[:dtrace]
    gcc.ldflags << "-lelf"
    gcc.ldflags << "machine/dtrace/probes.o"

    gcc.add_pre_link "rm -f machine/dtrace/probes.o"

    blk = lambda { |files| files.select { |f| f =~ %r[machine/.*\.o$] } }
    cmd = "dtrace -G -s machine/dtrace/probes.d -o machine/dtrace/probes.o %objects%"
    gcc.add_pre_link(cmd, &blk)
  end

  make = Rubinius::BUILD_CONFIG[:build_make]

  if RUBY_PLATFORM =~ /bsd/ and
      Rubinius::BUILD_CONFIG[:defines].include?('HAS_EXECINFO')
    gcc.ldflags << "-lexecinfo"
  end

  gcc.ldflags << Rubinius::BUILD_CONFIG[:system_ldflags]
  gcc.ldflags << Rubinius::BUILD_CONFIG[:user_ldflags]

  # Files
  subdirs = %w[ class diagnostics capi util instructions memory jit missing ].map do |x|
    "machine/#{x}/**/*.{cpp,c}"
  end

  files = i.source_files "machine/*.{cpp,c}", *subdirs
  files.delete_if { |f| %r[.*machine/main.cpp] =~ f.path }

  Dir["machine/interpreter/*.cpp"].each do |name|
    files << InstructionSourceFile.new(name)
  end

  perl = Rubinius::BUILD_CONFIG[:build_perl] || "perl"

  src = Rubinius::BUILD_CONFIG[:sourcedir]

  # Libraries
  gcc.cflags << "-I#{src}/build/libraries/rapidjson"

  ltm = i.external_lib "build/libraries/libtommath" do |l|
    l.cflags = ["-I#{src}/build/libraries/libtommath"] + gcc.cflags
    l.objects = [l.file("libtommath.a")]
    l.to_build do |x|
      x.command make
    end
  end
  gcc.add_library ltm
  files << ltm

  oniguruma = i.library_group "build/libraries/oniguruma" do |g|
    g.depends_on "config.h", "configure"

    gcc.cflags.unshift "-I#{src}/build/libraries/oniguruma"
    g.cflags = [ "-DHAVE_CONFIG_H", "-I#{src}/machine/include/capi" ]
    g.cflags += gcc.cflags

    g.static_library "libonig" do |l|
      l.source_files "*.c", "enc/*.c"
    end

    g.shared_library "enc/trans/big5"
    g.shared_library "enc/trans/chinese"
    g.shared_library "enc/trans/emoji"
    g.shared_library "enc/trans/emoji_iso2022_kddi"
    g.shared_library "enc/trans/emoji_sjis_docomo"
    g.shared_library "enc/trans/emoji_sjis_kddi"
    g.shared_library "enc/trans/emoji_sjis_softbank"
    g.shared_library "enc/trans/escape"
    g.shared_library "enc/trans/gb18030"
    g.shared_library "enc/trans/gbk"
    g.shared_library "enc/trans/iso2022"
    g.shared_library "enc/trans/japanese"
    g.shared_library "enc/trans/japanese_euc"
    g.shared_library "enc/trans/japanese_sjis"
    g.shared_library "enc/trans/korean"
    g.shared_library "enc/trans/newline"
    g.shared_library "enc/trans/single_byte"
    g.shared_library "enc/trans/utf8_mac"
    g.shared_library "enc/trans/utf_16_32"
  end
  files << oniguruma

  double_conversion = i.external_lib "build/libraries/double-conversion" do |l|
    l.cflags = ["-Ibuild/libraries/double-conversion/src"] + gcc.cflags
    l.objects = [l.file("libdoubleconversion.a")]
    l.to_build do |x|
      x.command make
    end
  end
  gcc.add_library double_conversion
  files << double_conversion

  ffi = i.external_lib "build/libraries/libffi" do |l|
    l.cflags = ["-I#{src}/build/libraries/libffi/include"] + gcc.cflags
    l.objects = [l.file(".libs/libffi.a")]
    l.to_build do |x|
      x.command "sh -c './configure --disable-builddir'" unless File.exist?("Makefile")
      x.command make
    end
  end
  gcc.add_library ffi
  files << ffi

  if Rubinius::BUILD_CONFIG[:vendor_zlib]
    zlib = i.external_lib "build/libraries/zlib" do |l|
      l.cflags = ["-I#{src}/build/libraries/zlib"] + gcc.cflags
      l.objects = [l.file("libz.a")]
      l.to_build do |x|
        unless File.exist?("Makefile") and File.exist?("zconf.h")
          x.command "sh -c ./configure"
        end

        if Rubinius::BUILD_CONFIG[:windows]
          x.command "make -f win32/Makefile.gcc"
        else
          x.command make
        end
      end
    end
    gcc.add_library zlib
    files << zlib
  else
    gcc.ldflags << "-lz"
  end

  if Rubinius::BUILD_CONFIG[:vendor_libsodium]
    sodium = i.external_lib "build/libraries/libsodium" do |l|
      l.cflags = ["-I#{src}/build/libraries/libsodium/src/libsodium/include"] + gcc.cflags
      l.objects = [l.file("src/libsodium/.libs/libsodium.a")]
      l.to_build do |x|
        unless File.exist?("Makefile")
          x.command "sh -c ./configure"
        end

        x.command make
      end
    end
    gcc.add_library sodium
    files << sodium
  end

  if Rubinius::BUILD_CONFIG[:windows]
    winp = i.external_lib "build/libraries/winpthreads" do |l|
      l.cflags = ["-I#{src}/build/libraries/winpthreads/include"] + gcc.cflags
      l.objects = [l.file("libpthread.a")]
      l.to_build do |x|
        x.command "sh -c ./configure" unless File.exist?("Makefile")
        x.command make
      end
    end

    gcc.add_library winp

    files << winp
  end

  conf = Rubinius::BUILD_CONFIG[:llvm_configure]

  include_dir = `#{conf} --includedir`.chomp
  gcc.cflags << "-I#{include_dir}"
  gcc.cxxflags << Rubinius::BUILD_CONFIG[:llvm_cxxflags]

  flags = `#{conf} --cflags`.strip.split(/\s+/)

  flags.keep_if { |x| x =~ /^-[DI]/ }

  flags.delete_if { |x| x.index("-O") == 0 }
  flags.delete_if { |x| x =~ /-D__STDC/ }
  flags.delete_if { |x| x == "-DNDEBUG" }
  flags.delete_if { |x| x == "-fomit-frame-pointer" }
  flags.delete_if { |x| x == "-pedantic" }
  flags.delete_if { |x| x == "-W" }
  flags.delete_if { |x| x == "-Wextra" }

  # llvm-config may leak FORTIFY_SOURCE in the CFLAGS list on certain
  # platforms. If this is the case then debug builds will fail. Sadly there's
  # no strict guarantee on how LLVM formats this option, hence the Regexp.
  #
  # For example, on CentOS the option is added as -Wp,-D_FORTIFY_SOURCE=2.
  # There's no strict guarantee that I know of that it will always be this
  # exact format.
  if Rubinius::BUILD_CONFIG[:debug_build]
    flags.delete_if { |x| x =~ /_FORTIFY_SOURCE/ }
  end

  flags << "-DENABLE_LLVM"

  ldflags = Rubinius::BUILD_CONFIG[:llvm_ldflags]

  if Rubinius::BUILD_CONFIG[:llvm_shared_objs]
    objects = Rubinius::BUILD_CONFIG[:llvm_shared_objs]
  else
    objects = `#{conf} --libs`.strip.split(/\s+/)
  end

  if Rubinius::BUILD_CONFIG[:windows]
    ldflags = ldflags.sub(%r[-L/([a-zA-Z])/], '-L\1:/')

    objects.select do |f|
      f.sub!(%r[^/([a-zA-Z])/], '\1:/')
      File.file? f
    end
  end

  gcc.cflags.concat flags
  gcc.ldflags.concat objects

  gcc.ldflags << ldflags

  # Make sure to push these up front so machine/ stuff has priority
  dirs = %w[ /machine /machine/include ]
  gcc.cflags.unshift "#{dirs.map { |d| "-I#{src}#{d}" }.join(" ")} -I. -Imachine/test/cxxtest"

  gcc.cflags << "-Wall"
  gcc.cflags << "-Werror"
  gcc.cflags << "-Wno-unused-function"
  gcc.cflags << "-Wno-unused-parameter"
  gcc.cflags << "-Wwrite-strings"
  gcc.cflags << "-Wmissing-field-initializers"
  gcc.cflags << "-Wcovered-switch-default"
  gcc.cflags << "-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS"
  gcc.cflags << "-D_LARGEFILE_SOURCE"
  gcc.cflags << "-D_FILE_OFFSET_BITS=64"

  program = files.dup
  program << i.source_file("machine/main.cpp")

  exe = RUBY_PLATFORM =~ /mingw|mswin/ ? 'machine/vm.exe' : 'machine/vm'
  i.program exe, *program
end
