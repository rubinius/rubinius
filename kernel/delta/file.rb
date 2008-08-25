class File
  private_class_method :dirsep?, :next_path, :range, :name_match

  # these will be necessary when we run on Windows
  DOSISH = !!(RUBY_PLATFORM =~ /mswin/)
  CASEFOLD_FILESYSTEM = DOSISH
  FNM_SYSCASE = CASEFOLD_FILESYSTEM ? FNM_CASEFOLD : false
end
