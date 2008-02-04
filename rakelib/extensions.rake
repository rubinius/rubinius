desc "Build extensions from lib/ext"
task :extensions => %w[
  build:shotgun
  build:rbc

  extension:digest_rmd160
  extension:digest_md5
  extension:digest_sha1
  extension:digest_sha2

  extension:syck
  extension:zlib
  extension:readline
]

namespace :extension do
  task :digest_md5 => "lib/ext/digest/md5/md5.#{$dlext}"

  file "lib/ext/digest/md5/md5.#{$dlext}" => FileList[
    'lib/ext/digest/md5/build.rb',
    'lib/ext/digest/md5/*.c',
    'lib/ext/digest/md5/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile 'lib/ext/digest/md5'
  end

  task :digest_rmd160 => "lib/ext/digest/rmd160/rmd160.#{$dlext}"

  file "lib/ext/digest/rmd160/rmd160.#{$dlext}" => FileList[
    'lib/ext/digest/rmd160/build.rb',
    'lib/ext/digest/rmd160/*.c',
    'lib/ext/digest/rmd160/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile 'lib/ext/digest/rmd160'
  end

  task :digest_sha1 => "lib/ext/digest/sha1/sha1.#{$dlext}"

  file "lib/ext/digest/sha1/sha1.#{$dlext}" => FileList[
    'lib/ext/digest/sha1/build.rb',
    'lib/ext/digest/sha1/*.c',
    'lib/ext/digest/sha1/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile 'lib/ext/digest/sha1'
  end

  task :digest_sha2 => "lib/ext/digest/sha2/sha2.#{$dlext}"

  file "lib/ext/digest/sha2/sha2.#{$dlext}" => FileList[
    'lib/ext/digest/sha2/build.rb',
    'lib/ext/digest/sha2/*.c',
    'lib/ext/digest/sha2/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile 'lib/ext/digest/sha2'
  end

  task :syck => "lib/ext/syck/rbxext.#{$dlext}"

  file "lib/ext/syck/rbxext.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/syck/build.rb',
    'lib/ext/syck/*.c',
    'lib/ext/syck/*.h',
  ] do
    compile "lib/ext/syck"
  end
  
  task :mongrel => "lib/ext/mongrel/http11.#{$dlext}"

  file "lib/ext/mongrel/http11.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/mongrel/build.rb',
    'lib/ext/mongrel/*.c',
    'lib/ext/mongrel/*.h',
  ] do
    compile "lib/ext/mongrel"
  end

  task :zlib => %W[lib/ext/zlib/zlib.#{$dlext} lib/zlib.rb]

  file "lib/ext/zlib/zlib.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/zlib/build.rb',
    'lib/ext/zlib/*.c'
  ] do
    compile "lib/ext/zlib"
  end

  task :readline => %W[lib/ext/readline/readline.#{$dlext} lib/readline.rb]

  file "lib/ext/readline/readline.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/readline/build.rb',
    'lib/ext/readline/*.c'
  ] do
    compile "lib/ext/readline"
  end
end

