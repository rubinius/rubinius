# All the tasks to manage building the Rubinius core library--which is
# essentially the Ruby core library plus Rubinius-specific files. The core
# bootstraps a Ruby environment to the point that user code can be loaded and
# executed.
#
# The basic rule is that any generated file should be specified as a file
# task, not hidden inside some arbitrary task. Generated files are created by
# rule (e.g. the rule for compiling a .rb file into a .rbc file) or by a block
# attached to the file task for that particular file.
#
# The only tasks should be those names needed by the user to invoke specific
# parts of the build (including the top-level build task for generating the
# entire core library).

require "rakelib/digest_files"

# drake does not allow invoke to be called inside tasks
def core_clean
  rm_rf Dir["**/*.rbc",
           "**/.*.rbc",
           "#{BUILD_CONFIG[:sourcedir]}/rubinius-codedb-cache*",
           "#{BUILD_CONFIG[:sourcedir]}/rubinius-codetools-cache*",
           "#{BUILD_CONFIG[:sourcedir]}/rubinius-stdlib-cache*",
           "#{BUILD_CONFIG[:builddir]}#{BUILD_CONFIG[:codedbdir]}/platform.conf",
           "#{BUILD_CONFIG[:builddir]}#{BUILD_CONFIG[:codedbdir]}/source",
           "#{BUILD_CONFIG[:prefixdir]}/#{BUILD_CONFIG[:archdir]}/**/*",
           "#{BUILD_CONFIG[:prefixdir]}/#{BUILD_CONFIG[:archdir]}/**/*.*",
           "#{BUILD_CONFIG[:codetoolsdir]}/**/*",
           "#{BUILD_CONFIG[:codetoolsdir]}/**/*.*",
           "#{BUILD_CONFIG[:stdlibdir]}/**/*",
           "#{BUILD_CONFIG[:stdlibdir]}/**/*.*",
           "spec/capi/ext/*.{o,sig,#{$dlext}}",
          ],
    :verbose => $verbose
end

codedb_source = []

codedbdir = "#{BUILD_CONFIG[:builddir]}#{BUILD_CONFIG[:codedbdir]}"
platform_conf = "#{codedbdir}/platform.conf"

codedb_cache = "#{codedbdir}/cache"
codedb_cache_next = codedb_cache + ".next"

library_dir = "#{BUILD_CONFIG[:sourcedir]}/library"
codetools_dir = BUILD_CONFIG[:codetoolsdir]
stdlib_dir = BUILD_CONFIG[:stdlibdir]

core_load_order = "core/load_order.txt"

def codedb_source_task(db, origin, source)
  db << source

  file source => origin do |t|
    dir = File.dirname t.name
    mkdir_p dir, :verbose => $verbose unless File.directory? dir

    cp t.prerequisites.first, t.name, :verbose => $verbose
  end
end

# Add core files
IO.foreach core_load_order do |name|
  origin = "core/#{name.chomp}"

  codedb_source_task codedb_source, origin, "#{codedbdir}/source/#{origin}"
end

FileList["#{library_dir}/**/*.*"].each do |file|
  source = "#{codedbdir}/source/#{file[(library_dir.size+1)..-1]}"

  unless File.directory? file
    codedb_source_task codedb_source, file, source
  end
end

# Add codetools
FileList["#{codetools_dir}/{lib,ext}/**/*.rb"].each do |file|
  source = "#{codedbdir}/source/#{file[(codetools_dir.size+5)..-1]}"

  unless File.directory? file
    codedb_source_task codedb_source, file, source
  end
end

# Add stdlib
FileList["#{stdlib_dir}/{lib,ext}/**/*.rb"].each do |file|
  source = "#{codedbdir}/source/#{file[(stdlib_dir.size+5)..-1]}"

  unless File.directory? file
    codedb_source_task codedb_source, file, source
  end
end

config_files = FileList[
  "Rakefile",
  "build/config/config.rb",
  "rakelib/*.rb",
  "rakelib/*.rake"
]

ext_source = FileList[
  "#{stdlib_dir}/**/*.{c,h}pp",
  "#{stdlib_dir}/**/grammar.y",
  "#{stdlib_dir}/**/lex.c.*",
  "#{codetools_dir}/**/*.{c,h}pp",
  "#{codetools_dir}/**/grammar.y",
  "#{codetools_dir}/**/lex.c.*",
]

melbourne_ext = FileList["#{codetools_dir}/ext/**/melbourne/extconf.rb"].first
extconf_source = FileList["#{stdlib_dir}/ext/**/extconf.rb"]

extensions_dir = "#{BUILD_CONFIG[:builddir]}/#{BUILD_CONFIG[:archdir]}"
directory extensions_dir

signature_files = codedb_source + config_files + ext_source

def build_extension(file, melbourne=false)
  FileUtils.mkdir_p "#{BUILD_CONFIG[:builddir]}#{BUILD_CONFIG[:includedir]}/ruby/digest"

  Dir.chdir File.dirname(file) do
    if file =~ /openssl/ and openssl = ENV["OPENSSL_DIR"]
      options = "--with-cppflags=-I#{openssl}/include --with-ldflags=-L#{openssl}/lib"
    else
      options = nil
    end

    unless File.exist? "Makefile"
      begin
        ENV["RBX_PREFIX_PATH"] = BUILD_CONFIG[:builddir]

        extconf = melbourne ? "--main rubinius/code/melbourne/extconf.rb" : "extconf.rb"

        sh "#{BUILD_CONFIG[:build_exe]} -v --disable-gems #{extconf} #{options}", :verbose => $verbose
      ensure
        ENV.delete "RBX_PREFIX_PATH"
      end
    end

    sh "#{BUILD_CONFIG[:build_make]}", :verbose => $verbose
    sh "#{BUILD_CONFIG[:build_make]} install", :verbose => $verbose
  end
end

namespace :codedb do
  task :signature => signature_files do
    SIGNATURE_HASH = digest_files signature_files
  end

  task :extensions => [extensions_dir, melbourne_ext] + extconf_source do
    # It would be better to define file commands for every extension but we
    # don't know the extension shared library without some other source
    # parsing so we just iterate based on files.

    begin
      destdir = ENV["DESTDIR"]
      ENV.delete "DESTDIR"

      ENV["CXX"] = BUILD_CONFIG[:cxx]
      ENV["CXXFLAGS"] = BUILD_CONFIG[:system_cxxflags]
      ENV["CPPFLAGS"] = BUILD_CONFIG[:system_cppflags]
      ENV["INCFLAGS"] = BUILD_CONFIG[:system_incflags]
      ENV["LDSHAREDXX"] = BUILD_CONFIG[:ldsharedxx]
      ENV["LDFLAGS"] = BUILD_CONFIG[:system_ldflags]

      build_extension melbourne_ext, true

      Dir.chdir "#{codetools_dir}/lib" do
        FileList["./**/*.#{RbConfig::CONFIG["DLEXT"]}"].each do |lib|
          lib_dir = "#{extensions_dir}/#{File.dirname(lib)}"
          mkdir_p lib_dir

          cp lib, lib_dir, :verbose => $verbose
        end
      end
    ensure
      ENV["DESTDIR"] = destdir

      ENV.delete "CXX"
      ENV.delete "CXXFLAGS"
      ENV.delete "CPPFLAGS"
      ENV.delete "INCFLAGS"
      ENV.delete "LDSHAREDXX"
      ENV.delete "LDFLAGS"
    end

    # Rebuild the CodeDB cache to include platform-specific code
    begin
      ENV["RBX_PREFIX_PATH"] = BUILD_CONFIG[:builddir]

      Dir.chdir BUILD_CONFIG[:builddir] do
        sh "#{BUILD_CONFIG[:build_exe]} -v --disable-gems #{BUILD_CONFIG[:scriptdir]}/create_codedb_cache.rb #{codedb_cache_next}", :verbose => $verbose
      end

      mv codedb_cache_next, codedb_cache, :verbose => $verbose
    ensure
      ENV.delete "RBX_PREFIX_PATH"
    end

    extconf_source.each do |file|
      build_extension file
    end
  end
end

signature_header = "machine/signature.h"

file signature_header => "codedb:signature" do |t|
  File.open t.name, "wb" do |file|
    file.puts <<-EOC
#ifndef RBX_SIGNATURE_H
#define RBX_SIGNATURE_H

#define RBX_SIGNATURE          #{SIGNATURE_HASH.inspect}

#endif
    EOC
  end
end

desc "Build all core library files"
task :core => 'core:build'

namespace :core do
  desc "Build all core and library files"
  task :build => [platform_conf, signature_header] + codedb_source + ["codedb:extensions"]

  desc "Delete all core and library artifacts"
  task :clean do
    core_clean
  end
end
