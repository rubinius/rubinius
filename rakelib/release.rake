require "rakelib/release"

task :revision_file do
  if git_directory
    File.open(revision_file, "w") { |f| f.write release_revision.join(" ") }
  end
end

desc "Create a release tarball"
task :release => %W[revision_file package:tar]
