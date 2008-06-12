
MINIMUM_VERSION = [1, 5, 3]

def is_git_dir(dir)
  File.directory?(dir) and File.directory?(File.join(dir, ".git"))
end

def git_branch
  `git branch | grep "*"`.strip[2..-1]
end

def compare_git_ver
  v = `git version`.scan(/version (\d+).(\d+).(\d+)/).flatten.map { |s| s.to_i }
  m = [1, 5, 3]

  (v <=> MINIMUM_VERSION) >= 0
end

def check_git_ver
  raise "Invalid git version, use at least #{MINIMUM_VERSION.join(".")}" unless
    compare_git_ver
end

def on_master
  branch = git_branch()
  switch = if branch != "master" then
             `git checkout master`
             puts "* Switching back to master..."
             true
           end

  yield

  if switch
    puts "* Porting changes into #{branch}..."
    `git checkout #{branch}`
    sh "git rebase master"
  end
end

def with_git_changes
  `git diff-files --quiet`
  if $?.exitstatus == 1 then
    yield
    true
  end
end

def git_update
  check_git_ver

  clear = false
  stash = with_git_changes do
    clear = `git stash list`.scan("\n").size == 0
    puts "* Saving changes..."
    `git stash save`
    true
  end

  on_master do
    puts "* Pulling in new commits..."
    sh "git fetch"
    sh "git rebase origin"
  end

  if stash then
    puts "* Applying changes..."
    sh "git stash apply"
    `git stash clear` if clear
  end
end

def git_push
  branch = git_branch

  with_git_changes do
    abort "You have outstanding changes. Please commit them first."
  end if branch != "master"

  on_master do
    puts "* Merging topic '#{branch}' back into master..."
    sh "git merge #{branch}"
    puts "* Pushing changes..."
    sh "git push"
  end
end
