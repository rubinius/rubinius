# Naive way of looking into platforms, please include others like FreeBSD?
Given %r{^I'm running a POSIX operating system$} do
  unless RUBY_PLATFORM =~ /linux|darwin/ then
    raise Cucumber::Pending.new("You need a POSIX operating system, no cheating ;-)")
  end
end

Given %r{^I've installed cross compile toolchain$} do
  compilers = %w(i586-mingw32msvc-gcc i386-mingw32-gcc)
  paths = ENV['PATH'].split(File::PATH_SEPARATOR)
  compiler = compilers.find do |comp|
    paths.find do |path|
      File.exist? File.join(path, comp)
    end
  end
  pending "Cannot locate suitable compiler in the PATH." unless compiler
end

Then /^binaries for platform '(.*)' get generated$/ do |platform|
  ext = binary_extension(platform)

  ext_for_platform = Dir.glob("tmp/#{platform}/**/*.#{ext}")
  ext_for_platform.should_not be_empty
end

Then /^binaries for platform '(.*)' version '(.*)' get copied$/ do |platform, version|
  lib_path = "lib/#{version}"
  ext = binary_extension(platform)

  ext_for_platform = Dir.glob("#{lib_path}/*.#{ext}")
  ext_for_platform.should_not be_empty
end
