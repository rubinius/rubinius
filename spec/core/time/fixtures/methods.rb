def with_timezone(name, offset, daylight_saving_zone = "")
  # TZ convention is backwards
  offset = -offset

  zone = name.dup
  zone << offset.to_s
  zone << ":00:00"
  zone << daylight_saving_zone

  old = ENV["TZ"]
  ENV["TZ"] = zone

  begin
    yield
  ensure
    ENV["TZ"] = old
  end
end

def localtime(seconds)
  if RUBY_PLATFORM =~ /darwin/
    `date -r #{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
  elsif RUBY_PLATFORM =~ /linux/
    `date -d @#{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
  else
    `date -j -f "%s" #{seconds} "+%a %b %d %H:%M:%S %z %Y"`.chomp
  end
end
