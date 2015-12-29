require 'rubygems'
require 'rubygems/installer'
require 'rubinius/build_config'

puts "Pre-installing gems for #{RUBY_VERSION}..."

BUILD_CONFIG = Rubinius::BUILD_CONFIG
gems = BUILD_CONFIG[:gem_files]
install_dir = "#{BUILD_CONFIG[:build_prefix]}#{BUILD_CONFIG[:gemsdir]}"
unless BUILD_CONFIG[:darwin] and
         ENV["TRAVIS_OS_NAME"] != "osx" and
         `which brew`.chomp.size > 0 and
         $?.success? and
         (openssl = `brew --prefix #{ENV["RBX_OPENSSL"] || "openssl"}`.chomp).size > 0
  openssl = false
end

options = {
  :bin_dir              => nil,
  :build_args           => [],
  :env_shebang          => true,
  :force                => false,
  :format_executable    => false,
  :ignore_dependencies  => true,
  :security_policy      => nil,
  :user_install         => nil,
  :wrappers             => true,
  :install_as_default   => false,
  :install_dir          => install_dir
}

def install(gem, install_dir, options)
  return if File.directory? "#{install_dir}/gems/#{gem[0..-5]}"

  file = File.join(BUILD_CONFIG[:gems_cache], "#{gem}")

  installer = Gem::Installer.new file, options
  spec = installer.install
  puts "Installed #{spec.name} (#{spec.version})"
end

gems.each do |gem|
  next if gem =~ /readline/

  if gem =~ /openssl/ and openssl
    options[:build_args] =
      ["--with-cppflags=-I#{openssl}/include --with-ldflags=-L#{openssl}/lib"]
  else
    options[:build_args] = []
  end

  install gem, install_dir, options
end

if RUBY_PLATFORM =~ /freebsd/
  install(gems.find { |g| g =~ /rubysl-readline/ }, install_dir, options)
else
  install(gems.find { |g| g =~ /rb-readline/ }, install_dir, options)
end
