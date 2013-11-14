desc "Create a release tarball"
task :release do
  Dir.chdir "vendor/cache" do
    sh "rm -f *", :verbose => $verbose
    sh "gem fetch bundler", :verbose => $verbose
  end
  sh "bundle update", :verbose => $verbose
  sh "./configure --release-config", :verbose => $verbose
  Rake::Task['package:tar'].invoke
end
