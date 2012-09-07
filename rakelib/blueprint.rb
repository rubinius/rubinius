Daedalus.blueprint do |i|
  gcc = i.gcc!

  gcc.cflags << "-Ivm -Ivm/test/cxxtest -I. "

  # -fno-omit-frame-pointer is needed to get a backtrace on FreeBSD.
  # It is enabled by default on OS X, on the other hand, not on Linux.
  # To use same build flags across platforms, it is added explicitly.
  gcc.cflags << "-pipe -Wall -fno-omit-frame-pointer"

  # Due to a Clang bug (http://llvm.org/bugs/show_bug.cgi?id=9825),
  # -mno-omit-leaf-frame-pointer is needed for Clang on Linux.
  # On other combinations of platform and compiler, this flag is implicitly
  # assumed from -fno-omit-frame-pointer. To use same build flags across
  # platforms, -mno-omit-leaf-frame-pointer is added explicitly.
  gcc.cflags << "-mno-omit-leaf-frame-pointer" if Rubinius::BUILD_CONFIG[:cc] == "clang"

  gcc.cflags << "-Wno-unused-function"
  gcc.cflags << "-g -Werror"
  gcc.cflags << "-DRBX_PROFILER"
  gcc.cflags << "-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS"
  gcc.cflags << "-D_LARGEFILE_SOURCE"
  gcc.cflags << "-D_FILE_OFFSET_BITS=64"

  gcc.cflags << Rubinius::BUILD_CONFIG[:user_cflags]

  if ENV['DEV']
    gcc.cflags << "-O0"
    gcc.mtime_only = true
  else
    gcc.cflags << "-O2"
  end

  if ENV['POKE']
    gcc.mtime_only = true
  end

  # This is necessary for the gcc sync prims to fully work
  if Rubinius::BUILD_CONFIG[:x86_32]
    gcc.cflags << "-march=i686"
  end

  Rubinius::BUILD_CONFIG[:defines].each do |flag|
    gcc.cflags << "-D#{flag}"
  end

  if RUBY_PLATFORM =~ /darwin/i
    gcc.cflags << "-D_DARWIN_USE_64_BIT_INODE"

    if `sw_vers` =~ /10\.4/
      gcc.cflags << "-DHAVE_STRLCAT -DHAVE_STRLCPY"
    end

    # This flag makes the executable non-relocatable (and
    # slightly faster), but 4.3 does not support it.
    # TODO: Look for workarounds.
    unless `gcc -v 2>&1` =~ /gcc version 4\.3/i
      gcc.cflags << "-mdynamic-no-pic"
    end
  end

  gcc.ldflags << "-lstdc++" << "-lm"

  Rubinius::BUILD_CONFIG[:lib_dirs].each do |path|
    gcc.ldflags << "-L#{path}" if File.exists? path
  end

  make = "make"

  # TODO: Fix with Platform object
  case RUBY_PLATFORM
  when /linux/i
    gcc.ldflags << '-Wl,--export-dynamic' << "-lrt" << "-lcrypt" << "-ldl" << "-lpthread"
  when /freebsd/i
    gcc.ldflags << '-lcrypt' << '-pthread' << '-rdynamic'
    make = "gmake"
  when /openbsd|netbsd/i
    gcc.ldflags << '-lcrypto' << '-pthread' << '-lssl' << "-rdynamic" << "-Wl,--export-dynamic"
    make = "gmake"
  when /haiku/i
    gcc.ldflags << "-ldl" << "-lnetwork"
  when /bsd/i
    gcc.ldflags << "-ldl" << "-lcrypt" << "-rdynamic"
    make = "gmake"
  when /mingw|win32/i
    gcc.ldflags << "-lws2_32"
  when /solaris/
    gcc.cflags << "-fPIC -Wno-strict-aliasing"
    gcc.ldflags << "-lsocket" << "-lnsl" << "-fPIC"
    make = "gmake"
  else
    gcc.ldflags << "-ldl" << "-lpthread"
  end

  if RUBY_PLATFORM =~ /bsd/ and
      Rubinius::BUILD_CONFIG[:defines].include?('HAS_EXECINFO')
    gcc.ldflags << "-lexecinfo"
  end

  gcc.ldflags << Rubinius::BUILD_CONFIG[:user_ldflags]

  # Files
  subdirs = %w[ builtin capi util instruments gc llvm missing ].map do |x|
    "vm/#{x}/*.{cpp,c}"
  end

  files = i.source_files "vm/*.{cpp,c}", *subdirs

  perl = Rubinius::BUILD_CONFIG[:build_perl] || "perl"

  # Libraries
  ltm = i.external_lib "vendor/libtommath" do |l|
    l.cflags = ["-Ivendor/libtommath"]
    l.objects = [l.file("libtommath.a")]
    l.to_build do |x|
      x.command make
    end
  end

  oniguruma = i.library_group "vendor/oniguruma" do |g|
    g.depends_on "config.h", "configure"

    gcc.cflags << "-Ivendor/oniguruma"
    g.cflags = ["-DHAVE_CONFIG_H", "-I.", "-I../../vm/capi/19/include"]

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

  gdtoa = i.external_lib "vendor/libgdtoa" do |l|
    l.cflags = ["-Ivendor/libgdtoa"]
    l.objects = [l.file("libgdtoa.a")]
    l.to_build do |x|
      x.command make
    end
  end

  ffi = i.external_lib "vendor/libffi" do |l|
    l.cflags = ["-Ivendor/libffi/include"]
    l.objects = [l.file(".libs/libffi.a")]
    l.to_build do |x|
      x.command "sh -c './configure --disable-builddir'" unless File.exists?("Makefile")
      x.command make
    end
  end

  udis = i.external_lib "vendor/udis86" do |l|
    l.cflags = ["-Ivendor/udis86"]
    l.objects = [l.file("libudis86/.libs/libudis86.a")]
    l.to_build do |x|
      unless File.exists?("Makefile") and File.exists?("libudis86/Makefile")
        x.command "sh -c ./configure"
      end
      x.command make
    end
  end

  if Rubinius::BUILD_CONFIG[:vendor_zlib]
    zlib = i.external_lib "vendor/zlib" do |l|
      l.cflags = ["-Ivendor/zlib"]
      l.objects = []
      l.to_build do |x|
        unless File.exists?("Makefile") and File.exists?("zconf.h")
          x.command "sh -c ./configure"
        end

        if Rubinius::BUILD_CONFIG[:windows]
          x.command "make -f win32/Makefile.gcc"
        else
          x.command make
        end
      end
    end
  end

  case Rubinius::BUILD_CONFIG[:llvm]
  when :prebuilt, :svn
    llvm = i.external_lib "vendor/llvm" do |l|
      l.cflags = ["-Ivendor/llvm/include"]
      l.objects = []
    end

    gcc.add_library llvm
  end

  case Rubinius::BUILD_CONFIG[:llvm]
  when :config, :prebuilt, :svn
    conf = Rubinius::BUILD_CONFIG[:llvm_configure]
    flags = `#{conf} --cflags`.strip.split(/\s+/)
    flags.delete_if { |x| x.index("-O") == 0 }
    flags.delete_if { |x| x =~ /-D__STDC/ }
    flags.delete_if { |x| x == "-DNDEBUG" }
    flags.delete_if { |x| x == "-fomit-frame-pointer" }
    flags.delete_if { |x| x == "-pedantic" }
    flags.delete_if { |x| x == "-W" }
    flags.delete_if { |x| x == "-Wextra" }

    flags << "-DENABLE_LLVM"

    ldflags = `#{conf} --ldflags`.strip.split(/\s+/)
    objects = `#{conf} --libfiles`.strip.split(/\s+/)

    if Rubinius::BUILD_CONFIG[:windows]
      ldflags = ldflags.sub(%r[-L/([a-zA-Z])/], '-L\1:/')

      objects.select do |f|
        f.sub!(%r[^/([a-zA-Z])/], '\1:/')
        File.file? f
      end
    end

    gcc.cflags.concat flags
    gcc.ldflags.concat ldflags
    gcc.ldflags.concat objects
  when :no
    # nothing, not using LLVM
  else
    STDERR.puts "Unsupported LLVM configuration: #{Rubinius::BUILD_CONFIG[:llvm]}"
    raise "get out"
  end

  gcc.add_library zlib if Rubinius::BUILD_CONFIG[:vendor_zlib]
  gcc.add_library udis
  gcc.add_library ffi
  gcc.add_library gdtoa
  gcc.add_library ltm

  Rubinius::BUILD_CONFIG[:include_dirs].each do |path|
    gcc.cflags << "-I#{path} " if File.exists? path
  end

  if Rubinius::BUILD_CONFIG[:windows]
    winp = i.external_lib "vendor/winpthreads" do |l|
      l.cflags = ["-Ivendor/winpthreads/include"]
      l.objects = [l.file("libpthread.a")]
      l.to_build do |x|
        x.command "sh -c ./configure" unless File.exists?("Makefile")
        x.command make
      end
    end

    gcc.add_library winp

    files << winp
  end

  files << zlib if Rubinius::BUILD_CONFIG[:vendor_zlib]
  files << udis
  files << ffi
  files << gdtoa
  files << oniguruma
  files << ltm

  cli = files.dup
  cli << i.source_file("vm/drivers/cli.cpp")

  exe = RUBY_PLATFORM =~ /mingw|mswin/ ? 'vm/vm.exe' : 'vm/vm'
  i.program exe, *cli

  test_files = files.dup
  test_files << i.source_file("vm/test/runner.cpp") { |f|
    tests = Dir["vm/test/**/test_*.hpp"].sort

    f.depends_on tests

    f.autogenerate do |x|
      x.command("#{perl} vm/test/cxxtest/cxxtestgen.pl --error-printer --have-eh " +
        "--abort-on-fail -include=string.h -include=stdlib.h " +
        "-include=vm/test/test_setup.h -o vm/test/runner.cpp " +
        tests.join(' '))
    end
  }

  i.program "vm/test/runner", *test_files
end
