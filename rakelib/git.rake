namespace :git do

  desc "Switch to the commiter URL"
  task :committer do
    sh "git config remote.origin.url git@github.com:rubinius/rubinius.git"
    puts "\nYou're now accessing rubinius via the committer URL." 
  end

  desc "Switch to the anonymous URL"
  task :anon do
    sh "git config remote.origin.url git://github.com/rubinius/rubinius.git"
    puts "\nYou're now accessing rubinius via the anonymous URL."
  end

end
