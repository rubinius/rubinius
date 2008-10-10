class Struct
  # HACK something seems to be corrupt in block calling
  Struct.new 'Tms', :utime, :stime, :cutime, :cstime
end
