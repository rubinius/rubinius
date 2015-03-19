require "rakelib/release"

task :revision_file do
  if git_directory
    File.open(revision_file, "w") { |f| f.write release_revision.join(" ") }
  end
end

desc "Create a release tarball"
task :release => :revision_file do
  Dir.chdir BUILD_CONFIG[:gems_cache] do
    sh "rm -f *", :verbose => $verbose
  end
  sh "gem install bundler", :verbose => $verbose
  sh "bundle update", :verbose => $verbose
  Rake::Task['gems:update_list'].invoke
  Dir.chdir BUILD_CONFIG[:gems_cache] do
    sh "rm -f bundler*", :verbose => $verbose
    sh "gem fetch bundler", :verbose => $verbose
  end
  Rake::Task['package:tar'].invoke
end
