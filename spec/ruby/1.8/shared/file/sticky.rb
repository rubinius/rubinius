shared :file_sticky do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
  end
end
