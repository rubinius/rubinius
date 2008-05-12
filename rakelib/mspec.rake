# -*- ruby -*-

namespace :mspec do
  desc "Initialize git submodule for mspec"
  task :init do
    unless File.exist? "mspec/bin/mspec"
      puts "Initializing mspec submodule..."
      rm_rf "mspec"
      sh "git submodule init mspec"
      sh "git submodule update mspec"
    end
  end

  desc "Synchronize mspec submodule to current remote version"
  task :sync do
    Dir.chdir "mspec" do
      sh "git fetch"
      sh "git rebase origin"
    end
    version = `git log --pretty=oneline -1 mspec`[0..7]
    sh "git add mspec"
    sh "git commit -m 'Updated MSpec submodule to #{version}'"
  end

  desc "Update mspec sources to current submodule version"
  task :update => :init do
    sh "git submodule update mspec"
  end

  task :pull => :update
end
