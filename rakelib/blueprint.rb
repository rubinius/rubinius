Daedalus.blueprint do |i|
  llvm = i.external_lib "vm/external_libs/llvm" do |l|
    conf = "vm/external_libs/llvm/Release/bin/llvm-config"
    flags = `#{conf} --cflags`.strip.split(/\s+/)
    flags.delete_if { |x| x.index("-O") == 0 || x.index("-I") == 0 }
    flags << "-Ivm/external_libs/llvm/include"
    l.cflags = flags
    l.ldflags = [`#{conf} --ldflags`.strip]
    l.objects = `#{conf} --libfiles`.strip.split(/\s+/)
  end

  ltm = i.external_lib "vm/external_libs/libtommath" do |l|
    l.cflags = ["-Ivm/external_libs/libtommath"]
    l.objects = [l.file("libtommath.a")]
    l.to_build do |x|
      x.command "make"
    end
  end

  onig = i.external_lib "vm/external_libs/onig" do |l|
    l.cflags = ["-Ivm/external_libs/onig"]
    l.objects = [l.file(".libs/libonig.a")]
    l.to_build do |x|
      x.command "./configure" unless File.exists?("Makefile")
      x.command "make"
    end
  end

  gdtoa = i.external_lib "vm/external_libs/libgdtoa" do |l|
    l.cflags = ["-Ivm/external_libs/libgdtoa"]
    l.objects = [l.file("libgdtoa.a")]
    l.to_build do |x|
      x.command "make"
    end
  end

  ffi = i.external_lib "vm/external_libs/libffi" do |l|
    l.cflags = ["-Ivm/external_libs/libffi/include"]
    l.objects = [l.file(".libs/libffi.a")]
    l.to_build do |x|
      x.command "./configure" unless File.exists?("Makefile")
      x.command "make"
    end
  end

  udis = i.external_lib "vm/external_libs/udis86" do |l|
    l.cflags = ["-Ivm/external_libs/udis86"]
    l.objects = [l.file("libudis86/.libs/libudis86.a")]
    l.to_build do |x|
      x.command "./configure" unless File.exists?("Makefile")
      x.command "make"
    end
  end

  subdirs = %w!builtin capi util instruments gc llvm!.map do |x|
    "vm/#{x}/*.{cpp,c}"
  end

  files = i.source_files "vm/*.{cpp,c}", *subdirs

  gcc = i.gcc!

  gcc.cflags << "-Ivm -Ivm/test/cxxtest -I. -I/usr/local/include -I/opt/local/include "
  gcc.cflags << "-pipe -Wall -fno-omit-frame-pointer"
  gcc.cflags << "-ggdb3 -Werror"
  gcc.cflags << "-DENABLE_LLVM -DRBX_PROFILER"

  if ENV['DEV']
    gcc.cflags << "-O0"
  else
    gcc.cflags << "-O2"
  end

  Rubinius::BUILD_CONFIG[:defines].each do |flag|
    gcc.cflags << "-D#{flag}"
  end

  if RUBY_PLATFORM =~ /darwin/i
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

  gcc.ldflags << "-lstdc++"
  gcc.ldflags << "-L/usr/local/lib -L/opt/local/lib -ldl"

  gcc.ldflags << '-Wl,--export-dynamic' if RUBY_PLATFORM =~ /linux|openbsd/i
  gcc.ldflags << '-rdynamic'            if RUBY_PLATFORM =~ /bsd/

  gcc.add_library udis
  gcc.add_library ffi
  gcc.add_library gdtoa
  gcc.add_library onig
  gcc.add_library ltm
  gcc.add_library llvm

  files << udis
  files << ffi
  files << gdtoa
  files << onig
  files << ltm
  files << llvm

  cli = files.dup
  cli << i.source_file("vm/drivers/cli.cpp")

  i.program "vm/vm", *cli

  test_files = files.dup
  test_files << i.source_file("vm/test/runner.cpp") { |f|
    tests = Dir["vm/test/**/test_*.hpp"]

    f.depends_on tests

    f.autogenerate do |x|
      x.command("vm/test/cxxtest/cxxtestgen.pl --error-printer --have-eh " +
        "--abort-on-fail -include=string.h -include=stdlib.h " +
        "-include=vm/test/test_setup.h -o vm/test/runner.cpp " +
        tests.join(' '))
    end
  }

  i.program "vm/test/runner", *test_files
end
