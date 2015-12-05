require 'aws-sdk'

desc "Deploy a Rubinius release"
task :deploy do
  s3 = Aws::S3::Resource.new(region: "us-west-2")

  # Build and upload the release tarball to S3.
  if ENV["TRAVIS_OS_NAME"] == "linux" and ENV["CC"] == "clang" and ENV["RVM"] == "rbx-2"
    # TODO: extract the name to a method
    tarball = "rubinius-#{rbx_version}.tar.bz2"

    puts "Deploying release tarball #{tarball}..."

    Rake::Task['release'].invoke

    obj = s3.bucket("rubinius-releases-rubinius-com").object(tarball)
    obj.upload_file(tarball)
  end
end
