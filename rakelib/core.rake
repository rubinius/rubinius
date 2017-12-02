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
           "codedb/platform.conf",
           "codedb/source",
           "spec/capi/ext/*.{o,sig,#{$dlext}}",
           "#{BUILD_CONFIG[:prefixdir]}/#{BUILD_CONFIG[:archdir]}/**/*.*",
           "#{BUILD_CONFIG[:bootstrap_gems_dir]}/**/Makefile",
          ],
    :verbose => $verbose
end

codedb_source = []
codedb_library = []

library_dir = "#{BUILD_CONFIG[:sourcedir]}/library"
bootstrap_gems_dir = BUILD_CONFIG[:bootstrap_gems_dir]

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

  codedb_source_task codedb_source, origin, "codedb/source/#{origin}"
end

# Add library files
FileList["#{library_dir}/**/*.rb"].each do |file|
  source = "codedb/source/#{file[(library_dir.size+1)..-1]}"

  codedb_source_task codedb_source, file, source
end

FileList["#{library_dir}/**/*.*"].exclude("#{library_dir}/**/*.rb").each do |file|
  source = "codedb/source/#{file[(library_dir.size+1)..-1]}"

  unless File.directory? file
    codedb_source_task codedb_library, file, source
  end
end

# Add pre-installed gems
FileList["#{bootstrap_gems_dir}/*/{lib,ext}/**/*.rb"].each do |file|
  m = %r[#{bootstrap_gems_dir}/[^/]+/(lib|ext)/(.*\.rb)$].match file

  if m and m[2]
    source = "codedb/source/#{m[2]}"
    codedb_source_task codedb_source, file, source
  else
    raise RuntimeError, "pre-installed gem file not matched: #{file}"
  end
end

config_files = FileList[
  "Rakefile",
  "build/config/config.rb",
  "rakelib/*.rb",
  "rakelib/*.rake"
]

ext_source = FileList[
  "#{bootstrap_gems_dir}/**/*.{c,h}pp",
  "#{bootstrap_gems_dir}/**/grammar.y",
  "#{bootstrap_gems_dir}/**/lex.c.*"
]

melbourne_ext = FileList["#{bootstrap_gems_dir}/rubinius-melbourne*/ext/**/extconf.rb"]
extconf_source = FileList["#{bootstrap_gems_dir}/**/{lib,ext}/**/extconf.rb"
                         ].exclude(melbourne_ext)

extensions_dir = "#{BUILD_CONFIG[:prefixdir]}/#{BUILD_CONFIG[:archdir]}"
directory extensions_dir

signature_files = codedb_source + config_files + ext_source

def build_extension(gems_dir, file)
  extconf = %r[#{gems_dir}/[^/]+/(lib|ext)/(.*\.rb)$].match(file)[2]

  Dir.chdir File.dirname(file) do
    if file =~ /openssl/ and openssl = ENV["OPENSSL_DIR"]
      options = "--with-cppflags=-I#{openssl}/include --with-ldflags=-L#{openssl}/lib"
    else
      options = nil
    end

    unless File.exist? "Makefile"
      sh "#{BUILD_CONFIG[:build_exe]} -v --disable-gems --main #{extconf} #{options}", :verbose => $verbose
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

    melbourne_ext.each do |file|
      build_extension bootstrap_gems_dir, file

      ext_dir = %r[(#{bootstrap_gems_dir}/[^/]+)/.*$].match(file)[1]

      Dir.chdir "#{ext_dir}/lib" do
        FileList["./**/*.#{RbConfig::CONFIG["DLEXT"]}"].each do |lib|
          lib_dir = "#{extensions_dir}/#{File.dirname(lib)}"
          mkdir_p lib_dir

          cp lib, lib_dir, :verbose => $verbose
        end
      end
    end

    extconf_source.each do |file|
      build_extension bootstrap_gems_dir, file
    end
  end
end

signature_header = "machine/gen/signature.h"

file signature_header => "codedb:signature" do |t|
  File.open t.name, "wb" do |file|
    file.puts "#define RBX_SIGNATURE          #{SIGNATURE_HASH.inspect}"
  end
end

desc "Build all core library files"
task :core => 'core:build'

namespace :core do
  desc "Build all core and library files"
  task :build => ["codedb/platform.conf", signature_header] + codedb_source + codedb_library + ["codedb:extensions"]

  desc "Delete all core and library artifacts"
  task :clean do
    core_clean
  end
end
