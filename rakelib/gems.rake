namespace :gems do
  desc 'Install the pre-installed gems'
  task :install do
    ENV['GEM_HOME'] = ENV['GEM_PATH'] = nil

    if BUILD_CONFIG[:stagingdir]
      rbx = "#{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"
    else
      rbx = "#{BUILD_CONFIG[:sourcedir]}/bin/#{BUILD_CONFIG[:program_name]}"
    end

    BUILD_CONFIG[:version_list].each do |ver|
      sh "#{rbx} -X#{ver} #{BUILD_CONFIG[:sourcedir]}/rakelib/preinstall_gems.rb"
    end
  end
end
