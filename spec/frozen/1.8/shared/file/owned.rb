shared :file_owned do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
  end
end
