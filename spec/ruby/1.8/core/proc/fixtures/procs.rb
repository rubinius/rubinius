module ProcSpecs
  def self.returner
    Proc.new { return }.call
  end
end
