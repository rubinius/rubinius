desc 'Ensure all the cross compiled versions are installed'
task :bootstrap do
  fail "Sorry, this only works on OSX and Linux" if RUBY_PLATFORM =~ /mswin|mingw/

  versions = %w(1.8.6-p398 1.9.1-p243 1.9.2-p0)

  versions.each do |version|
    puts "[INFO] Attempt to cross-compile Ruby #{version}"
    ruby "-Ilib bin/rake-compiler cross-ruby VERSION=#{version}"
  end
end
