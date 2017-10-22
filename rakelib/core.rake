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
           "codedb",
           "spec/capi/ext/*.{o,sig,#{$dlext}}",
           "#{BUILD_CONFIG[:bootstrap_gems_dir]}/**/Makefile",
          ],
    :verbose => $verbose
end

codedb_files = FileList[
  "codedb/cache/contents",
  "codedb/cache/data",
  "codedb/cache/index",
  "codedb/cache/initialize",
  "codedb/cache/signature",
  "codedb/cache/platform.conf",
]

codedb_cache = []
codedb_contents = []
codedb_data = []
codedb_initialize = []
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
  "config.rb",
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

extensions_dir = "#{BUILD_CONFIG[:sourcedir]}/codedb/extensions"
directory extensions_dir

signature_files = codedb_source + config_files + ext_source

def build_extension(gems_dir, ext_dir, file)
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
    # TODO: Fix: Collapse the digest to a 64bit quantity
    hd = digest_files signature_files
    SIGNATURE_HASH = hd[0, 16].to_i(16) ^ hd[16,16].to_i(16) ^ hd[32,8].to_i(16)
  end

  task :extensions => [extensions_dir, melbourne_ext] + codedb_files + extconf_source do
    # It would be better to define file commands for every extension but we
    # don't know the extension shared library without some other source
    # parsing so we just iterate based on files.

    melbourne_ext.each do |file|
      build_extension bootstrap_gems_dir, extensions_dir, file

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
      build_extension bootstrap_gems_dir, extensions_dir, file
    end

    ffi_cache = []
    ffi_contents = []
    ffi_data = []

    FileList["#{bootstrap_gems_dir}/**/*.rb.ffi"]
            .map { |x| x.sub(/\.ffi$/, "") }
            .each do |file|
      puts "RBC #{file}" if $verbose

      m = %r[(#{bootstrap_gems_dir}/[^/]+/lib/)(.*\.rb)$].match(file)

      dir = m[1]
      content = m[2]

      Dir.chdir dir do
        code = CodeDBCompiler.compile(content, 1, [:default, :kernel])

        m_id = code.code_id

        ffi_cache << [m_id, code]
        ffi_contents << [content, m_id]
      end
    end

    while x = ffi_cache.shift
      id, cc = x

      cc.literals.each_with_index do |value, index|
        if value.kind_of? Rubinius::CompiledCode
          cc.literals[index] = i = value.code_id
          ffi_cache.unshift [i, value]
        end
      end

      marshaled = CodeDBCompiler.marshal cc

      ffi_data << [id, marshaled]
    end

    File.open "codedb/cache/data", "ab" do |f|
      f.seek 0, IO::SEEK_END

      ffi_data.map! do |m_id, data|
        offset = f.pos
        f.write data

        [m_id, offset, f.pos - offset]
      end
    end

    File.open "codedb/cache/index", "ab" do |f|
      ffi_data.each { |m_id, offset, length| f.puts "#{m_id} #{offset} #{length}" }
    end

    File.open "codedb/cache/contents", "ab" do |f|
      ffi_contents.each { |file, m_id| f.puts "#{file} . #{m_id} 0" }
    end
  end
end

signature_header = "machine/gen/signature.h"

file signature_header => "codedb:signature" do |t|
  File.open t.name, "wb" do |file|
    file.puts "#define RBX_SIGNATURE          #{SIGNATURE_HASH}ULL"
  end
end

file "codedb/cache/signature" => ["codedb:signature", "codedb/cache"] do |t|
  File.open t.name, "wb" do |file|
    puts "GEN #{t.name}"
    file.puts SIGNATURE_HASH
  end
end

namespace :compiler do
  task :load => :signature do
    Rubinius::Signature = SIGNATURE_HASH

    require "rubinius/bridge"
    require "rubinius/code/toolset"

    Rubinius::ToolSets.create :build do
      require "rubinius/code/melbourne"
      require "rubinius/code/processor"
      require "rubinius/code/compiler"
      require "rubinius/code/ast"
    end
  end

  task :signature => "codedb:signature"
end

class CodeDBCompiler
  def self.compile(file, line, transforms)
    compiler = Rubinius::ToolSets::Build::Compiler.new :file, :compiled_code

    parser = compiler.parser
    parser.root Rubinius::ToolSets::Build::AST::Script

    if transforms.kind_of? Array
      transforms.each { |t| parser.enable_category t }
    else
      parser.enable_category transforms
    end

    parser.input file, line

    generator = compiler.generator
    generator.processor Rubinius::ToolSets::Build::Generator

    compiler.run
  end

  def self.marshal(code)
    marshaler = Rubinius::ToolSets::Build::CompiledFile::Marshal.new
    marshaler.marshal code
  end
end

directory "codedb/cache"

file "codedb/cache/data" => ["codedb/cache", core_load_order] + codedb_source do |t|
  puts "CodeDB: writing data..."

  Dir.chdir "codedb/source" do
    codedb_source.each do |file|
      puts "RBC #{file}" if $verbose

      content = %r[codedb/source/(.*)$].match(file)[1]

      code = CodeDBCompiler.compile(content, 1, [:default, :kernel])
      id = code.code_id

      codedb_cache << [id, code]
      codedb_contents << [content, id]

      codedb_initialize << id if %r[^core/] =~ content
    end
  end

  while x = codedb_cache.shift
    id, cc = x

    cc.literals.each_with_index do |value, index|
      if value.kind_of? Rubinius::CompiledCode
        cc.literals[index] = i = value.code_id
        codedb_cache.unshift [i, value]
      end
    end

    marshaled = CodeDBCompiler.marshal cc

    codedb_data << [id, marshaled]
  end

  File.open t.name, "wb" do |f|
    codedb_data.map! do |m_id, data|
      offset = f.pos
      f.write data

      [m_id, offset, f.pos - offset]
    end
  end
end

file "codedb/cache/index" => "codedb/cache/data" do |t|
  puts "CodeDB: writing index..."

  unless codedb_data.empty?
    File.open t.name, "wb" do |f|
      codedb_data.each { |id, offset, length| f.puts "#{id} #{offset} #{length}" }
    end
  end
end

file "codedb/cache/contents" => "codedb/cache/data" do |t|
  puts "CodeDB: writing contents..."

  unless codedb_contents.empty?
    File.open t.name, "wb" do |f|
      codedb_contents.each { |file, id| f.puts "#{file} . #{id} 0" }
    end
  end
end

file "codedb/cache/initialize" => "codedb/cache/data" do |t|
  puts "CodeDB: writing initialize..."

  unless codedb_initialize.empty?
    File.open t.name, "wb" do |f|
      codedb_initialize.each { |id| f.puts id }
    end
  end
end

file "codedb/cache/signature" => "codedb:signature" do |t|
  puts "CodeDB: writing signature..."

  File.open t.name, "wb" do |f|
    f.puts SIGNATURE_HASH
  end
end

desc "Build all core library files (alias for core:build)"
task :core => 'core:build'

namespace :core do
  desc "Build all core and library files"
  task :build => ["compiler:load"] + codedb_files + codedb_library + ["codedb:extensions"]

  desc "Delete all core and library artifacts"
  task :clean do
    core_clean
  end
end
