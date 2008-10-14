require 'lib/ffi/generator_task'

desc "Build extensions from lib/ext"
task :extensions => %w[
  vm/vm
  kernel:build

  extension:readline
]

#  lib/etc.rb
#  lib/fcntl.rb
#  lib/openssl/digest.rb
#  lib/syslog.rb
#  lib/zlib.rb

#  extension:digest_rmd160
#  extension:digest_md5
#  extension:digest_sha1
#  extension:digest_sha2

#
# Ask the VM to build an extension from source.
#
def compile_extension(path, flags = "-d -p -C,-ggdb3 -C,-O0 -I#{Dir.pwd}/vm/subtend")
  command = "./bin/rbx compile #{flags} #{path}"

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
       "lib/ext/readline/readline.c",
       "lib/ext/readline/build.rb",
       "vm/vm"
  ] do
    FileList["lib/ext/readline/readline.{o,#{$dlext}}"].each do |f|
      rm f, :verbose => $verbose
    end

    compile_extension 'lib/ext/readline'
  end

  # The ones below are not used currently.

  FFI::Generator::Task.new %w[
    lib/etc.rb
    lib/fcntl.rb
    lib/openssl/digest.rb
    lib/syslog.rb
    lib/zlib.rb
  ]

  task :digest_md5 => "lib/ext/digest/md5/md5.#{$dlext}"

  file "lib/ext/digest/md5/md5.#{$dlext}" => FileList[
    'shotgun/lib/subtend/*',
    'lib/ext/digest/md5/build.rb',
    'lib/ext/digest/md5/*.c',
    'lib/ext/digest/md5/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile_ruby 'lib/ext/digest/md5'
  end

  task :digest_rmd160 => "lib/ext/digest/rmd160/rmd160.#{$dlext}"

  file "lib/ext/digest/rmd160/rmd160.#{$dlext}" => FileList[
    'shotgun/lib/subtend/*',
    'lib/ext/digest/rmd160/build.rb',
    'lib/ext/digest/rmd160/*.c',
    'lib/ext/digest/rmd160/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile_ruby 'lib/ext/digest/rmd160'
  end

  task :digest_sha1 => "lib/ext/digest/sha1/sha1.#{$dlext}"

  file "lib/ext/digest/sha1/sha1.#{$dlext}" => FileList[
    'shotgun/lib/subtend/*',
    'lib/ext/digest/sha1/build.rb',
    'lib/ext/digest/sha1/*.c',
    'lib/ext/digest/sha1/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile_ruby 'lib/ext/digest/sha1'
  end

  task :digest_sha2 => "lib/ext/digest/sha2/sha2.#{$dlext}"

  file "lib/ext/digest/sha2/sha2.#{$dlext}" => FileList[
    'shotgun/lib/subtend/*',
    'lib/ext/digest/sha2/build.rb',
    'lib/ext/digest/sha2/*.c',
    'lib/ext/digest/sha2/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile_ruby 'lib/ext/digest/sha2'
  end

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

