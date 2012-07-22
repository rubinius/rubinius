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

  desc "Build and package the result as an OS X .pkg"
  task :osx do
    unless osx = ENV['OSX']
      raise "Please set OSX to the version of OS X this is for"
    end

    prefix = "/usr/local/rubinius/#{RBX_VERSION}"
    ENV["RELEASE"] = "1"
    sh "./configure --prefix=#{prefix} --preserve-prefix"

    root = BUILD_CONFIG[:stagingdir][0...-BUILD_CONFIG[:prefixdir].size]

    sh "rake -q clean; rake -q build"
    sh "mkdir -p #{root}/usr/local/bin"
    sh "ln -sf #{prefix}/bin/rbx #{root}/usr/local/bin/rbx"

    package_name = "rubinius-#{RBX_VERSION}-#{osx}.pkg"
    sh "rm -rf #{package_name}"

    sh "/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --root #{root} --id com.engineyard.rubinius -o rubinius-#{RBX_VERSION}.pkg -t Rubinius -v"

    sh "mv rubinius-#{RBX_VERSION}.pkg #{package_name}"
    sh "zip -r #{package_name}.zip #{package_name}"
    sh "rm -rf #{package_name}"
  end
end
