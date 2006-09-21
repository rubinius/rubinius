
class MethodContext
  def line
    self.lines.each do |t|
      start = t.at(0)
      nd = t.at(1)
      op = t.at(2)
    end
  end
end
