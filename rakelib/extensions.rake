require 'lib/ffi/generator_task'

desc "Build extensions from lib/ext"
task :extensions => %w[
  vm/vm
  kernel:build

  lib/etc.rb
  lib/fcntl.rb
  lib/openssl/digest.rb
  lib/syslog.rb
  lib/zlib.rb
]
#  extension:digest_rmd160
#  extension:digest_md5
#  extension:digest_sha1
#  extension:digest_sha2
#
#  extension:readline

namespace :extension do
  desc "Cleans all compiled extension files (lib/ext)"
  task :clean do
    Dir["lib/ext/**/*#{$dlext}"].each do |f|
      rm_f f, :verbose => $verbose
    end
  end

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

  task :readline => %W[lib/ext/readline/readline.#{$dlext} lib/readline.rb]

  file "lib/ext/readline/readline.#{$dlext}" => FileList[
    'shotgun/lib/subtend/*',
    'lib/ext/readline/build.rb',
    'lib/ext/readline/*.c'
  ] do
    compile_ruby "lib/ext/readline"
  end
end

