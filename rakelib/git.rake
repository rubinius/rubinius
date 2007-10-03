require 'readline'

def git_branch
  `git branch | grep "*"`.strip[2..-1]
end

namespace :git do
  desc "Show the current status of the checkout"
  task :status do
    system "git status"
  end

  task :topic do
    name = Readline.readline "Topic name: "
    sh "git checkout -b #{name}"
  end

  task :push => :update do
    branch = git_branch()
    if branch != "master"
      `git diff-files --quiet`
      if $?.exitstatus == 1
        puts "You have outstanding changes. Please commit them first."
        exit 1
      end

      puts "* Merging topic '#{branch}' back into master..."
      `git checkout master`
      sh "git merge #{branch}"
      switch = true
    else
      switch = false
    end

    puts "* Pushing changes..."
    sh "git push"

    if switch
      puts "* Switching back to #{branch}..."
      `git checkout #{branch}`
    end
  end

  task :update do
    `git diff-files --quiet`
    if $?.exitstatus == 1
      stash = true
      puts "* Saving changes..."
      `git stash save`
    else
      stash = false
    end

    branch = git_branch()
    if branch != "master"
      switch = true
      `git checkout master`
      puts "* Switching back to master..."
    else
      switch = false
    end

    puts "* Pulling in new commits..."
    sh "git pull"
    
    if switch
      puts "* Porting changes into #{branch}..."
      `git checkout #{branch}`
      sh "git rebase master"
    end
    
    if stash
      puts "* Applying changes..."
      sh "git stash apply"
    end
  end
end
