class File
  private_class_method :dirsep?, :next_path, :range, :name_match

  # these will be necessary when we run on Windows
  const_set :DOSISH, RUBY_PLATFORM.match("mswin")
  const_set :CASEFOLD_FILESYSTEM, DOSISH
  const_set :FNM_SYSCASE, CASEFOLD_FILESYSTEM ? FNM_CASEFOLD : false
end
