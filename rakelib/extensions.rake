desc "Build extensions from lib/ext"
task :extensions => %w[
  vm/vm
  kernel:build

  extension:readline
  extension:digest
]

#  lib/etc.rb
#  lib/fcntl.rb
#  lib/openssl/digest.rb
#  lib/syslog.rb
#  lib/zlib.rb

#
# Ask the VM to build an extension from source.
#
def compile_extension(path, flags = "-d -p -I#{Dir.pwd}/vm/subtend")
  cflags = Object.const_get(:FLAGS).reject {|f| f =~ /-Wno-deprecated|-Weffc\+\+/ }

  cflags.each {|flag| flags << " -C,#{flag}" }

  command = "./bin/rbx compile #{flags} #{path}"

  sh command
end

namespace :extension do

  desc "Cleans all C extension libraries and build products."
  task :clean do
    Dir["lib/ext/**/*.{o,#{$dlext}}"].each do |f|
      rm_f f, :verbose => $verbose
    end
  end

  desc "Build the readline extension"
  task :readline => "lib/ext/readline/readline.#{$dlext}"

  file "lib/ext/readline/readline.#{$dlext}" => FileList[
       "lib/ext/readline/build.rb",
       "lib/ext/readline/readline.c",
  ] do
    compile_extension 'lib/ext/readline'
  end

  desc "Build the Digest extensions"
  task :digest => %w[extension:digest:md5 extension:digest:rmd160
                     extension:digest:sha1 extension:digest:sha2]


  namespace :digest do
    def digest_task name
      desc "Build Digest's #{name} extension."
      task name => "lib/ext/digest/#{name}/#{name}.#{$dlext}"
      file "lib/ext/digest/#{name}/#{name}.#{$dlext}" =>
        FileList["lib/ext/digest/#{name}/build.rb",
                 "lib/ext/digest/#{name}/{#{name},#{name}init}.c",
                 "lib/ext/digest/#{name}/#{name}.h",
                 "lib/ext/digest/defs.h"] do
        compile_extension "lib/ext/digest/#{name}"
      end
    end

    digest_task "md5"
    digest_task "rmd160"
    digest_task "sha1"
    digest_task "sha2"
  end


  # Undocumented, used by spec/subtend/subtend_helper to build the spec exts.
  namespace :specs do

    FileList["spec/subtend/ext/*.c"].each do |source|
      name = File.basename source, ".c"
      library = source.sub(/\.c/, ".#{$dlext}")

      task name => library

      file library => source do
        compile_extension source
      end
    end

  end

  # The ones below are not used currently.

  task :mongrel => "lib/ext/mongrel/http11.#{$dlext}"

  file "lib/ext/mongrel/http11.#{$dlext}" => FileList[
    'shotgun/lib/subtend/*',
    'lib/ext/mongrel/build.rb',
    'lib/ext/mongrel/*.c',
    'lib/ext/mongrel/*.h',
  ] do
    compile_ruby "lib/ext/mongrel"
  end
end

