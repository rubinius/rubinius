require 'aws-sdk'
require 'pathname'

desc "Deploy a Rubinius release"
task :deploy do
  s3 = Aws::S3::Client.new(region: "us-west-2")
  file_exts = ["", ".md5", ".sha1", ".sha512"]

  # TODO: extract the name to a method
  basename = "rubinius-#{rbx_version}.tar.bz2"

  # Build and upload the release tarball to S3.
  if ENV["TRAVIS_OS_NAME"] == "linux" and ENV["CC"] == "clang" and ENV["RVM"] == "rbx-2"
    puts "Deploying release tarball #{basename}..."

    Rake::Task['release'].invoke

    bucket = "rubinius-releases-rubinius-com"

    index = "index.txt"
    index_list = s3.get_object(bucket: bucket, key: index).body.read

    file_exts.each do |ext|
      name = basename + ext
      s3.put_object bucket: bucket, key: name, body: File.read(name)

      index_list << name << "\n" unless index_list =~ /#{name}/
    end

    s3.put_object bucket: bucket, key: index, body: index_list
  end

  # Build and upload a binary to S3.
  if ENV["RVM"] == "rbx-2"
    puts "Deploying Travis binary #{basename}..."

    Rake::Task['package:binary'].invoke

    url_prefix = "https://rubinius-binaries-rubinius-com.s3-us-west-2.amazonaws.com/"
    bucket = "rubinius-binaries-rubinius-com"

    if ENV["TRAVIS_OS_NAME"] == "linux"
      path_prefix = "ubuntu/12.04/x86_64/"
    else
      path_prefix = "osx/10.9/x86_64/"
    end

    index_list = s3.get_object(bucket: bucket, key: index).body.read

    file_exts.each do |ext|
      name = basename + ext
      key = prefix + name
      s3.put_object bucket: bucket, key: key, body: File.read(name)

      path = url_prefix + key
      index_list << path << "\n" unless index_list =~ /#{path}/
    end

    s3.put_object bucket: bucket, key: index, body: index_list
  end
end
