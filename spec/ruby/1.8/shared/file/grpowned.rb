shared :file_grpowned do |cmd, klass, name|
  describe "#{name || klass}.#{cmd}" do
  end
end
