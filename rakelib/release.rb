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
  elsif File.file? release_revision
    IO.read release_revision
  else
    "build"
  end
end

def record_revision
  if git_directory
    File.open(release_revision, "wb") { |f| f.write read_revision }
  end
end

def release_revision
  File.expand_path "../../.revision", __FILE__
end

def rubinius_version
  "#{BUILD_CONFIG[:libversion]}.#{BUILD_CONFIG[:patch_version]}"
end

def write_release(path, version, date, revision)
  date ||= default_release_date

  File.open path, "wb" do |f|
    f.puts %[#define RBX_VERSION       "#{version}"]
    f.puts %[#define RBX_RELEASE_DATE  "#{date}"]
    f.puts %[#define RBX_BUILD_REV     "#{revision}"]
  end
end

def default_release_date
  Time.now.strftime "%F"
end

def validate_revision
  read_revision == BUILD_CONFIG[:revision]
end
