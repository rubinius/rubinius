desc "Create a release tarball"
task :release do
  Dir.chdir BUILD_CONFIG[:gems_cache] do
    sh "rm -f *", :verbose => $verbose
    sh "gem fetch bundler", :verbose => $verbose
  end
  sh "bundle update", :verbose => $verbose
  Rake::Task['gems:update_list'].invoke
  Rake::Task['package:tar'].invoke
end
