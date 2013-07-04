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

def write_version(path, version, versions)
  File.open path, "wb" do |f|
    versions.each do |v|
      f.puts "#define LANGUAGE_#{v}_ENABLED  (#{version == v})"
    end
  end
end

def write_release(path, version, date, revision)
  File.open path, "wb" do |f|
    f.puts %[#define RBX_VERSION       "#{version}"]
    f.puts %[#define RBX_RELEASE_DATE  "#{date}"]
    f.puts %[#define RBX_BUILD_REV     "#{revision}"]
  end
end

def config_rubinius_version
  version = rubinius_version
  version += ".#{BUILD_CONFIG[:release]}" if BUILD_CONFIG[:release]
  version
end

def config_release_date
  BUILD_CONFIG[:release_date]
end

def default_rubinius_version
  rubinius_version + ".#{validate_release_label}"
end

def default_release_date
  validate_release_date
end

def validate_revision
  read_revision == BUILD_CONFIG[:revision]
end

def validate_release_label(label="nightly")
  date = Time.now
  case label
  when "nightly"
    "n#{date.strftime("%j").to_i}"
  when "weekly"
    "w#{date.strftime("%V").to_i}"
  when "monthly"
    "m#{date.month}"
  when /^rc\d+/
    label
  when "final"
    nil
  else
    raise "unknown release label #{label}"
  end
end

def validate_release_date(date=Time.now.to_s)
  Date.parse(date).strftime
end
