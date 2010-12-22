namespace :package do
  desc "Package up the LLVM build into a tar.gz"
  task :llvm do
    if host_triple == "i686-pc-linux-gnu" || host_triple == "x86_64-unknown-linux-gnu"
      prebuilt_archive = "llvm-#{llvm_version}-#{host_triple}-#{gcc_major_version}.tar.bz2"
    else
      prebuilt_archive = "llvm-#{llvm_version}-#{host_triple}.tar.bz2"
    end

    sh "tar -c -C vm/external_libs/llvm --exclude .svn --exclude \"*.dylib\" --exclude \"*.so\" -f - Release/lib Release/bin/llvm-config include | bzip2 -9 > #{prebuilt_archive}"

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
  task :osx => "build" do
    Dir.mkdir "pkgtmp"

    ENV['FAKEROOT'] = "pkgtmp"

    Rake::Task['install'].invoke

    sh "mkdir -p pkgtmp/usr/local/bin"
    sh "ln -sf /usr/local/rubinius/#{Rubinius::BUILD_CONFIG[:version]}/bin/rbx pkgtmp/usr/local/bin/rbx"

    sh "/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --root pkgtmp --id com.engineyard.rubinius -o rubinius-#{Rubinius::BUILD_CONFIG[:version]}.pkg -t Rubinius -v"

    rm_rf "pkgtmp"
  end
end
