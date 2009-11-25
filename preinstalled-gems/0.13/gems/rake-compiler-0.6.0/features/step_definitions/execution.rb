Given %r{^I've already successfully executed rake task '(.*)'$} do |task_name|
  emptyness = `rake #{task_name} 2>&1`
  unless $?.success?
    warn emptyness
    raise "rake failed with #{$?.exitstatus}"
  end
end

When /^rake task '(.*)' is invoked$/ do |task_name|
  @output ||= {}
  @result ||= {}
  @output[task_name] = `rake #{task_name} 2>&1`
  @result[task_name] = $?.success?
end

Then /^rake task '(.*)' succeeded$/ do |task_name|
  if @result.nil? || !@result.include?(task_name) then
    raise "The task #{task_name} should be invoked first."
  else
    @result[task_name].should be_true
  end
end

Then /^output of rake task '(.*)' (contains|do not contain) \/(.*)\/$/ do |task_name, condition, regex|
  if condition == 'contains' then
    @output[task_name].should match(%r(#{regex}))
  else
    @output[task_name].should_not match(%r(#{regex}))
  end
end
