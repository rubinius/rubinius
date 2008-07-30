require 'readline'

namespace :git do

  task :fixup do
    `git config submodule.mspec.url`
    if $?.exitstatus == 0
      `git config --remove-section submodule.mspec`
    end

    `git config submodule.spec/frozen.url`
    if $?.exitstatus == 0
      `git config --remove-section submodule.spec/frozen`
    end
  end

  desc "Executed post update. Lets the new Rakefile perform any fixups"
  task :post_update do
    last_commit = ENV['LAST']

    Rake::Task['git:fixup'].invoke
  end

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
    puts "top-level:"
    puts
    system "git status"
    puts
    puts "spec/ruby:"
    puts
    system "(cd spec/ruby; git status)"
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
  task :update => :fixup do
    cur = `git rev-parse HEAD`.strip
    git_update
    sh "rake git:post_update LAST=#{cur}"
  end

  task :pull => :update

  desc "Make patches upto current"
  task :make_patches do
    sh "git format-patch HEAD^"
  end

  desc "Pull, build, spec, push"
  task :safe_push => %w[git:pull build spec git:push]

  desc "Pull, build, spec:full, push (very slow)"
  task :really_safe_push => %w[git:pull build spec:full git:push]
end
