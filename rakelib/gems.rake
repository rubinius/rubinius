namespace :gems do
  desc 'Install the pre-installed gems'
  task :install do
    STDOUT.puts "Installing pre-installed gems..."
    ENV['GEM_HOME'] = ENV['GEM_PATH'] = nil

    if BUILD_CONFIG[:stagingdir]
      rbx = "#{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"
    else
      rbx = "#{BUILD_CONFIG[:sourcedir]}/bin/#{BUILD_CONFIG[:program_name]}"
    end
    gems = Dir["preinstalled-gems/*.gem"]
    options = "--local --conservative --ignore-dependencies --no-rdoc --no-ri"

    BUILD_CONFIG[:version_list].each do |ver|
      gems.each do |gem|
        parts = File.basename(gem, ".gem").split "-"
        gem_name = parts[0..-2].join "-"
        gem_version = parts[-1]

        system "#{rbx} -X#{ver} -S gem query --name-matches #{gem_name} --installed --version #{gem_version} > #{DEV_NULL}"

        unless $?.success?
          sh "#{rbx} -X#{ver} -S gem install #{options} #{gem}"
        end
      end
    end
  end
end
