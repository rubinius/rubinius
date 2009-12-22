namespace :package do
  desc "Package up the LLVM build into a tar.gz"
  task :llvm do
    sh "tar -c -C vm/external_libs/llvm --exclude .svn --exclude \"*.dylib\" --exclude \"*.so\" -f - Release/lib Release/bin/llvm-config include | bzip2 -9 > llvm-#{host_triple}.tar.bz2"
  end

  desc "Build and package the result as an OS X .pkg"
  task :osx => "build" do
    Dir.mkdir "pkgtmp"

    ENV['FAKEROOT'] = "pkgtmp"

    Rake::Task['install'].invoke

    sh "/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --root pkgtmp --id com.engineyard.rubinius -o rubinius-#{Rubinius::BUILD_CONFIG[:version]}.pkg -t Rubinius -v"

    rm_rf "pkgtmp"

  end
end
