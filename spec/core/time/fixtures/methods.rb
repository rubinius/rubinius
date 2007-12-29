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
<<<<<<< HEAD:spec/core/time/fixtures/methods.rb
  platform :os => [:darwin, :bsd] do
=======
  platform_is :darwin, :bsd do
>>>>>>> Updated guards in specs.:spec/core/time/fixtures/methods.rb
    return `date -r #{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
  end
  
<<<<<<< HEAD:spec/core/time/fixtures/methods.rb
  platform :os => :linux do
=======
  platform_is :linux do
>>>>>>> Updated guards in specs.:spec/core/time/fixtures/methods.rb
    return `date -d @#{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
  end
  
  return `date -j -f "%s" #{seconds} "+%a %b %d %H:%M:%S %z %Y"`.chomp
end
