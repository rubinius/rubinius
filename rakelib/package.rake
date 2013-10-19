require 'rakelib/package'
require 'date'

namespace :package do
  desc "Package up the LLVM build into a tar.gz"
  task :llvm do
    host_triple = Rubinius::BUILD_CONFIG[:host]
    llvm_version = Rubinius::BUILD_CONFIG[:llvm_version]
    gcc_major_version = Rubinius::BUILD_CONFIG[:gcc_major]
    if host_triple == "i686-pc-linux-gnu" || host_triple == "x86_64-unknown-linux-gnu"
      prebuilt_archive = "llvm-#{llvm_version}-#{host_triple}-#{gcc_major_version}.tar.bz2"
    else
      prebuilt_archive = "llvm-#{llvm_version}-#{host_triple}.tar.bz2"
    end

    sh "tar -c -C vendor/llvm --exclude .svn --exclude \"*.dylib\" --exclude \"*.so\" -f - Release/lib Release/bin/llvm-config include | bzip2 -9 > #{prebuilt_archive}"

    write_md5_digest_file prebuilt_archive
    write_sha1_digest_file prebuilt_archive
  end

  desc "Create a release tarball from the source"
  task :tar do
    archive = "rubinius-#{BUILD_CONFIG[:version]}.tar.bz2"
    files = "{ git ls-files; ls config.rb; ls vendor/cache/*.gem; }"
    prefix = "-s '|^|rubinius-#{BUILD_CONFIG[:version]}/|'"
    sh "#{files} | sort | uniq | tar -c #{prefix} -T - -f - | bzip2 > #{archive}"

    write_md5_digest_file archive
    write_sha1_digest_file archive
  end

  task :binary_builder do
    pkg = RubiniusPackager.new(
      :prefix       => ENV["RBX_BINARY_PREFIX"],
      :root         => ENV["RBX_BINARY_ROOT"],
      :bin          => ENV["RBX_BINARY_BIN"],
      :config       => ENV["RBX_BINARY_CONFIG"],
      :archive      => ENV["RBX_BINARY_ARCHIVE"],
      :package      => ENV["RBX_BINARY_PACKAGE"]
    )

    pkg.build
  end

  desc "Build a general Unix/Linux binary package"
  task :binary do
    sh "rake package:binary_builder"
  end

  desc "Build a general Linux Heroku binary package"
  task :heroku do
    heroku_package = "ruby-#{BUILD_CONFIG[:ruby_version]}-rbx-#{BUILD_CONFIG[:version]}"

    sh "rake package:binary_builder RBX_BINARY_PACKAGE=#{heroku_package} " \
       "RBX_BINARY_PREFIX=/app/vendor/#{heroku_package}"
  end
end
