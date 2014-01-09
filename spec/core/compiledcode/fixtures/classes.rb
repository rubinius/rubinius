module CompiledCodeSpecs
  # -----------------------------------------------------------------------
  # Keep the methods between these delimiters in the same place to preserve
  # the line numbers for the #locate_line specs.
  def line_test
    1 + 1
    2 + 2
  end

  def block_line_test
    1 + 1
    1.times do
      2 + 2
    end
  end
  # -----------------------------------------------------------------------

  # TODO: Make a mirror for this functionality
  def self.executable(name)
    method_table.lookup(name).method
  end

  def call_site_test
    "".length
    [].size
  end

  def method_om(a=1,b)
  end

  def method_mop(a, b=1, c)
  end
end
