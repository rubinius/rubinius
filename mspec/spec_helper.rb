class CaptureOutput < String
  def write(str)
    self << str.to_s
  end
  
  def print(str)
    write(str.to_s + $\.to_s)
  end
end
