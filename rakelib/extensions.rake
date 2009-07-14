desc "Build extensions from lib/ext"
task :extensions => %w[
  extension:melbourne
  extension:readline
  extension:digest
  extension:bigdecimal
  extension:syck
]

#  lib/etc.rb
#  lib/fcntl.rb
#  lib/openssl/digest.rb
#  lib/syslog.rb
#  lib/zlib.rb

#
# Ask the VM to build an extension from source.
#
def compile_extension(path, flags = "-p -I#{Dir.pwd}/vm/capi")
  cflags = BASIC_FLAGS.reject {|f| f =~ /-Wno-deprecated|-Weffc\+\+/ }

  cflags.each {|flag| flags << " -C,#{flag}" }

  # @todo: this is a hack because the change from FLAGS above to
  # BASIC_FLAGS means the debug flags don't come through. this
  # all needs to be sorted out with a build system rewrite.
  flags << " -C,-ggdb3"

  verbose = $verbose ? "-d" : ""

  command = "./bin/rbx compile #{verbose} #{flags} #{path}"

  if $verbose
    sh command
  else
    puts "Building extension #{path}"
    sh command, :verbose => false
  end
end

namespace :extension do

  desc "Cleans all C extension libraries and build products."
  task :clean do
    Dir["lib/ext/**/*.{o,#{$dlext}}"].each do |f|
      rm_f f, :verbose => $verbose
    end
  end

  desc "Build the bigdecimal extension"
  task :bigdecimal => %W[kernel:build lib/ext/bigdecimal/bigdecimal.#{$dlext}]
  file "lib/ext/bigdecimal/bigdecimal.#{$dlext}" => FileList[
    "lib/ext/bigdecimal/build.rb",
    "lib/ext/bigdecimal/bigdecimal.c",
    "lib/ext/bigdecimal/bigdecimal.h",
    "vm/capi/ruby.h"
  ] do
    compile_extension 'lib/ext/bigdecimal'
  end

  desc "Build the readline extension"
  task :readline => %W[kernel:build lib/ext/readline/readline.#{$dlext}]

  file "lib/ext/readline/readline.#{$dlext}" => FileList[
       "lib/ext/readline/build.rb",
       "lib/ext/readline/readline.c",
       "vm/capi/ruby.h"
  ] do
    compile_extension 'lib/ext/readline'
  end

  desc "Build the Digest extensions"
  task :digest => %w[extension:digest:digest
                     extension:digest:md5
                     extension:digest:rmd160
                     extension:digest:sha1
                     extension:digest:sha2
                     extension:digest:bubblebabble]

  namespace :digest do
    def digest_task name
      desc "Build Digest's #{name} extension."
      task name => %W[kernel:build lib/ext/digest/#{name}/#{name}.#{$dlext}]
      file "lib/ext/digest/#{name}/#{name}.#{$dlext}" =>
        FileList["lib/ext/digest/#{name}/build.rb",
                 "lib/ext/digest/#{name}/{#{name},#{name}init}.c",
                 "lib/ext/digest/#{name}/#{name}.h",
                 "lib/ext/digest/defs.h",
                 "vm/capi/ruby.h"
      ] do
        compile_extension "lib/ext/digest/#{name}"
      end
    end

    desc "Build Digest extension."
    task :digest => %W[kernel:build lib/ext/digest/digest.#{$dlext}]
    file "lib/ext/digest/digest.#{$dlext}" =>
      FileList["lib/ext/digest/build.rb",
               "lib/ext/digest/digest.c",
               "lib/ext/digest/digest.h",
               "lib/ext/digest/defs.h",
               "vm/capi/ruby.h"
    ] do
      compile_extension "lib/ext/digest"
    end

    digest_task "md5"
    digest_task "rmd160"
    digest_task "sha1"
    digest_task "sha2"
    digest_task "bubblebabble"
  end

  desc "Build the syck extension"
  task :syck => %W[kernel:build lib/ext/syck/syck.#{$dlext}]
  file "lib/ext/syck/syck.#{$dlext}" => FileList[
    "lib/ext/syck/build.rb",
    "lib/ext/syck/bytecode.c",
    "lib/ext/syck/emitter.c",
    "lib/ext/syck/gram.c",
    "lib/ext/syck/gram.h",
    "lib/ext/syck/handler.c",
    "lib/ext/syck/implicit.c",
    "lib/ext/syck/node.c",
    "lib/ext/syck/rubyext.c",
    "lib/ext/syck/syck.h",
    "lib/ext/syck/syck.c",
    "lib/ext/syck/token.c",
    "lib/ext/syck/yaml2byte.c",
    "lib/ext/syck/yamlbyte.h",
    "lib/ext/syck/st.h",
    "lib/ext/syck/st.c",
    "vm/capi/ruby.h"
  ] do
    compile_extension 'lib/ext/syck'
  end

  desc "Build the melbourne extension"
  task :melbourne => %W[kernel:build lib/ext/melbourne/melbourne.#{$dlext}]
  file "lib/ext/melbourne/melbourne.#{$dlext}" => FileList[
    "lib/ext/melbourne/build.rb",
    "lib/ext/melbourne/grammar.y",
    "lib/ext/melbourne/grammar.hpp",
    "lib/ext/melbourne/grammar.cpp",
    "lib/ext/melbourne/internal.hpp",
    "lib/ext/melbourne/melbourne.cpp",
    "lib/ext/melbourne/node.hpp",
    "lib/ext/melbourne/visitor.hpp",
    "lib/ext/melbourne/visitor.cpp",
    "lib/ext/melbourne/local_state.hpp",
    "lib/ext/melbourne/node_types.hpp",
    "lib/ext/melbourne/node_types.cpp",
    "lib/ext/melbourne/var_table.hpp",
    "lib/ext/melbourne/var_table.cpp",
  ] do
    compile_extension "lib/ext/melbourne"
  end
end
