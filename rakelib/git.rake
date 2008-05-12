require 'readline'

namespace :git do

  desc "Switch to the commiter URL"
  task :committer do
    sh "git config remote.origin.url git@git.rubini.us:code"
    puts "\nYou're now accessing rubinius via the committer URL." 
  end

  desc "Switch to the anonymous URL"
  task :anon do
    sh "git config remote.origin.url git://git.rubini.us/code"
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
    sh "git checkout -b #{name}"
  end

  desc "Push all changes to the rubinius repository"
  task :push => :update do
    git_push
  end

  desc "Pull new commits from the rubinius repository"
  task :update => 'spec:update' do
    git_update
  end

  task :pull => :update

  desc "Make patches upto current"
  task :make_patches do
    sh "git format-patch HEAD^"
  end

  desc "Pull, build, spec, push"
  task :safe_push => %w[git:pull build spec git:push]
end
