
c = ::RBe::Cpp::Class["Handler"]
if c
  c.wrapper = true

  # hooks
  c.set_hook "MessageReceived"

  # Perform function (TODO)
  %w(Perform).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true }
  end

  # Locking functions
  %w(LockLooper
     LockLooperWithTimeout
     UnlockLooper).each do |name|
    c.functions(name).each{|f| f.set_option :custom, true }
  end

  # Message Filter functions (TODO)
  %w(SetFilterList FilterList
     AddFilter RemoveFilter).each do |name|
    c.functions(name).each{|f| f.set_option(:noimp, true)}
  end

  # X Watching functions (TOD)
  %w(StartWatching
     StartWatchingAll
     StopWatching
     StopWatchingAll).each do |name|
    c.functions(name).each{|f| f.set_option(:noimp, true)}
  end
end
