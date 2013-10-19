def git_directory
  git_dir = File.expand_path "../../.git", __FILE__
  File.directory?(git_dir) && git_dir
end

def read_revision
  `git rev-parse HEAD`.chomp
end

def build_revision
  if git_directory
    read_revision
  else
    Rubinius::BUILD_CONFIG[:revision] || "build"
  end
end

def default_release_date
  Time.now.strftime "%F"
end

def write_release(path, version, date)
  date ||= default_release_date

  File.open path, "wb" do |f|
    f.puts %[#define RBX_RUBY_VERSION  "#{Rubinius::BUILD_CONFIG[:ruby_version]}"]
    f.puts %[#define RBX_VERSION       "#{version}"]
    f.puts %[#define RBX_RELEASE_DATE  "#{date}"]
    f.puts %[#define RBX_BUILD_REV     "#{build_revision}"]
  end
end
