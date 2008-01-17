require File.dirname(__FILE__) + '/capture'

def spec_runner
  unless $runner_mock_runner
    obj = Object.new
    def obj.method_missing(*) self end
    $runner_mock_runner = obj
  end
  $runner_mock_runner
end
