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

def git_update
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
  sh "git fetch"
  sh "git rebase origin"

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

def git_push
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
