# for building and installing rubinius-specific gems.

namespace :gems do

  checkout_dir = File.dirname File.dirname(File.expand_path(__FILE__))
  relative_path = File.join 'gems', 'rubinius', LIBVER

  GEMS_DEVELOPMENT_PRE_INSTALLED_DIR = File.join checkout_dir, 'tmp',
                                                 relative_path
  GEMS_PRE_INSTALLED_DIR = File.join CODEPATH, 'rubinius', relative_path 

  rule '.gem' => '.gemspec' do |task|
    Dir.chdir File.dirname(task.source) do
      rbx 'gem', 'build', File.basename(task.source)
    end
  end

  gemspecs = Dir['gems/*/*.gemspec']
  gems = []
  installed_gemspecs = []

  # rake complains about circular dependencies with .gemspec => .gem rule
  gemspecs.each do |gemspec|
    gem = gemspec.sub(/spec$/, '')
    gems << gem

    gem_name = gemspec.split(File::SEPARATOR).last

    installed_gemspec = File.join GEMS_DEVELOPMENT_PRE_INSTALLED_DIR,
                                  'specifications', gem_name

    installed_gemspecs << installed_gemspec

    file installed_gemspec => gem do
      Dir.chdir File.dirname(gem) do
        rbx 'gem', 'install', File.basename(gem), '-i',
            GEMS_DEVELOPMENT_PRE_INSTALLED_DIR
      end
    end
  end

  task :build => %w[extensions] + gems

  task :clean do
    gems.each { |gem| rm_f gem, :verbose => $verbose }
    rm_rf GEMS_DEVELOPMENT_PRE_INSTALLED_DIR, :verbose => $verbose
  end

  task :install_development => [:build] + installed_gemspecs

  task :install => :build do
    Dir['gems/*/*.gem'].each do |gem|
      Dir.chdir File.dirname(gem) do
        rbx 'gem', 'install', File.basename(gem), '-i',
            GEMS_PRE_INSTALLED_DIR
      end
    end
  end

end

