require 'rakelib/package'

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

  desc "Create a tarball from the source"
  task :tar do
    stamp = Time.now.strftime("%Y%m%d")
    archive = "rubinius-#{RBX_VERSION}-#{stamp}.tar.gz"
    sh "git archive --format=tar --prefix=rubinius-#{RBX_VERSION}/ HEAD | gzip > #{archive}"

    write_md5_digest_file archive
    write_sha1_digest_file archive
  end

  task :binary_build do
    RBX_BINARY_PREFIX = ENV["RBX_BINARY_PREFIX"] || "/usr/local/rubinius/#{RBX_VERSION}"
    RBX_BINARY_BIN = ENV["RBX_BINARY_BIN"]

    ENV["RELEASE"] = "1"
    sh "./configure --prefix=#{RBX_BINARY_PREFIX} --preserve-prefix"
    load_configuration

    RBX_BINARY_ROOT = BUILD_CONFIG[:stagingdir][0...-BUILD_CONFIG[:prefixdir].size]

    sh "rake -q clean; rake -q build"

    if RBX_BINARY_BIN
      sh "mkdir -p #{RBX_BINARY_ROOT}#{File.dirname(RBX_BINARY_BIN)}"

      bin = "#{RBX_BINARY_PREFIX}#{BUILD_CONFIG[:bindir]}"
      bin_link = "#{RBX_BINARY_ROOT}#{RBX_BINARY_BIN}"
      sh "ln -sf #{bin} #{bin_link}"
    end
  end

  task :setup_osx do
    unless ENV['OSX']
      raise "Please set OSX to the version of OS X this is for"
    end

    ENV["RBX_BINARY_PREFIX"] = "/usr/local/rubinius/#{RBX_VERSION}"
    ENV["RBX_BINARY_BIN"] = "/usr/local/bin/rbx"
  end

  desc "Build an OS X .pkg"
  task :osx => [:setup_osx, :binary_build] do
    package_name = "rubinius-#{RBX_VERSION}-#{ENV["OSX"]}.pkg"
    sh "rm -rf #{package_name}"

    sh "/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --root #{RBX_BINARY_ROOT} --id com.engineyard.rubinius -o rubinius-#{RBX_VERSION}.pkg -t Rubinius -v"

    zip_archive = "#{package_name}.zip"
    sh "mv rubinius-#{RBX_VERSION}.pkg #{package_name}"
    sh "zip -r #{zip_archive} #{package_name}"
    sh "rm -rf #{package_name}"

    write_md5_digest_file zip_archive
    write_sha1_digest_file zip_archive
  end

  desc "Build a general Unix/Linux binary package"
  task :binary => :binary_build do
    time_stamp = Time.now.strftime("%Y%m%d")
    package_name = "rubinius-#{RBX_VERSION}-#{time_stamp}-#{BUILD_CONFIG[:host]}.zip"
    sh "rm -rf #{package_name}"

    Dir.chdir RBX_BINARY_ROOT do
      sh "zip --symlinks -r #{package_name} *"
    end

    sh "mv #{RBX_BINARY_ROOT}/#{package_name} #{BUILD_CONFIG[:sourcedir]}"

    write_md5_digest_file package_name
    write_sha1_digest_file package_name
  end
end
