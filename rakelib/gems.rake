namespace :gems do
  desc 'Install the pre-installed gems'
  task :install do
    ENV['GEM_HOME'] = ENV['GEM_PATH'] = nil

    Dir.chdir "preinstalled-gems" do
      sh "#{BUILD_CONFIG[:build_exe]} #{BUILD_CONFIG[:sourcedir]}/rakelib/preinstall_gems.rb"
    end
  end
end
