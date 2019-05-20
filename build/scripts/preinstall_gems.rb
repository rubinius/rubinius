require 'rubygems'
require 'rubygems/installer'

puts "Pre-installing gems for #{RUBY_VERSION}..."

BUILD_CONFIG = Rubinius::BUILD_CONFIG
gems = BUILD_CONFIG[:installed_gems]

install_dir = "#{BUILD_CONFIG[:builddir]}#{BUILD_CONFIG[:gemsdir]}"
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
  puts "Attempting to install gem #{gem} to #{install_dir}..." if $verbose
  return if File.directory? "#{install_dir}/gems/#{gem[0..-5]}"

  file = File.join(BUILD_CONFIG[:gems_cache], "#{gem}")

  return unless File.exist? file

  installer = Gem::Installer.new file, options
  spec = installer.install
  puts "Installed #{spec.name} (#{spec.version})"
end

gems.each do |gem|
  next if gem =~ /readline/

  install gem, install_dir, options

  if gem =~ /bundler/
    worker = "#{install_dir}/gems/#{gem[0..-5]}/lib/bundler/worker.rb"

    lines = File.readlines worker

    a = "      @size = size\n"
    b = "      @size = 1 # Bundler is not thread-safe\n"

    if lines[28] == a
      lines[28] = b
    elsif lines[28] != b
      raise "Unable to patch Bundler to be thread-safe on Rubinius"
    end

    File.write worker, lines.join
  end
end

if RUBY_PLATFORM =~ /freebsd/
  install(gems.find { |g| g =~ /rubysl-readline/ }, install_dir, options)
else
  install(gems.find { |g| g =~ /rb-readline/ }, install_dir, options)
end
