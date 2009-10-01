namespace :package do
  desc "Package up the LLVM build into a tar.gz"
  task :llvm do
    sh "tar -c -C vm/external_libs/llvm --exclude .svn --exclude \"*.dylib\" -f - Release/lib Release/bin/llvm-config include | bzip2 -9 > llvm-#{host_triple}.tar.bz2"
  end
end
