require 'rakelib/release'
require 'rakelib/package'
require 'date'


namespace :package do

  desc "Create a release tarball from the source"
  task :tar do
    archive = "rubinius-#{rbx_version}.tar.bz2"
    sh "rm -rf #{archive}"

    files = "{ git ls-files; ls .revision; ls rubinius-codedb-cache.bz2*; ls build/libraries/cache/*.gem; }"
    prefix = "-s '|^|rubinius-#{rbx_version}/|'"
    sh "#{files} | sort | uniq | tar -c #{prefix} -T - -f - | bzip2 > #{archive}"

    write_sha512_digest_file archive
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
    heroku_package = "ruby-#{BUILD_CONFIG[:ruby_version]}-rbx-#{rbx_version}"

    sh "rake package:binary_builder RBX_BINARY_PACKAGE=#{heroku_package} " \
       "RBX_BINARY_PREFIX=/app/vendor/#{heroku_package}"
  end

  desc "Build a binary package for Homebrew"
  task :homebrew do
    ENV["LDFLAGS"] = "-Wl,-macosx_version_min,10.8"
    homebrew_package = "rubinius-#{rbx_version}"
    prefix = "/#{homebrew_package}"
    lib = "lib/rubinius/#{rbx_version}"
    config = "'--with-include-dir=#{ENV["OPENSSL_DIR"]}/include " \
             "--with-lib-dir=#{ENV["OPENSSL_DIR"]}/lib " \
             "--bindir=#{prefix}/bin --appdir=#{prefix}/#{lib} " \
             "--includedir=#{prefix}/include/rubinius " \
             "--gemsdir=#{prefix}/#{lib}/gems " \
             "--mandir=#{prefix}/man'"

    sh "rake package:binary_builder RBX_BINARY_PACKAGE=#{homebrew_package} " \
       "RBX_BINARY_CONFIG=#{config}"
  end
end
