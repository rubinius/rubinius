namespace :package do
  desc "Package up the LLVM build into a tar.gz"
  task :llvm do
    if host_triple == "i686-pc-linux-gnu" || host_triple == "x86_64-unknown-linux-gnu"
      prebuilt_archive = "llvm-#{llvm_version}-#{host_triple}-#{gcc_major_version}.tar.bz2"
    else
      prebuilt_archive = "llvm-#{llvm_version}-#{host_triple}.tar.bz2"
    end

    sh "tar -c -C vendor/llvm --exclude .svn --exclude \"*.dylib\" --exclude \"*.so\" -f - Release/lib Release/bin/llvm-config include | bzip2 -9 > #{prebuilt_archive}"

    require 'digest/md5'
    md5 = Digest::MD5.new

    File.open(prebuilt_archive) do |f|
      until f.eof?
        md5 << f.read(1024)
      end
    end

    File.open("#{prebuilt_archive}.md5", "w") do |f|
      f.puts md5.hexdigest
    end

    puts "Computed MD5 to #{prebuilt_archive}.md5"
  end

  task :binary_build do
    RBX_BINARY_PREFIX = ENV["RBX_BINARY_PREFIX"] || "/usr/local/rubinius/#{RBX_VERSION}"
    RBX_BINARY_BIN = ENV["RBX_BINARY_BIN"] || "/usr/local/bin/rbx"

    ENV["RELEASE"] = "1"
    sh "./configure --prefix=#{RBX_BINARY_PREFIX} --preserve-prefix"
    require File.expand_path("../../config.rb", __FILE__)

    RBX_BINARY_ROOT = BUILD_CONFIG[:stagingdir][0...-BUILD_CONFIG[:prefixdir].size]

    sh "rake -q clean; rake -q build"
    sh "mkdir -p #{RBX_BINARY_ROOT}#{File.dirname(RBX_BINARY_BIN)}"

    bin = "#{RBX_BINARY_PREFIX}#{BUILD_CONFIG[:bindir]}"
    bin_link = "#{RBX_BINARY_ROOT}#{RBX_BINARY_BIN}"
    sh "ln -sf #{bin} #{bin_link}"
  end

  task :check_osx do
    unless osx = ENV['OSX']
      raise "Please set OSX to the version of OS X this is for"
    end
  end

  desc "Build an OS X .pkg"
  task :osx => [:check_osx, :binary_build] do
    package_name = "rubinius-#{RBX_VERSION}-#{osx}.pkg"
    sh "rm -rf #{package_name}"

    sh "/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --root #{RBX_BINARY_ROOT} --id com.engineyard.rubinius -o rubinius-#{RBX_VERSION}.pkg -t Rubinius -v"

    sh "mv rubinius-#{RBX_VERSION}.pkg #{package_name}"
    sh "zip -r #{package_name}.zip #{package_name}"
    sh "rm -rf #{package_name}"
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
  end
end
