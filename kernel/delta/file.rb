class File
  private_class_method :dirsep?, :next_path, :range, :name_match

  # these will be necessary when we run on Windows
  DOSISH = !!(RUBY_PLATFORM =~ /mswin/)
  CASEFOLD_FILESYSTEM = DOSISH
  FNM_SYSCASE = CASEFOLD_FILESYSTEM ? FNM_CASEFOLD : 0

  ##
  # Return the equivalent S-Expression of the file given.
  # Raises +SyntaxError+ if there is a syntax issue in the
  # file, making it unparsable.
  #  File.to_sexp("/tmp/test.rb") #=> s(...)
  def self.to_sexp(name)
    File.read(name).to_sexp(name)
  end
end
