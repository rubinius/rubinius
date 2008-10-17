require 'lib/ffi/generator_task'

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
def compile_extension(path, flags = nil, extra_flags = nil)
  flags ||= "-d -p -C,-ggdb3 -C,-O0 -I#{Dir.pwd}/vm/subtend"
  command = "./bin/rbx compile #{flags} #{extra_flags} #{path}"

  puts "Executing `#{command}`" if $verbose

  sh command
end

namespace :extension do

  desc "Cleans all C extension libraries and build products."
  task :clean do
    Dir["lib/ext/**/*.{o,#{$dlext}}"].each do |f|
      rm_f f, :verbose => $verbose
    end

#    FileList["lib/ext/**/*.o"].each {|f| rm f, :verbose => $verbose }
  end

  desc "Build the readline extension"
  task :readline => "lib/ext/readline/readline.#{$dlext}"

  file "lib/ext/readline/readline.#{$dlext}" => FileList[
       "lib/ext/readline/build.rb",
       "lib/ext/readline/readline.c",
       "vm/vm"
  ] do
    FileList["lib/ext/readline/readline.{o,#{$dlext}}"].each do |f|
      rm f, :verbose => $verbose
    end

    compile_extension 'lib/ext/readline'
  end

  desc "Build the Digest extensions"
  task :digest => %w[extension:digest:md5 extension:digest:rmd160
                     extension:digest:sha1 extension:digest:sha2]


  namespace :digest do

    desc "Build Digest's MD5 extension."
    task :md5 => "lib/ext/digest/md5/md5.#{$dlext}"

    # TODO: propogate the changes here down to the other "*"s below:
    task "lib/ext/digest/md5/md5.#{$dlext}" =>
      FileList['lib/ext/digest/md5/build.rb',
               'lib/ext/digest/md5/{md5,md5init}.c',
               'lib/ext/digest/md5/md5.h',
               'lib/ext/digest/defs.h',
               "vm/vm"] do
      FileList["lib/ext/digest/md5/{md5,md5init}.{o,#{$dlext}}"].each do |f|
        rm f, :verbose => $verbose
      end

      compile_extension 'lib/ext/digest/md5', nil
    end

    desc "Build Digest's RMD160 extension."
    task :rmd160 => "lib/ext/digest/rmd160/rmd160.#{$dlext}"

    file "lib/ext/digest/rmd160/rmd160.#{$dlext}" => FileList[
      'lib/ext/digest/rmd160/build.rb',
      'lib/ext/digest/rmd160/*.c',
      'lib/ext/digest/rmd160/*.h',
      'lib/ext/digest/defs.h',
      "vm/vm"
    ] do
      FileList["lib/ext/digest/rmd160/*.{o,#{$dlext}}"].each do |f|
        rm f, :verbose => $verbose
      end

      compile_extension 'lib/ext/digest/rmd160'
    end

    desc "Build Digest's SHA1 extension."
    task :sha1 => "lib/ext/digest/sha1/sha1.#{$dlext}"

    file "lib/ext/digest/sha1/sha1.#{$dlext}" => FileList[
      'lib/ext/digest/sha1/build.rb',
      'lib/ext/digest/sha1/*.c',
      'lib/ext/digest/sha1/*.h',
      'lib/ext/digest/defs.h',
      "vm/vm"
    ] do
      FileList["lib/ext/digest/sha1/*.{o,#{$dlext}}"].each do |f|
        rm f, :verbose => $verbose
      end

      compile_extension 'lib/ext/digest/sha1'
    end

    desc "Build Digest's SHA2 extension."
    task :sha2 => "lib/ext/digest/sha2/sha2.#{$dlext}"

    file "lib/ext/digest/sha2/sha2.#{$dlext}" => FileList[
      'lib/ext/digest/sha2/build.rb',
      'lib/ext/digest/sha2/*.c',
      'lib/ext/digest/sha2/*.h',
      'lib/ext/digest/defs.h',
      "vm/vm"
    ] do
      FileList["lib/ext/digest/sha2/*.{o,#{$dlext}}"].each do |f|
        rm f, :verbose => $verbose
      end

      compile_extension 'lib/ext/digest/sha2'
    end

  end   # :digest

  # The ones below are not used currently.

  FFI::Generator::Task.new %w[
    lib/etc.rb
    lib/fcntl.rb
    lib/openssl/digest.rb
    lib/syslog.rb
    lib/zlib.rb
  ]

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

