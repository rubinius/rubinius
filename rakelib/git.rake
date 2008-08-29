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

  desc "Switch to the commiter URL"
  task :committer do
    sh "git config remote.origin.url git@github.com:evanphx/rubinius.git"
    puts "\nYou're now accessing rubinius via the committer URL." 
  end

  desc "Switch to the anonymous URL"
  task :anon do
    sh "git config remote.origin.url git://github.com/evanphx/rubinius.git"
    puts "\nYou're now accessing rubinius via the anonymous URL."
  end

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
    sh "git branch --track #{name}"
    sh "git checkout #{name}"
  end

  desc "Push all changes to the rubinius repository"
  task :push => :update do
    branch = git_branch()

    rem = `git config branch.#{branch}.remote`.strip
    if $?.exitstatus == 1
      puts "This branch does not have tracking setup, so I'm unable to automatically pull into it. Please use 'rake git:topic' or 'git branch --track' to create your topic branches."
      exit 1
    end

    puts "* Pushing changes..."
    sh "git push origin cpp"
  end

  desc "Pull new commits from the rubinius repository"
  task :pull do
    check_git_ver
    branch = git_branch()

    # Test if this is a "root" branch or a topic branch.

    rem = `git config branch.#{branch}.remote`.strip
    if $?.exitstatus == 1
      puts "This branch does not have tracking setup, so I'm unable to automatically pull into it. Please use 'rake git:topic' or 'git branch --track' to create your topic branches."
      exit 1
    end

    `git diff-files --quiet`
    if $?.exitstatus == 1
      stash = true
      clear = `git stash list`.scan("\n").size == 0
      puts "* Saving changes..."
      `git stash save`
    else
      stash = false
    end

    puts "* Pulling in new commits..."
    sh "git pull --rebase"

    if stash
      puts "* Applying changes..."
      sh "git stash apply"
      `git stash clear` if clear
    end
  end

  task :update => :pull

  desc "Make patches upto current"
  task :make_patches do
    sh "git format-patch HEAD^"
  end

  desc "Pull, build, spec, push"
  task :safe_push => %w[git:pull build vm:test git:push]
end
