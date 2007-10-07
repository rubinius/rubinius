require 'readline'

def git_branch
  `git branch | grep "*"`.strip[2..-1]
end



def compare_git_ver
  m = /git version (\d+).(\d+).(\d+)/.match(`git version`.strip)
  return true  if m[1].to_i > 1
  return false if m[1].to_i < 1
  return true  if m[2].to_i > 5
  return false if m[2].to_i < 5
  return true  if m[3].to_i >= 3
  return false
end

def check_git_ver
  raise "Invalid git version, use at least 1.5.3" unless compare_git_ver
end

namespace :git do
  desc "Show the current status of the checkout"
  task :status do
    system "git status"
  end

  desc "Create a new topic branch"
  task :topic do
    total = `git branch`.scan("quick").size
    if total == 0
      default = "quick"
    else
      default = "quick#{total + 1}"
    end
    name = Readline.readline "Topic name (default #{default}): "
    if name.strip.empty?
      name = default
    end
    sh "git checkout -b #{name}"
  end

  desc "Push all changes to the rubinius repository"
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

  desc "Pull new commits from the rubinius repository"
  task :update do
    check_git_ver
    `git diff-files --quiet`
    if $?.exitstatus == 1
      stash = true
      clear = `git stash list`.scan("\n").size == 0
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
      `git stash clear` if clear
    end
  end
end
