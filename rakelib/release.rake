RBX_VERSION = BUILD_CONFIG[:version]

namespace :release do
  desc "Create the tarball for the release"
  task :tar do
    stamp = Time.now.strftime("%Y%m%d")
    @rbx_tar = "rubinius-#{RBX_VERSION}-#{stamp}.tar.gz"
    sh "git archive --format=tar --prefix=rubinius-#{RBX_VERSION}/ HEAD | gzip > #{@rbx_tar}"
  end

  desc "Tag HEAD with the current release"
  task :tag do
    sh "git tag release-#{RBX_VERSION} HEAD"
  end

  desc "Create a release"
  task :all do
    # A simple sanity check, so that RBX_VERSION is updated first.
    unless RBX_VERSION == ENV['VERSION']
      p [RBX_VERSION, ENV['VERSION']]
      raise "Please set VERSION to the same as RBX_VERSION to be released"
    end

    STDOUT.sync = true
    print "Did you update @config_version in configure? [y/n]"
    unless STDIN.gets.strip == "y"
      puts "Well go change it then!"
      exit
    end

    puts "* Making tar.gz..."
    Rake::Task['release:tar'].invoke

    puts "* Testing tar.gz..."
    sh "rm -rf release-test; mkdir release-test; cd release-test; tar zxf ../#{@rbx_tar}; cd rubinius-#{RBX_VERSION}; ./configure && rake -q build"
    # Do this seperate, so that the testing can fail and we leave the directory
    # where it is.
    if osx = ENV['OSX']
      puts "* Cleaning and building OS X package..."
      sh "cd release-test/rubinius-#{RBX_VERSION}; rake -q clean; RELEASE=1 ./configure --prefix=/usr/local/rubinius/#{RBX_VERSION}; rake -q package:osx && mv rubinius-#{RBX_VERSION}.pkg ../.."
      pref = "rubinius-#{RBX_VERSION}-#{osx}"
      sh "mv rubinius-#{RBX_VERSION}.pkg #{pref}.pkg"
      sh "zip -r #{pref}.pkg.zip #{pref}.pkg"
    end
    sh "rm -rf release-test"

    # Comes last, means everything went fine.
    puts "* Tagging..."
    Rake::Task['release:tag'].invoke

    puts "* DONE!"
  end

  desc "Create an OS X package from a release tarball"
  task :osx do
    unless file = ENV['FILE']
      raise "Please set FILE to the tar.gz for the release"
    end

    unless osx = ENV['OSX']
      raise "Please set OSX to the version of OS X this is for"
    end

    unless ver = ENV['VERSION']
      ver = RBX_VERSION
    end

    puts "* Building OS X package..."
    sh "rm -rf osx-build; mkdir osx-build"

    Dir.chdir "osx-build" do
      sh "tar zxf ../#{file}"

      unless File.directory?("rubinius-#{ver}")
        raise "tar file didn't contain the proper release directory"
      end

      sh "cd rubinius-#{ver}; RELEASE=1 ./configure --prefix=/usr/local/rubinius/#{ver}; rake -q package:osx && mv rubinius-#{ver}.pkg ../.."
    end

    pref = "rubinius-#{ver}-#{osx}"
    sh "mv rubinius-#{ver}.pkg #{pref}.pkg"
    sh "zip -r #{pref}.pkg.zip #{pref}.pkg"
  end
end
